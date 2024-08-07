/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ALTERA_AVALON_EPCQ_REGS_H
#define _ALTERA_AVALON_EPCQ_REGS_H

#include <stdbool.h>
#include <bsp_system.h>

/*
 * EPCQ_RD_STATUS register description macros
 */

/** Write in progress bit */
#define ALTERA_EPCQ_CONTROLLER2_STATUS_WIP_MASK                  (0x00000001)
#define ALTERA_EPCQ_CONTROLLER2_STATUS_WIP_AVAILABLE             (0x00000000)
#define ALTERA_EPCQ_CONTROLLER2_STATUS_WIP_BUSY                  (0x00000001)
/** When to time out a poll of the write in progress bit */
/* 0.7 sec time out */
#define ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE                700000

/*
 * EPCQ_RD_SID register description macros
 *
 * Specific device values obtained from Table 14 of:
 * "Serial Configuration (EPCS) Devices Datasheet"
 */
#define ALTERA_EPCQ_CONTROLLER2_SID_MASK                         (0x000000FF)
#define ALTERA_EPCQ_CONTROLLER2_SID_EPCS16                       (0x00000014)
#define ALTERA_EPCQ_CONTROLLER2_SID_EPCS64                       (0x00000016)
#define ALTERA_EPCQ_CONTROLLER2_SID_EPCS128                      (0x00000018)

/*
 * EPCQ_RD_RDID register description macros
 *
 * Specific device values obtained from Table 28 of:
 *  "Quad-Serial Configuration
 *  (EPCQ (www.altera.com/literature/hb/cfg/cfg_cf52012.pdf))
 *  Devices Datasheet"
 */
#define ALTERA_EPCQ_CONTROLLER2_RDID_MASK                         (0x000000FF)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ16                       (0x00000015)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ32                       (0x00000016)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ64                       (0x00000017)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ128                      (0x00000018)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ256                      (0x00000019)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ512                      (0x00000020)
#define ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ1024                     (0x00000021)

/*
 * EPCQ_MEM_OP register description macros
 */
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_CMD_MASK                  (0x00000003)
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_BULK_ERASE_CMD            (0x00000001)
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_ERASE_CMD          (0x00000002)
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_PROTECT_CMD        (0x00000003)
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_WRITE_ENABLE_CMD          (0x00000004)

/** see datasheet for sector values */
#define ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_VALUE_MASK         (0x00FFFF00)

/*
 * EPCQ_ISR register description macros
 */
#define ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_ERASE_MASK           (0x00000001)
#define ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_ERASE_ACTIVE         (0x00000001)

#define ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_WRITE_MASK           (0x00000002)
#define ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_WRITE_ACTIVE         (0x00000002)

/*
 * EPCQ_IMR register description macros
 */
#define ALTERA_EPCQ_CONTROLLER2_IMR_ILLEGAL_ERASE_MASK           (0x00000001)
#define ALTERA_EPCQ_CONTROLLER2_IMR_ILLEGAL_ERASE_ENABLED        (0x00000001)

#define ALTERA_EPCQ_CONTROLLER2_IMR_ILLEGAL_WRITE_MASK           (0x00000002)
#define ALTERA_EPCQ_CONTROLLER2_IMR_ILLEGAL_WRITE_ENABLED        (0x00000002)

/*
 * EPCQ_CHIP_SELECT register description macros
 */
#define ALTERA_EPCQ_CHIP1_SELECT        (0x00000001)
#define ALTERA_EPCQ_CHIP2_SELECT        (0x00000002)
#define ALTERA_EPCQ_CHIP3_SELECT        (0x00000003)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  volatile uint32_t rd_status;
  volatile uint32_t rd_sid;
  volatile uint32_t rd_rdid;
  volatile uint32_t mem_op;
  volatile uint32_t isr;
  volatile uint32_t imr;
  volatile uint32_t chip_select;
  volatile uint32_t flag_status;
  volatile uint32_t dev_id_0;
  volatile uint32_t dev_id_1;
  volatile uint32_t dev_id_2;
  volatile uint32_t dev_id_3;
  volatile uint32_t dev_id_4;
}altera_avalon_epcq_regs;

#define EPCQ_REGS \
  (( volatile altera_avalon_epcq_regs* )EPCQ_CONTROLLER_AVL_CSR_BASE )
#define EPCQ_MEM \
  (( volatile uint8_t* )EPCQ_CONTROLLER_AVL_MEM_BASE )
#define EPCQ_MEM_32 \
  (( volatile uint32_t* )EPCQ_CONTROLLER_AVL_MEM_BASE )

void epcq_initialize( void );
int epcq_read_buffer( int offset, uint8_t *dest_addr, int length );
int epcq_write_buffer (
  int offset,
  const uint8_t* src_addr,
  int length,
  bool erase
);

#ifdef __cplusplus
}
#endif

#endif
