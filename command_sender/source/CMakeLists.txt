CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

project(GRAMSBalloonCommandSender CXX)
set(PROJECT_VERSION 1.0)
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(send_command send_command.cc CommandSender.cc CommandBuilder.cc)
add_executable(dump_command dump_command.cc CommandBuilder.cc)
