#pragma once

#include "types.hpp"
#include <boost/filesystem/path.hpp>
#include <map>
#include <string>

typedef std::function<void(ResponsePtr, RequestPtr)> ServeFunction;

ServeFunction serveContent(
        ResourceCache *cache,
        const KVMap &defConfig,
        path resPath,
        const KVMap &config);

ServeFunction hardRedirect(
        HttpServer &server,
        std::string rel);

ServeFunction redirect(
        std::string rel);

ServeFunction serveDefault(
        ResourceCache *cache,
        path rootPath);

void serveStaticContent(
        HttpServer *server,
        ResourceCache *cache,
        path rootPath,
        std::string ext);
