#pragma once

#include "fmt.hpp"
#include <string>
#include <utility>
#include <cstdarg>

namespace det {
    struct no_ret {};
}

template<typename Ret>
class Expected {
public:
    Expected(Ret &&ret) :
            m_ret(std::forward<Ret>(ret)),
            m_hasError(false) {}

    Expected(std::string fmt, ...) :
            m_hasError(true) {
        va_list args;
        va_start(args, fmt);
        m_error = fmt::vsprintf(fmt, args);
        va_end(args);
    }

    bool hasError() const {
        return m_hasError;
    }

    const std::string &getError() const {
        return m_error;
    }

    Ret &get() {
        return m_ret;
    }

    const Ret &get() const {
        return m_ret;
    }

private:
    Ret m_ret;
    bool m_hasError;
    std::string m_error;
};
