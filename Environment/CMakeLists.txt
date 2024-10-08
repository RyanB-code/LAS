# Recursevily search subdirs for all .cpp files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Set the runtime path for the EXE to look for libs in the specific folder
if(DEBUG)
    set(CMAKE_BUILD_RPATH "${CMAKE_SOURCE_DIR}/bin/Environment/Debug")
endif()

# Add sources to EXE
add_executable(${LAS_ENV_NAME} EXCLUDE_FROM_ALL "${sources}")


# Setup config file with versioning
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h")

# Add headers
target_include_directories(${LAS_ENV_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")   # Adds headers include
target_include_directories(${LAS_ENV_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")       # Adds headers from src

# Find OpenGL shared objects
find_package(OpenGL REQUIRED)

# Specifiy libs
target_link_libraries(${LAS_ENV_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a")
target_link_libraries(${LAS_ENV_NAME} PRIVATE ${OPENGL_LIBRARIES})



# Pre build commands
if(DEBUG)
    add_custom_command( TARGET ${LAS_ENV_NAME} PRE_BUILD 
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/bin/Environment/Debug/*.exe"   # Remove any old .exe
    )
else()
    add_custom_command( TARGET ${LAS_ENV_NAME} PRE_BUILD 
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/bin/Environment/Release/*.exe" # Remove any old .exe
    )
endif()


# Post build commands
add_custom_command( TARGET ${LAS_ENV_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h"      # Remove temporary file 
    COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/SDK/include/Config.h"          # Remove temporary file from other target
)

# Conditional commands
if(DEBUG)
    set_target_properties(${LAS_ENV_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Environment/Debug")      # Set output directory
    set_target_properties(${LAS_ENV_NAME} PROPERTIES OUTPUT_NAME "${LAS_ENV_NAME}-dev.exe")                                     # Set name

    target_link_libraries(${LAS_ENV_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libLAS-SDK-dev.so")                          # Specify debug lib

    target_compile_definitions  (${LAS_ENV_NAME} PRIVATE DEBUG)                                                                 # Basically #define DEBUG for the source code

    add_custom_command( TARGET ${LAS_ENV_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_SOURCE_DIR}/bin/SDK/Debug"                     "${CMAKE_SOURCE_DIR}/bin/Environment/Debug"     # Copy all debug libs to Env/Debug to the LAS-ENV-dev.exe can use it
        
        COMMAND ${CMAKE_COMMAND} -E rm -rf  "${CMAKE_SOURCE_DIR}/bin/Environment/Debug/include"         # Remove include dir thats not needed
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/bin/Environment/Debug/*.so"            # Remove the un-versioned LAS-SDK lib
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/bin/Environment/Debug/*[0-9].[0-9]"    # Remove the fully versioned LAS-SDK lib 
    )
else()
    set_target_properties(${LAS_ENV_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Environment/Release")    # Set output directory
    set_target_properties(${LAS_ENV_NAME} PROPERTIES OUTPUT_NAME "${LAS_ENV_NAME}.exe")                                         # Set name

    target_link_libraries(${LAS_ENV_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libLAS-SDK.so")                              # Specify release lib
endif()

