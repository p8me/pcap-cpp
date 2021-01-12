// out.h
// Created by p8me in Nov 2020.

#ifndef ___OUT_H__
#define ___OUT_H__

extern bool verbose;
#include <iostream>

#define DEBUG_OUT(...)                             \
    {                                              \
        if (verbose)                               \
            std::cout << __VA_ARGS__ << std::endl; \
    }
#define WARNING_OUT(...) std::cout << __VA_ARGS__ << std::endl;
#define ERROR_OUT(...) std::cerr << __VA_ARGS__ << std::endl;
#define FATAL_OUT_EXIT(...)                                                                                \
    {                                                                                                      \
        std::cerr << __FILE__ << ":" << __LINE__ << ":" << __func__ << "(): " << __VA_ARGS__ << std::endl; \
        exit(1);                                                                                           \
    }

#endif // ___OUT_H__
