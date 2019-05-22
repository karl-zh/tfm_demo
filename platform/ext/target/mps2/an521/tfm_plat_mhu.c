/*
 * Copyright (c) 2017-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"
#include "platform/include/tfm_plat_defs.h"
#include "platform_retarget_dev.h"
#include "platform/include/tfm_plat_mhu.h"

enum tfm_plat_err_t tfm_plat_mhu_configuration(void)
{
    /* No initialisation needed */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_mhu_set(enum tfm_cpu_id_t cpu_id, uint32_t channel)
{
    if (channel == 0) {
        (void)arm_mhu_sse_200_set(&ARM_MHU0_DEV_S,
            (enum arm_mhu_sse_200_cpu_id_t)cpu_id, 0x01);
    } else {
        (void)arm_mhu_sse_200_set(&ARM_MHU1_DEV_S,
            (enum arm_mhu_sse_200_cpu_id_t)cpu_id, 0x01);
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_mhu_get_status(enum tfm_cpu_id_t cpu_id, uint32_t *status)
{
    (void)arm_mhu_sse_200_status(&ARM_MHU0_DEV_S, (enum arm_mhu_sse_200_cpu_id_t)cpu_id, status);
	
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_mhu_clear(enum tfm_cpu_id_t cpu_id, uint32_t clear_val)
{
    (void)arm_mhu_sse_200_clear(&ARM_MHU0_DEV_S, (enum arm_mhu_sse_200_cpu_id_t)cpu_id, clear_val);

    return TFM_PLAT_ERR_SUCCESS;
}
