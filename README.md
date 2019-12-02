# Boost.Nowide

![](https://github.com/Flamefire/nowide/workflows/CI%20Tests/badge.svg?branch=master)
[![Build status](https://ci.appveyor.com/api/projects/status/p5tt7txqmq8h2qb1/branch/master?svg=true)](https://ci.appveyor.com/project/Flamefire/nowide/branch/master)


Library for cross-platform, unicode aware programming.

The library provides an implementation of standard C and C++ library functions, such that their inputs are UTF-8 aware on Windows without requiring to use Wide API.

# Quickstart

Instead of using the standard library functions use the corresponding member of Boost.Nowide with the same name.
On Linux those are aliases for the `std` ones, but on Windows they accept UTF-8 as input and use the wide API for the underlying functionality.

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

See the [Documentation](http://cppcms.com/files/nowide/html/) for details.

# Compile

Boost.Nowide fully supports CMake.
So you can use `add_subdirectory("path-to-boost-nowide-repo")` and link your project against the target `nowide::nowide`.

You can also pre-compile and install Boost.Nowide via the usual workflow:
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```
A CMake-Config file will be installed alongside Boost.Nowide so `find_package(nowide)` does work out-of the box
(provided it was installed into a "standard" location or its `INSTALL_PREFIX` was added to `CMAKE_PREFIX_PATH`).

Note that only on Windows there is an actual library to be linked.
On Linux the target is a simple `INTERFACE` target with no actual linking done.

# Boost.Filesystem integration

Boost.Nowide cleanly integrates with Boost.Filesystem:
- Call `boost::nowide::nowide_filesystem()` to imbue UTF-8 into Boost.Filesystem (for use by `boost::filesystem::path`)
- If `NOWIDE_USE_FILESYSTEM` (in CMake) is set, then `boost::nowide::[io]fstream` redirect to `boost::filesystem::[io]fstream` (Linux) or provide overloads accepting a `boost::filesystem::path` (Windows).

# Standalone version

There is also a standalone version that does not require Boost at all.
Simply replace `boost::nowide::foo` by `nowide::foo` in your using projects.