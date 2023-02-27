set(ANL_MODULES
    command.cc
    telemetry.cc
    )

add_library(${MY_LIBRARY} SHARED ${ANL_MODULES})
target_link_libraries(
    ${MY_LIBRARY}
    ${ANLNEXT_LIB}
)


target_include_directories(
    ${MY_LIBRALY}
    PUBLIC
    ${ANL_NEXT_INC_DIR}
    include
)
install(TARGETS ${MY_LIBRARY} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(${USE_RUBY})
    add_subdirectory(rubyext)
endif(${USE_RUBY})