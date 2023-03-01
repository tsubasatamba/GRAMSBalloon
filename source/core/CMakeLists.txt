cmake_minimum_required(VERSION 3.0)

include_directories(include
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  )

link_directories(
  ${BOOST_LIB_DIR}
  )

set(GB_CORE_CLASSES
  src/bme68x.c
)

add_library(GBCore SHARED
  ${GB_CORE_CLASSES}
  )

target_link_libraries(GBCore
  ${ADD_LIB}
  ${BOOST_LIB}
  pigpiod_if2
  rt)

install(TARGETS GBCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
