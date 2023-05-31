CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(GRAMSBalloonCommandSender CXX)
set(PROJECT_VERSION 1.0)
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

### boost
find_package(Boost 1.60.0 REQUIRED COMPONENTS system chrono thread)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")

add_executable(send_command send_command.cc CommandSender.cc CommandBuilder.cc SaveCommand.cc)
target_link_libraries(send_command ${BOOST_LIB})
add_executable(dump_command dump_command.cc CommandBuilder.cc)
