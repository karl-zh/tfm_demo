/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_erpc.h"
#include "erpc_psa_api_server.h"
#include "erpc_common.h"
#include "erpc_mbf_setup.h"
#include "erpc_transport_setup.h"
#include "erpc_server_setup.h"

void tfm_erpc_init(struct rpmsg_endpoint *ep)
{
    void * transport = erpc_transport_rpmsg_openamp_init(ep);
    void * message_buffer_factory = erpc_mbf_static_init();

    erpc_server_init(transport ,message_buffer_factory);
}

void tfm_erpc_poll()
{
    erpc_server_poll();
}

void tfm_erpc_exit()
{
    erpc_server_stop();
}
