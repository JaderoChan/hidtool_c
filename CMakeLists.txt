cmake_minimum_required(VERSION 3.26)

project(hidtool_c VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MSG_PREFIX "[HID Tool C]")

# In-source build guard
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(
        FATAL_ERROR
        "${MSG_PREFIX} In-source builds are not allowed. "
        "Please create a build directory and run CMake from there.")
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

###########
# Options #
###########

option(HIDTOOL_C_BUILD_SHARED   "Build hidtool_c as a shared library" OFF)
option(HIDTOOL_C_BUILD_EXAMPLES "Build hidtool_c example programs"    OFF)

###############################
# Build hidtool C++ submodule #
###############################

add_subdirectory(3rdparty/hidtool)

####################################
# Generate C binding config header #
####################################

# Mirror the keyboard/mouse feature flags from the hidtool submodule.
set(HIDTOOL_C_HAS_KEYBOARD ${HIDTOOL_BUILD_WITH_KEYBOARD})
set(HIDTOOL_C_HAS_MOUSE    ${HIDTOOL_BUILD_WITH_MOUSE})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/hidtool_c_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/hidtool_c/hidtool_c_config.h
    @ONLY
)

#########
# Build #
#########

set(HIDTOOL_C_SRCS src/hidtool_c.cpp)

if(HIDTOOL_C_BUILD_SHARED)
    message(STATUS "${MSG_PREFIX} Building as shared library.")
    add_library(${PROJECT_NAME} SHARED ${HIDTOOL_C_SRCS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HIDTOOL_C_BUILD_SHARED)
else()
    message(STATUS "${MSG_PREFIX} Building as static library.")
    add_library(${PROJECT_NAME} STATIC ${HIDTOOL_C_SRCS})
endif()

# Alias for consistent target referencing
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)

# UTF-8 source encoding for MSVC
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

#######################
# Include directories #
#######################

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/hidtool/include>
)

################
# Dependencies #
################

target_link_libraries(${PROJECT_NAME} PRIVATE hidtool::hidtool)

############
# Examples #
############

if(HIDTOOL_C_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
