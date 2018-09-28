#include "strutil.hpp"
#include <algorithm>
#include <utility>

using namespace std;

bool ends_with(const string &val, const string &end) {
    if (end.size() > val.size()) {
        return false;
    }
    return equal(end.rbegin(), end.rend(), val.rbegin());
}

void force_move(string &dst, string &src) {
    dst.~string();
    ::new(&dst) string(move(src));
}
