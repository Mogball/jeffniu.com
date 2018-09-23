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

static function<void(ResponsePtr, RequestPtr)> serveResource(path resPath) {
    return [resPath](ResponsePtr res, RequestPtr req) {
        auto ret = serveResource(res, resPath.string());
        if (ret.hasError()) {
            res->write(StatusCode::client_error_bad_request, ret.getError());
        }
    };
}

static void serveDefault(ResponsePtr res, RequestPtr req) {
    auto rootPath = canonical("web");
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
        res->write(StatusCode::client_error_bad_request, ret.getError());
    }
}

int main() {
    auto rootPath = canonical("web");
    auto htmlPath = rootPath / "html";

    HttpServer server;
    server.config.port = 8080;

    server.resource["/"]["GET"] = serveResource(htmlPath / "index.html");
    server.default_resource["GET"] = serveDefault;
    server.on_error = [](RequestPtr, const SimpleWeb::error_code &) {};

    thread server_thread([&server]() { server.start(); });
    server_thread.join();
}
