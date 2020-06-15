//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_CSTDIO_HPP_INCLUDED
#define NOWIDE_CSTDIO_HPP_INCLUDED

#include <cstdio>
#include <nowide/config.hpp>

namespace nowide {
#ifndef NOWIDE_WINDOWS
using std::fopen;
using std::freopen;
using std::remove;
using std::rename;
using std::tmpnam;
#else

///
/// \brief Same as freopen but file_name and mode are UTF-8 strings
///
NOWIDE_DECL FILE* freopen(const char* file_name, const char* mode, FILE* stream);
///
/// \brief Same as fopen but file_name and mode are UTF-8 strings
///
NOWIDE_DECL FILE* fopen(const char* file_name, const char* mode);
///
/// \brief Same as rename but old_name and new_name are UTF-8 strings
///
NOWIDE_DECL int rename(const char* old_name, const char* new_name);
///
/// \brief Same as rename but name is UTF-8 string
///
NOWIDE_DECL int remove(const char* name);
#endif
} // namespace nowide

#endif