/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_ERPC_H_
#define TFM_ERPC_H_

#include <openamp/open_amp.h>

extern void tfm_erpc_init(struct rpmsg_endpoint *ep);
extern void tfm_erpc_poll(void);
extern void tfm_erpc_exit(void);

#endif /* TFM_ERPC_H_ */
