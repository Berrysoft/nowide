#include <boost/nowide/fstream.hpp>
#include <boost/nowide/cstdio.hpp>
#define BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "test.hpp"

#ifdef BOOST_MSVC
#  pragma warning(disable : 4996)
#endif

namespace nw = boost::nowide;
template<typename FStream>
class io_fstream {
public:
    void open(char const *file)
    {
        f_.open(file,std::fstream::out | std::fstream::in | std::fstream::trunc);
        TEST(f_);
    }
    void write(char *buf,int size)
    {
        f_.write(buf,size);
    }
    void read(char *buf,int size)
    {
        f_.read(buf,size);
    }
    void rewind()
    {
        f_.seekg(0);
        f_.seekp(0);
    }
    void flush()
    {
        f_<<std::flush;
    }
    void close()
    {
        f_.close();
    }
private:
    FStream f_;
};

class io_stdio {
public:
    void open(char const *file)
    {
        f_ = fopen(file,"w+");
        TEST(f_);
    }
    void write(char *buf,int size)
    {
        fwrite(buf,1,size,f_);
    }
    void read(char *buf,int size)
    {
        fread(buf,1,size,f_);
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
        f_=0;
    }
private:
    FILE *f_;
};


template<typename FStream>
void test_io(char *file,char const *type)
{
    std::cout << "Testing I/O performance " << type << std::endl;
    FStream tmp;
    tmp.open(file);
    int data_size = 64*1024*1024;
    for(int block_size = 16;block_size <= 8192;block_size*=2) {
        std::vector<char> buf(block_size,' ');
        int size = 0;
        tmp.rewind();
        boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();
        while(size < data_size) {
            tmp.write(&buf[0],block_size);
            size += block_size;
        }
        tmp.flush(); 
        boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
        double tm = boost::chrono::duration_cast<boost::chrono::milliseconds>(t2-t1).count() * 1e-3;
        // heatup
        if(block_size >= 32)
            std::cout << "  write block size " << std::setw(8) << block_size << " " << std::fixed << std::setprecision(3) << (data_size / 1024.0 / 1024 / tm) << " MB/s" << std::endl;
    }
    for(int block_size = 32;block_size <= 8192;block_size*=2) {
        std::vector<char> buf(block_size,' ');
        int size = 0;
        tmp.rewind();
        boost::chrono::high_resolution_clock::time_point t1 = boost::chrono::high_resolution_clock::now();
        while(size < data_size) {
            tmp.read(&buf[0],block_size);
            size += block_size;
        } 
        boost::chrono::high_resolution_clock::time_point t2 = boost::chrono::high_resolution_clock::now();
        double tm = boost::chrono::duration_cast<boost::chrono::milliseconds>(t2-t1).count() *1e-3;
        std::cout << "   read block size " << std::setw(8) << block_size << " " << std::fixed << std::setprecision(3) << (data_size / 1024.0 / 1024 / tm) << " MB/s" << std::endl;
    }
    tmp.close();
    std::remove(file);
}


void test_perf(char *file)
{
    test_io<io_stdio>(file,"stdio");
    test_io<io_fstream<std::fstream> >(file,"std::fstream");
    test_io<io_fstream<boost::nowide::fstream> >(file,"nowide::fstream");
}

void makeEmptyFile(const char* filepath){
    nw::ofstream f(filepath, std::ios_base::out | std::ios::trunc);
    TEST(f);
}

int main(int argc,char **argv)
{
    
    char const *example = "\xd7\xa9-\xd0\xbc-\xce\xbd" ".txt";
#ifdef BOOST_WINDOWS
    wchar_t const *wexample = L"\u05e9-\u043c-\u03bd.txt";
#endif    

    try {
        
        std::cout << "Testing fstream" << std::endl;
        {
            nw::ofstream fo;
            fo.open(example);
            TEST(fo);
            fo<<"test"<<std::endl;
            fo.close();
            #ifdef BOOST_WINDOWS
            {
                FILE *tmp=_wfopen(wexample,L"r");
                TEST(tmp);
                TEST(fgetc(tmp)=='t');
                TEST(fgetc(tmp)=='e');
                TEST(fgetc(tmp)=='s');
                TEST(fgetc(tmp)=='t');
                TEST(fgetc(tmp)=='\n');
                TEST(fgetc(tmp)==EOF);
                fclose(tmp);
            }
            #endif
            {
                nw::ifstream fi;
                fi.open(example);
                TEST(fi);
                std::string tmp;
                fi  >> tmp;
                TEST(tmp=="test");
                fi.close();
            }
            {
                nw::ifstream fi(example);
                TEST(fi);
                std::string tmp;
                fi  >> tmp;
                TEST(tmp=="test");
                fi.close();
            }
            #if defined(BOOST_WINDOWS)
            // C++11 interfaces aren't enabled at all platforms so need to skip
            // for std::*fstream
            {
                std::string name = example;
                nw::ifstream fi(name);
                TEST(fi);
                std::string tmp;
                fi  >> tmp;
                TEST(tmp=="test");
                fi.close();
            }
            {
                nw::ifstream fi;
                fi.open(std::string(example));
                TEST(fi);
                std::string tmp;
                fi  >> tmp;
                TEST(tmp=="test");
                fi.close();
            }
            #endif
            {
                nw::ifstream fi(example,std::ios::binary);
                TEST(fi);
                std::string tmp;
                fi  >> tmp;
                TEST(tmp=="test");
                fi.close();
            }

            {
                nw::ifstream fi;
                nw::remove(example);
                fi.open(example);
                TEST(!fi);
            }
            {
                // Create with default flags
                makeEmptyFile(example);
                nw::fstream f(example);
                TEST(f);
                f << "test2";
                std::string tmp;
                f.seekg(0);
                f >> tmp;
                TEST(tmp == "test2");
                f.close();
            }
            {
                // Open with default flags
                makeEmptyFile(example);
                nw::fstream f;
                f.open(example);
                TEST(f);
                f << "test2";
                std::string tmp;
                f.seekg(0);
                f >> tmp;
                TEST(tmp == "test2");
                f.close();
            }
            {
                nw::fstream f(example, nw::fstream::in | nw::fstream::out | nw::fstream::trunc | nw::fstream::binary);
                TEST(f);
                f << "test2";
                std::string tmp;
                f.seekg(0);
                f >> tmp;
                TEST(tmp == "test2");
                f.close();
            }
            {
                nw::ifstream fi(example,nw::fstream::ate | nw::fstream::binary);
                TEST(fi);
                TEST(fi.tellg()==std::streampos(5));
                fi.seekg(-2,std::ios_base::cur);
                std::string tmp;
                fi >> tmp;
                TEST(tmp == "t2");
                fi.close();
            }
            nw::remove(example);
        }
        
        for(int i=-1;i<16;i++) {
            std::cout << "Complex io with buffer = " << i << std::endl;
            char buf[16];
            nw::fstream f;
            if(i==0)
                f.rdbuf()->pubsetbuf(0,0);
            else if (i > 0) 
                f.rdbuf()->pubsetbuf(buf,i);
            
            f.open(example,nw::fstream::in | nw::fstream::out | nw::fstream::trunc | nw::fstream::binary);
            f.put('a');
            f.put('b');
            f.put('c');
            f.put('d');
            f.put('e');
            f.put('f');
            f.put('g');
            f.seekg(0);
            TEST(f.get()=='a');
            f.seekg(1,std::ios::cur);
            TEST(f.get()=='c');
            f.seekg(-1,std::ios::cur);
            TEST(f.get()=='c');
            TEST(f.seekg(1));
            f.put('B');
            TEST(f.get()=='c');
            TEST(f.seekg(1));
            TEST(f.get() == 'B');
            f.seekg(2);
            f.put('C');
            TEST(f.get()=='d');
            f.seekg(0);
            TEST(f.get()=='a');
            TEST(f.get()=='B');
            TEST(f.get()=='C');
            TEST(f.get()=='d');
            TEST(f.get()=='e');
            TEST(f.putback('e'));
            TEST(f.putback('d'));
            TEST(f.get()=='d');
            TEST(f.get()=='e');
            TEST(f.get()=='f');
            TEST(f.get()=='g');
            TEST(f.get()==EOF);
            f.clear();
            f.seekg(1);
            TEST(f.get()=='B');
            TEST(f.putback('B'));
            TEST(f.putback('a'));
            TEST(!f.putback('x'));
            f.close();
            TEST(boost::nowide::remove(example)==0);
            
        }
        {
            makeEmptyFile(example);
            const std::string filename2 = "test.txt";
            // Make sure file does not exist yet
            boost::nowide::remove(filename2.c_str());
            TEST(boost::nowide::fopen(filename2.c_str(), "r") == NULL);
            boost::nowide::filebuf buf;
            TEST(buf.open(example, std::ios_base::out) == &buf);
            TEST(buf.is_open());
            // Opening when already open fails
            TEST(buf.open(filename2, std::ios_base::out) == NULL);
            // Still open
            TEST(buf.is_open());
            TEST(buf.close() == &buf);
            // Failed opening did not create file
            TEST(boost::nowide::fopen(filename2.c_str(), "r") == NULL);
            // But it should work now:
            TEST(buf.open(filename2, std::ios_base::out) == &buf);
            TEST(buf.close() == &buf);
            FILE* f = boost::nowide::fopen(filename2.c_str(), "r");
            TEST(f != NULL);
            std::fclose(f);
            TEST(boost::nowide::remove(example) == 0);
            TEST(boost::nowide::remove(filename2.c_str()) == 0);
        }
        if(argc==2) {
            test_perf(argv[1]);
        }
    }
    catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << "Ok" << std::endl;
    return 0;

}
