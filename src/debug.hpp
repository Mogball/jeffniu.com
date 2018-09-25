#pragma once

#ifndef BOOST_SPIRIT_THREADSAFE
#error "Please define BOOST_SPIRIT_THREADSAFE"
#endif

#ifndef SERVER_PORT
#error "SERVER_PORT must be defined"
#endif

#ifdef WEB_DEBUG
#include <iostream>
#include <boost/format.hpp>
#define DEBUG_PRINT(x) { using boost::format; std::cout << x; }
#else
#define DEBUG_PRINT(x)
#endif

#ifdef WEB_DEBUG
#define DEBUG_IF(cond, expr) if(cond) { expr; }
#else
#define DEBUG_IF(cond, expr)
#endif

#ifdef TODO_WIP
#warning "WARNING: Building TODO version"
#endif
