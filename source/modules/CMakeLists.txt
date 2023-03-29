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
  src/GetRaspiStatus.cc
  )

if(GB_USE_PIGPIO)
  set(GB_PIGPIO_MODULES
    src/SPIManager.cc
    src/GetEnvironmentalData.cc
    src/MeasureTemperatureWithRTDSensor.cc
    )
endif(GB_USE_PIGPIO)

if(GB_USE_WAVEFORMS)
  set(GB_WAVEFORMS_MODULES
    src/AnalogDiscoveryManager.cc
    src/ControlHighVoltage.cc
    src/ReadWaveform.cc
    src/ReceiveCommand.cc
    src/SendTelemetry.cc
    )
endif(GB_USE_WAVEFORMS)

add_library(GBModules SHARED
  ${GB_MODULES}
  ${GB_PIGPIO_MODULES}
  ${GB_WAVEFORMS_MODULES}
  )

target_link_libraries(GBModules
  GBCore
  ${BME68x_LIB}
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${G4_LIB}
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  )

install(TARGETS GBModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
