
# Recursevily search subdirs for all .cpp files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Makes the library with the sources
add_library(${LAS_SDK_NAME} SHARED "${sources}")

if (DEBUG)
    target_compile_definitions(${LAS_SDK_NAME} PRIVATE DEBUG)
    set_target_properties(${LAS_SDK_NAME} PROPERTIES OUTPUT_NAME "${LAS_SDK_NAME}-dev")
endif()

# Make the config file
configure_file(src/Config.h.in "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h")

# Let the compiler know where the header files are
# The PRIVATE specifier means the headers are only used for the specified target
target_include_directories(${LAS_SDK_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Make variable with all public headers
file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS "include/*.h")

set_target_properties(${LAS_SDK_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}") # Declares the public API of the library
set_target_properties(${LAS_SDK_NAME} PROPERTIES VERSION ${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH})    # Set version of LAS-SDK
set_target_properties(${LAS_SDK_NAME} PROPERTIES SOVERSION ${LAS_SDK_VERSION_MAJOR}) # Sets the symlink of libNAME.so.X to the full name
set_target_properties(${LAS_SDK_NAME} PROPERTIES    LIBRARY_OUTPUT_DIRECTORY "/tmp")    # Set directory to output the SO, NEEDS to be NOT a NAS to work 

add_custom_command( TARGET ${LAS_SDK_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f           "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h"  # Remove temporary file
)


if(DEBUG)
    add_custom_command( TARGET ${LAS_SDK_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_CURRENT_SOURCE_DIR}/include"   "${CMAKE_SOURCE_DIR}/bin/SDK/Debug/include"   # Copy to bin/SDK

        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}-dev.so"                               "${CMAKE_SOURCE_DIR}/bin/SDK/Debug"
        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}-dev.so.${LAS_SDK_VERSION_MAJOR}"      "${CMAKE_SOURCE_DIR}/bin/SDK/Debug"
        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}-dev.so.${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH}"      "${CMAKE_SOURCE_DIR}/bin/SDK/Debug"

        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/bin/SDK/Debug/lib${LAS_SDK_NAME}-dev.so"        "${CMAKE_SOURCE_DIR}/Environment/lib"   # Copy built SO to Environment/lib

        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}-dev.so"
        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}-dev.so.${LAS_SDK_VERSION_MAJOR}"
        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}-dev.so.${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH}"

    )
else()
    add_custom_command( TARGET ${LAS_SDK_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_CURRENT_SOURCE_DIR}/include"   "${CMAKE_SOURCE_DIR}/bin/SDK/Release/include"   # Copy to bin/SDK

        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}.so"                               "${CMAKE_SOURCE_DIR}/bin/SDK/Release"
        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}.so.${LAS_SDK_VERSION_MAJOR}"      "${CMAKE_SOURCE_DIR}/bin/SDK/Release"
        COMMAND ${CMAKE_COMMAND} -E copy "/tmp/lib${LAS_SDK_NAME}.so.${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH}"      "${CMAKE_SOURCE_DIR}/bin/SDK/Release"

        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/bin/SDK/Release/lib${LAS_SDK_NAME}.so"        "${CMAKE_SOURCE_DIR}/Environment/lib"   # Copy built SO to Environment/lib

        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}.so"
        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}.so.${LAS_SDK_VERSION_MAJOR}"
        COMMAND ${CMAKE_COMMAND} -E rm  -f "/tmp/lib${LAS_SDK_NAME}.so.${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH}"

    )
endif()
