#include "types.hpp"
#include "serve.hpp"
#include "debug.hpp"
#include "template.hpp"
#include <client_http.hpp>
#include <server_http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace SimpleWeb;

typedef function<void(ResponsePtr, RequestPtr)> ServeFunction;

static ServeFunction serveContent(ResourceCache *cache, path resPath) {
    return [cache, resPath](ResponsePtr res, RequestPtr req) {
        auto ret = cache->serveResource(res, resPath.string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

static void serveStaticContent(HttpServer *server, ResourceCache *cache, path rootPath, string ext) {
    server->resource["^/([0-9A-Za-z-_]*)\\." + ext]["GET"] = [cache, rootPath, ext](ResponsePtr res, RequestPtr req) {
        auto file = req->path_match[1].str() + "." + ext;
        auto resPath = rootPath / ext / file;
        auto ret = cache->serveResource(res, resPath.string());
        if (ret.hasError()) {
            DEBUG_PRINT(format("ERROR - Static content 404 [%1%]: %2%\n") % ext % resPath);
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

static ServeFunction hardRedirect(HttpServer &server, string rel) {
    return [&server, rel](ResponsePtr res, RequestPtr req) {
        DEBUG_PRINT(format("REDIRECT: %1% -> %2%\n") % req->path % rel);
        server.resource[rel]["GET"](res, req);
    };
}

static ServeFunction redirect(string rel) {
    return [rel](ResponsePtr res, RequestPtr req) {
        DEBUG_PRINT(format("REDIRECT: %1% -> %2%\n") % req->path % rel);
        Header header;
        header.emplace("Location", "/");
        res->write(StatusCode::redirection_permanent_redirect, header);
    };
}

static ServeFunction serveDefault(ResourceCache *cache, path rootPath) {
    return [cache, rootPath](ResponsePtr res, RequestPtr req) {
        auto resPath = rootPath / req->path;
        if (distance(rootPath.begin(), rootPath.end()) > distance(resPath.begin(), resPath.end()) ||
                !equal(rootPath.begin(), rootPath.end(), resPath.begin())) {
            res->write(StatusCode::client_error_bad_request, "invalid access path");
            return;
        }
        if (is_directory(resPath)) {
            resPath /= "index.html";
        }

        auto ret = cache->serveResource(res, resPath.string());
        if (ret.hasError()) {
            DEBUG_PRINT(format("ERROR - Path 404: %1%\n") % resPath);
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

#ifdef TODO_WIP
static ServeFunction serveTodo(ResourceCache *cache, path rootPath) {
    return [cache, rootPath](ResponsePtr res, RequestPtr req) {
        DEBUG_PRINT(format("TODO Request: %1%\n") % req->path);
        auto ret = cache->serveResource(res, (rootPath / "html" / "todo.html").string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}
#endif

int main() {
    auto rootPath = canonical("web");
    auto htmlPath = rootPath / "html";

    HttpServer server;
    ResourceCache cache;
    server.config.port = SERVER_PORT;

    DEBUG_PRINT(format("Starting webserver on port %1%\n") % SERVER_PORT);

    serveStaticContent(&server, &cache, rootPath, "css");
    serveStaticContent(&server, &cache, rootPath, "js");
    serveStaticContent(&server, &cache, rootPath, "jpg");
    serveStaticContent(&server, &cache, rootPath, "png");
    serveStaticContent(&server, &cache, rootPath, "ico");
    server.resource["^(/[A-Za-z]{5}){1,}(/)?$"]["GET"] = redirect("/"); //hardRedirect(server, "/");
    server.default_resource["GET"] = serveDefault(&cache, rootPath);

#ifndef TODO_WIP
    serveStaticContent(&server, &cache, rootPath, "html");
    server.resource["/"]["GET"] = serveContent(&cache, htmlPath / "index.html");
    server.resource["/about"]["GET"] = serveContent(&cache, htmlPath / "about.html");
    server.resource["/projects"]["GET"] = serveContent(&cache, htmlPath / "portfolio.html");
#else
    server.resource["/"]["GET"] = serveContent(&cache, htmlPath / "todo.html");
#endif

    server.on_error = [](RequestPtr, const SimpleWeb::error_code &) {};
    thread server_thread([&server]() { server.start(); });
    server_thread.join();
}
