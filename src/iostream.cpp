//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define NOWIDE_SOURCE
#include <cassert>
#include <cstring>
#include <iostream>
#include <new>
#include <nowide/convert.hpp>
#include <nowide/iostream.hpp>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#ifdef NOWIDE_MSVC
#define NOWIDE_IOSTREAM_DECL
#else
#define NOWIDE_IOSTREAM_DECL NOWIDE_DECL
#endif // NOWIDE_MSVC

namespace nowide {
namespace detail {
    static bool is_atty_handle(HANDLE h) noexcept
    {
        if(h)
        {
            DWORD dummy;
            return GetConsoleMode(h, &dummy) != FALSE;
        }
        return false;
    }

    template<typename T, std::size_t N>
    constexpr std::size_t bufsize(T (&buffer)[N]) noexcept
    {
        for(std::size_t i = 0; i < N; i++)
        {
            if(!buffer[i])
                return i;
        }
        return N;
    }

    template<typename CharType, typename TraitsType = std::char_traits<CharType>>
    class basic_oconbuf : public std::basic_streambuf<CharType, TraitsType>
    {
    public:
        basic_oconbuf(HANDLE h) : handle_(h)
        {}

    protected:
        using typename std::basic_streambuf<CharType, TraitsType>::traits_type;
        using typename std::basic_streambuf<CharType, TraitsType>::int_type;

        int_type overflow(int_type c) override
        {
            namespace uf = detail::utf;
            if(!handle_)
                return traits_type::eof();
            if(traits_type::eq_int_type(c, traits_type::eof()))
                return 0;
            std::size_t bsize = bufsize(buffer_);
            CharType inc = traits_type::to_char_type(c);
            wchar_t outc[encoder::max_width];
            std::size_t osize = 0;
            uf::code_point code;
            CharType* pbuf = buffer_;
            if(bsize >= decoder::max_width)
            {
                code = decoder::decode(pbuf, pbuf + decoder::max_width);
            } else
            {
                buffer_[bsize] = inc;
                code = decoder::decode(pbuf, pbuf + bsize + 1);
            }
            if(code == uf::incomplete)
            {
                return 0;
            } else if(code == uf::illegal)
            {
                outc[0] = NOWIDE_REPLACEMENT_CHARACTER;
                osize = 1;
            } else
            {
                wchar_t* p = encoder::encode(code, outc);
                osize = p - outc;
            }
            std::memset(buffer_, 0, sizeof(buffer_));
            if(!WriteConsoleW(handle_, outc, static_cast<DWORD>(osize), 0, 0))
                return -1;
            return static_cast<int_type>(osize);
        }

    private:
        using decoder = detail::utf::utf_traits<CharType>;
        using encoder = detail::utf::utf_traits<wchar_t>;

        CharType buffer_[decoder::max_width];
        HANDLE handle_;
    };

    using oconbuf = basic_oconbuf<char>;

    template<typename CharType, typename TraitsType = std::char_traits<CharType>>
    class basic_iconbuf : public std::basic_streambuf<CharType, TraitsType>
    {
    public:
        basic_iconbuf(HANDLE h) : handle_(h), was_newline_(true)
        {}

    protected:
        using typename std::basic_streambuf<CharType, TraitsType>::traits_type;
        using typename std::basic_streambuf<CharType, TraitsType>::int_type;

        int sync() override
        {
            if(!FlushConsoleInputBuffer(handle_))
                return -1;
            this->setg(nullptr, nullptr, nullptr);
            return 0;
        }

        int_type pbackfail(int_type c) override
        {
            if(traits_type::eq_int_type(c, traits_type::eof()))
                return traits_type::eof();

            buffer_[0] = traits_type::to_char_type(c);
            this->setg(buffer_, buffer_, buffer_ + bufsize(buffer_));

            return 0;
        }

        int_type underflow() override
        {
            namespace uf = detail::utf;

            if(!handle_)
                return traits_type::eof();

            std::memset(buffer_, 0, sizeof(buffer_));

            wchar_t inc;
            std::size_t bsize = bufsize(wbuffer_);
            DWORD read_chars;
            if(!ReadConsoleW(handle_, &inc, 1, &read_chars, 0))
                return traits_type::eof();
            wbuffer_[bsize] = inc;
            wchar_t* pbuf = wbuffer_;
            uf::code_point code = decoder::decode(pbuf, pbuf + bsize + 1);
            std::size_t out_size;
            if(code == uf::incomplete)
                return underflow();
            else if(code == uf::illegal)
            {
                buffer_[1] = traits_type::to_char_type(NOWIDE_REPLACEMENT_CHARACTER);
                out_size = 1;
            } else
            {
                CharType* p = encoder::encode(code, buffer_ + 1);
                for(CharType* pcur = buffer_ + 1; pcur < p;)
                {
                    if(*pcur == CharType{'\r'})
                    {
                        std::memmove(pcur, pcur + 1, (p - pcur - 1) * sizeof(CharType));
                        p--;
                    } else
                        pcur++;
                }
                out_size = p - buffer_ - 1;
            }
            std::memset(wbuffer_, 0, sizeof(wbuffer_));
            if(out_size == 0)
                return underflow();
            this->setg(buffer_ + 1, buffer_ + 1, buffer_ + 1 + out_size);
            // A CTRL+Z at the start of the line should be treated as EOF
            if(was_newline_ && out_size && buffer_[1] == '\x1a')
            {
                sync();
                return traits_type::eof();
            }
            was_newline_ = out_size == 0 || buffer_[1] == '\n';
            return traits_type::to_int_type(*this->gptr());
        }

    private:
        using decoder = detail::utf::utf_traits<wchar_t>;
        using encoder = detail::utf::utf_traits<CharType>;

        wchar_t wbuffer_[decoder::max_width];
        CharType buffer_[encoder::max_width + 1];
        HANDLE handle_;
        bool was_newline_;
    };

    using iconbuf = basic_iconbuf<char>;
} // namespace detail

alignas(detail::iconbuf) static char cin_buf[sizeof(detail::iconbuf)];
alignas(detail::oconbuf) static char cout_buf[sizeof(detail::oconbuf)];
alignas(detail::oconbuf) static char cerr_buf[sizeof(detail::oconbuf)];

NOWIDE_IOSTREAM_DECL alignas(std::istream) char cin[sizeof(std::istream)];
NOWIDE_IOSTREAM_DECL alignas(std::ostream) char cout[sizeof(std::ostream)];
NOWIDE_IOSTREAM_DECL alignas(std::ostream) char cerr[sizeof(std::ostream)];
NOWIDE_IOSTREAM_DECL alignas(std::ostream) char clog[sizeof(std::ostream)];

#ifdef NOWIDE_MSVC
#define NOWIDE_ISTREAM(name) "?" #name "@nowide@@3V?$basic_istream@DU?$char_traits@D@std@@@std@@A"
#define NOWIDE_OSTREAM(name) "?" #name "@nowide@@3V?$basic_ostream@DU?$char_traits@D@std@@@std@@A"
#define NOWIDE_PCHAR(name) "?" #name "@nowide@@3PADA"

#define NOWIDE_MSVC_EXPORT(exp, ori) "/EXPORT:" exp "=" ori

#define NOWIDE_EXPORT_ISTREAM(name) NOWIDE_MSVC_EXPORT(NOWIDE_ISTREAM(name), NOWIDE_PCHAR(name))
#define NOWIDE_EXPORT_OSTREAM(name) NOWIDE_MSVC_EXPORT(NOWIDE_OSTREAM(name), NOWIDE_PCHAR(name))

#pragma comment(linker, NOWIDE_EXPORT_ISTREAM(cin))
#pragma comment(linker, NOWIDE_EXPORT_OSTREAM(cout))
#pragma comment(linker, NOWIDE_EXPORT_OSTREAM(cerr))
#pragma comment(linker, NOWIDE_EXPORT_OSTREAM(clog))
#endif // NOWIDE_MSVC

namespace detail {
    struct DoInit
    {
        DoInit();
        ~DoInit();
    };

    DoInit::DoInit()
    {
        HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE error_handle = GetStdHandle(STD_ERROR_HANDLE);

        std::istream* pcin;
        if(is_atty_handle(input_handle))
        {
            pcin = new(cin) std::istream(new(cin_buf) iconbuf(input_handle));
        } else
        {
            pcin = new(cin) std::istream(std::cin.rdbuf());
        }
        std::ostream* pcout;
        if(is_atty_handle(output_handle))
        {
            pcout = new(cout) std::ostream(new(cout_buf) oconbuf(output_handle));
        } else
        {
            pcout = new(cout) std::ostream(std::cout.rdbuf());
        }
        std::ostream* pcerr;
        if(is_atty_handle(error_handle))
        {
            pcerr = new(cerr) std::ostream(new(cerr_buf) oconbuf(error_handle));
        } else
        {
            pcerr = new(cerr) std::ostream(std::cerr.rdbuf());
        }
        new(clog) std::ostream(pcerr->rdbuf());
        pcin->tie(pcout);
        pcerr->tie(pcout);
    }

    DoInit::~DoInit()
    {
        auto pcin = std::launder(reinterpret_cast<std::istream*>(cin));
        auto pcout = std::launder(reinterpret_cast<std::ostream*>(cout));
        auto pcerr = std::launder(reinterpret_cast<std::ostream*>(cerr));
        auto pclog = std::launder(reinterpret_cast<std::ostream*>(clog));

        pcout->flush();
        pclog->flush();

        pclog->~basic_ostream();
        pcerr->~basic_ostream();
        pcout->~basic_ostream();
        pcin->~basic_istream();

        HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE error_handle = GetStdHandle(STD_ERROR_HANDLE);

        auto pcin_buf = std::launder(reinterpret_cast<iconbuf*>(cin_buf));
        auto pcout_buf = std::launder(reinterpret_cast<oconbuf*>(cout_buf));
        auto pcerr_buf = std::launder(reinterpret_cast<oconbuf*>(cerr_buf));

        if(is_atty_handle(error_handle))
        {
            pcerr_buf->~oconbuf();
        }
        if(is_atty_handle(output_handle))
        {
            pcout_buf->~oconbuf();
        }
        if(is_atty_handle(input_handle))
        {
            pcin_buf->~iconbuf();
        }
    }
} // namespace detail

namespace ios_base {
    Init::Init()
    {
        static nowide::detail::DoInit __do_init{};
    }

    Init::~Init()
    {}
} // namespace ios_base
} // namespace nowide
