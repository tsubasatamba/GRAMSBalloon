########################################################
set(TARGET_EXT_LIBRARY GRAMSBalloon)
set(EXT_LIB_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ../source/modules/include
  ../source/core/include
  ../source/BME68x-Sensor-API/include
  ${ANLNEXT_INC_DIR}
  ${BOOST_INC_DIR}
  )
set(RUBY_EXT_LIBRARY_DIRS
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ADD_LIB_DIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  )
set(RUBY_EXT_LIBRARIES
  ${ANLNEXT_LIB}
  ${BME68x_LIB}
  GBCore
  GBModules
  ${PIGPIO_LIB_DAEMON}
  ${PIGPIO_LIB_RT}
  ${WAVEFORMS_LIB}
)


########################################################
set(cxx_definitions )
if(GB_USE_PIGPIO)
  list(APPEND cxx_definitions -DUSE_PIGPIO)
endif()
if(CS_USE_WAVEFORMS)
  list(APPEND cxx_definitions -DUSE_WAVEFORMS)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigRuby)
