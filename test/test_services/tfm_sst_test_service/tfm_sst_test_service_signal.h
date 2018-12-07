/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_TEST_SERVICE_SIGNAL_H__
#define __SST_TEST_SERVICE_SIGNAL_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API
#define PSA_SST_TEST_SERVICE_SETUP         (1 << (0 + 4))
#define PSA_SST_TEST_SERVICE_DUMMY_ENCRYPT (1 << (1 + 4))
#define PSA_SST_TEST_SERVICE_DUMMY_DECRYPT (1 << (2 + 4))
#define PSA_SST_TEST_SERVICE_CLEAN         (1 << (3 + 4))
#endif /* TFM_PSA_API */

#endif /* __SST_TEST_SERVICE_SIGNAL_H__ */
