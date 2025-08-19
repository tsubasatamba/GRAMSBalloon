include_directories(
  include
  ../BME68x-Sensor-API/include
  ../ICM20948-linux-i2c/src
  ../ICM20948-linux-i2c/I2C/src
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  ${PIGPIO_INC_DIR}
  ${MYSQL_INC_DIR}
  ${RAPIDJSON_INC_DIR}
  )

link_directories(
  ${BOOST_LIB_DIR}
  ${MYSQL_LIB_DIR}
  ${RAPIDJSON_LIB_DIR}
  )

set(GB_CORE_CLASSES
  src/SerialCommunication.cc
  src/TelemetryDefinition.cc
  src/CommunicationFormat.cc
  src/CRC16.cc
  src/ErrorManager.cc
  src/DateManager.cc
  src/EncodedSerialCommunication.cc
  src/OrientationInformation.cc
  src/MQTTFormatConverter.cc
  src/BaseTelemetryDefinition.cc
  )

if(GB_USE_PIGPIO)
  set(GB_PIGPIO_CORE_CLASSES
    src/BME680IO.cc
    src/MAX31865IO.cc
    src/SlowADCIO.cc
    )
endif(GB_USE_PIGPIO)

if(GB_USE_WAVEFORMS)
  set(GB_WAVEFORMS_CORE_CLASSES
    src/AnalogDiscoveryIO.cc
    src/DAQIO.cc
    )
endif(GB_USE_WAVEFORMS)

if(GB_USE_ICM20948)
  set(GB_ICM20948_CORE_CLASSES
    src/ICM20948IO.cc
  )
endif(GB_USE_ICM20948)

if(GB_USE_MYSQL)
  set(GB_MYSQL_CORE_CLASSES
    src/MySQLIO.cc
    )
endif(GB_USE_MYSQL)
set(GB_MOSQUITTO_CORE_CLASSES
    src/MosquittoIO.cc
  )
if (GB_SOCKET_CLIENT)
  set(GB_SOCKET_CLIENT_CORE_CLASSES
    src/SocketCommunicationClient.cc
  )
else(GB_SOCKET_CLIENT)
  set(GB_SOCKET_SERVER_CORE_CLASSES
    src/SocketCommunicationServer.cc
  )
endif(GB_SOCKET_CLIENT)

add_library(GBCore SHARED
  ${GB_CORE_CLASSES}
  ${GB_PIGPIO_CORE_CLASSES}
  ${GB_WAVEFORMS_CORE_CLASSES}
  ${GB_ICM20948_CORE_CLASSES}
  ${GB_MYSQL_CORE_CLASSES}
  ${GB_MOSQUITTO_CORE_CLASSES}
  ${GB_SOCKET_CLIENT_CORE_CLASSES}
  ${GB_SOCKET_SERVER_CORE_CLASSES}
  )

target_link_libraries(GBCore
  ${BME68x_LIB}
  ${ADD_LIB}
  ${BOOST_LIB}
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
  ${ICM20948_LIB}
  ${MYSQL_LIB}
  ${MOSQUITTO_LIB}
  ${RAPIDJSON_LIB}
  )
target_compile_definitions(GBCore PUBLIC ${cxx_definitions})

install(TARGETS GBCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GRAMSBalloon
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
