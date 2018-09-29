#include "debug.hpp"
#include "serve.hpp"
#include "request.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

int main() {
    auto rootPath = canonical("assets");
    auto htmlPath = rootPath / "html";
    auto tmplPath = rootPath / "template";

    HttpServer server;
    ResourceCache cache;
    server.config.port = SERVER_PORT;

    DEBUG_PRINT(format("Starting webserver on port %1%\n") % SERVER_PORT);

    serveStaticContent(&server, &cache, rootPath, "css");
    serveStaticContent(&server, &cache, rootPath, "js");
    serveStaticContent(&server, &cache, rootPath, "jpeg");
    serveStaticContent(&server, &cache, rootPath, "png");
    serveStaticContent(&server, &cache, rootPath, "ico");
    serveStaticContent(&server, &cache, rootPath, "pdf");
    server.resource["^(/[A-Za-z]{5}){1,}(/)?$"]["GET"] = redirect("/"); //hardRedirect(server, "/");
    server.default_resource["GET"] = serveDefault(&cache, rootPath);

    static const KVMap defaultConfig{
        {"urlGithub",   "https://github.com/mogball"},
        {"urlLinkedIn", "https://linkedin.com/in/jeffniu22"},
        {"urlInsta",    "https://instagram.com/jeffniu2w"},
        {"urlRoot",     "/"},
        {"urlAbout",    "/about"},
        {"urlProjects", "/projects"},
        {"urlEmail", "me@jeffniu.com"},

        {"urlPhImg", "https://spacergif.org/spacer.gif"},

        {"phoneNum", "1-905-806-8846"},
    };

#ifndef TODO_WIP
    server.resource["/"]["GET"] = serveContent(&cache, defaultConfig, tmplPath / "index.tmpl.html",
        {{"navActive", "1"}});
    server.resource["/about"]["GET"] = serveContent(&cache, defaultConfig, tmplPath / "about.tmpl.html",
        {{"navActive", "2"}});
    server.resource["/projects"]["GET"] = serveContent(&cache, defaultConfig, tmplPath / "portfolio.tmpl.html",
        {{"navActive", "3"}});
    server.resource["/superset"]["GET"] = serveContent(&cache, defaultConfig, tmplPath / "superset.tmpl.html", {});
#else
    server.resource["/"]["GET"] = serveContent(&cache, defaultConfig, htmlPath / "todo.html", {});
#endif

    server.on_error = [](RequestPtr, const SimpleWeb::error_code &) {};
    thread server_thread([&server]() { server.start(); });
    server_thread.join();
}
