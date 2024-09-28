# The PRIVATE specifier means the headers are only used for the specified target
target_include_directories(${LAS_SDK_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Make variable with all public headers
file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS "*.h")