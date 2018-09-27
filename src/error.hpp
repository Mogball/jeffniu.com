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

    Expected(bool hasError, std::string msg):
            m_hasError(hasError),
            m_error(std::move(msg)) {}

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

    Ret &&move() {
        return std::move(m_ret);
    }

private:
    Ret m_ret;
    bool m_hasError;
    std::string m_error;
};
