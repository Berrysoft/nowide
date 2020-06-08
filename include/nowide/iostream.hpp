//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_IOSTREAM_HPP_INCLUDED
#define NOWIDE_IOSTREAM_HPP_INCLUDED

#include <nowide/config.hpp>
#ifdef NOWIDE_WINDOWS
#include <istream>
#include <nowide/ios.hpp>
#include <ostream>
#else
#include <iostream>
#endif

namespace nowide {
#ifndef NOWIDE_WINDOWS
using std::cin;
using std::cout;
using std::cerr;
using std::clog;
#else

/// \cond INTERNAL
namespace ios_base {
#ifndef NOWIDE_SOURCE
    static const Init __init{};
#endif // !NOWIDE_SOURCE
} // namespace ios_base

/// \endcond

#ifndef NOWIDE_SOURCE
///
/// \brief Same as std::cin, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern NOWIDE_DECL std::istream cin;
///
/// \brief Same as std::cout, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern NOWIDE_DECL std::ostream cout;
///
/// \brief Same as std::cerr, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern NOWIDE_DECL std::ostream cerr;
///
/// \brief Same as std::clog, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern NOWIDE_DECL std::ostream clog;
#endif // !NOWIDE_SOURCE
#endif
} // namespace nowide

#endif
