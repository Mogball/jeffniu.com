cmake_minimum_required(VERSION 3.1.0)
project(website VERSION 1.0 LANGUAGES CXX)

add_subdirectory(webserver)
find_package(Boost 1.53.0 COMPONENTS system thread filesystem REQUIRED)

add_executable(website website.cpp)
target_link_libraries(website simple-web-server ${Boost_LIBRARIES})
target_include_directories(website PRIVATE webserver ${Boost_INCLUDE_DIR})
set_target_properties(website PROPERTIES CXX_STANDARD 14)
