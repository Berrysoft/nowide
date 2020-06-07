//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019-2020 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <nowide/stackstring.hpp>
#include <string_view>
#include <vector>

#include "test.hpp"
#include "test_sets.hpp"

#if defined(NOWIDE_MSVC) && NOWIDE_MSVC < 1700
#pragma warning(disable : 4428) // universal-character-name encountered in source
#endif

template<typename CharOut, typename CharIn, size_t BufferSize>
class test_basic_stackstring : public nowide::basic_stackstring<CharOut, CharIn, BufferSize>
{
public:
    using parent = nowide::basic_stackstring<CharOut, CharIn, BufferSize>;

    using parent::parent;
    using parent::uses_stack_memory;
    bool uses_heap_memory() const
    {
        return !uses_stack_memory() && this->data();
    }
};

using test_wstackstring = test_basic_stackstring<wchar_t, char, 256>;
using test_stackstring = test_basic_stackstring<char, wchar_t, 256>;

std::wstring stackstring_to_wide(const std::string& s)
{
    const test_wstackstring ss(s.c_str());
    TEST(ss.uses_stack_memory());
    return ss.data();
}

std::string stackstring_to_narrow(const std::wstring& s)
{
    const test_stackstring ss(s.c_str());
    TEST(ss.uses_stack_memory());
    return ss.data();
}

std::wstring heap_stackstring_to_wide(const std::string& s)
{
    const test_basic_stackstring<wchar_t, char, 1> ss(s.c_str());
    TEST(ss.uses_heap_memory() || s.empty());
    return ss.data();
}

std::string heap_stackstring_to_narrow(const std::wstring& s)
{
    const test_basic_stackstring<char, wchar_t, 1> ss(s.c_str());
    TEST(ss.uses_heap_memory() || s.empty());
    return ss.data();
}

void test_main(int, char**, char**)
{
    std::string hello = "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d";
    std::wstring whello = nowide::widen(hello);
    const wchar_t* wempty = L"";

    {
        std::cout << "-- Default constructed string is nullptr" << std::endl;
        const nowide::short_stackstring s;
        TEST(s.data() == nullptr);
    }
    {
        std::cout << "-- nullptr passed to ctor results in nullptr" << std::endl;
        const nowide::short_stackstring s(nullptr);
        TEST(s.data() == nullptr);
        const nowide::short_stackstring s2(nullptr, nullptr);
        TEST(s2.data() == nullptr);
    }
    {
        std::cout << "-- empty string view to ctor results in nullptr" << std::endl;
        const std::wstring_view whello_view = whello;
        const nowide::short_stackstring s(std::wstring_view{});
        TEST(s.data() == nullptr);
        const nowide::short_stackstring s2(whello_view.substr(0, 0));
        TEST(s.data() == nullptr);
    }
    {
        std::cout << "-- nullptr passed to convert results in nullptr" << std::endl;
        nowide::short_stackstring s(L"foo");
        TEST(s.data() == std::string_view("foo"));
        s.convert(nullptr);
        TEST(s.data() == nullptr);
        nowide::short_stackstring s2(L"foo");
        TEST(s2.data() == std::string_view("foo"));
        s2.convert(nullptr, nullptr);
        TEST(s2.data() == nullptr);
    }
    {
        std::cout << "-- An empty string is accepted" << std::endl;
        const nowide::short_stackstring s(wempty);
        TEST(s.data());
        TEST(s == std::string_view());
        const nowide::short_stackstring s2(wempty, wempty);
        TEST(s2.data());
        TEST(s2 == std::string_view());
    }
    {
        std::cout << "-- An empty string is accepted" << std::endl;
        nowide::short_stackstring s, s2;
        TEST(s.convert(wempty));
        TEST(s.empty());
        TEST(s == std::string_view());
        TEST(s2.convert(wempty, wempty));
        TEST(s2.empty());
        TEST(s2 == std::string_view());
    }
    {
        std::cout << "-- Will be put on heap" << std::endl;
        test_basic_stackstring<wchar_t, char, 3> sw;
        TEST(sw.convert(hello.c_str()));
        TEST(sw.uses_heap_memory());
        TEST(sw.data() == whello);
        TEST(sw.convert(hello.c_str(), hello.c_str() + hello.size()));
        TEST(sw.uses_heap_memory());
        TEST(sw.data() == whello);
    }
    {
        std::cout << "-- Will be put on stack" << std::endl;
        test_basic_stackstring<wchar_t, char, 40> sw;
        TEST(sw.convert(hello.c_str()));
        TEST(sw.uses_stack_memory());
        TEST(sw.data() == whello);
        TEST(sw.convert(hello.c_str(), hello.c_str() + hello.size()));
        TEST(sw.uses_stack_memory());
        TEST(sw.data() == whello);
    }
    {
        std::cout << "-- Will be put on heap" << std::endl;
        test_basic_stackstring<char, wchar_t, 3> sw;
        TEST(sw.convert(whello.c_str()));
        TEST(sw.uses_heap_memory());
        TEST(sw.data() == hello);
        TEST(sw.convert(whello.c_str(), whello.c_str() + whello.size()));
        TEST(sw.uses_heap_memory());
        TEST(sw.data() == hello);
    }
    {
        std::cout << "-- Will be put on stack" << std::endl;
        test_basic_stackstring<char, wchar_t, 40> sw;
        TEST(sw.convert(whello.c_str()));
        TEST(sw.uses_stack_memory());
        TEST(sw.data() == hello);
        TEST(sw.convert(whello.c_str(), whello.c_str() + whello.size()));
        TEST(sw.uses_stack_memory());
        TEST(sw.data() == hello);
    }
    {
        using stackstring = test_basic_stackstring<wchar_t, char, 6>;
        const std::wstring_view heapVal = L"heapValue";
        const std::wstring_view stackVal = L"stack";
        const stackstring heap(nowide::narrow(heapVal).c_str());
        const stackstring stack(nowide::narrow(stackVal).c_str());
        TEST(heap.uses_heap_memory());
        TEST(stack.uses_stack_memory());

        {
            stackstring sw2(heap), sw3, sEmpty;
            sw3 = heap;
            TEST(sw2 == heapVal);
            TEST(sw3 == heapVal);
            // Self assign avoiding clang self-assign-overloaded warning
            sw3 = static_cast<const stackstring&>(sw3); //-V570
            TEST(sw3 == heapVal);
            // Assign empty
            sw3 = sEmpty; //-V820
            TEST(sw3.empty());
        }
        {
            stackstring sw2(stack), sw3, sEmpty;
            sw3 = stack;
            TEST(sw2 == stackVal);
            TEST(sw3 == stackVal);
            // Self assign avoiding clang self-assign-overloaded warning
            sw3 = static_cast<const stackstring&>(sw3); //-V570
            TEST(sw3 == stackVal);
            // Assign empty
            sw3 = sEmpty; //-V820
            TEST(sw3.empty());
        }
        {
            stackstring sw2(stack);
            sw2 = heap;
            TEST(sw2 == heapVal);
        }
        {
            stackstring sw2(heap);
            sw2 = stack;
            TEST(sw2 == stackVal);
        }
        {
            stackstring sw2(heap), sw3(stack), sEmpty1, sEmpty2;
            swap(sw2, sw3);
            TEST(sw2 == stackVal);
            TEST(sw3 == heapVal);
            swap(sw2, sw3);
            TEST(sw2 == heapVal);
            TEST(sw3 == stackVal);
            swap(sw2, sEmpty1);
            TEST(sEmpty1 == heapVal);
            TEST(sw2.empty());
            swap(sw3, sEmpty2);
            TEST(sEmpty2 == stackVal);
            TEST(sw3.empty());
        }
        {
            stackstring sw2(heap), sw3(heap);
            sw3[0] = 'z';
            const std::wstring val2 = sw3.data();
            swap(sw2, sw3);
            TEST(sw2.data() == val2);
            TEST(sw3 == heapVal);
        }
        {
            stackstring sw2(stack), sw3(stack);
            sw3[0] = 'z';
            const std::wstring val2 = sw3.data();
            swap(sw2, sw3);
            TEST(sw2.data() == val2);
            TEST(sw3 == stackVal);
        }
        std::cout << "-- Sanity check" << std::endl;
        TEST(stack == stackVal);
        TEST(heap == heapVal);
    }
    {
        std::cout << "-- Test putting stackstrings into vector (done by args) class" << std::endl;
        // Use a smallish buffer, to have stack and heap values
        using stackstring = nowide::basic_stackstring<wchar_t, char, 5>;
        std::vector<stackstring> strings;
        strings.resize(2);
        TEST(strings[0].convert("1234") == std::wstring_view(L"1234"));
        TEST(strings[1].convert("Hello World") == std::wstring_view(L"Hello World"));
        strings.push_back(stackstring("FooBar"));
        TEST(strings[0] == std::wstring_view(L"1234"));
        TEST(strings[1] == std::wstring_view(L"Hello World"));
        TEST(strings[2] == std::wstring_view(L"FooBar"));
    }
    std::cout << "- Stackstring" << std::endl;
    run_all(stackstring_to_wide, stackstring_to_narrow);
    std::cout << "- Heap Stackstring" << std::endl;
    run_all(heap_stackstring_to_wide, heap_stackstring_to_narrow);
}
