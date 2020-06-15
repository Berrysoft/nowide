//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019-2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_FILEBUF_HPP_INCLUDED
#define NOWIDE_FILEBUF_HPP_INCLUDED

#include <nowide/config.hpp>
#ifdef NOWIDE_WINDOWS
#include <nowide/cstdio.hpp>
#include <nowide/filesystem.hpp>
#include <nowide/stackstring.hpp>
#endif
#include <fstream>

namespace nowide {
#ifndef NOWIDE_WINDOWS
using std::basic_filebuf;
using std::filebuf;
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

#endif // windows

} // namespace nowide

#endif
