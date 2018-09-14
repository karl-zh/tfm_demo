/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_led_defs.h"
#include "tfm_ns_lock.h"

enum tfm_led_err tfm_led_toggle()
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_LED_TOGGLE,
                                    0,
                                    0,
                                    0,
                                    0);
}
