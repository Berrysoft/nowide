#!/bin/bash

set -euo pipefail

cd "$(dirname "$0")/.."

if ! [ -e "tools/create_standalone.sh" ]; then
  echo "Could not change to repo root"
  exit 1
fi

targetFolder="${1:-nowide_standalone}"

# If target folder exists fail, unless it is the default in which case it is removed
if [ -e "$targetFolder" ]; then
  if [[ "$targetFolder" == "nowide_standalone" ]]; then
    rm -r "$targetFolder"
  else
    echo "Target folder $targetFolder exists"
    exit 1
  fi
fi

mkdir -p "$targetFolder"/include/nowide
mkdir -p "$targetFolder"/src
mkdir -p "$targetFolder"/test

cp include/boost/nowide/*.hpp "$targetFolder"/include/nowide
cp src/*.cpp "$targetFolder"/src
cp test/*.hpp test/*.cpp "$targetFolder"/test

SOURCES="$targetFolder/test/* $targetFolder/src/* $targetFolder/include/nowide/*"

sed 's/BOOST_NOWIDE_/NOWIDE_/g' -i $SOURCES
sed 's/BOOST_/NOWIDE_/g' -i $SOURCES
sed 's/boost::nowide::/nowide::/g' -i $SOURCES
sed 's/boost::nowide/nowide/g' -i $SOURCES
sed 's/boost::locale::/nowide::/g' -i $SOURCES
sed 's/boost::chrono::/std::chrono::/g' -i $SOURCES
sed 's/boost::/nowide::/g' -i $SOURCES
sed 's/namespace boost {//' -i $SOURCES
sed 's/} *\/\/ *namespace boost//' -i $SOURCES
sed 's/<boost\/locale\//<nowide\//g' -i $SOURCES
sed 's/<boost\/chrono.hpp/<chrono/g' -i $SOURCES
sed 's/<boost\/nowide\//<nowide\//g' -i $SOURCES
sed 's/<boost\//<nowide\//g' -i $SOURCES

cp standalone/*.hpp "$targetFolder"/include/nowide
cp CMakeLists.txt Config.cmake.in "$targetFolder"
cp test/CMakeLists.txt "$targetFolder"/test
