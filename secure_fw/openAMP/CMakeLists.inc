#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(SS_OPENAMP_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

set (SS_OPENAMP_C_SRC "${CMAKE_CURRENT_LIST_DIR}/tfm_openamp.c")

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SS_OPENAMP_C_SRC})
unset(SS_OPENAMP_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../zephyr/ext/hal/libmetal/libmetal/build_libmetal/lib/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../zephyr/ext/lib/ipc/open-amp/open-amp/lib/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/service ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/infra ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/setup ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/port ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/config ABSOLUTE)
