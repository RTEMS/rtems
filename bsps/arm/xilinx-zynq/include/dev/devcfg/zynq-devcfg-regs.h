/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup zynq_devcfg
 * @brief Device configuration interface register definitions.
 */

/*
 * Copyright (c) 2016
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Florida.  All rights reserved.
 * Copyright (c) 2017
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Pittsburgh.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of CHREC.
 *
 * Author: Patrick Gauvin <gauvin@hcs.ufl.edu>
 */

/**
 * @defgroup zynq_devcfg_regs Device Configuration Interface Register Definitions
 * @ingroup zynq_devcfg
 * @brief Device Configuration Interface Register Definitions
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_REGS_H

#include <stdint.h>
#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Zynq-7000 series devcfg address */
#define ZYNQ_DEVCFG_BASE_ADDR 0xF8007000UL
/* For use with the PCAP DMA */
#define ZYNQ_DEVCFG_BITSTREAM_ADDR 0xFFFFFFFFUL
#define ZYNQ_DEVCFG_INTERRUPT_VECTOR 40

typedef struct {
  uint32_t ctrl;
#define ZYNQ_DEVCFG_CTRL_FORCE_RST( val ) BSP_FLD32( val, 31, 31 )
#define ZYNQ_DEVCFG_CTRL_FORCE_RST_GET( reg ) BSP_FLD32GET( reg, 31, 31 )
#define ZYNQ_DEVCFG_CTRL_PCFG_PROG_B_GET( reg ) BSP_FLD32GET( reg, 30, 30 )
#define ZYNQ_DEVCFG_CTRL_PCFG_PROG_B( val ) BSP_FLD32( val, 30, 30 )
#define ZYNQ_DEVCFG_CTRL_PCFG_POR_CNT_4K_GET( reg ) BSP_FLD32GET( reg, 29, 29 )
#define ZYNQ_DEVCFG_CTRL_PCFG_POR_CNT_4K( val ) BSP_FLD32( val, 29, 29 )
#define ZYNQ_DEVCFG_CTRL_PCAP_PR( val ) BSP_FLD32( val, 27, 27 )
#define ZYNQ_DEVCFG_CTRL_PCAP_PR_GET( reg ) BSP_FLD32GET( reg, 27, 27 )
#define ZYNQ_DEVCFG_CTRL_PCAP_PR_ICAP ( 0 )
#define ZYNQ_DEVCFG_CTRL_PCAP_PR_PCAP ( 1 )
#define ZYNQ_DEVCFG_CTRL_PCAP_MODE( val ) BSP_FLD32( val, 26, 26 )
#define ZYNQ_DEVCFG_CTRL_PCAP_MODE_GET( reg ) BSP_FLD32GET( reg, 26, 26 )
#define ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN( val ) BSP_FLD32( val, 25, 25 )
#define ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN_GET( reg ) \
  BSP_FLD32GET( reg, 25, 25 )
#define ZYNQ_DEVCFG_CTRL_MULTIBOOT_EN( val ) BSP_FLD32( val, 24, 24 )
#define ZYNQ_DEVCFG_CTRL_MULTIBOOT_EN_GET( reg ) BSP_FLD32GET( reg, 24, 24 )
#define ZYNQ_DEVCFG_CTRL_JTAG_CHAIN_DIS( val ) BSP_FLD32( val, 23, 23 )
#define ZYNQ_DEVCFG_CTRL_JTAG_CHAIN_DIS_GET( reg ) BSP_FLD32GET( reg, 23, 23 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_FUSE( val ) BSP_FLD32( val, 12, 12 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_FUSE_GET( reg ) BSP_FLD32GET( reg, 12, 12 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_FUSE_BBRAM ( 0 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_FUSE_EFUSE ( 1 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_EN( val ) BSP_FLD32( val, 9, 11 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_EN_GET( reg ) BSP_FLD32GET( reg, 9, 11 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_EN_ENABLE ( 0x3 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_EN_DISABLE ( 0x0 )
#define ZYNQ_DEVCFG_CTRL_PCFG_AES_EN_LOCKDOWN ( 0x1 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SEU_EN( val ) BSP_FLD32( val, 8, 8 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SEU_EN_GET( reg ) BSP_FLD32GET( reg, 8, 8 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SEC_EN_GET( reg ) BSP_FLD32GET( reg, 7, 7 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SPNIDEN( val ) BSP_FLD32( val, 6, 6 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SPNIDEN_GET( reg ) BSP_FLD32GET( reg, 6, 6 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SPIDEN( val ) BSP_FLD32( val, 5, 5 )
#define ZYNQ_DEVCFG_CTRL_PCFG_SPIDEN_GET( reg ) BSP_FLD32GET( reg, 5, 5 )
#define ZYNQ_DEVCFG_CTRL_PCFG_NIDEN( val ) BSP_FLD32( val, 4, 4 )
#define ZYNQ_DEVCFG_CTRL_PCFG_NIDEN_GET( reg ) BSP_FLD32GET( reg, 4, 4 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DBGEN( val ) BSP_FLD32( val, 3, 3 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DBGEN_GET( reg ) BSP_FLD32GET( reg, 3, 3 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DAP_EN( val ) BSP_FLD32( val, 0, 2 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DAP_EN_GET( reg ) BSP_FLD32GET( reg, 0, 2 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DAP_EN_ENABLE ( 0x3 )
#define ZYNQ_DEVCFG_CTRL_PCFG_DAP_EN_BYPASS ( 0x0 )
#define ZYNQ_DEVCFG_CTRL_RESERVED_BITS ( 0x6000 )
  uint32_t lock;
  uint32_t cfg;
  /* int_sts and int_mask directly overlap, so they share the ZYNQ_DEVCFG_INT_*
   * macros */
  uint32_t int_sts;
  uint32_t int_mask;
#define ZYNQ_DEVCFG_INT_PSS_CFG_RESET_B_INT BSP_BIT32( 27 )
#define ZYNQ_DEVCFG_INT_PSS_CFG_RESET_B_INT_GET( reg ) \
  BSP_FLD32GET( reg, 27, 27 )
#define ZYNQ_DEVCFG_INT_AXI_WERR_INT_GET( reg ) BSP_FLD32GET( reg, 22, 22 )
#define ZYNQ_DEVCFG_INT_AXI_RTO_INT_GET( reg ) BSP_FLD32GET( reg, 21, 21 )
#define ZYNQ_DEVCFG_INT_AXI_RERR_INT_GET( reg ) BSP_FLD32GET( reg, 20, 20 )
#define ZYNQ_DEVCFG_INT_RX_FIFO_OV_INT_GET( reg ) \
  BSP_FLD32GET( reg, 18, 18 )
#define ZYNQ_DEVCFG_INT_DMA_CMD_ERR_INT_GET( reg ) \
  BSP_FLD32GET( reg, 15, 15 )
#define ZYNQ_DEVCFG_INT_DMA_Q_OV_INT_GET( reg ) BSP_FLD32GET( reg, 14, 14 )
#define ZYNQ_DEVCFG_INT_DMA_DONE_INT BSP_BIT32( 13 )
#define ZYNQ_DEVCFG_INT_DMA_DONE_INT_GET( reg ) BSP_FLD32GET( reg, 13, 13 )
#define ZYNQ_DEVCFG_INT_D_P_DONE_INT BSP_BIT32( 12 )
#define ZYNQ_DEVCFG_INT_D_P_DONE_INT_GET( reg ) BSP_FLD32GET( reg, 12, 12 )
#define ZYNQ_DEVCFG_INT_P2D_LEN_ERR_INT_GET( reg ) \
  BSP_FLD32GET( reg, 11, 11 )
#define ZYNQ_DEVCFG_INT_PCFG_HMAC_ERR_INT_GET( reg ) \
  BSP_FLD32GET( reg, 6, 6 )
#define ZYNQ_DEVCFG_INT_PCFG_SEU_ERR_INT_GET( reg ) \
  BSP_FLD32GET( reg, 5, 5 )
#define ZYNQ_DEVCFG_INT_PCFG_POR_B_INT_GET( reg ) BSP_FLD32GET( reg, 4, 4 )
#define ZYNQ_DEVCFG_INT_PCFG_CFG_RST_INT_GET( reg ) \
  BSP_FLD32GET( reg, 3, 3 )
#define ZYNQ_DEVCFG_INT_PCFG_DONE_INT BSP_BIT32( 2 )
#define ZYNQ_DEVCFG_INT_PCFG_DONE_INT_GET( reg ) BSP_FLD32GET( reg, 2, 2 )
#define ZYNQ_DEVCFG_INT_PCFG_INIT_PE_INT BSP_BIT32( 1 )
#define ZYNQ_DEVCFG_INT_PCFG_INIT_PE_INT_GET( reg ) \
  BSP_FLD32GET( reg, 1, 1 )
#define ZYNQ_DEVCFG_INT_PCFG_INIT_NE_INT BSP_BIT32( 0 )
#define ZYNQ_DEVCFG_INT_PCFG_INIT_NE_INT_GET( reg ) \
  BSP_FLD32GET( reg, 0, 0 )
#define ZYNQ_DEVCFG_INT_ALL ( 0xf8f7f87f )
  uint32_t status;
#define ZYNQ_DEVCFG_STATUS_DMA_CMD_Q_F( val ) BSP_FLD32( val, 31, 31 )
#define ZYNQ_DEVCFG_STATUS_DMA_CMD_Q_F_GET( reg ) BSP_FLD32GET( reg, 31, 31 )
#define ZYNQ_DEVCFG_STATUS_PCFG_INIT_GET( reg ) BSP_FLD32GET( reg, 4, 4 )
  uint32_t dma_src_addr;
#define ZYNQ_DEVCFG_DMA_SRC_ADDR_DMA_DONE_INT_WAIT_PCAP ( 0x1 )
  uint32_t dma_dst_addr;
#define ZYNQ_DEVCFG_DMA_DST_ADDR_DMA_DONE_INT_WAIT_PCAP ( 0x1 )
  uint32_t dma_src_len;
#define ZYNQ_DEVCFG_DMA_SRC_LEN_LEN( val ) BSP_FLD32( val, 0, 26 )
  uint32_t dma_dest_len; /* (sic) */
#define ZYNQ_DEVCFG_DMA_DEST_LEN_LEN( val ) BSP_FLD32( val, 0, 26 )
  uint32_t reserved0;
  uint32_t multiboot_addr;
  uint32_t reserved1;
  uint32_t unlock;
  uint32_t reserved2[18];
  uint32_t mctrl;
#define ZYNQ_DEVCFG_MCTRL_PS_VERSION_GET( reg ) BSP_FLD32GET( reg, 28, 31 )
#define ZYNQ_DEVCFG_MCTRL_PS_VERSION_1_0 0x0
#define ZYNQ_DEVCFG_MCTRL_PS_VERSION_2_0 0x1
#define ZYNQ_DEVCFG_MCTRL_PS_VERSION_3_0 0x2
#define ZYNQ_DEVCFG_MCTRL_PS_VERSION_3_1 0x3
#define ZYNQ_DEVCFG_MCTRL_PCFG_POR_B_GET( reg ) BSP_FLD32GET( reg, 8, 8 )
#define ZYNQ_DEVCFG_MCTRL_INT_PCAP_LPBK_GET( reg ) BSP_FLD32GET( reg, 4, 4 )
#define ZYNQ_DEVCFG_MCTRL_INT_PCAP_LPBK( val ) BSP_FLD32( val, 4, 4 )
#define ZYNQ_DEVCFG_MCTRL_RESERVED_SET_BITS ( 0x800000 )
#define ZYNQ_DEVCFG_MCTRL_RESERVED_UNSET_BITS ( 0x3 )
#define ZYNQ_DEVCFG_MCTRL_SET( reg, val ) ( ( ( reg ) & \
  ~ZYNQ_DEVCFG_MCTRL_RESERVED_UNSET_BITS ) | \
  ZYNQ_DEVCFG_MCTRL_RESERVED_SET_BITS | ( val ) )
  uint32_t reserved3[32];
  uint32_t xadcif_cfg;
  uint32_t xadcif_int_sts;
  uint32_t xadcif_int_mask;
  uint32_t xadcif_msts;
  uint32_t xadcif_cmdfifo;
  uint32_t xadcif_rdfifo;
  uint32_t xadcif_mctrl;
} zynq_devcfg_regs;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_REGS_H */
