#pragma once

#include "error.hpp"
#include <boost/filesystem.hpp>
#include <string>
#include <tuple>
#include <map>

Expected<std::string> parseIncludes(const std::string &tmpl, const boost::filesystem::path &root);
Expected<std::string> parseControls(const std::string &tmpl);
Expected<std::string> parseReplaces(const std::string &tmpl, const std::map<std::string, std::string> &kv);
std::tuple<std::string, std::string> processTemplate(const std::string &res, const std::map<std::string, std::string> &kv);
