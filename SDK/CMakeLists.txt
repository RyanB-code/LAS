
# Makes include and src dirs known
add_subdirectory(src)
add_subdirectory(include)

set_target_properties(${LAS_SDK_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}") # Declares the public API of the library

configure_file(src/Config.h.in "${CMAKE_CURRENT_BINARY_DIR}/Config.h")

target_include_directories(${LAS_SDK_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
