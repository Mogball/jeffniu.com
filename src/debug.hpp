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
#define DEBUG_PRINT(x) std::cout << x
#else
#define DEBUG_PRINT(x)
#endif

#ifdef TODO_WIP
#warning "WARNING: Building TODO version"
#endif
