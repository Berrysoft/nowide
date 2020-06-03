//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_NOWIDE_SOURCE

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#elif(defined(__MINGW32__) || defined(__CYGWIN__)) && defined(__STRICT_ANSI__)
// Need the _w* functions which are extensions on MinGW/Cygwin
#undef __STRICT_ANSI__
#endif

#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/stackstring.hpp>

namespace boost {
namespace nowide {
    FILE* freopen(const char* file_name, const char* mode, FILE* stream)
    {
        const wstackstring wname(file_name);
        const wshort_stackstring wmode(mode);
        return _wfreopen(wname.data(), wmode.data(), stream);
    }

    FILE* fopen(const char* file_name, const char* mode)
    {
        const wstackstring wname(file_name);
        const wshort_stackstring wmode(mode);
        return _wfopen(wname.data(), wmode.data());
    }

    int rename(const char* old_name, const char* new_name)
    {
        const wstackstring wold(old_name), wnew(new_name);
        return _wrename(wold.data(), wnew.data());
    }

    int remove(const char* name)
    {
        const wstackstring wname(name);
        return _wremove(wname.data());
    }

    static char tmpnam_buffer[L_tmpnam];
    char* tmpnam(char* filename)
    {
        basic_stackstring<wchar_t, char, L_tmpnam> wfilename(filename);
        if(_wtmpnam(wfilename.data()))
        {
            return narrow(filename ? filename : tmpnam_buffer, L_tmpnam, wfilename);
        }
        return nullptr;
    }
} // namespace nowide
} // namespace boost
