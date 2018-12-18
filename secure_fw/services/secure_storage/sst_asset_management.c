/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_asset_management.h"

#include <stddef.h>
#include <stdio.h>

#include "assets/sst_asset_defs.h"
#include "sst_object_system.h"
#include "sst_utils.h"
#include "tfm_secure_api.h"
#include "tfm_sst_defs.h"
#ifdef TFM_PSA_API
#include "psa_service.h"
#include "tfm_sst_signal.h"
#endif

#ifdef TFM_PSA_API
#define SST_MAX_BUF_SIZE 16
typedef psa_status_t (*sst_func_t)(psa_msg_t *msg);
#endif

/******************************/
/* Asset management functions */
/******************************/

/* Policy database */
extern struct sst_asset_policy_t asset_perms[];
extern struct sst_asset_perm_t asset_perms_modes[];

/**
 * \brief Looks up for policy entry for give client and uuid
 *
 * \param[in] db_entry  Asset specific entry
 * \param[in] client_id Identify of the client calling the service
 *
 * \return Returns the perms entry on successful lookup
 */
static struct sst_asset_perm_t *sst_am_lookup_client_perms(
                                      const struct sst_asset_policy_t *db_entry,
                                      int32_t client_id)
{
    struct sst_asset_perm_t *perm_entry;
    uint32_t i;

    for (i = 0; i < db_entry->perms_count; i++) {
        perm_entry = &asset_perms_modes[db_entry->perms_modes_start_idx+i];
        if (perm_entry->client_id == client_id) {
            return perm_entry;
        }
    }

    return NULL;
}

/**
 * \brief Gets pointer to policy entry for an asset
 *
 * \param[in] uuid  Unique identifier of the object being accessed
 *
 * \return Returns the pointer for entry for specified asset
 */
static struct sst_asset_policy_t *sst_am_lookup_db_entry(uint32_t uuid)
{
    uint32_t i;

    /* Lookup in db for matching entry */
    for (i = 0; i < SST_NUM_ASSETS; i++) {
        if (asset_perms[i].asset_uuid == uuid) {
            return &asset_perms[i];
        }
    }

    return NULL;
}

/**
 * \brief Checks the compile time policy for secure/non-secure separation
 *
 * \param[in] client_id     Client ID
 * \param[in] request_type  requested action to perform
 *
 * \return Returns the sanitized request_type
 */
static uint16_t sst_am_check_s_ns_policy(int32_t client_id,
                                         uint16_t request_type)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SUCCESS;
    uint16_t access;

    /* FIXME: based on level 1 tfm isolation, any entity on the secure side
     * can have full access if it uses secure client ID to make the call.
     * When the secure caller passes on the client_id of non-secure entity,
     * the code only allows read by reference. I.e. if the client_id
     * has the reference permission, the secure caller will be allowed
     * to read the entry. This needs a revisit when for higher level
     * of isolation.
     *
     * FIXME: current code allows only a referenced read, however there
     * is a case for refereced create/write/delete as well, for example
     * a NS entity may ask another secure service to derive a key and securely
     * store it, and make references for encryption/decryption and later on
     * delete it.
     * For now it is for the other secure service to create/delete/write
     * resources with the secure client ID.
     */
#ifdef TFM_PSA_API
    /*
     * If the caller from secure, the err is still set to default
     * PSA_SST_ERR_SUCCESS, otherwise, set to an error value. Using
     * PSA_SST_ERR_PARAM_ERROR here to stand for a generic error value.
     */
    if (TFM_CLIENT_ID_IS_S(client_id) == 0) {
        err = PSA_SST_ERR_PARAM_ERROR;
    }
#else
    err = sst_utils_validate_secure_caller();
#endif

    if (err == PSA_SST_ERR_SUCCESS) {
        if (TFM_CLIENT_ID_IS_S(client_id) == 0) {
            if (request_type & SST_PERM_REFERENCE) {
                access = SST_PERM_REFERENCE;
            } else {
                /* Other permissions can not be delegated */
                access = SST_PERM_FORBIDDEN;
            }
        } else {
            /* a call from secure entity on it's own behalf.
             * In level 1 isolation, any secure entity has
             * full access to storage.
             */
            access = SST_PERM_BYPASS;
        }
    } else if (TFM_CLIENT_ID_IS_S(client_id) == 1) {
        /* non secure caller spoofing as secure caller */
        access = SST_PERM_FORBIDDEN;
    } else {
        access = request_type;
    }
    return access;
}

/**
 * \brief Gets asset's permissions if the client is allowed
 *        based on the request_type
 *
 * \param[in] client_id     Client ID
 * \param[in] uuid          Asset's unique identifier
 * \param[in] request_type  Type of requested access
 *
 * \note If request_type contains multiple permissions, this function
 *       returns the entry pointer for specified asset if at least one
 *       of those permissions match.
 *
 * \return Returns the entry pointer for specified asset
 */
static struct sst_asset_policy_t *sst_am_get_db_entry(int32_t client_id,
                                                      uint32_t uuid,
                                                      uint8_t request_type)
{
    struct sst_asset_perm_t   *perm_entry;
    struct sst_asset_policy_t *db_entry;

    request_type = sst_am_check_s_ns_policy(client_id, request_type);

    /* security access violation */
    if (request_type == SST_PERM_FORBIDDEN) {
        /* FIXME: this is prone to timing attacks. Ideally the time
         * spent in this function should always be constant irrespective
         * of success or failure of checks. Timing attacks will be
         * addressed in later version.
         */
        return NULL;
    }

    /* Find policy db entry for the the asset */
    db_entry = sst_am_lookup_db_entry(uuid);
    if (db_entry == NULL) {
        return NULL;
    }

    if (request_type == SST_PERM_BYPASS) {
         return db_entry;
     }

    /* Find the client ID entry in the database */
    perm_entry = sst_am_lookup_client_perms(db_entry, client_id);
    if (perm_entry == NULL) {
        return NULL;
    }

     /* Check if the db permission matches with at least one of the
      * requested permissions types.
      */
    if ((perm_entry->perm & request_type) != 0) {
        return db_entry;
    }
    return NULL;
}

/**
 * \brief Validates the policy database's integrity
 *        Stub function.
 *
 * \return Returns value specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t validate_policy_db(void)
{
    /* Currently the policy database is inbuilt
     * in the code. It's sanity is assumed to be correct.
     * In the later revisions if access policy is
     * stored differently, it may require sanity check
     * as well.
     */
    return PSA_SST_ERR_SUCCESS;
}

/* Common interfaces for both modes */
static enum psa_sst_err_t _sst_am_create(int32_t client_id,
                                         uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    db_entry = sst_am_get_db_entry(client_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_create(asset_uuid, s_token, db_entry->type,
                            db_entry->max_size);

    return err;
}

static enum psa_sst_err_t
    _sst_am_get_info(int32_t client_id, uint32_t asset_uuid,
                     const struct tfm_sst_token_t *s_token,
                     struct psa_sst_asset_info_t *info)
{
    struct sst_asset_policy_t *db_entry;
    struct psa_sst_asset_info_t tmp_info;
    enum psa_sst_err_t err;
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;

    db_entry = sst_am_get_db_entry(client_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_get_info(asset_uuid, s_token, &tmp_info);
    if (err == PSA_SST_ERR_SUCCESS) {
        /* Use tmp_info to not leak information in case the previous function
         * returns and error. It avoids to leak information in case of error.
         * So, copy the tmp_info content into the attrs only if that tmp_info
         * data is valid.
         */
        sst_utils_memcpy(info, &tmp_info, PSA_SST_ASSET_INFO_SIZE);
    }

    return err;
}

static enum psa_sst_err_t
    _sst_am_get_attributes(int32_t client_id, uint32_t asset_uuid,
                           const struct tfm_sst_token_t *s_token,
                           struct psa_sst_asset_attrs_t *attrs)
{
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;
    struct sst_asset_policy_t *db_entry;
    enum psa_sst_err_t err;
    struct psa_sst_asset_attrs_t tmp_attrs;

    db_entry = sst_am_get_db_entry(client_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_get_attributes(asset_uuid, s_token, &tmp_attrs);
    if (err == PSA_SST_ERR_SUCCESS) {
        /* Use tmp_attrs to not leak information incase the previous function
         * returns and error. It avoids to leak information in case of error.
         * So, copy the tmp_attrs content into the attrs only if that tmp_attrs
         * data is valid.
         */
        sst_utils_memcpy(attrs, &tmp_attrs, PSA_SST_ASSET_ATTR_SIZE);
    }

    return err;
}

static enum psa_sst_err_t
    _sst_am_set_attributes(int32_t client_id, uint32_t asset_uuid,
                           const struct tfm_sst_token_t *s_token,
                           const struct psa_sst_asset_attrs_t *attrs)
{
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;
    struct sst_asset_policy_t *db_entry;
    enum psa_sst_err_t err;

    db_entry = sst_am_get_db_entry(client_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* FIXME: Validity attributes are not supported in the current service
     *        implementation. It is mandatory to set start and end subattributes
     *        to 0.
     */
    if (attrs->validity.start != 0 || attrs->validity.end != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    /* FIXME: Check which bit attributes have been changed and check if those
     *        can be modified or not.
     */
    err = sst_object_set_attributes(asset_uuid, s_token, attrs);

    return err;
}

static enum psa_sst_err_t _sst_am_read(int32_t caller_id, int32_t client_id,
                                       uint32_t asset_uuid,
                                       const struct tfm_sst_token_t *s_token,
                                       struct tfm_sst_buf_t *data)
{
    struct sst_asset_policy_t *db_entry;
    enum psa_sst_err_t err;

    /* Check if it is a read by reference request */
    if (client_id != SST_DIRECT_CLIENT_READ) {
        /* Only secure partitions can request it */
#ifdef TFM_PSA_API
        if (TFM_CLIENT_ID_IS_S(caller_id) == 1) {
#else
        if (sst_utils_validate_secure_caller() == PSA_SST_ERR_SUCCESS) {
#endif
            /* Reference read access requested, check if the client has
             * reference permission, otherwise reject the request.
             */
            db_entry = sst_am_get_db_entry(client_id, asset_uuid,
                                           SST_PERM_REFERENCE);
            if (db_entry == NULL) {
                return PSA_SST_ERR_ASSET_NOT_FOUND;
            }
        } else {
            /* A non-secure caller is not allowed to specify any client ID to
             * request a read by reference.
             */
            return PSA_SST_ERR_ASSET_NOT_FOUND;
        }
    }

    /* Check client ID permissions */
    db_entry = sst_am_get_db_entry(caller_id, asset_uuid, SST_PERM_READ);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    if (data->offset != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
#endif

    err = sst_object_read(asset_uuid, s_token, data->data,
                          data->offset, data->size);

    return err;
}

static enum psa_sst_err_t _sst_am_write(int32_t client_id, uint32_t asset_uuid,
                                        const struct tfm_sst_token_t *s_token,
                                        const struct tfm_sst_buf_t *data)
{
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    /* Check data pointer */
    if (data->data == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Check client ID permissions */
    db_entry = sst_am_get_db_entry(client_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Boundary check the incoming request */
    err = sst_utils_check_contained_in(0, db_entry->max_size,
                                       data->offset, data->size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    if (data->offset != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
#endif

    err = sst_object_write(asset_uuid, s_token, data->data,
                           data->offset, data->size);

    return err;
}

static enum psa_sst_err_t _sst_am_delete(int32_t client_id, uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    db_entry = sst_am_get_db_entry(client_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_delete(asset_uuid, s_token);

    return err;
}

#ifdef TFM_PSA_API
/**
 * SST IPC wrap functions mainly focus on getting caller input parameters
 * through ipc psa_read, and calling the corresponding service.
 */
static psa_status_t sst_am_create_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t asset_uuid;
    struct tfm_sst_token_t s_token;
    size_t num = 0, in_size[2];

    client_id = msg->client_id;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_am_create(client_id, asset_uuid, &s_token);
}

static psa_status_t sst_am_get_info_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t asset_uuid;
    struct tfm_sst_token_t s_token;
    struct psa_sst_asset_info_t info;
    psa_status_t status;
    size_t num = 0, in_size[2], out_size[1];

    client_id = msg->client_id;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    out_size[0] = msg->out_size[0];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t) ||
        out_size[0] != (sizeof(struct psa_sst_asset_info_t))) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    status = _sst_am_get_info(client_id, asset_uuid, &s_token, &info);
    if (!status) {
        psa_write(msg->handle, 0, &info, sizeof(struct psa_sst_asset_info_t));
    }
    return status;
}

static psa_status_t sst_am_get_attributes_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t asset_uuid;
    struct tfm_sst_token_t s_token;
    struct psa_sst_asset_attrs_t attrs;
    psa_status_t status;
    size_t num = 0, in_size[2], out_size[1];

    client_id = msg->client_id;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    out_size[0] = msg->out_size[0];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t) ||
        out_size[0] != sizeof(struct psa_sst_asset_attrs_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    status = _sst_am_get_attributes(client_id, asset_uuid, &s_token, &attrs);
    if (!status) {
        psa_write(msg->handle, 0, &attrs, sizeof(struct psa_sst_asset_attrs_t));
    }
    return status;
}

static psa_status_t sst_am_set_attributes_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t asset_uuid;
    struct tfm_sst_token_t s_token;
    struct psa_sst_asset_attrs_t attrs;
    size_t num = 0, in_size[3];

    client_id = msg->client_id;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    in_size[2] = msg->in_size[2];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t) ||
        in_size[2] != sizeof(struct psa_sst_asset_attrs_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 2, &attrs,
                   sizeof(struct psa_sst_asset_attrs_t));
    if (num != sizeof(struct psa_sst_asset_attrs_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_am_set_attributes(client_id, asset_uuid, &s_token, &attrs);
}

static psa_status_t sst_am_read_ipc_wrap(psa_msg_t *msg)
{
    int32_t caller_id;
    uint32_t offset, size;
    uint8_t data[SST_MAX_BUF_SIZE];
    struct tfm_sst_token_t s_token;
    struct tfm_sst_buf_t s_data = {0};
    struct tfm_sst_id_t s_id = {0};
    psa_status_t status;
    size_t num = 0, in_size[3];

    caller_id = msg->client_id;
    size = msg->out_size[0];
    s_data.data = data;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    in_size[2] = msg->in_size[2];
    if (in_size[0] != sizeof(struct tfm_sst_id_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t) ||
        in_size[2] != sizeof(uint32_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    num = psa_read(msg->handle, 0, &s_id, sizeof(struct tfm_sst_id_t));
    if (num != sizeof(struct tfm_sst_id_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 2, &offset, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    s_data.offset = offset;

    /**
     * Some test cases pass invalid thread name, need to call sst read
     * service first to return the corresponding error code regardless
     * of the size.
     */
    do {
        s_data.size = size;
        if (size > SST_MAX_BUF_SIZE) {
            s_data.size = SST_MAX_BUF_SIZE;
        }
        status = _sst_am_read(caller_id, s_id.client_id, s_id.asset_uuid,
                              &s_token, &s_data);
        if (!status) {
            psa_write(msg->handle, 0, s_data.data, s_data.size);
        }
        s_data.offset += s_data.size;
        size -= s_data.size;
    } while (size);
    return status;
}

static psa_status_t sst_am_write_ipc_wrap(psa_msg_t *msg)
{
    uint8_t buffer[SST_MAX_BUF_SIZE];
    int32_t client_id;
    uint32_t asset_uuid, offset, size;
    struct tfm_sst_token_t s_token;
    struct tfm_sst_buf_t data;
    psa_status_t status;
    size_t num = 0, in_size[3];

    client_id = msg->client_id;
    size = msg->in_size[3];
    data.data = buffer;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    in_size[2] = msg->in_size[2];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t) ||
        in_size[2] != sizeof(uint32_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 2, &offset, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    data.offset = offset;

    /**
     * Some test cases pass invalid thread name, need to call sst write
     * service first to return the corresponding error code regardless
     * of the size.
     */
    do {
        data.size = size;
        if (size > SST_MAX_BUF_SIZE) {
            data.size = SST_MAX_BUF_SIZE;
        }
        num = psa_read(msg->handle, 3, data.data, data.size);
        if (num != data.size) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
        status = _sst_am_write(client_id, asset_uuid, &s_token, &data);
        if (status != PSA_SST_ERR_SUCCESS) {
            return status;
        }

        size -= num;
        data.offset += num;
    } while (size);
    return status;
}

static psa_status_t sst_am_delete_ipc_wrap(psa_msg_t *msg)
{
    int32_t client_id;
    uint32_t asset_uuid;
    struct tfm_sst_token_t s_token;
    size_t num = 0, in_size[2];

    client_id = msg->client_id;
    in_size[0] = msg->in_size[0];
    in_size[1] = msg->in_size[1];
    if (in_size[0] != sizeof(uint32_t) ||
        in_size[1] != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
    num = psa_read(msg->handle, 0, &asset_uuid, sizeof(uint32_t));
    if (num != sizeof(uint32_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 1, &s_token, sizeof(struct tfm_sst_token_t));
    if (num != sizeof(struct tfm_sst_token_t)) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_am_delete(client_id, asset_uuid, &s_token);
}
#endif

enum psa_sst_err_t sst_am_prepare(void)
{
    enum psa_sst_err_t err;
    /* FIXME: outcome of this function should determine
     * state machine of asset manager. If this
     * step fails other APIs shouldn't entertain
     * any user calls. Not a major issue for now
     * as policy db check is a dummy function, and
     * sst core maintains it's own state machine.
     */

    /* Validate policy database */
    err = validate_policy_db();

    /* Initialize underlying storage system */
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_system_prepare();
#ifdef SST_CREATE_FLASH_LAYOUT
    /* If SST_CREATE_FLASH_LAYOUT is set, it indicates that it is required to
     * create a SST flash layout. SST service will generate an empty and valid
     * SST flash layout to store assets. It will erase all data located in the
     * assigned SST memory area before generating the SST layout.
     * This flag is required to be set if the SST memory area is located in a
     * non-persistent memory.
     * This flag can be set if the SST memory area is located in a persistent
     * memory without a previous valid SST flash layout in it. That is the case
     * when it is the first time in the device life that the SST service is
     * executed.
     */
    if (err != PSA_SST_ERR_SUCCESS) {
        /* Remove all data in the SST memory area and creates a valid SST flash
         * layout in that area.
         */
        sst_system_wipe_all();

        /* Attempt to initialise again */
        err = sst_system_prepare();
    }
#endif /* SST_CREATE_FLASH_LAYOUT */

    return err;
}

#ifdef TFM_PSA_API
static void sst_signal_handle(psa_signal_t signal, sst_func_t pfn)
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

enum psa_sst_err_t sst_am_init(void)
{
    enum psa_sst_err_t err;
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;
#endif

    err = sst_am_prepare();
#ifdef TFM_PSA_API
    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & PSA_SST_CREATE) {
            sst_signal_handle(PSA_SST_CREATE, sst_am_create_ipc_wrap);
        } else if (signals & PSA_SST_GET_INFO) {
            sst_signal_handle(PSA_SST_GET_INFO, sst_am_get_info_ipc_wrap);
        } else if (signals & PSA_SST_GET_ATTRIBUTES) {
            sst_signal_handle(PSA_SST_GET_ATTRIBUTES,
                              sst_am_get_attributes_ipc_wrap);
        } else if (signals & PSA_SST_SET_ATTRIBUTES) {
            sst_signal_handle(PSA_SST_SET_ATTRIBUTES,
                              sst_am_set_attributes_ipc_wrap);
        } else if (signals & PSA_SST_READ) {
            sst_signal_handle(PSA_SST_READ, sst_am_read_ipc_wrap);
        } else if (signals & PSA_SST_WRITE) {
            sst_signal_handle(PSA_SST_WRITE, sst_am_write_ipc_wrap);
        } else if (signals & PSA_SST_DELETE) {
            sst_signal_handle(PSA_SST_DELETE, sst_am_delete_ipc_wrap);
        } else {
            printf("signal is invalid!\r\n");
        }
    }
#endif
    return err;
}

/**
 * \brief Validate incoming iovec structure
 *
 * \param[in] src        Incoming iovec for the read/write request
 * \param[in] dest       Pointer to local copy of the iovec
 * \param[in] client_id  Client ID of the caller
 * \param[in] access     Access type to be permormed on the given dest->data
 *                       address
 *
 * \return Returns value specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t validate_copy_validate_iovec(
                                                const struct tfm_sst_buf_t *src,
                                                struct tfm_sst_buf_t *dest,
                                                int32_t client_id,
                                                uint32_t access)
{
    /* iovec struct needs to be used as veneers do not allow
     * more than four params.
     * First validate the pointer for iovec itself, then copy
     * the iovec, then validate the local copy of iovec.
     */
    enum psa_sst_err_t bound_check;

    bound_check = sst_utils_bound_check_and_copy((uint8_t *) src,
                                                 (uint8_t *) dest,
                                                 sizeof(struct tfm_sst_buf_t),
                                                 client_id);
    if (bound_check == PSA_SST_ERR_SUCCESS) {
        bound_check = sst_utils_memory_bound_check(dest->data, dest->size,
                                                   client_id, access);
    }

    return bound_check;
}

enum psa_sst_err_t sst_am_get_info(uint32_t asset_uuid,
                                   const struct tfm_sst_token_t *s_token,
                                   struct psa_sst_asset_info_t *info)
{
    enum psa_sst_err_t bound_check;
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    bound_check = sst_utils_memory_bound_check(info, PSA_SST_ASSET_INFO_SIZE,
                                               client_id,
                                               TFM_MEMORY_ACCESS_RW);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    return _sst_am_get_info(client_id, asset_uuid, s_token, info);
}

enum psa_sst_err_t sst_am_get_attributes(uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token,
                                         struct psa_sst_asset_attrs_t *attrs)
{
    enum psa_sst_err_t bound_check;
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    bound_check = sst_utils_memory_bound_check(attrs, PSA_SST_ASSET_ATTR_SIZE,
                                               client_id,
                                               TFM_MEMORY_ACCESS_RW);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
    return _sst_am_get_attributes(client_id, asset_uuid, s_token, attrs);
}

enum psa_sst_err_t sst_am_set_attributes(uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    enum psa_sst_err_t bound_check;
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    bound_check = sst_utils_memory_bound_check((uint8_t *)attrs,
                                               PSA_SST_ASSET_ATTR_SIZE,
                                               client_id,
                                               TFM_MEMORY_ACCESS_RO);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    return _sst_am_set_attributes(client_id, asset_uuid, s_token, attrs);
}

enum psa_sst_err_t sst_am_create(uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token)
{
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_am_create(client_id, asset_uuid, s_token);
}

enum psa_sst_err_t sst_am_read(int32_t client_id, uint32_t asset_uuid,
                               const struct tfm_sst_token_t *s_token,
                               struct tfm_sst_buf_t *data)
{
    int32_t caller_id;
    enum psa_sst_err_t err;
    struct tfm_sst_buf_t local_data;

    if (tfm_core_get_caller_client_id(&caller_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Make a local copy of the iovec data structure */
    err = validate_copy_validate_iovec(data, &local_data,
                                       caller_id, TFM_MEMORY_ACCESS_RW);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    return _sst_am_read(caller_id, client_id, asset_uuid, s_token, &local_data);
}

enum psa_sst_err_t sst_am_write(uint32_t asset_uuid,
                                const struct tfm_sst_token_t *s_token,
                                const struct tfm_sst_buf_t *data)
{
    struct tfm_sst_buf_t local_data;
    enum psa_sst_err_t err;
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Make a local copy of the iovec data structure */
    err = validate_copy_validate_iovec(data, &local_data,
                                       client_id, TFM_MEMORY_ACCESS_RO);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    return _sst_am_write(client_id, asset_uuid, s_token, &local_data);
}

enum psa_sst_err_t sst_am_delete(uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token)
{
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return _sst_am_delete(client_id, asset_uuid, s_token);
}
