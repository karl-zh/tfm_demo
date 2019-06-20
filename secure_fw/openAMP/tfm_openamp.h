/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_OPEN_AMP_H_
#define TFM_OPEN_AMP_H_

#include "region_defs.h"

#define VDEV_STATUS_ADDR    (NS_RAM_ALIAS_BASE + OPENAMP_SHARED_MEMORY_OFFSET)

#define SHM_START_ADDR      (VDEV_STATUS_ADDR + 0x400)
#define SHM_SIZE            0x7c00
#define SHM_DEVICE_NAME     "sramx.shm"

#define VRING_COUNT         2
#define VRING_RX_ADDRESS    (VDEV_STATUS_ADDR + SHM_SIZE - 0x400)
#define VRING_TX_ADDRESS    (VDEV_STATUS_ADDR + SHM_SIZE)
#define VRING_ALIGNMENT     4
#define VRING_SIZE          16

extern int tfm_openamp_init(void);
extern struct rpmsg_endpoint *tfm_openamp_get_ep(void);
extern void tfm_openamp_exit(void);

#endif /* TFM_OPEN_AMP_H_ */
