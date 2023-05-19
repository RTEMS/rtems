/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IMX_GPCREG_H
#define IMX_GPCREG_H

#include <bsp/utility.h>

typedef struct {
	uint32_t lpcr_a7_bsc;
	uint32_t lpcr_a7_ad;
	uint32_t lpcr_m4;
	uint32_t reserved_0c[2];
	uint32_t slpcr;
	uint32_t reserved_18[2];
	uint32_t mlpcr;
	uint32_t pgc_ack_sel_a7;
	uint32_t pgc_ack_sel_m4;
	uint32_t misc;
	uint32_t imr1_core0_a7;
	uint32_t imr2_core0_a7;
	uint32_t imr3_core0_a7;
	uint32_t imr4_core0_a7;
	uint32_t imr1_core1_a7;
	uint32_t imr2_core1_a7;
	uint32_t imr3_core1_a7;
	uint32_t imr4_core1_a7;
	uint32_t imr1_m4;
	uint32_t imr2_m4;
	uint32_t imr3_m4;
	uint32_t imr4_m4;
	uint32_t reserved_60[4];
	uint32_t isr1_a7;
	uint32_t isr2_a7;
	uint32_t isr3_a7;
	uint32_t isr4_a7;
	uint32_t isr1_m4;
	uint32_t isr2_m4;
	uint32_t isr3_m4;
	uint32_t isr4_m4;
	uint32_t reserved_90[8];
	uint32_t slt0_cfg;
	uint32_t slt1_cfg;
	uint32_t slt2_cfg;
	uint32_t slt3_cfg;
	uint32_t slt4_cfg;
	uint32_t slt5_cfg;
	uint32_t slt6_cfg;
	uint32_t slt7_cfg;
	uint32_t slt8_cfg;
	uint32_t slt9_cfg;
	uint32_t reserved_d8[5];
	uint32_t pgc_cpu_mapping;
#define IMX_GPC_CPU_PGC_SCU_A7 BSP_BIT32(2)
#define IMX_GPC_CPU_PGC_CORE1_A7 BSP_BIT32(1)
#define IMX_GPC_CPU_PGC_CORE0_A7 BSP_BIT32(0)
#define IMX_GPC_PU_PGC_USB_HSIC_PHY BSP_BIT32(4)
#define IMX_GPC_PU_PGC_USB_OTG2_PHY BSP_BIT32(3)
#define IMX_GPC_PU_PGC_USB_OTG1_PHY BSP_BIT32(2)
#define IMX_GPC_PU_PGC_PCIE_PHY BSP_BIT32(1)
#define IMX_GPC_PU_PGC_MIPI_PHY BSP_BIT32(0)
	uint32_t cpu_pgc_sw_pup_req;
	uint32_t reserved_f4;
	uint32_t pu_pgc_sw_pup_req;
	uint32_t cpu_pgc_sw_pdn_req;
	uint32_t reserved_100;
	uint32_t pu_pgc_sw_pdn_req;
	uint32_t reserved_108[10];
	uint32_t cpu_pgc_pup_status1;
	uint32_t a7_mix_pgc_pup_status0;
	uint32_t a7_mix_pgc_pup_status1;
	uint32_t a7_mix_pgc_pup_status2;
	uint32_t m4_mix_pgc_pup_status0;
	uint32_t m4_mix_pgc_pup_status1;
	uint32_t m4_mix_pgc_pup_status2;
	uint32_t a7_pu_pgc_pup_status0;
	uint32_t a7_pu_pgc_pup_status1;
	uint32_t a7_pu_pgc_pup_status2;
	uint32_t m4_pu_pgc_pup_status0;
	uint32_t m4_pu_pgc_pup_status1;
	uint32_t m4_pu_pgc_pup_status2;
	uint32_t reserved_164[3];
	uint32_t cpu_pgc_pdn_status1;
	uint32_t reserved_174[6];
	uint32_t a7_pu_pgc_pdn_status0;
	uint32_t a7_pu_pgc_pdn_status1;
	uint32_t a7_pu_pgc_pdn_status2;
	uint32_t m4_pu_pgc_pdn_status0;
	uint32_t m4_pu_pgc_pdn_status1;
	uint32_t m4_pu_pgc_pdn_status2;
	uint32_t reserved_1a4[3];
	uint32_t a7_mix_pdn_flg;
	uint32_t a7_pu_pdn_flg;
	uint32_t m4_mix_pdn_flg;
	uint32_t m4_pu_pdn_flg;
#define IMX_GPC_PGC_CTRL_MEMPWR_TCD1_TDR_TRM(val) BSP_FLD32(val, 24, 29)
#define IMX_GPC_PGC_CTRL_MEMPWR_TCD1_TDR_TRM_GET(reg) BSP_FLD32GET(reg, 24, 29)
#define IMX_GPC_PGC_CTRL_MEMPWR_TCD1_TDR_TRM_SET(reg, val) BSP_FLD32SET(reg, val, 24, 29)
#define IMX_GPC_PGC_CTRL_L2RETN_TCD1_TDR(val) BSP_FLD32(val, 16, 21)
#define IMX_GPC_PGC_CTRL_L2RETN_TCD1_TDR_GET(reg) BSP_FLD32GET(reg, 16, 21)
#define IMX_GPC_PGC_CTRL_L2RETN_TCD1_TDR_SET(reg, val) BSP_FLD32SET(reg, val, 16, 21)
#define IMX_GPC_PGC_CTRL_DFTRAM_TCD1(val) BSP_FLD32(val, 8, 13)
#define IMX_GPC_PGC_CTRL_DFTRAM_TCD1_GET(reg) BSP_FLD32GET(reg, 8, 13)
#define IMX_GPC_PGC_CTRL_DFTRAM_TCD1_SET(reg, val) BSP_FLD32SET(reg, val, 8, 13)
#define IMX_GPC_PGC_CTRL_L2RSTDIS(val) BSP_FLD32(val, 1, 6)
#define IMX_GPC_PGC_CTRL_L2RSTDIS_GET(reg) BSP_FLD32GET(reg, 1, 6)
#define IMX_GPC_PGC_CTRL_L2RSTDIS_SET(reg, val) BSP_FLD32SET(reg, val, 1, 6)
#define IMX_GPC_PGC_CTRL_PCR BSP_BIT32(0)
	uint32_t reserved_1c0[400];
	uint32_t pgc_a7core0_ctrl;
	uint32_t pgc_a7core0_pupscr;
	uint32_t pgc_a7core0_pdnscr;
	uint32_t pgc_a7core0_sr;
	uint32_t reserved_810[12];
	uint32_t pgc_a7core1_ctrl;
	uint32_t pgc_a7core1_pupscr;
	uint32_t pgc_a7core1_pdnscr;
	uint32_t pgc_a7core1_sr;
	uint32_t reserved_850[12];
	uint32_t pgc_a7scu_ctrl;
	uint32_t pgc_a7scu_pupscr;
	uint32_t pgc_a7scu_pdnscr;
	uint32_t pgc_a7scu_sr;
	uint32_t pgc_scu_auxsw;
	uint32_t reserved_894[11];
	uint32_t pgc_mix_ctrl;
	uint32_t pgc_mix_pupscr;
	uint32_t pgc_mix_pdnscr;
	uint32_t pgc_mix_sr;
	uint32_t reserved_8d0[12];
	uint32_t pgc_mipi_ctrl;
	uint32_t pgc_mipi_pupscr;
	uint32_t pgc_mipi_pdnscr;
	uint32_t pgc_mipi_sr;
	uint32_t reserved_910[12];
	uint32_t pgc_pcie_ctrl;
	uint32_t pgc_pcie_pupscr;
	uint32_t pgc_pcie_pdnscr;
	uint32_t pgc_pcie_sr;
	uint32_t reserved_950[176];
	uint32_t pgc_mipi_auxsw;
	uint32_t reserved_c14[15];
	uint32_t pgc_pcie_auxsw;
	uint32_t reserved_c54[43];
	uint32_t pgc_hsic_ctrl;
	uint32_t pgc_hsic_pupscr;
	uint32_t pgc_hsic_pdnscr;
	uint32_t pgc_hsic_sr;
} imx_gpc;

#endif /* IMX_GPCREG_H */
