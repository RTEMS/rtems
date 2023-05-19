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

#ifndef IMX_SRCREG_H
#define IMX_SRCREG_H

#include <bsp/utility.h>

typedef struct {
	uint32_t scr;
#define IMX_SRC_SCR_DOM_EN BSP_BIT32(31)
#define IMX_SRC_SCR_LOCK BSP_BIT32(30)
#define IMX_SRC_SCR_DOMAIN3 BSP_BIT32(27)
#define IMX_SRC_SCR_DOMAIN2 BSP_BIT32(26)
#define IMX_SRC_SCR_DOMAIN1 BSP_BIT32(25)
#define IMX_SRC_SCR_DOMAIN0 BSP_BIT32(24)
#define IMX_SRC_SCR_MASK_TEMPSENSE_RESET(val) BSP_FLD32(val, 4, 7)
#define IMX_SRC_SCR_MASK_TEMPSENSE_RESET_GET(reg) BSP_FLD32GET(reg, 4, 7)
#define IMX_SRC_SCR_MASK_TEMPSENSE_RESET_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
	uint32_t a7rcr0;
#define IMX_SRC_A7RCR0_DOM_EN BSP_BIT32(31)
#define IMX_SRC_A7RCR0_LOCK BSP_BIT32(30)
#define IMX_SRC_A7RCR0_DOMAIN3 BSP_BIT32(27)
#define IMX_SRC_A7RCR0_DOMAIN2 BSP_BIT32(26)
#define IMX_SRC_A7RCR0_DOMAIN1 BSP_BIT32(25)
#define IMX_SRC_A7RCR0_DOMAIN0 BSP_BIT32(24)
#define IMX_SRC_A7RCR0_A7_L2RESET BSP_BIT32(21)
#define IMX_SRC_A7RCR0_A7_SOC_DBG_RESET BSP_BIT32(20)
#define IMX_SRC_A7RCR0_MASK_WDOG1_RST(val) BSP_FLD32(val, 16, 19)
#define IMX_SRC_A7RCR0_MASK_WDOG1_RST_GET(reg) BSP_FLD32GET(reg, 16, 19)
#define IMX_SRC_A7RCR0_MASK_WDOG1_RST_SET(reg, val) BSP_FLD32SET(reg, val, 16, 19)
#define IMX_SRC_A7RCR0_A7_ETM_RESET1 BSP_BIT32(13)
#define IMX_SRC_A7RCR0_A7_ETM_RESET0 BSP_BIT32(12)
#define IMX_SRC_A7RCR0_A7_DBG_RESET1 BSP_BIT32(9)
#define IMX_SRC_A7RCR0_A7_DBG_RESET0 BSP_BIT32(8)
#define IMX_SRC_A7RCR0_A7_CORE_RESET1 BSP_BIT32(5)
#define IMX_SRC_A7RCR0_A7_CORE_RESET0 BSP_BIT32(4)
#define IMX_SRC_A7RCR0_A7_CORE_POR_RESET1 BSP_BIT32(1)
#define IMX_SRC_A7RCR0_A7_CORE_POR_RESET0 BSP_BIT32(0)
	uint32_t a7rcr1;
#define IMX_SRC_A7RCR1_DOM_EN BSP_BIT32(31)
#define IMX_SRC_A7RCR1_LOCK BSP_BIT32(30)
#define IMX_SRC_A7RCR1_DOMAIN3 BSP_BIT32(27)
#define IMX_SRC_A7RCR1_DOMAIN2 BSP_BIT32(26)
#define IMX_SRC_A7RCR1_DOMAIN1 BSP_BIT32(25)
#define IMX_SRC_A7RCR1_DOMAIN0 BSP_BIT32(24)
#define IMX_SRC_A7RCR1_A7_CORE1_ENABLE BSP_BIT32(1)
	uint32_t m4rcr;
#define IMX_SRC_M4RCR_DOM_EN BSP_BIT32(31)
#define IMX_SRC_M4RCR_LOCK BSP_BIT32(30)
#define IMX_SRC_M4RCR_DOMAIN3 BSP_BIT32(27)
#define IMX_SRC_M4RCR_DOMAIN2 BSP_BIT32(26)
#define IMX_SRC_M4RCR_DOMAIN1 BSP_BIT32(25)
#define IMX_SRC_M4RCR_DOMAIN0 BSP_BIT32(24)
#define IMX_SRC_M4RCR_WDOG3_RST_OPTION BSP_BIT32(9)
#define IMX_SRC_M4RCR_WDOG3_RST_OPTION_M4 BSP_BIT32(8)
#define IMX_SRC_M4RCR_MASK_WDOG3_RST(val) BSP_FLD32(val, 4, 7)
#define IMX_SRC_M4RCR_MASK_WDOG3_RST_GET(reg) BSP_FLD32GET(reg, 4, 7)
#define IMX_SRC_M4RCR_MASK_WDOG3_RST_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define IMX_SRC_M4RCR_ENABLE_M4 BSP_BIT32(3)
#define IMX_SRC_M4RCR_SW_M4P_RST BSP_BIT32(2)
#define IMX_SRC_M4RCR_SW_M4C_RST BSP_BIT32(1)
#define IMX_SRC_M4RCR_SW_M4C_NON_SCLR_RST BSP_BIT32(0)
	uint32_t reserved_10;
	uint32_t ercr;
	uint32_t reserved_18;
	uint32_t hsicphy_rcr;
	uint32_t usbophy1_rcr;
	uint32_t usbophy2_rcr;
	uint32_t mipiphy_rcr;
	uint32_t pciephy_rcr;
	uint32_t reserved_30[10];
	uint32_t sbmr1;
	uint32_t srsr;
	uint32_t reserved_60[2];
	uint32_t sisr;
	uint32_t simr;
	uint32_t sbmr2;
	uint32_t gpr1;
	uint32_t gpr2;
	uint32_t gpr3;
	uint32_t gpr4;
	uint32_t gpr5;
	uint32_t gpr6;
	uint32_t gpr7;
	uint32_t gpr8;
	uint32_t gpr9;
	uint32_t gpr10;
	uint32_t reserved_9c[985];
	uint32_t ddrc_rcr;
} imx_src;

#endif /* IMX_SRCREG_H */
