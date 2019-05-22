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

/**
 * \file mhu_sse200_drv.h
 * \brief Generic driver for CoreLink SSE 200 Message Handling Units (MHUs).
 */

#ifndef __MHU_SSE_200_DRV_H__
#define __MHU_SSE_200_DRV_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ARM MHU device configuration structure */
struct arm_mhu_sse_200_dev_cfg_t {
    const uint32_t base;  /*!< MHU base address */
};

/* ARM MHU device structure */
struct arm_mhu_sse_200_dev_t {
    const struct arm_mhu_sse_200_dev_cfg_t* const cfg; /*!< MHU configuration */
};

/* ARM MHU enumeration types */
enum arm_mhu_sse_200_error_t {
    ARM_MHU_ERR_NONE = 0,     /*!< No error */
    ARM_MHU_ERR_INVALID_ARG,  /*!< Invalid argument */
};

/* ARM MHU enumeration types */
enum arm_mhu_sse_200_cpu_id_t {
    ARM_MHU_CPU0 = 0,  /*!< CPU 0 */
    ARM_MHU_CPU1,      /*!< CPU 1 */
};

/**
 * \brief Gets MHU status register value.
 *
 * \param[in]  dev     MHU configuration \ref arm_mhu_sse_200_dev_t
 * \param[in]  cpu_id  CPU Id to check the status \ref arm_mhu_sse_200_cpu_id_t
 * \param[out] status  MHU status value
 *
 * \return Returns error code as specified in \ref arm_mhu_sse_200_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum arm_mhu_sse_200_error_t arm_mhu_sse_200_status(
                                        const struct arm_mhu_sse_200_dev_t* dev,
                                        enum arm_mhu_sse_200_cpu_id_t cpu_id,
                                        uint32_t* status);

/**
 * \brief Sets MHU set register value.
 *
 * \param[in] dev      MHU configuration \ref arm_mhu_sse_200_dev_t
 * \param[in] cpu_id   CPU Id to check the status \ref arm_mhu_sse_200_cpu_id_t
 * \param[in] set_val  Value to set
 *
 * \note This function doesn't check if dev is NULL.
 */
void arm_mhu_sse_200_set(const struct arm_mhu_sse_200_dev_t* dev,
                         enum arm_mhu_sse_200_cpu_id_t cpu_id,
                         uint32_t set_val);

/**
 * \brief Clears MHU bit in the associated status register.
 *
 * \param[in] dev        Timer configuration \ref arm_mhu_sse_200_dev_t
 * \param[in] cpu_id     CPU Id to clear the bits \ref arm_mhu_sse_200_cpu_id_t
 * \param[in] clear_val  Bits to clear (1 means clear)
 *
 * \note This function doesn't check if dev is NULL.
 */
void arm_mhu_sse_200_clear(const struct arm_mhu_sse_200_dev_t* dev,
                           enum arm_mhu_sse_200_cpu_id_t cpu_id,
                           uint32_t clear_val);
#ifdef __cplusplus
}
#endif
#endif /* __MHU_SSE_200_DRV_H__ */
