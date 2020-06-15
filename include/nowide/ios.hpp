//
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_IOS_HPP_INCLUDED
#define NOWIDE_IOS_HPP_INCLUDED

#include <nowide/config.hpp>
#ifndef NOWIDE_WINDOWS
#include <ios>
#endif // !NOWIDE_WINDOWS

namespace nowide::ios_base {
#ifndef NOWIDE_WINDOWS
using Init = typename std::ios_base::Init;
#else
/// \brief Class to initialize UTF-8 iostreams
struct Init
{
    NOWIDE_DECL Init();
    NOWIDE_DECL ~Init();
};
#endif // !NOWIDE_WINDOWS
} // namespace nowide::ios_base

#endif // !NOWIDE_IOS_HPP_INCLUDED
