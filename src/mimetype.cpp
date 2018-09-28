#include "mimetype.hpp"
#include <algorithm>
#include <vector>
#include <cassert>

using namespace std;
using namespace boost::filesystem;

static vector<string> s_extNames = {
    ".js",
    ".css",
    ".html",

    ".png",
    ".jpeg",
    ".ico",
    ".svg",

    ".woff",
    ".woff2",
    ".ttf",
    ".eot",
};

static vector<string> s_mimeTypes = {
    "application/javascript",
    "text/css",
    "text/html",

    "image/png",
    "image/jpeg",
    "image/x-icon",
    "image/svg+xml",

    "application/font-woff",
    "application/font-woff2",
    "application/x-font-ttf",
    "application/vnd.ms-fontobject",
};

static Mime::Type getExtensionType(const Mime::Path &path) {
    auto it = find(s_extNames.begin(), s_extNames.end(), path.extension());
    auto index = it - s_extNames.begin();
    assert(0 <= index && index < Mime::Type::UNKNOWN && "Invalid MIME type");
    return static_cast<Mime::Type>(index);
}

Mime::Mime(const Path &path):
    m_type(getExtensionType(path)) {}

Mime::Type Mime::type() const {
    return m_type;
}

bool Mime::canGzip() const {
    return JS <= m_type && m_type <= HTML;
}

const string &Mime::repr() const {
    return s_mimeTypes[m_type];
}
