#-------------------------------------------------------------------------------
# Copyright (c) 2019 Linaro Limited
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set (SS_ERPC_C_SRC "${CMAKE_CURRENT_LIST_DIR}/tfm_erpc.c")

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SS_ERPC_C_SRC})
unset(SS_ERPC_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/service ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/infra ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/setup ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/port ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/../erpc/erpc_c/config ABSOLUTE)
