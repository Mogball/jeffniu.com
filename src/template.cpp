#include "cmp.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "template.hpp"
#include "string_view.hpp"
#include <utility>
#include <list>
#include <tuple>
#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

using namespace std;
using namespace boost::filesystem;

static string INCLUDE_MATCH = "{{include";
static string REPLACE_MATCH = "{{.";
static string IF_MATCH = "{{if";
static string ELSE_MATCH = "{{else}}";
static string ENDIF_MATCH = "{{endif}}";
static string END_MATCH = "}}";

static tuple<string, bool> readWholeFile(string res) {
    ifstream ifs(res);
    if (!ifs) {
        return forward_as_tuple("", true);
    }
    string buf;
    ifs.seekg(0, ios::end);
    buf.resize(ifs.tellg());
    ifs.seekg(0, ios::beg);
    ifs.read(&buf[0], buf.size());
    ifs.close();
    auto ret = make_tuple(move(buf), false);
    assert(0 == buf.size());
    return ret;
}

static pair<size_t, size_t> lineColNumber(const string &s, size_t k) {
    assert(k < s.size());
    size_t ret = 0;
    size_t last = 0;
    for (size_t i = 0; i <= k; ++i) {
        if ('\n' == s[i]) {
            last = i;
            ++ret;
        }
    }
    return make_pair(ret, k - last);
}

static Expected<bool> recursiveParseIncludes(
        list<string_view> &parts,
        list<string> &alloc,
        const string &tmpl,
        const path &root) {
    size_t start = 0;
    size_t size = tmpl.size();
    while (start < size) {
        bool quit = false;
        size_t loc = tmpl.find(INCLUDE_MATCH, start);
        if (string::npos == loc) {
            quit = true;
            loc = size;
        }
        string_view view(tmpl, start, loc);
        parts.push_back(move(view));
        if (quit) {
            break;
        }
        size_t end = tmpl.find(END_MATCH, loc);
        if (string::npos == end) {
            return {true, "Template error: unmatched braces {{"};
        }
        start = end + END_MATCH.size();
        loc += INCLUDE_MATCH.size() + 1;
        string included = tmpl.substr(loc, end - loc);
        DEBUG_PRINT(format("--> %1%\n") % included);
        auto data = readWholeFile((root / included).string());
        if (true == get<1>(data)) {
            return {true, "Template error: file not found: " + (root / included).string()};
        }
        alloc.push_back(move(get<0>(data)));
        assert(0 == get<0>(data).size());
        auto ret = recursiveParseIncludes(parts, alloc, alloc.back(), root);
        if (ret.hasError()) {
            return ret;
        }
    }
    return true;
}

static Expected<bool> iterativeParseReplaces(
        list<string_view> &parts,
        const map<string, string> &kv,
        const string &tmpl) {
    size_t start = 0;
    size_t size = tmpl.size();
    while (start < size) {
        bool quit = false;
        size_t loc = tmpl.find(REPLACE_MATCH, start);
        if (string::npos == loc) {
            quit = true;
            loc = size;
        }
        string_view view(tmpl, start, loc);
        parts.push_back(move(view));
        if (quit) {
            break;
        }
        size_t end = tmpl.find(END_MATCH, loc);
        if (string::npos == end) {
            return {true, "Template error: unmatched braces {{"};
        }
        start = end + END_MATCH.size();
        loc += REPLACE_MATCH.size();
        string name = tmpl.substr(loc, end - loc);
        auto it = kv.find(name);
        if (it == kv.end()) {
            return {true, "Template error: invalid key: " + name};
        }
        string_view part(it->second);
        parts.push_back(move(part));
    }
    return true;
}

static Expected<bool> iterativeParseControls(
        list<string_view> &parts,
        const string &tmpl) {
    size_t start = 0;
    size_t size = tmpl.size();
    while (start < size) {
        bool quit = false;
        size_t loc = tmpl.find(IF_MATCH, start);
        if (string::npos == loc) {
            quit = true;
            loc = size;
        }
        string_view view(tmpl, start, loc);
        parts.push_back(move(view));
        if (quit) {
            break;
        }
        size_t end = tmpl.find(END_MATCH, loc);
        if (string::npos == end) {
            return {true, "Template error: unmatched braces {{"};
        }
        size_t endifLoc = tmpl.find(ENDIF_MATCH, end);
        if (string::npos == endifLoc) {
            return {true, "Template error: missing endif"};
        }
        start = endifLoc + ENDIF_MATCH.size();
        loc += IF_MATCH.size() + 1;
        string_view a(tmpl, loc, tmpl.find(' ', loc));
        string_view op(tmpl, a.end + 1, a.end + 3);
        string_view b(tmpl, op.end + 1, end);
        // Lexicographic comparison of numbers works for non-negatives
        cmp_op op_code = static_cast<cmp_op>(op.data[op.start] + op.data[op.end - 1]);
        bool result = compare(a, b, op_code);
        end += END_MATCH.size();
        string_view block(tmpl);
        size_t elseLoc = tmpl.find(ELSE_MATCH, end);
        if (result) {
            block.start = end;
            block.end = min(elseLoc, endifLoc);
        } else if (elseLoc < endifLoc) {
            block.start = elseLoc + ELSE_MATCH.size();
            block.end = endifLoc;
        } else {
            continue;
        }
        parts.push_back(move(block));
    }
    return true;
}

Expected<string> parseIncludes(const string &tmpl, const path &root) {
    list<string_view> parts;
    list<string> alloc;
    auto ret = recursiveParseIncludes(parts, alloc, tmpl, root);
    if (ret.hasError()) {
        return {true, ret.getError()};
    }
    size_t total = 0;
    for (const string_view &view : parts) {
        total += view.size();
    }
    string data(total, 0);
    char *pCur = &data[0];
    for (const string_view &view : parts) {
        memcpy(pCur, view.front(), view.size());
        pCur += view.size();
    }
    return data;
}

Expected<string> parseReplaces(const string &tmpl, const map<string, string> &kv) {
    list<string_view> parts;
    auto ret = iterativeParseReplaces(parts, kv, tmpl);
    if (ret.hasError()) {
        return {true, ret.getError()};
    }
    size_t total = 0;
    size_t index = 0;
    for (const string_view &view : parts) {
        switch (index % 2) {
        case 0:
            total += view.size();
            break;
        default:
            total += view.data.size();
            break;
        }
        ++index;
    }
    string data(total, 0);
    char *pCur = &data[0];
    index = 0;
    for (const string_view &view : parts) {
        const char *pStart = view.data.data();
        size_t size;
        switch (index % 2) {
        case 0:
            pStart += view.start;
            size = view.size();
            break;
        default:
            size = view.data.size();
            break;
        }
        memcpy(pCur, pStart, size);
        pCur += size;
        ++index;
    }
    return data;
}

Expected<std::string> parseControls(const std::string &tmpl) {
    list<string_view> parts;
    auto ret = iterativeParseControls(parts, tmpl);
    if (ret.hasError()) {
        return {true, ret.getError()};
    }
    string data(tmpl.size(), 0);
    char *pCur = &data[0];
    for (const string_view &view : parts) {
        memcpy(pCur, view.front(), view.size());
        pCur += view.size();
    }
    return data;
}

tuple<string, string> processTemplate(const string &tmpl, const map<string, string> &kv) {
    auto root = canonical("assets") / "template";
    auto ret = parseIncludes(tmpl, root);
    if (ret.hasError()) {
        return make_tuple("", "Template include error: " + ret.getError());
    }
    ret = parseReplaces(ret.get(), kv);
    if (ret.hasError()) {
        return make_tuple("", "Template substitute error: " + ret.getError());
    }
    ret = parseControls(ret.get());
    if (ret.hasError()) {
        return make_tuple("", "Template controls error: " + ret.getError());
    }
    auto tup = make_tuple(move(ret.get()), "");
    assert(0 == ret.get().size());
    return tup;
}
