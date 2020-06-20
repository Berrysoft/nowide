//
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at  http://www.boost.org/LICENSE_1_0.txt)
//

#define NOWIDE_SOURCE

#if(defined(__MINGW32__) || defined(__CYGWIN__)) && defined(__STRICT_ANSI__)
// Need the _w* functions which are extensions on MinGW/Cygwin
#undef __STRICT_ANSI__
#endif

#include <cerrno>
#include <nowide/stackstring.hpp>
#include <nowide/stat.hpp>

namespace nowide::detail {
int stat(const char* NOWIDE_RESTRICT path, stat_t* NOWIDE_RESTRICT buffer, std::size_t buffer_size)
{
    const wstackstring wpath(path);
    switch(buffer_size)
    {
#ifndef _WIN64
    case sizeof(struct _stat32i64): return _wstat32i64(wpath.data(), reinterpret_cast<struct _stat32i64*>(buffer));
#endif // !_WIN64
    case sizeof(struct _stat64): return _wstat64(wpath.data(), reinterpret_cast<struct _stat64*>(buffer));
    default: errno = EINVAL; return -1;
    }
}

int stat(const char* NOWIDE_RESTRICT path, posix_stat_t* NOWIDE_RESTRICT buffer, std::size_t buffer_size)
{
    const wstackstring wpath(path);
    switch(buffer_size)
    {
#ifndef _WIN64
    case sizeof(struct _stat32): return _wstat32(wpath.data(), reinterpret_cast<struct _stat32*>(buffer));
#endif // !_WIN64
    case sizeof(struct _stat64i32): return _wstat64i32(wpath.data(), reinterpret_cast<struct _stat64i32*>(buffer));
    default: errno = EINVAL; return -1;
    }
}
} // namespace nowide::detail
