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
    /// Convert string view to NULL terminated string
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char, typename CharIn, typename TraitsIn = std::char_traits<CharIn> >
    inline Char* convert(Char* output, std::size_t output_size, std::basic_string_view<CharIn, TraitsIn> source)
    {
        return detail::convert_buffer(output, output_size, source.data(), source.data() + source.size());
    }

    /// Convert string to NULL terminated string
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char, typename CharIn>
    inline Char* convert(Char* output, std::size_t output_size, const CharIn* source)
    {
        return convert<Char, CharIn>(output, output_size, std::basic_string_view<CharIn>{source});
    }

    /// Convert string to NULL terminated string
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char, typename CharIn>
    inline Char* convert(Char* output, std::size_t output_size, const CharIn* source, std::size_t source_size)
    {
        return convert<Char, CharIn>(output, output_size, std::basic_string_view<CharIn>{source, source_size});
    }

    /// Convert string to NULL terminated string
    /// stored in \a output of size \a output_size (including NULL)
    ///
    /// If there is not enough room NULL is returned, else output is returned.
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char,
             typename CharIn,
             typename TraitsIn = std::char_traits<CharIn>,
             typename AllocIn = std::allocator<CharIn> >
    inline Char*
    convert(Char* output, std::size_t output_size, const std::basic_string<CharIn, TraitsIn, AllocIn>& source)
    {
        return convert<Char, CharIn, TraitsIn>(output, output_size, std::basic_string_view<CharIn, TraitsIn>{source});
    }

    ///
    /// Convert string.
    ///
    /// \param s Input string view
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char,
             typename CharIn,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char>,
             typename TraitsIn = std::char_traits<CharIn> >
    inline std::basic_string<Char, Traits, Alloc> convert(std::basic_string_view<CharIn, TraitsIn> s,
                                                          Alloc const& alloc = {})
    {
        return detail::convert_string<Char>(s.data(), s.data() + s.size(), alloc);
    }

    ///
    /// Convert string.
    ///
    /// \param s Input string
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char,
             typename CharIn,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char> >
    inline std::basic_string<Char, Traits, Alloc> convert(const CharIn* s, Alloc const& alloc = {})
    {
        return convert<Char, CharIn, Traits, Alloc>(std::basic_string_view<CharIn>{s}, alloc);
    }

    ///
    /// Convert string.
    ///
    /// \param s Input string
    /// \param size Intput string size
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char,
             typename CharIn,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char> >
    inline std::basic_string<Char, Traits, Alloc> convert(const CharIn* s, std::size_t size, Alloc const& alloc = {})
    {
        return convert<Char, CharIn, Traits, Alloc>(std::basic_string_view<CharIn>{s, size}, alloc);
    }

    ///
    /// Convert string.
    ///
    /// \param s Input string
    /// \param alloc Output string allocator
    /// Any illegal sequences are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename Char,
             typename CharIn,
             typename Traits = std::char_traits<Char>,
             typename Alloc = std::allocator<Char>,
             typename TraitsIn = std::char_traits<CharIn>,
             typename AllocIn = std::allocator<CharIn> >
    inline std::basic_string<Char, Traits, Alloc> convert(const std::basic_string<CharIn, TraitsIn, AllocIn>& s,
                                                          Alloc const& alloc = {})
    {
        return convert<Char, CharIn, Traits, Alloc, TraitsIn>(std::basic_string_view<CharIn, TraitsIn>{s}, alloc);
    }
} // namespace nowide
} // namespace boost

#endif
