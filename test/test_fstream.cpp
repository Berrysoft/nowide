//
//  Copyright (c) 2012 Artyom Beilis (Tonkikh)
//  Copyright (c) 2019 Alexander Grund
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/nowide/fstream.hpp>
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/convert.hpp>
#define BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "test.hpp"

#ifdef BOOST_MSVC
#pragma warning(disable : 4996)
#endif

namespace nw = boost::nowide;
template<typename FStream>
class io_fstream
{
public:
    void open(char const *file)
    {
        f_.open(file, std::fstream::out | std::fstream::in | std::fstream::trunc);
        TEST(f_);
    }
    void write(char *buf, int size)
    {
        f_.write(buf, size);
    }
    void read(char *buf, int size)
    {
        f_.read(buf, size);
    }
    void rewind()
    {
        f_.seekg(0);
        f_.seekp(0);
    }
    void flush()
    {
        f_ << std::flush;
    }
    void close()
    {
        f_.close();
    }

private:
    FStream f_;
};

class io_stdio
{
public:
    void open(char const *file)
    {
        f_ = fopen(file, "w+");
        TEST(f_);
    }
    void write(char *buf, int size)
    {
        fwrite(buf, 1, size, f_);
    }
    void read(char *buf, int size)
    {
        size_t res = fread(buf, 1, size, f_);
        (void)res;
    }
    void rewind()
    {
        ::rewind(f_);
    }
    void flush()
    {
        fflush(f_);
    }
    void close()
    {
        fclose(f_);
        f_ = 0;
    }

private:
    FILE *f_;
};

template<typename FStream>
void test_io(char *file, char const *type)
{
    std::cout << "Testing I/O performance " << type << std::endl;
    FStream tmp;
    tmp.open(file);
    int data_size = 64 * 1024 * 1024;
    for(int block_size = 16; block_size <= 8192; block_size *= 2)
    {
        std::vector<char> buf(block_size, ' ');
        int size = 0;
        tmp.rewind();
        boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();
        while(size < data_size)
        {
            tmp.write(&buf[0], block_size);
            size += block_size;
        }
        tmp.flush();
        boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
        double tm = boost::chrono::duration_cast<boost::chrono::milliseconds>(t2 - t1).count() * 1e-3;
        // heatup
        if(block_size >= 32)
            std::cout << "  write block size " << std::setw(8) << block_size << " " << std::fixed << std::setprecision(3)
                      << (data_size / 1024.0 / 1024 / tm) << " MB/s" << std::endl;
    }
    for(int block_size = 32; block_size <= 8192; block_size *= 2)
    {
        std::vector<char> buf(block_size, ' ');
        int size = 0;
        tmp.rewind();
        boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();
        while(size < data_size)
        {
            tmp.read(&buf[0], block_size);
            size += block_size;
        }
        boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
        double tm = boost::chrono::duration_cast<boost::chrono::milliseconds>(t2 - t1).count() * 1e-3;
        std::cout << "   read block size " << std::setw(8) << block_size << " " << std::fixed << std::setprecision(3)
                  << (data_size / 1024.0 / 1024 / tm) << " MB/s" << std::endl;
    }
    tmp.close();
    std::remove(file);
}

void test_perf(char *file)
{
    test_io<io_stdio>(file, "stdio");
    test_io<io_fstream<std::fstream> >(file, "std::fstream");
    test_io<io_fstream<nw::fstream> >(file, "nowide::fstream");
}

void make_empty_file(const char *filepath)
{
    nw::ofstream f(filepath, std::ios_base::out | std::ios::trunc);
    TEST(f);
}

bool file_exists(const char *filepath)
{
    FILE *f = nw::fopen(filepath, "r");
    if(f)
    {
        std::fclose(f);
        return true;
    } else
        return false;
}

template<size_t N>
bool file_contents_equal(const char *filepath, const char (&contents)[N], bool binary_mode = false)
{
    FILE *f = nw::fopen(filepath, binary_mode ? "rb" : "r");
    if(!f)
        return false;
    for(size_t i = 0; i + 1 < N; i++)
    {
        if(std::fgetc(f) != contents[i])
            return false;
    }
    if(std::fgetc(f) != EOF)
        return false;
    std::fclose(f);
    return true;
}

template<typename FStream>
void test_with_different_buffer_sizes(const char *filepath)
{
    /* Important part of the standard for mixing input with output:
       However, output shall not be directly followed by input without an intervening call to the fflush function
       or to a file positioning function (fseek, fsetpos, or rewind),
       and input shall not be directly followed by output without an intervening call to a file positioning function,
       unless the input operation encounters end-of-file.
    */
    for(int i = -1; i < 16; i++)
    {
        std::cout << "Buffer size = " << i << std::endl;
        char buf[16];
        FStream f;
        // Different conditions when setbuf might be called: Usually before opening a file is OK
        if(i >= 0)
            f.rdbuf()->pubsetbuf((i == 0) ? NULL : buf, i);
        f.open(filepath, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        TEST(f);

        TEST(f.put('a'));
        TEST(f.put('b'));
        TEST(f.put('c'));
        TEST(f.put('d'));
        TEST(f.put('e'));
        TEST(f.put('f'));
        TEST(f.put('g'));
        TEST(f.seekg(0));
        TEST(f.get() == 'a');
        TEST(f.seekg(1, std::ios::cur));
        TEST(f.get() == 'c');
        TEST(f.seekg(-1, std::ios::cur));
        TEST(f.get() == 'c');
        TEST(f.seekg(1));
        TEST(f.put('B'));
        TEST(f.flush()); // Flush when changing out->in
        TEST(f.get() == 'c');
        TEST(f.seekg(1));
        TEST(f.get() == 'B');
        TEST(f.seekg(2));
        TEST(f.put('C'));
        TEST(f.seekg(3)); // Seek when changing out->in
        TEST(f.get() == 'd');
        TEST(f.seekg(0));
        TEST(f.get() == 'a');
        TEST(f.get() == 'B');
        TEST(f.get() == 'C');
        TEST(f.get() == 'd');
        TEST(f.get() == 'e');
        // Putback after flush
        TEST(f << std::flush);
        TEST(f.putback('e'));
        TEST(f.putback('d'));
        TEST(f.get() == 'd');
        TEST(f.get() == 'e');
        TEST(f.get() == 'f');
        TEST(f.get() == 'g');
        TEST(f.get() == EOF);
        f.clear();
        TEST(f.seekg(1));
        TEST(f.get() == 'B');
        TEST(f.putback('B'));
        TEST(f.putback('a'));
        TEST(!f.putback('x')); // At beginning of file -> No putback possible
        // Get characters that were putback to avoid MSVC bug https://github.com/microsoft/STL/issues/342
        f.clear();
        TEST(f.get() == 'a');
        TEST(f.get() == 'B');
        f.close();
        TEST(nw::remove(filepath) == 0);
    }
}

template<typename FileBuf>
void test_close(const char *filepath)
{
    const std::string filepath2 = std::string(filepath) + ".2";
    // Make sure file does not exist yet
    nw::remove(filepath2.c_str());
    TEST(!file_exists(filepath2.c_str()));
    FileBuf buf;
    TEST(buf.open(filepath, std::ios_base::out) == &buf);
    TEST(buf.is_open());
    // Opening when already open fails
    TEST(buf.open(filepath2.c_str(), std::ios_base::out) == NULL);
    // Still open
    TEST(buf.is_open());
    TEST(buf.close() == &buf);
    // Failed opening did not create file
    TEST(!file_exists(filepath2.c_str()));
    // But it should work now:
    TEST(buf.open(filepath2.c_str(), std::ios_base::out) == &buf);
    TEST(buf.close() == &buf);
    TEST(file_exists(filepath2.c_str()));
    TEST(nw::remove(filepath) == 0);
    TEST(nw::remove(filepath2.c_str()) == 0);
}

template<typename IFStream, typename OFStream>
void test_flush(const char *filepath)
{
    OFStream fo(filepath, std::ios_base::out | std::ios::trunc);
    TEST(fo);
    std::string curValue;
    for(int repeat = 0; repeat < 2; repeat++)
    {
        for(size_t len = 1; len <= 1024; len *= 2)
        {
            char c = static_cast<char>(len % 13 + repeat + 'a'); // semi-random char
            std::string input(len, c);
            fo << input;
            curValue += input;
            TEST(fo.flush());
            std::string s;
            // Note: Flush on read area is implementation defined, so check whole file instead
            IFStream fi(filepath);
            TEST(fi >> s);
            TEST(s == curValue);
        }
    }
}

void test_ofstream_creates_file(const char *filename)
{
    nw::remove(filename);
    TEST(!file_exists(filename));
    // Ctor
    {
        nw::ofstream fo(filename);
        TEST(fo);
    }
    TEST(file_exists(filename));
    TEST(file_contents_equal(filename, ""));
    nw::remove(filename);
    // Open
    {
        nw::ofstream fo;
        fo.open(filename);
        TEST(fo);
    }
    TEST(file_exists(filename));
    TEST(file_contents_equal(filename, ""));
    nw::remove(filename);
}

// Create filename file with content "test\n"
void test_ofstream_write(const char *filename)
{
    // char* ctor
    {
        nw::ofstream fo(filename);
        TEST(fo << "test" << 2 << std::endl);
    }
    // char* open
    TEST(file_contents_equal(filename, "test2\n"));
    TEST(nw::remove(filename) == 0);
    {
        nw::ofstream fo;
        fo.open(filename);
        TEST(fo << "test" << 2 << std::endl);
    }
    TEST(file_contents_equal(filename, "test2\n"));
    TEST(nw::remove(filename) == 0);
    // C++11 interfaces aren't enabled at all platforms so need to skip std::string arg for std::*fstream
#if defined(BOOST_WINDOWS)
    // string ctor
    {
        std::string name = filename;
        nw::ofstream fo(name);
        TEST(fo << "test" << 2 << std::endl);
    }
    TEST(file_contents_equal(filename, "test2\n"));
    TEST(nw::remove(filename) == 0);
    // string open
    {
        nw::ofstream fo;
        fo.open(std::string(filename));
        TEST(fo << "test" << 2 << std::endl);
    }
    TEST(file_contents_equal(filename, "test2\n"));
    TEST(nw::remove(filename) == 0);
#endif
    // Binary mode
    {
        nw::ofstream fo(filename, std::ios::binary);
        TEST(fo << "test" << 2 << std::endl);
    }
    TEST(file_contents_equal(filename, "test2\n", true));
    TEST(nw::remove(filename) == 0);
    // At end
    {
        {
            nw::ofstream fo(filename);
            TEST(fo << "test" << 2 << std::endl);
        }
        nw::ofstream fo(filename, std::ios::ate | std::ios::in);
        fo << "second" << 2 << std::endl;
    }
    TEST(file_contents_equal(filename, "test2\nsecond2\n"));
    TEST(nw::remove(filename) == 0);
}

void test_ifstream_open_read(const char *filename)
{
    // Create test file
    {
        nw::ofstream fo(filename);
        TEST(fo << "test" << std::endl);
    }

    // char* Ctor
    {
        nw::ifstream fi(filename);
        TEST(fi);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "test");
    }
    // char* open
    {
        nw::ifstream fi;
        fi.open(filename);
        TEST(fi);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "test");
    }
    // C++11 interfaces aren't enabled at all platforms so need to skip std::string arg for std::*fstream
#if defined(BOOST_WINDOWS)
    // string ctor
    {
        std::string name = filename;
        nw::ifstream fi(name);
        TEST(fi);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "test");
    }
    // string open
    {
        nw::ifstream fi;
        fi.open(std::string(filename));
        TEST(fi);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "test");
    }
#endif
    // Binary mode
    {
        nw::ifstream fi(filename, std::ios::binary);
        TEST(fi);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "test");
    }
    // At end
    {
        // Need binary file or position check might be throw off by newline conversion
        {
            nw::ofstream fo(filename, nw::fstream::binary);
            TEST(fo << "test");
        }
        nw::ifstream fi(filename, nw::fstream::ate | nw::fstream::binary);
        TEST(fi);
        TEST(fi.tellg() == std::streampos(4));
        fi.seekg(-2, std::ios_base::cur);
        std::string tmp;
        TEST(fi >> tmp);
        TEST(tmp == "st");
    }
    // Fail on non-existing file
    TEST(nw::remove(filename) == 0);
    {
        nw::ifstream fi(filename);
        TEST(!fi);
    }
}

void test_fstream(const char *filename)
{
    nw::remove(filename);
    TEST(!file_exists(filename));
    // Fail on non-existing file
    {
        nw::fstream f(filename);
        TEST(!f);
    }
    {
        nw::fstream f;
        f.open(filename);
        TEST(!f);
    }
    TEST(!file_exists(filename));
    // Create empty file (Ctor)
    {
        nw::fstream f(filename, std::ios::out);
        TEST(f);
    }
    TEST(file_contents_equal(filename, ""));
    // Ctor
    {
        nw::fstream f(filename);
        TEST(f);
        TEST(f << "test");
        std::string tmp;
        TEST(f.seekg(0));
        TEST(f >> tmp);
        TEST(tmp == "test");
    }
    TEST(file_contents_equal(filename, "test"));
    TEST(nw::remove(filename) == 0);
    // Create empty file (open)
    {
        nw::fstream f;
        f.open(filename, std::ios::out);
        TEST(f);
    }
    TEST(file_contents_equal(filename, ""));
    // Open
    {
        nw::fstream f;
        f.open(filename);
        TEST(f);
        TEST(f << "test");
        std::string tmp;
        TEST(f.seekg(0));
        TEST(f >> tmp);
        TEST(tmp == "test");
    }
    TEST(file_contents_equal(filename, "test"));
    // Ctor existing file
    {
        nw::fstream f(filename);
        TEST(f);
        std::string tmp;
        TEST(f >> tmp);
        TEST(tmp == "test");
        TEST(f.eof());
        f.clear();
        TEST(f << "second");
    }
    TEST(file_contents_equal(filename, "testsecond"));
    // Trunc & binary
    {
        nw::fstream f(filename, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        TEST(f);
        TEST(f << "test2");
        std::string tmp;
        TEST(f.seekg(0));
        TEST(f >> tmp);
        TEST(tmp == "test2");
    }
    TEST(file_contents_equal(filename, "test2"));
    // Reading in write mode fails (existing file!)
    {
        nw::fstream f(filename, std::ios::out);
        std::string tmp;
        TEST(!(f >> tmp));
        f.clear();
        TEST(f << "foo");
        TEST(f.seekg(0));
        TEST(!(f >> tmp));
    }
    TEST(file_contents_equal(filename, "foo"));
    // Writing in read mode fails (existing file!)
    {
        nw::fstream f(filename, std::ios::in);
        TEST(!(f << "bar"));
        f.clear();
        std::string tmp;
        TEST(f >> tmp);
        TEST(tmp == "foo");
    }
    TEST(file_contents_equal(filename, "foo"));
    TEST(nw::remove(filename) == 0);
}

int main(int argc, char **argv)
{
    const std::string exampleFilename = std::string(argv[0]) + "-\xd7\xa9-\xd0\xbc-\xce\xbd.txt";
    try
    {
        std::cout << "Testing fstream" << std::endl;
        test_ofstream_creates_file(exampleFilename.c_str());
        test_ofstream_write(exampleFilename.c_str());
        test_ifstream_open_read(exampleFilename.c_str());
        test_fstream(exampleFilename.c_str());

        std::cout << "Complex IO - Sanity Check" << std::endl;
        // Don't use chars the std stream can't properly handle
        test_with_different_buffer_sizes<std::fstream>((std::string(argv[0]) + "-bufferSize.txt").c_str());
        std::cout << "Complex IO - Test" << std::endl;
        test_with_different_buffer_sizes<nw::fstream>(exampleFilename.c_str());

        std::cout << "filebuf::close - Sanity Check" << std::endl;
        // Don't use chars the std stream can't properly handle
        test_close<std::filebuf>((std::string(argv[0]) + "-bufferSize.txt").c_str());
        std::cout << "filebuf::close - Test" << std::endl;
        test_close<nw::filebuf>(exampleFilename.c_str());

        std::cout << "Flush - Sanity Check" << std::endl;
        test_flush<std::ifstream, std::ofstream>(exampleFilename.c_str());
        std::cout << "Flush - Test" << std::endl;
        test_flush<nw::ifstream, nw::ofstream>(exampleFilename.c_str());

        if(argc == 2)
        {
            test_perf(argv[1]);
        }
    } catch(std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << "Ok" << std::endl;
    return 0;
}
