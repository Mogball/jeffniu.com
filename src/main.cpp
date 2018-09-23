#include "types.hpp"
#include "serve.hpp"
#include <client_http.hpp>
#include <server_http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;
using namespace boost::filesystem;
using namespace SimpleWeb;

typedef function<void(ResponsePtr, RequestPtr)> ServeFunction;

static ServeFunction serveResource(path resPath) {
    return [resPath](ResponsePtr res, RequestPtr req) {
        auto ret = serveResource(res, resPath.string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

static void serveStaticContent(HttpServer *server, path rootPath, string ext) {
    server->resource["^/([0-9A-Za-z_]*)\\." + ext]["GET"] = [rootPath, ext](ResponsePtr res, RequestPtr req) {
        auto file = req->path_match[1].str() + "." + ext;
        auto ret = serveResource(res, (rootPath / ext / file).string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

static ServeFunction serveDefault(path rootPath) {
    return [rootPath](ResponsePtr res, RequestPtr req) {
        auto resPath = canonical(rootPath / req->path);
        if (distance(rootPath.begin(), rootPath.end()) > distance(resPath.begin(), resPath.end()) ||
                !equal(rootPath.begin(), rootPath.end(), resPath.begin())) {
            throw invalid_argument("path must be within root path");
        }
        if (is_directory(resPath)) {
            resPath /= "index.html";
        }

        auto ret = serveResource(res, resPath.string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_not_found, ret.getError());
        }
    };
}

int main() {
    auto rootPath = canonical("web");
    auto htmlPath = rootPath / "html";

    HttpServer server;
    server.config.port = 8080;

    serveStaticContent(&server, rootPath, "html");
    serveStaticContent(&server, rootPath, "css");
    serveStaticContent(&server, rootPath, "js");
    serveStaticContent(&server, rootPath, "jpeg");
    server.resource["/"]["GET"] = serveResource(htmlPath / "index.html");
    server.default_resource["GET"] = serveDefault(rootPath);
    server.on_error = [](RequestPtr, const SimpleWeb::error_code &) {};

    thread server_thread([&server]() { server.start(); });
    server_thread.join();
}
