/*
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include "cmsis.h"
#include "flash_layout.h"

/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;

#define SPI_FLASH_ACCESS_LENGTH       (0x40000)

struct _qspi_ip6514e_reg_t {
    volatile uint32_t qspi_cfg;                           /*!< 0x00 (R/W) */
    volatile uint32_t device_read_inst;                   /*!< 0x04 (R/W) */
    volatile uint32_t device_write_inst;                  /*!< 0x08 (R/W) */
    volatile uint32_t hidden1[2];
    volatile uint32_t device_size;                        /*!< 0x14 (R/W) */
    volatile uint32_t hidden2;
    volatile uint32_t indirect_ahb_addr_trigger;          /*!< 0x1C (R/W) */
    volatile uint32_t hidden3;
    volatile uint32_t remap_addr;                         /*!< 0x24 (R/W) */
    volatile uint32_t hidden4;
    volatile uint32_t sram_fill_level;                    /*!< 0x2C (R/W) */
    volatile uint32_t hidden5[12];
    volatile uint32_t indirect_read_transfer;             /*!< 0x60 (R/W) */
    volatile uint32_t hidden6;
    volatile uint32_t indirect_read_transfer_start_addr;  /*!< 0x68 (R/W) */
    volatile uint32_t indirect_read_transfer_nbr_bytes;   /*!< 0x6C (R/W) */
    volatile uint32_t indirect_write_transfer;            /*!< 0x70 (R/W) */
    volatile uint32_t hidden7;
    volatile uint32_t indirect_write_transfer_start_addr; /*!< 0x78 (R/W) */
    volatile uint32_t indirect_write_transfer_nbr_bytes;  /*!< 0x7C (R/W) */
    volatile uint32_t indirect_trigger_addr_range;        /*!< 0x80 (R/W) */
    volatile uint32_t reserved1[3];
    volatile uint32_t flash_cmd_ctrl;                     /*!< 0x90 (R/W) */
    volatile uint32_t flash_cmd_addr;                     /*!< 0x94 (R/W) */
    volatile uint32_t reserved2[2];
    volatile uint32_t flash_cmd_read_data_lower;          /*!< 0xA0 (R/ ) */
    volatile uint32_t flash_cmd_read_data_upper;          /*!< 0xA4 (R/ ) */
    volatile uint32_t flash_cmd_write_data_lower;         /*!< 0xA8 (R/W) */
    volatile uint32_t flash_cmd_write_data_upper;         /*!< 0xAC (R/W) */
    volatile uint32_t hidden8[2];
};

#define  QSPI_CONTROLLER ((volatile struct _qspi_ip6514e_reg_t *)0x5010A000)

void Code_Copy_To_SRAM(void) {
    uint32_t Src, Dest, Len, i;

    Src  = FLASH_BASE_ADDRESS + FLASH_AREA_IMAGE_0_OFFSET;
    Dest = FLASH_AREA_IMAGE_0_OFFSET; //just the offset, in code sram
    Len  = FLASH_AREA_IMAGE_SIZE;
    __DSB();
    __ISB();

    for( ; Len > SPI_FLASH_ACCESS_LENGTH;
            Len = Len - SPI_FLASH_ACCESS_LENGTH) {

        QSPI_CONTROLLER->qspi_cfg &= ~1U; //disable controller
        QSPI_CONTROLLER->remap_addr = Src - FLASH_BASE_ADDRESS; //remap
        QSPI_CONTROLLER->qspi_cfg |= (1U << 16); //remap enable
        QSPI_CONTROLLER->qspi_cfg |= 1U; //controller enable
        __DSB();
        __ISB();

        for(i = 0; i < SPI_FLASH_ACCESS_LENGTH; i += 4) {
            *((uint32_t *)Dest) = 
               *((uint32_t *)(FLASH_BASE_ADDRESS + i));
            Dest += 4;
        }

        Src += SPI_FLASH_ACCESS_LENGTH;
    }
    __DSB();
   __ISB();

    QSPI_CONTROLLER->qspi_cfg &= ~1U; //disable controller
    QSPI_CONTROLLER->remap_addr = 0;
    QSPI_CONTROLLER->qspi_cfg |= (1U << 16); //remap enable
    QSPI_CONTROLLER->qspi_cfg |= 1U; //controller enable
    __DSB();
    __ISB();
    return;
}
