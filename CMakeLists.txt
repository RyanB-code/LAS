cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD		23)
set(CMAKE_CXX_STANDARD_REQUIRED	True)

project(LAS VERSION 1.0.0)

set (LAS_SDK_NAME		    "LAS-SDK")
set (LAS_SDK_VERSION_MAJOR 	"1")
set (LAS_SDK_VERSION_MINOR 	"0")
set (LAS_SDK_VERSION_PATCH 	"0")

set (LAS_ENV_NAME	        "LAS-ENV")
set (LAS_ENV_VERSION_MAJOR 	"1")
set (LAS_ENV_VERSION_MINOR 	"0")
set (LAS_ENV_VERSION_PATCH 	"0")


add_subdirectory(SDK)
add_subdirectory(Environment)


