/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_plat_mhu.h"
#include <openamp/open_amp.h>
#include <metal/device.h>
#include "tfm_openamp.h"
#include "secure_utilities.h"

static metal_phys_addr_t shm_physmap[] = { SHM_START_ADDR };
static struct metal_device shm_device = {
    .name = SHM_DEVICE_NAME,
    .bus = NULL,
    .num_regions = 1,
    {
        {
            .virt       = (void *) SHM_START_ADDR,
            .physmap    = shm_physmap,
            .size       = SHM_SIZE,
            .page_shift = 0xffffffff,
            .page_mask  = 0xffffffff,
            .mem_flags  = 0,
            .ops        = { NULL },
        },
    },
    .node = { NULL },
    .irq_num = 0,
    .irq_info = NULL
};

static volatile unsigned int received_data;
static volatile unsigned int data_sem = 0;
static volatile unsigned int ept_sem = 0;
static volatile unsigned int data_rx_sem = 0;

static struct virtio_vring_info rvrings[2] = {
    [0] = {
        .info.align = VRING_ALIGNMENT,
    },
    [1] = {
        .info.align = VRING_ALIGNMENT,
    },
};
static struct virtio_device vdev;
static struct rpmsg_virtio_device rvdev;
static struct metal_io_region *io;
static struct virtqueue *vq[2];

#define WAIT_DATA_FOREVER do{while (data_sem == 0){} \
            data_sem--; virtqueue_notification(vq[0]);} \
            while(0)

static unsigned char virtio_get_status(struct virtio_device *vdev)
{
    return VIRTIO_CONFIG_STATUS_DRIVER_OK;
}

static void virtio_set_status(struct virtio_device *vdev, unsigned char status)
{
    LOG_MSG(__func__);
    *(uint32_t *)VDEV_STATUS_ADDR = status;
}

static uint32_t virtio_get_features(struct virtio_device *vdev)
{
    LOG_MSG(__func__);
    return 1 << VIRTIO_RPMSG_F_NS;
}

static void virtio_set_features(struct virtio_device *vdev,
                uint32_t features)
{
    LOG_MSG(__func__);
}

static void virtio_notify(struct virtqueue *vq)
{
    tfm_plat_mhu_set(ARM_MHU_CPU1, 0);
}

struct virtio_dispatch dispatch = {
    .get_status = virtio_get_status,
    .set_status = virtio_set_status,
    .get_features = virtio_get_features,
    .set_features = virtio_set_features,
    .notify = virtio_notify,
};

void MHU0_Handler(void)
{
    uint32_t doorbell_status;

    data_sem++;
    (void)tfm_plat_mhu_get_status(TFM_CPU0, &doorbell_status);
    (void)tfm_plat_mhu_clear(TFM_CPU0, doorbell_status);
}

int endpoint_cb(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv)
{
    received_data = *((unsigned int *) data);

    return RPMSG_SUCCESS;
}

struct rpmsg_endpoint my_ept;
struct rpmsg_endpoint *ep = &my_ept;

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;
    rpmsg_destroy_ept(ep);
}

void ns_bind_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
    (void)rpmsg_create_ept(ep, rdev, name,
            RPMSG_ADDR_ANY, dest,
            endpoint_cb,
            rpmsg_service_unbind);

    LOG_MSG(__func__);

    ept_sem++;
}

static struct rpmsg_virtio_shm_pool shpool;

void tfm_openamp_init(void)
{
    unsigned int message = 0U;
    int status = 0;
    struct metal_device *device;
    struct metal_init_params metal_params = METAL_INIT_DEFAULTS;

    LOG_MSG("\r\nOpenAMP[master] demo started\r\n");

    status = metal_init(&metal_params);
    if (status != 0) {
        LOG_MSG("metal_init: failed - error code\n");
        return;
    }

    status = metal_register_generic_device(&shm_device);
    if (status != 0) {
        LOG_MSG("Couldn't register shared memory device\n");
        return;
    }

    status = metal_device_open("generic", SHM_DEVICE_NAME, &device);
    if (status != 0) {
        LOG_MSG("metal_device_open failed\n");
        return;
    }

    io = metal_device_io_region(device, 0);
    if (io == NULL) {
        LOG_MSG("metal_device_io_region failed to get region\n");
        return;
    }

    /* setup vdev */
    vq[0] = virtqueue_allocate(VRING_SIZE);
    if (vq[0] == NULL) {
        LOG_MSG("virtqueue_allocate failed to alloc vq[0]\n");
        return;
    }
    vq[1] = virtqueue_allocate(VRING_SIZE);
    if (vq[1] == NULL) {
        LOG_MSG("virtqueue_allocate failed to alloc vq[1]\n");
        return;
    }

    vdev.role = RPMSG_MASTER;
    vdev.vrings_num = VRING_COUNT;
    vdev.func = &dispatch;
    rvrings[0].io = io;
    rvrings[0].info.vaddr = (void *)VRING_TX_ADDRESS;
    rvrings[0].info.num_descs = VRING_SIZE;
    rvrings[0].info.align = VRING_ALIGNMENT;
    rvrings[0].vq = vq[0];

    rvrings[1].io = io;
    rvrings[1].info.vaddr = (void *)VRING_RX_ADDRESS;
    rvrings[1].info.num_descs = VRING_SIZE;
    rvrings[1].info.align = VRING_ALIGNMENT;
    rvrings[1].vq = vq[1];

    vdev.vrings_info = &rvrings[0];

    /* setup rvdev */
    rpmsg_virtio_init_shm_pool(&shpool, (void *)SHM_START_ADDR, SHM_SIZE);
    status = rpmsg_init_vdev(&rvdev, &vdev, ns_bind_cb, io, &shpool);
    if (status != 0) {
        LOG_MSG("rpmsg_init_vdev failed\n");
        return;
    }
    LOG_MSG("rpmsg_init_vdev Done!\n");

    /* Since we are using name service, we need to wait for a response
     * from NS setup and than we need to process it
     */
    WAIT_DATA_FOREVER;
    while (ept_sem == 0){} ept_sem--;
    LOG_MSG("response to NS Done!\n");

    while (message < 100) {
        status = rpmsg_send(ep, &message, sizeof(message));
        if (status < 0) {
            LOG_MSG("send_message failed\r\n");
            goto _cleanup;
        }

        WAIT_DATA_FOREVER;
        message = received_data;
        printf("Master core received a message %d\r\n", message);

        message++;
    }

    _cleanup:
        rpmsg_deinit_vdev(&rvdev);
        metal_finish();

        LOG_MSG("OpenAMP demo ended.\n");
}
