//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_CONVERT_HPP_INCLUDED
#define NOWIDE_CONVERT_HPP_INCLUDED

#include <nowide/utf/convert.hpp>
#include <string>
#include <string_view>

namespace nowide {
///
/// Convert string view to NULL terminated string
/// stored in \a output of size \a output_size (including NULL)
///
/// If there is not enough room NULL is returned, else output is returned.
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
template<typename CharOut, typename CharIn, typename TraitsIn = std::char_traits<CharIn>>
inline CharOut*
convert(CharOut* output, std::size_t output_size, std::basic_string_view<CharIn, TraitsIn> source) noexcept
{
    return utf::convert_buffer(output, output_size, source.data(), source.data() + source.length());
}

///
/// Convert wide string (UTF-16/32) in range [begin,end) to NULL terminated narrow string (UTF-8)
/// stored in \a output of size \a output_size (including NULL)
///
/// If there is not enough room NULL is returned, else output is returned.
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline char* narrow(char* output, size_t output_size, const wchar_t* begin, const wchar_t* end) noexcept
{
    return utf::convert_buffer(output, output_size, begin, end);
}
///
/// Convert NULL terminated wide string (UTF-16/32) to NULL terminated narrow string (UTF-8)
/// stored in \a output of size \a output_size (including NULL)
///
/// If there is not enough room NULL is returned, else output is returned.
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline char* narrow(char* output, size_t output_size, std::wstring_view source) noexcept
{
    return narrow(output, output_size, source.data(), source.data() + source.length());
}

///
/// Convert narrow string (UTF-8) in range [begin,end) to NULL terminated wide string (UTF-16/32)
/// stored in \a output of size \a output_size (including NULL)
///
/// If there is not enough room NULL is returned, else output is returned.
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline wchar_t* widen(wchar_t* output, size_t output_size, const char* begin, const char* end) noexcept
{
    return utf::convert_buffer(output, output_size, begin, end);
}
///
/// Convert NULL terminated narrow string (UTF-8) to NULL terminated wide string (UTF-16/32)
/// most output_size (including NULL)
///
/// If there is not enough room NULL is returned, else output is returned.
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline wchar_t* widen(wchar_t* output, size_t output_size, std::string_view source) noexcept
{
    return widen(output, output_size, source.data(), source.data() + source.length());
}

///
/// Convert string view to string.
///
/// \param s Input string
/// \param count Number of characters to convert
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
template<typename CharOut,
         typename CharIn,
         typename TraitsOut = std::char_traits<CharOut>,
         typename AllocOut = std::allocator<CharOut>,
         typename TraitsIn = std::char_traits<CharIn>>
inline std::basic_string<CharOut, TraitsOut, AllocOut> convert(std::basic_string_view<CharIn, TraitsIn> s,
                                                               const AllocOut& alloc = {})
{
    return utf::convert_string<CharOut, CharIn, TraitsOut, AllocOut>(s.data(), s.data() + s.length(), alloc);
}

///
/// Convert wide string (UTF-16/32) to narrow string (UTF-8).
///
/// \param s Input string
/// \param count Number of characters to convert
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline std::string narrow(const wchar_t* s, size_t count)
{
    return utf::convert_string<char>(s, s + count);
}
///
/// Convert wide string (UTF-16/32) to narrow string (UTF-8).
///
/// \param s Input string
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline std::string narrow(std::wstring_view s)
{
    return narrow(s.data(), s.size());
}

///
/// Convert narrow string (UTF-8) to wide string (UTF-16/32).
///
/// \param s Input string
/// \param count Number of characters to convert
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline std::wstring widen(const char* s, size_t count)
{
    return utf::convert_string<wchar_t>(s, s + count);
}
///
/// Convert narrow string (UTF-8) to wide string (UTF-16/32).
///
/// \param s Input string
/// Any illegal sequences are replaced with the replacement character, see #NOWIDE_REPLACEMENT_CHARACTER
///
inline std::wstring widen(std::string_view s)
{
    return widen(s.data(), s.size());
}
} // namespace nowide

#endif
