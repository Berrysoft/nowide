//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019-2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_FILEBUF_HPP_INCLUDED
#define BOOST_NOWIDE_FILEBUF_HPP_INCLUDED

#include <boost/nowide/config.hpp>
#ifdef BOOST_WINDOWS
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/stackstring.hpp>
#endif
#include <fstream>

namespace boost {
namespace nowide {
#ifndef BOOST_WINDOWS
    using std::basic_filebuf;
    using std::filebuf;
    using std::wfilebuf;
#else // Windows
    ///
    /// \brief The basic_filebuf type.
    ///
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_filebuf : public std::basic_filebuf<CharType, Traits>
    {
    public:
        using std::basic_filebuf<CharType, Traits>::basic_filebuf;

        std::basic_filebuf<CharType, Traits>* open(const char* s, std::ios_base::openmode mode)
        {
            const wstackstring name(s);
            return std::basic_filebuf<CharType, Traits>::open(name.data(), mode);
        }

        std::basic_filebuf<CharType, Traits>* open(const wchar_t* s, std::ios_base::openmode mode)
        {
            return std::basic_filebuf<CharType, Traits>::open(s, mode);
        }

        std::basic_filebuf<CharType, Traits>* open(const std::string& str, std::ios_base::openmode mode)
        {
            return this->open(str.c_str(), mode);
        }

        std::basic_filebuf<CharType, Traits>* open(const filesystem::path& p, std::ios_base::openmode mode)
        {
            return std::basic_filebuf<CharType, Traits>::open(p, mode);
        }
    };

    ///
    /// \brief Convenience typedef
    ///
    using filebuf = basic_filebuf<char>;
    using wfilebuf = basic_filebuf<wchar_t>;

#endif // windows

} // namespace nowide
} // namespace boost

#endif
