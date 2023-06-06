####### CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

### Initial definition of cmake variables
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
set(CMAKE_BUILD_TYPE Release CACHE STRING "build type")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "CXX_FLAGS for debug")
set(CMAKE_C_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "C_FLAGS for debug")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "CXX_FLAGS for release")
set(CMAKE_C_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "C_FLAGS for release")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

### Definition of project
project(display_telemetry)
add_definitions("-std=c++17")

### ROOT ###
set(ROOTSYS $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH ${ROOTSYS})
find_package(ROOT REQUIRED)
set(ROOT_INC_DIR ${ROOT_INCLUDE_DIRS})
set(ROOT_LIB_DIR ${ROOT_LIBRARY_DIR})
set(ROOT_LIB ${ROOT_LIBRARIES})
#list(APPEND CMAKE_INSTALL_RPATH ${ROOT_LIBRARY_DIR})
message("-- ROOTSYS = ${ROOTSYS}")
message("-- ROOT_INC_DIR = ${ROOT_INC_DIR}")
message("-- ROOT_LIB_DIR = ${ROOT_LIB_DIR}")
message("-- ROOT libraries = ${ROOT_LIB}")

include_directories(
  include
  $ENV{HOME}/include
  /opt/homebrew/include
  ${ROOT_INC_DIR}

  )

link_directories(
  ${ROOT_LIB_DIR}
   $ENV{HOME}/lib
  )

set(TARGET make_telemetry_root)

add_executable(${TARGET}
  make_telemetry_root.cc
  )

target_link_libraries(${TARGET}
  ${ROOT_LIB}
  libGBCore.dylib
  )
