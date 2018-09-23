#include "fmt.hpp"
#include <vector>
#include <stdexcept>

using namespace std;

string fmt::sprintf(string fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string ret = fmt::vsprintf(fmt, args);
    va_end(args);
    return ret;
}

string fmt::vsprintf(string fmt, va_list args) {
    vector<char> buf(2 * fmt.size());
    va_list again;
    va_copy(again, args);
    int wrt = vsnprintf(buf.data(), buf.capacity(), fmt.c_str(), args);
    if (wrt < 0) {
        throw runtime_error("fmt::vsprintf string encoding error");
    }
    if (wrt >= buf.capacity()) {
        buf.resize(wrt + 1);
        wrt = vsnprintf(buf.data(), buf.capacity(), fmt.c_str(), again);
        if (wrt < 0 || wrt >= buf.capacity()) {
            throw runtime_error("fmt::vsprintf buffer overflow");
        }
    }
    va_end(again);
    return {buf.data(), static_cast<string::size_type>(wrt)};
}
