cmake_minimum_required(VERSION 3.16)
project(opuspy)

set(CMAKE_CXX_STANDARD 14)

message("CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # MacOS support.
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew)
    set(CMAKE_OSX_ARCHITECTURES "arm64")

endif()


list(APPEND CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu)

# Optimize.
set(CMAKE_CXX_FLAGS " -Wall -O2")

message("heljlojd")

# Pybind11
find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)


# opus
find_library(opus_LIBRARIES opus REQUIRED)
find_path(opus_INCLUDE_DIRS
        NAMES opus.h
        PATH_SUFFIXES opus
        REQUIRED
)
include_directories(${opus_INCLUDE_DIRS})
message("opus_LIBRARIES ${opus_LIBRARIES} opus_INCLUDE_DIRS ${opus_INCLUDE_DIRS}")


# opusFile
find_library(opusfile_LIBRARIES opusfile REQUIRED)
find_path(
    opusfile_INCLUDE_DIRS
    NAMES opusfile.h
    PATH_SUFFIXES opus
    REQUIRED
)
include_directories(${opusfile_INCLUDE_DIRS})
message("opusfile_LIBRARIES ${opusfile_LIBRARIES} opusfile_INCLUDE_DIRS ${opusfile_INCLUDE_DIRS}")


# opusEnc
find_library(opusenc_LIBRARIES opusenc REQUIRED)
find_path(
        opusenc_INCLUDE_DIRS
        NAMES opusenc.h
        PATH_SUFFIXES opus
        REQUIRED
)
include_directories(${opusenc_INCLUDE_DIRS})
message("opusenc_LIBRARIES ${opusenc_LIBRARIES} opusenc_INCLUDE_DIRS ${opusenc_INCLUDE_DIRS}")


pybind11_add_module(opuspy opuspy.cc)
target_link_libraries(opuspy PRIVATE ${opusfile_LIBRARIES} ${opusenc_LIBRARIES})



