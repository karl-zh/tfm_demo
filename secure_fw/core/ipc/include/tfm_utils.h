/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_UTILS_H__
#define __TFM_UTILS_H__

/* CPU spin here */
void tfm_panic(void);

/* Assert and spin */
#define TFM_ASSERT(cond)                                            \
            do {                                                    \
                if (!(cond)) {                                      \
                    printf("Assert:%s:%d", __FUNCTION__, __LINE__); \
                    while (1)                                       \
                        ;                                           \
                }                                                   \
            } while (0)

/* Get the container function address from a member */
#define TFM_TO_CONTAINER(ptr, type, member) \
    (type *)((unsigned long)(ptr) - (unsigned long)&((type *)0)->member)

int32_t tfm_bitcount(uint32_t n);

#endif
