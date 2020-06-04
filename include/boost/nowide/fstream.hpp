//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_FSTREAM_HPP_INCLUDED
#define BOOST_NOWIDE_FSTREAM_HPP_INCLUDED

#include <boost/nowide/config.hpp>
#include <boost/nowide/filebuf.hpp>
#include <istream>
#include <ostream>

namespace boost {
namespace nowide {
#ifndef BOOST_WINDOWS
    using std::basic_ifstream;
    using std::basic_ofstream;
    using std::basic_fstream;
    using std::ifstream;
    using std::ofstream;
    using std::fstream;
    using std::wifstream;
    using std::wofstream;
    using std::wfstream;
#else
    /// \cond INTERNAL
    namespace detail {
        struct StreamTypeIn
        {
            static constexpr std::ios_base::openmode mode = std::ios_base::in;
            static constexpr std::ios_base::openmode mode_modifier = mode;
            template<typename CharType, typename Traits>
            struct stream_base
            {
                using type = std::basic_istream<CharType, Traits>;
            };
        };
        struct StreamTypeOut
        {
            static constexpr std::ios_base::openmode mode = std::ios_base::out;
            static constexpr std::ios_base::openmode mode_modifier = mode;
            template<typename CharType, typename Traits>
            struct stream_base
            {
                using type = std::basic_ostream<CharType, Traits>;
            };
        };
        struct StreamTypeInOut
        {
            static constexpr std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out;
            static constexpr std::ios_base::openmode mode_modifier = {};
            template<typename CharType, typename Traits>
            struct stream_base
            {
                using type = std::basic_iostream<CharType, Traits>;
            };
        };

        /// Base class for all basic_*fstream classes
        /// Contains basic_filebuf instance so its pointer can be used to construct basic_*stream
        /// Provides common functions to reduce boilerplate code including inheriting from
        /// the correct std::basic_[io]stream class and initializing it
        /// \tparam T_StreamType One of StreamType* above.
        template<typename CharType, typename Traits, typename T_StreamType>
        class fstream_impl;
    } // namespace detail
    /// \endcond

    ///
    /// \brief Same as std::basic_ifstream<char> but accepts UTF-8 strings under Windows
    ///
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_ifstream : public detail::fstream_impl<CharType, Traits, detail::StreamTypeIn>
    {
        using fstream_impl = detail::fstream_impl<CharType, Traits, detail::StreamTypeIn>;

    public:
        using fstream_impl::fstream_impl;

        basic_ifstream(const basic_ifstream&) = delete;
        basic_ifstream& operator=(const basic_ifstream&) = delete;
        basic_ifstream(basic_ifstream&& other) noexcept : fstream_impl(std::move(other))
        {}
        basic_ifstream& operator=(basic_ifstream&& rhs) noexcept
        {
            fstream_impl::operator=(std::move(rhs));
            return *this;
        }
    };

    ///
    /// \brief Same as std::basic_ofstream<char> but accepts UTF-8 strings under Windows
    ///

    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_ofstream : public detail::fstream_impl<CharType, Traits, detail::StreamTypeOut>
    {
        using fstream_impl = detail::fstream_impl<CharType, Traits, detail::StreamTypeOut>;

    public:
        using fstream_impl::fstream_impl;

        basic_ofstream(const basic_ofstream&) = delete;
        basic_ofstream& operator=(const basic_ofstream&) = delete;
        basic_ofstream(basic_ofstream&& other) noexcept : fstream_impl(std::move(other))
        {}
        basic_ofstream& operator=(basic_ofstream&& rhs)
        {
            fstream_impl::operator=(std::move(rhs));
            return *this;
        }
    };

    ///
    /// \brief Same as std::basic_fstream<char> but accepts UTF-8 strings under Windows
    ///
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_fstream : public detail::fstream_impl<CharType, Traits, detail::StreamTypeInOut>
    {
        using fstream_impl = detail::fstream_impl<CharType, Traits, detail::StreamTypeInOut>;

    public:
        using fstream_impl::fstream_impl;

        basic_fstream(const basic_fstream&) = delete;
        basic_fstream& operator=(const basic_fstream&) = delete;
        basic_fstream(basic_fstream&& other) noexcept : fstream_impl(std::move(other))
        {}
        basic_fstream& operator=(basic_fstream&& rhs)
        {
            fstream_impl::operator=(std::move(rhs));
            return *this;
        }
    };

    ///
    /// Same as std::ifstream but accepts UTF-8 strings under Windows
    /// and *\::filesystem::path on all systems
    ///
    using ifstream = basic_ifstream<char>;
    using wifstream = basic_ifstream<wchar_t>;
    ///
    /// Same as std::ofstream but accepts UTF-8 strings under Windows
    /// and *\::filesystem::path on all systems
    ///
    using ofstream = basic_ofstream<char>;
    using wofstream = basic_ofstream<wchar_t>;
    ///
    /// Same as std::fstream but accepts UTF-8 strings under Windows
    /// and *\::filesystem::path on all systems
    ///
    using fstream = basic_fstream<char>;
    using wfstream = basic_fstream<wchar_t>;

    // Implementation
    namespace detail {
        /// Holds an instance of T
        /// Required to make sure this is constructed first before passing it to sibling classes
        template<typename T>
        struct buf_holder
        {
            T buf_;
        };
        template<typename CharType, typename Traits, typename T_StreamType>
        class fstream_impl : private buf_holder<basic_filebuf<CharType, Traits>>, // must be first due to init order
                             public T_StreamType::template stream_base<CharType, Traits>::type
        {
            using internal_buffer_type = basic_filebuf<CharType, Traits>;
            using base_buf_holder = buf_holder<internal_buffer_type>;
            using stream_base = typename T_StreamType::template stream_base<CharType, Traits>::type;

        public:
            using stream_base::setstate;
            using stream_base::clear;

            using base_buf_holder::buf_;

            fstream_impl() : stream_base(&buf_)
            {}
            explicit fstream_impl(const char* file_name, std::ios_base::openmode mode = T_StreamType::mode) :
                fstream_impl()
            {
                open(file_name, mode);
            }
            explicit fstream_impl(const wchar_t* file_name, std::ios_base::openmode mode = T_StreamType::mode) :
                fstream_impl()
            {
                open(file_name, mode);
            }
            explicit fstream_impl(const std::string& file_name, std::ios_base::openmode mode = T_StreamType::mode) :
                fstream_impl()
            {
                open(file_name, mode);
            }
            explicit fstream_impl(const filesystem::path& file_name,
                                  std::ios_base::openmode mode = T_StreamType::mode) :
                fstream_impl()
            {
                open(file_name, mode);
            }
            void swap(fstream_impl& other)
            {
                stream_base::swap(other);
                rdbuf()->swap(*other.rdbuf());
            }

            void open(const std::string& file_name, std::ios_base::openmode mode = T_StreamType::mode)
            {
                open(file_name.c_str(), mode);
            }
            void open(const filesystem::path& file_name, std::ios_base::openmode mode = T_StreamType::mode)
            {
                open(file_name.c_str(), mode);
            }
            void open(const char* file_name, std::ios_base::openmode mode = T_StreamType::mode)
            {
                if(!rdbuf()->open(file_name, mode | T_StreamType::mode_modifier))
                    setstate(std::ios_base::failbit);
                else
                    clear();
            }
            void open(const wchar_t* file_name, std::ios_base::openmode mode = T_StreamType::mode)
            {
                if(!rdbuf()->open(file_name, mode | T_StreamType::mode_modifier))
                    setstate(std::ios_base::failbit);
                else
                    clear();
            }
            bool is_open() const
            {
                return rdbuf()->is_open();
            }
            void close()
            {
                if(!rdbuf()->close())
                    setstate(std::ios_base::failbit);
            }

            internal_buffer_type* rdbuf() const
            {
                return const_cast<internal_buffer_type*>(&buf_);
            }

        protected:
            fstream_impl(const fstream_impl&) = delete;
            fstream_impl& operator=(const fstream_impl&) = delete;

            // coverity[exn_spec_violation]
            fstream_impl(fstream_impl&& other) noexcept :
                base_buf_holder(std::move(other)), stream_base(std::move(other))
            {
                this->set_rdbuf(rdbuf());
            }
            fstream_impl& operator=(fstream_impl&& rhs) noexcept
            {
                base_buf_holder::operator=(std::move(rhs));
                stream_base::operator=(std::move(rhs));
                return *this;
            }
        };
    } // namespace detail

#endif // windows

} // namespace nowide
} // namespace boost

#endif
