/**
 * @file
 *
 * @ingroup lpc32xx_reg
 *
 * @brief Register base addresses.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http:
 */

#ifndef LIBBSP_ARM_LPC32XX_LPC32XX_H
#define LIBBSP_ARM_LPC32XX_LPC32XX_H

#include <stdint.h>

#include <bsp/utility.h>

/**
 * @defgroup lpc32xx_reg Register Definitions
 *
 * @ingroup lpc32xx
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

#define LPC32XX_BASE_ADC 0x40048000
#define LPC32XX_BASE_SYSCON 0x40004000
#define LPC32XX_BASE_DEBUG_CTRL 0x40040000
#define LPC32XX_BASE_DMA 0x31000000
#define LPC32XX_BASE_EMC 0x31080000
#define LPC32XX_BASE_EMC_CS_0 0xe0000000
#define LPC32XX_BASE_EMC_CS_1 0xe1000000
#define LPC32XX_BASE_EMC_CS_2 0xe2000000
#define LPC32XX_BASE_EMC_CS_3 0xe3000000
#define LPC32XX_BASE_EMC_DYCS_0 0x80000000
#define LPC32XX_BASE_EMC_DYCS_1 0xa0000000
#define LPC32XX_BASE_ETB_CFG 0x310c0000
#define LPC32XX_BASE_ETB_DATA 0x310e0000
#define LPC32XX_BASE_ETHERNET 0x31060000
#define LPC32XX_BASE_GPIO 0x40028000
#define LPC32XX_BASE_I2C_1 0x400a0000
#define LPC32XX_BASE_I2C_2 0x400a8000
#define LPC32XX_BASE_I2S_0 0x20094000
#define LPC32XX_BASE_I2S_1 0x2009c000
#define LPC32XX_BASE_IRAM 0x08000000
#define LPC32XX_BASE_IROM 0x0c000000
#define LPC32XX_BASE_KEYSCAN 0x40050000
#define LPC32XX_BASE_LCD 0x31040000
#define LPC32XX_BASE_MCPWM 0x400e8000
#define LPC32XX_BASE_MIC 0x40008000
#define LPC32XX_BASE_NAND_MLC 0x200a8000
#define LPC32XX_BASE_NAND_SLC 0x20020000
#define LPC32XX_BASE_PWM_1 0x4005c000
#define LPC32XX_BASE_PWM_2 0x4005c004
#define LPC32XX_BASE_PWM_3 0x4002c000
#define LPC32XX_BASE_PWM_4 0x40030000
#define LPC32XX_BASE_RTC 0x40024000
#define LPC32XX_BASE_RTC_RAM 0x40024080
#define LPC32XX_BASE_SDCARD 0x20098000
#define LPC32XX_BASE_SIC_1 0x4000c000
#define LPC32XX_BASE_SIC_2 0x40010000
#define LPC32XX_BASE_SPI_1 0x20088000
#define LPC32XX_BASE_SPI_2 0x20090000
#define LPC32XX_BASE_SSP_0 0x20084000
#define LPC32XX_BASE_SSP_1 0x2008c000
#define LPC32XX_BASE_TIMER_0 0x40044000
#define LPC32XX_BASE_TIMER_1 0x4004c000
#define LPC32XX_BASE_TIMER_2 0x40058000
#define LPC32XX_BASE_TIMER_3 0x40060000
#define LPC32XX_BASE_TIMER_5 0x4002c000
#define LPC32XX_BASE_TIMER_6 0x40030000
#define LPC32XX_BASE_TIMER_HS 0x40038000
#define LPC32XX_BASE_TIMER_MS 0x40034000
#define LPC32XX_BASE_UART_1 0x40014000
#define LPC32XX_BASE_UART_2 0x40018000
#define LPC32XX_BASE_UART_3 0x40080000
#define LPC32XX_BASE_UART_4 0x40088000
#define LPC32XX_BASE_UART_5 0x40090000
#define LPC32XX_BASE_UART_6 0x40098000
#define LPC32XX_BASE_UART_7 0x4001c000
#define LPC32XX_BASE_USB 0x31020000
#define LPC32XX_BASE_USB_OTG_I2C 0x31020300
#define LPC32XX_BASE_WDT 0x4003c000

/** @} */

/**
 * @name Miscanellanous Registers
 *
 * @{
 */

#define LPC32XX_U3CLK (*(volatile uint32_t *) 0x400040d0)
#define LPC32XX_U4CLK (*(volatile uint32_t *) 0x400040d4)
#define LPC32XX_U5CLK (*(volatile uint32_t *) 0x400040d8)
#define LPC32XX_U6CLK (*(volatile uint32_t *) 0x400040dc)
#define LPC32XX_IRDACLK (*(volatile uint32_t *) 0x400040e0)
#define LPC32XX_UART_CTRL (*(volatile uint32_t *) 0x40054000)
#define LPC32XX_UART_CLKMODE (*(volatile uint32_t *) 0x40054004)
#define LPC32XX_UART_LOOP (*(volatile uint32_t *) 0x40054008)
#define LPC32XX_SW_INT (*(volatile uint32_t *) 0x400040a8)
#define LPC32XX_MAC_CLK_CTRL (*(volatile uint32_t *) 0x40004090)
#define LPC32XX_USB_DIV (*(volatile uint32_t *) 0x4000401c)
#define LPC32XX_OTG_CLK_CTRL (*(volatile uint32_t *) 0x31020ff4)
#define LPC32XX_OTG_CLK_STAT (*(volatile uint32_t *) 0x31020ff8)
#define LPC32XX_OTG_STAT_CTRL (*(volatile uint32_t *) 0x31020110)
#define LPC32XX_I2C_RX (*(volatile uint32_t *) 0x31020300)
#define LPC32XX_I2C_TX (*(volatile uint32_t *) 0x31020300)
#define LPC32XX_I2C_STS (*(volatile uint32_t *) 0x31020304)
#define LPC32XX_I2C_CTL (*(volatile uint32_t *) 0x31020308)
#define LPC32XX_I2C_CLKHI (*(volatile uint32_t *) 0x3102030c)
#define LPC32XX_I2C_CLKLO (*(volatile uint32_t *) 0x31020310)
#define LPC32XX_PWR_CTRL (*(volatile uint32_t *) 0x40004044)
#define LPC32XX_OSC_CTRL (*(volatile uint32_t *) 0x4000404c)
#define LPC32XX_SYSCLK_CTRL (*(volatile uint32_t *) 0x40004050)
#define LPC32XX_PLL397_CTRL (*(volatile uint32_t *) 0x40004048)
#define LPC32XX_HCLKPLL_CTRL (*(volatile uint32_t *) 0x40004058)
#define LPC32XX_HCLKDIV_CTRL (*(volatile uint32_t *) 0x40004040)
#define LPC32XX_TEST_CLK (*(volatile uint32_t *) 0x400040a4)
#define LPC32XX_AUTOCLK_CTRL (*(volatile uint32_t *) 0x400040ec)
#define LPC32XX_START_ER_PIN (*(volatile uint32_t *) 0x40004030)
#define LPC32XX_START_ER_INT (*(volatile uint32_t *) 0x40004020)
#define LPC32XX_P0_INTR_ER (*(volatile uint32_t *) 0x40004018)
#define LPC32XX_START_SR_PIN (*(volatile uint32_t *) 0x40004038)
#define LPC32XX_START_SR_INT (*(volatile uint32_t *) 0x40004028)
#define LPC32XX_START_RSR_PIN (*(volatile uint32_t *) 0x40004034)
#define LPC32XX_START_RSR_INT (*(volatile uint32_t *) 0x40004024)
#define LPC32XX_START_APR_PIN (*(volatile uint32_t *) 0x4000403c)
#define LPC32XX_START_APR_INT (*(volatile uint32_t *) 0x4000402c)
#define LPC32XX_USB_CTRL (*(volatile uint32_t *) 0x40004064)
#define LPC32XX_USBDIV_CTRL (*(volatile uint32_t *) 0x4000401c)
#define LPC32XX_MS_CTRL (*(volatile uint32_t *) 0x40004080)
#define LPC32XX_DMACLK_CTRL (*(volatile uint32_t *) 0x400040e8)
#define LPC32XX_FLASHCLK_CTRL (*(volatile uint32_t *) 0x400040c8)
#define LPC32XX_MACCLK_CTRL (*(volatile uint32_t *) 0x40004090)
#define LPC32XX_LCDCLK_CTRL (*(volatile uint32_t *) 0x40004054)
#define LPC32XX_I2S_CTRL (*(volatile uint32_t *) 0x4000407c)
#define LPC32XX_SSP_CTRL (*(volatile uint32_t *) 0x40004078)
#define LPC32XX_SPI_CTRL (*(volatile uint32_t *) 0x400040c4)
#define LPC32XX_I2CCLK_CTRL (*(volatile uint32_t *) 0x400040ac)
#define LPC32XX_TIMCLK_CTRL1 (*(volatile uint32_t *) 0x400040c0)
#define LPC32XX_TIMCLK_CTRL (*(volatile uint32_t *) 0x400040bc)
#define LPC32XX_ADCLK_CTRL (*(volatile uint32_t *) 0x400040b4)
#define LPC32XX_ADCLK_CTRL1 (*(volatile uint32_t *) 0x40004060)
#define LPC32XX_KEYCLK_CTRL (*(volatile uint32_t *) 0x400040b0)
#define LPC32XX_PWMCLK_CTRL (*(volatile uint32_t *) 0x400040b8)
#define LPC32XX_UARTCLK_CTRL (*(volatile uint32_t *) 0x400040e4)
#define LPC32XX_POS0_IRAM_CTRl (*(volatile uint32_t *) 0x40004110)
#define LPC32XX_POS1_IRAM_CTRl (*(volatile uint32_t *) 0x40004114)

/** @} */

/**
 * @name GPIO Registers
 *
 * @{
 */

#define LPC32XX_P0_INP_STATE (*(volatile uint32_t *) 0x40028040)
#define LPC32XX_P0_OUTP_SET (*(volatile uint32_t *) 0x40028044)
#define LPC32XX_P0_OUTP_CLR (*(volatile uint32_t *) 0x40028048)
#define LPC32XX_P0_DIR_SET (*(volatile uint32_t *) 0x40028050)
#define LPC32XX_P0_DIR_CLR (*(volatile uint32_t *) 0x40028054)
#define LPC32XX_P0_DIR_STATE (*(volatile uint32_t *) 0x40028058)
#define LPC32XX_P0_OUTP_STATE (*(volatile uint32_t *) 0x4002804c)
#define LPC32XX_P1_INP_STATE (*(volatile uint32_t *) 0x40028060)
#define LPC32XX_P1_OUTP_SET (*(volatile uint32_t *) 0x40028064)
#define LPC32XX_P1_OUTP_CLR (*(volatile uint32_t *) 0x40028068)
#define LPC32XX_P1_DIR_SET (*(volatile uint32_t *) 0x40028070)
#define LPC32XX_P1_DIR_CLR (*(volatile uint32_t *) 0x40028074)
#define LPC32XX_P1_DIR_STATE (*(volatile uint32_t *) 0x40028078)
#define LPC32XX_P1_OUTP_STATE (*(volatile uint32_t *) 0x4002806c)
#define LPC32XX_P2_INP_STATE (*(volatile uint32_t *) 0x4002801c)
#define LPC32XX_P2_OUTP_SET (*(volatile uint32_t *) 0x40028020)
#define LPC32XX_P2_OUTP_CLR (*(volatile uint32_t *) 0x40028024)
#define LPC32XX_P2_DIR_SET (*(volatile uint32_t *) 0x40028010)
#define LPC32XX_P2_DIR_CLR (*(volatile uint32_t *) 0x40028014)
#define LPC32XX_P2_DIR_STATE (*(volatile uint32_t *) 0x40028018)
#define LPC32XX_P3_INP_STATE (*(volatile uint32_t *) 0x40028000)
#define LPC32XX_P3_OUTP_SET (*(volatile uint32_t *) 0x40028004)
#define LPC32XX_P3_OUTP_CLR (*(volatile uint32_t *) 0x40028008)
#define LPC32XX_P3_OUTP_STATE (*(volatile uint32_t *) 0x4002800c)

/** @} */

/**
 * @name Power Control Register (PWR_CTRL)
 *
 * @{
 */

#define PWR_STOP BIT32(0)
#define PWR_HIGHCORE_ALWAYS BIT32(1)
#define PWR_NORMAL_RUN_MODE BIT32(2)
#define PWR_SYSCLKEN_ALWAYS BIT32(3)
#define PWR_SYSCLKEN_HIGH BIT32(4)
#define PWR_HIGHCORE_HIGH BIT32(5)
#define PWR_SDRAM_AUTO_REFRESH BIT32(7)
#define PWR_UPDATE_EMCSREFREQ BIT32(8)
#define PWR_EMCSREFREQ BIT32(9)
#define PWR_HCLK_USES_PERIPH_CLK BIT32(10)

/** @} */

/**
 * @name HCLK PLL Control Register (HCLKPLL_CTRL)
 *
 * @{
 */

#define HCLK_PLL_LOCK BIT32(0)
#define HCLK_PLL_M(val) FIELD32(val, 1, 8)
#define HCLK_PLL_N(val) FIELD32(val, 9, 10)
#define HCLK_PLL_P(val) FIELD32(val, 11, 12)
#define HCLK_PLL_FBD_FCLKOUT BIT32(13)
#define HCLK_PLL_DIRECT BIT32(14)
#define HCLK_PLL_BYPASS BIT32(15)
#define HCLK_PLL_POWER BIT32(16)

/** @} */

/**
 * @name HCLK Divider Control Register (HCLKDIV_CTRL)
 *
 * @{
 */

#define HCLK_DIV_HCLK(val) FIELD32(val, 0, 1)
#define HCLK_DIV_PERIPH_CLK(val) FIELD32(val, 2, 6)
#define HCLK_DIV_DDRAM_CLK(val) FIELD32(val, 7, 8)

/** @} */

/** @} */

#endif /* LIBBSP_ARM_LPC32XX_LPC32XX_H */
