#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the mbedtls libraries with
#the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

#Define where mbedtls intermediate output files are stored.
set (LIBMETAL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/libmetal")

set (LIBMETAL_TARGET_NAME "libmetal_cm33")
#set (LIBMETAL_SOURCE_DIR "${CMAKE_CURRENT_DIR}/../libmetal/libmetal/")

set (LIBMETAL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/libmetal")
set (LIBMETAL_INSTALL_DIR ${LIBMETAL_BINARY_DIR}/libmetal_install)

if(NOT LIBMETAL_DEBUG)
	set(MBEDTLS_BUILD_TYPE "Debug")
endif()

#Build mbedtls as external project.
#This ensures mbedtls is built with exactly defined settings.
#mbedtls will be used from is't install location
include(ExternalProject)

externalproject_add(${LIBMETAL_TARGET_NAME}
	SOURCE_DIR ${LIBMETAL_SOURCE_DIR}

	#Inherit the build setting of this project
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${LIBMETAL_BUILD_TYPE}

	#C compiler settings
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:string=${CMAKE_C_COMPILER}
	CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_ID:string=${CMAKE_C_COMPILER_ID}
	CMAKE_CACHE_ARGS -DCMAKE_TOOLCHAIN_FILE:string=${LIBMETAL_SOURCE_DIR}/cmake/platforms/arm-cm33-generic.cmake

	#Install location
	CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:string=${LIBMETAL_INSTALL_DIR}

	BINARY_DIR ${LIBMETAL_BINARY_DIR})

#Add an install target to force installation after each mbedtls build. Without
#this target installation happens only when a clean mbedtls build is executed.
#add_custom_target(${LIBMETAL_TARGET_NAME}_install
#	 COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/libmetal -- install
#	 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libmetal
#	 COMMENT "Installing libmetal to ${LIBMETAL_INSTALL_DIR}"
#	 VERBATIM)
#Make install rule depend on mbedtls library build
#add_dependencies(${LIBMETAL_TARGET_NAME}_install ${LIBMETAL_TARGET_NAME})
