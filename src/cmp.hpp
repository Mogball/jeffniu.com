#pragma once

enum cmp_op {
    GT = 'g' + 't',
    GE = 'g' + 'e',
    LT = 'l' + 't',
    LE = 'l' + 'e',
    EQ = 'e' + 'q',
    NE = 'e' + 'q'
};

struct string_view;

bool compare(const string_view &a, const string_view &b, cmp_op op_code);
