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
#include "secure_utilities.h"

/*!
* @brief erpcMatrixMultiply function implementation.
 *
 * This is the implementation of the erpcMatrixMultiply function called by the primary core.
 *
 * @param matrix1 First matrix
 * @param matrix2 Second matrix
 * @param result_matrix Result matrix
 */
void erpcMatrixMultiply(Matrix matrix1, Matrix matrix2, Matrix result_matrix)
{
    int32_t i, j, k;
    const int32_t matrix_size = 5;

    LOG_MSG("Calculating the matrix multiplication...\r\n");

    /* Clear the result matrix */
    for (i = 0; i < matrix_size; ++i)
    {
        for (j = 0; j < matrix_size; ++j)
        {
           result_matrix[i][j] = 0;
        }
    }

    /* Multiply two matrices */
    for (i = 0; i < matrix_size; ++i)
    {
        for (j = 0; j < matrix_size; ++j)
        {
            for (k = 0; k < matrix_size; ++k)
            {
                result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    LOG_MSG("Done!\r\n");
}

void tfm_erpc_init(struct rpmsg_endpoint *ep)
{
    void * transport = erpc_transport_rpmsg_openamp_init(ep);
    void * message_buffer_factory = erpc_mbf_static_init();

    erpc_server_init(transport ,message_buffer_factory);

    /* adding the service to the server */
    erpc_add_service_to_server(create_MatrixMultiplyService_service());
    erpc_add_service_to_server(create_PsaFrameworkVersionService_service());
    erpc_add_service_to_server(create_PsaVersionService_service());
    erpc_add_service_to_server(create_PsaConnectService_service());
    erpc_add_service_to_server(create_PsaCallService_service());
    erpc_add_service_to_server(create_PsaCloseService_service());

    LOG_MSG("eRPC services added\r\n");
}

void tfm_erpc_poll()
{
    erpc_server_poll();
}

void tfm_erpc_exit()
{
    erpc_server_stop();
}
