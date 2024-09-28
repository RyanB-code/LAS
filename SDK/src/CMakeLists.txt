
# Recursevily search subdirs for all .cpp files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")

# Makes the library
add_library(${LAS_SDK_NAME} SHARED "${sources}")


