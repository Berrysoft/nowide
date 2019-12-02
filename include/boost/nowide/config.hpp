//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_CONFIG_HPP_INCLUDED
#define BOOST_NOWIDE_CONFIG_HPP_INCLUDED

/// @file

#include <boost/config.hpp>
#include <boost/nowide/replacement.hpp>

#ifndef BOOST_SYMBOL_VISIBLE
#define BOOST_SYMBOL_VISIBLE
#endif

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_NOWIDE_DYN_LINK)
#ifdef BOOST_NOWIDE_SOURCE
#define BOOST_NOWIDE_DECL BOOST_SYMBOL_EXPORT
#else
#define BOOST_NOWIDE_DECL BOOST_SYMBOL_IMPORT
#endif // BOOST_NOWIDE_SOURCE
#endif // DYN_LINK

#ifndef BOOST_NOWIDE_DECL
#define BOOST_NOWIDE_DECL
#endif

/// @def BOOST_NOWIDE_USE_WIN_FSTREAM
/// @brief Define to 1 to use internal classes from fstream.hpp
///
/// - On Non-Windows platforms: Define to 1 to use the same classes from header <fstream.hpp>
/// that are used on Windows.
/// - On Windows: No effect, always overwritten to 1
///
/// Affects boost::nowide::basic_filebuf,
/// boost::nowide::basic_ofstream, boost::nowide::basic_ifstream, boost::nowide::basic_fstream
#if defined(BOOST_WINDOWS)
#ifdef BOOST_NOWIDE_USE_WIN_FSTREAM
#undef BOOST_NOWIDE_USE_WIN_FSTREAM
#endif
#define BOOST_NOWIDE_USE_WIN_FSTREAM 1
#elif !defined(BOOST_NOWIDE_USE_WIN_FSTREAM)
#define BOOST_NOWIDE_USE_WIN_FSTREAM 0
#endif

#endif // boost/nowide/config.hpp
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
