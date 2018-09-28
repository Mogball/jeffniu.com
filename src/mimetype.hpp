#pragma once

#include <boost/filesystem.hpp>
#include <string>

class Mime {
public:
    typedef boost::filesystem::path Path;

    explicit Mime(const Path &path);

    enum Type {
        JS,
        CSS,
        HTML,

        PNG,
        JPEG,
        ICO,
        SVG,

        WOFF,
        WOFF2,
        TTF,
        EOT,

        UNKNOWN
    };

    Type type() const;
    bool canGzip() const;
    const std::string &repr() const;

private:
    Type m_type;
};
