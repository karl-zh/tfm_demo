/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "tfm_sst_veneers.h"
#include "tfm_sst_defs.h"
#include "tfm_ns_lock.h"

/*
 * Defines for SID and minor version number. These SIDs must align with the
 * value in service manifest file.
 */
#define SST_AM_CREATE_SID               (0x00002000)
#define SST_AM_GET_INFO_SID             (0x00002001)
#define SST_AM_GET_ATTRIBUTES_SID       (0x00002002)
#define SST_AM_SET_ATTRIBUTES_SID       (0x00002003)
#define SST_AM_READ_SID                 (0x00002004)
#define SST_AM_WRITE_SID                (0x00002005)
#define SST_AM_DELETE_SID               (0x00002006)
#define SST_AM_CREATE_MIN_VER           (0x0001)
#define SST_AM_GET_INFO_MIN_VER         (0x0001)
#define SST_AM_GET_ATTRIBUTES_MIN_VER   (0x0001)
#define SST_AM_SET_ATTRIBUTES_MIN_VER   (0x0001)
#define SST_AM_READ_MIN_VER             (0x0001)
#define SST_AM_WRITE_MIN_VER            (0x0001)
#define SST_AM_DELETE_MIN_VER           (0x0001)

/*
 * This define uses the TF-M invalid client ID to specify a direct client read,
 * as that it can not be used to identify a client.
 */
#define SST_DIRECT_CLIENT_READ  TFM_INVALID_CLIENT_ID

static psa_status_t psa_sst_common(uint32_t sid, uint32_t minor_version,
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

enum psa_sst_err_t psa_sst_create(uint32_t asset_uuid, const uint8_t *token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[2] = {{&asset_uuid, sizeof(uint32_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_CREATE_SID, SST_AM_CREATE_MIN_VER,
                          invecs, 2, NULL, 0);
}

enum psa_sst_err_t psa_sst_get_info(uint32_t asset_uuid,
                                    const uint8_t *token,
                                    uint32_t token_size,
                                    struct psa_sst_asset_info_t *info)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[2] = {{&asset_uuid, sizeof(uint32_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)} };
    struct psa_outvec outvecs[1] = {{info,
                                     sizeof(struct psa_sst_asset_info_t)} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_GET_INFO_SID, SST_AM_GET_INFO_MIN_VER,
                          invecs, 2, outvecs, 1);
}

enum psa_sst_err_t psa_sst_get_attributes(uint32_t asset_uuid,
                                          const uint8_t *token,
                                          uint32_t token_size,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[2] = {{&asset_uuid, sizeof(uint32_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)} };
    struct psa_outvec outvecs[1] = {{attrs,
                                     sizeof(struct psa_sst_asset_attrs_t)} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_GET_ATTRIBUTES_SID,
                          SST_AM_GET_ATTRIBUTES_MIN_VER,
                          invecs, 2, outvecs, 1);
}

enum psa_sst_err_t
    psa_sst_set_attributes(uint32_t asset_uuid,
                           const uint8_t *token, uint32_t token_size,
                           const struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[3] = {{&asset_uuid, sizeof(uint32_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)},
                                  {attrs,
                                   sizeof(struct psa_sst_asset_attrs_t)} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_SET_ATTRIBUTES_SID,
                          SST_AM_SET_ATTRIBUTES_MIN_VER,
                          invecs, 3, NULL, 0);
}

enum psa_sst_err_t psa_sst_read(uint32_t asset_uuid,
                                const uint8_t *token,
                                uint32_t token_size,
                                uint32_t size,
                                uint32_t offset,
                                uint8_t *data)
{
    struct tfm_sst_token_t s_token;
    struct tfm_sst_id_t s_id;
    struct psa_invec invecs[3] = {{&s_id, sizeof(struct tfm_sst_id_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)},
                                  {&offset, sizeof(uint32_t)} };
    struct psa_outvec outvecs[1] = {{data, size} };

    /* Pack the id information in the id structure */
    s_id.client_id = SST_DIRECT_CLIENT_READ;
    s_id.asset_uuid = asset_uuid;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_READ_SID, SST_AM_READ_MIN_VER,
                          invecs, 3, outvecs, 1);
}

enum psa_sst_err_t psa_sst_write(uint32_t asset_uuid,
                                 const uint8_t *token,
                                 uint32_t token_size,
                                 uint32_t size,
                                 uint32_t offset,
                                 const uint8_t *data)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[4] = {{&asset_uuid, sizeof(uint32_t)},
                               {&s_token, sizeof(struct tfm_sst_token_t)},
                               {&offset, sizeof(uint32_t)},
                               {data, size} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_WRITE_SID, SST_AM_WRITE_MIN_VER,
                          invecs, 4, NULL, 0);
}

enum psa_sst_err_t psa_sst_delete(uint32_t asset_uuid,
                                  const uint8_t *token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;
    struct psa_invec invecs[2] = {{&asset_uuid, sizeof(uint32_t)},
                                  {&s_token, sizeof(struct tfm_sst_token_t)} };

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return psa_sst_common(SST_AM_DELETE_SID, SST_AM_DELETE_MIN_VER,
                          invecs, 2, NULL, 0);
}
