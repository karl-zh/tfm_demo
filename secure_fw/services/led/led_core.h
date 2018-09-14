/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __LED_CORE_H__
#define __LED_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "tfm_led_defs.h"

/*!
 * \brief Initializes the led service
 *        during the TFM boot up process
 *
 * \return Returns TFM_LED_ERR_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref tfm_led_err
 */
enum tfm_led_err led_core_init(void);

/*!
 * \brief Toggles the led on GPIO2
 *
 * \return Returns TFM_LED_ERR_SUCCESS
 */
enum tfm_led_err led_toggle(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_CORE_H_ */
