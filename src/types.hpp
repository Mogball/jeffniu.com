#pragma once

#include <client_http.hpp>
#include <server_http.hpp>
#include <client_https.hpp>
#include <server_https.hpp>
#include <boost/filesystem/path.hpp>
#include <map>
#include <string>
#include <vector>

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
typedef SimpleWeb::Server<SimpleWeb::HTTPS> HttpsServer;
typedef SimpleWeb::Client<SimpleWeb::HTTPS> HttpsClient;

typedef SimpleWeb::error_code errcode_t;
typedef SimpleWeb::CaseInsensitiveMultimap Header;

typedef std::shared_ptr<HttpServer::Response> ResponsePtr;
typedef std::shared_ptr<HttpServer::Request>  RequestPtr;

typedef boost::filesystem::path path;
typedef std::map<std::string, std::string> KVMap;

extern template class SimpleWeb::Server<SimpleWeb::HTTP>;
extern template class SimpleWeb::Client<SimpleWeb::HTTP>;
extern template class SimpleWeb::Server<SimpleWeb::HTTPS>;
extern template class SimpleWeb::Client<SimpleWeb::HTTPS>;

extern template class std::basic_string<char>;
extern template class std::map<std::string, std::string>;
extern template class std::vector<std::string>;
