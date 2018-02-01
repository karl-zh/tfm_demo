#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${CMAKE_CURRENT_LIST_DIR}/../../../ConfigDefault.cmake")
project(tfm_audit LANGUAGES ASM C)
embedded_project_fixup()

###Some project global settings
set (AUDIT_LOGGING_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${AUDIT_LOGGING_DIR}/../../.." ABSOLUTE)

###Get the definition of what files we need to build
set (ENABLE_AUDIT_LOGGING ON)
include(CMakeLists.inc)

###Configure how we build our target
if(DEFINED CORE_TEST)
	set (TFM_LVL 3)
else()
	set (TFM_LVL 1)
endif()

#Specify what we build (for the audit logging service, build as a static library)
add_library(tfm_audit STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})
embedded_set_target_compile_defines(TARGET tfm_audit LANGUAGE C DEFINES __ARM_FEATURE_CMSE=3 __thumb2__ TFM_LVL=${TFM_LVL})

embedded_project_end(tfm_audit)