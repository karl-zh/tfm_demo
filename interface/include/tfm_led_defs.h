/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LED_DEFS_H__
#define __TFM_LED_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_api.h"
#include "limits.h"

/* The return value is shared with the TFM partition status value. The LED
 * return codes shouldn't overlap with predefined TFM status values.
 */
#define TFM_LED_ERR_OFFSET (TFM_PARTITION_SPECIFIC_ERROR_MIN)

enum tfm_led_err {
    TFM_LED_ERR_SUCCESS = 0,
    TFM_LED_ERR_FAILURE = TFM_LED_ERR_OFFSET,

    /* Following entry is only to ensure the error code of int size */
    TFM_LED_ERR_FORCE_INT_SIZE = INT_MAX
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LED_DEFS_H__ */
