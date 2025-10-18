cmake_minimum_required(VERSION 3.15...3.26)
cmake_policy(SET CMP0144 NEW)
cmake_policy(SET CMP0167 NEW)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Set module path before any find_package calls
# Turn off PCRE tests and utilities to avoid conflicts
set(BUILD_PCRE_INTERNAL ON CACHE BOOL "Use internal PCRE" FORCE)
set(PCRE_BUILD_TESTS OFF CACHE BOOL "Build PCRE tests" FORCE)
set(PCRE_BUILD_PCREGREP OFF CACHE BOOL "Build pcregrep" FORCE)
set(PCRE_SUPPORT_UTF8 ON CACHE BOOL "Enable UTF-8 support in PCRE" FORCE)
set(PCRE_SUPPORT_UNICODE_PROPERTIES ON CACHE BOOL "Enable Unicode properties support in PCRE")

include(GNUInstallDirs)
include(ExternalProject)
include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
  # default to Release
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

if("$ENV{AUDITWHEEL_PLAT}" MATCHES "musllinux")
  # use clang(++) on musllinux
  set(CMAKE_CXX_COMPILER clang++)
  set(CMAKE_C_COMPILER clang)
endif()

# versions
set(BOOST_VERSION 1.87.0)
set(PCRE_VERSION 8.45)
set(RAGEL_VERSION 6.10)
set(RAGEL_REPO https://github.com/adrian-thurston/ragel.git)

# filename version converts dots to underscores, e.g. 1.87.0 -> 1_87_0
string(REPLACE "." "_" BOOST_FILENAME_VERSION ${BOOST_VERSION})

# configure hyperscan with ragel path
set(USE_VECTORSCAN TRUE)

# find_package(RAGEL 6.9 MODULE)
set(hyperscan_PREFIX_DIR ${CMAKE_BINARY_DIR}/libhs-prefix)
set(hyperscan_SOURCE_DIR ${hyperscan_PREFIX_DIR}/src)
set(hyperscan_BINARY_DIR ${hyperscan_PREFIX_DIR}/build)
set(hyperscan_VENDOR_DIR ${hyperscan_PREFIX_DIR}/vendor)
set(hyperscan_STAMP_DIR ${hyperscan_BINARY_DIR}/stamp)

# Python extension
set(HS_EXT_NAME _hs_ext)
find_package(
  Python REQUIRED
  COMPONENTS Interpreter Development.Module ${SKBUILD_SABI_COMPONENT}
)
python_add_library(
  ${HS_EXT_NAME} MODULE src/hyperscan/extension.c WITH_SOABI
)

# Force C++ linker for the extension since we link against C++ libraries
set_target_properties(${HS_EXT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(NOT HS_BUILD_LIB_ROOT OR NOT HS_SRC_ROOT)
  message(STATUS "Building Hyperscan from source")
  set(HS_BUILD_LIB_ROOT ${hyperscan_BINARY_DIR}/lib)
  set(HS_SRC_ROOT ${hyperscan_SOURCE_DIR})
  set(HS_BUILD_REQUIRED TRUE)

  # configure Hyperscan/Vectorscan source repo and version
  if(WIN32)
    # Ensure we're using MSVC on Windows
    if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
      message(FATAL_ERROR "On Windows, only MSVC/Visual Studio generators are supported for building Python extensions")
    endif()

    set(USE_VECTORSCAN FALSE)
    set(HYPERSCAN_VERSION 5.4.2)
    set(HYPERSCAN_TAG v5.4.2)
    set(HYPERSCAN_REPO https://github.com/intel/hyperscan.git)
    message(STATUS "Using Hyperscan ${HYPERSCAN_VERSION} from ${HYPERSCAN_REPO}")
  else()
    set(HYPERSCAN_VERSION 5.4.12)
    set(HYPERSCAN_TAG vectorscan/5.4.12)
    set(HYPERSCAN_REPO https://github.com/VectorCamp/vectorscan.git)
    message(STATUS "Using VectorScan ${HYPERSCAN_VERSION} from ${HYPERSCAN_REPO}")
  endif()
else()
  set(HS_BUILD_REQUIRED FALSE)
endif()

set(HS_LIBS
  hs
  hs_runtime
  chimera
  pcre)

set(HS_BUILD_BYPRODUCTS)

foreach(lib ${HS_LIBS})
  add_library(${lib} STATIC IMPORTED)

  if(WIN32)
    set(object_name "${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set(object_name "lib${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  endif()

  set(object_path "${HS_BUILD_LIB_ROOT}/${object_name}")

  list(APPEND HS_BUILD_BYPRODUCTS "${object_path}")

  if(NOT HS_BUILD_REQUIRED AND NOT EXISTS "${object_path}")
    message(FATAL_ERROR "${object_name} not found at ${HS_BUILD_LIB_ROOT}")
  endif()

  set_target_properties(${lib} PROPERTIES
    IMPORTED_LOCATION "${object_path}")
endforeach()

function(hs_locate_ragel)
  set(options)
  set(oneValueArgs VERSION)
  set(multiValueArgs)
  cmake_parse_arguments(HS_R "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  find_program(_ragel_executable NAMES ragel)

  if(NOT _ragel_executable)
    set(RAGEL_FOUND FALSE PARENT_SCOPE)
    return()
  endif()

  set(_ragel_version "")
  execute_process(
    COMMAND ${_ragel_executable} --version
    OUTPUT_VARIABLE _ragel_stdout
    ERROR_VARIABLE _ragel_stderr
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _ragel_result)

  if(_ragel_result EQUAL 0)
    string(REGEX MATCH "([0-9]+\\.[0-9]+(\\.[0-9]+)?)" _ragel_version "${_ragel_stdout}")
  endif()

  if(NOT _ragel_version)
    set(_ragel_version "0.0")
  endif()

  if(HS_R_VERSION AND _ragel_version VERSION_LESS HS_R_VERSION)
    message(STATUS "Found ragel ${_ragel_version} at ${_ragel_executable} but ${HS_R_VERSION}+ is required")
    set(RAGEL_FOUND FALSE PARENT_SCOPE)
    return()
  endif()

  set(RAGEL_EXECUTABLE "${_ragel_executable}" PARENT_SCOPE)
  set(RAGEL_VERSION "${_ragel_version}" PARENT_SCOPE)
  set(RAGEL_FOUND TRUE PARENT_SCOPE)
endfunction()

if(HS_BUILD_REQUIRED)
  hs_locate_ragel(VERSION "6.9")

  if(NOT RAGEL_FOUND)
    # Build and install Ragel if necessary
    if(WIN32)
      # if Windows, expect MSYS2 to be installed at C:/msys64 or abort
      # this should be the case on Windows Server GitHub Actions runners
      # simply use the MSYS2/MinGW package for ragel since we just need
      # the binary
      if(NOT EXISTS C:/msys64)
        message(FATAL_ERROR "MSYS2 not found at C:/msys64")
      endif()

      set(BASH_PATH C:/msys64/usr/bin/bash.exe)
      execute_process(
        COMMAND ${BASH_PATH} -c "/usr/bin/pacman -Syuu --noconfirm"
        RESULT_VARIABLE MSYS2_UPDATE_RESULT
      )

      if(MSYS2_UPDATE_RESULT)
        message(FATAL_ERROR "Failed to update MSYS2 packages")
      endif()

      execute_process(
        COMMAND ${BASH_PATH} -c "/usr/bin/pacman -S --noconfirm mingw-w64-x86_64-ragel"
        RESULT_VARIABLE MSYS2_RAGEL_INSTALL_RESULT
      )

      if(MSYS2_RAGEL_INSTALL_RESULT)
        message(FATAL_ERROR "Failed to install ragel")
      endif()

      set(RAGEL_EXECUTABLE C:/msys64/mingw64/bin/ragel.exe CACHE PATH "Ragel executable" FORCE)
      set(RAGEL ${RAGEL_EXECUTABLE})
      set(RAGEL_FOUND TRUE)
      message(STATUS "Ragel executable: ${RAGEL_EXECUTABLE}")
    else()
      # prerequisites (in addition to a build toolchain): autoconf, kelbt
      find_program(AUTORECONF autoreconf REQUIRED)
      find_program(KELBT kelbt REQUIRED)
      ExternalProject_Add(
        ragel
        GIT_REPOSITORY ${RAGEL_REPO}
        GIT_TAG ragel-${RAGEL_VERSION}
        BUILD_IN_SOURCE TRUE
        CONFIGURE_COMMAND ${AUTORECONF} -f -i
        COMMAND ./configure --prefix=${CMAKE_BINARY_DIR} --disable-manual
        BUILD_COMMAND make -j4
        INSTALL_COMMAND ""
      )
      set(RAGEL_EXECUTABLE ${ragel_BINARY_DIR}/bin/ragel CACHE PATH "Ragel executable" FORCE)
      set(RAGEL_VERSION ${RAGEL_VERSION})
      set(RAGEL_FOUND TRUE)
      set(RAGEL_FOUND TRUE)
      message(STATUS "Ragel executable: ${RAGEL_EXECUTABLE}")
      add_custom_target(ragel COMMAND ${RAGEL_EXECUTABLE} -V)
    endif()
  else()
    set(RAGEL_EXECUTABLE ${RAGEL_EXECUTABLE})
    add_custom_target(ragel COMMAND ${RAGEL_EXECUTABLE} -V)
    message(STATUS "Ragel executable: ${RAGEL_EXECUTABLE}")
  endif()

  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # Ensure downstream projects opt into modern policy behavior with new CMake
  set(_cmake_policy_version "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
  set(HS_CMAKE_CACHE_ARGS
    -DCMAKE_POLICY_VERSION:STRING=${_cmake_policy_version}
    -DCMAKE_POLICY_VERSION_MINIMUM:STRING=3.5
  )

  # Hyperscan/Vectorscan platform-specific build configuration
  if(WIN32)
    # Ensure consistent MSVC runtime flags
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

    set(HS_CMAKE_COMMON_FLAGS "/arch:SSE2 /FS /GS-")

    if(NOT CMAKE_BUILD_PARALLEL_LEVEL)
      set(CMAKE_BUILD_PARALLEL_LEVEL 2)
    endif()

    set(HS_CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${HS_CMAKE_COMMON_FLAGS}")
    set(HS_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HS_CMAKE_COMMON_FLAGS}")
    set(HS_CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

    if(NOT ${CMAKE_GENERATOR} MATCHES "^Visual Studio")
      set(HS_GENERATOR "Visual Studio 17 2022")

      # Ensure x64 architecture
      set(HS_CMAKE_ARGS ${HS_CMAKE_ARGS} -A x64)
    else()
      set(HS_GENERATOR ${CMAKE_GENERATOR})
    endif()
  else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    
    # Architecture-specific compiler flags
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64|arm64)")
      # ARM architecture - use conservative flags with SIMDE_BACKEND
      set(HS_CMAKE_COMMON_FLAGS "-fPIC")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|X86|amd64|AMD64|x86_64|i[3-6]86)")
      # x86/x86_64 architecture - use compatible x86-64 baseline
      set(HS_CMAKE_COMMON_FLAGS "-march=x86-64 -fPIC")
    else()
      # Other architectures - rely on SIMDE_BACKEND for portability
      set(HS_CMAKE_COMMON_FLAGS "-fPIC")
    endif()
    
    
    set(HS_CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${HS_CMAKE_COMMON_FLAGS}")
    set(HS_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HS_CMAKE_COMMON_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")
    
    # Add clang warning suppression for vectorscan builds
    if(USE_VECTORSCAN AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER MATCHES "[Cc]lang.*"))
      set(HS_CMAKE_CXX_FLAGS "${HS_CMAKE_CXX_FLAGS} -Wno-unqualified-std-cast-call")
    endif()
    
    set(HS_CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    set(HS_GENERATOR "Unix Makefiles")
  endif()

  include_directories(${hyperscan_SOURCE_DIR}/src)
  include_directories(${hyperscan_SOURCE_DIR}/chimera)

  if(USE_VECTORSCAN)
    include_directories(${hyperscan_PREFIX_DIR})
  endif()

  link_directories(${hyperscan_BINARY_DIR})
  link_directories(${hyperscan_BINARY_DIR}/lib)
  file(
    DOWNLOAD https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_FILENAME_VERSION}.tar.gz
    ${hyperscan_VENDOR_DIR}/boost.tar.gz
    EXPECTED_HASH SHA256=f55c340aa49763b1925ccf02b2e83f35fdcf634c9d5164a2acb87540173c741d
  )

  if(EXISTS ${hyperscan_VENDOR_DIR}/boost)
    file(REMOVE_RECURSE ${hyperscan_VENDOR_DIR}/boost)
  endif()

  if(EXISTS ${hyperscan_VENDOR_DIR}/boost_${BOOST_FILENAME_VERSION})
    file(REMOVE_RECURSE ${hyperscan_VENDOR_DIR}/boost_${BOOST_FILENAME_VERSION})
  endif()

  file(
    ARCHIVE_EXTRACT INPUT ${hyperscan_VENDOR_DIR}/boost.tar.gz DESTINATION ${hyperscan_VENDOR_DIR} PATTERNS "boost_${BOOST_FILENAME_VERSION}/boost/*"
  )
  file(RENAME ${hyperscan_VENDOR_DIR}/boost_${BOOST_FILENAME_VERSION} ${hyperscan_VENDOR_DIR}/boost)
  message(STATUS "Boost downloaded to ${hyperscan_VENDOR_DIR}/boost")

  if(NOT USE_VECTORSCAN)
    # patch boost config.hpp to fix __has_cpp_attribute check
    file(READ "${hyperscan_VENDOR_DIR}/boost/boost/system/detail/config.hpp" BOOST_CONFIG_CONTENT)
    string(REPLACE
      "#if defined(__has_cpp_attribute)"
      "#if defined(__clang__) && defined(__has_cpp_attribute)"
      BOOST_CONFIG_CONTENT "${BOOST_CONFIG_CONTENT}")
    file(WRITE "${hyperscan_VENDOR_DIR}/boost/boost/system/detail/config.hpp" "${BOOST_CONFIG_CONTENT}")
  endif()

  if(EXISTS ${HS_SRC_ROOT}/boost)
    file(REMOVE_RECURSE ${HS_SRC_ROOT}/boost)
  endif()

  if(EXISTS ${HS_SRC_ROOT}/pcre)
    file(REMOVE_RECURSE ${HS_SRC_ROOT}/pcre)
  endif()

  message(STATUS "Downloading PCRE ${PCRE_VERSION}")
  file(
    DOWNLOAD https://sourceforge.net/projects/pcre/files/pcre/${PCRE_VERSION}/pcre-${PCRE_VERSION}.tar.bz2
    ${hyperscan_VENDOR_DIR}/pcre-${PCRE_VERSION}.tar.bz2
    EXPECTED_HASH SHA256=4dae6fdcd2bb0bb6c37b5f97c33c2be954da743985369cddac3546e3218bffb8
  )

  if(EXISTS ${hyperscan_VENDOR_DIR}/pcre)
    file(REMOVE_RECURSE ${hyperscan_VENDOR_DIR}/pcre)
  endif()

  file(
    ARCHIVE_EXTRACT INPUT ${hyperscan_VENDOR_DIR}/pcre-${PCRE_VERSION}.tar.bz2 DESTINATION ${hyperscan_VENDOR_DIR} PATTERNS "pcre-${PCRE_VERSION}/*"
  )
  file(RENAME ${hyperscan_VENDOR_DIR}/pcre-${PCRE_VERSION} ${hyperscan_VENDOR_DIR}/pcre)
  message(STATUS "PCRE downloaded to ${hyperscan_VENDOR_DIR}/pcre")

  # Patch PCRE CMakeLists.txt to update CMAKE_MINIMUM_REQUIRED
  set(PCRE_CMAKE_FILE ${hyperscan_VENDOR_DIR}/pcre/CMakeLists.txt)

  if(EXISTS ${PCRE_CMAKE_FILE})
    file(READ ${PCRE_CMAKE_FILE} _pcre_cmake_content_orig)
    set(_pcre_cmake_content_current "${_pcre_cmake_content_orig}")
    set(_any_patch_applied FALSE)

    # --- Patch 1: CMAKE_MINIMUM_REQUIRED ---
    message(STATUS "Attempting PCRE Patch 1: CMAKE_MINIMUM_REQUIRED")
    set(MIN_REQ_REGEX_TO_MATCH "(cmake_minimum_required|CMAKE_MINIMUM_REQUIRED)[^\n]*")
    set(MIN_REQ_REPLACEMENT_LINE "CMAKE_MINIMUM_REQUIRED(VERSION 3.15)")
    set(_pcre_content_before_patch1 "${_pcre_cmake_content_current}")
    string(REGEX REPLACE "${MIN_REQ_REGEX_TO_MATCH}" "${MIN_REQ_REPLACEMENT_LINE}" _pcre_cmake_content_current "${_pcre_cmake_content_current}")

    if(NOT "${_pcre_cmake_content_current}" STREQUAL "${_pcre_content_before_patch1}")
      set(_any_patch_applied TRUE)
    endif()

    # --- Patch 2: CMAKE_POLICY CMP0026 ---
    set(CMP0026_REGEX_TO_MATCH "(cmake_policy|CMAKE_POLICY)\\s*\\([^\\n]*CMP0026[^\\n]*OLD[^\\n]*\\)")
    set(CMP0026_REPLACEMENT "CMAKE_POLICY(SET CMP0026 NEW)")
    set(_pcre_content_before_patch2 "${_pcre_cmake_content_current}")
    string(REGEX REPLACE "${CMP0026_REGEX_TO_MATCH}" "${CMP0026_REPLACEMENT}" _pcre_cmake_content_current "${_pcre_cmake_content_current}")

    if(NOT "${_pcre_cmake_content_current}" STREQUAL "${_pcre_content_before_patch2}")
      set(_any_patch_applied TRUE)
    endif()

    # --- Patch 3: GET_TARGET_PROPERTY pcretest DEBUG_LOCATION ---
    set(GTP_REGEX_TO_MATCH "GET_TARGET_PROPERTY\\(PCRETEST_EXE pcretest DEBUG_LOCATION\\)")
    set(GTP_REPLACEMENT "set(PCRETEST_EXE $<TARGET_FILE:pcretest>)")
    set(_pcre_content_before_patch3 "${_pcre_cmake_content_current}")
    string(REGEX REPLACE "${GTP_REGEX_TO_MATCH}" "${GTP_REPLACEMENT}" _pcre_cmake_content_current "${_pcre_cmake_content_current}")

    if(NOT "${_pcre_cmake_content_current}" STREQUAL "${_pcre_content_before_patch3}")
      set(_any_patch_applied TRUE)
    endif()

    # --- Patch 4: Force PCRE UTF-8 Support ---
    message(STATUS "Attempting PCRE Patch 4: Force UTF-8 Support")
    # Find the OPTION commands and force UTF-8 to ON
    string(REGEX REPLACE "OPTION\\(PCRE_SUPPORT_UTF \"Enable support for the Unicode UTF-8 encoding\\.\" OFF\\)" 
                         "OPTION(PCRE_SUPPORT_UTF \"Enable support for the Unicode UTF-8 encoding.\" ON)" 
                         _pcre_cmake_content_current "${_pcre_cmake_content_current}")
    string(REGEX REPLACE "OPTION\\(PCRE_SUPPORT_UNICODE_PROPERTIES \"Enable support for Unicode properties\\.\" OFF\\)"
                         "OPTION(PCRE_SUPPORT_UNICODE_PROPERTIES \"Enable support for Unicode properties.\" ON)"
                         _pcre_cmake_content_current "${_pcre_cmake_content_current}")
    # Also handle cases where it might be PCRE_SUPPORT_UTF8
    string(REGEX REPLACE "OPTION\\(PCRE_SUPPORT_UTF8 \"Enable support for the Unicode UTF-8 encoding\\.\" OFF\\)" 
                         "OPTION(PCRE_SUPPORT_UTF8 \"Enable support for the Unicode UTF-8 encoding.\" ON)" 
                         _pcre_cmake_content_current "${_pcre_cmake_content_current}")
    set(_any_patch_applied TRUE)

    if(_any_patch_applied)
      file(WRITE ${PCRE_CMAKE_FILE} "${_pcre_cmake_content_current}")
      message(STATUS "Final patched PCRE CMakeLists.txt written to ${PCRE_CMAKE_FILE} as changes were applied.")
    else()
      message(STATUS "No changes made to PCRE CMakeLists.txt after attempting all patches, file not rewritten.")
    endif()
  else()
    message(WARNING "PCRE CMakeLists.txt not found at ${PCRE_CMAKE_FILE}, skipping all patches.")
  endif()

  # Force PCRE UTF-8 settings globally for all platforms
  set(PCRE_SUPPORT_UTF8 ON CACHE BOOL "Enable UTF-8 support in PCRE" FORCE)
  set(PCRE_SUPPORT_UNICODE_PROPERTIES ON CACHE BOOL "Enable Unicode properties support in PCRE" FORCE)
  
  set(
    HS_CMAKE_ARGS
    -DBOOST_USE_STATIC_LIBS=ON
    -DBUILD_STATIC_LIBS=ON
    -DBUILD_SHARED_LIBS=OFF
    -DFAT_RUNTIME=OFF
    -DCORRECT_PCRE_VERSION=YES
    -DPCRE_SOURCE=${hyperscan_VENDOR_DIR}/pcre
    -DPCRE_SUPPORT_UTF8=ON
    -DPCRE_SUPPORT_UNICODE_PROPERTIES=ON
    -DPCRE_VERSION=${PCRE_VERSION}
    -DPCRE_STATIC=ON
    -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_FLAGS=${HS_CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${HS_CMAKE_CXX_FLAGS}
    -DARCH_C_FLAGS=${HS_CMAKE_C_FLAGS}
    -DARCH_CXX_FLAGS=${HS_CMAKE_CXX_FLAGS}
    -DCMAKE_EXE_LINKER_FLAGS=${HS_CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
  )

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${HS_CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HS_CMAKE_CXX_FLAGS}")

  if(USE_VECTORSCAN)
    list(
      APPEND HS_CMAKE_ARGS
      -DSIMDE_BACKEND=ON
      -DRAGEL=${RAGEL_EXECUTABLE}
      -DPCRE_BUILD_SOURCE=ON
      -DBUILD_STATIC_LIBS=ON
      -DBUILD_EXAMPLES=OFF
      -DBUILD_BENCHMARKS=OFF
      -DBOOST_ROOT=${hyperscan_VENDOR_DIR}/boost
      -DUSE_CPU_NATIVE=OFF
      # Explicitly ensure PCRE UTF-8 support in vectorscan builds
      -DPCRE_SUPPORT_UTF8=ON
      -DPCRE_SUPPORT_UNICODE_PROPERTIES=ON
    )
    set(HS_TARGETS --target hs --target hs_runtime --target chimera --target pcre)
  else()
    list(
      APPEND HS_CMAKE_ARGS 
      -DRAGEL=${RAGEL_EXECUTABLE} 
      -DBOOST_ROOT=${hyperscan_VENDOR_DIR}/boost
      # Explicitly ensure PCRE UTF-8 support in hyperscan builds
      -DPCRE_SUPPORT_UTF8=ON
      -DPCRE_SUPPORT_UNICODE_PROPERTIES=ON
    )
    set(HS_TARGETS --target hs --target hs_runtime --target chimera --target pcre)
  endif()

  ExternalProject_Add(
    libhs
    GIT_REPOSITORY ${HYPERSCAN_REPO}
    GIT_TAG ${HYPERSCAN_TAG}
    GIT_SHALLOW TRUE
    GIT_PROGRESS ON
    SYSTEM
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_DIR ${hyperscan_SOURCE_DIR}
    BINARY_DIR ${hyperscan_BINARY_DIR}
    STAMP_DIR ${hyperscan_STAMP_DIR}
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${HS_GENERATOR}
    CMAKE_ARGS ${HS_CMAKE_ARGS} -Wno-dev
    CMAKE_CACHE_ARGS ${HS_CMAKE_CACHE_ARGS}
    BUILD_BYPRODUCTS ${HS_BUILD_BYPRODUCTS}
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release --parallel 4 ${HS_TARGETS}
  )
  add_dependencies(${HS_EXT_NAME} libhs)
else()
  message(STATUS "Using pre-built Hyperscan")
  find_library(HS NAMES hs libhs PATHS ${HS_BUILD_LIB_ROOT})
  find_library(CHIMERA NAMES chimera libchimera PATHS ${HS_BUILD_LIB_ROOT})
  find_library(PCRE NAMES pcre libpcre PATHS ${HS_BUILD_LIB_ROOT})
endif()

set_target_properties(${HS_EXT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${HS_EXT_NAME} PUBLIC ${Python_INCLUDE_DIRS})
target_include_directories(${HS_EXT_NAME} PRIVATE ${HS_SRC_ROOT}/src)
target_include_directories(${HS_EXT_NAME} PRIVATE ${HS_SRC_ROOT}/chimera)

if(USE_VECTORSCAN)
  target_include_directories(${HS_EXT_NAME} PRIVATE ${HS_SRC_ROOT})
  target_include_directories(${HS_EXT_NAME} PRIVATE ${hyperscan_BINARY_DIR})
  target_include_directories(${HS_EXT_NAME} PRIVATE ${HS_BUILD_LIB_ROOT})
endif()

if(WIN32)
  target_compile_options(${HS_EXT_NAME} PRIVATE ${HS_CMAKE_COMMON_FLAGS})
else()
  target_compile_options(${HS_EXT_NAME} PRIVATE -fPIC -D_GLIBCXX_USE_CXX11_ABI=1)
  target_link_options(${HS_EXT_NAME} PRIVATE -O0)
endif()

target_link_libraries(${HS_EXT_NAME} PRIVATE Python::Module)

if("$ENV{AUDITWHEEL_PLAT}" MATCHES "musllinux")
  target_link_options(${HS_EXT_NAME} PRIVATE
    -Wl,-s
    -Wl,--gc-sections
    -static-libgcc
    -Wl,--whole-archive
    -l:libstdc++.a
    -Wl,--no-whole-archive
  )
  target_link_libraries(${HS_EXT_NAME} PRIVATE ${HS_LIBS})
  target_compile_options(${HS_EXT_NAME} PRIVATE -fPIC)
elseif(APPLE)
  # macOS specific linking
  target_link_libraries(${HS_EXT_NAME} PRIVATE ${HS_LIBS})
  target_link_libraries(${HS_EXT_NAME} PRIVATE c++)
elseif(NOT WIN32)
  # Linux specific linking
  if("$ENV{AUDITWHEEL_PLAT}" MATCHES "manylinux2014")
    target_link_options(${HS_EXT_NAME} PRIVATE
      -Wl,--no-as-needed
      -Wl,--copy-dt-needed-entries
      -Wl,--no-allow-shlib-undefined
    )
    target_link_libraries(${HS_EXT_NAME} PRIVATE ${HS_LIBS})
    target_link_libraries(${HS_EXT_NAME} PRIVATE -Wl,--push-state -Wl,-Bstatic -lstdc++ -Wl,--pop-state)
  else()
    # Other Linux platforms
    target_link_options(${HS_EXT_NAME} PRIVATE -Wl,--no-as-needed)
    target_link_libraries(${HS_EXT_NAME} PRIVATE ${HS_LIBS})
    target_link_libraries(${HS_EXT_NAME} PRIVATE stdc++)
  endif()
else()
  # Windows
  target_link_libraries(${HS_EXT_NAME} PRIVATE ${HS_LIBS})
endif()

install(
  TARGETS ${HS_EXT_NAME}
  LIBRARY
  DESTINATION hyperscan
  COMPONENT hyperscan
)
add_custom_target(hs_python_extension DEPENDS ${HS_EXT_NAME})
