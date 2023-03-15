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
  ./interface
  )
set(RUBY_EXT_LIBRARY_DIRS
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ADD_LIB_DIR}
  ${CMAKE_INSTALL_PREFIX}/lib
  )
set(RUBY_EXT_LIBRARIES
  ${ANLNEXT_LIB}
  GBBME68x
  GBCore
  GBModules
  pigpiod_if2
  rt
  dwf
)


########################################################
set(cxx_definitions )
if(CS_USE_SIMX)
  list(APPEND cxx_definitions -DUSE_SIMX)
endif()
if(CS_USE_FITSIO)
  list(APPEND cxx_definitions -DUSE_FITSIO)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigRuby)
