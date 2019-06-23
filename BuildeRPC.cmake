#-------------------------------------------------------------------------------
# Copyright (c) 2019 Linaro Limited
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the eRPC library with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

#Define where eRPC intermediate output files are stored.
set (LIBEPRC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/eRPC")

if(NOT DEFINED DUAL_CORE_IPC)
	message(FATAL_ERROR "Please set DUAL_CORE_IPC to 'True' before including this file.")
endif()

if(NOT DUAL_CORE_IPC)
	message(FATAL_ERROR "Please enable DUAL_CORE_IPC before including this file.")
endif()

if(NOT DEFINED LIBEPRC_SOURCE_DIR)
	message(FATAL_ERROR "Please set LIBEPRC_SOURCE_DIR before including this file.")
endif()

if(NOT DEFINED LIBEPRC_TARGET_NAME)
	message(FATAL_ERROR "Please set LIBEPRC_TARGET_NAME before including this file.")
endif()

set (LIBEPRC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/liberpc")
set (LIBEPRC_INSTALL_DIR "${LIBEPRC_BINARY_DIR}/liberpc_install")

if(NOT LIBEPRC_DEBUG)
	set(LIBEPRC_BUILD_TYPE "Release")
else()
	set(LIBEPRC_BUILD_TYPE "Debug")
endif()

#Build liberpc as external project.
include(ExternalProject)

externalproject_add(${LIBEPRC_TARGET_NAME}
	SOURCE_DIR ${LIBEPRC_SOURCE_DIR}

	#Inherit the build setting of this project
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${LIBEPRC_BUILD_TYPE}

	#C compiler settings
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:string=${CMAKE_C_COMPILER}
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_ID:string=${CMAKE_C_COMPILER_ID}
	CMAKE_CACHE_ARGS -DTOOLCHAIN_DIR:string=${GNUARM_PATH}
	CMAKE_CACHE_ARGS -DCMAKE_TOOLCHAIN_FILE:string=${LIBEPRC_SOURCE_DIR}/armgcc.cmake
	CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_DEBUG:string=${CMAKE_C_FLAGS_DEBUG}
	CMAKE_CACHE_ARGS -DCMAKE_C_OUTPUT_EXTENSION:string=.o
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_WORKS:bool=true

	#Install location
	CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:string=${LIBEPRC_INSTALL_DIR}

	BINARY_DIR ${LIBEPRC_BINARY_DIR})
