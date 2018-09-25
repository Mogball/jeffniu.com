#pragma once

#include <string>
#include <cstddef>

struct string_view {
    const std::string &data;
    std::size_t start;
    std::size_t end;

    explicit string_view(const std::string &t_data, std::size_t t_start = 0, std::size_t t_end = 0);

    std::size_t size() const;
    std::string get() const;
    const char *front() const;

    bool operator==(const string_view &o) const;
    bool operator!=(const string_view &o) const;
    bool operator< (const string_view &o) const;
    bool operator<=(const string_view &o) const;
    bool operator> (const string_view &o) const;
    bool operator>=(const string_view &o) const;
};
