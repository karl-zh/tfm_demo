/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_OPEN_AMP_H_
#define TFM_OPEN_AMP_H_

#define VDEV_STATUS_ADDR	0x28180000 //0x04000000

#define SHM_START_ADDR		(VDEV_STATUS_ADDR + 0x400) //0x04000400
#define SHM_SIZE		0x7c00
#define SHM_DEVICE_NAME		"sramx.shm"

#define VRING_COUNT		2
#define VRING_RX_ADDRESS	(VDEV_STATUS_ADDR + SHM_SIZE - 0x400) //0x04007800
#define VRING_TX_ADDRESS	(VDEV_STATUS_ADDR + SHM_SIZE) //0x04007C00
#define VRING_ALIGNMENT		4
#define VRING_SIZE		16

extern void tfm_openamp_init(void);

#endif /* TFM_OPEN_AMP_H_ */


