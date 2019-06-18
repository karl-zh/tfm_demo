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

enum tfm_plat_err_t
tfm_platform_hal_pin_service(const psa_invec  *in_vec,  uint32_t num_invec,
                             const psa_outvec *out_vec, uint32_t num_outvec)
{
    (void)in_vec;
    (void)num_invec;
    (void)out_vec;
    (void)num_outvec;
    /* Not needed for this platform */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_cpu_id_t tfm_platform_hal_get_cpu_id(void)
{
    volatile uint32_t* p_cpu_id = (volatile uint32_t*)CPU_ID_UNIT_BASE;
    return *p_cpu_id;
}
