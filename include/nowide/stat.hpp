//
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_STAT_HPP_INCLUDED
#define NOWIDE_STAT_HPP_INCLUDED

#include <cstddef>
#include <nowide/config.hpp>
#include <sys/types.h>
// Include after sys/types.h
#include <sys/stat.h>

namespace nowide {
#ifndef NOWIDE_WINDOWS
using stat_t = struct stat;
using posix_stat_t = struct stat;

using ::stat;
#else
/// \brief Typedef for the file info structure.
/// Able to hold 64 bit filesize and timestamps on Windows and usually also on other 64 Bit systems
/// This allows to write portable code with option LFS support
typedef struct ::_stati64 stat_t;
/// \brief Typedef for the file info structure used in the POSIX stat call
/// Resolves to `struct _stat` on Windows and `struct stat` otherwise
/// This allows to write portable code using the default stat function
typedef struct ::_stat posix_stat_t;

/// \cond INTERNAL
namespace detail {
    NOWIDE_DECL int stat(const char* NOWIDE_RESTRICT path, stat_t* NOWIDE_RESTRICT buffer, std::size_t buffer_size);
    NOWIDE_DECL int
    stat(const char* NOWIDE_RESTRICT path, posix_stat_t* NOWIDE_RESTRICT buffer, std::size_t buffer_size);
} // namespace detail
/// \endcond

///
/// \brief UTF-8 aware stat function, returns 0 on success
///
/// Return information about a file from an UTF-8 encoded path
///
inline int stat(const char* NOWIDE_RESTRICT path, stat_t* NOWIDE_RESTRICT buffer)
{
    return detail::stat(path, buffer, sizeof(*buffer));
}
///
/// \brief UTF-8 aware stat function, returns 0 on success
///
/// Return information about a file from an UTF-8 encoded path
///
inline int stat(const char* NOWIDE_RESTRICT path, posix_stat_t* NOWIDE_RESTRICT buffer)
{
    return detail::stat(path, buffer, sizeof(*buffer));
}
#endif
} // namespace nowide

#endif
