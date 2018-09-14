/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_led_veneers.h"
#include "secure_fw/services/led/led_core.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"

__tfm_secure_gateway_attributes__
enum tfm_led_err tfm_led_veneer_toggle()
{
    TFM_CORE_SFN_REQUEST(TFM_SP_LED_ID, led_toggle,
                         0, 0, 0, 0);
}
