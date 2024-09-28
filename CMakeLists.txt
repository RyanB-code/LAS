cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD		23)
set(CMAKE_CXX_STANDARD_REQUIRED	True)

project(LAS VERSION 1.0.0)

set (LAS_SDK_NAME		"LAS-SDK")
set (LAS_SDK_VERSION_MAJOR 	"1")
set (LAS_SDK_VERSION_MINOR 	"0")
set (LAS_SDK_VERSION_PATCH 	"0")

set (LAS_ENV_NAME	"LAS.exe")

add_subdirectory(SDK)

set_target_properties(${LAS_SDK_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/SDK") # Set directory to output the SO
set_target_properties(${LAS_SDK_NAME} PROPERTIES VERSION ${LAS_SDK_VERSION_MAJOR}.${LAS_SDK_VERSION_MINOR}.${LAS_SDK_VERSION_PATCH})    # Set version of LAS-SDK
set_target_properties(${LAS_SDK_NAME} PROPERTIES SOVERSION 1) # Sets the symlink of libNAME.so.X to the full name
