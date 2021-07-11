set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${lib6502_SOURCE_DIR}/include)
add_library(lib6502 CPU_6502.c)