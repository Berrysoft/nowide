//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_IOSTREAM_HPP_INCLUDED
#define BOOST_NOWIDE_IOSTREAM_HPP_INCLUDED

#include <boost/nowide/config.hpp>
#ifdef BOOST_WINDOWS
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

namespace boost::nowide {
#ifndef BOOST_WINDOWS
using std::cout;
using std::cerr;
using std::cin;
using std::clog;
#else

/// \cond INTERNAL
namespace ios {
    /// \brief Class to initialize UTF-8 iostreams
    struct Init
    {
        BOOST_NOWIDE_DECL Init();
        BOOST_NOWIDE_DECL ~Init();
    };

#ifndef BOOST_NOWIDE_SOURCE
    static const Init __init{};
#endif // !BOOST_NOWIDE_SOURCE
} // namespace ios

/// \endcond

#ifndef BOOST_NOWIDE_SOURCE
///
/// \brief Same as std::cin, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern BOOST_NOWIDE_DECL std::istream cin;
///
/// \brief Same as std::cout, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern BOOST_NOWIDE_DECL std::ostream cout;
///
/// \brief Same as std::cerr, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern BOOST_NOWIDE_DECL std::ostream cerr;
///
/// \brief Same as std::clog, but uses UTF-8
///
/// Note, the stream is not synchronized with stdio and not affected by std::ios::sync_with_stdio
///
extern BOOST_NOWIDE_DECL std::ostream clog;
#endif // !BOOST_NOWIDE_SOURCE
#endif

} // namespace boost::nowide

#endif
