#include "serve.hpp"
#include <string>

using namespace std;

static constexpr long NUM_RESOURCES = 128;

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
