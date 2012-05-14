/**
 * @file
 *
 * @ingroup lpc24xx_regs
 *
 * @brief Register definitions.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LPC17XX_REGS_H
#define LPC17XX_REGS_H

#include <bsp/utility.h>

#define LPC17XX_BASE 0x00

typedef struct {
#define LPC17XX_PLL_CON_PLLE BSP_BIT32(0)
#define LPC17XX_PLL_SEL_MSEL(val) BSP_FLD32(val, 0, 4)
#define LPC17XX_PLL_SEL_MSEL_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LPC17XX_PLL_SEL_MSEL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
#define LPC17XX_PLL_SEL_PSEL(val) BSP_FLD32(val, 5, 6)
#define LPC17XX_PLL_SEL_PSEL_GET(reg) BSP_FLD32GET(reg, 5, 6)
#define LPC17XX_PLL_SEL_PSEL_SET(reg, val) BSP_FLD32SET(reg, val, 5, 6)
#define LPC17XX_PLL_STAT_PLLE BSP_BIT32(8)
#define LPC17XX_PLL_STAT_PLOCK BSP_BIT32(10)
	uint32_t con;
	uint32_t cfg;
	uint32_t stat;
	uint32_t feed;
} lpc17xx_pll;

typedef struct {
	uint32_t flashcfg;
#define LPC17XX_SCB_FLASHCFG_FLASHTIM(val) BSP_FLD32(val, 12, 15)
#define LPC17XX_SCB_FLASHCFG_FLASHTIM_GET(reg) BSP_FLD32GET(reg, 12, 15)
#define LPC17XX_SCB_FLASHCFG_FLASHTIM_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
	uint32_t reserved_04 [15];
	uint32_t memmap;
#define LPC17XX_SCB_MEMMAP_MAP BSP_BIT32(0)
	uint32_t reserved_44 [15];
	lpc17xx_pll pll_0;
	uint32_t reserved_90 [4];
	lpc17xx_pll pll_1;
	uint32_t reserved_b0 [4];
	uint32_t pcon;
#define LPC17XX_SCB_PCON_PM0 BSP_BIT32(0)
#define LPC17XX_SCB_PCON_PM1 BSP_BIT32(1)
#define LPC17XX_SCB_PCON_BODRPM BSP_BIT32(2)
#define LPC17XX_SCB_PCON_BOGD BSP_BIT32(3)
#define LPC17XX_SCB_PCON_BORD BSP_BIT32(4)
#define LPC17XX_SCB_PCON_SMFLAG BSP_BIT32(8)
#define LPC17XX_SCB_PCON_DSFLAG BSP_BIT32(9)
#define LPC17XX_SCB_PCON_PDFLAG BSP_BIT32(10)
#define LPC17XX_SCB_PCON_DPDFLAG BSP_BIT32(11)
	uint32_t pconp;
#define LPC17XX_SCB_PCONP_LCD BSP_BIT32(0)
#define LPC17XX_SCB_PCONP_TIMER_0 BSP_BIT32(1)
#define LPC17XX_SCB_PCONP_TIMER_1 BSP_BIT32(2)
#define LPC17XX_SCB_PCONP_UART_0 BSP_BIT32(3)
#define LPC17XX_SCB_PCONP_UART_1 BSP_BIT32(4)
#define LPC17XX_SCB_PCONP_PWM_0 BSP_BIT32(5)
#define LPC17XX_SCB_PCONP_PWM_1 BSP_BIT32(6)
#define LPC17XX_SCB_PCONP_I2C_0 BSP_BIT32(7)
#define LPC17XX_SCB_PCONP_UART_4 BSP_BIT32(8)
#define LPC17XX_SCB_PCONP_RTC BSP_BIT32(9)
#define LPC17XX_SCB_PCONP_SSP_1 BSP_BIT32(10)
#define LPC17XX_SCB_PCONP_EMC BSP_BIT32(11)
#define LPC17XX_SCB_PCONP_ADC BSP_BIT32(12)
#define LPC17XX_SCB_PCONP_CAN_0 BSP_BIT32(13)
#define LPC17XX_SCB_PCONP_CAN_1 BSP_BIT32(14)
#define LPC17XX_SCB_PCONP_GPIO BSP_BIT32(15)
#define LPC17XX_SCB_PCONP_QEI BSP_BIT32(17)
#define LPC17XX_SCB_PCONP_I2C_1 BSP_BIT32(18)
#define LPC17XX_SCB_PCONP_SSP_2 BSP_BIT32(19)
#define LPC17XX_SCB_PCONP_SSP_0 BSP_BIT32(20)
#define LPC17XX_SCB_PCONP_TIMER_2 BSP_BIT32(21)
#define LPC17XX_SCB_PCONP_TIMER_3 BSP_BIT32(22)
#define LPC17XX_SCB_PCONP_UART_2 BSP_BIT32(23)
#define LPC17XX_SCB_PCONP_UART_3 BSP_BIT32(24)
#define LPC17XX_SCB_PCONP_I2C_2 BSP_BIT32(25)
#define LPC17XX_SCB_PCONP_I2S BSP_BIT32(26)
#define LPC17XX_SCB_PCONP_SDC BSP_BIT32(27)
#define LPC17XX_SCB_PCONP_GPDMA BSP_BIT32(28)
#define LPC17XX_SCB_PCONP_ENET BSP_BIT32(29)
#define LPC17XX_SCB_PCONP_USB BSP_BIT32(30)
#define LPC17XX_SCB_PCONP_MCPWM BSP_BIT32(31)
	uint32_t reserved_c8 [14];
	uint32_t emcclksel;
#define LPC17XX_SCB_EMCCLKSEL_EMCDIV BSP_BIT32(0)
	uint32_t cclksel;
#define LPC17XX_SCB_CCLKSEL_CCLKDIV(val) BSP_FLD32(val, 0, 4)
#define LPC17XX_SCB_CCLKSEL_CCLKDIV_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LPC17XX_SCB_CCLKSEL_CCLKDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
#define LPC17XX_SCB_CCLKSEL_CCLKSEL BSP_BIT32(8)
	uint32_t usbclksel;
#define LPC17XX_SCB_USBCLKSEL_USBDIV(val) BSP_FLD32(val, 0, 4)
#define LPC17XX_SCB_USBCLKSEL_USBDIV_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LPC17XX_SCB_USBCLKSEL_USBDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
#define LPC17XX_SCB_USBCLKSEL_USBSEL(val) BSP_FLD32(val, 8, 9)
#define LPC17XX_SCB_USBCLKSEL_USBSEL_GET(reg) BSP_FLD32GET(reg, 8, 9)
#define LPC17XX_SCB_USBCLKSEL_USBSEL_SET(reg, val) BSP_FLD32SET(reg, val, 8, 9)
	uint32_t clksrcsel;
#define LPC17XX_SCB_CLKSRCSEL_CLKSRC BSP_BIT32(0)
	uint32_t reserved_110 [12];
	uint32_t extint;
	uint32_t reserved_144;
	uint32_t extmode;
	uint32_t extpolar;
	uint32_t reserved_150 [12];
	uint32_t rsid;
	uint32_t reserved_184 [7];
	uint32_t scs;
#define LPC17XX_SCB_SCS_EMC_SHIFT_CTL BSP_BIT32(0)
#define LPC17XX_SCB_SCS_EMC_RESET_DIS BSP_BIT32(1)
#define LPC17XX_SCB_SCS_EMC_BURST_CTL BSP_BIT32(2)
#define LPC17XX_SCB_SCS_MCIPWR BSP_BIT32(3)
#define LPC17XX_SCB_SCS_OSC_RANGE_SEL BSP_BIT32(4)
#define LPC17XX_SCB_SCS_OSC_ENABLE BSP_BIT32(5)
#define LPC17XX_SCB_SCS_OSC_STATUS BSP_BIT32(6)
	uint32_t reserved_1a4;
	uint32_t pclksel;
#define LPC17XX_SCB_PCLKSEL_PCLKDIV(val) BSP_FLD32(val, 0, 4)
#define LPC17XX_SCB_PCLKSEL_PCLKDIV_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LPC17XX_SCB_PCLKSEL_PCLKDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
	uint32_t reserved_1ac;
	uint32_t pboost;
#define LPC17XX_SCB_PBOOST_BOOST BSP_BIT32(0)
	uint32_t reserved_1b4 [5];
	uint32_t clkoutcfg;
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTSEL(val) BSP_FLD32(val, 3, 0)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTSEL_GET(reg) BSP_FLD32GET(reg, 3, 0)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTSEL_SET(reg, val) BSP_FLD32SET(reg, val, 3, 0)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTDIV(val) BSP_FLD32(val, 7, 4)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTDIV_GET(reg) BSP_FLD32GET(reg, 7, 4)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUTDIV_SET(reg, val) BSP_FLD32SET(reg, val, 7, 4)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUT_EN BSP_BIT32(8)
#define LPC17XX_SCB_CLKOUTCFG_CLKOUT_ACT BSP_BIT32(9)
	uint32_t rstcon0;
	uint32_t rstcon1;
	uint32_t reserved_1d4 [2];
	uint32_t emcdlyctl;
#define LPC17XX_SCB_EMCDLYCTL_CMDDLY(val) BSP_FLD32(val, 0, 4)
#define LPC17XX_SCB_EMCDLYCTL_CMDDLY_GET(reg) BSP_FLD32GET(reg, 0, 4)
#define LPC17XX_SCB_EMCDLYCTL_CMDDLY_SET(reg, val) BSP_FLD32SET(reg, val, 0, 4)
#define LPC17XX_SCB_EMCDLYCTL_FBCLKDLY(val) BSP_FLD32(val, 8, 12)
#define LPC17XX_SCB_EMCDLYCTL_FBCLKDLY_GET(reg) BSP_FLD32GET(reg, 8, 12)
#define LPC17XX_SCB_EMCDLYCTL_FBCLKDLY_SET(reg, val) BSP_FLD32SET(reg, val, 8, 12)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT0DLY(val) BSP_FLD32(val, 16, 20)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT0DLY_GET(reg) BSP_FLD32GET(reg, 16, 20)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT0DLY_SET(reg, val) BSP_FLD32SET(reg, val, 16, 20)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT1DLY(val) BSP_FLD32(val, 24, 28)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT1DLY_GET(reg) BSP_FLD32GET(reg, 24, 28)
#define LPC17XX_SCB_EMCDLYCTL_CLKOUT1DLY_SET(reg, val) BSP_FLD32SET(reg, val, 24, 28)
	uint32_t emccal;
#define LPC17XX_SCB_EMCCAL_CALVALUE(val) BSP_FLD32(val, 0, 7)
#define LPC17XX_SCB_EMCCAL_CALVALUE_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define LPC17XX_SCB_EMCCAL_CALVALUE_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
#define LPC17XX_SCB_EMCCAL_START BSP_BIT32(14)
#define LPC17XX_SCB_EMCCAL_DONE BSP_BIT32(15)
} lpc17xx_scb;

#define LPC17XX_SCB (*(volatile lpc17xx_scb *) (LPC17XX_BASE + 0x400fc000))

typedef struct {
	uint32_t reserved_00 [268693504];
	lpc17xx_scb scb;
} lpc17xx;

#define LPC17XX (*(volatile lpc17xx *) (LPC17XX_BASE))

#endif /* LPC17XX_REGS_H */
