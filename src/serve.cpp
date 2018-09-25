#include "debug.hpp"
#include "serve.hpp"
#include "types.hpp"
#include "error.hpp"
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

static constexpr long NUM_RESOURCES = 128;
static constexpr long BUFFER_SIZE = 1 << 17;
static constexpr long CACHE_TIMEOUT = 60 * 60 * 1000; // 60 minutes

static long long getTimestamp() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    ).count();
}

Resource::Resource(Timestamp createTime, Timestamp duration, size_t size):
    m_createTime(createTime),
    m_duration(duration),
    m_data(size) {}

ResourceCache::ResourceCache():
    m_cache(NUM_RESOURCES * 2) {}

Expected<bool> ResourceCache::serveResource(const ResponsePtr &res, string resPath) {
    Resource::Timestamp curTime = getTimestamp();
    Header header;

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
    auto pVal = m_cache.emplace(move(resPath), move(newData));
    assert(true == pVal.second);
    assert(0 == newData.m_data.capacity());

    header.emplace("Content-Length", to_string(numBytes));
    res->write(header);

    Resource::Buffer &bufferRef = pVal.first->second.m_data;
    streamsize bytesRead = ifs.read(bufferRef.data(), bufferRef.capacity()).gcount();

    assert(numBytes == bytesRead);
    assert(numBytes == bufferRef.size());
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
