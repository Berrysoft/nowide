# Boost.Nowide

Library for cross-platform, unicode aware programming.

The library provides an implementation of standard C and C++ library functions, such that their inputs are UTF-8 aware on Windows without requiring to use the Wide API.

### License

Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

### Properties

* C++17 support
* Usable via CMake
* Compiled library only on Windows

### Requirements (All versions)

* C++17 (or higher) compatible compiler
    * MSVC 2019 and up work
    * libstdc++ < 9 is unsupported as it is silently lacking C++17 features

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

### More information

* [Original repo](https://github.com/boostorg/nowide)
