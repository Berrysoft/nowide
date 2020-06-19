//
//  Copyright (c) 2020 Alexander Grund
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
// MinGW64 with msvcrt could not find _*stat32*
#if defined(__MINGW64__) && !defined(_UCRT)
static int _wstat32i64(const wchar_t* path, struct _stat32i64* buffer)
{
    struct _stat64 s;
    int res = _wstat64(path, &s);
    if(res != 0)
        return res;
    buffer->st_dev = s.st_dev;
    buffer->st_ino = s.st_ino;
    buffer->st_mode = s.st_mode;
    buffer->st_nlink = s.st_nlink;
    buffer->st_uid = s.st_uid;
    buffer->st_gid = s.st_gid;
    buffer->st_rdev = s.st_rdev;
    buffer->st_size = s.st_size;
    buffer->st_atime = static_cast<__time32_t>(s.st_atime);
    buffer->st_mtime = static_cast<__time32_t>(s.st_mtime);
    buffer->st_ctime = static_cast<__time32_t>(s.st_ctime);
    return 0;
}

static int _wstat32(const wchar_t* path, struct _stat32* buffer)
{
    struct _stat64i32 s;
    int res = _wstat64i32(path, &s);
    if(res != 0)
        return res;
    buffer->st_dev = s.st_dev;
    buffer->st_ino = s.st_ino;
    buffer->st_mode = s.st_mode;
    buffer->st_nlink = s.st_nlink;
    buffer->st_uid = s.st_uid;
    buffer->st_gid = s.st_gid;
    buffer->st_rdev = s.st_rdev;
    buffer->st_size = s.st_size;
    buffer->st_atime = static_cast<__time32_t>(s.st_atime);
    buffer->st_mtime = static_cast<__time32_t>(s.st_mtime);
    buffer->st_ctime = static_cast<__time32_t>(s.st_ctime);
    return 0;
}
#else
using ::_wstat32i64;
using ::_wstat32;
#endif

int stat(const char* path, stat_t* buffer, std::size_t buffer_size)
{
    const wstackstring wpath(path);
    switch(buffer_size)
    {
    case sizeof(struct _stat32i64):
        return nowide::detail::_wstat32i64(wpath.data(), reinterpret_cast<struct _stat32i64*>(buffer));
    case sizeof(struct _stat64): return _wstat64(wpath.data(), reinterpret_cast<struct _stat64*>(buffer));
    default: errno = EINVAL; return -1;
    }
}

int stat(const char* path, posix_stat_t* buffer, std::size_t buffer_size)
{
    const wstackstring wpath(path);
    switch(buffer_size)
    {
    case sizeof(struct _stat32):
        return nowide::detail::_wstat32(wpath.data(), reinterpret_cast<struct _stat32*>(buffer));
    case sizeof(struct _stat64i32): return _wstat64i32(wpath.data(), reinterpret_cast<struct _stat64i32*>(buffer));
    default: errno = EINVAL; return -1;
    }
}
} // namespace nowide::detail
