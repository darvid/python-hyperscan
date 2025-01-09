#!/bin/sh

set -e

# Default Visual Studio version and year
DEFAULT_VS_VERSION=17
DEFAULT_VS_YEAR=2022

# Parse arguments for MSBuild path, Visual Studio version, and year
if [ -z "$1" ]; then
  read -p "Enter the full path to MSBuild.exe: " msbuildpath
else
  msbuildpath=$1
fi

vs_version=${2:-$DEFAULT_VS_VERSION}
vs_year=${3:-$DEFAULT_VS_YEAR}

# Set MSBuild path
export MSBUILD_PATH="${msbuildpath}"
echo -e "\nMSBUILD_PATH has been set to: [${MSBUILD_PATH}]"
echo ""

# Configuration
export CPU_ARCH=x64
export PLATFORM="Visual Studio ${vs_version} ${vs_year}"

# Third-party libraries
export BOOST_VER=1_78_0
export BOOST_VER2=1.78.0
export RAGEL_VER=6.10
export PCRE_VER=8.41
export HYPERSCAN_VER=5.4.0
export HYPERSCAN_ZIP="v${HYPERSCAN_VER}.zip"

# Download URLs
BOOST_DOWNLOAD_URL="https://archives.boost.io/release/${BOOST_VER2}/source/boost_${BOOST_VER}.zip"
RAGEL_DOWNLOAD_URL="http://www.colm.net/files/ragel/ragel-${RAGEL_VER}.tar.gz"
PCRE_DOWNLOAD_URL="https://sourceforge.net/projects/pcre/files/pcre/${PCRE_VER}/pcre-${PCRE_VER}.tar.bz2"
CMAKE_DOWNLOAD_URL="https://github.com/Kitware/CMake/releases/download/v3.21.0/cmake-3.21.0-windows-x86_64.zip"
SQLITE_DOWNLOAD_URL="https://www.sqlite.org/2018/sqlite-amalgamation-3260000.zip"
HYPERSCAN_DOWNLOAD_URL="https://github.com/intel/hyperscan/archive/refs/tags/${HYPERSCAN_ZIP}"

# Verify MSBuild path
if [ ! -f "${MSBUILD_PATH}/MSBuild.exe" ]; then
  echo "[${MSBUILD_PATH}] is not a valid path to MSBuild.exe. Please set the correct path."
  exit 1
fi
export PATH=$PATH:"${MSBUILD_PATH}"

# Check required utilities
echo -e "\nVerifying required utilities..."
for util in python curl git unzip tar make gcc; do
  echo "Checking $util..."
  if ! which "$util" > /dev/null 2>&1; then
    echo "Error: $util is not installed. Please install it and try again."
    exit 1
  fi
done

# Download helper function
download() {
  if [ ! -e "$(basename "$1")" ]; then
    echo "Downloading: $1"
    curl -L -O "$1"
  fi
}

# Download third-party libraries
echo -e "\nDownloading third-party libraries..."
download "${HYPERSCAN_DOWNLOAD_URL}"
unzip -q "${HYPERSCAN_ZIP}"
mv hyperscan-${HYPERSCAN_VER} hyperscan

download "${BOOST_DOWNLOAD_URL}"
download "${RAGEL_DOWNLOAD_URL}"
download "${PCRE_DOWNLOAD_URL}"
download "${CMAKE_DOWNLOAD_URL}"
download "${SQLITE_DOWNLOAD_URL}"

# Extract libraries
echo -e "\nExtracting third-party libraries..."
tar zxf ragel-${RAGEL_VER}.tar.gz
mv ragel-${RAGEL_VER} hyperscan/

unzip -q cmake-3.21.0-windows-x86_64.zip
mv cmake-3.21.0-windows-x86_64 hyperscan/

unzip -q sqlite-amalgamation-3260000.zip
mv sqlite-amalgamation-3260000 hyperscan/sqlite3

tar jxf pcre-${PCRE_VER}.tar.bz2
mv pcre-${PCRE_VER} hyperscan/

unzip boost_${BOOST_VER}.zip
mv boost_${BOOST_VER} hyperscan/

# Build process
cd hyperscan
echo -e "\nBuilding and installing Ragel..."
pushd ragel-${RAGEL_VER}
./configure
make
make install
popd

echo -e "\nBuilding Hyperscan..."
mkdir -p build
cd build
CXXFLAGS="/MP /FS" CFLAGS="/MP /FS" ../cmake-3.21.0-windows-x86_64/bin/cmake -G "$PLATFORM" -DBOOST_ROOT=boost_${BOOST_VER} ../

echo -e "\nRelease Build..."
MsBuild.exe ALL_BUILD.vcxproj /t:build /p:Configuration=Release
mkdir -p lib/${CPU_ARCH}.Release
mv lib/*.lib lib/${CPU_ARCH}.Release/

echo -e "\nDebug Build..."
MsBuild.exe ALL_BUILD.vcxproj /t:build /p:Configuration=Debug
mkdir -p lib/${CPU_ARCH}.Debug
mv lib/*.lib lib/${CPU_ARCH}.Debug/
mv lib/*.pdb lib/${CPU_ARCH}.Debug/

echo -e "\nBuild completed. Current directory: $(pwd)"
