#include "gzip.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "serve.hpp"
#include "types.hpp"
#include "strutil.hpp"
#include "mimetype.hpp"
#include "template.hpp"
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;
using namespace boost::filesystem;

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

Resource::Resource(Timestamp createTime, Timestamp duration, size_t size):
    m_createTime(createTime),
    m_duration(duration),
    m_data(size, 0) {}

Resource::Resource(Timestamp createTime, Timestamp duration, string &&data):
    m_createTime(createTime),
    m_duration(duration),
    m_data(forward<string>(data)) {}

ResourceCache::ResourceCache():
    m_cache(NUM_RESOURCES * 2) {}

Expected<bool> ResourceCache::serveResource(
        const ResponsePtr &res,
        path resource,
        const map<string, string> &kv) {
    Resource::Timestamp curTime = getTimestamp();
    Mime mimeType(resource);
    Header header;
    header.emplace("Content-Type", mimeType.repr());
    if (mimeType.canGzip()) {
        header.emplace("Content-Encoding", "gzip");
    }

    string resPath = resource.string();
    auto it = m_cache.find(resPath);
    if (it != m_cache.end()) {
        Resource &cached = it->second;
        if (cached.m_createTime + cached.m_duration > curTime) {
            header.emplace("Content-Length", to_string(cached.m_data.size()));
            res->write(header);
            readAndSend(res, cached.m_data.begin(), cached.m_data.end());
            return true;
        } else {
            DEBUG_PRINT(format("Cache timeout: %1%\n") % resPath);
            m_cache.erase(it);
        }
    }

    DEBUG_PRINT(format("Loading resource: %1%\n") % resPath);
    ifstream ifs(resPath, ifstream::in | ios::binary | ios::ate);
    if (!ifs) {
        return {true, "ERROR: could not read file: " + resPath};
    }
    ifs.seekg(0, ios::end);
    size_t numBytes = ifs.tellg();
    ifs.seekg(0, ios::beg);

    string data(numBytes, 0);
    streamsize bytesRead = ifs.read(&data[0], data.size()).gcount();
    assert(numBytes == bytesRead);
    assert(numBytes == data.size());

    if (ends_with(resPath, "tmpl.html")) {
        DEBUG_PRINT("Processing template ...\n");
        auto ret = processTemplate(data, kv);
        if (!get<1>(ret).empty()) {
            return {true, get<1>(ret)};
        }
        force_move(data, get<0>(ret));
        assert(0 == get<0>(ret).size());
    }
    if (mimeType.canGzip()) {
        string gzipped = compress(data);
        DEBUG_PRINT(format("Compression %1%\n")
                % (100.0 * gzipped.size() / data.size()));
        force_move(data, gzipped);
        assert(0 == gzipped.size());
    }
    header.emplace("Content-Length", to_string(data.size()));
    res->write(header);

    readAndSend(res, data.begin(), data.end());
    Resource newData(curTime, CACHE_TIMEOUT, move(data));
    assert(0 == data.size());
    auto pVal = m_cache.emplace(move(resPath), move(newData));
    assert(true == pVal.second);
    assert(0 == newData.m_data.size());
    Resource::Buffer &bufRef = pVal.first->second.m_data;
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
