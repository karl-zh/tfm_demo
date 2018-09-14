/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LED_VENEERS_H__
#define __TFM_LED_VENEERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_led_defs.h"

/**
 */
enum tfm_led_err tfm_led_veneer_toggle();

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LED_VENEERS_H__ */
