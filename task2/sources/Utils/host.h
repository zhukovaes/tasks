/**
 * @file: host.h 
 * Assertion related routines of Utils library for compiler prototype project
 * @defgroup Asserts Assertions
 * @brief Assertion routines/macros
 * @ingroup Utils
 */
/*
 * Copyright (C) 2012  Boris Shurygin
 */
#pragma once
#ifndef UTILS_HOST_H
#define UTILS_HOST_H


/* Determine OS */
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define OS_WIN
#  define OS_WIN32
#  define OS_WIN64
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define OS_WIN
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define OS_WINCE
#  else
#    define OS_WIN32
#  endif
#elif defined (__linux__)
#  define OS_LINUX
#endif

/* Determine compiler */
#if defined(_MSC_VER)
#  define Q_CC_MSVC
#  if defined(__INTEL_COMPILER)
#    define CC_INTEL
#  endif
#elif defined(__GNUC__)
#  define CC_GNU
#  if defined(__MINGW32__)
#    define CC_MINGW
#  endif
#  if defined(__INTEL_COMPILER)
#    define CC_INTEL
#  endif
#endif

/* System headers */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <list>
#include <stack>
#include <vector>
#include <cstdio>
#include <cstdarg>
#include <numeric>
#include <iomanip>
#include <exception>
#include <set>
#include <bitset>
#include <iterator>

using namespace std;

/* Path-related routines */
namespace Host
{
    /**
     * Concatenate path strings
     */
    inline std::string concatPaths( const std::string &basedir,
                                    const std::string &subpath)
    {
        char sep = '/'; //Linux separator
        string tmp = basedir;

#ifdef OS_WIN
        sep = '\\'; // Windows separator
#endif

        if ( basedir[ basedir.length()] != sep) { // Check if basedir string ends with separator
            tmp += sep;                //add separator
            return (tmp + subpath);
        } else
        {
            return (basedir + subpath);
        }
    }
}

#endif /* UTILS_HOST_H */
