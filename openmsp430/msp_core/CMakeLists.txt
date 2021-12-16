message(STATUS "Generating Verilog config.v")
configure_file(vrased-config.v.in ${CMAKE_CURRENT_SOURCE_DIR}/vrased-config.v)
