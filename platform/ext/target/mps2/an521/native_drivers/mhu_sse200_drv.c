/*
 * Copyright (c) 2016-2019 ARM Limited
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

#include "mhu_sse200_drv.h"

#include <stddef.h>

/* MHU SSE 200 register map structure */
struct mhu_reg_map_t {
    volatile uint32_t cpu0intr_stat; /* (R/ ) CPU 0 Interrupt Status Register */
    volatile uint32_t cpu0intr_set;  /* ( /W) CPU 0 Interrupt Set Register */
    volatile uint32_t cpu0intr_clr;  /* ( /W) CPU 0 Interrupt Clear Register */
    volatile uint32_t reserved0;
    volatile uint32_t cpu1intr_stat; /* (R/ ) CPU 1 Interrupt Status Register */
    volatile uint32_t cpu1intr_set;  /* ( /W) CPU 1 Interrupt Set Register */
    volatile uint32_t cpu1intr_clr;  /* ( /W) CPU 1 Interrupt Clear Register */
    volatile uint32_t reserved1[1004];
    volatile uint32_t pidr4;         /* ( /W) Peripheral ID 4 */
    volatile uint32_t reserved2[3];
    volatile uint32_t pidr0;         /* ( /W) Peripheral ID 0 */
    volatile uint32_t pidr1;         /* ( /W) Peripheral ID 1 */
    volatile uint32_t pidr2;         /* ( /W) Peripheral ID 2 */
    volatile uint32_t pidr3;         /* ( /W) Peripheral ID 3 */
    volatile uint32_t cidr0;         /* ( /W) Component ID 0 */
    volatile uint32_t cidr1;         /* ( /W) Component ID 1 */
    volatile uint32_t cidr2;         /* ( /W) Component ID 2 */
    volatile uint32_t cidr3;         /* ( /W) Component ID 3 */
};

enum arm_mhu_sse_200_error_t arm_mhu_sse_200_status(
                                        const struct arm_mhu_sse_200_dev_t* dev,
                                        enum arm_mhu_sse_200_cpu_id_t cpu_id,
                                        uint32_t* status)
{
    struct mhu_reg_map_t* p_mhu_dev;

    if(status == NULL) {
        return ARM_MHU_ERR_INVALID_ARG;
    }

    p_mhu_dev = (struct mhu_reg_map_t*)dev->cfg->base;

    switch(cpu_id) {
    case ARM_MHU_CPU1:
        *status = p_mhu_dev->cpu1intr_stat;
        break;
    case ARM_MHU_CPU0:
    default:
        *status = p_mhu_dev->cpu0intr_stat;
        break;
    }

    return ARM_MHU_ERR_NONE;
}

void arm_mhu_sse_200_set(const struct arm_mhu_sse_200_dev_t* dev,
                         enum arm_mhu_sse_200_cpu_id_t cpu_id,
                         uint32_t set_val)
{
    struct mhu_reg_map_t* p_mhu_dev;

    p_mhu_dev = (struct mhu_reg_map_t*)dev->cfg->base;

    switch(cpu_id) {
    case ARM_MHU_CPU1:
        p_mhu_dev->cpu1intr_set = set_val;
        break;
    case ARM_MHU_CPU0:
    default:
        p_mhu_dev->cpu0intr_set = set_val;
        break;
    }
}

void arm_mhu_sse_200_clear(const struct arm_mhu_sse_200_dev_t* dev,
                           enum arm_mhu_sse_200_cpu_id_t cpu_id,
                           uint32_t clear_val)
{
    struct mhu_reg_map_t* p_mhu_dev;

    p_mhu_dev = (struct mhu_reg_map_t*)dev->cfg->base;

    switch(cpu_id) {
    case ARM_MHU_CPU1:
        p_mhu_dev->cpu1intr_clr = clear_val;
        break;
    case ARM_MHU_CPU0:
    default:
        p_mhu_dev->cpu0intr_clr = clear_val;
        break;
    }
}
