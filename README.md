# Boost.Nowide

Library for cross-platform, unicode aware programming.

The library provides an implementation of standard C and C++ library functions, such that their inputs are UTF-8 aware on Windows without requiring to use the Wide API.

### License

Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

### Properties

* C++17 support
* Usable outside of Boost via CMake
* Compiled library on every OS

### Requirements (All versions)

* C++17 (or higher) compatible compiler
    * MSVC 2019 and up work
    * libstdc++ < 5 is unsupported as it is silently lacking C++11 features

### Requirements (Boost version)

* Boost (>= 1.56)
* CMake

# Quickstart

Instead of using the standard library functions use the corresponding member of Boost.Nowide with the same name.
On Linux those are (mostly) aliases for the `std` ones, but on Windows they accept UTF-8 as input and use the wide API for the underlying functionality.

Examples:
- `std::ifstream -> boost::nowide::ifstream`
- `std::fopen -> boost::nowide::fopen`
- `std::fclose -> boost::nowide::fclose`
- `std::getenv -> boost::nowide::getenv`
- `std::putenv -> boost::nowide::putenv`
- `std::cout -> boost::nowide::cout`

To also convert your input arguments to UTF-8 on Windows use:

```
int main(int argc, char **argv)
{
    boost::nowide::args _(argc, argv); // Must use an instance!
    ...
}
```

See the [Documentation](https://www.boost.org/doc/libs/master/libs/nowide/index.html) for details.

# Compile

## With Boost

Compile and install the Boost super project the usual way via `./b2`.
The headers and library will then be available together with all other Boost libraries.
From within CMake you can then use `find_package(boost_nowide)` and link against `Boost::nowide`.

## With CMake

Boost.Nowide fully supports CMake.
So you can use `add_subdirectory("path-to-boost-nowide-repo")` and link your project against the target `Boost::nowide`.

You can also pre-compile and install Boost.Nowide via the usual workflow:
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

A CMake-Config file will be installed alongside Boost.Nowide so `find_package(boost_nowide)` does work out-of the box
(provided it was installed into a "standard" location or its `INSTALL_PREFIX` was added to `CMAKE_PREFIX_PATH`).

# Boost.Filesystem integration

Boost.Nowide integrates with Boost.Filesystem:
- Call `boost::nowide::nowide_filesystem()` to imbue UTF-8 into Boost.Filesystem (for use by `boost::filesystem::path`) such that narrow strings passed into Boost.Filesystem are treated as UTF-8 on Windows

### More information

* [Ask questions](http://stackoverflow.com/questions/ask?tags=c%2B%2B,boost,boost-nowide)
* [Report bugs](https://github.com/boostorg/nowide/issues): Be sure to mention Boost version, platform and compiler you're using. A small compilable code sample to reproduce the problem is always good as well.
* Submit your patches as pull requests against **develop** branch. Note that by submitting patches you agree to license your modifications under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).
* Discussions about the library are held on the [Boost developers mailing list](http://www.boost.org/community/groups.html#main). Be sure to read the [discussion policy](http://www.boost.org/community/policy.html) before posting and add the `[nowide]` tag at the beginning of the subject line.
