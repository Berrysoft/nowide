//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED
#define BOOST_NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED

#include <boost/filesystem/path.hpp>
#include <boost/nowide/utf8_codecvt.hpp>
namespace boost {
namespace nowide {
    ///
    /// \brief Install utf8_codecvt facet into boost::filesystem::path so all char strings are interpreted as utf-8 strings
    ///
    /// Returns the old locale of boost::filesystem::path
    ///
    inline std::locale nowide_filesystem()
    {
        std::locale tmp = std::locale(std::locale(), new boost::nowide::utf8_codecvt<wchar_t>());
        return boost::filesystem::path::imbue(tmp);
    }
} // namespace nowide
} // namespace boost

#endif
///
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
