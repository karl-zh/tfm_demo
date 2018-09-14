/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_ns_svc.h"
#include "tfm_led_veneers.h"

enum tfm_led_err tfm_led_svc_toggle()
{
    return tfm_led_veneer_toggle();
}
