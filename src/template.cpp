#include "template.hpp"
#include "error.hpp"
#include <utility>
#include <list>
#include <tuple>
#include <fstream>
#include <string>
#include <cassert>

using namespace std;
using namespace boost::filesystem;

static string INCLUDE_MATCH = "{{include";
static string END_MATCH = "}}";

struct string_view {
    const string &data;
    size_t start;
    size_t end;
};

static tuple<string, bool> readWholeFile(const path &root, const string &name) {
    ifstream ifs((root / name).string());
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

static Expected<bool> recursiveParseIncludes(list<string_view> &parts, list<string> &alloc, const string &tmpl, const path &root) {
    size_t start = 0;
    size_t size = tmpl.size();
    while (start < size) {
        bool quit = false;
        size_t loc = tmpl.find(INCLUDE_MATCH, start);
        if (string::npos == loc) {
            quit = true;
            loc = size;
        }
        string_view view = {.data = tmpl, .start = start, .end = loc};
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
        auto data = readWholeFile(root, included);
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

Expected<string> parseIncludes(const string &tmpl, const path &root) {
    list<string_view> parts;
    list<string> alloc;
    auto ret = recursiveParseIncludes(parts, alloc, tmpl, root);
    if (ret.hasError()) {
        return {true, ret.getError()};
    }
    size_t total = 0;
    for (const string_view &view : parts) {
        total += view.end - view.start;
    }
    string data(total, 0);
    char *pCur = &data[0];
    for (const string_view &view : parts) {
        const char *pStart = &view.data[0] + view.start;
        size_t size = view.end - view.start;
        memcpy(pCur, pStart, size);
        pCur += size;
    }
    return data;
}
