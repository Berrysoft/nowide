//
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED
#define BOOST_NOWIDE_INTEGRATION_FILESYSTEM_HPP_INCLUDED

#include <boost/nowide/convert.hpp>
#include <filesystem>

namespace boost {
namespace nowide {
    namespace filesystem {
#ifndef BOOST_WINDOWS
        using namespace std::filesystem;
#else  // Windows
        namespace {
            using namespace std::filesystem;
        }

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
            path(std::string_view&& string) : path_base(widen(string))
            {}
            path(const std::filesystem::path& p) : path_base(p)
            {}
            path(std::filesystem::path&& p) : path_base(std::move(p))
            {}
            path(const path& p) : path_base(p)
            {}
            path(path&& p) noexcept : path_base(std::move(p))
            {}

            std::string string() const
            {
                return narrow(path_base::wstring());
            }
            std::string generic_string() const
            {
                return narrow(path_base::generic_wstring());
            }

            path& operator=(const std::filesystem::path& p)
            {
                path_base::operator=(p);
                return *this;
            }
            path& operator=(std::filesystem::path&& p)
            {
                path_base::operator=(std::move(p));
                return *this;
            }

            path& operator=(const path& p)
            {
                path_base::operator=(p);
                return *this;
            }
            path& operator=(path&& p) noexcept
            {
                path_base::operator=(std::move(p));
                return *this;
            }
        };
#endif // Windows
    }  // namespace filesystem
} // namespace nowide
} // namespace boost

#endif
