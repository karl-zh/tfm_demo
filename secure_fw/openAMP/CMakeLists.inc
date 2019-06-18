#-------------------------------------------------------------------------------
# Copyright (c) 2019 Linaro Limited
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set (SS_OPENAMP_C_SRC "${CMAKE_CURRENT_LIST_DIR}/tfm_openamp.c")

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SS_OPENAMP_C_SRC})
unset(SS_OPENAMP_C_SRC)

#Setting include directories
embedded_include_directories(PATH ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
embedded_include_directories(PATH ${CMAKE_CURRENT_BINARY_DIR}/libmetal/lib/include ABSOLUTE)
embedded_include_directories(PATH ${CMAKE_CURRENT_BINARY_DIR}/libopen_amp/libopen_amp_install/include ABSOLUTE)
