#pragma once

#include "error.hpp"
#include <boost/filesystem.hpp>
#include <string>

Expected<std::string> parseIncludes(const std::string &tmpl, const boost::filesystem::path &root);
