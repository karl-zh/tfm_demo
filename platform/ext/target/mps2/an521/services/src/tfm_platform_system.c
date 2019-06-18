/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_platform_system.h"
#include "cmsis.h"

#define CPU_ID_UNIT_BASE  0x5001F000

void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    (void)request;
    (void)in_vec;
    (void)out_vec;

    /* Not needed for this platform */
    return TFM_PLATFORM_ERR_NOT_SUPPORTED;
}

enum tfm_cpu_id_t tfm_platform_hal_get_cpu_id(void)
{
    volatile uint32_t* p_cpu_id = (volatile uint32_t*)CPU_ID_UNIT_BASE;
    return *p_cpu_id;
}
