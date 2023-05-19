/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#ifndef IMX6UL_CCMREG_H
#define IMX6UL_CCMREG_H

#include <bsp/utility.h>

typedef struct {
	uint32_t ccr;
	uint32_t ccdr;
	uint32_t csr;
	uint32_t ccsr;
	uint32_t cacrr;
	uint32_t cbcdr;
	uint32_t cbcmr;
	uint32_t cscmr1;
	uint32_t cscmr2;
	uint32_t cscdr1;
	uint32_t cs1cdr;
	uint32_t cs2cdr;
	uint32_t cdcdr;
	uint32_t chsccdr;
	uint32_t cscdr2;
	uint32_t cscdr3;
	uint32_t reserved_40[2];
	uint32_t cdhipr;
	uint32_t reserved_4c[2];
	uint32_t clpcr;
	uint32_t cisr;
	uint32_t cimr;
	uint32_t ccosr;
	uint32_t cgpr;
	uint32_t ccgr0;
	uint32_t ccgr1;
	uint32_t ccgr2;
	uint32_t ccgr3;
	uint32_t ccgr4;
	uint32_t ccgr5;
	uint32_t ccgr6;
	uint32_t reserved_84[1];
	uint32_t cmeor;
} imx6ul_ccm;

typedef struct {
	uint32_t pll_arm;
	uint32_t pll_arm_set;
	uint32_t pll_arm_clr;
	uint32_t pll_arm_tog;
	uint32_t pll_usb1;
	uint32_t pll_usb1_set;
	uint32_t pll_usb1_clr;
	uint32_t pll_usb1_tog;
	uint32_t pll_usb2;
	uint32_t pll_usb2_set;
	uint32_t pll_usb2_clr;
	uint32_t pll_usb2_tog;
	uint32_t pll_sys;
	uint32_t pll_sys_set;
	uint32_t pll_sys_clr;
	uint32_t pll_sys_tog;
	uint32_t pll_sys_ss;
	uint32_t reserved_44[3];
	uint32_t pll_sys_num;
	uint32_t reserved_54[3];
	uint32_t pll_sys_denom;
	uint32_t reserved_64[3];
	uint32_t pll_audio;
	uint32_t pll_audio_set;
	uint32_t pll_audio_clr;
	uint32_t pll_audio_tog;
	uint32_t pll_audio_num;
	uint32_t reserved_84[3];
	uint32_t pll_audio_denom;
	uint32_t reserved_94[3];
	uint32_t pll_video;
	uint32_t pll_video_set;
	uint32_t pll_video_clr;
	uint32_t pll_video_tog;
	uint32_t pll_video_num;
	uint32_t reserved_b4[3];
	uint32_t pll_video_denom;
	uint32_t reserved_c4[7];
	uint32_t pll_enet;
#define IMX6UL_CCM_ANALOG_PLL_ENET_LOCK BSP_BIT32(31)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN BSP_BIT32(21)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET2_125M_EN BSP_BIT32(20)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENABLE_125M BSP_BIT32(19)
#define IMX6UL_CCM_ANALOG_PLL_ENET_PFD_OFFSET_EN BSP_BIT32(18)
#define IMX6UL_CCM_ANALOG_PLL_ENET_BYPASS BSP_BIT32(16)
#define IMX6UL_CCM_ANALOG_PLL_ENET_BYPASS_CLK_SRC(val) BSP_FLD32(val, 14, 15)
#define IMX6UL_CCM_ANALOG_PLL_ENET_BYPASS_CLK_SRC_GET(val) BSP_FLD32GET(val, 14, 15)
#define IMX6UL_CCM_ANALOG_PLL_ENET_BYPASS_CLK_SRC_SET(val) BSP_FLD32SET(val, 14, 15)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET1_125M_EN BSP_BIT32(13)
#define IMX6UL_CCM_ANALOG_PLL_ENET_POWERDOWN BSP_BIT32(12)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET1_DIV_SELECT(val) BSP_FLD32(val, 3, 2)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET1_DIV_SELECT_GET(val) BSP_FLD32GET(val, 3, 2)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET1_DIV_SELECT_SET(val) BSP_FLD32SET(val, 3, 2)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET0_DIV_SELECT(val) BSP_FLD32(val, 0, 1)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET0_DIV_SELECT_GET(val) BSP_FLD32GET(val, 0, 1)
#define IMX6UL_CCM_ANALOG_PLL_ENET_ENET0_DIV_SELECT_SET(val) BSP_FLD32SET(val, 0, 1)
	uint32_t pll_enet_set;
	uint32_t pll_enet_clr;
	uint32_t pll_enet_tog;
	uint32_t pfd_480;
	uint32_t pfd_480_set;
	uint32_t pfd_480_clr;
	uint32_t pfd_480_tog;
	uint32_t pfd_528;
	uint32_t pfd_528_set;
	uint32_t pfd_528_clr;
	uint32_t pfd_528_tog;
	uint32_t reserved_110[16];
	uint32_t misc0;
	uint32_t misc0_set;
	uint32_t misc0_clr;
	uint32_t misc0_tog;
	uint32_t misc1;
	uint32_t misc1_set;
	uint32_t misc1_clr;
	uint32_t misc1_tog;
	uint32_t misc2;
	uint32_t misc2_set;
	uint32_t misc2_clr;
	uint32_t misc2_tog;
} imx6ul_ccm_analog;

#endif /* IMX6UL_CCMREG_H */
