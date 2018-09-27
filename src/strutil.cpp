#include "strutil.hpp"
#include <algorithm>

using namespace std;

bool ends_with(const string &val, const string &end) {
    if (end.size() > val.size()) {
        return false;
    }
    return equal(end.rbegin(), end.rend(), val.rbegin());
}
