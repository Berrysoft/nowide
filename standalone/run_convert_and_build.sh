#!/bin/bash
export WINEPATH="/usr/lib/gcc/i686-w64-mingw32/5.3-win32/;/usr/lib/gcc/i686-w64-mingw32/5"

rm -fr /tmp/nwlin /tmp/nw

pushd .
./convert && mkdir nowide_standalone/build && cd nowide_standalone/build && cmake -DCMAKE_TOOLCHAIN_FILE=MinGW.cmake -DCMAKE_INSTALL_PREFIX=/tmp/nw -DRUN_WITH_WINE=ON .. && make && make test && make install
popd 

pushd .
./convert && mkdir nowide_standalone/build && cd nowide_standalone/build && cmake -DCMAKE_INSTALL_PREFIX=/tmp/nwlin .. && make && make test && make install
popd





