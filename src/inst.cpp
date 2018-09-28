#include "types.hpp"

template class SimpleWeb::Server<SimpleWeb::HTTP>;
template class SimpleWeb::Client<SimpleWeb::HTTP>;
template class SimpleWeb::Server<SimpleWeb::HTTPS>;
template class SimpleWeb::Client<SimpleWeb::HTTPS>;

template class std::shared_ptr<HttpServer::Response>;
template class std::shared_ptr<HttpServer::Request>;

template class std::basic_string<char>;
template class std::map<std::string, std::string>;
template class std::vector<std::string>;
