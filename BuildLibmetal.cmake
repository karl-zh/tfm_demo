#-------------------------------------------------------------------------------
# Copyright (c) 2019 Linaro Limited
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the libmetal library with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

#Define where libmetal intermediate output files are stored.
set (LIBMETAL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/libmetal")

if(NOT DEFINED DUAL_CORE_IPC)
	message(FATAL_ERROR "Please set DUAL_CORE_IPC to 'True' before including this file.")
endif()

if(NOT DUAL_CORE_IPC)
	message(FATAL_ERROR "Please enable DUAL_CORE_IPC before including this file.")
endif()

if(NOT DEFINED LIBMETAL_SOURCE_DIR)
	message(FATAL_ERROR "Please set LIBMETAL_SOURCE_DIR before including this file.")
endif()

if(NOT DEFINED LIBMETAL_TARGET_NAME)
	message(FATAL_ERROR "Please set LIBMETAL_TARGET_NAME before including this file.")
endif()

set (LIBMETAL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/libmetal")
set (LIBMETAL_INSTALL_DIR ${LIBMETAL_BINARY_DIR}/libmetal_install)

if(NOT LIBMETAL_DEBUG)
	set(LIBMETAL_BUILD_TYPE "Release")
endif()

#Build libmetal as external project.
include(ExternalProject)

externalproject_add(${LIBMETAL_TARGET_NAME}
	SOURCE_DIR ${LIBMETAL_SOURCE_DIR}

	#Inherit the build setting of this project
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${LIBMETAL_BUILD_TYPE}

	#C compiler settings
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:string=${CMAKE_C_COMPILER}
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_ID:string=${CMAKE_C_COMPILER_ID}
	CMAKE_CACHE_ARGS -DCMAKE_TOOLCHAIN_FILE:string=${LIBMETAL_SOURCE_DIR}/cmake/platforms/arm-cm33-generic.cmake
	CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_DEBUG:string=${CMAKE_C_FLAGS_DEBUG}
	CMAKE_CACHE_ARGS -DCMAKE_C_OUTPUT_EXTENSION:string=.o
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_WORKS:bool=true

	#Install location
	CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:string=${LIBMETAL_INSTALL_DIR}

	BINARY_DIR ${LIBMETAL_BINARY_DIR})
