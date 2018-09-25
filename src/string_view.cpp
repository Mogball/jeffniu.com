#include "string_view.hpp"
#include <cstddef>
#include <cstring>

using namespace std;

string_view::string_view(const string &t_data, size_t t_start, size_t t_end):
    data(t_data),
    start(t_start),
    end(t_end) {}

size_t string_view::size() const {
    return end - start;
}

string string_view::get() const {
    return data.substr(start, size());
}

const char *string_view::front() const {
    return data.data() + start;
}

bool string_view::operator==(const string_view &o) const {
    if (size() != o.size()) {
        return false;
    }
    return 0 == strncmp(front(), o.front(), size());
}

bool string_view::operator<(const string_view &o) const {
    size_t least = min(size(), o.size());
    int val = strncmp(front(), o.front(), least);
    if (val != 0) {
        return val < 0;
    }
    return size() < o.size();
}

bool string_view::operator!=(const string_view &o) const {
    return !(*this == o);
}

bool string_view::operator>(const string_view &o) const {
    return o < *this;
}

bool string_view::operator<=(const string_view &o) const {
    return !(o < *this);
}

bool string_view::operator>=(const string_view &o) const {
    return !(*this < o);
}
