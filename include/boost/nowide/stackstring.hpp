//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_DETAILS_WIDESTR_H_INCLUDED
#define BOOST_NOWIDE_DETAILS_WIDESTR_H_INCLUDED

#include <boost/nowide/convert.hpp>
#include <boost/static_assert.hpp>
#include <cstring>
#include <algorithm>
#include <cassert>

namespace boost {
namespace nowide {

    ///
    /// \brief A class that allows to create a temporary wide or narrow UTF strings from
    /// wide or narrow UTF source.
    ///
    /// It uses a stack buffer if the string is short enough
    /// otherwise allocates a buffer on the heap.
    ///
    /// If invalid UTF characters are detected they are replaced with the replacement character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    template<typename CharOut = wchar_t, typename CharIn = char, size_t BufferSize = 256>
    class basic_stackstring
    {
    public:
        static const size_t buffer_size = BufferSize;
        typedef CharOut output_char;
        typedef CharIn input_char;

        basic_stackstring(basic_stackstring const &other) : mem_buffer_(0)
        {
            if(other.mem_buffer_)
            {
                size_t len = 0;
                while(other.mem_buffer_[len])
                    len++;
                mem_buffer_ = new output_char[len + 1];
                memcpy(mem_buffer_, other.mem_buffer_, sizeof(output_char) * (len + 1));
            } else
            {
                memcpy(buffer_, other.buffer_, buffer_size * sizeof(output_char));
            }
        }

        friend void swap(basic_stackstring &lhs, basic_stackstring &rhs)
        {
            std::swap(lhs.mem_buffer_, rhs.mem_buffer_);
            for(size_t i = 0; i < buffer_size; i++)
                std::swap(lhs.buffer_[i], rhs.buffer_[i]);
        }
        basic_stackstring &operator=(basic_stackstring const &other)
        {
            if(this != &other)
            {
                clear();
                if(other.mem_buffer_)
                {
                    size_t len = 0;
                    while(other.mem_buffer_[len])
                        len++;
                    mem_buffer_ = new output_char[len + 1];
                    memcpy(mem_buffer_, other.mem_buffer_, sizeof(output_char) * (len + 1));
                } else
                {
                    memcpy(buffer_, other.buffer_, buffer_size * sizeof(output_char));
                }
            }
            return *this;
        }

        basic_stackstring() : mem_buffer_(0)
        {
            buffer_[0] = 0;
        }
        explicit basic_stackstring(input_char const *input) : mem_buffer_(0)
        {
            convert(input);
        }
        basic_stackstring(input_char const *begin, input_char const *end) : mem_buffer_(0)
        {
            convert(begin, end);
        }
        output_char *convert(input_char const *input)
        {
            return convert(input, details::basic_strend(input));
        }
        output_char *convert(input_char const *begin, input_char const *end)
        {
            clear();

            size_t space = get_space(sizeof(input_char), sizeof(output_char), end - begin) + 1;
            if(space <= buffer_size)
            {
                output_char *res = basic_convert(buffer_, buffer_size, begin, end);
                assert(res);
                (void)res;
            } else
            {
                mem_buffer_ = new output_char[space];
                output_char *res = basic_convert(mem_buffer_, space, begin, end);
                assert(res);
                (void)res;
            }
            return c_str();
        }
        output_char *c_str()
        {
            if(mem_buffer_)
                return mem_buffer_;
            return buffer_;
        }
        output_char const *c_str() const
        {
            if(mem_buffer_)
                return mem_buffer_;
            return buffer_;
        }
        void clear()
        {
            if(mem_buffer_)
            {
                delete[] mem_buffer_;
                mem_buffer_ = 0;
            }
            buffer_[0] = 0;
        }
        ~basic_stackstring()
        {
            clear();
        }

    private:
        static size_t get_space(size_t insize, size_t outsize, size_t in)
        {
            if(insize <= outsize)
                return in;
            else if(insize == 2 && outsize == 1)
                return 3 * in;
            else if(insize == 4 && outsize == 1)
                return 4 * in;
            else // if(insize == 4 && outsize == 2)
                return 2 * in;
        }
        output_char buffer_[buffer_size];
        output_char *mem_buffer_;
    }; // basic_stackstring

    ///
    /// Convenience typedef
    ///
    typedef basic_stackstring<wchar_t, char, 256> wstackstring;
    ///
    /// Convenience typedef
    ///
    typedef basic_stackstring<char, wchar_t, 256> stackstring;
    ///
    /// Convenience typedef
    ///
    typedef basic_stackstring<wchar_t, char, 16> wshort_stackstring;
    ///
    /// Convenience typedef
    ///
    typedef basic_stackstring<char, wchar_t, 16> short_stackstring;

} // namespace nowide
} // namespace boost

#endif
///
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
