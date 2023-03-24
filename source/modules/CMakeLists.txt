include_directories(
  include
  ../core/include
  ../BME68x-Sensor-API/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  #${PIGPIO_LIB_DIR}
  )

set(GB_MODULES
  src/SimpleLoop.cc
  src/SPIManager.cc
  src/GetEnvironmentalData.cc
  src/ReceiveCommand.cc
  src/AnalogDiscoveryManager.cc
  src/RegulateHighVoltage.cc
  src/MeasureTemperatureWithRTDSensor.cc
  src/ReadDAQ.cc
  src/SendTelemetry.cc
  )

add_library(GBModules SHARED
  ${GB_MODULES}
  )

target_link_libraries(GBModules
  GBCore
  GBBME68x
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${G4_LIB}
  pigpiod_if2
  rt
  dwf
  )

install(TARGETS GBModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
