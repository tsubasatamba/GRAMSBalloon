include_directories(
  include
  ../core/include
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${BOOST_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  ${MYSQL_INC_DIR}
)

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${MYSQL_LIB_DIR}
)

set(GB_MODULES
  src/SimpleLoop.cc
  src/ReceiveTelemetry.cc
  src/RunIDManager.cc
  src/ReadTelemetry.cc
  src/DumpSerial.cc
  src/GetArduinoData.cc
  src/EncodedSerialCommunicator.cc
  src/GetMHADCData.cc
  src/GetCompressorData.cc
  src/GetPressure.cc
  src/PressureGaugeManager.cc
  src/InterpretTelemetry.cc
  src/Sleep.cc
  src/MosquittoManager.cc
  src/ReceiveCommand.cc
  src/DistributeCommand.cc
  src/MeasureOrientationByMHADC.cc
  src/SocketCommunicationManager.cc
  src/ReceiveStatusFromDAQComputer.cc
  src/DividePacket.cc
  src/PassTelemetry.cc
  src/InterpretDAQFormattedTelemetry.cc
  src/IoContextManager.cc
  src/MeasureTemperatureWithRTDSensorByMHADC.cc
  src/SendCommandToDAQComputer.cc
  src/SendTelemetry.cc
)

if(GB_DEMO_MODE)
  set(GB_DEMO_MODULES
    ../demo/src/GBBasicDemoModule.cc
    ../demo/src/MeasureTemperatureWithRTDSensorDemo.cc
    ../demo/src/ShutdownSystemDemo.cc
    ../demo/src/GetRaspiStatusDemo.cc
    ../demo/src/MeasureTemperatureWithRTDSensorByArduino.cc
  )
  list(APPEND GB_MODULES
    src/ReceiveCommand.cc
    src/SendTelemetry.cc
  )
  include_directories(../demo/include)
endif(GB_DEMO_MODE)


if(GB_USE_RASPISYS)
  set(GB_RASPISYS_MODULES
    src/GetRaspiStatus.cc
    src/ShutdownSystem.cc
  )
endif(GB_USE_RASPISYS)

if(GB_USE_HSQUICKLOOK)
  set(GB_HSQUICKLOOK_MODULES
    src/PushToMongoDB.cc
  )
endif(GB_USE_HSQUICKLOOK)

if(GB_USE_MYSQL)
  set(GB_MYSQL_MODULES
    src/PushToMySQL.cc
  )
endif(GB_USE_MYSQL)

add_library(GBModules SHARED
  ${GB_MODULES}
  ${GB_RASPISYS_MODULES}
  ${GB_HSQUICKLOOK_MODULES}
  ${GB_ROOT_MODULES}
  ${GB_DEMO_MODULES}
  ${GB_MYSQL_MODULES}
  ${GB_MOSQUITTO_MODULES}
)
target_link_libraries(GBModules
  GBCore
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${MONGODB_LIB}
  ${HSQUICKLOOK_LIB}
  ${MYSQL_LIB}
  ${MOSQUITTO_LIB}
)

target_compile_definitions(GBModules PUBLIC ${cxx_definitions})

install(TARGETS GBModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")

  if(GB_DEMO_MODE)
    install(DIRECTORY ../demo/include/
      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
      FILES_MATCHING PATTERN "*.hh")
  endif(GB_DEMO_MODE)
endif(INSTALL_HEADERS)
