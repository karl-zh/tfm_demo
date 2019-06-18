/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_MHU_H__
#define __TFM_PLAT_MHU_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Config the MHU.
 *
 * \details Initialize the MHU from CPU0.
 */
enum tfm_plat_err_t tfm_plat_mhu_configuration(void);

/**
 * \brief Sets MHU by channel number.
 *
 * \param[in] cpu_id   CPU Id to set the channel \ref tfm_cpu_id_t
 * \param[in] channel  Channel number to set
 *
 * \details Trigger the MHU interrupt.
 */
enum tfm_plat_err_t tfm_plat_mhu_set(enum tfm_cpu_id_t cpu_id, uint32_t channel);

/**
 * \brief Gets MHU status register value.
 *
 * \param[in]  cpu_id   CPU Id to check the status \ref tfm_cpu_id_t
 * \param[out] status   MHU status value
 *
 * \details Get the status of MHU.
 */
enum tfm_plat_err_t tfm_plat_mhu_get_status(enum tfm_cpu_id_t cpu_id, uint32_t *status);

/**
 * \brief Clears MHU bit in the associated status register.
 *
 * \param[in]  cpu_id    CPU Id to clear the status \ref tfm_cpu_id_t
 * \param[in]  clear_val Bits to clear (1 means clear)
 *
 * \details Clear the MHU bit status.
 */
enum tfm_plat_err_t tfm_plat_mhu_clear(enum tfm_cpu_id_t cpu_id, uint32_t clear_val);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_MHU_H__ */
