/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_OPEN_AMP_H_
#define TFM_OPEN_AMP_H_

#define SHM_START_ADDR		0x04000400
#define SHM_SIZE		0x7c00
#define SHM_DEVICE_NAME		"sramx.shm"

#define VRING_COUNT		2
#define VRING_RX_ADDRESS	0x04007800
#define VRING_TX_ADDRESS	0x04007C00
#define VRING_ALIGNMENT		4
#define VRING_SIZE		16

#define VDEV_STATUS_ADDR	0x04000000

extern void tfm_openamp_init(void);

#endif /* TFM_OPEN_AMP_H_ */


