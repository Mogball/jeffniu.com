#pragma once

#include <string>
#include <cstdarg>

namespace fmt {
    std::string sprintf(std::string fmt, ...);
    std::string vsprintf(std::string fmt, va_list args);
}
