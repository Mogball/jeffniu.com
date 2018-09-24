cmake_minimum_required(VERSION 3.1.0)
project(website VERSION 1.0 LANGUAGES CXX)

add_subdirectory(webserver)
find_package(Boost 1.53.0 COMPONENTS filesystem)

file(GLOB SRCS src/*.cpp)
file(GLOB HDRS src/*.hpp)
add_executable(website ${SRCS} ${HDRS})
target_link_libraries(website simple-web-server ${Boost_LIBRARIES})
target_include_directories(website PRIVATE webserver ${Boost_INCLUDE_DIR})
set_target_properties(website PROPERTIES CXX_STANDARD 14)
target_compile_definitions(website PUBLIC
    BOOST_SPIRIT_THREADSAFE
    TODO_WIP
    WEB_DEBUG
    SERVER_PORT=8080)
