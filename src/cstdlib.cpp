//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#define NOWIDE_SOURCE

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#elif(defined(__MINGW32__) || defined(__CYGWIN__)) && defined(__STRICT_ANSI__)
// Need the _w* functions which are extensions on MinGW/Cygwin
#undef __STRICT_ANSI__
#endif

#include <Windows.h>
#include <cerrno>
#include <nowide/cstdlib.hpp>
#include <nowide/stackstring.hpp>
#include <string>

namespace nowide {
char* getenv(const char* key)
{
    static stackstring value;
    const wshort_stackstring name(key);

    // An environment variable has a maximum size limit of 32767 characters
    constexpr size_t buf_size = 32767;
    wchar_t buf[buf_size];
    if(!GetEnvironmentVariableW(name.data(), buf, buf_size) && GetLastError() == ERROR_ENVVAR_NOT_FOUND)
        return nullptr;
    value.convert(buf);
    return value.data();
}

int setenv(const char* key, const char* value, int overwrite)
{
    const wshort_stackstring name(key);
    if(!overwrite)
    {
        wchar_t unused[2];
        if(GetEnvironmentVariableW(name.data(), unused, 2) || GetLastError() != ERROR_ENVVAR_NOT_FOUND)
            return 0;
    }
    const wstackstring wval(value);
    if(SetEnvironmentVariableW(name.data(), wval.data()))
        return 0;
    switch(GetLastError())
    {
    case ERROR_OUTOFMEMORY: errno = ENOMEM; break;
    default: errno = EINVAL; break;
    }
    return -1;
}

int unsetenv(const char* key)
{
    const wshort_stackstring name(key);
    if(SetEnvironmentVariableW(name.data(), nullptr))
        return 0;
    errno = EINVAL;
    return -1;
}

int putenv(char* string)
{
    const char* key = string;
    const char* key_end = string;
    while(*key_end != '=' && *key_end != '\0')
        key_end++;
    if(*key_end == '\0')
    {
        errno = EINVAL;
        return -1;
    }
    const wshort_stackstring wkey(key, key_end);
    const wstackstring wvalue(key_end + 1);

    if(SetEnvironmentVariableW(wkey.data(), wvalue.data()))
        return 0;
    errno = ENOMEM;
    return -1;
}

int system(const char* cmd)
{
    if(!cmd)
        return _wsystem(0);
    const wstackstring wcmd(cmd);
    return _wsystem(wcmd.data());
}
} // namespace nowide
