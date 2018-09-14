/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "led_core.h"
#include "tfm_led_defs.h"
#include "tfm_secure_api.h"

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
enum tfm_led_err led_core_init(void)
{
    //Output enable set for GPIO2 (LED RED) 
    *((uint32_t* )0x50110010) |= 0x4;
    //Writing 1 to GPIO2 to switch off the led
    *((uint32_t* )0x50110004) |= 0x4;
    return TFM_LED_ERR_SUCCESS;
}

enum tfm_led_err led_toggle(void)
{
    //Fetching the value of GPIO port
    uint32_t data = *((uint32_t* )0x50110000);
    //Writing the toggled value to the dataout register
    if(data & 0x4) {
        *((uint32_t* )0x50110004) = data & ~0x4;
    } else {
        *((uint32_t* )0x50110004) = data | 0x4;
    }

    return TFM_LED_ERR_SUCCESS;
}
/*!@}*/
