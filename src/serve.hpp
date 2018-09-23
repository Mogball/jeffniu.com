#pragma once

#include "types.hpp"
#include "error.hpp"

Expected<bool> serveResource(const ResponsePtr &res, std::string resPath);
