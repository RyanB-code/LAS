cmake_minimum_required(VERSION 3.22.1)

project(LAS VERSION 1.0.0)

set(CMAKE_CXX_STANDARD		23)
set(CMAKE_CXX_STANDARD_REQUIRED	True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # So clangd lsp can find compile commands
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})


set (LAS_SDK_NAME		    "LAS-SDK")
set (LAS_SDK_VERSION_MAJOR 	"0")
set (LAS_SDK_VERSION_MINOR 	"8")
set (LAS_SDK_VERSION_PATCH 	"2")

set (LAS_ENV_NAME	        "LAS")
set (LAS_ENV_VERSION_MAJOR 	"0")
set (LAS_ENV_VERSION_MINOR 	"9")
set (LAS_ENV_VERSION_PATCH 	"0")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(DEBUG "DEBUG" ON)
    set(CMAKE_BUILD_TYPE Debug)
endif()


add_subdirectory(SDK)
add_subdirectory(Environment)


