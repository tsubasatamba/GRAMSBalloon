cmake_minimum_required(VERSION 3.0)
#CMAKE_MINIMUM_REQUIRED_VERSION(VERSION 3.14)

### Initial definition of cmake variables
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
# set(CMAKE_BUILD_TYPE Release CACHE STRING "build type")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "build type")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "CXX_FLAGS for debug")
set(CMAKE_C_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "C_FLAGS for debug")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "CXX_FLAGS for release")
set(CMAKE_C_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "C_FLAGS for release")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

### Definition of project
project(GRAMSBalloon CXX C)
set(PROJECT_VERSION 1.0)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(USE_RUBY ON)
set(MY_LIBRARY GRAMSBalloon)
#set(RASPI ON)

find_package(Ruby 2.7 REQUIRED)

### options
## library options
option(GB_USE_PIGPIO "enable PIGPIO" ON)
option(GB_USE_WAVEFORMS "enable waveforms")
## install options
option(GB_INSTALL_HEADERS "install all header files" ON)
option(GB_INSTALL_CMAKE_FILES "install all cmake files" ON)
set(INSTALL_HEADERS ${GB_INSTALL_HEADERS})
set(INSTALL_CMAKE_FILES ${GB_INSTALL_CMAKE_FILES})
## shortcut options
option(GB_MAC "shortcut" OFF)

if (GB_MAC)
  set(GB_USE_PIGPIO OFF)
  set(GB_USE_WAVEFORMS OFF)
endif(GB_MAC)

### External libraries
### BOOST ###
find_package(Boost 1.60.0 REQUIRED COMPONENTS system chrono thread)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")

#ANL
if(NOT DEFINED ANLNEXT_INSTALL)
  if(DEFINED ENV{ANLNEXT_INSTALL})
    set(ANLNEXT_INSTALL $ENV{ANLNEXT_INSTALL})
  else()
    set(ANLNEXT_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED ANLNEXT_INSTALL)
set(ANLNEXT_INC_DIR ${ANLNEXT_INSTALL}/include)
set(ANLNEXT_LIB_DIR ${ANLNEXT_INSTALL}/lib)
set(ANLNEXT_LIB ANLNext)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ANLNEXT_LIB_DIR}/anlnext)
message("-- ANLNEXT_INSTALL = ${ANLNEXT_INSTALL}")

### PIGPIO ###
if(GB_USE_PIGPIO)
  set(PIGPIO_INSTALL_PREFIX /home/grams/software/pigpio/util)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PIGPIO_INSTALL_PREFIX})
  find_package(pigpio REQUIRED)
  set(PIGPIO_INC_DIR ${pigpio_INCLUDE_DIRS})
  set(PIGPIO_LIB_DIR ${pigpio_LIBRARY_DIRS})
  set(PIGPIO_LIB ${pigpio_LIBRARIES})
  message("-- PIGPIO_INC_DIR: ${PIGPIO_INC_DIR}")
  message("-- PIGPIO_LIB_DIR: ${PIGPIO_LIB_DIR}")
  message("-- PIGPIO_LIB: ${PIGPIO_LIB}")
  set(PIGPIO_LIB_DAEMON pigpiod_if2)
  message("-- PIGPIO_LIB_DAEMON: ${PIGPIO_LIB_DAEMON}")
  set(PIGPIO_LIB_RT rt)
  message("-- PIGPIO_LIB_RT: ${PIGPIO_LIB_RT}")
  set(BME68x_LIB GBBME68x)
endif(GB_USE_PIGPIO)

### Waveforms ###
if(GB_USE_WAVEFORMS)
  set(WAVEFORMS_LIB dwf)
  message("-- WAVEFORMS_LIB: ${WAVEFORMS_LIB}")
endif(GB_USE_WAVEFORMS)

### subdirectories
add_subdirectory(source)
if(USE_RUBY)
  add_subdirectory(rubyext)
endif(USE_RUBY)


