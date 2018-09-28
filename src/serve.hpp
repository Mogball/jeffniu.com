#pragma once

#include "types.hpp"
#include "error.hpp"
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <string>

static_assert(sizeof(long long) >= 8, "Expected sizeof(long long) to be 64 bits");

class ResourceCache;

class Resource {
    friend class ResourceCache;

    typedef long long Timestamp;
    typedef std::string Buffer;
    typedef Buffer::iterator BufferIt;

    Resource(Timestamp createTime, Timestamp duration, std::size_t size);
    Resource(Timestamp createTime, Timestamp duration, std::string &&size);

    Timestamp m_createTime;
    Timestamp m_duration;
    Buffer m_data;
};

class ResourceCache {
public:
    ResourceCache();

    Expected<bool> serveResource(
            const ResponsePtr &res,
            boost::filesystem::path resPath,
            const std::map<std::string, std::string> &kv);

    ResourceCache(const ResourceCache &) = delete;
    ResourceCache &operator=(const ResourceCache &) = delete;

private:

    void readAndSend(
            const ResponsePtr &res,
            Resource::BufferIt begin,
            Resource::BufferIt end);

    std::unordered_map<std::string, Resource> m_cache;
};
