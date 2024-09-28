
# Makes include and src dirs known
add_subdirectory(src)
add_subdirectory(include)

configure_file(src/Config.h.in "${CMAKE_CURRENT_BINARY_DIR}/Config.h")

target_include_directories(${LAS_SDK_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(${LAS_SDK_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}") # Declares the public API of the library
set_target_properties(${LAS_SDK_NAME} PROPERTIES VERSION ${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH})    # Set version of LAS-SDK
set_target_properties(${LAS_SDK_NAME} PROPERTIES SOVERSION 1) # Sets the symlink of libNAME.so.X to the full name

set_target_properties(${LAS_SDK_NAME} PROPERTIES    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/SDK" # Set directory to output the SO
                                                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/SDK" # Set archive dir
                                                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/SDK" # Set runtime dir
                    )   
if(TARGET ${LAS_SDK_NAME} )
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/include" DESTINATION "${CMAKE_SOURCE_DIR}/bin/SDK/")
    file(GLOB_RECURSE TXT_FILES  "${CMAKE_SOURCE_DIR}/bin/SDK/*.txt")
    file(REMOVE "${TXT_FILES}")
endif()