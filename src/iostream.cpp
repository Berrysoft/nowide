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

    class console_output_buffer : public std::streambuf
    {
    public:
        console_output_buffer(HANDLE h) : handle_(h)
        {}

        bool is_atty() noexcept
        {
            return is_atty_handle(handle_);
        }

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

    class console_input_buffer : public std::streambuf
    {
    public:
        console_input_buffer(HANDLE h) : handle_(h), wsize_(0), was_newline_(true)
        {}

        bool is_atty() noexcept
        {
            return is_atty_handle(handle_);
        }

    protected:
        int sync() override
        {
            if(FlushConsoleInputBuffer(handle_) == 0)
                return -1;
            wsize_ = 0;
            was_newline_ = true;
            pback_buffer_.clear();
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

            char* pnext;
            if(pback_buffer_.empty())
            {
                pback_buffer_.resize(4);
                pnext = &pback_buffer_[0] + pback_buffer_.size() - 1u;
            } else
            {
                size_t n = pback_buffer_.size();
                pback_buffer_.resize(n * 2);
                std::memcpy(&pback_buffer_[n], &pback_buffer_[0], n);
                pnext = &pback_buffer_[0] + n - 1;
            }

            char* pFirst = &pback_buffer_[0];
            char* pLast = pFirst + pback_buffer_.size();
            setg(pFirst, pnext, pLast);
            *gptr() = traits_type::to_char_type(c);

            return 0;
        }

        int underflow() override
        {
            if(!handle_)
                return -1;
            if(!pback_buffer_.empty())
                pback_buffer_.clear();

            size_t n = read();
            setg(buffer_, buffer_, buffer_ + n);
            if(n == 0)
                return traits_type::eof();
            return std::char_traits<char>::to_int_type(*gptr());
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
        std::vector<char> pback_buffer_;
        bool was_newline_;
    };
} // namespace detail

alignas(detail::console_input_buffer) static char cin_buf[sizeof(detail::console_input_buffer)];
alignas(detail::console_output_buffer) static char cout_buf[sizeof(detail::console_output_buffer)];
alignas(detail::console_output_buffer) static char cerr_buf[sizeof(detail::console_output_buffer)];

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
        auto pcin_buf = new(cin_buf) detail::console_input_buffer(GetStdHandle(STD_INPUT_HANDLE));
        auto pcout_buf = new(cout_buf) detail::console_output_buffer(GetStdHandle(STD_OUTPUT_HANDLE));
        auto pcerr_buf = new(cerr_buf) detail::console_output_buffer(GetStdHandle(STD_ERROR_HANDLE));

        std::istream* pcin;
        if(pcin_buf->is_atty())
        {
            pcin = new(cin) std::istream(pcin_buf);
        } else
        {
            pcin = new(cin) std::istream(std::cin.rdbuf());
        }
        std::ostream* pcout;
        if(pcout_buf->is_atty())
        {
            pcout = new(cout) std::ostream(pcout_buf);
        } else
        {
            pcout = new(cout) std::ostream(std::cout.rdbuf());
        }
        std::ostream* pcerr;
        if(pcerr_buf->is_atty())
        {
            pcerr = new(cerr) std::ostream(pcerr_buf);
            new(clog) std::ostream(pcerr_buf);
        } else
        {
            pcerr = new(cerr) std::ostream(std::cerr.rdbuf());
            new(clog) std::ostream(std::clog.rdbuf());
        }

        pcin->tie(pcout);
        pcerr->tie(pcout);
    }

    DoInit::~DoInit()
    {
        auto pcin = std::launder(reinterpret_cast<std::istream*>(cin));
        auto pcout = std::launder(reinterpret_cast<std::ostream*>(cout));
        auto pcerr = std::launder(reinterpret_cast<std::ostream*>(cerr));
        auto pclog = std::launder(reinterpret_cast<std::ostream*>(clog));
        try
        {
            pclog->flush();
        } catch(...)
        {}
        pclog->~basic_ostream();
        try
        {
            pcerr->flush();
        } catch(...)
        {}
        pcerr->~basic_ostream();
        try
        {
            pcout->flush();
        } catch(...)
        {}
        pcout->~basic_ostream();
        pcin->~basic_istream();

        auto pcin_buf = std::launder(reinterpret_cast<detail::console_input_buffer*>(cin_buf));
        auto pcout_buf = std::launder(reinterpret_cast<detail::console_output_buffer*>(cout_buf));
        auto pcerr_buf = std::launder(reinterpret_cast<detail::console_output_buffer*>(cerr_buf));

        pcerr_buf->~console_output_buffer();
        pcout_buf->~console_output_buffer();
        pcin_buf->~console_input_buffer();
    }
} // namespace detail

namespace ios {
    Init::Init()
    {
        static nowide::detail::DoInit __do_init{};
    }

    Init::~Init()
    {}
} // namespace ios
} // namespace nowide
