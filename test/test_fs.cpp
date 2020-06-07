//
//  Copyright (c) 2015 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/fstream.hpp>

#include "test.hpp"

void test_main(int, char**, char**)
{
    const std::string prefix = "nowide-";
    const std::string utf8_name =
      prefix + "\xf0\x9d\x92\x9e-\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82-\xE3\x82\x84\xE3\x81\x82.txt";

    {
        boost::nowide::ofstream f(utf8_name);
        TEST(f);
        f << "Test" << std::endl;
    }

    const boost::nowide::filesystem::path path = utf8_name;

    TEST(std::filesystem::is_regular_file(path));

    TEST(boost::nowide::remove(utf8_name.c_str()) == 0);

    TEST(!std::filesystem::is_regular_file(path));

    {
        boost::nowide::ofstream f(path);
        TEST(f);
        f << "Test" << std::endl;
        TEST(is_regular_file(path));
    }
    {
        boost::nowide::ifstream f(path);
        TEST(f);
        std::string test;
        f >> test;
        TEST(test == "Test");
    }
    {
        boost::nowide::fstream f(path);
        TEST(f);
        std::string test;
        f >> test;
        TEST(test == "Test");
    }
    std::filesystem::remove(path);
}
