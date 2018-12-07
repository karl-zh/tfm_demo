/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service.h"

#include "secure_fw/core/tfm_secure_api.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"
#include "secure_fw/services/secure_storage/sst_utils.h"
#include "psa_sst_api.h"
#ifdef TFM_PSA_API
#include "stdio.h"
#include "psa_service.h"
#include "tfm_sst_test_service_signal.h"
#endif

#define SST_TEST_SERVICE_KEY { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, \
                               0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD, 0xF0, 0x0D, }
#define SST_TEST_SERVICE_KEY_SIZE SST_ASSET_MAX_SIZE_AES_KEY_128

/* Define default asset's token */
#define ASSET_TOKEN      NULL
#define ASSET_TOKEN_SIZE 0

#ifdef TFM_PSA_API
/*
 * FixMe: Temporarily alloc static size, will alloc the size dynamically when
 * implement memory alloc functions.
 */
#define SST_MAX_BUF_SIZE 40
typedef psa_status_t (*sst_test_func_t)(psa_msg_t *msg);
#endif

/* Common interfaces for both modes */
static enum psa_sst_err_t _sst_test_service_sfn_dummy_encrypt(int32_t client_id,
                                                              uint32_t key_uuid,
                                                              uint8_t *buf,
                                                              uint32_t buf_size)
{
    enum psa_sst_err_t err;
    uint32_t i;
    uint8_t key_data[SST_TEST_SERVICE_KEY_SIZE];

    /* Read the key from the asset using the non-secure caller's client ID */
    err = psa_sst_reference_read(client_id, key_uuid, ASSET_TOKEN,
                                 ASSET_TOKEN_SIZE, SST_TEST_SERVICE_KEY_SIZE,
                                 0, key_data);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Encrypt the data (very badly) using the key from secure storage */
    for (i = 0; i < buf_size; i++) {
        buf[i] ^= key_data[i % SST_TEST_SERVICE_KEY_SIZE];
    }

    return PSA_SST_ERR_SUCCESS;
}

#ifdef TFM_PSA_API
/**
 * SST test service wrap functions mainly focus on getting caller input
 * parameters through ipc psa_read, and calling the corresponding service.
 */
psa_status_t sst_test_service_sfn_setup_ipc_wrap(psa_msg_t *msg)
{
    psa_status_t status;

    status = sst_test_service_sfn_setup();
    return status;
}

psa_status_t sst_test_service_sfn_dummy_encrypt_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t key_uuid, size;
    uint8_t buffer[SST_MAX_BUF_SIZE];
    psa_status_t status;
    size_t num = 0;

    client_id = msg->client_id;
    size = msg->in_size[1];
    num = psa_read(msg->handle, 0, &key_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, buffer, size);
    if (num != size) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    status = _sst_test_service_sfn_dummy_encrypt(client_id, key_uuid, buffer,
                                                 size);
    psa_write(msg->handle, 0, buffer, size);
    return status;
}

/* SST test service dummy decrypt IPC part */
static enum psa_sst_err_t _sst_test_service_sfn_dummy_decrypt(int32_t client_id,
                                                              uint32_t key_uuid,
                                                              uint8_t *buf,
                                                              uint32_t buf_size)
{
    /*
     * In the current implementation encrypt and decrypt are the same
     * operation.
     */
    return _sst_test_service_sfn_dummy_encrypt(client_id, key_uuid, buf,
                                               buf_size);
}

static psa_status_t sst_test_service_sfn_dummy_decrypt_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t key_uuid, size;
    uint8_t buffer[SST_MAX_BUF_SIZE];
    psa_status_t status;
    size_t num = 0;

    client_id = msg->client_id;
    size = msg->in_size[1];
    num = psa_read(msg->handle, 0, &key_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, buffer, size);
    if (num != size) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    status = _sst_test_service_sfn_dummy_decrypt(client_id, key_uuid, buffer,
                                                 size);
    psa_write(msg->handle, 0, buffer, size);
    return status;
}

static psa_status_t sst_test_service_sfn_clean_ipc_wrap(psa_msg_t *msg)
{
    psa_status_t status;

    status = sst_test_service_sfn_clean();
    return status;
}

static void sst_test_service_handle(psa_signal_t signal, sst_test_func_t pfn)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    if (status) {
        printf("Failed to get message!\r\n");
        return;
    }

    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = pfn(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        break;
    }
}
#endif

/**
 * \brief Service initialisation function. No special initialisation is
 *        required.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_init(void)
{
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;

    while(1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & PSA_SST_TEST_SERVICE_SETUP) {
            sst_test_service_handle(PSA_SST_TEST_SERVICE_SETUP,
                                    sst_test_service_sfn_setup_ipc_wrap);
        } else if (signals & PSA_SST_TEST_SERVICE_DUMMY_ENCRYPT) {
            sst_test_service_handle(PSA_SST_TEST_SERVICE_DUMMY_ENCRYPT,
                                   sst_test_service_sfn_dummy_encrypt_ipc_wrap);
        } else if (signals & PSA_SST_TEST_SERVICE_DUMMY_DECRYPT) {
            sst_test_service_handle(PSA_SST_TEST_SERVICE_DUMMY_DECRYPT,
                                   sst_test_service_sfn_dummy_decrypt_ipc_wrap);
        } else if (signals & PSA_SST_TEST_SERVICE_CLEAN) {
            sst_test_service_handle(PSA_SST_TEST_SERVICE_CLEAN,
                                    sst_test_service_sfn_clean_ipc_wrap);
        } else {
            printf("signal is invalid!\r\n");
        }
    }
#else
    return PSA_SST_ERR_SUCCESS;
#endif
}

enum psa_sst_err_t sst_test_service_sfn_setup(void)
{
    enum psa_sst_err_t err;
    const uint32_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    static uint8_t key_data[SST_TEST_SERVICE_KEY_SIZE] = SST_TEST_SERVICE_KEY;


    /* Create the key asset using our secure app ID */
    err = psa_sst_create(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Write the key to the asset using our secure app ID */
    err = psa_sst_write(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        SST_TEST_SERVICE_KEY_SIZE, 0, key_data);

    return err;
}

enum psa_sst_err_t sst_test_service_sfn_dummy_encrypt(uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_test_service_sfn_dummy_encrypt(client_id, key_uuid,
                                               buf, buf_size);
}

enum psa_sst_err_t sst_test_service_sfn_dummy_decrypt(uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    /* In the current implementation encrypt and decrypt are the same
     * operation.
     */
    return sst_test_service_sfn_dummy_encrypt(key_uuid, buf, buf_size);
}

enum psa_sst_err_t sst_test_service_sfn_clean(void)
{
    enum psa_sst_err_t err;
    const uint32_t key_uuid = SST_ASSET_ID_AES_KEY_128;

    /* Delete the key asset using our secure app ID */
    err = psa_sst_delete(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);

    return err;
}
