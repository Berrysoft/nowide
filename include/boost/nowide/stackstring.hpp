//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_NOWIDE_STACKSTRING_HPP_INCLUDED
#define BOOST_NOWIDE_STACKSTRING_HPP_INCLUDED

#include <boost/nowide/convert.hpp>
#include <cassert>
#include <cstring>
#include <string_view>

namespace boost {
namespace nowide {

    ///
    /// \brief A class that allows to create a temporary wide or narrow UTF strings from
    /// wide or narrow UTF source.
    ///
    /// It uses a stack buffer if the string is short enough
    /// otherwise allocates a buffer on the heap.
    ///
    /// Invalid UTF characters are replaced by the substitution character, see #BOOST_NOWIDE_REPLACEMENT_CHARACTER
    ///
    /// If a NULL pointer is passed to the constructor or convert method, NULL will be returned by c_str.
    /// Similarily a default constructed stackstring will return NULL on calling c_str.
    ///
    template<typename CharOut = wchar_t, typename CharIn = char, std::size_t BufferSize = 256>
    class basic_stackstring
    {
    public:
        /// Size of the stack buffer
        static constexpr std::size_t buffer_size = BufferSize;
        /// Type of the output character (converted to)
        using output_char = CharOut;
        /// Type of the input character (converted from)
        using input_char = CharIn;

        /// Creates a NULL stackstring
        constexpr basic_stackstring() noexcept
        {
            buffer_[0] = 0;
        }
        /// Convert the NULL terminated string input and store in internal buffer
        /// If input is NULL, nothing will be stored
        explicit basic_stackstring(const input_char* input)
        {
            convert(input);
        }
        /// Convert the sequence [begin, end) and store in internal buffer
        /// If begin is NULL, nothing will be stored
        basic_stackstring(const input_char* begin, const input_char* end)
        {
            convert(begin, end);
        }
        /// Copy construct from other
        basic_stackstring(const basic_stackstring& other)
        {
            *this = other;
        }
        /// Move construct from other
        basic_stackstring(basic_stackstring&& other) noexcept
        {
            *this = std::move(other);
        }
        /// Copy assign from other
        basic_stackstring& operator=(const basic_stackstring& other)
        {
            if(this != &other)
            {
                clear();
                const std::size_t len = other.length();
                if(other.uses_stack_memory())
                    data_ = buffer_;
                else if(other.data_)
                    data_ = new output_char[len + 1];
                else
                {
                    data_ = nullptr;
                    return *this;
                }
                std::memcpy(data_, other.data_, sizeof(output_char) * (len + 1));
            }
            return *this;
        }
        /// Move assign from other
        basic_stackstring& operator=(basic_stackstring&& other) noexcept
        {
            if(this != &other)
            {
                clear();
                if(other.uses_stack_memory())
                {
                    const std::size_t len = other.length();
                    data_ = buffer_;
                    std::memcpy(data_, other.data_, sizeof(output_char) * (len + 1));
                } else
                {
                    data_ = other.data_;
                    other.data_ = nullptr;
                }
            }
            return *this;
        }

        ~basic_stackstring()
        {
            clear();
        }

        /// Convert the NULL terminated string input and store in internal buffer
        /// If input is NULL, the current buffer will be reset to NULL
        output_char* convert(const input_char* input)
        {
            if(input)
                return convert(input, input + std::char_traits<input_char>::length(input));
            clear();
            return data();
        }
        /// Convert the sequence [begin, end) and store in internal buffer
        /// If begin is NULL, the current buffer will be reset to NULL
        output_char* convert(const input_char* begin, const input_char* end)
        {
            clear();

            if(begin)
            {
                const std::size_t input_len = end - begin;
                // Minimum size required: 1 output char per input char + trailing NULL
                const std::size_t min_output_size = input_len + 1;
                // If there is a chance the converted string fits on stack, try it
                if(min_output_size <= buffer_size && detail::convert_buffer(buffer_, buffer_size, begin, end))
                    data_ = buffer_;
                else
                {
                    // Fallback: Allocate a buffer that is surely large enough on heap
                    // Max size: Every input char is transcoded to the output char with maximum with + trailing NULL
                    const std::size_t max_output_size = input_len * detail::utf::utf_traits<output_char>::max_width + 1;
                    data_ = new output_char[max_output_size];
                    const bool success = detail::convert_buffer(data_, max_output_size, begin, end) == data_;
                    assert(success);
                    (void)success;
                }
            }
            return data();
        }
        /// Return the converted, NULL-terminated string or NULL if no string was converted
        constexpr output_char* data() noexcept
        {
            return data_;
        }
        /// Return the converted, NULL-terminated string or NULL if no string was converted
        constexpr const output_char* data() const noexcept
        {
            return data_;
        }
        /// Return the converted, NULL-terminated string or NULL if no string was converted
        constexpr const output_char* c_str() const noexcept
        {
            return data();
        }
        /// Reset the internal buffer to NULL
        void clear() noexcept
        {
            if(!uses_stack_memory())
                delete[] data_;
            data_ = nullptr;
        }
        /// Swap lhs with rhs
        friend void swap(basic_stackstring& lhs, basic_stackstring& rhs) noexcept
        {
            if(lhs.uses_stack_memory())
            {
                if(rhs.uses_stack_memory())
                {
                    std::swap(lhs.buffer_, rhs.buffer_);
                } else
                {
                    lhs.data_ = rhs.data_;
                    rhs.data_ = rhs.buffer_;
                    std::swap(lhs.buffer_, rhs.buffer_);
                }
            } else if(rhs.uses_stack_memory())
            {
                rhs.data_ = lhs.data_;
                lhs.data_ = lhs.buffer_;
                std::swap(lhs.buffer_, rhs.buffer_);
            } else
                std::swap(lhs.data_, rhs.data_);
        }

        constexpr operator std::basic_string_view<output_char>() const noexcept
        {
            if(!data_)
                return {};
            else
                return data_;
        }

        constexpr output_char& operator[](std::size_t index) noexcept
        {
            return data_[index];
        }
        constexpr const output_char& operator[](std::size_t index) const noexcept
        {
            return data_[index];
        }

        /// Return the current length of the string excluding the NULL terminator
        /// If NULL is stored returns NULL
        constexpr std::size_t length() const noexcept
        {
            if(!data_)
                return 0;
            else
                return std::char_traits<output_char>::length(data_);
        }

        constexpr bool empty() const noexcept
        {
            if(!data_)
                return true;
            else
                return !data_[0];
        }

    protected:
        /// True if the stack memory is used
        constexpr bool uses_stack_memory() const noexcept
        {
            return data_ == buffer_;
        }

    private:
        output_char buffer_[buffer_size];
        output_char* data_{nullptr};
    }; // basic_stackstring

    ///
    /// Convenience typedef
    ///
    using wstackstring = basic_stackstring<wchar_t, char, 256>;
    ///
    /// Convenience typedef
    ///
    using stackstring = basic_stackstring<char, wchar_t, 256>;
    ///
    /// Convenience typedef
    ///
    using wshort_stackstring = basic_stackstring<wchar_t, char, 16>;
    ///
    /// Convenience typedef
    ///
    using short_stackstring = basic_stackstring<char, wchar_t, 16>;

} // namespace nowide
} // namespace boost

#endif
