/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMimxrt
 *
 * @brief Pin function definitions for i.MXRT1050
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_IMXRT_IMXRT1050_PINFUNC_H
#define LIBBSP_ARM_IMXRT_IMXRT1050_PINFUNC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* GPIO_AD_B0_00 */
#define IMXRT_PAD_GPIO_AD_B0_00__PWM2_A3                   0x00bc 0x02ac 0x0474 0 2
#define IMXRT_PAD_GPIO_AD_B0_00__XBARA1_OUT14              0x00bc 0x02ac 0x0644 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_14 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_00__XBARA1_IN14               0x00bc 0x02ac 0x0644 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_14 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_00__XTALOSC24M_REF_CLK_32K    0x00bc 0x02ac 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_00__USB2_usb_otg2_id          0x00bc 0x02ac 0x03f8 3 0
#define IMXRT_PAD_GPIO_AD_B0_00__LPI2C1_SCLS               0x00bc 0x02ac 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_00__GPIO1_gpio_io00           0x00bc 0x02ac 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_00__USDHC1_usdhc_reset_b      0x00bc 0x02ac 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_00__LPSPI3_SCK                0x00bc 0x02ac 0x0510 7 0
#define IMXRT_PAD_GPIO_AD_B0_00__CMP1_IN4                  0x00bc 0x02ac 0x0000 5 0

/* GPIO_AD_B0_01 */
#define IMXRT_PAD_GPIO_AD_B0_01__PWM2_B3                   0x00c0 0x02b0 0x0484 0 2
#define IMXRT_PAD_GPIO_AD_B0_01__XBARA1_OUT15              0x00c0 0x02b0 0x0648 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_15 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_01__XBARA1_IN15               0x00c0 0x02b0 0x0648 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_15 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_01__XTALOSC24M_REF_CLK_24M    0x00c0 0x02b0 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_01__USB1_usb_otg1_id          0x00c0 0x02b0 0x03f4 3 0
#define IMXRT_PAD_GPIO_AD_B0_01__LPI2C1_SDAS               0x00c0 0x02b0 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_01__GPIO1_gpio_io01           0x00c0 0x02b0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_01__EWM_EWM_OUT_B             0x00c0 0x02b0 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_01__LPSPI3_SDO                0x00c0 0x02b0 0x0518 7 0
#define IMXRT_PAD_GPIO_AD_B0_01__CMP2_IN4                  0x00c0 0x02b0 0x0000 5 0

/* GPIO_AD_B0_02 */
#define IMXRT_PAD_GPIO_AD_B0_02__CAN2_TX                   0x00c4 0x02b4 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_02__XBARA1_OUT16              0x00c4 0x02b4 0x064c 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_16 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_02__XBARA1_IN16               0x00c4 0x02b4 0x064c 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_16 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_02__LPUART6_TX                0x00c4 0x02b4 0x0554 2 1
#define IMXRT_PAD_GPIO_AD_B0_02__USB1_usb_otg1_pwr         0x00c4 0x02b4 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_02__PWM1_X0                   0x00c4 0x02b4 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_02__GPIO1_gpio_io02           0x00c4 0x02b4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_02__LPI2C1_HREQ               0x00c4 0x02b4 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_02__LPSPI3_SDI                0x00c4 0x02b4 0x0514 7 0
#define IMXRT_PAD_GPIO_AD_B0_02__CMP3_IN4                  0x00c4 0x02b4 0x0000 5 0

/* GPIO_AD_B0_03 */
#define IMXRT_PAD_GPIO_AD_B0_03__CAN2_RX                   0x00c8 0x02b8 0x0450 0 1
#define IMXRT_PAD_GPIO_AD_B0_03__XBARA1_OUT17              0x00c8 0x02b8 0x062c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_03__XBARA1_IN17               0x00c8 0x02b8 0x062c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_03__LPUART6_RX                0x00c8 0x02b8 0x0550 2 1
#define IMXRT_PAD_GPIO_AD_B0_03__USB1_usb_otg1_oc          0x00c8 0x02b8 0x05d0 3 0
#define IMXRT_PAD_GPIO_AD_B0_03__PWM1_X1                   0x00c8 0x02b8 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_03__GPIO1_gpio_io03           0x00c8 0x02b8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_03__XTALOSC24M_REF_CLK_24M    0x00c8 0x02b8 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_03__LPSPI3_PCS0               0x00c8 0x02b8 0x050c 7 0
#define IMXRT_PAD_GPIO_AD_B0_03__CMP4_IN4                  0x00c8 0x02b8 0x0000 5 0

/* GPIO_AD_B0_04 */
#define IMXRT_PAD_GPIO_AD_B0_04__SRC_BOOT_MODE0            0x00cc 0x02bc 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_04__MQS_RIGHT                 0x00cc 0x02bc 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_04__ENET_enet_tx_data3        0x00cc 0x02bc 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_04__SAI2_sai_tx_sync          0x00cc 0x02bc 0x05c4 3 1
#define IMXRT_PAD_GPIO_AD_B0_04__CSI_csi_data09            0x00cc 0x02bc 0x041c 4 1
#define IMXRT_PAD_GPIO_AD_B0_04__GPIO1_gpio_io04           0x00cc 0x02bc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_04__PIT_TRIGGER0              0x00cc 0x02bc 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_04__LPSPI3_PCS1               0x00cc 0x02bc 0x0000 7 0

/* GPIO_AD_B0_05 */
#define IMXRT_PAD_GPIO_AD_B0_05__SRC_BOOT_MODE1            0x00d0 0x02c0 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_05__MQS_LEFT                  0x00d0 0x02c0 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_05__ENET_enet_tx_data2        0x00d0 0x02c0 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_05__SAI2_sai_tx_bclk          0x00d0 0x02c0 0x05c0 3 1
#define IMXRT_PAD_GPIO_AD_B0_05__CSI_csi_data08            0x00d0 0x02c0 0x0418 4 1
#define IMXRT_PAD_GPIO_AD_B0_05__GPIO1_gpio_io05           0x00d0 0x02c0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_05__XBARA1_OUT17              0x00d0 0x02c0 0x062c 6 2 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_05__XBARA1_IN17               0x00d0 0x02c0 0x062c 6 2 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_05__LPSPI3_PCS2               0x00d0 0x02c0 0x0000 7 0

/* GPIO_AD_B0_06 */
#define IMXRT_PAD_GPIO_AD_B0_06__JTAG_TMS                  0x00d4 0x02c4 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_06__GPT2_gpt_compare1         0x00d4 0x02c4 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_06__ENET_enet_rx_clk          0x00d4 0x02c4 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_06__SAI2_sai_rx_bclk          0x00d4 0x02c4 0x05b4 3 1
#define IMXRT_PAD_GPIO_AD_B0_06__CSI_csi_data07            0x00d4 0x02c4 0x0414 4 1
#define IMXRT_PAD_GPIO_AD_B0_06__GPIO1_gpio_io06           0x00d4 0x02c4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_06__XBARA1_OUT18              0x00d4 0x02c4 0x0630 6 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_18 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_06__XBARA1_IN18               0x00d4 0x02c4 0x0630 6 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_18 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_06__LPSPI3_PCS3               0x00d4 0x02c4 0x0000 7 0

/* GPIO_AD_B0_07 */
#define IMXRT_PAD_GPIO_AD_B0_07__JTAG_TCK                  0x00d8 0x02c8 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_07__GPT2_gpt_compare2         0x00d8 0x02c8 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_07__ENET_enet_tx_er           0x00d8 0x02c8 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_07__SAI2_sai_rx_sync          0x00d8 0x02c8 0x05bc 3 1
#define IMXRT_PAD_GPIO_AD_B0_07__CSI_csi_data06            0x00d8 0x02c8 0x0410 4 1
#define IMXRT_PAD_GPIO_AD_B0_07__GPIO1_gpio_io07           0x00d8 0x02c8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_07__XBARA1_OUT19              0x00d8 0x02c8 0x0654 6 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_19 to 0x1 */
#define IMXRT_PAD_GPIO_AD_B0_07__XBARA1_IN19               0x00d8 0x02c8 0x0654 6 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_19 to 0 */
#define IMXRT_PAD_GPIO_AD_B0_07__ENET_1588_EVENT3_OUT      0x00d8 0x02c8 0x0000 7 0

/* GPIO_AD_B0_08 */
#define IMXRT_PAD_GPIO_AD_B0_08__JTAG_MOD                  0x00dc 0x02cc 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_08__GPT2_gpt_compare3         0x00dc 0x02cc 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_08__ENET_enet_rx_data3        0x00dc 0x02cc 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_08__SAI2_sai_rx_data          0x00dc 0x02cc 0x05b8 3 1
#define IMXRT_PAD_GPIO_AD_B0_08__CSI_csi_data05            0x00dc 0x02cc 0x040c 4 1
#define IMXRT_PAD_GPIO_AD_B0_08__GPIO1_gpio_io08           0x00dc 0x02cc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_08__XBARA1_IN20               0x00dc 0x02cc 0x0634 6 1
#define IMXRT_PAD_GPIO_AD_B0_08__ENET_1588_EVENT3_IN       0x00dc 0x02cc 0x0000 7 0

/* GPIO_AD_B0_09 */
#define IMXRT_PAD_GPIO_AD_B0_09__JTAG_TDI                  0x00e0 0x02d0 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_09__PWM2_A3                   0x00e0 0x02d0 0x0474 1 3
#define IMXRT_PAD_GPIO_AD_B0_09__ENET_enet_rx_data2        0x00e0 0x02d0 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_09__SAI2_sai_tx_data          0x00e0 0x02d0 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_09__CSI_csi_data04            0x00e0 0x02d0 0x0408 4 1
#define IMXRT_PAD_GPIO_AD_B0_09__GPIO1_gpio_io09           0x00e0 0x02d0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_09__XBARA1_IN21               0x00e0 0x02d0 0x0658 6 1
#define IMXRT_PAD_GPIO_AD_B0_09__GPT2_gpt_clk              0x00e0 0x02d0 0x0000 7 0

/* GPIO_AD_B0_10 */
#define IMXRT_PAD_GPIO_AD_B0_10__JTAG_TDO                  0x00e4 0x02d4 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_10__PWM1_A3                   0x00e4 0x02d4 0x0454 1 3
#define IMXRT_PAD_GPIO_AD_B0_10__ENET_enet_crs             0x00e4 0x02d4 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_10__SAI2_sai_mclk             0x00e4 0x02d4 0x05b0 3 1
#define IMXRT_PAD_GPIO_AD_B0_10__CSI_csi_data03            0x00e4 0x02d4 0x0404 4 1
#define IMXRT_PAD_GPIO_AD_B0_10__GPIO1_gpio_io10           0x00e4 0x02d4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_10__XBARA1_IN22               0x00e4 0x02d4 0x0638 6 1
#define IMXRT_PAD_GPIO_AD_B0_10__ENET_1588_EVENT0_OUT      0x00e4 0x02d4 0x0000 7 0

/* GPIO_AD_B0_11 */
#define IMXRT_PAD_GPIO_AD_B0_11__JTAG_TRSTB                0x00e8 0x02d8 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_11__PWM1_B3                   0x00e8 0x02d8 0x0464 1 3
#define IMXRT_PAD_GPIO_AD_B0_11__ENET_enet_col             0x00e8 0x02d8 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_11__WDOG1_wdog_b              0x00e8 0x02d8 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_11__CSI_csi_data02            0x00e8 0x02d8 0x0400 4 1
#define IMXRT_PAD_GPIO_AD_B0_11__GPIO1_gpio_io11           0x00e8 0x02d8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_11__XBARA1_IN23               0x00e8 0x02d8 0x063c 6 1
#define IMXRT_PAD_GPIO_AD_B0_11__ENET_1588_EVENT0_IN       0x00e8 0x02d8 0x0444 7 1

/* GPIO_AD_B0_12 */
#define IMXRT_PAD_GPIO_AD_B0_12__LPI2C4_SCL                0x00ec 0x02dc 0x04e4 0 1
#define IMXRT_PAD_GPIO_AD_B0_12__CCM_PMIC_RDY              0x00ec 0x02dc 0x03fc 1 1
#define IMXRT_PAD_GPIO_AD_B0_12__LPUART1_TX                0x00ec 0x02dc 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_12__WDOG2_wdog_b              0x00ec 0x02dc 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_12__PWM1_X2                   0x00ec 0x02dc 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_12__GPIO1_gpio_io12           0x00ec 0x02dc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_12__ENET_1588_EVENT1_OUT      0x00ec 0x02dc 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_12__ARM_arm_nmi               0x00ec 0x02dc 0x0568 7 0
#define IMXRT_PAD_GPIO_AD_B0_12__ADC1_IN1                  0x00ec 0x02dc 0x0000 5 0

/* GPIO_AD_B0_13 */
#define IMXRT_PAD_GPIO_AD_B0_13__LPI2C4_SDA                0x00f0 0x02e0 0x04e8 0 1
#define IMXRT_PAD_GPIO_AD_B0_13__GPT1_gpt_clk              0x00f0 0x02e0 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B0_13__LPUART1_RX                0x00f0 0x02e0 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_13__EWM_EWM_OUT_B             0x00f0 0x02e0 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_13__PWM1_X3                   0x00f0 0x02e0 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B0_13__GPIO1_gpio_io13           0x00f0 0x02e0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_13__ENET_1588_EVENT1_IN       0x00f0 0x02e0 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_13__XTALOSC24M_REF_CLK_24M    0x00f0 0x02e0 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B0_13__ADC1_IN2                  0x00f0 0x02e0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_13__CMP1_IN2                  0x00f0 0x02e0 0x0000 5 0

/* GPIO_AD_B0_14 */
#define IMXRT_PAD_GPIO_AD_B0_14__USB2_usb_otg2_oc          0x00f4 0x02e4 0x05cc 0 0
#define IMXRT_PAD_GPIO_AD_B0_14__XBARA1_IN24               0x00f4 0x02e4 0x0640 1 1
#define IMXRT_PAD_GPIO_AD_B0_14__LPUART1_CTS_B             0x00f4 0x02e4 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_14__ENET_1588_EVENT0_OUT      0x00f4 0x02e4 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B0_14__CSI_csi_vsync             0x00f4 0x02e4 0x0428 4 0
#define IMXRT_PAD_GPIO_AD_B0_14__GPIO1_gpio_io14           0x00f4 0x02e4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_14__CAN2_TX                   0x00f4 0x02e4 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B0_14__ADC1_IN3                  0x00f4 0x02e4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_14__CMP2_IN2                  0x00f4 0x02e4 0x0000 5 0

/* GPIO_AD_B0_15 */
#define IMXRT_PAD_GPIO_AD_B0_15__USB2_usb_otg2_pwr         0x00f8 0x02e8 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B0_15__XBARA1_IN25               0x00f8 0x02e8 0x0650 1 0
#define IMXRT_PAD_GPIO_AD_B0_15__LPUART1_RTS_B             0x00f8 0x02e8 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B0_15__ENET_1588_EVENT0_IN       0x00f8 0x02e8 0x0444 3 0
#define IMXRT_PAD_GPIO_AD_B0_15__CSI_csi_hsync             0x00f8 0x02e8 0x0420 4 0
#define IMXRT_PAD_GPIO_AD_B0_15__GPIO1_gpio_io15           0x00f8 0x02e8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_15__CAN2_RX                   0x00f8 0x02e8 0x0450 6 2
#define IMXRT_PAD_GPIO_AD_B0_15__WDOG1_wdog_rst_b_deb      0x00f8 0x02e8 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B0_15__ADC1_IN4                  0x00f8 0x02e8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B0_15__CMP3_IN2                  0x00f8 0x02e8 0x0000 5 0

/* GPIO_AD_B1_00 */
#define IMXRT_PAD_GPIO_AD_B1_00__USB2_usb_otg2_id          0x00fc 0x02ec 0x03f8 0 1
#define IMXRT_PAD_GPIO_AD_B1_00__TMR3_TIMER0               0x00fc 0x02ec 0x057c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_AD_B1_00__LPUART2_CTS_B             0x00fc 0x02ec 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B1_00__LPI2C1_SCL                0x00fc 0x02ec 0x04cc 3 1
#define IMXRT_PAD_GPIO_AD_B1_00__WDOG1_wdog_b              0x00fc 0x02ec 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B1_00__GPIO1_gpio_io16           0x00fc 0x02ec 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_00__USDHC1_usdhc_wp           0x00fc 0x02ec 0x05d8 6 2
#define IMXRT_PAD_GPIO_AD_B1_00__KPP_kpp_row7              0x00fc 0x02ec 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_00__ADC1_IN5                  0x00fc 0x02ec 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_00__ADC2_IN5                  0x00fc 0x02ec 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_00__CMP4_IN2                  0x00fc 0x02ec 0x0000 5 0

/* GPIO_AD_B1_01 */
#define IMXRT_PAD_GPIO_AD_B1_01__USB1_usb_otg1_pwr         0x0100 0x02f0 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B1_01__TMR3_TIMER1               0x0100 0x02f0 0x0580 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_AD_B1_01__LPUART2_RTS_B             0x0100 0x02f0 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B1_01__LPI2C1_SDA                0x0100 0x02f0 0x04d0 3 1
#define IMXRT_PAD_GPIO_AD_B1_01__CCM_PMIC_RDY              0x0100 0x02f0 0x03fc 4 2
#define IMXRT_PAD_GPIO_AD_B1_01__GPIO1_gpio_io17           0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__USDHC1_usdhc_vselect      0x0100 0x02f0 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B1_01__KPP_kpp_col7              0x0100 0x02f0 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_01__ADC1_IN6                  0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__ADC2_IN6                  0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__CMP1_IN0                  0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__CMP2_IN0                  0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__CMP3_IN0                  0x0100 0x02f0 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_01__CMP4_IN0                  0x0100 0x02f0 0x0000 5 0

/* GPIO_AD_B1_02 */
#define IMXRT_PAD_GPIO_AD_B1_02__USB1_usb_otg1_id          0x0104 0x02f4 0x03f4 0 1
#define IMXRT_PAD_GPIO_AD_B1_02__TMR3_TIMER2               0x0104 0x02f4 0x0584 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_AD_B1_02__LPUART2_TX                0x0104 0x02f4 0x0530 2 1
#define IMXRT_PAD_GPIO_AD_B1_02__SPDIF_spdif_out           0x0104 0x02f4 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_02__ENET_1588_EVENT2_OUT      0x0104 0x02f4 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B1_02__GPIO1_gpio_io18           0x0104 0x02f4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_02__USDHC1_usdhc_cd_b         0x0104 0x02f4 0x05d4 6 1
#define IMXRT_PAD_GPIO_AD_B1_02__KPP_kpp_row6              0x0104 0x02f4 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_02__ADC1_IN7                  0x0104 0x02f4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_02__ADC2_IN7                  0x0104 0x02f4 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_02__CMP1_IN3                  0x0104 0x02f4 0x0000 5 0

/* GPIO_AD_B1_03 */
#define IMXRT_PAD_GPIO_AD_B1_03__USB1_usb_otg1_oc          0x0108 0x02f8 0x05d0 0 1
#define IMXRT_PAD_GPIO_AD_B1_03__TMR3_TIMER3               0x0108 0x02f8 0x0588 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_AD_B1_03__LPUART2_RX                0x0108 0x02f8 0x052c 2 1
#define IMXRT_PAD_GPIO_AD_B1_03__SPDIF_spdif_in            0x0108 0x02f8 0x05c8 3 0
#define IMXRT_PAD_GPIO_AD_B1_03__ENET_1588_EVENT2_IN       0x0108 0x02f8 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B1_03__GPIO1_gpio_io19           0x0108 0x02f8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_03__USDHC2_usdhc_cd_b         0x0108 0x02f8 0x05e0 6 0
#define IMXRT_PAD_GPIO_AD_B1_03__KPP_kpp_col6              0x0108 0x02f8 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_03__ADC1_IN8                  0x0108 0x02f8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_03__ADC2_IN8                  0x0108 0x02f8 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_03__CMP2_IN3                  0x0108 0x02f8 0x0000 5 0

/* GPIO_AD_B1_04 */
#define IMXRT_PAD_GPIO_AD_B1_04__FLEXSPI_FLEXSPI_B_DATA3   0x010c 0x02fc 0x04c4 0 1
#define IMXRT_PAD_GPIO_AD_B1_04__ENET_enet_mdc             0x010c 0x02fc 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_04__LPUART3_CTS_B             0x010c 0x02fc 0x0534 2 1
#define IMXRT_PAD_GPIO_AD_B1_04__SPDIF_spdif_sr_clk        0x010c 0x02fc 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_04__CSI_csi_pixclk            0x010c 0x02fc 0x0424 4 0
#define IMXRT_PAD_GPIO_AD_B1_04__GPIO1_gpio_io20           0x010c 0x02fc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_04__USDHC2_usdhc_data0        0x010c 0x02fc 0x05e8 6 1
#define IMXRT_PAD_GPIO_AD_B1_04__KPP_kpp_row5              0x010c 0x02fc 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_04__ADC1_IN9                  0x010c 0x02fc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_04__ADC2_IN9                  0x010c 0x02fc 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_04__CMP3_IN3                  0x010c 0x02fc 0x0000 5 0

/* GPIO_AD_B1_05 */
#define IMXRT_PAD_GPIO_AD_B1_05__FLEXSPI_FLEXSPI_B_DATA2   0x0110 0x0300 0x04c0 0 1
#define IMXRT_PAD_GPIO_AD_B1_05__ENET_enet_mdio            0x0110 0x0300 0x0430 1 0
#define IMXRT_PAD_GPIO_AD_B1_05__LPUART3_RTS_B             0x0110 0x0300 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B1_05__SPDIF_spdif_out           0x0110 0x0300 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_05__CSI_csi_mclk              0x0110 0x0300 0x0000 4 0
#define IMXRT_PAD_GPIO_AD_B1_05__GPIO1_gpio_io21           0x0110 0x0300 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_05__USDHC2_usdhc_data1        0x0110 0x0300 0x05ec 6 1
#define IMXRT_PAD_GPIO_AD_B1_05__KPP_kpp_col5              0x0110 0x0300 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_05__ADC1_IN10                 0x0110 0x0300 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_05__ADC2_IN10                 0x0110 0x0300 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_05__CMP4_IN3                  0x0110 0x0300 0x0000 5 0

/* GPIO_AD_B1_06 */
#define IMXRT_PAD_GPIO_AD_B1_06__FLEXSPI_FLEXSPI_B_DATA1   0x0114 0x0304 0x04bc 0 1
#define IMXRT_PAD_GPIO_AD_B1_06__LPI2C3_SDA                0x0114 0x0304 0x04e0 1 2
#define IMXRT_PAD_GPIO_AD_B1_06__LPUART3_TX                0x0114 0x0304 0x053c 2 0
#define IMXRT_PAD_GPIO_AD_B1_06__SPDIF_spdif_lock          0x0114 0x0304 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_06__CSI_csi_vsync             0x0114 0x0304 0x0428 4 1
#define IMXRT_PAD_GPIO_AD_B1_06__GPIO1_gpio_io22           0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__USDHC2_usdhc_data2        0x0114 0x0304 0x05f0 6 1
#define IMXRT_PAD_GPIO_AD_B1_06__KPP_kpp_row4              0x0114 0x0304 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_06__ADC1_IN11                 0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__ADC2_IN11                 0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__CMP1_IN1                  0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__CMP2_IN1                  0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__CMP3_IN1                  0x0114 0x0304 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_06__CMP4_IN1                  0x0114 0x0304 0x0000 5 0

/* GPIO_AD_B1_07 */
#define IMXRT_PAD_GPIO_AD_B1_07__FLEXSPI_FLEXSPI_B_DATA0   0x0118 0x0308 0x04b8 0 1
#define IMXRT_PAD_GPIO_AD_B1_07__LPI2C3_SCL                0x0118 0x0308 0x04dc 1 2
#define IMXRT_PAD_GPIO_AD_B1_07__LPUART3_RX                0x0118 0x0308 0x0538 2 0
#define IMXRT_PAD_GPIO_AD_B1_07__SPDIF_spdif_ext_clk       0x0118 0x0308 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_07__CSI_csi_hsync             0x0118 0x0308 0x0420 4 1
#define IMXRT_PAD_GPIO_AD_B1_07__GPIO1_gpio_io23           0x0118 0x0308 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_07__USDHC2_usdhc_data3        0x0118 0x0308 0x05f4 6 1
#define IMXRT_PAD_GPIO_AD_B1_07__KPP_kpp_col4              0x0118 0x0308 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_07__ADC1_IN12                 0x0118 0x0308 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_07__ADC2_IN12                 0x0118 0x0308 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_07__CMP1_IN5                  0x0118 0x0308 0x0000 5 0

/* GPIO_AD_B1_08 */
#define IMXRT_PAD_GPIO_AD_B1_08__FLEXSPI_FLEXSPI_A_SS1_B   0x011c 0x030c 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B1_08__PWM4_A0                   0x011c 0x030c 0x0494 1 1
#define IMXRT_PAD_GPIO_AD_B1_08__CAN1_TX                   0x011c 0x030c 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B1_08__CCM_PMIC_RDY              0x011c 0x030c 0x03fc 3 3
#define IMXRT_PAD_GPIO_AD_B1_08__CSI_csi_data09            0x011c 0x030c 0x041c 4 0
#define IMXRT_PAD_GPIO_AD_B1_08__GPIO1_gpio_io24           0x011c 0x030c 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_08__USDHC2_usdhc_cmd          0x011c 0x030c 0x05e4 6 1
#define IMXRT_PAD_GPIO_AD_B1_08__KPP_kpp_row3              0x011c 0x030c 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_08__ADC1_IN13                 0x011c 0x030c 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_08__ADC2_IN13                 0x011c 0x030c 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_08__CMP2_IN5                  0x011c 0x030c 0x0000 5 0

/* GPIO_AD_B1_09 */
#define IMXRT_PAD_GPIO_AD_B1_09__FLEXSPI_FLEXSPI_A_DQS     0x0120 0x0310 0x04a4 0 1
#define IMXRT_PAD_GPIO_AD_B1_09__PWM4_A1                   0x0120 0x0310 0x0498 1 1
#define IMXRT_PAD_GPIO_AD_B1_09__CAN1_RX                   0x0120 0x0310 0x044c 2 2
#define IMXRT_PAD_GPIO_AD_B1_09__SAI1_sai_mclk             0x0120 0x0310 0x058c 3 1
#define IMXRT_PAD_GPIO_AD_B1_09__CSI_csi_data08            0x0120 0x0310 0x0418 4 0
#define IMXRT_PAD_GPIO_AD_B1_09__GPIO1_gpio_io25           0x0120 0x0310 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_09__USDHC2_usdhc_clk          0x0120 0x0310 0x05dc 6 1
#define IMXRT_PAD_GPIO_AD_B1_09__KPP_kpp_col3              0x0120 0x0310 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_09__ADC1_IN14                 0x0120 0x0310 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_09__ADC2_IN14                 0x0120 0x0310 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_09__CMP3_IN5                  0x0120 0x0310 0x0000 5 0

/* GPIO_AD_B1_10 */
#define IMXRT_PAD_GPIO_AD_B1_10__FLEXSPI_FLEXSPI_A_DATA3   0x0124 0x0314 0x04b4 0 1
#define IMXRT_PAD_GPIO_AD_B1_10__WDOG1_wdog_b              0x0124 0x0314 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_10__LPUART8_TX                0x0124 0x0314 0x0564 2 1
#define IMXRT_PAD_GPIO_AD_B1_10__SAI1_sai_rx_sync          0x0124 0x0314 0x05a4 3 1
#define IMXRT_PAD_GPIO_AD_B1_10__CSI_csi_data07            0x0124 0x0314 0x0414 4 0
#define IMXRT_PAD_GPIO_AD_B1_10__GPIO1_gpio_io26           0x0124 0x0314 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_10__USDHC2_usdhc_wp           0x0124 0x0314 0x0608 6 1
#define IMXRT_PAD_GPIO_AD_B1_10__KPP_kpp_row2              0x0124 0x0314 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_10__ADC1_IN15                 0x0124 0x0314 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_10__ADC2_IN15                 0x0124 0x0314 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_10__CMP4_IN5                  0x0124 0x0314 0x0000 5 0

/* GPIO_AD_B1_11 */
#define IMXRT_PAD_GPIO_AD_B1_11__FLEXSPI_FLEXSPI_A_DATA2   0x0128 0x0318 0x04b0 0 1
#define IMXRT_PAD_GPIO_AD_B1_11__EWM_EWM_OUT_B             0x0128 0x0318 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_11__LPUART8_RX                0x0128 0x0318 0x0560 2 1
#define IMXRT_PAD_GPIO_AD_B1_11__SAI1_sai_rx_bclk          0x0128 0x0318 0x0590 3 1
#define IMXRT_PAD_GPIO_AD_B1_11__CSI_csi_data06            0x0128 0x0318 0x0410 4 0
#define IMXRT_PAD_GPIO_AD_B1_11__GPIO1_gpio_io27           0x0128 0x0318 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_11__USDHC2_usdhc_reset_b      0x0128 0x0318 0x0000 6 0
#define IMXRT_PAD_GPIO_AD_B1_11__KPP_kpp_col2              0x0128 0x0318 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_11__ADC1_IN0                  0x0128 0x0318 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_11__ADC2_IN0                  0x0128 0x0318 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_11__CMP1_IN6                  0x0128 0x0318 0x0000 5 0

/* GPIO_AD_B1_12 */
#define IMXRT_PAD_GPIO_AD_B1_12__FLEXSPI_FLEXSPI_A_DATA1   0x012c 0x031c 0x04ac 0 1
#define IMXRT_PAD_GPIO_AD_B1_12__CMP1_OUT                  0x012c 0x031c 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_12__LPSPI3_PCS0               0x012c 0x031c 0x050c 2 1
#define IMXRT_PAD_GPIO_AD_B1_12__SAI1_sai_rx_data0         0x012c 0x031c 0x0594 3 1
#define IMXRT_PAD_GPIO_AD_B1_12__CSI_csi_data05            0x012c 0x031c 0x040c 4 0
#define IMXRT_PAD_GPIO_AD_B1_12__GPIO1_gpio_io28           0x012c 0x031c 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_12__USDHC2_usdhc_data4        0x012c 0x031c 0x05f8 6 1
#define IMXRT_PAD_GPIO_AD_B1_12__KPP_kpp_row1              0x012c 0x031c 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_12__ADC2_IN1                  0x012c 0x031c 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_12__CMP2_IN6                  0x012c 0x031c 0x0000 5 0

/* GPIO_AD_B1_13 */
#define IMXRT_PAD_GPIO_AD_B1_13__FLEXSPI_FLEXSPI_A_DATA0   0x0130 0x0320 0x04a8 0 1
#define IMXRT_PAD_GPIO_AD_B1_13__CMP2_OUT                  0x0130 0x0320 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_13__LPSPI3_SDI                0x0130 0x0320 0x0514 2 1
#define IMXRT_PAD_GPIO_AD_B1_13__SAI1_sai_tx_data0         0x0130 0x0320 0x0000 3 0
#define IMXRT_PAD_GPIO_AD_B1_13__CSI_csi_data04            0x0130 0x0320 0x0408 4 0
#define IMXRT_PAD_GPIO_AD_B1_13__GPIO1_gpio_io29           0x0130 0x0320 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_13__USDHC2_usdhc_data5        0x0130 0x0320 0x05fc 6 1
#define IMXRT_PAD_GPIO_AD_B1_13__KPP_kpp_col1              0x0130 0x0320 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_13__ADC2_IN2                  0x0130 0x0320 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_13__CMP3_IN6                  0x0130 0x0320 0x0000 5 0

/* GPIO_AD_B1_14 */
#define IMXRT_PAD_GPIO_AD_B1_14__FLEXSPI_FLEXSPI_A_SCLK    0x0134 0x0324 0x04c8 0 1
#define IMXRT_PAD_GPIO_AD_B1_14__CMP3_OUT                  0x0134 0x0324 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_14__LPSPI3_SDO                0x0134 0x0324 0x0518 2 1
#define IMXRT_PAD_GPIO_AD_B1_14__SAI1_sai_tx_bclk          0x0134 0x0324 0x05a8 3 1
#define IMXRT_PAD_GPIO_AD_B1_14__CSI_csi_data03            0x0134 0x0324 0x0404 4 0
#define IMXRT_PAD_GPIO_AD_B1_14__GPIO1_gpio_io30           0x0134 0x0324 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_14__USDHC2_usdhc_data6        0x0134 0x0324 0x0600 6 1
#define IMXRT_PAD_GPIO_AD_B1_14__KPP_kpp_row0              0x0134 0x0324 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_14__ADC2_IN3                  0x0134 0x0324 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_14__CMP4_IN6                  0x0134 0x0324 0x0000 5 0

/* GPIO_AD_B1_15 */
#define IMXRT_PAD_GPIO_AD_B1_15__FLEXSPI_FLEXSPI_A_SS0_B   0x0138 0x0328 0x0000 0 0
#define IMXRT_PAD_GPIO_AD_B1_15__CMP4_OUT                  0x0138 0x0328 0x0000 1 0
#define IMXRT_PAD_GPIO_AD_B1_15__LPSPI3_SCK                0x0138 0x0328 0x0000 2 0
#define IMXRT_PAD_GPIO_AD_B1_15__SAI1_sai_tx_sync          0x0138 0x0328 0x05ac 3 1
#define IMXRT_PAD_GPIO_AD_B1_15__CSI_csi_data02            0x0138 0x0328 0x0400 4 0
#define IMXRT_PAD_GPIO_AD_B1_15__GPIO1_gpio_io31           0x0138 0x0328 0x0000 5 0
#define IMXRT_PAD_GPIO_AD_B1_15__USDHC2_usdhc_data7        0x0138 0x0328 0x0604 6 1
#define IMXRT_PAD_GPIO_AD_B1_15__KPP_kpp_col0              0x0138 0x0328 0x0000 7 0
#define IMXRT_PAD_GPIO_AD_B1_15__ADC2_IN4                  0x0138 0x0328 0x0000 5 0

/* GPIO_B0_00 */
#define IMXRT_PAD_GPIO_B0_00__LCDIF_lcdif_clk              0x013c 0x032c 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_00__TMR1_TIMER0                  0x013c 0x032c 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER1_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_00__MQS_RIGHT                    0x013c 0x032c 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_00__LPSPI4_PCS0                  0x013c 0x032c 0x051c 3 0
#define IMXRT_PAD_GPIO_B0_00__FLEXIO2_IO00                 0x013c 0x032c 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_00__GPIO2_gpio_io00              0x013c 0x032c 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_00__SEMC_CSX1                    0x013c 0x032c 0x0000 6 0

/* GPIO_B0_01 */
#define IMXRT_PAD_GPIO_B0_01__LCDIF_lcdif_enable           0x0140 0x0330 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_01__TMR1_TIMER1                  0x0140 0x0330 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER1_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_01__MQS_LEFT                     0x0140 0x0330 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_01__LPSPI4_SDI                   0x0140 0x0330 0x0524 3 0
#define IMXRT_PAD_GPIO_B0_01__FLEXIO2_IO01                 0x0140 0x0330 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_01__GPIO2_gpio_io01              0x0140 0x0330 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_01__SEMC_CSX2                    0x0140 0x0330 0x0000 6 0

/* GPIO_B0_02 */
#define IMXRT_PAD_GPIO_B0_02__LCDIF_lcdif_hsync            0x0144 0x0334 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_02__TMR1_TIMER2                  0x0144 0x0334 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER1_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_02__CAN1_TX                      0x0144 0x0334 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_02__LPSPI4_SDO                   0x0144 0x0334 0x0528 3 0
#define IMXRT_PAD_GPIO_B0_02__FLEXIO2_IO02                 0x0144 0x0334 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_02__GPIO2_gpio_io02              0x0144 0x0334 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_02__SEMC_CSX3                    0x0144 0x0334 0x0000 6 0

/* GPIO_B0_03 */
#define IMXRT_PAD_GPIO_B0_03__LCDIF_lcdif_vsync            0x0148 0x0338 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_03__TMR2_TIMER0                  0x0148 0x0338 0x056c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_03__CAN1_RX                      0x0148 0x0338 0x044c 2 3
#define IMXRT_PAD_GPIO_B0_03__LPSPI4_SCK                   0x0148 0x0338 0x0520 3 0
#define IMXRT_PAD_GPIO_B0_03__FLEXIO2_IO03                 0x0148 0x0338 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_03__GPIO2_gpio_io03              0x0148 0x0338 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_03__WDOG2_wdog_rst_b_deb         0x0148 0x0338 0x0000 6 0

/* GPIO_B0_04 */
#define IMXRT_PAD_GPIO_B0_04__LCDIF_lcdif_data00           0x014c 0x033c 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_04__TMR2_TIMER1                  0x014c 0x033c 0x0570 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_04__LPI2C2_SCL                   0x014c 0x033c 0x04d4 2 1
#define IMXRT_PAD_GPIO_B0_04__ARM_TRACE0                   0x014c 0x033c 0x0000 3 0
#define IMXRT_PAD_GPIO_B0_04__FLEXIO2_IO04                 0x014c 0x033c 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_04__GPIO2_gpio_io04              0x014c 0x033c 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_04__SRC_BOOT_CFG0                0x014c 0x033c 0x0000 6 0

/* GPIO_B0_05 */
#define IMXRT_PAD_GPIO_B0_05__LCDIF_lcdif_data01           0x0150 0x0340 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_05__TMR2_TIMER2                  0x0150 0x0340 0x0574 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_05__LPI2C2_SDA                   0x0150 0x0340 0x04d8 2 1
#define IMXRT_PAD_GPIO_B0_05__ARM_TRACE1                   0x0150 0x0340 0x0000 3 0
#define IMXRT_PAD_GPIO_B0_05__FLEXIO2_IO05                 0x0150 0x0340 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_05__GPIO2_gpio_io05              0x0150 0x0340 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_05__SRC_BOOT_CFG1                0x0150 0x0340 0x0000 6 0

/* GPIO_B0_06 */
#define IMXRT_PAD_GPIO_B0_06__LCDIF_lcdif_data02           0x0154 0x0344 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_06__TMR3_TIMER0                  0x0154 0x0344 0x057c 1 2 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_06__PWM2_A0                      0x0154 0x0344 0x0478 2 1
#define IMXRT_PAD_GPIO_B0_06__ARM_TRACE2                   0x0154 0x0344 0x0000 3 0
#define IMXRT_PAD_GPIO_B0_06__FLEXIO2_IO06                 0x0154 0x0344 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_06__GPIO2_gpio_io06              0x0154 0x0344 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_06__SRC_BOOT_CFG2                0x0154 0x0344 0x0000 6 0

/* GPIO_B0_07 */
#define IMXRT_PAD_GPIO_B0_07__LCDIF_lcdif_data03           0x0158 0x0348 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_07__TMR3_TIMER1                  0x0158 0x0348 0x0580 1 2 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_07__PWM2_B0                      0x0158 0x0348 0x0488 2 1
#define IMXRT_PAD_GPIO_B0_07__ARM_TRACE3                   0x0158 0x0348 0x0000 3 0
#define IMXRT_PAD_GPIO_B0_07__FLEXIO2_IO07                 0x0158 0x0348 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_07__GPIO2_gpio_io07              0x0158 0x0348 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_07__SRC_BOOT_CFG3                0x0158 0x0348 0x0000 6 0

/* GPIO_B0_08 */
#define IMXRT_PAD_GPIO_B0_08__LCDIF_lcdif_data04           0x015c 0x034c 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_08__TMR3_TIMER2                  0x015c 0x034c 0x0584 1 2 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_08__PWM2_A1                      0x015c 0x034c 0x047c 2 1
#define IMXRT_PAD_GPIO_B0_08__LPUART3_TX                   0x015c 0x034c 0x053c 3 2
#define IMXRT_PAD_GPIO_B0_08__FLEXIO2_IO08                 0x015c 0x034c 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_08__GPIO2_gpio_io08              0x015c 0x034c 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_08__SRC_BOOT_CFG4                0x015c 0x034c 0x0000 6 0

/* GPIO_B0_09 */
#define IMXRT_PAD_GPIO_B0_09__LCDIF_lcdif_data05           0x0160 0x0350 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_09__TMR4_TIMER0                  0x0160 0x0350 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER4_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_09__PWM2_B1                      0x0160 0x0350 0x048c 2 1
#define IMXRT_PAD_GPIO_B0_09__LPUART3_RX                   0x0160 0x0350 0x0538 3 2
#define IMXRT_PAD_GPIO_B0_09__FLEXIO2_IO09                 0x0160 0x0350 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_09__GPIO2_gpio_io09              0x0160 0x0350 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_09__SRC_BOOT_CFG5                0x0160 0x0350 0x0000 6 0

/* GPIO_B0_10 */
#define IMXRT_PAD_GPIO_B0_10__LCDIF_lcdif_data06           0x0164 0x0354 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_10__TMR4_TIMER1                  0x0164 0x0354 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER4_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_10__PWM2_A2                      0x0164 0x0354 0x0480 2 1
#define IMXRT_PAD_GPIO_B0_10__SAI1_sai_tx_data3            0x0164 0x0354 0x0598 3 1
#define IMXRT_PAD_GPIO_B0_10__FLEXIO2_IO10                 0x0164 0x0354 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_10__GPIO2_gpio_io10              0x0164 0x0354 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_10__SRC_BOOT_CFG6                0x0164 0x0354 0x0000 6 0

/* GPIO_B0_11 */
#define IMXRT_PAD_GPIO_B0_11__LCDIF_lcdif_data07           0x0168 0x0358 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_11__TMR4_TIMER2                  0x0168 0x0358 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER4_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B0_11__PWM2_B2                      0x0168 0x0358 0x0490 2 1
#define IMXRT_PAD_GPIO_B0_11__SAI1_sai_tx_data2            0x0168 0x0358 0x059c 3 1
#define IMXRT_PAD_GPIO_B0_11__FLEXIO2_IO11                 0x0168 0x0358 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_11__GPIO2_gpio_io11              0x0168 0x0358 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_11__SRC_BOOT_CFG7                0x0168 0x0358 0x0000 6 0

/* GPIO_B0_12 */
#define IMXRT_PAD_GPIO_B0_12__LCDIF_lcdif_data08           0x016c 0x035c 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_12__XBARA1_OUT10                 0x016c 0x035c 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_10 to 0x1 */
#define IMXRT_PAD_GPIO_B0_12__XBARA1_IN10                  0x016c 0x035c 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_10 to 0 */
#define IMXRT_PAD_GPIO_B0_12__ARM_arm_trace_clk            0x016c 0x035c 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_12__SAI1_sai_tx_data1            0x016c 0x035c 0x05a0 3 1
#define IMXRT_PAD_GPIO_B0_12__FLEXIO2_IO12                 0x016c 0x035c 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_12__GPIO2_gpio_io12              0x016c 0x035c 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_12__SRC_BOOT_CFG8                0x016c 0x035c 0x0000 6 0

/* GPIO_B0_13 */
#define IMXRT_PAD_GPIO_B0_13__LCDIF_lcdif_data09           0x0170 0x0360 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_13__XBARA1_OUT11                 0x0170 0x0360 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_11 to 0x1 */
#define IMXRT_PAD_GPIO_B0_13__XBARA1_IN11                  0x0170 0x0360 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_11 to 0 */
#define IMXRT_PAD_GPIO_B0_13__ARM_arm_trace_swo            0x0170 0x0360 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_13__SAI1_sai_mclk                0x0170 0x0360 0x058c 3 2
#define IMXRT_PAD_GPIO_B0_13__FLEXIO2_IO13                 0x0170 0x0360 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_13__GPIO2_gpio_io13              0x0170 0x0360 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_13__SRC_BOOT_CFG9                0x0170 0x0360 0x0000 6 0

/* GPIO_B0_14 */
#define IMXRT_PAD_GPIO_B0_14__LCDIF_lcdif_data10           0x0174 0x0364 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_14__XBARA1_OUT12                 0x0174 0x0364 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_12 to 0x1 */
#define IMXRT_PAD_GPIO_B0_14__XBARA1_IN12                  0x0174 0x0364 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_12 to 0 */
#define IMXRT_PAD_GPIO_B0_14__ARM_arm_cm7_txev             0x0174 0x0364 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_14__SAI1_sai_rx_sync             0x0174 0x0364 0x05a4 3 2
#define IMXRT_PAD_GPIO_B0_14__FLEXIO2_IO14                 0x0174 0x0364 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_14__GPIO2_gpio_io14              0x0174 0x0364 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_14__SRC_BOOT_CFG10               0x0174 0x0364 0x0000 6 0

/* GPIO_B0_15 */
#define IMXRT_PAD_GPIO_B0_15__LCDIF_lcdif_data11           0x0178 0x0368 0x0000 0 0
#define IMXRT_PAD_GPIO_B0_15__XBARA1_OUT13                 0x0178 0x0368 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_13 to 0x1 */
#define IMXRT_PAD_GPIO_B0_15__XBARA1_IN13                  0x0178 0x0368 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_13 to 0 */
#define IMXRT_PAD_GPIO_B0_15__ARM_arm_cm7_rxev             0x0178 0x0368 0x0000 2 0
#define IMXRT_PAD_GPIO_B0_15__SAI1_sai_rx_bclk             0x0178 0x0368 0x0590 3 2
#define IMXRT_PAD_GPIO_B0_15__FLEXIO2_IO15                 0x0178 0x0368 0x0000 4 0
#define IMXRT_PAD_GPIO_B0_15__GPIO2_gpio_io15              0x0178 0x0368 0x0000 5 0
#define IMXRT_PAD_GPIO_B0_15__SRC_BOOT_CFG11               0x0178 0x0368 0x0000 6 0

/* GPIO_B1_00 */
#define IMXRT_PAD_GPIO_B1_00__LCDIF_lcdif_data12           0x017c 0x036c 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_00__XBARA1_OUT14                 0x017c 0x036c 0x0644 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_14 to 0x1 */
#define IMXRT_PAD_GPIO_B1_00__XBARA1_IN14                  0x017c 0x036c 0x0644 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_14 to 0 */
#define IMXRT_PAD_GPIO_B1_00__LPUART4_TX                   0x017c 0x036c 0x0544 2 2
#define IMXRT_PAD_GPIO_B1_00__SAI1_sai_rx_data0            0x017c 0x036c 0x0594 3 2
#define IMXRT_PAD_GPIO_B1_00__FLEXIO2_IO16                 0x017c 0x036c 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_00__GPIO2_gpio_io16              0x017c 0x036c 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_00__PWM1_A3                      0x017c 0x036c 0x0454 6 4

/* GPIO_B1_01 */
#define IMXRT_PAD_GPIO_B1_01__LCDIF_lcdif_data13           0x0180 0x0370 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_01__XBARA1_OUT15                 0x0180 0x0370 0x0648 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_15 to 0x1 */
#define IMXRT_PAD_GPIO_B1_01__XBARA1_IN15                  0x0180 0x0370 0x0648 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_15 to 0 */
#define IMXRT_PAD_GPIO_B1_01__LPUART4_RX                   0x0180 0x0370 0x0540 2 2
#define IMXRT_PAD_GPIO_B1_01__SAI1_sai_tx_data0            0x0180 0x0370 0x0000 3 0
#define IMXRT_PAD_GPIO_B1_01__FLEXIO2_IO17                 0x0180 0x0370 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_01__GPIO2_gpio_io17              0x0180 0x0370 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_01__PWM1_B3                      0x0180 0x0370 0x0464 6 4

/* GPIO_B1_02 */
#define IMXRT_PAD_GPIO_B1_02__LCDIF_lcdif_data14           0x0184 0x0374 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_02__XBARA1_OUT16                 0x0184 0x0374 0x064c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_16 to 0x1 */
#define IMXRT_PAD_GPIO_B1_02__XBARA1_IN16                  0x0184 0x0374 0x064c 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_16 to 0 */
#define IMXRT_PAD_GPIO_B1_02__LPSPI4_PCS2                  0x0184 0x0374 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_02__SAI1_sai_tx_bclk             0x0184 0x0374 0x05a8 3 2
#define IMXRT_PAD_GPIO_B1_02__FLEXIO2_IO18                 0x0184 0x0374 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_02__GPIO2_gpio_io18              0x0184 0x0374 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_02__PWM2_A3                      0x0184 0x0374 0x0474 6 4

/* GPIO_B1_03 */
#define IMXRT_PAD_GPIO_B1_03__LCDIF_lcdif_data15           0x0188 0x0378 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_03__XBARA1_OUT17                 0x0188 0x0378 0x062c 1 3 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0x1 */
#define IMXRT_PAD_GPIO_B1_03__XBARA1_IN17                  0x0188 0x0378 0x062c 1 3 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0 */
#define IMXRT_PAD_GPIO_B1_03__LPSPI4_PCS1                  0x0188 0x0378 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_03__SAI1_sai_tx_sync             0x0188 0x0378 0x05ac 3 2
#define IMXRT_PAD_GPIO_B1_03__FLEXIO2_IO19                 0x0188 0x0378 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_03__GPIO2_gpio_io19              0x0188 0x0378 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_03__PWM2_B3                      0x0188 0x0378 0x0484 6 3

/* GPIO_B1_04 */
#define IMXRT_PAD_GPIO_B1_04__LCDIF_lcdif_data16           0x018c 0x037c 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_04__LPSPI4_PCS0                  0x018c 0x037c 0x051c 1 1
#define IMXRT_PAD_GPIO_B1_04__CSI_csi_data15               0x018c 0x037c 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_04__ENET_enet_rx_data0           0x018c 0x037c 0x0434 3 1
#define IMXRT_PAD_GPIO_B1_04__FLEXIO2_IO20                 0x018c 0x037c 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_04__GPIO2_gpio_io20              0x018c 0x037c 0x0000 5 0

/* GPIO_B1_05 */
#define IMXRT_PAD_GPIO_B1_05__LCDIF_lcdif_data17           0x0190 0x0380 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_05__LPSPI4_SDI                   0x0190 0x0380 0x0524 1 1
#define IMXRT_PAD_GPIO_B1_05__CSI_csi_data14               0x0190 0x0380 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_05__ENET_enet_rx_data1           0x0190 0x0380 0x0438 3 1
#define IMXRT_PAD_GPIO_B1_05__FLEXIO2_IO21                 0x0190 0x0380 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_05__GPIO2_gpio_io21              0x0190 0x0380 0x0000 5 0

/* GPIO_B1_06 */
#define IMXRT_PAD_GPIO_B1_06__LCDIF_lcdif_data18           0x0194 0x0384 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_06__LPSPI4_SDO                   0x0194 0x0384 0x0528 1 1
#define IMXRT_PAD_GPIO_B1_06__CSI_csi_data13               0x0194 0x0384 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_06__ENET_enet_rx_en              0x0194 0x0384 0x043c 3 1
#define IMXRT_PAD_GPIO_B1_06__FLEXIO2_IO22                 0x0194 0x0384 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_06__GPIO2_gpio_io22              0x0194 0x0384 0x0000 5 0

/* GPIO_B1_07 */
#define IMXRT_PAD_GPIO_B1_07__LCDIF_lcdif_data19           0x0198 0x0388 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_07__LPSPI4_SCK                   0x0198 0x0388 0x0520 1 1
#define IMXRT_PAD_GPIO_B1_07__CSI_csi_data12               0x0198 0x0388 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_07__ENET_enet_tx_data0           0x0198 0x0388 0x0000 3 0
#define IMXRT_PAD_GPIO_B1_07__FLEXIO2_IO23                 0x0198 0x0388 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_07__GPIO2_gpio_io23              0x0198 0x0388 0x0000 5 0

/* GPIO_B1_08 */
#define IMXRT_PAD_GPIO_B1_08__LCDIF_lcdif_data20           0x019c 0x038c 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_08__TMR1_TIMER3                  0x019c 0x038c 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER1_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B1_08__CSI_csi_data11               0x019c 0x038c 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_08__ENET_enet_tx_data1           0x019c 0x038c 0x0000 3 0
#define IMXRT_PAD_GPIO_B1_08__FLEXIO2_IO24                 0x019c 0x038c 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_08__GPIO2_gpio_io24              0x019c 0x038c 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_08__CAN2_TX                      0x019c 0x038c 0x0000 6 0

/* GPIO_B1_09 */
#define IMXRT_PAD_GPIO_B1_09__LCDIF_lcdif_data21           0x01a0 0x0390 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_09__TMR2_TIMER3                  0x01a0 0x0390 0x0578 1 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B1_09__CSI_csi_data10               0x01a0 0x0390 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_09__ENET_enet_tx_en              0x01a0 0x0390 0x0000 3 0
#define IMXRT_PAD_GPIO_B1_09__FLEXIO2_IO25                 0x01a0 0x0390 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_09__GPIO2_gpio_io25              0x01a0 0x0390 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_09__CAN2_RX                      0x01a0 0x0390 0x0450 6 3

/* GPIO_B1_10 */
#define IMXRT_PAD_GPIO_B1_10__LCDIF_lcdif_data22           0x01a4 0x0394 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_10__TMR3_TIMER3                  0x01a4 0x0394 0x0588 1 2 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B1_10__CSI_csi_data00               0x01a4 0x0394 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_10__ENET_enet_tx_clk             0x01a4 0x0394 0x0448 3 1
#define IMXRT_PAD_GPIO_B1_10__FLEXIO2_IO26                 0x01a4 0x0394 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_10__GPIO2_gpio_io26              0x01a4 0x0394 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_10__ENET_enet_ref_clk            0x01a4 0x0394 0x042c 6 1

/* GPIO_B1_11 */
#define IMXRT_PAD_GPIO_B1_11__LCDIF_lcdif_data23           0x01a8 0x0398 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_11__TMR4_TIMER3                  0x01a8 0x0398 0x0000 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER4_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_B1_11__CSI_csi_data01               0x01a8 0x0398 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_11__ENET_enet_rx_er              0x01a8 0x0398 0x0440 3 1
#define IMXRT_PAD_GPIO_B1_11__FLEXIO2_IO27                 0x01a8 0x0398 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_11__GPIO2_gpio_io27              0x01a8 0x0398 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_11__LPSPI4_PCS3                  0x01a8 0x0398 0x0000 6 0

/* GPIO_B1_12 */
#define IMXRT_PAD_GPIO_B1_12__LPUART5_TX                   0x01ac 0x039c 0x054c 1 1
#define IMXRT_PAD_GPIO_B1_12__CSI_csi_pixclk               0x01ac 0x039c 0x0424 2 1
#define IMXRT_PAD_GPIO_B1_12__ENET_1588_EVENT0_IN          0x01ac 0x039c 0x0444 3 2
#define IMXRT_PAD_GPIO_B1_12__FLEXIO2_IO28                 0x01ac 0x039c 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_12__GPIO2_gpio_io28              0x01ac 0x039c 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_12__USDHC1_usdhc_cd_b            0x01ac 0x039c 0x05d4 6 2

/* GPIO_B1_13 */
#define IMXRT_PAD_GPIO_B1_13__WDOG1_wdog_b                 0x01b0 0x03a0 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_13__LPUART5_RX                   0x01b0 0x03a0 0x0548 1 1
#define IMXRT_PAD_GPIO_B1_13__CSI_csi_vsync                0x01b0 0x03a0 0x0428 2 2
#define IMXRT_PAD_GPIO_B1_13__ENET_1588_EVENT0_OUT         0x01b0 0x03a0 0x0000 3 0
#define IMXRT_PAD_GPIO_B1_13__FLEXIO2_IO29                 0x01b0 0x03a0 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_13__GPIO2_gpio_io29              0x01b0 0x03a0 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_13__USDHC1_usdhc_wp              0x01b0 0x03a0 0x05d8 6 3

/* GPIO_B1_14 */
#define IMXRT_PAD_GPIO_B1_14__ENET_enet_mdc                0x01b4 0x03a4 0x0000 0 0
#define IMXRT_PAD_GPIO_B1_14__PWM4_A2                      0x01b4 0x03a4 0x049c 1 1
#define IMXRT_PAD_GPIO_B1_14__CSI_csi_hsync                0x01b4 0x03a4 0x0420 2 2
#define IMXRT_PAD_GPIO_B1_14__XBARA1_IN02                  0x01b4 0x03a4 0x060c 3 1
#define IMXRT_PAD_GPIO_B1_14__FLEXIO2_IO30                 0x01b4 0x03a4 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_14__GPIO2_gpio_io30              0x01b4 0x03a4 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_14__USDHC1_usdhc_vselect         0x01b4 0x03a4 0x0000 6 0

/* GPIO_B1_15 */
#define IMXRT_PAD_GPIO_B1_15__ENET_enet_mdio               0x01b8 0x03a8 0x0430 0 2
#define IMXRT_PAD_GPIO_B1_15__PWM4_A3                      0x01b8 0x03a8 0x04a0 1 1
#define IMXRT_PAD_GPIO_B1_15__CSI_csi_mclk                 0x01b8 0x03a8 0x0000 2 0
#define IMXRT_PAD_GPIO_B1_15__XBARA1_IN03                  0x01b8 0x03a8 0x0610 3 1
#define IMXRT_PAD_GPIO_B1_15__FLEXIO2_IO31                 0x01b8 0x03a8 0x0000 4 0
#define IMXRT_PAD_GPIO_B1_15__GPIO2_gpio_io31              0x01b8 0x03a8 0x0000 5 0
#define IMXRT_PAD_GPIO_B1_15__USDHC1_usdhc_reset_b         0x01b8 0x03a8 0x0000 6 0

/* GPIO_EMC_00 */
#define IMXRT_PAD_GPIO_EMC_00__SEMC_DATA00                 0x0014 0x0204 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_00__PWM4_A0                     0x0014 0x0204 0x0494 1 0
#define IMXRT_PAD_GPIO_EMC_00__LPSPI2_SCK                  0x0014 0x0204 0x0500 2 1
#define IMXRT_PAD_GPIO_EMC_00__XBARA1_IN02                 0x0014 0x0204 0x060c 3 0
#define IMXRT_PAD_GPIO_EMC_00__FLEXIO1_IO00                0x0014 0x0204 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_00__GPIO4_gpio_io00             0x0014 0x0204 0x0000 5 0

/* GPIO_EMC_01 */
#define IMXRT_PAD_GPIO_EMC_01__SEMC_DATA01                 0x0018 0x0208 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_01__PWM4_B0                     0x0018 0x0208 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_01__LPSPI2_PCS0                 0x0018 0x0208 0x04fc 2 1
#define IMXRT_PAD_GPIO_EMC_01__XBARA1_IN03                 0x0018 0x0208 0x0610 3 0
#define IMXRT_PAD_GPIO_EMC_01__FLEXIO1_IO01                0x0018 0x0208 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_01__GPIO4_gpio_io01             0x0018 0x0208 0x0000 5 0

/* GPIO_EMC_02 */
#define IMXRT_PAD_GPIO_EMC_02__SEMC_DATA02                 0x001c 0x020c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_02__PWM4_A1                     0x001c 0x020c 0x0498 1 0
#define IMXRT_PAD_GPIO_EMC_02__LPSPI2_SDO                  0x001c 0x020c 0x0508 2 1
#define IMXRT_PAD_GPIO_EMC_02__XBARA1_OUT04                0x001c 0x020c 0x0614 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_4 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_02__XBARA1_IN04                 0x001c 0x020c 0x0614 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_4 to 0 */
#define IMXRT_PAD_GPIO_EMC_02__FLEXIO1_IO02                0x001c 0x020c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_02__GPIO4_gpio_io02             0x001c 0x020c 0x0000 5 0

/* GPIO_EMC_03 */
#define IMXRT_PAD_GPIO_EMC_03__SEMC_DATA03                 0x0020 0x0210 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_03__PWM4_B1                     0x0020 0x0210 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_03__LPSPI2_SDI                  0x0020 0x0210 0x0504 2 1
#define IMXRT_PAD_GPIO_EMC_03__XBARA1_OUT05                0x0020 0x0210 0x0618 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_5 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_03__XBARA1_IN05                 0x0020 0x0210 0x0618 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_5 to 0 */
#define IMXRT_PAD_GPIO_EMC_03__FLEXIO1_IO03                0x0020 0x0210 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_03__GPIO4_gpio_io03             0x0020 0x0210 0x0000 5 0

/* GPIO_EMC_04 */
#define IMXRT_PAD_GPIO_EMC_04__SEMC_DATA04                 0x0024 0x0214 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_04__PWM4_A2                     0x0024 0x0214 0x049c 1 0
#define IMXRT_PAD_GPIO_EMC_04__SAI2_sai_tx_data            0x0024 0x0214 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_04__XBARA1_OUT06                0x0024 0x0214 0x061c 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_6 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_04__XBARA1_IN06                 0x0024 0x0214 0x061c 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_6 to 0 */
#define IMXRT_PAD_GPIO_EMC_04__FLEXIO1_IO04                0x0024 0x0214 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_04__GPIO4_gpio_io04             0x0024 0x0214 0x0000 5 0

/* GPIO_EMC_05 */
#define IMXRT_PAD_GPIO_EMC_05__SEMC_DATA05                 0x0028 0x0218 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_05__PWM4_B2                     0x0028 0x0218 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_05__SAI2_sai_tx_sync            0x0028 0x0218 0x05c4 2 0
#define IMXRT_PAD_GPIO_EMC_05__XBARA1_OUT07                0x0028 0x0218 0x0620 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_7 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_05__XBARA1_IN07                 0x0028 0x0218 0x0620 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_7 to 0 */
#define IMXRT_PAD_GPIO_EMC_05__FLEXIO1_IO05                0x0028 0x0218 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_05__GPIO4_gpio_io05             0x0028 0x0218 0x0000 5 0

/* GPIO_EMC_06 */
#define IMXRT_PAD_GPIO_EMC_06__SEMC_DATA06                 0x002c 0x021c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_06__PWM2_A0                     0x002c 0x021c 0x0478 1 0
#define IMXRT_PAD_GPIO_EMC_06__SAI2_sai_tx_bclk            0x002c 0x021c 0x05c0 2 0
#define IMXRT_PAD_GPIO_EMC_06__XBARA1_OUT08                0x002c 0x021c 0x0624 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_8 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_06__XBARA1_IN08                 0x002c 0x021c 0x0624 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_8 to 0 */
#define IMXRT_PAD_GPIO_EMC_06__FLEXIO1_IO06                0x002c 0x021c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_06__GPIO4_gpio_io06             0x002c 0x021c 0x0000 5 0

/* GPIO_EMC_07 */
#define IMXRT_PAD_GPIO_EMC_07__SEMC_DATA07                 0x0030 0x0220 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_07__PWM2_B0                     0x0030 0x0220 0x0488 1 0
#define IMXRT_PAD_GPIO_EMC_07__SAI2_sai_mclk               0x0030 0x0220 0x05b0 2 0
#define IMXRT_PAD_GPIO_EMC_07__XBARA1_OUT09                0x0030 0x0220 0x0628 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_9 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_07__XBARA1_IN09                 0x0030 0x0220 0x0628 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_9 to 0 */
#define IMXRT_PAD_GPIO_EMC_07__FLEXIO1_IO07                0x0030 0x0220 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_07__GPIO4_gpio_io07             0x0030 0x0220 0x0000 5 0

/* GPIO_EMC_08 */
#define IMXRT_PAD_GPIO_EMC_08__SEMC_DM0                    0x0034 0x0224 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_08__PWM2_A1                     0x0034 0x0224 0x047c 1 0
#define IMXRT_PAD_GPIO_EMC_08__SAI2_sai_rx_data            0x0034 0x0224 0x05b8 2 0
#define IMXRT_PAD_GPIO_EMC_08__XBARA1_OUT17                0x0034 0x0224 0x062c 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_08__XBARA1_IN17                 0x0034 0x0224 0x062c 3 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_17 to 0 */
#define IMXRT_PAD_GPIO_EMC_08__FLEXIO1_IO08                0x0034 0x0224 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_08__GPIO4_gpio_io08             0x0034 0x0224 0x0000 5 0

/* GPIO_EMC_09 */
#define IMXRT_PAD_GPIO_EMC_09__SEMC_ADDR00                 0x0038 0x0228 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_09__PWM2_B1                     0x0038 0x0228 0x048c 1 0
#define IMXRT_PAD_GPIO_EMC_09__SAI2_sai_rx_sync            0x0038 0x0228 0x05bc 2 0
#define IMXRT_PAD_GPIO_EMC_09__CAN2_TX                     0x0038 0x0228 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_09__FLEXIO1_IO09                0x0038 0x0228 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_09__GPIO4_gpio_io09             0x0038 0x0228 0x0000 5 0

/* GPIO_EMC_10 */
#define IMXRT_PAD_GPIO_EMC_10__SEMC_ADDR01                 0x003c 0x022c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_10__PWM2_A2                     0x003c 0x022c 0x0480 1 0
#define IMXRT_PAD_GPIO_EMC_10__SAI2_sai_rx_bclk            0x003c 0x022c 0x05b4 2 0
#define IMXRT_PAD_GPIO_EMC_10__CAN2_RX                     0x003c 0x022c 0x0450 3 0
#define IMXRT_PAD_GPIO_EMC_10__FLEXIO1_IO10                0x003c 0x022c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_10__GPIO4_gpio_io10             0x003c 0x022c 0x0000 5 0

/* GPIO_EMC_11 */
#define IMXRT_PAD_GPIO_EMC_11__SEMC_ADDR02                 0x0040 0x0230 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_11__PWM2_B2                     0x0040 0x0230 0x0490 1 0
#define IMXRT_PAD_GPIO_EMC_11__LPI2C4_SDA                  0x0040 0x0230 0x04e8 2 0
#define IMXRT_PAD_GPIO_EMC_11__USDHC2_usdhc_reset_b        0x0040 0x0230 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_11__FLEXIO1_IO11                0x0040 0x0230 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_11__GPIO4_gpio_io11             0x0040 0x0230 0x0000 5 0

/* GPIO_EMC_12 */
#define IMXRT_PAD_GPIO_EMC_12__SEMC_ADDR03                 0x0044 0x0234 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_12__XBARA1_IN24                 0x0044 0x0234 0x0640 1 0
#define IMXRT_PAD_GPIO_EMC_12__LPI2C4_SCL                  0x0044 0x0234 0x04e4 2 0
#define IMXRT_PAD_GPIO_EMC_12__USDHC1_usdhc_wp             0x0044 0x0234 0x05d8 3 0
#define IMXRT_PAD_GPIO_EMC_12__PWM1_A3                     0x0044 0x0234 0x0454 4 1
#define IMXRT_PAD_GPIO_EMC_12__GPIO4_gpio_io12             0x0044 0x0234 0x0000 5 0

/* GPIO_EMC_13 */
#define IMXRT_PAD_GPIO_EMC_13__SEMC_ADDR04                 0x0048 0x0238 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_13__XBARA1_IN25                 0x0048 0x0238 0x0650 1 1
#define IMXRT_PAD_GPIO_EMC_13__LPUART3_TX                  0x0048 0x0238 0x053c 2 1
#define IMXRT_PAD_GPIO_EMC_13__MQS_RIGHT                   0x0048 0x0238 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_13__PWM1_B3                     0x0048 0x0238 0x0464 4 1
#define IMXRT_PAD_GPIO_EMC_13__GPIO4_gpio_io13             0x0048 0x0238 0x0000 5 0

/* GPIO_EMC_14 */
#define IMXRT_PAD_GPIO_EMC_14__SEMC_ADDR05                 0x004c 0x023c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_14__XBARA1_OUT19                0x004c 0x023c 0x0654 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_19 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_14__XBARA1_IN19                 0x004c 0x023c 0x0654 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_19 to 0 */
#define IMXRT_PAD_GPIO_EMC_14__LPUART3_RX                  0x004c 0x023c 0x0538 2 1
#define IMXRT_PAD_GPIO_EMC_14__MQS_LEFT                    0x004c 0x023c 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_14__LPSPI2_PCS1                 0x004c 0x023c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_14__GPIO4_gpio_io14             0x004c 0x023c 0x0000 5 0

/* GPIO_EMC_15 */
#define IMXRT_PAD_GPIO_EMC_15__SEMC_ADDR06                 0x0050 0x0240 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_15__XBARA1_IN20                 0x0050 0x0240 0x0634 1 0
#define IMXRT_PAD_GPIO_EMC_15__LPUART3_CTS_B               0x0050 0x0240 0x0534 2 0
#define IMXRT_PAD_GPIO_EMC_15__SPDIF_spdif_out             0x0050 0x0240 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_15__TMR3_TIMER0                 0x0050 0x0240 0x057c 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_15__GPIO4_gpio_io15             0x0050 0x0240 0x0000 5 0

/* GPIO_EMC_16 */
#define IMXRT_PAD_GPIO_EMC_16__SEMC_ADDR07                 0x0054 0x0244 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_16__XBARA1_IN21                 0x0054 0x0244 0x0658 1 0
#define IMXRT_PAD_GPIO_EMC_16__LPUART3_RTS_B               0x0054 0x0244 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_16__SPDIF_spdif_in              0x0054 0x0244 0x05c8 3 1
#define IMXRT_PAD_GPIO_EMC_16__TMR3_TIMER1                 0x0054 0x0244 0x0580 4 1 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_16__GPIO4_gpio_io16             0x0054 0x0244 0x0000 5 0

/* GPIO_EMC_17 */
#define IMXRT_PAD_GPIO_EMC_17__SEMC_ADDR08                 0x0058 0x0248 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_17__PWM4_A3                     0x0058 0x0248 0x04a0 1 0
#define IMXRT_PAD_GPIO_EMC_17__LPUART4_CTS_B               0x0058 0x0248 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_17__CAN1_TX                     0x0058 0x0248 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_17__TMR3_TIMER2                 0x0058 0x0248 0x0584 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_17__GPIO4_gpio_io17             0x0058 0x0248 0x0000 5 0

/* GPIO_EMC_18 */
#define IMXRT_PAD_GPIO_EMC_18__SEMC_ADDR09                 0x005c 0x024c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_18__PWM4_B3                     0x005c 0x024c 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_18__LPUART4_RTS_B               0x005c 0x024c 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_18__CAN1_RX                     0x005c 0x024c 0x044c 3 1
#define IMXRT_PAD_GPIO_EMC_18__TMR3_TIMER3                 0x005c 0x024c 0x0588 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER3_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_18__GPIO4_gpio_io18             0x005c 0x024c 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_18__SNVS_snvs_vio_5_ctl         0x005c 0x024c 0x0000 6 0

/* GPIO_EMC_19 */
#define IMXRT_PAD_GPIO_EMC_19__SEMC_ADDR11                 0x0060 0x0250 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_19__PWM2_A3                     0x0060 0x0250 0x0474 1 1
#define IMXRT_PAD_GPIO_EMC_19__LPUART4_TX                  0x0060 0x0250 0x0544 2 1
#define IMXRT_PAD_GPIO_EMC_19__ENET_enet_rx_data1          0x0060 0x0250 0x0438 3 0
#define IMXRT_PAD_GPIO_EMC_19__TMR2_TIMER0                 0x0060 0x0250 0x056c 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM0_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_19__GPIO4_gpio_io19             0x0060 0x0250 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_19__SNVS_snvs_vio_5_b           0x0060 0x0250 0x0000 6 0

/* GPIO_EMC_20 */
#define IMXRT_PAD_GPIO_EMC_20__SEMC_ADDR12                 0x0064 0x0254 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_20__PWM2_B3                     0x0064 0x0254 0x0484 1 1
#define IMXRT_PAD_GPIO_EMC_20__LPUART4_RX                  0x0064 0x0254 0x0540 2 1
#define IMXRT_PAD_GPIO_EMC_20__ENET_enet_rx_data0          0x0064 0x0254 0x0434 3 0
#define IMXRT_PAD_GPIO_EMC_20__TMR2_TIMER1                 0x0064 0x0254 0x0570 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM1_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_20__GPIO4_gpio_io20             0x0064 0x0254 0x0000 5 0

/* GPIO_EMC_21 */
#define IMXRT_PAD_GPIO_EMC_21__SEMC_BA0                    0x0068 0x0258 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_21__PWM3_A3                     0x0068 0x0258 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_21__LPI2C3_SDA                  0x0068 0x0258 0x04e0 2 0
#define IMXRT_PAD_GPIO_EMC_21__ENET_enet_tx_data1          0x0068 0x0258 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_21__TMR2_TIMER2                 0x0068 0x0258 0x0574 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM2_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_21__GPIO4_gpio_io21             0x0068 0x0258 0x0000 5 0

/* GPIO_EMC_22 */
#define IMXRT_PAD_GPIO_EMC_22__SEMC_BA1                    0x006c 0x025c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_22__PWM3_B3                     0x006c 0x025c 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_22__LPI2C3_SCL                  0x006c 0x025c 0x04dc 2 0
#define IMXRT_PAD_GPIO_EMC_22__ENET_enet_tx_data0          0x006c 0x025c 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_22__TMR2_TIMER3                 0x006c 0x025c 0x0578 4 0 /* Maybe you want to set IOMUXC_GPR_GPR6 QTIMER2_TRM3_INPUT_SEL to 0 */
#define IMXRT_PAD_GPIO_EMC_22__GPIO4_gpio_io22             0x006c 0x025c 0x0000 5 0

/* GPIO_EMC_23 */
#define IMXRT_PAD_GPIO_EMC_23__SEMC_ADDR10                 0x0070 0x0260 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_23__PWM1_A0                     0x0070 0x0260 0x0458 1 0
#define IMXRT_PAD_GPIO_EMC_23__LPUART5_TX                  0x0070 0x0260 0x054c 2 0
#define IMXRT_PAD_GPIO_EMC_23__ENET_enet_rx_en             0x0070 0x0260 0x043c 3 0
#define IMXRT_PAD_GPIO_EMC_23__GPT1_gpt_capture2           0x0070 0x0260 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_23__GPIO4_gpio_io23             0x0070 0x0260 0x0000 5 0

/* GPIO_EMC_24 */
#define IMXRT_PAD_GPIO_EMC_24__SEMC_semc_cas               0x0074 0x0264 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_24__PWM1_B0                     0x0074 0x0264 0x0468 1 0
#define IMXRT_PAD_GPIO_EMC_24__LPUART5_RX                  0x0074 0x0264 0x0548 2 0
#define IMXRT_PAD_GPIO_EMC_24__ENET_enet_tx_en             0x0074 0x0264 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_24__GPT1_gpt_capture1           0x0074 0x0264 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_24__GPIO4_gpio_io24             0x0074 0x0264 0x0000 5 0

/* GPIO_EMC_25 */
#define IMXRT_PAD_GPIO_EMC_25__SEMC_semc_ras               0x0078 0x0268 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_25__PWM1_A1                     0x0078 0x0268 0x045c 1 0
#define IMXRT_PAD_GPIO_EMC_25__LPUART6_TX                  0x0078 0x0268 0x0554 2 0
#define IMXRT_PAD_GPIO_EMC_25__ENET_enet_tx_clk            0x0078 0x0268 0x0448 3 0
#define IMXRT_PAD_GPIO_EMC_25__ENET_enet_ref_clk           0x0078 0x0268 0x042c 4 0
#define IMXRT_PAD_GPIO_EMC_25__GPIO4_gpio_io25             0x0078 0x0268 0x0000 5 0

/* GPIO_EMC_26 */
#define IMXRT_PAD_GPIO_EMC_26__SEMC_semc_clk               0x007c 0x026c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_26__PWM1_B1                     0x007c 0x026c 0x046c 1 0
#define IMXRT_PAD_GPIO_EMC_26__LPUART6_RX                  0x007c 0x026c 0x0550 2 0
#define IMXRT_PAD_GPIO_EMC_26__ENET_enet_rx_er             0x007c 0x026c 0x0440 3 0
#define IMXRT_PAD_GPIO_EMC_26__FLEXIO1_IO12                0x007c 0x026c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_26__GPIO4_gpio_io26             0x007c 0x026c 0x0000 5 0

/* GPIO_EMC_27 */
#define IMXRT_PAD_GPIO_EMC_27__SEMC_semc_cke               0x0080 0x0270 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_27__PWM1_A2                     0x0080 0x0270 0x0460 1 0
#define IMXRT_PAD_GPIO_EMC_27__LPUART5_RTS_B               0x0080 0x0270 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_27__LPSPI1_SCK                  0x0080 0x0270 0x04f0 3 0
#define IMXRT_PAD_GPIO_EMC_27__FLEXIO1_IO13                0x0080 0x0270 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_27__GPIO4_gpio_io27             0x0080 0x0270 0x0000 5 0

/* GPIO_EMC_28 */
#define IMXRT_PAD_GPIO_EMC_28__SEMC_semc_we                0x0084 0x0274 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_28__PWM1_B2                     0x0084 0x0274 0x0470 1 0
#define IMXRT_PAD_GPIO_EMC_28__LPUART5_CTS_B               0x0084 0x0274 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_28__LPSPI1_SDO                  0x0084 0x0274 0x04f8 3 0
#define IMXRT_PAD_GPIO_EMC_28__FLEXIO1_IO14                0x0084 0x0274 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_28__GPIO4_gpio_io28             0x0084 0x0274 0x0000 5 0

/* GPIO_EMC_29 */
#define IMXRT_PAD_GPIO_EMC_29__SEMC_CS0                    0x0088 0x0278 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_29__PWM3_A0                     0x0088 0x0278 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_29__LPUART6_RTS_B               0x0088 0x0278 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_29__LPSPI1_SDI                  0x0088 0x0278 0x04f4 3 0
#define IMXRT_PAD_GPIO_EMC_29__FLEXIO1_IO15                0x0088 0x0278 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_29__GPIO4_gpio_io29             0x0088 0x0278 0x0000 5 0

/* GPIO_EMC_30 */
#define IMXRT_PAD_GPIO_EMC_30__SEMC_DATA08                 0x008c 0x027c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_30__PWM3_B0                     0x008c 0x027c 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_30__LPUART6_CTS_B               0x008c 0x027c 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_30__LPSPI1_PCS0                 0x008c 0x027c 0x04ec 3 1
#define IMXRT_PAD_GPIO_EMC_30__CSI_csi_data23              0x008c 0x027c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_30__GPIO4_gpio_io30             0x008c 0x027c 0x0000 5 0

/* GPIO_EMC_31 */
#define IMXRT_PAD_GPIO_EMC_31__SEMC_DATA09                 0x0090 0x0280 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_31__PWM3_A1                     0x0090 0x0280 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_31__LPUART7_TX                  0x0090 0x0280 0x055c 2 1
#define IMXRT_PAD_GPIO_EMC_31__LPSPI1_PCS1                 0x0090 0x0280 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_31__CSI_csi_data22              0x0090 0x0280 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_31__GPIO4_gpio_io31             0x0090 0x0280 0x0000 5 0

/* GPIO_EMC_32 */
#define IMXRT_PAD_GPIO_EMC_32__SEMC_DATA10                 0x0094 0x0284 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_32__PWM3_B1                     0x0094 0x0284 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_32__LPUART7_RX                  0x0094 0x0284 0x0558 2 1
#define IMXRT_PAD_GPIO_EMC_32__CCM_PMIC_RDY                0x0094 0x0284 0x03fc 3 4
#define IMXRT_PAD_GPIO_EMC_32__CSI_csi_data21              0x0094 0x0284 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_32__GPIO3_gpio_io18             0x0094 0x0284 0x0000 5 0

/* GPIO_EMC_33 */
#define IMXRT_PAD_GPIO_EMC_33__SEMC_DATA11                 0x0098 0x0288 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_33__PWM3_A2                     0x0098 0x0288 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_33__USDHC1_usdhc_reset_b        0x0098 0x0288 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_33__SAI3_sai_rx_data            0x0098 0x0288 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_33__CSI_csi_data20              0x0098 0x0288 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_33__GPIO3_gpio_io19             0x0098 0x0288 0x0000 5 0

/* GPIO_EMC_34 */
#define IMXRT_PAD_GPIO_EMC_34__SEMC_DATA12                 0x009c 0x028c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_34__PWM3_B2                     0x009c 0x028c 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_34__USDHC1_usdhc_vselect        0x009c 0x028c 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_34__SAI3_sai_rx_sync            0x009c 0x028c 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_34__CSI_csi_data19              0x009c 0x028c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_34__GPIO3_gpio_io20             0x009c 0x028c 0x0000 5 0

/* GPIO_EMC_35 */
#define IMXRT_PAD_GPIO_EMC_35__SEMC_DATA13                 0x00a0 0x0290 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_35__XBARA1_OUT18                0x00a0 0x0290 0x0630 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_18 to 0x1 */
#define IMXRT_PAD_GPIO_EMC_35__XBARA1_IN18                 0x00a0 0x0290 0x0630 1 0 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_18 to 0 */
#define IMXRT_PAD_GPIO_EMC_35__GPT1_gpt_compare1           0x00a0 0x0290 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_35__SAI3_sai_rx_bclk            0x00a0 0x0290 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_35__CSI_csi_data18              0x00a0 0x0290 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_35__GPIO3_gpio_io21             0x00a0 0x0290 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_35__USDHC1_usdhc_cd_b           0x00a0 0x0290 0x05d4 6 0

/* GPIO_EMC_36 */
#define IMXRT_PAD_GPIO_EMC_36__SEMC_DATA14                 0x00a4 0x0294 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_36__XBARA1_IN22                 0x00a4 0x0294 0x0638 1 0
#define IMXRT_PAD_GPIO_EMC_36__GPT1_gpt_compare2           0x00a4 0x0294 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_36__SAI3_sai_tx_data            0x00a4 0x0294 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_36__CSI_csi_data17              0x00a4 0x0294 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_36__GPIO3_gpio_io22             0x00a4 0x0294 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_36__USDHC1_usdhc_wp             0x00a4 0x0294 0x05d8 6 1

/* GPIO_EMC_37 */
#define IMXRT_PAD_GPIO_EMC_37__SEMC_DATA15                 0x00a8 0x0298 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_37__XBARA1_IN23                 0x00a8 0x0298 0x063c 1 0
#define IMXRT_PAD_GPIO_EMC_37__GPT1_gpt_compare3           0x00a8 0x0298 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_37__SAI3_sai_mclk               0x00a8 0x0298 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_37__CSI_csi_data16              0x00a8 0x0298 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_37__GPIO3_gpio_io23             0x00a8 0x0298 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_37__USDHC2_usdhc_wp             0x00a8 0x0298 0x0608 6 0

/* GPIO_EMC_38 */
#define IMXRT_PAD_GPIO_EMC_38__SEMC_DM1                    0x00ac 0x029c 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_38__PWM1_A3                     0x00ac 0x029c 0x0454 1 2
#define IMXRT_PAD_GPIO_EMC_38__LPUART8_TX                  0x00ac 0x029c 0x0564 2 2
#define IMXRT_PAD_GPIO_EMC_38__SAI3_sai_tx_bclk            0x00ac 0x029c 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_38__CSI_csi_field               0x00ac 0x029c 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_38__GPIO3_gpio_io24             0x00ac 0x029c 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_38__USDHC2_usdhc_vselect        0x00ac 0x029c 0x0000 6 0

/* GPIO_EMC_39 */
#define IMXRT_PAD_GPIO_EMC_39__SEMC_semc_dqs               0x00b0 0x02a0 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_39__PWM1_B3                     0x00b0 0x02a0 0x0464 1 2
#define IMXRT_PAD_GPIO_EMC_39__LPUART8_RX                  0x00b0 0x02a0 0x0560 2 2
#define IMXRT_PAD_GPIO_EMC_39__SAI3_sai_tx_sync            0x00b0 0x02a0 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_39__WDOG1_wdog_b                0x00b0 0x02a0 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_39__GPIO3_gpio_io25             0x00b0 0x02a0 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_39__USDHC2_usdhc_cd_b           0x00b0 0x02a0 0x05e0 6 1

/* GPIO_EMC_40 */
#define IMXRT_PAD_GPIO_EMC_40__SEMC_semc_rdy               0x00b4 0x02a4 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_40__GPT2_gpt_capture2           0x00b4 0x02a4 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_40__LPSPI1_PCS2                 0x00b4 0x02a4 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_40__USB2_usb_otg2_oc            0x00b4 0x02a4 0x05cc 3 1
#define IMXRT_PAD_GPIO_EMC_40__ENET_enet_mdc               0x00b4 0x02a4 0x0000 4 0
#define IMXRT_PAD_GPIO_EMC_40__GPIO3_gpio_io26             0x00b4 0x02a4 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_40__USDHC2_usdhc_reset_b        0x00b4 0x02a4 0x0000 6 0

/* GPIO_EMC_41 */
#define IMXRT_PAD_GPIO_EMC_41__SEMC_CSX0                   0x00b8 0x02a8 0x0000 0 0
#define IMXRT_PAD_GPIO_EMC_41__GPT2_gpt_capture1           0x00b8 0x02a8 0x0000 1 0
#define IMXRT_PAD_GPIO_EMC_41__LPSPI1_PCS3                 0x00b8 0x02a8 0x0000 2 0
#define IMXRT_PAD_GPIO_EMC_41__USB2_usb_otg2_pwr           0x00b8 0x02a8 0x0000 3 0
#define IMXRT_PAD_GPIO_EMC_41__ENET_enet_mdio              0x00b8 0x02a8 0x0430 4 1
#define IMXRT_PAD_GPIO_EMC_41__GPIO3_gpio_io27             0x00b8 0x02a8 0x0000 5 0
#define IMXRT_PAD_GPIO_EMC_41__USDHC1_usdhc_vselect        0x00b8 0x02a8 0x0000 6 0

/* GPIO_SD_B0_00 */
#define IMXRT_PAD_GPIO_SD_B0_00__USDHC1_usdhc_cmd          0x01bc 0x03ac 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_00__PWM1_A0                   0x01bc 0x03ac 0x0458 1 1
#define IMXRT_PAD_GPIO_SD_B0_00__LPI2C3_SCL                0x01bc 0x03ac 0x04dc 2 1
#define IMXRT_PAD_GPIO_SD_B0_00__XBARA1_OUT04              0x01bc 0x03ac 0x0614 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_4 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_00__XBARA1_IN04               0x01bc 0x03ac 0x0614 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_4 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_00__LPSPI1_SCK                0x01bc 0x03ac 0x04f0 4 1
#define IMXRT_PAD_GPIO_SD_B0_00__GPIO3_gpio_io12           0x01bc 0x03ac 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B0_00__FLEXSPI_FLEXSPI_A_SS1_B   0x01bc 0x03ac 0x0000 6 0

/* GPIO_SD_B0_01 */
#define IMXRT_PAD_GPIO_SD_B0_01__USDHC1_usdhc_clk          0x01c0 0x03b0 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_01__PWM1_B0                   0x01c0 0x03b0 0x0468 1 1
#define IMXRT_PAD_GPIO_SD_B0_01__LPI2C3_SDA                0x01c0 0x03b0 0x04e0 2 1
#define IMXRT_PAD_GPIO_SD_B0_01__XBARA1_OUT05              0x01c0 0x03b0 0x0618 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_5 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_01__XBARA1_IN05               0x01c0 0x03b0 0x0618 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_5 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_01__LPSPI1_PCS0               0x01c0 0x03b0 0x04ec 4 0
#define IMXRT_PAD_GPIO_SD_B0_01__GPIO3_gpio_io13           0x01c0 0x03b0 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B0_01__FLEXSPI_FLEXSPI_B_SS1_B   0x01c0 0x03b0 0x0000 6 0

/* GPIO_SD_B0_02 */
#define IMXRT_PAD_GPIO_SD_B0_02__USDHC1_usdhc_data0        0x01c4 0x03b4 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_02__PWM1_A1                   0x01c4 0x03b4 0x045c 1 1
#define IMXRT_PAD_GPIO_SD_B0_02__LPUART8_CTS_B             0x01c4 0x03b4 0x0000 2 0
#define IMXRT_PAD_GPIO_SD_B0_02__XBARA1_OUT06              0x01c4 0x03b4 0x061c 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_6 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_02__XBARA1_IN06               0x01c4 0x03b4 0x061c 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_6 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_02__LPSPI1_SDO                0x01c4 0x03b4 0x04f8 4 1
#define IMXRT_PAD_GPIO_SD_B0_02__GPIO3_gpio_io14           0x01c4 0x03b4 0x0000 5 0

/* GPIO_SD_B0_03 */
#define IMXRT_PAD_GPIO_SD_B0_03__USDHC1_usdhc_data1        0x01c8 0x03b8 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_03__PWM1_B1                   0x01c8 0x03b8 0x046c 1 1
#define IMXRT_PAD_GPIO_SD_B0_03__LPUART8_RTS_B             0x01c8 0x03b8 0x0000 2 0
#define IMXRT_PAD_GPIO_SD_B0_03__XBARA1_OUT07              0x01c8 0x03b8 0x0620 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_7 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_03__XBARA1_IN07               0x01c8 0x03b8 0x0620 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_7 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_03__LPSPI1_SDI                0x01c8 0x03b8 0x04f4 4 1
#define IMXRT_PAD_GPIO_SD_B0_03__GPIO3_gpio_io15           0x01c8 0x03b8 0x0000 5 0

/* GPIO_SD_B0_04 */
#define IMXRT_PAD_GPIO_SD_B0_04__USDHC1_usdhc_data2        0x01cc 0x03bc 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_04__PWM1_A2                   0x01cc 0x03bc 0x0460 1 1
#define IMXRT_PAD_GPIO_SD_B0_04__LPUART8_TX                0x01cc 0x03bc 0x0564 2 0
#define IMXRT_PAD_GPIO_SD_B0_04__XBARA1_OUT08              0x01cc 0x03bc 0x0624 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_8 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_04__XBARA1_IN08               0x01cc 0x03bc 0x0624 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_8 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_04__FLEXSPI_FLEXSPI_B_SS0_B   0x01cc 0x03bc 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B0_04__GPIO3_gpio_io16           0x01cc 0x03bc 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B0_04__CCM_CLKO1                 0x01cc 0x03bc 0x0000 6 0

/* GPIO_SD_B0_05 */
#define IMXRT_PAD_GPIO_SD_B0_05__USDHC1_usdhc_data3        0x01d0 0x03c0 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B0_05__PWM1_B2                   0x01d0 0x03c0 0x0470 1 1
#define IMXRT_PAD_GPIO_SD_B0_05__LPUART8_RX                0x01d0 0x03c0 0x0560 2 0
#define IMXRT_PAD_GPIO_SD_B0_05__XBARA1_OUT09              0x01d0 0x03c0 0x0628 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_9 to 0x1 */
#define IMXRT_PAD_GPIO_SD_B0_05__XBARA1_IN09               0x01d0 0x03c0 0x0628 3 1 /* Maybe you want to set IOMUXC_GPR_GPR6 IOMUXC_XBAR_DIR_SEL_9 to 0 */
#define IMXRT_PAD_GPIO_SD_B0_05__FLEXSPI_FLEXSPI_B_DQS     0x01d0 0x03c0 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B0_05__GPIO3_gpio_io17           0x01d0 0x03c0 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B0_05__CCM_CLKO2                 0x01d0 0x03c0 0x0000 6 0

/* GPIO_SD_B1_00 */
#define IMXRT_PAD_GPIO_SD_B1_00__USDHC2_usdhc_data3        0x01d4 0x03c4 0x05f4 0 0
#define IMXRT_PAD_GPIO_SD_B1_00__FLEXSPI_FLEXSPI_B_DATA3   0x01d4 0x03c4 0x04c4 1 0
#define IMXRT_PAD_GPIO_SD_B1_00__PWM1_A3                   0x01d4 0x03c4 0x0454 2 0
#define IMXRT_PAD_GPIO_SD_B1_00__SAI1_sai_tx_data3         0x01d4 0x03c4 0x0598 3 0
#define IMXRT_PAD_GPIO_SD_B1_00__LPUART4_TX                0x01d4 0x03c4 0x0544 4 0
#define IMXRT_PAD_GPIO_SD_B1_00__GPIO3_gpio_io00           0x01d4 0x03c4 0x0000 5 0

/* GPIO_SD_B1_01 */
#define IMXRT_PAD_GPIO_SD_B1_01__USDHC2_usdhc_data2        0x01d8 0x03c8 0x05f0 0 0
#define IMXRT_PAD_GPIO_SD_B1_01__FLEXSPI_FLEXSPI_B_DATA2   0x01d8 0x03c8 0x04c0 1 0
#define IMXRT_PAD_GPIO_SD_B1_01__PWM1_B3                   0x01d8 0x03c8 0x0464 2 0
#define IMXRT_PAD_GPIO_SD_B1_01__SAI1_sai_tx_data2         0x01d8 0x03c8 0x059c 3 0
#define IMXRT_PAD_GPIO_SD_B1_01__LPUART4_RX                0x01d8 0x03c8 0x0540 4 0
#define IMXRT_PAD_GPIO_SD_B1_01__GPIO3_gpio_io01           0x01d8 0x03c8 0x0000 5 0

/* GPIO_SD_B1_02 */
#define IMXRT_PAD_GPIO_SD_B1_02__USDHC2_usdhc_data1        0x01dc 0x03cc 0x05ec 0 0
#define IMXRT_PAD_GPIO_SD_B1_02__FLEXSPI_FLEXSPI_B_DATA1   0x01dc 0x03cc 0x04bc 1 0
#define IMXRT_PAD_GPIO_SD_B1_02__PWM2_A3                   0x01dc 0x03cc 0x0474 2 0
#define IMXRT_PAD_GPIO_SD_B1_02__SAI1_sai_tx_data1         0x01dc 0x03cc 0x05a0 3 0
#define IMXRT_PAD_GPIO_SD_B1_02__CAN1_TX                   0x01dc 0x03cc 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B1_02__GPIO3_gpio_io02           0x01dc 0x03cc 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B1_02__CCM_WAIT                  0x01dc 0x03cc 0x0000 6 0

/* GPIO_SD_B1_03 */
#define IMXRT_PAD_GPIO_SD_B1_03__USDHC2_usdhc_data0        0x01e0 0x03d0 0x05e8 0 0
#define IMXRT_PAD_GPIO_SD_B1_03__FLEXSPI_FLEXSPI_B_DATA0   0x01e0 0x03d0 0x04b8 1 0
#define IMXRT_PAD_GPIO_SD_B1_03__PWM2_B3                   0x01e0 0x03d0 0x0484 2 0
#define IMXRT_PAD_GPIO_SD_B1_03__SAI1_sai_mclk             0x01e0 0x03d0 0x058c 3 0
#define IMXRT_PAD_GPIO_SD_B1_03__CAN1_RX                   0x01e0 0x03d0 0x044c 4 0
#define IMXRT_PAD_GPIO_SD_B1_03__GPIO3_gpio_io03           0x01e0 0x03d0 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B1_03__CCM_PMIC_RDY              0x01e0 0x03d0 0x03fc 6 0

/* GPIO_SD_B1_04 */
#define IMXRT_PAD_GPIO_SD_B1_04__USDHC2_usdhc_clk          0x01e4 0x03d4 0x05dc 0 0
#define IMXRT_PAD_GPIO_SD_B1_04__FLEXSPI_FLEXSPI_B_SCLK    0x01e4 0x03d4 0x0000 1 0
#define IMXRT_PAD_GPIO_SD_B1_04__LPI2C1_SCL                0x01e4 0x03d4 0x04cc 2 0
#define IMXRT_PAD_GPIO_SD_B1_04__SAI1_sai_rx_sync          0x01e4 0x03d4 0x05a4 3 0
#define IMXRT_PAD_GPIO_SD_B1_04__FLEXSPI_FLEXSPI_A_SS1_B   0x01e4 0x03d4 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B1_04__GPIO3_gpio_io04           0x01e4 0x03d4 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B1_04__CCM_STOP                  0x01e4 0x03d4 0x0000 6 0

/* GPIO_SD_B1_05 */
#define IMXRT_PAD_GPIO_SD_B1_05__USDHC2_usdhc_cmd          0x01e8 0x03d8 0x05e4 0 0
#define IMXRT_PAD_GPIO_SD_B1_05__FLEXSPI_FLEXSPI_A_DQS     0x01e8 0x03d8 0x04a4 1 0
#define IMXRT_PAD_GPIO_SD_B1_05__LPI2C1_SDA                0x01e8 0x03d8 0x04d0 2 0
#define IMXRT_PAD_GPIO_SD_B1_05__SAI1_sai_rx_bclk          0x01e8 0x03d8 0x0590 3 0
#define IMXRT_PAD_GPIO_SD_B1_05__FLEXSPI_FLEXSPI_B_SS0_B   0x01e8 0x03d8 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B1_05__GPIO3_gpio_io05           0x01e8 0x03d8 0x0000 5 0

/* GPIO_SD_B1_06 */
#define IMXRT_PAD_GPIO_SD_B1_06__USDHC2_usdhc_reset_b      0x01ec 0x03dc 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B1_06__FLEXSPI_FLEXSPI_A_SS0_B   0x01ec 0x03dc 0x0000 1 0
#define IMXRT_PAD_GPIO_SD_B1_06__LPUART7_CTS_B             0x01ec 0x03dc 0x0000 2 0
#define IMXRT_PAD_GPIO_SD_B1_06__SAI1_sai_rx_data0         0x01ec 0x03dc 0x0594 3 0
#define IMXRT_PAD_GPIO_SD_B1_06__LPSPI2_PCS0               0x01ec 0x03dc 0x04fc 4 0
#define IMXRT_PAD_GPIO_SD_B1_06__GPIO3_gpio_io06           0x01ec 0x03dc 0x0000 5 0

/* GPIO_SD_B1_07 */
#define IMXRT_PAD_GPIO_SD_B1_07__SEMC_CSX1                 0x01f0 0x03e0 0x0000 0 0
#define IMXRT_PAD_GPIO_SD_B1_07__FLEXSPI_FLEXSPI_A_SCLK    0x01f0 0x03e0 0x04c8 1 0
#define IMXRT_PAD_GPIO_SD_B1_07__LPUART7_RTS_B             0x01f0 0x03e0 0x0000 2 0
#define IMXRT_PAD_GPIO_SD_B1_07__SAI1_sai_tx_data0         0x01f0 0x03e0 0x0000 3 0
#define IMXRT_PAD_GPIO_SD_B1_07__LPSPI2_SCK                0x01f0 0x03e0 0x0500 4 0
#define IMXRT_PAD_GPIO_SD_B1_07__GPIO3_gpio_io07           0x01f0 0x03e0 0x0000 5 0

/* GPIO_SD_B1_08 */
#define IMXRT_PAD_GPIO_SD_B1_08__USDHC2_usdhc_data4        0x01f4 0x03e4 0x05f8 0 0
#define IMXRT_PAD_GPIO_SD_B1_08__FLEXSPI_FLEXSPI_A_DATA0   0x01f4 0x03e4 0x04a8 1 0
#define IMXRT_PAD_GPIO_SD_B1_08__LPUART7_TX                0x01f4 0x03e4 0x055c 2 0
#define IMXRT_PAD_GPIO_SD_B1_08__SAI1_sai_tx_bclk          0x01f4 0x03e4 0x05a8 3 0
#define IMXRT_PAD_GPIO_SD_B1_08__LPSPI2_SDO                0x01f4 0x03e4 0x0508 4 0
#define IMXRT_PAD_GPIO_SD_B1_08__GPIO3_gpio_io08           0x01f4 0x03e4 0x0000 5 0
#define IMXRT_PAD_GPIO_SD_B1_08__SEMC_CSX2                 0x01f4 0x03e4 0x0000 6 0

/* GPIO_SD_B1_09 */
#define IMXRT_PAD_GPIO_SD_B1_09__USDHC2_usdhc_data5        0x01f8 0x03e8 0x05fc 0 0
#define IMXRT_PAD_GPIO_SD_B1_09__FLEXSPI_FLEXSPI_A_DATA1   0x01f8 0x03e8 0x04ac 1 0
#define IMXRT_PAD_GPIO_SD_B1_09__LPUART7_RX                0x01f8 0x03e8 0x0558 2 0
#define IMXRT_PAD_GPIO_SD_B1_09__SAI1_sai_tx_sync          0x01f8 0x03e8 0x05ac 3 0
#define IMXRT_PAD_GPIO_SD_B1_09__LPSPI2_SDI                0x01f8 0x03e8 0x0504 4 0
#define IMXRT_PAD_GPIO_SD_B1_09__GPIO3_gpio_io09           0x01f8 0x03e8 0x0000 5 0

/* GPIO_SD_B1_10 */
#define IMXRT_PAD_GPIO_SD_B1_10__USDHC2_usdhc_data6        0x01fc 0x03ec 0x0600 0 0
#define IMXRT_PAD_GPIO_SD_B1_10__FLEXSPI_FLEXSPI_A_DATA2   0x01fc 0x03ec 0x04b0 1 0
#define IMXRT_PAD_GPIO_SD_B1_10__LPUART2_RX                0x01fc 0x03ec 0x052c 2 0
#define IMXRT_PAD_GPIO_SD_B1_10__LPI2C2_SDA                0x01fc 0x03ec 0x04d8 3 0
#define IMXRT_PAD_GPIO_SD_B1_10__LPSPI2_PCS2               0x01fc 0x03ec 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B1_10__GPIO3_gpio_io10           0x01fc 0x03ec 0x0000 5 0

/* GPIO_SD_B1_11 */
#define IMXRT_PAD_GPIO_SD_B1_11__USDHC2_usdhc_data7        0x0200 0x03f0 0x0604 0 0
#define IMXRT_PAD_GPIO_SD_B1_11__FLEXSPI_FLEXSPI_A_DATA3   0x0200 0x03f0 0x04b4 1 0
#define IMXRT_PAD_GPIO_SD_B1_11__LPUART2_TX                0x0200 0x03f0 0x0530 2 0
#define IMXRT_PAD_GPIO_SD_B1_11__LPI2C2_SCL                0x0200 0x03f0 0x04d4 3 0
#define IMXRT_PAD_GPIO_SD_B1_11__LPSPI2_PCS3               0x0200 0x03f0 0x0000 4 0
#define IMXRT_PAD_GPIO_SD_B1_11__GPIO3_gpio_io11           0x0200 0x03f0 0x0000 5 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_IMXRT_IMXRT1050_PINFUNC_H */
