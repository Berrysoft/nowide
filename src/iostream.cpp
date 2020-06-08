//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2020 Berrysoft
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define NOWIDE_SOURCE
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include <new>
#include <nowide/convert.hpp>
#include <nowide/iostream.hpp>
#include <vector>

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

    class oconbuf : public std::streambuf
    {
    public:
        oconbuf(HANDLE h) : handle_(h)
        {}

    protected:
        int sync() override
        {
            return overflow(traits_type::eof());
        }
        int overflow(int c) override
        {
            if(!handle_)
                return -1;
            int n = static_cast<int>(pptr() - pbase());
            int r = 0;

            if(n > 0 && (r = write(pbase(), n)) < 0)
                return -1;
            if(r < n)
            {
                std::memmove(pbase(), pbase() + r, n - r);
            }
            setp(buffer_, buffer_ + buffer_size);
            pbump(n - r);
            if(c != traits_type::eof())
                sputc(traits_type::to_char_type(c));
            return 0;
        }

    private:
        using decoder = detail::utf::utf_traits<char>;
        using encoder = detail::utf::utf_traits<wchar_t>;

        int write(const char* p, int n)
        {
            namespace uf = detail::utf;
            const char* b = p;
            const char* e = p + n;
            DWORD size = 0;
            if(n > buffer_size)
                return -1;
            wchar_t* out = wbuffer_;
            uf::code_point c;
            size_t decoded = 0;
            while((c = decoder::decode(p, e)) != uf::incomplete)
            {
                if(c == uf::illegal)
                    c = NOWIDE_REPLACEMENT_CHARACTER;
                assert(out - wbuffer_ + encoder::width(c) <= static_cast<int>(wbuffer_size));
                out = encoder::encode(c, out);
                decoded = p - b;
            }
            if(!WriteConsoleW(handle_, wbuffer_, static_cast<DWORD>(out - wbuffer_), &size, 0))
                return -1;
            return static_cast<int>(decoded);
        }

        static constexpr int buffer_size = 1024;
        static constexpr int wbuffer_size = buffer_size * encoder::max_width;
        char buffer_[buffer_size];
        wchar_t wbuffer_[wbuffer_size];
        HANDLE handle_;
    };

    class iconbuf : public std::streambuf
    {
    public:
        iconbuf(HANDLE h) : handle_(h), wsize_(0), was_newline_(true)
        {}

    protected:
        int sync() override
        {
            if(FlushConsoleInputBuffer(handle_) == 0)
                return -1;
            wsize_ = 0;
            was_newline_ = true;
            pback_char_ = traits_type::eof();
            setg(0, 0, 0);
            return 0;
        }
        int pbackfail(int c) override
        {
            if(c == traits_type::eof())
                return traits_type::eof();

            if(gptr() != eback())
            {
                gbump(-1);
                *gptr() = traits_type::to_char_type(c);
                return 0;
            }

            setg(&pback_char_, &pback_char_, &pback_char_ + 1);
            *gptr() = traits_type::to_char_type(c);

            return 0;
        }

        int underflow() override
        {
            if(!handle_)
                return -1;
            pback_char_ = traits_type::eof();

            size_t n = read();
            setg(buffer_, buffer_, buffer_ + n);
            if(n == 0)
                return traits_type::eof();
            return traits_type::to_int_type(*gptr());
        }

    private:
        using decoder = detail::utf::utf_traits<wchar_t>;
        using encoder = detail::utf::utf_traits<char>;

        size_t read()
        {
            DWORD read_wchars = 0;
            const size_t n = wbuffer_size - wsize_;
            if(!ReadConsoleW(handle_, wbuffer_ + wsize_, static_cast<DWORD>(n), &read_wchars, 0))
                return 0;
            wsize_ += read_wchars;
            char* out = buffer_;
            const wchar_t* cur_input_ptr = wbuffer_;
            const wchar_t* const end_input_ptr = wbuffer_ + wsize_;
            while(cur_input_ptr != end_input_ptr)
            {
                const wchar_t* const prev_input_ptr = cur_input_ptr;
                detail::utf::code_point c = decoder::decode(cur_input_ptr, end_input_ptr);
                // If incomplete restore to beginning of incomplete char to use on next buffer
                if(c == detail::utf::incomplete)
                {
                    cur_input_ptr = prev_input_ptr;
                    break;
                }
                if(c == detail::utf::illegal)
                    c = NOWIDE_REPLACEMENT_CHARACTER;
                assert(out - buffer_ + encoder::width(c) <= static_cast<int>(buffer_size));
                // Skip \r chars as std::cin does
                if(c != '\r')
                    out = encoder::encode(c, out);
            }

            wsize_ = end_input_ptr - cur_input_ptr;
            if(wsize_ > 0)
                std::memmove(wbuffer_, end_input_ptr - wsize_, sizeof(wchar_t) * wsize_);

            // A CTRL+Z at the start of the line should be treated as EOF
            if(was_newline_ && out > buffer_ && buffer_[0] == '\x1a')
            {
                sync();
                return 0;
            }
            was_newline_ = out == buffer_ || out[-1] == '\n';

            return out - buffer_;
        }

        static constexpr size_t wbuffer_size = 1024;
        static constexpr size_t buffer_size = wbuffer_size * encoder::max_width;
        char buffer_[buffer_size];
        wchar_t wbuffer_[wbuffer_size];
        HANDLE handle_;
        size_t wsize_;
        char pback_char_;
        bool was_newline_;
    };

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
        auto pcout = std::launder(reinterpret_cast<std::ostream*>(cout));
        auto pclog = std::launder(reinterpret_cast<std::ostream*>(clog));

        pclog->flush();
        pcout->flush();
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
