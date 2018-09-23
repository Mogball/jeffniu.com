#pragma once

#include <client_http.hpp>
#include <server_http.hpp>
#include <client_https.hpp>
#include <server_https.hpp>

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
typedef SimpleWeb::Server<SimpleWeb::HTTPS> HttpsServer;
typedef SimpleWeb::Client<SimpleWeb::HTTPS> HttpsClient;

typedef SimpleWeb::error_code errcode_t;
typedef SimpleWeb::CaseInsensitiveMultimap Header;

typedef std::shared_ptr<HttpServer::Response> ResponsePtr;
typedef std::shared_ptr<HttpServer::Request>  RequestPtr;
