#include "debug.hpp"
#include "serve.hpp"
#include "request.hpp"
#include <status_code.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace SimpleWeb;

static const map<string, string> s_kv;

ServeFunction serveContent(
        ResourceCache *cache,
        const KVMap &defConfig,
        path resPath,
        const KVMap &config) {
    KVMap kv(config);
    kv.insert(defConfig.begin(), defConfig.end());
    return [kv, cache, resPath](ResponsePtr res, RequestPtr req) {
        auto ret = cache->serveResource(res, resPath, kv);
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

ServeFunction hardRedirect(
        HttpServer &server,
        std::string rel) {
    return [&server, rel](ResponsePtr res, RequestPtr req) {
        DEBUG_PRINT(format("REDIRECT: %1% -> %2%\n") % req->path % rel);
        server.resource[rel]["GET"](res, req);
    };
}

ServeFunction redirect(
        std::string rel) {
    return [rel](ResponsePtr res, RequestPtr req) {
        DEBUG_PRINT(format("REDIRECT: %1% -> %2%\n") % req->path % rel);
        Header header;
        header.emplace("Location", "/");
        res->write(StatusCode::redirection_permanent_redirect, header);
    };
}

ServeFunction serveDefault(
        ResourceCache *cache,
        path rootPath) {
    return [cache, rootPath](ResponsePtr res, RequestPtr req) {
        auto resPath = rootPath / req->path;
        if (distance(rootPath.begin(), rootPath.end()) > distance(resPath.begin(), resPath.end()) ||
                !equal(rootPath.begin(), rootPath.end(), resPath.begin())) {
            res->write(StatusCode::client_error_bad_request, "invalid access path");
            return;
        }
        auto ret = cache->serveResource(res, resPath, s_kv);
        if (ret.hasError()) {
            DEBUG_PRINT(format("ERROR - Path 404: %1%\n") % resPath);
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

void serveStaticContent(
        HttpServer *server,
        ResourceCache *cache,
        path rootPath,
        std::string ext) {
    server->resource["^/([0-9A-Za-z-_]*)\\." + ext]["GET"] = [cache, rootPath, ext](ResponsePtr res, RequestPtr req) {
        auto file = req->path_match[1].str() + "." + ext;
        auto resPath = rootPath / ext / file;
        auto ret = cache->serveResource(res, resPath, s_kv);
        if (ret.hasError()) {
            DEBUG_PRINT(format("ERROR - Static content 404 [%1%]: %2%\n") % ext % resPath);
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}
