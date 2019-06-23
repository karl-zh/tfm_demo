#-------------------------------------------------------------------------------
# Copyright (c) 2019 Linaro Limited
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the openAMP library with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

#Define where openAMP intermediate output files are stored.
set (LIBOPENAMP_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/openAMP")

if(NOT DEFINED DUAL_CORE_IPC)
	message(FATAL_ERROR "Please set DUAL_CORE_IPC to 'True' before including this file.")
endif()

if(NOT DUAL_CORE_IPC)
	message(FATAL_ERROR "Please enable DUAL_CORE_IPC before including this file.")
endif()

if(NOT DEFINED LIBOPENAMP_SOURCE_DIR)
	message(FATAL_ERROR "Please set LIBOPENAMP_SOURCE_DIR before including this file.")
endif()

if(NOT DEFINED LIBOPENAMP_TARGET_NAME)
	message(FATAL_ERROR "Please set LIBOPENAMP_TARGET_NAME before including this file.")
endif()

set (LIBOPENAMP_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/libopen_amp")
set (LIBOPENAMP_INSTALL_DIR ${LIBOPENAMP_BINARY_DIR}/libopen_amp_install)

if(NOT LIBOPENAMP_DEBUG)
	set(LIBOPENAMP_BUILD_TYPE "Release")
endif()

#Build libopen_amp as external project.
include(ExternalProject)

externalproject_add(${LIBOPENAMP_TARGET_NAME}
	SOURCE_DIR ${LIBOPENAMP_SOURCE_DIR}

	#Inherit the build setting of this project
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${LIBOPENAMP_BUILD_TYPE}

	#C compiler settings
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:string=${CMAKE_C_COMPILER}
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_ID:string=${CMAKE_C_COMPILER_ID}
	CMAKE_CACHE_ARGS -DCMAKE_TOOLCHAIN_FILE:string=${LIBOPENAMP_SOURCE_DIR}/cmake/platforms/cross_generic_gcc.cmake
	CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_DEBUG:string=${CMAKE_C_FLAGS_DEBUG}
	CMAKE_CACHE_ARGS -DLIBMETAL_INCLUDE_DIR:string=${CMAKE_CURRENT_BINARY_DIR}/libmetal/lib/include
	CMAKE_CACHE_ARGS -DLIBMETAL_LIB:string=${CMAKE_CURRENT_BINARY_DIR}/libmetal/lib

	CMAKE_CACHE_ARGS -DCMAKE_C_OUTPUT_EXTENSION:string=.o
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_WORKS:bool=true

	CMAKE_CACHE_ARGS -DCMAKE_SYSTEM_PROCESSOR:string=arm
	CMAKE_CACHE_ARGS -DMACHINE:string=template
	CMAKE_CACHE_ARGS -DCROSS_PREFIX:string=arm-none-eabi-
	CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS:string=-mcpu=cortex-m33+nodsp

	#Install location
	CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:string=${LIBOPENAMP_INSTALL_DIR}

	BINARY_DIR ${LIBOPENAMP_BINARY_DIR})
