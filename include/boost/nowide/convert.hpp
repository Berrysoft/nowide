//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_CONVERT_HPP_INCLUDED
#define BOOST_NOWIDE_CONVERT_HPP_INCLUDED

#include <boost/nowide/detail/convert.hpp>
#include <string>

namespace boost {
namespace nowide {

    ///
    /// Convert wide string (UTF-16/32) in range [begin,end) to NULL terminated narrow string (UTF-8)
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    inline char* narrow(char* output, size_t output_size, const wchar_t* begin, const wchar_t* end)
    {
        return detail::convert_buffer(output, output_size, begin, end);
    }
    ///
    /// Convert NULL terminated wide string (UTF-16/32) to NULL terminated narrow string (UTF-8)
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    inline char* narrow(char* output, size_t output_size, const wchar_t* source)
    {
        return narrow(output, output_size, source, source + detail::strlen(source));
    }

    ///
    /// Convert narrow string (UTF-8) in range [begin,end) to NULL terminated wide string (UTF-16/32)
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    inline wchar_t* widen(wchar_t* output, size_t output_size, const char* begin, const char* end)
    {
        return detail::convert_buffer(output, output_size, begin, end);
    }
    ///
    /// Convert NULL terminated narrow string (UTF-8) to NULL terminated wide string (UTF-16/32)
    /// most output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    inline wchar_t* widen(wchar_t* output, size_t output_size, const char* source)
    {
        return widen(output, output_size, source, source + detail::strlen(source));
    }

    ///
    /// Convert wide string (UTF-16/32) to narrow string (UTF-8).
    ///
    /// \param s Input string view
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char = char,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char>,
             typename CharIn = wchar_t,
             typename TraitsIn = std::char_traits<CharIn> >
    inline std::basic_string<Char, Traits, Alloc> narrow(std::basic_string_view<CharIn, TraitsIn> s,
                                                         Alloc const& alloc = {})
    {
        return detail::convert_string<Char>(s.data(), s.data() + s.size(), alloc);
    }

    ///
    /// Convert narrow string (UTF-8) to wide string (UTF-16/32).
    ///
    /// \param s Input string view
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char = wchar_t,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char>,
             typename CharIn = char,
             typename TraitsIn = std::char_traits<CharIn> >
    inline std::basic_string<Char, Traits, Alloc> widen(std::basic_string_view<CharIn, TraitsIn> s,
                                                        Alloc const& alloc = {})
    {
        return detail::convert_string<Char>(s.data(), s.data() + s.size(), alloc);
    }
} // namespace nowide
} // namespace boost

#endif
