/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_SYSTEM_H__
#define __TFM_PLATFORM_SYSTEM_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Resets the system.
 *
 * \details Requests a system reset to reset the MCU.
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_system_reset(void);

/**
 * \brief Get the current CPU Id
 *
 * \return Returns the CPU Id \ref tfm_cpu_id_t
 */
enum tfm_cpu_id_t tfm_platform_hal_get_cpu_id(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_SYSTEM_H__ */
