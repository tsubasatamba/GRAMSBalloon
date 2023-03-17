cmake_minimum_required(VERSION 3.0)

include_directories(
  include
  ../BME68x-Sensor-API/include
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  )

link_directories(
  ${BOOST_LIB_DIR}
  )

set(GB_CORE_CLASSES
  src/BME680IO.cc
  src/SerialCommunication.cc
  src/AnalogDiscoveryIO.cc
  src/MAX31865IO.cc
)

add_library(GBCore SHARED
  ${GB_CORE_CLASSES}
  )

target_link_libraries(GBCore
  GBBME68x
  ${ADD_LIB}
  ${BOOST_LIB}
  pigpiod_if2
  rt
  dwf
  )

install(TARGETS GBCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
