/**
 * @file
 *
 * @ingroup lpc32xx_reg
 *
 * @brief Register base addresses.
 */

/*
 * Copyright (c) 2009, 2010
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
#include <bsp/lpc-timer.h>
#include <bsp/lpc-dma.h>
#include <bsp/lpc-i2s.h>
#include <bsp/lpc-emc.h>

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
#define LPC32XX_POS0_IRAM_CTRL (*(volatile uint32_t *) 0x40004110)
#define LPC32XX_POS1_IRAM_CTRL (*(volatile uint32_t *) 0x40004114)
#define LPC32XX_SDRAMCLK_CTRL (*(volatile uint32_t *) 0x40004068)

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

#define LPC32XX_FILL(a, b, s) uint8_t reserved_ ## b [b - a - sizeof(s)]
#define LPC32XX_RESERVE(a, b) uint8_t reserved_ ## b [b - a]

typedef struct {
} lpc32xx_nand_slc;

typedef struct {
} lpc32xx_ssp;

typedef struct {
} lpc32xx_spi;

typedef struct {
} lpc32xx_sd_card;

typedef struct {
} lpc32xx_usb;

typedef struct {
} lpc32xx_lcd;

typedef struct {
} lpc32xx_etb;

typedef struct {
} lpc32xx_syscon;

typedef struct {
} lpc32xx_uart_ctrl;

typedef struct {
} lpc32xx_uart;

typedef struct {
} lpc32xx_ms_timer;

typedef struct {
} lpc32xx_hs_timer;

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
} lpc32xx_wdt;

typedef struct {
} lpc32xx_debug;

typedef struct {
} lpc32xx_adc;

typedef struct {
} lpc32xx_keyscan;

typedef struct {
} lpc32xx_pwm;

typedef struct {
} lpc32xx_mcpwm;

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
} lpc32xx_eth;

typedef struct {
  uint32_t er;
  uint32_t rsr;
  uint32_t sr;
  uint32_t apr;
  uint32_t atr;
  uint32_t itr;
} lpc32xx_irq;

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
  LPC32XX_RESERVE(0x034, 0x040);
  uint32_t p0_inp_state;
  uint32_t p0_outp_set;
  uint32_t p0_outp_clr;
  uint32_t p0_outp_state;
  uint32_t p0_dir_set;
  uint32_t p0_dir_clr;
  uint32_t p0_dir_state;
  LPC32XX_RESERVE(0x05c, 0x060);
  uint32_t p1_inp_state;
  uint32_t p1_outp_set;
  uint32_t p1_outp_clr;
  uint32_t p1_outp_state;
  uint32_t p1_dir_set;
  uint32_t p1_dir_clr;
  uint32_t p1_dir_state;
  LPC32XX_RESERVE(0x07c, 0x110);
  uint32_t p3_mux_set;
  uint32_t p3_mux_clr;
  uint32_t p3_mux_state;
  uint32_t p0_mux_set;
  uint32_t p0_mux_clr;
  uint32_t p0_mux_state;
  uint32_t p1_mux_set;
  uint32_t p1_mux_clr;
  uint32_t p1_mux_state;
} lpc32xx_gpio;

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
} lpc32xx_i2c;

typedef struct {
  uint32_t ucount;
  uint32_t dcount;
  uint32_t match0;
  uint32_t match1;
  uint32_t ctrl;
  uint32_t intstat;
  uint32_t key;
  uint32_t sram [32];
} lpc32xx_rtc;

typedef struct {
  uint32_t control;
  uint32_t status;
  uint32_t timeout;
  uint32_t reserved_0 [5];
} lpc32xx_emc_ahb;

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
} lpc32xx_nand_mlc;

typedef struct {
  lpc32xx_nand_slc nand_slc;
  LPC32XX_FILL(0x20020000, 0x20084000, lpc32xx_nand_slc);
  lpc32xx_ssp ssp_0;
  LPC32XX_FILL(0x20084000, 0x20088000, lpc32xx_ssp); 
  lpc32xx_spi spi_1;
  LPC32XX_FILL(0x20088000, 0x2008c000, lpc32xx_spi);
  lpc32xx_ssp ssp_1;
  LPC32XX_FILL(0x2008c000, 0x20090000, lpc32xx_ssp);
  lpc32xx_spi spi_2;
  LPC32XX_FILL(0x20090000, 0x20094000, lpc32xx_spi);
  lpc_i2s i2s_0;
  LPC32XX_FILL(0x20094000, 0x20098000, lpc_i2s);
  lpc32xx_sd_card sd_card;
  LPC32XX_FILL(0x20098000, 0x2009c000, lpc32xx_sd_card);
  lpc_i2s i2s_1;
  LPC32XX_FILL(0x2009c000, 0x200a8000, lpc_i2s);
  lpc32xx_nand_mlc nand_mlc;
  LPC32XX_FILL(0x200a8000, 0x31000000, lpc32xx_nand_mlc);
  lpc_dma dma;
  LPC32XX_FILL(0x31000000, 0x31020000, lpc_dma);
  lpc32xx_usb usb;
  LPC32XX_FILL(0x31020000, 0x31040000, lpc32xx_usb);
  lpc32xx_lcd lcd;
  LPC32XX_FILL(0x31040000, 0x31060000, lpc32xx_lcd);
  lpc32xx_eth eth;
  LPC32XX_FILL(0x31060000, 0x31080000, lpc32xx_eth);
  lpc_emc emc;
  LPC32XX_FILL(0x31080000, 0x31080400, lpc_emc);
  lpc32xx_emc_ahb emc_ahb [5];
  LPC32XX_FILL(0x31080400, 0x310c0000, lpc32xx_emc_ahb [5]);
  lpc32xx_etb etb;
  LPC32XX_FILL(0x310c0000, 0x40004000, lpc32xx_etb);
  lpc32xx_syscon syscon;
  LPC32XX_FILL(0x40004000, 0x40008000, lpc32xx_syscon);
  lpc32xx_irq mic;
  LPC32XX_FILL(0x40008000, 0x4000c000, lpc32xx_irq);
  lpc32xx_irq sic_1;
  LPC32XX_FILL(0x4000c000, 0x40010000, lpc32xx_irq);
  lpc32xx_irq sic_2;
  LPC32XX_FILL(0x40010000, 0x40014000, lpc32xx_irq);
  lpc32xx_uart uart_1;
  LPC32XX_FILL(0x40014000, 0x40018000, lpc32xx_uart);
  lpc32xx_uart uart_2;
  LPC32XX_FILL(0x40018000, 0x4001c000, lpc32xx_uart);
  lpc32xx_uart uart_7;
  LPC32XX_FILL(0x4001c000, 0x40024000, lpc32xx_uart);
  lpc32xx_rtc rtc;
  LPC32XX_FILL(0x40024000, 0x40028000, lpc32xx_rtc);
  lpc32xx_gpio gpio;
  LPC32XX_FILL(0x40028000, 0x4002c000, lpc32xx_gpio);
  lpc_timer timer_4;
  LPC32XX_FILL(0x4002c000, 0x40030000, lpc_timer);
  lpc_timer timer_5;
  LPC32XX_FILL(0x40030000, 0x40034000, lpc_timer);
  lpc32xx_ms_timer ms_timer;
  LPC32XX_FILL(0x40034000, 0x40038000, lpc32xx_ms_timer);
  lpc32xx_hs_timer hs_timer;
  LPC32XX_FILL(0x40038000, 0x4003c000, lpc32xx_hs_timer);
  lpc32xx_wdt wdt;
  LPC32XX_FILL(0x4003c000, 0x40040000, lpc32xx_wdt);
  lpc32xx_debug debug;
  LPC32XX_FILL(0x40040000, 0x40044000, lpc32xx_debug);
  lpc_timer timer_0;
  LPC32XX_FILL(0x40044000, 0x40048000, lpc_timer);
  lpc32xx_adc adc;
  LPC32XX_FILL(0x40048000, 0x4004c000, lpc32xx_adc);
  lpc_timer timer_1;
  LPC32XX_FILL(0x4004c000, 0x40050000, lpc_timer);
  lpc32xx_keyscan keyscan;
  LPC32XX_FILL(0x40050000, 0x40054000, lpc32xx_keyscan);
  lpc32xx_uart_ctrl uart_ctrl;
  LPC32XX_FILL(0x40054000, 0x40058000, lpc32xx_uart_ctrl);
  lpc_timer timer_2;
  LPC32XX_FILL(0x40058000, 0x4005c000, lpc_timer);
  lpc32xx_pwm pwm_1_and_pwm_2;
  LPC32XX_FILL(0x4005c000, 0x40060000, lpc32xx_pwm);
  lpc_timer timer3;
  LPC32XX_FILL(0x40060000, 0x40080000, lpc_timer);
  lpc32xx_uart uart_3;
  LPC32XX_FILL(0x40080000, 0x40088000, lpc32xx_uart);
  lpc32xx_uart uart_4;
  LPC32XX_FILL(0x40088000, 0x40090000, lpc32xx_uart);
  lpc32xx_uart uart_5;
  LPC32XX_FILL(0x40090000, 0x40098000, lpc32xx_uart);
  lpc32xx_uart uart_6;
  LPC32XX_FILL(0x40098000, 0x400a0000, lpc32xx_uart);
  lpc32xx_i2c i2c_1;
  LPC32XX_FILL(0x400a0000, 0x400a8000, lpc32xx_i2c);
  lpc32xx_i2c i2c_2;
  LPC32XX_FILL(0x400a8000, 0x400e8000, lpc32xx_i2c);
  lpc32xx_mcpwm mcpwm;
} lpc32xx_registers;

extern volatile lpc32xx_registers lpc32xx;

/** @} */

#endif /* LIBBSP_ARM_LPC32XX_LPC32XX_H */
