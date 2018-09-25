#include "cmp.hpp"
#include "string_view.hpp"

bool compare(const string_view &a, const string_view &b, cmp_op op_code) {
    switch (op_code) {
        case GT: return a >  b;
        case GE: return a >= b;
        case LT: return a <  b;
        case LE: return a <= b;
        case EQ: return a == b;
        default: return a != b;
    }
}
