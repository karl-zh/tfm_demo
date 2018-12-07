/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service_api.h"
#include "sst_test_service_veneers.h"

#define DATA "TEST_DATA_ONE_TWO_THREE_FOUR_FIVE"
#define BUF_SIZE (sizeof(DATA))

#define SST_TEST_SERVICE_SFN_SETUP_SID              (0x00003000)
#define SST_TEST_SERVICE_SFN_DUMMY_ENCRYPT_SID      (0x00003001)
#define SST_TEST_SERVICE_SFN_DUMMY_DECRYPT_SID      (0x00003002)
#define SST_TEST_SERVICE_SFN_CLEAN_SID              (0x00003003)
#define SST_TEST_SERVICE_SFN_SETUP_MIN_VER          (0x0001)
#define SST_TEST_SERVICE_SFN_DUMMY_ENCRYPT_MIN_VER  (0x0001)
#define SST_TEST_SERVICE_SFN_DUMMY_DECRYPT_MIN_VER  (0x0001)
#define SST_TEST_SERVICE_SFN_CLEAN_MIN_VER          (0x0001)

static psa_status_t psa_sst_test_common(uint32_t sid, uint32_t minor_version,
                                        const psa_invec *in_vecs, size_t in_len,
                                        psa_outvec *out_vecs, size_t out_len)
{
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(sid, minor_version);
    if (handle <= 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    status = psa_call(handle, in_vecs, in_len, out_vecs, out_len);
    if (status < 0) {
        status = PSA_SST_ERR_SYSTEM_ERROR;
    }

    psa_close(handle);
    return status;
}

enum psa_sst_err_t sst_test_service_setup(void)
{
    return psa_sst_test_common(SST_TEST_SERVICE_SFN_SETUP_SID,
                               SST_TEST_SERVICE_SFN_SETUP_MIN_VER,
                               NULL, 0 , NULL, 0);
}

enum psa_sst_err_t sst_test_service_dummy_encrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    struct psa_invec invecs[2] = {{&key_uuid, sizeof(uint32_t)},
                                  {buf, buf_size}};
    struct psa_outvec outvecs[1] = {{buf, buf_size}};

    return psa_sst_test_common(SST_TEST_SERVICE_SFN_DUMMY_ENCRYPT_SID,
                               SST_TEST_SERVICE_SFN_DUMMY_ENCRYPT_MIN_VER,
                               invecs, 2, outvecs, 1);
}

enum psa_sst_err_t sst_test_service_dummy_decrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    struct psa_invec invecs[2] = {{&key_uuid, sizeof(uint32_t)},
                                  {buf, buf_size}};
    struct psa_outvec outvecs[1] = {{buf, buf_size}};

    return psa_sst_test_common(SST_TEST_SERVICE_SFN_DUMMY_DECRYPT_SID,
                               SST_TEST_SERVICE_SFN_DUMMY_DECRYPT_MIN_VER,
                               invecs, 2, outvecs, 1);
}

enum psa_sst_err_t sst_test_service_clean(void)
{
    return psa_sst_test_common(SST_TEST_SERVICE_SFN_CLEAN_SID,
                               SST_TEST_SERVICE_SFN_CLEAN_MIN_VER,
                               NULL, 0, NULL, 0);
}
