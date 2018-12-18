/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_SIGNAL_H__
#define __SST_SIGNAL_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API
#define PSA_SST_CREATE         (1 << (0 + 4))
#define PSA_SST_GET_INFO       (1 << (1 + 4))
#define PSA_SST_GET_ATTRIBUTES (1 << (2 + 4))
#define PSA_SST_SET_ATTRIBUTES (1 << (3 + 4))
#define PSA_SST_READ           (1 << (4 + 4))
#define PSA_SST_WRITE          (1 << (5 + 4))
#define PSA_SST_DELETE         (1 << (6 + 4))
#endif /* TFM_PSA_API */

#endif /* __SST_SIGNAL_H__ */
