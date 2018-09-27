#include "debug.hpp"
#include "serve.hpp"
#include "types.hpp"
#include "error.hpp"
#include "strutil.hpp"
#include "template.hpp"
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

enum MimeType {
    JS,
    CSS,
    HTML
};

static constexpr long NUM_RESOURCES = 128;
static constexpr long BUFFER_SIZE = 1 << 17;
static constexpr long CACHE_TIMEOUT = 60 * 60 * 1000; // 60 minutes

static long long getTimestamp() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    ).count();
}

static vector<string> s_mimeExts = {"js", "css", "html"};

static string getMimeType(const string &resPath) {
    size_t loc = resPath.rfind('.');
    if (string::npos == loc) {
        return "text/plain";
    }
    ++loc;
    string ext = resPath.substr(loc, resPath.size() - loc);
    switch (find(s_mimeExts.begin(), s_mimeExts.end(), ext) - s_mimeExts.begin()) {
        case JS:
            return "text/javascript";
        case CSS:
            return "text/css";
        case HTML:
            return "text/html";
        default:
            return "text/plain";
    }
}

Resource::Resource(Timestamp createTime, Timestamp duration, size_t size):
    m_createTime(createTime),
    m_duration(duration),
    m_data(size, 0) {}

ResourceCache::ResourceCache():
    m_cache(NUM_RESOURCES * 2) {}

Expected<bool> ResourceCache::serveResource(
        const ResponsePtr &res,
        string resPath,
        const map<string, string> &kv) {
    Resource::Timestamp curTime = getTimestamp();
    Header header;
    header.emplace("Content-Type", getMimeType(resPath));

    auto it = m_cache.find(resPath);
    if (it != m_cache.end()) {
        Resource &cached = it->second;
        if (cached.m_createTime + cached.m_duration > curTime) {
            DEBUG_PRINT(format("Used cached value: %1%\n") % resPath);
            header.emplace("Content-Length", to_string(cached.m_data.size()));
            res->write(header);
            readAndSend(res, cached.m_data.begin(), cached.m_data.end());
            return true;
        } else {
            DEBUG_PRINT(format("Cache timeout: %1%\n") % resPath);
            m_cache.erase(it);
        }
    }

    ifstream ifs(resPath, ifstream::in | ios::binary | ios::ate);
    if (!ifs) {
        return {true, "ERROR: could not read file: " + resPath};
    }
    ifs.seekg(0, ios::end);
    size_t numBytes = ifs.tellg();
    ifs.seekg(0, ios::beg);

    Resource newData(curTime, CACHE_TIMEOUT, numBytes);
    header.emplace("Content-Length", to_string(numBytes));
    res->write(header);

    streamsize bytesRead = ifs.read(
        &newData.m_data[0],
        newData.m_data.capacity()
    ).gcount();
    assert(numBytes == bytesRead);
    assert(numBytes == newData.m_data.size());

    if (ends_with(resPath, "tmpl.html")) {
        DEBUG_PRINT(format("Processing template: %1%\n") % resPath);
        auto ret = processTemplate(newData.m_data, kv);
        if (!get<1>(ret).empty()) {
            return {true, get<1>(ret)};
        }
        string data = move(get<0>(ret));
        assert(0 == get<0>(ret).size());
        newData.m_data = data;
    }

    auto pVal = m_cache.emplace(resPath, move(newData));
    assert(true == pVal.second);
    assert(0 == newData.m_data.size());
    Resource::Buffer &bufferRef = pVal.first->second.m_data;

    if (bytesRead > 0) {
        readAndSend(res, bufferRef.begin(), bufferRef.end());
    }
    ifs.close();
    return true;
}

void ResourceCache::readAndSend(
        const ResponsePtr &res,
        Resource::BufferIt begin,
        Resource::BufferIt end) {
   auto wrt = min(BUFFER_SIZE, end - begin);
   res->write(&*begin, min(BUFFER_SIZE, end - begin));
   begin += wrt;
   if (begin != end) {
       res->send([this, res, begin, end](const errcode_t &ec) {
        if (!ec) {
            readAndSend(res, begin, end);
        } else {
            DEBUG_PRINT("ERROR: Connection interrupted\n");
        }
       });
   }
}
