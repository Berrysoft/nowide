//
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED
#define NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED

#include <filesystem>
#include <nowide/convert.hpp>

///
/// \brief This namespace includes partial implementation of <filesystem> with UTF-8 awareness
///
namespace nowide::filesystem {
#ifndef NOWIDE_WINDOWS
using std::filesystem::path;
#else  // Windows
///
/// \brief Same as std::filesystem::path but accepts UTF-8 strings under Windows
///
class path : public std::filesystem::path
{
private:
    using path_base = std::filesystem::path;

public:
    path()
    {}
    path(const char* string) : path_base(widen(string))
    {}
    path(const std::string& string) : path_base(widen(string))
    {}
    path(const std::string_view& string) : path_base(widen(string))
    {}
    path(const path_base& p) : path_base(p)
    {}
    path(path_base&& p) : path_base(std::move(p))
    {}
    path(const path& p) : path_base(static_cast<const path_base&>(p))
    {}
    path(path&& p) noexcept : path_base(static_cast<path_base&&>(std::move(p)))
    {}

    std::string string() const
    {
        return narrow(path_base::wstring());
    }
    std::string generic_string() const
    {
        return narrow(path_base::generic_wstring());
    }

    path& operator=(const char* string)
    {
        path_base::operator=(widen(string));
        return *this;
    }
    path& operator=(const std::string& string)
    {
        path_base::operator=(widen(string));
        return *this;
    }
    path& operator=(const std::string_view& string)
    {
        path_base::operator=(widen(string));
        return *this;
    }

    path& operator=(const path_base& p)
    {
        path_base::operator=(p);
        return *this;
    }
    path& operator=(path_base&& p)
    {
        path_base::operator=(std::move(p));
        return *this;
    }

    path& operator=(const path& p)
    {
        path_base::operator=(static_cast<const path_base&>(p));
        return *this;
    }
    path& operator=(path&& p) noexcept
    {
        path_base::operator=(static_cast<path_base&&>(std::move(p)));
        return *this;
    }
};
#endif // Windows
} // namespace nowide::filesystem

#endif
