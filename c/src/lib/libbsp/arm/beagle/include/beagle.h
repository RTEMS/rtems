/**
 * @file
 *
 * @ingroup beagle_reg
 *
 * @brief Register base addresses.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_BEAGLE_H
#define LIBBSP_ARM_BEAGLE_BEAGLE_H

#include <stdint.h>

#include <bsp/beagle-timer.h>

#define __arch_getb(a)      (*(volatile unsigned char *)(a))
#define __arch_getw(a)      (*(volatile unsigned short *)(a))
#define __arch_getl(a)      (*(volatile unsigned int *)(a))

#define __arch_putb(v,a)    (*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)    (*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)    (*(volatile unsigned int *)(a) = (v))
/*
 * TODO: do we need a barrier here?
 */

#define writeb(v,c) ({ unsigned char  __v = v; __arch_putb(__v,c); __v; })
#define writew(v,c) ({ unsigned short __v = v; __arch_putw(__v,c); __v; })
#define writel(v,c) ({ unsigned int __v = v; __arch_putl(__v,c); __v; })

#define readb(c)  ({ unsigned char  __v = __arch_getb(c); __v; })
#define readw(c)  ({ unsigned short __v = __arch_getw(c); __v; })
#define readl(c)  ({ unsigned int __v = __arch_getl(c); __v; })

#define SYSTEM_CLOCK_12       12000000
#define SYSTEM_CLOCK_13       13000000
#define SYSTEM_CLOCK_192      19200000
#define SYSTEM_CLOCK_96       96000000

#define OMAP34XX_CORE_L4_IO_BASE  0x48000000

#define BEAGLE_BASE_UART_1 0x4806A000
#define BEAGLE_BASE_UART_2 0x4806C000
#define BEAGLE_BASE_UART_3 0x49020000
#define BEAGLE_BASE_UART_4 0x49020000
#define BEAGLE_BASE_UART_5 0x49020000
#define BEAGLE_BASE_UART_5 0x49020000
#define BEAGLE_BASE_UART_6 0x49020000
#define BEAGLE_BASE_UART_7 0x49020000

#define BEAGLE_UART_DLL_REG_OFFSET 0x000
#define BEAGLE_UART_RHR_REG_OFFSET 0x000
#define BEAGLE_UART_THR_REG_OFFSET 0x000
#define BEAGLE_UART_DLH_REG_OFFSET 0x004
#define BEAGLE_UART_IER_REG_OFFSET 0x004
#define BEAGLE_UART_IIR_REG_OFFSET 0x008
#define BEAGLE_UART_FCR_REG_OFFSET 0x008
#define BEAGLE_UART_EFR_REG_OFFSET 0x008
#define BEAGLE_UART_LCR_REG_OFFSET 0x00C
#define BEAGLE_UART_MCR_REG_OFFSET 0x010
#define BEAGLE_UART_XON1_ADDR1_REG_OFFSET 0x010
#define BEAGLE_UART_LSR_REG_OFFSET 0x014
#define BEAGLE_UART_XON2_ADDR2_REG_OFFSET 0x014
#define BEAGLE_UART_MSR_REG_OFFSET 0x018
#define BEAGLE_UART_TCR_REG_OFFSET 0x018
#define BEAGLE_UART_XOFF1_REG_OFFSET 0x018
#define BEAGLE_UART_SPR_REG_OFFSET 0x01C
#define BEAGLE_UART_TLR_REG_OFFSET 0x01C
#define BEAGLE_UART_XOFF2_REG_OFFSET 0x01C
#define BEAGLE_UART_MDR1_REG_OFFSET 0x020
#define BEAGLE_UART_MDR2_REG_OFFSET 0x024
#define BEAGLE_UART_SFLSR_REG_OFFSET 0x028
#define BEAGLE_UART_TXFLL_REG_OFFSET 0x028
#define BEAGLE_UART_RESUME_REG_OFFSET 0x02C
#define BEAGLE_UART_TXFLH_REG_OFFSET 0x02C
#define BEAGLE_UART_SFREGL_REG_OFFSET 0x030
#define BEAGLE_UART_RXFLL_REG_OFFSET 0x030
#define BEAGLE_UART_SFREGH_REG_OFFSET 0x034
#define BEAGLE_UART_RXFLH_REG_OFFSET 0x034
#define BEAGLE_UART_UASR_REG_OFFSET 0x038
#define BEAGLE_UART_BLR_REG_OFFSET 0x038
#define BEAGLE_UART_ACREG_REG_OFFSET 0x03C
#define BEAGLE_UART_SCR_REG_OFFSET 0x040
#define BEAGLE_UART_SSR_REG_OFFSET 0x044
#define BEAGLE_UART_EBLR_REG_OFFSET 0x048
#define BEAGLE_UART_MVR_REG_OFFSET 0x050
#define BEAGLE_UART_SYSC_REG_OFFSET 0x054
#define BEAGLE_UART_SYSS_REG_OFFSET 0x058
#define BEAGLE_UART_WER_REG_OFFSET 0x05C
#define BEAGLE_UART_CFPS_REG_OFFSET 0x060

#define BEAGLE_UART5_DLL = BAGLE_BASE_UART5 + BEAGLE_UART_DLL_REG_OFFSET
#define BEAGLE_UART5_RHR = BAGLE_BASE_UART5 + \
  BEAGLE_UART_BEAGLE_UART_RHR_REG_OFFSET
#define BEAGLE_UART5_THR = BAGLE_BASE_UART5 + BEAGLE_UART_THR_REG_OFFSET
#define BEAGLE_UART5_DLH = BAGLE_BASE_UART5 + BEAGLE_UART_DLH_REG_OFFSET
#define BEAGLE_UART5_IER = BAGLE_BASE_UART5 + BEAGLE_UART_IER_REG_OFFSET
#define BEAGLE_UART5_IIR = BAGLE_BASE_UART5 + BEAGLE_UART_IIR_REG_OFFSET
#define BEAGLE_UART5_FCR = BAGLE_BASE_UART5 + BEAGLE_UART_FCR_REG_OFFSET
#define BEAGLE_UART5_EFR = BAGLE_BASE_UART5 + BEAGLE_UART_EFR_REG_OFFSET
#define BEAGLE_UART5_LCR = BAGLE_BASE_UART5 + BEAGLE_UART_LCR_REG_OFFSET
#define BEAGLE_UART5_MCR = BAGLE_BASE_UART5 + BEAGLE_UART_MCR_REG_OFFSET
#define BEAGLE_UART5_XON1_ADDR1 = BAGLE_BASE_UART5 + \
  BEAGLE_UART_XON1_ADDR1_REG_OFFSET
#define BEAGLE_UART5_LSR = BAGLE_BASE_UART5 + BEAGLE_UART_LSR_REG_OFFSET
#define BEAGLE_UART5_XON2_ADDR2 = BAGLE_BASE_UART5 + \
  BEAGLE_UART_XON2_ADDR2_REG_OFFSET
#define BEAGLE_UART5_MSR = BAGLE_BASE_UART5 + BEAGLE_UART_MSR_REG_OFFSET
#define BEAGLE_UART5_TCR = BAGLE_BASE_UART5 + BEAGLE_UART_TCR_REG_OFFSET
#define BEAGLE_UART5_XOFF1 = BAGLE_BASE_UART5 + BEAGLE_UART_XOFF1_REG_OFFSET
#define BEAGLE_UART5_SPR = BAGLE_BASE_UART5 + BEAGLE_UART_SPR_REG_OFFSET
#define BEAGLE_UART5_TLR = BAGLE_BASE_UART5 + BEAGLE_UART_TLR_REG_OFFSET
#define BEAGLE_UART5_XOFF2 = BAGLE_BASE_UART5 + BEAGLE_UART_XOFF2_REG_OFFSET
#define BEAGLE_UART5_MDR1 = BAGLE_BASE_UART5 + BEAGLE_UART_MDR1_REG_OFFSET
#define BEAGLE_UART5_MDR2 = BAGLE_BASE_UART5 + BEAGLE_UART_MDR2_REG_OFFSET
#define BEAGLE_UART5_SFLSR = BAGLE_BASE_UART5 + BEAGLE_UART_SFLSR_REG_OFFSET
#define BEAGLE_UART5_TXFLL = BAGLE_BASE_UART5 + BEAGLE_UART_TXFLL_REG_OFFSET
#define BEAGLE_UART5_RESUME = BAGLE_BASE_UART5 + BEAGLE_UART_RESUME_REG_OFFSET
#define BEAGLE_UART5_TXFLH = BAGLE_BASE_UART5 + BEAGLE_UART_TXFLH_REG_OFFSET
#define BEAGLE_UART5_SFREGL = BAGLE_BASE_UART5 + BEAGLE_UART_SFREGL_REG_OFFSET
#define BEAGLE_UART5_RXFLL = BAGLE_BASE_UART5 + BEAGLE_UART_RXFLL_REG_OFFSET
#define BEAGLE_UART5_SFREGH = BAGLE_BASE_UART5 + BEAGLE_UART_SFREGH_REG_OFFSET
#define BEAGLE_UART5_RXFLH = BAGLE_BASE_UART5 + BEAGLE_UART_RXFLH_REG_OFFSET
#define BEAGLE_UART5_UASR = BAGLE_BASE_UART5 + BEAGLE_UART_UASR_REG_OFFSET
#define BEAGLE_UART5_BLR = BAGLE_BASE_UART5 + BEAGLE_UART_BLR_REG_OFFSET
#define BEAGLE_UART5_ACREG = BAGLE_BASE_UART5 + BEAGLE_UART_ACREG_REG_OFFSET
#define BEAGLE_UART5_SCR = BAGLE_BASE_UART5 + BEAGLE_UART_SCR_REG_OFFSET
#define BEAGLE_UART5_SSR = BAGLE_BASE_UART5 + BEAGLE_UART_SSR_REG_OFFSET
#define BEAGLE_UART5_EBLR = BAGLE_BASE_UART5 + BEAGLE_UART_EBLR_REG_OFFSET
#define BEAGLE_UART5_MVR = BAGLE_BASE_UART5 + BEAGLE_UART_MVR_REG_OFFSET
#define BEAGLE_UART5_SYSC = BAGLE_BASE_UART5 + BEAGLE_UART_SYSC_REG_OFFSET
#define BEAGLE_UART5_SYSS = BAGLE_BASE_UART5 + BEAGLE_UART_SYSS_REG_OFFSET
#define BEAGLE_UART5_WER = BAGLE_BASE_UART5 + BEAGLE_UART_WER_REG_OFFSET
#define BEAGLE_UART5_CFPS = BAGLE_BASE_UART5 + BEAGLE_UART_CFPS_REG_OFFSET

/**
 * @defgroup beagle_reg Register Definitions
 *
 * @ingroup beagle
 *
 * @brief Register definitions.
 *
 * @{
 */

/**
 * @name Register Base Addresses
 *
 * @{
 */

#define BEAGLE_BASE_ADC 0x40048000
#define BEAGLE_BASE_SYSCON 0x40004000
#define BEAGLE_BASE_DEBUG_CTRL 0x40040000
#define BEAGLE_BASE_DMA 0x31000000
#define BEAGLE_BASE_EMC 0x31080000
#define BEAGLE_BASE_EMC_CS_0 0xe0000000
#define BEAGLE_BASE_EMC_CS_1 0xe1000000
#define BEAGLE_BASE_EMC_CS_2 0xe2000000
#define BEAGLE_BASE_EMC_CS_3 0xe3000000
#define BEAGLE_BASE_EMC_DYCS_0 0x80000000
#define BEAGLE_BASE_EMC_DYCS_1 0xa0000000
#define BEAGLE_BASE_ETB_CFG 0x310c0000
#define BEAGLE_BASE_ETB_DATA 0x310e0000
#define BEAGLE_BASE_ETHERNET 0x31060000
#define BEAGLE_BASE_GPIO 0x40028000
#define BEAGLE_BASE_I2C_1 0x400a0000
#define BEAGLE_BASE_I2C_2 0x400a8000
#define BEAGLE_BASE_I2S_0 0x20094000
#define BEAGLE_BASE_I2S_1 0x2009c000
#define BEAGLE_BASE_IRAM 0x08000000
#define BEAGLE_BASE_IROM 0x0c000000
#define BEAGLE_BASE_KEYSCAN 0x40050000
#define BEAGLE_BASE_LCD 0x31040000
#define BEAGLE_BASE_MCPWM 0x400e8000
#define BEAGLE_BASE_MIC 0x40008000
#define BEAGLE_BASE_NAND_MLC 0x200a8000
#define BEAGLE_BASE_NAND_SLC 0x20020000
#define BEAGLE_BASE_PWM_1 0x4005c000
#define BEAGLE_BASE_PWM_2 0x4005c004
#define BEAGLE_BASE_PWM_3 0x4002c000
#define BEAGLE_BASE_PWM_4 0x40030000
#define BEAGLE_BASE_RTC 0x40024000
#define BEAGLE_BASE_RTC_RAM 0x40024080
#define BEAGLE_BASE_SDCARD 0x20098000
#define BEAGLE_BASE_SIC_1 0x4000c000
#define BEAGLE_BASE_SIC_2 0x40010000
#define BEAGLE_BASE_SPI_1 0x20088000
#define BEAGLE_BASE_SPI_2 0x20090000
#define BEAGLE_BASE_SSP_0 0x20084000
#define BEAGLE_BASE_SSP_1 0x2008c000
#define BEAGLE_BASE_TIMER_0 0x40044000
#define BEAGLE_BASE_TIMER_1 0x4004c000
#define BEAGLE_BASE_TIMER_2 0x40058000
#define BEAGLE_BASE_TIMER_3 0x40060000
#define BEAGLE_BASE_TIMER_5 0x4002c000
#define BEAGLE_BASE_TIMER_6 0x40030000
#define BEAGLE_BASE_TIMER_HS 0x40038000
#define BEAGLE_BASE_TIMER_MS 0x40034000

//#define BEAGLE_BASE_UART_1 0x40014000
//#define BEAGLE_BASE_UART_2 0x40018000
//#define BEAGLE_BASE_UART_3 0x40080000
//#define BEAGLE_BASE_UART_4 0x40088000
//#define BEAGLE_BASE_UART_5 0x40090000
//#define BEAGLE_BASE_UART_5 0x49020000
//#define BEAGLE_BASE_UART_6 0x40098000
//#define BEAGLE_BASE_UART_7 0x4001c000

#define BEAGLE_BASE_USB 0x31020000
#define BEAGLE_BASE_USB_OTG_I2C 0x31020300
#define BEAGLE_BASE_WDT 0x4003c000

/** @} */

/**
 * @name Miscanellanous Registers
 *
 * @{
 */

#define BEAGLE_U3CLK (*(volatile uint32_t *) 0x400040d0)
#define BEAGLE_U4CLK (*(volatile uint32_t *) 0x400040d4)
#define BEAGLE_U5CLK (*(volatile uint32_t *) 0x400040d8)
#define BEAGLE_U6CLK (*(volatile uint32_t *) 0x400040dc)
#define BEAGLE_IRDACLK (*(volatile uint32_t *) 0x400040e0)
#define BEAGLE_UART_CTRL (*(volatile uint32_t *) 0x40054000)
#define BEAGLE_UART_CLKMODE (*(volatile uint32_t *) 0x40054004)
#define BEAGLE_UART_LOOP (*(volatile uint32_t *) 0x40054008)

#define BEAGLE_SW_INT (*(volatile uint32_t *) 0x400040a8)
#define BEAGLE_MAC_CLK_CTRL (*(volatile uint32_t *) 0x40004090)
#define BEAGLE_USB_DIV (*(volatile uint32_t *) 0x4000401c)
#define BEAGLE_OTG_CLK_CTRL (*(volatile uint32_t *) 0x31020ff4)
#define BEAGLE_OTG_CLK_STAT (*(volatile uint32_t *) 0x31020ff8)
#define BEAGLE_OTG_STAT_CTRL (*(volatile uint32_t *) 0x31020110)
#define BEAGLE_I2C_RX (*(volatile uint32_t *) 0x31020300)
#define BEAGLE_I2C_TX (*(volatile uint32_t *) 0x31020300)
#define BEAGLE_I2C_STS (*(volatile uint32_t *) 0x31020304)
#define BEAGLE_I2C_CTL (*(volatile uint32_t *) 0x31020308)
#define BEAGLE_I2C_CLKHI (*(volatile uint32_t *) 0x3102030c)
#define BEAGLE_I2C_CLKLO (*(volatile uint32_t *) 0x31020310)
#define BEAGLE_PWR_CTRL (*(volatile uint32_t *) 0x40004044)
#define BEAGLE_OSC_CTRL (*(volatile uint32_t *) 0x4000404c)
#define BEAGLE_SYSCLK_CTRL (*(volatile uint32_t *) 0x40004050)
#define BEAGLE_PLL397_CTRL (*(volatile uint32_t *) 0x40004048)
#define BEAGLE_HCLKPLL_CTRL (*(volatile uint32_t *) 0x40004058)
#define BEAGLE_HCLKDIV_CTRL (*(volatile uint32_t *) 0x40004040)
#define BEAGLE_TEST_CLK (*(volatile uint32_t *) 0x400040a4)
#define BEAGLE_AUTOCLK_CTRL (*(volatile uint32_t *) 0x400040ec)
#define BEAGLE_START_ER_PIN (*(volatile uint32_t *) 0x40004030)
#define BEAGLE_START_ER_INT (*(volatile uint32_t *) 0x40004020)
#define BEAGLE_P0_INTR_ER (*(volatile uint32_t *) 0x40004018)
#define BEAGLE_START_SR_PIN (*(volatile uint32_t *) 0x40004038)
#define BEAGLE_START_SR_INT (*(volatile uint32_t *) 0x40004028)
#define BEAGLE_START_RSR_PIN (*(volatile uint32_t *) 0x40004034)
#define BEAGLE_START_RSR_INT (*(volatile uint32_t *) 0x40004024)
#define BEAGLE_START_APR_PIN (*(volatile uint32_t *) 0x4000403c)
#define BEAGLE_START_APR_INT (*(volatile uint32_t *) 0x4000402c)
#define BEAGLE_USB_CTRL (*(volatile uint32_t *) 0x40004064)
#define BEAGLE_USBDIV_CTRL (*(volatile uint32_t *) 0x4000401c)
#define BEAGLE_MS_CTRL (*(volatile uint32_t *) 0x40004080)
#define BEAGLE_DMACLK_CTRL (*(volatile uint32_t *) 0x400040e8)
#define BEAGLE_FLASHCLK_CTRL (*(volatile uint32_t *) 0x400040c8)
#define BEAGLE_MACCLK_CTRL (*(volatile uint32_t *) 0x40004090)
#define BEAGLE_LCDCLK_CTRL (*(volatile uint32_t *) 0x40004054)
#define BEAGLE_I2S_CTRL (*(volatile uint32_t *) 0x4000407c)
#define BEAGLE_SSP_CTRL (*(volatile uint32_t *) 0x40004078)
#define BEAGLE_SPI_CTRL (*(volatile uint32_t *) 0x400040c4)
#define BEAGLE_I2CCLK_CTRL (*(volatile uint32_t *) 0x400040ac)
#define BEAGLE_TIMCLK_CTRL1 (*(volatile uint32_t *) 0x400040c0)
#define BEAGLE_TIMCLK_CTRL (*(volatile uint32_t *) 0x400040bc)
#define BEAGLE_ADCLK_CTRL (*(volatile uint32_t *) 0x400040b4)
#define BEAGLE_ADCLK_CTRL1 (*(volatile uint32_t *) 0x40004060)
#define BEAGLE_KEYCLK_CTRL (*(volatile uint32_t *) 0x400040b0)
#define BEAGLE_PWMCLK_CTRL (*(volatile uint32_t *) 0x400040b8)
#define BEAGLE_UARTCLK_CTRL (*(volatile uint32_t *) 0x400040e4)
#define BEAGLE_POS0_IRAM_CTRL (*(volatile uint32_t *) 0x40004110)
#define BEAGLE_POS1_IRAM_CTRL (*(volatile uint32_t *) 0x40004114)
#define BEAGLE_SDRAMCLK_CTRL (*(volatile uint32_t *) 0x40004068)

/** @} */

/**
 * @name Power Control Register (PWR_CTRL)
 *
 * @{
 */

#define PWR_STOP BSP_BIT32(0)
#define PWR_HIGHCORE_ALWAYS BSP_BIT32(1)
#define PWR_NORMAL_RUN_MODE BSP_BIT32(2)
#define PWR_SYSCLKEN_ALWAYS BSP_BIT32(3)
#define PWR_SYSCLKEN_HIGH BSP_BIT32(4)
#define PWR_HIGHCORE_HIGH BSP_BIT32(5)
#define PWR_SDRAM_AUTO_REFRESH BSP_BIT32(7)
#define PWR_UPDATE_EMCSREFREQ BSP_BIT32(8)
#define PWR_EMCSREFREQ BSP_BIT32(9)
#define PWR_HCLK_USES_PERIPH_CLK BSP_BIT32(10)

/** @} */

/**
 * @name HCLK PLL Control Register (HCLKPLL_CTRL)
 *
 * @{
 */

#define HCLK_PLL_LOCK BSP_BIT32(0)
#define HCLK_PLL_M(val) BSP_FLD32(val, 1, 8)
#define HCLK_PLL_M_GET(reg) BSP_FLD32GET(reg, 1, 8)
#define HCLK_PLL_N(val) BSP_FLD32(val, 9, 10)
#define HCLK_PLL_N_GET(reg) BSP_FLD32GET(reg, 9, 10)
#define HCLK_PLL_P(val) BSP_FLD32(val, 11, 12)
#define HCLK_PLL_P_GET(reg) BSP_FLD32GET(reg, 11, 12)
#define HCLK_PLL_FBD_FCLKOUT BSP_BIT32(13)
#define HCLK_PLL_DIRECT BSP_BIT32(14)
#define HCLK_PLL_BYPASS BSP_BIT32(15)
#define HCLK_PLL_POWER BSP_BIT32(16)

/** @} */

/**
 * @name HCLK Divider Control Register (HCLKDIV_CTRL)
 *
 * @{
 */

#define HCLK_DIV_HCLK(val) BSP_FLD32(val, 0, 1)
#define HCLK_DIV_HCLK_GET(reg) BSP_FLD32GET(reg, 0, 1)
#define HCLK_DIV_PERIPH_CLK(val) BSP_FLD32(val, 2, 6)
#define HCLK_DIV_PERIPH_CLK_GET(reg) BSP_FLD32GET(reg, 2, 6)
#define HCLK_DIV_DDRAM_CLK(val) BSP_FLD32(val, 7, 8)
#define HCLK_DIV_DDRAM_CLK_GET(reg) BSP_FLD32GET(reg, 7, 8)

/** @} */

/**
 * @name Timer Clock Control Register (TIMCLK_CTRL)
 *
 * @{
 */

#define TIMCLK_CTRL_WDT BSP_BIT32(0)
#define TIMCLK_CTRL_HST BSP_BIT32(1)

/** @} */

#define BEAGLE_FILL(a, b, s) uint8_t reserved_ ## b [b - a - sizeof(s)]
#define BEAGLE_RESERVE(a, b) uint8_t reserved_ ## b [b - a]

typedef struct {
} beagle_nand_slc;

typedef struct {
} beagle_ssp;

typedef struct {
} beagle_spi;

typedef struct {
} beagle_sd_card;

typedef struct {
} beagle_usb;

typedef struct {
} beagle_lcd;

typedef struct {
} beagle_etb;

typedef struct {
} beagle_syscon;

typedef struct {
} beagle_uart_ctrl;

typedef struct {
} beagle_uart;

typedef struct {
} beagle_ms_timer;

typedef struct {
} beagle_hs_timer;

/**
 * @name Watchdog Timer Interrupt Status Register (WDTIM_INT)
 *
 * @{
 */

#define WDTTIM_INT_MATCH_INT BSP_BIT32(0)

/** @} */

/**
 * @name Watchdog Timer Control Register (WDTIM_CTRL)
 *
 * @{
 */

#define WDTTIM_CTRL_COUNT_ENAB BSP_BIT32(0)
#define WDTTIM_CTRL_RESET_COUNT BSP_BIT32(1)
#define WDTTIM_CTRL_PAUSE_EN BSP_BIT32(2)

/** @} */

/**
 * @name Watchdog Timer Match Control Register (WDTIM_MCTRL)
 *
 * @{
 */

#define WDTTIM_MCTRL_MR0_INT BSP_BIT32(0)
#define WDTTIM_MCTRL_RESET_COUNT0 BSP_BIT32(1)
#define WDTTIM_MCTRL_STOP_COUNT0 BSP_BIT32(2)
#define WDTTIM_MCTRL_M_RES1 BSP_BIT32(3)
#define WDTTIM_MCTRL_M_RES2 BSP_BIT32(4)
#define WDTTIM_MCTRL_RESFRC1 BSP_BIT32(5)
#define WDTTIM_MCTRL_RESFRC2 BSP_BIT32(6)

/** @} */

/**
 * @name Watchdog Timer External Match Control Register (WDTIM_EMR)
 *
 * @{
 */

#define WDTTIM_EMR_EXT_MATCH0 BSP_BIT32(0)
#define WDTTIM_EMR_MATCH_CTRL(val) BSP_FLD32(val, 4, 5)
#define WDTTIM_EMR_MATCH_CTRL_SET(reg, val) BSP_FLD32SET(reg, val, 4, 5)

/** @} */

/**
 * @name Watchdog Timer Reset Source Register (WDTIM_RES)
 *
 * @{
 */

#define WDTTIM_RES_WDT BSP_BIT32(0)

/** @} */

typedef struct {
  uint32_t intr;
  uint32_t ctrl;
  uint32_t counter;
  uint32_t mctrl;
  uint32_t match0;
  uint32_t emr;
  uint32_t pulse;
  uint32_t res;
} beagle_wdt;

typedef struct {
} beagle_debug;

typedef struct {
} beagle_adc;

typedef struct {
} beagle_keyscan;

typedef struct {
} beagle_pwm;

typedef struct {
} beagle_mcpwm;

typedef struct {
  uint32_t mac1;
  uint32_t mac2;
  uint32_t ipgt;
  uint32_t ipgr;
  uint32_t clrt;
  uint32_t maxf;
  uint32_t supp;
  uint32_t test;
  uint32_t mcfg;
  uint32_t mcmd;
  uint32_t madr;
  uint32_t mwtd;
  uint32_t mrdd;
  uint32_t mind;
  uint32_t reserved_0 [2];
  uint32_t sa0;
  uint32_t sa1;
  uint32_t sa2;
  uint32_t reserved_1 [45];
  uint32_t command;
  uint32_t status;
  uint32_t rxdescriptor;
  uint32_t rxstatus;
  uint32_t rxdescriptornum;
  uint32_t rxproduceindex;
  uint32_t rxconsumeindex;
  uint32_t txdescriptor;
  uint32_t txstatus;
  uint32_t txdescriptornum;
  uint32_t txproduceindex;
  uint32_t txconsumeindex;
  uint32_t reserved_2 [10];
  uint32_t tsv0;
  uint32_t tsv1;
  uint32_t rsv;
  uint32_t reserved_3 [3];
  uint32_t flowcontrolcnt;
  uint32_t flowcontrolsts;
  uint32_t reserved_4 [34];
  uint32_t rxfilterctrl;
  uint32_t rxfilterwolsts;
  uint32_t rxfilterwolclr;
  uint32_t reserved_5 [1];
  uint32_t hashfilterl;
  uint32_t hashfilterh;
  uint32_t reserved_6 [882];
  uint32_t intstatus;
  uint32_t intenable;
  uint32_t intclear;
  uint32_t intset;
  uint32_t reserved_7 [1];
  uint32_t powerdown;
} beagle_eth;

typedef struct {
  uint32_t er;
  uint32_t rsr;
  uint32_t sr;
  uint32_t apr;
  uint32_t atr;
  uint32_t itr;
} beagle_irq;

typedef struct {
  uint32_t p3_inp_state;
  uint32_t p3_outp_set;
  uint32_t p3_outp_clr;
  uint32_t p3_outp_state;
  uint32_t p2_dir_set;
  uint32_t p2_dir_clr;
  uint32_t p2_dir_state;
  uint32_t p2_inp_state;
  uint32_t p2_outp_set;
  uint32_t p2_outp_clr;
  uint32_t p2_mux_set;
  uint32_t p2_mux_clr;
  uint32_t p2_mux_state;
  BEAGLE_RESERVE(0x034, 0x040);
  uint32_t p0_inp_state;
  uint32_t p0_outp_set;
  uint32_t p0_outp_clr;
  uint32_t p0_outp_state;
  uint32_t p0_dir_set;
  uint32_t p0_dir_clr;
  uint32_t p0_dir_state;
  BEAGLE_RESERVE(0x05c, 0x060);
  uint32_t p1_inp_state;
  uint32_t p1_outp_set;
  uint32_t p1_outp_clr;
  uint32_t p1_outp_state;
  uint32_t p1_dir_set;
  uint32_t p1_dir_clr;
  uint32_t p1_dir_state;
  BEAGLE_RESERVE(0x07c, 0x110);
  uint32_t p3_mux_set;
  uint32_t p3_mux_clr;
  uint32_t p3_mux_state;
  uint32_t p0_mux_set;
  uint32_t p0_mux_clr;
  uint32_t p0_mux_state;
  uint32_t p1_mux_set;
  uint32_t p1_mux_clr;
  uint32_t p1_mux_state;
} beagle_gpio;

typedef struct {
  uint32_t rx_or_tx;
  uint32_t stat;
  uint32_t ctrl;
  uint32_t clk_hi;
  uint32_t clk_lo;
  uint32_t adr;
  uint32_t rxfl;
  uint32_t txfl;
  uint32_t rxb;
  uint32_t txb;
  uint32_t s_tx;
  uint32_t s_txfl;
} beagle_i2c;

typedef struct {
  uint32_t ucount;
  uint32_t dcount;
  uint32_t match0;
  uint32_t match1;
  uint32_t ctrl;
  uint32_t intstat;
  uint32_t key;
  uint32_t sram [32];
} beagle_rtc;

typedef struct {
  uint32_t control;
  uint32_t status;
  uint32_t timeout;
  uint32_t reserved_0 [5];
} beagle_emc_ahb;

typedef struct {
  union {
    uint32_t w32;
    uint16_t w16;
    uint8_t w8;
  } buff;
  uint32_t reserved_0 [8191];
  union {
    uint32_t w32;
    uint16_t w16;
    uint8_t w8;
  } data;
  uint32_t reserved_1 [8191];
  uint32_t cmd;
  uint32_t addr;
  uint32_t ecc_enc;
  uint32_t ecc_dec;
  uint32_t ecc_auto_enc;
  uint32_t ecc_auto_dec;
  uint32_t rpr;
  uint32_t wpr;
  uint32_t rubp;
  uint32_t robp;
  uint32_t sw_wp_add_low;
  uint32_t sw_wp_add_hig;
  uint32_t icr;
  uint32_t time;
  uint32_t irq_mr;
  uint32_t irq_sr;
  uint32_t reserved_2;
  uint32_t lock_pr;
  uint32_t isr;
  uint32_t ceh;
} beagle_nand_mlc;

typedef struct {
  beagle_nand_slc nand_slc;
  BEAGLE_FILL(0x20020000, 0x20084000, beagle_nand_slc);
  beagle_ssp ssp_0;
  BEAGLE_FILL(0x20084000, 0x20088000, beagle_ssp);
  beagle_spi spi_1;
  BEAGLE_FILL(0x20088000, 0x2008c000, beagle_spi);
  beagle_ssp ssp_1;
  BEAGLE_FILL(0x2008c000, 0x20090000, beagle_ssp);
  beagle_spi spi_2;
  BEAGLE_FILL(0x20090000, 0x20094000, beagle_spi);
  //beagle_i2s i2s_0;
  //BEAGLE_FILL(0x20094000, 0x20098000, beagle_i2s);
  beagle_sd_card sd_card;
  BEAGLE_FILL(0x20098000, 0x2009c000, beagle_sd_card);
  //beagle_i2s i2s_1;
  //BEAGLE_FILL(0x2009c000, 0x200a8000, beagle_i2s);
  beagle_nand_mlc nand_mlc;
  BEAGLE_FILL(0x200a8000, 0x31000000, beagle_nand_mlc);
  //beagle_dma dma;
  //BEAGLE_FILL(0x31000000, 0x31020000, beagle_dma);
  beagle_usb usb;
  BEAGLE_FILL(0x31020000, 0x31040000, beagle_usb);
  beagle_lcd lcd;
  BEAGLE_FILL(0x31040000, 0x31060000, beagle_lcd);
  beagle_eth eth;
  BEAGLE_FILL(0x31060000, 0x31080000, beagle_eth);
  //beagle_emc emc;
  //BEAGLE_FILL(0x31080000, 0x31080400, beagle_emc);
  beagle_emc_ahb emc_ahb [5];
  BEAGLE_FILL(0x31080400, 0x310c0000, beagle_emc_ahb [5]);
  beagle_etb etb;
  BEAGLE_FILL(0x310c0000, 0x40004000, beagle_etb);
  beagle_syscon syscon;
  BEAGLE_FILL(0x40004000, 0x40008000, beagle_syscon);
  beagle_irq mic;
  BEAGLE_FILL(0x40008000, 0x4000c000, beagle_irq);
  beagle_irq sic_1;
  BEAGLE_FILL(0x4000c000, 0x40010000, beagle_irq);
  beagle_irq sic_2;
  BEAGLE_FILL(0x40010000, 0x40014000, beagle_irq);
  beagle_uart uart_1;
  BEAGLE_FILL(0x40014000, 0x40018000, beagle_uart);
  beagle_uart uart_2;
  BEAGLE_FILL(0x40018000, 0x4001c000, beagle_uart);
  beagle_uart uart_7;
  BEAGLE_FILL(0x4001c000, 0x40024000, beagle_uart);
  beagle_rtc rtc;
  BEAGLE_FILL(0x40024000, 0x40028000, beagle_rtc);
  beagle_gpio gpio;
  BEAGLE_FILL(0x40028000, 0x4002c000, beagle_gpio);
  beagle_timer timer_4;
  BEAGLE_FILL(0x4002c000, 0x40030000, beagle_timer);
  beagle_timer timer_5;
  BEAGLE_FILL(0x40030000, 0x40034000, beagle_timer);
  beagle_ms_timer ms_timer;
  BEAGLE_FILL(0x40034000, 0x40038000, beagle_ms_timer);
  beagle_hs_timer hs_timer;
  BEAGLE_FILL(0x40038000, 0x4003c000, beagle_hs_timer);
  beagle_wdt wdt;
  BEAGLE_FILL(0x4003c000, 0x40040000, beagle_wdt);
  beagle_debug debug;
  BEAGLE_FILL(0x40040000, 0x40044000, beagle_debug);
  beagle_timer timer_0;
  BEAGLE_FILL(0x40044000, 0x40048000, beagle_timer);
  beagle_adc adc;
  BEAGLE_FILL(0x40048000, 0x4004c000, beagle_adc);
  beagle_timer timer_1;
  BEAGLE_FILL(0x4004c000, 0x40050000, beagle_timer);
  beagle_keyscan keyscan;
  BEAGLE_FILL(0x40050000, 0x40054000, beagle_keyscan);
  beagle_uart_ctrl uart_ctrl;
  BEAGLE_FILL(0x40054000, 0x40058000, beagle_uart_ctrl);
  beagle_timer timer_2;
  BEAGLE_FILL(0x40058000, 0x4005c000, beagle_timer);
  beagle_pwm pwm_1_and_pwm_2;
  BEAGLE_FILL(0x4005c000, 0x40060000, beagle_pwm);
  beagle_timer timer3;
  BEAGLE_FILL(0x40060000, 0x40080000, beagle_timer);
  beagle_uart uart_3;
  BEAGLE_FILL(0x40080000, 0x40088000, beagle_uart);
  beagle_uart uart_4;
  BEAGLE_FILL(0x40088000, 0x40090000, beagle_uart);
  beagle_uart uart_5;
  BEAGLE_FILL(0x40090000, 0x40098000, beagle_uart);
  beagle_uart uart_6;
  BEAGLE_FILL(0x40098000, 0x400a0000, beagle_uart);
  beagle_i2c i2c_1;
  BEAGLE_FILL(0x400a0000, 0x400a8000, beagle_i2c);
  beagle_i2c i2c_2;
  BEAGLE_FILL(0x400a8000, 0x400e8000, beagle_i2c);
  beagle_mcpwm mcpwm;
} beagle_registers;

extern volatile beagle_registers beagle;

/** @} */

#endif /* LIBBSP_ARM_BEAGLE_BEAGLE_H */
