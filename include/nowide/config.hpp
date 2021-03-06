//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019 - 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_CONFIG_HPP_INCLUDED
#define NOWIDE_CONFIG_HPP_INCLUDED

/// @file

#include <nowide/replacement.hpp>

//! @cond Doxygen_Suppress

#if(defined(__WIN32) || defined(_WIN32) || defined(WIN32)) && !defined(__CYGWIN__)
#define NOWIDE_WINDOWS
#endif

#ifdef _MSC_VER
#define NOWIDE_MSVC _MSC_VER
#endif

#ifdef __GNUC__
#define NOWIDE_GCC __GNUC__
#endif // __GNUC__

#ifdef __clang__
#define NOWIDE_CLANG __clang__
#endif // __clang__

#ifdef NOWIDE_GCC
#define NOWIDE_SYMBOL_VISIBLE __attribute__((__visibility__("default")))
#endif

#ifndef NOWIDE_SYMBOL_VISIBLE
#define NOWIDE_SYMBOL_VISIBLE
#endif

#ifdef NOWIDE_WINDOWS
#define NOWIDE_SYMBOL_EXPORT __declspec(dllexport)
#define NOWIDE_SYMBOL_IMPORT __declspec(dllimport)
#else
#define NOWIDE_SYMBOL_EXPORT NOWIDE_SYMBOL_VISIBLE
#define NOWIDE_SYMBOL_IMPORT
#endif

#ifdef NOWIDE_DYN_LINK
#ifdef NOWIDE_SOURCE
#define NOWIDE_DECL NOWIDE_SYMBOL_EXPORT
#else
#define NOWIDE_DECL NOWIDE_SYMBOL_IMPORT
#endif // NOWIDE_SOURCE
#else
#define NOWIDE_DECL
#endif // NOWIDE_DYN_LINK

#ifndef NOWIDE_LIKELY_IF
#ifdef NOWIDE_GCC
#define NOWIDE_LIKELY_IF(x) if(__builtin_expect(!!(x), 1))
#elif __has_cpp_attribute(likely)
#define NOWIDE_LIKELY_IF(x) [[likely]] if(x)
#else
#define NOWIDE_LIKELY_IF(x) if(x)
#endif // NOWIDE_GCC
#endif // !NOWIDE_LIKELY_IF

#ifndef NOWIDE_UNLIKELY_IF
#ifdef NOWIDE_GCC
#define NOWIDE_UNLIKELY_IF(x) if(__builtin_expect(!!(x), 0))
#elif __has_cpp_attribute(likely)
#define NOWIDE_UNLIKELY_IF(x) [[unlikely]] if(x)
#else
#define NOWIDE_UNLIKELY_IF(x) if(x)
#endif // NOWIDE_GCC
#endif // !NOWIDE_UNLIKELY_IF

#ifndef NOWIDE_ELSE
#define NOWIDE_ELSE(x) else x
#endif // !NOWIDE_ELSE

#if defined(NOWIDE_MSVC) || defined(NOWIDE_GCC) || defined(NOWIDE_CLANG)
#define NOWIDE_RESTRICT __restrict
#else
#define NOWIDE_RESTRICT
#endif // NOWIDE_MSVC || NOWIDE_GCC || NOWIDE_CLANG

//! @endcond

///
/// \brief This namespace includes implementations of the standard library functions and
/// classes such that they accept UTF-8 strings on Windows.
/// On other platforms (i.e. not on Windows) those functions and classes are just aliases
/// of the corresponding ones from the std namespace or behave like them.
///
namespace nowide {}

#endif // boost/nowide/config.hpp
