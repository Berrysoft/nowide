//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019-2020 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_FILEBUF_HPP_INCLUDED
#define BOOST_NOWIDE_FILEBUF_HPP_INCLUDED

#include <boost/nowide/config.hpp>
#if BOOST_NOWIDE_USE_FILEBUF_REPLACEMENT
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/stackstring.hpp>
#include <filesystem>
#endif
#include <fstream>

namespace boost {
namespace nowide {
#if !BOOST_NOWIDE_USE_FILEBUF_REPLACEMENT && !defined(BOOST_NOWIDE_DOXYGEN)
    using std::basic_filebuf;
    using std::filebuf;
    using std::wfilebuf;
#else // Windows
    ///
    /// \brief This forward declaration defines the basic_filebuf type.
    ///
    /// it is implemented and specialized for CharType = char, it
    /// implements std::filebuf over standard C I/O
    ///
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_filebuf : public std::basic_filebuf<CharType, Traits>
    {
    public:
        using std::basic_filebuf<CharType, Traits>::basic_filebuf;

        std::basic_filebuf<CharType, Traits>* open(const char* s, std::ios_base::openmode mode)
        {
            const wstackstring name(s);
            return std::basic_filebuf<CharType, Traits>::open(name.get(), mode);
        }

#if BOOST_NOWIDE_USE_WCHAR_OVERLOADS
        std::basic_filebuf<CharType, Traits>* open(const wchar_t* s, std::ios_base::openmode mode)
        {
            return std::basic_filebuf<CharType, Traits>::open(s, mode);
        }
#endif

        std::basic_filebuf<CharType, Traits>* open(const std::string& str, std::ios_base::openmode mode)
        {
            return this->open(str.c_str(), mode);
        }

        std::basic_filebuf<CharType, Traits>* open(const std::filesystem::path& p, std::ios_base::openmode mode)
        {
            return std::basic_filebuf<CharType, Traits>::open(p, mode);
        }
    };

    ///
    /// \brief Convenience typedef
    ///
    typedef basic_filebuf<char> filebuf;

    typedef basic_filebuf<wchar_t> wfilebuf;

#endif // windows

} // namespace nowide
} // namespace boost

#endif
