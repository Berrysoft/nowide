//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Alexander Grund
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NOWIDE_UTF_HPP_INCLUDED
#define NOWIDE_UTF_HPP_INCLUDED

#include <cstddef>
#include <nowide/config.hpp>

///
/// \brief Namespace that holds basic operations on UTF encoded sequences
///
/// All functions defined in this namespace do not require linking with Boost.Nowide library
/// Extracted from Boost.Locale
///
namespace nowide::utf {

///
/// \brief The integral type that can hold a Unicode code point
///
using code_point = char32_t;

///
/// \brief Special constant that defines illegal code point
///
static constexpr code_point illegal = 0xFFFFFFFFu;

///
/// \brief Special constant that defines incomplete code point
///
static constexpr code_point incomplete = 0xFFFFFFFEu;

///
/// \brief the function checks if \a v is a valid code point
///
constexpr bool is_valid_codepoint(code_point v) noexcept
{
    if(v > 0x10FFFF)
        return false;
    if(0xD800 <= v && v <= 0xDFFF) // surrogates
        return false;
    return true;
}

template<typename CharType, std::size_t size = sizeof(CharType)>
struct utf_traits;

template<typename CharType>
struct utf_traits<CharType, 1>
{
    using char_type = CharType;

    static constexpr int trail_length(char_type ci) noexcept
    {
        unsigned char c = ci;
        if(c < 128)
            return 0;
        if(NOWIDE_UNLIKELY(c < 194))
            return -1;
        if(c < 224)
            return 1;
        if(c < 240)
            return 2;
        if(NOWIDE_LIKELY(c <= 244))
            return 3;
        return -1;
    }

    static constexpr int max_width = 4;

    static constexpr int width(code_point value) noexcept
    {
        if(value <= 0x7F)
        {
            return 1;
        } else if(value <= 0x7FF)
        {
            return 2;
        } else if(NOWIDE_LIKELY(value <= 0xFFFF))
        {
            return 3;
        } else
        {
            return 4;
        }
    }

    static constexpr bool is_trail(char_type ci) noexcept
    {
        unsigned char c = ci;
        return (c & 0xC0) == 0x80;
    }

    static constexpr bool is_lead(char_type ci) noexcept
    {
        return !is_trail(ci);
    }

    template<typename Iterator>
    static constexpr code_point decode(Iterator& p, Iterator e) noexcept(noexcept(*p++))
    {
        if(NOWIDE_UNLIKELY(p == e))
            return incomplete;

        unsigned char lead = *p++;

        // First byte is fully validated here
        int trail_size = trail_length(lead);

        if(NOWIDE_UNLIKELY(trail_size < 0))
            return illegal;

        //
        // OK as only ASCII may be of size = 0
        // also optimize for ASCII text
        //
        if(trail_size == 0)
            return lead;

        code_point c = lead & ((1 << (6 - trail_size)) - 1);

        // Read the rest
        unsigned char tmp;
        switch(trail_size)
        {
        case 3:
            if(NOWIDE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if(!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
            [[fallthrough]];
        case 2:
            if(NOWIDE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if(!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
            [[fallthrough]];
        case 1:
            if(NOWIDE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if(!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
        }

        // Check code point validity: no surrogates and
        // valid range
        if(NOWIDE_UNLIKELY(!is_valid_codepoint(c)))
            return illegal;

        // make sure it is the most compact representation
        if(NOWIDE_UNLIKELY(width(c) != trail_size + 1))
            return illegal;

        return c;
    }

    template<typename Iterator>
    static constexpr code_point decode_valid(Iterator& p) noexcept(noexcept(*p++))
    {
        unsigned char lead = *p++;
        if(lead < 192)
            return lead;

        int trail_size;

        if(lead < 224)
            trail_size = 1;
        else if(NOWIDE_LIKELY(lead < 240)) // non-BMP rare
            trail_size = 2;
        else
            trail_size = 3;

        code_point c = lead & ((1 << (6 - trail_size)) - 1);

        switch(trail_size)
        {
        case 3: c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F); [[fallthrough]];
        case 2: c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F); [[fallthrough]];
        case 1: c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
        }

        return c;
    }

    template<typename Iterator>
    static Iterator encode(code_point value, Iterator out) noexcept(noexcept(*out++))
    {
        if(value <= 0x7F)
        {
            *out++ = static_cast<char_type>(value);
        } else if(value <= 0x7FF)
        {
            *out++ = static_cast<char_type>((value >> 6) | 0xC0);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        } else if(NOWIDE_LIKELY(value <= 0xFFFF))
        {
            *out++ = static_cast<char_type>((value >> 12) | 0xE0);
            *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        } else
        {
            *out++ = static_cast<char_type>((value >> 18) | 0xF0);
            *out++ = static_cast<char_type>(((value >> 12) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        }
        return out;
    }
}; // utf8

template<typename CharType>
struct utf_traits<CharType, 2>
{
    using char_type = CharType;

    // See RFC 2781
    static constexpr bool is_first_surrogate(char_type x) noexcept
    {
        return 0xD800 <= x && x <= 0xDBFF;
    }
    static constexpr bool is_second_surrogate(char_type x) noexcept
    {
        return 0xDC00 <= x && x <= 0xDFFF;
    }
    static constexpr code_point combine_surrogate(char_type w1, char_type w2) noexcept
    {
        return ((code_point(w1 & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
    }
    static constexpr int trail_length(char_type c) noexcept
    {
        if(is_first_surrogate(c))
            return 1;
        if(is_second_surrogate(c))
            return -1;
        return 0;
    }
    ///
    /// Returns true if c is trail code unit, always false for UTF-32
    ///
    static constexpr bool is_trail(char_type c) noexcept
    {
        return is_second_surrogate(c);
    }
    ///
    /// Returns true if c is lead code unit, always true of UTF-32
    ///
    static constexpr bool is_lead(char_type c) noexcept
    {
        return !is_second_surrogate(c);
    }

    template<typename It>
    static constexpr code_point decode(It& current, It last) noexcept(noexcept(*current++))
    {
        if(NOWIDE_UNLIKELY(current == last))
            return incomplete;
        char16_t w1 = *current++;
        if(NOWIDE_LIKELY(w1 < 0xD800 || 0xDFFF < w1))
        {
            return w1;
        }
        if(w1 > 0xDBFF)
            return illegal;
        if(current == last)
            return incomplete;
        char16_t w2 = *current++;
        if(w2 < 0xDC00 || 0xDFFF < w2)
            return illegal;
        return combine_surrogate(w1, w2);
    }
    template<typename It>
    static constexpr code_point decode_valid(It& current) noexcept(noexcept(*current++))
    {
        char16_t w1 = *current++;
        if(NOWIDE_LIKELY(w1 < 0xD800 || 0xDFFF < w1))
        {
            return w1;
        }
        char16_t w2 = *current++;
        return combine_surrogate(w1, w2);
    }

    static constexpr int max_width = 2;
    static constexpr int width(code_point u) noexcept
    {
        return u >= 0x10000 ? 2 : 1;
    }
    template<typename It>
    static It encode(code_point u, It out) noexcept(noexcept(*out++))
    {
        if(NOWIDE_LIKELY(u <= 0xFFFF))
        {
            *out++ = static_cast<char_type>(u);
        } else
        {
            u -= 0x10000;
            *out++ = static_cast<char_type>(0xD800 | (u >> 10));
            *out++ = static_cast<char_type>(0xDC00 | (u & 0x3FF));
        }
        return out;
    }
}; // utf16;

template<typename CharType>
struct utf_traits<CharType, 4>
{
    using char_type = CharType;
    static constexpr int trail_length(char_type c) noexcept
    {
        if(is_valid_codepoint(c))
            return 0;
        return -1;
    }
    static constexpr bool is_trail(char_type /*c*/) noexcept
    {
        return false;
    }
    static constexpr bool is_lead(char_type /*c*/) noexcept
    {
        return true;
    }

    template<typename It>
    static constexpr code_point decode_valid(It& current) noexcept(noexcept(*current++))
    {
        return *current++;
    }

    template<typename It>
    static constexpr code_point decode(It& current, It last) noexcept(noexcept(*current++))
    {
        if(NOWIDE_UNLIKELY(current == last))
            return incomplete;
        code_point c = *current++;
        if(NOWIDE_UNLIKELY(!is_valid_codepoint(c)))
            return illegal;
        return c;
    }
    static constexpr int max_width = 1;
    static constexpr int width(code_point /*u*/) noexcept
    {
        return 1;
    }
    template<typename It>
    static It encode(code_point u, It out) noexcept(noexcept(*out++))
    {
        *out++ = static_cast<char_type>(u);
        return out;
    }

}; // utf32

} // namespace nowide::utf

#endif
