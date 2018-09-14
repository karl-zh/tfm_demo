/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LED_SVC_HANDLER_H__
#define __TFM_LED_SVC_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_led_defs.h"

/**
 * \brief Toggles the led on GPIO2
 *
 * \return Returns TFM_LED_ERR_SUCCESS \ref tfm_led_err
 */
enum tfm_led_err tfm_led_svc_toggle();

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LED_SVC_HANDLER_H__ */
