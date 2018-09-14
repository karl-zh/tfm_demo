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
project(tfm_led LANGUAGES ASM C)
embedded_project_fixup()

###Some project global settings
set (LED_SERVICE_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${LED_SERVICE_DIR}/../../.." ABSOLUTE)

###Get the definition of what files we need to build
set (ENABLE_LED_SERVICE ON)
include(CMakeLists.inc)

set (TFM_LVL 1)

#Specify what we build (for the led service, build as a static library)
add_library(tfm_led STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})
embedded_set_target_compile_defines(TARGET tfm_led LANGUAGE C DEFINES __ARM_FEATURE_CMSE=3 __thumb2__ TFM_LVL=${TFM_LVL})

#Set common compiler and linker flags
config_setting_shared_compiler_flags(tfm_led)
config_setting_shared_linker_flags(tfm_led)

embedded_project_end(tfm_led)
