/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Specific register definitions according to lpc176x family boards.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_H
#define LIBBSP_ARM_LPC176X_H

#include <bsp/lpc-i2s.h>

#define LPC176X_PLL_CON_PLLE BSP_BIT32( 0 )
#define LPC176X_PLL_CON_PLLC BSP_BIT32( 1 )
#define LPC176X_PLL_SEL_MSEL( val ) BSP_FLD32( val, 0, 14 )
#define LPC176X_PLL_SEL_MSEL_GET( reg ) BSP_FLD32GET( reg, 0, 14 )
#define LPC176X_PLL_SEL_MSEL_SET( reg, val ) BSP_FLD32SET( reg, val, 0, 14 )
#define LPC176X_PLL_SEL_PSEL( val ) BSP_FLD32( val, 16, 23 )
#define LPC176X_PLL_SEL_PSEL_GET( reg ) BSP_FLD32GET( reg, 16, 23 )
#define LPC176X_PLL_SEL_PSEL_SET( reg, val ) BSP_FLD32SET( reg, val, 16, 23 )
#define LPC176X_PLL_STAT_PLLE BSP_BIT32( 24 )
#define LPC176X_PLL_STAT_PLLC BSP_BIT32( 25 )
#define LPC176X_PLL_STAT_PLOCK BSP_BIT32( 26 )

/**
 * @brief Phase-Locked Loop representation.
 */
typedef struct {
  /**
   * @brief PLL Control Register.
   */
  uint32_t con;
  /**
   * @brief PLL Configuration Register.
   */
  uint32_t cfg;
  /**
   * @brief PLL Status Register.
   */
  uint32_t stat;
  /**
   * @brief PLL Feed Register.
   */
  uint32_t feed;
} lpc176x_pll;

#define LPC176X_SCB_BASE_ADDR 0x400FC000U
#define LPC176X_SCB_FLASHCFG_FLASHTIM( val ) BSP_FLD32( val, 12, 15 )
#define LPC176X_SCB_FLASHCFG_FLASHTIM_GET( reg ) BSP_FLD32GET( reg, 12, 15 )
#define LPC176X_SCB_FLASHCFG_FLASHTIM_SET( reg, val ) BSP_FLD32SET( reg, val, \
  12, 15 )
#define LPC176X_SCB_MEMMAP_MAP BSP_BIT32( 0 )
/* POWER MODE CONTROL REGISTER (PCON) */
/* Power mode control bit 0 */
#define LPC176X_SCB_PCON_PM0 BSP_BIT32( 0 )
/* Power mode control bit 1 */
#define LPC176X_SCB_PCON_PM1 BSP_BIT32( 1 )
/* Brown-Out Reduced ower Down */
#define LPC176X_SCB_PCON_BODRPM BSP_BIT32( 2 )
/* Brown-Out Global Disable */
#define LPC176X_SCB_PCON_BOGD BSP_BIT32( 3 )
/* Brown-Out Reset Disable */
#define LPC176X_SCB_PCON_BORD BSP_BIT32( 4 )
/* Sleep Mode entry flag */
#define LPC176X_SCB_PCON_SMFLAG BSP_BIT32( 8 )
/* Deep Sleep entry flag */
#define LPC176X_SCB_PCON_DSFLAG BSP_BIT32( 9 )
/* Power-Down entry flag */
#define LPC176X_SCB_PCON_PDFLAG BSP_BIT32( 10 )
/* Deep Power-Down entry flag */
#define LPC176X_SCB_PCON_DPDFLAG BSP_BIT32( 11 )
/* POWER CONTROL for PERIPHERALS REGISTER (PCONP) */
/* 0 - Reserved */
/* Timer/Counter 0 power/clock control bit */
#define LPC176X_SCB_PCONP_TIMER_0 BSP_BIT32( 1 )
/* Timer/Counter 1 power/clock control bit */
#define LPC176X_SCB_PCONP_TIMER_1 BSP_BIT32( 2 )
/* UART 0 power/clock control bit */
#define LPC176X_SCB_PCONP_UART_0 BSP_BIT32( 3 )
/* UART 1 power/clock control bit */
#define LPC176X_SCB_PCONP_UART_1 BSP_BIT32( 4 )
/* 5 - Reserved */
/* PWM 1 power/clock control bit */
#define LPC176X_SCB_PCONP_PWM_0 BSP_BIT32( 6 )
/* The I2C0 interface power/clock control bit */
#define LPC176X_SCB_PCONP_I2C_0 BSP_BIT32( 7 )
/* The SPI interface power/clock control bit */
#define LPC176X_SCB_PCONP_SPI BSP_BIT32( 8 )
/* The RTC power/clock control bit */
#define LPC176X_SCB_PCONP_RTC BSP_BIT32( 9 )
/* The SSP1 interface power/clock control bit */
#define LPC176X_SCB_PCONP_SSP_1 BSP_BIT32( 10 )
/* 11 - Reserved */
/* A/D converter (ADC) power/clock control bit */
#define LPC176X_SCB_PCONP_ADC BSP_BIT32( 12 )
/* CAN Controller 1 power/clock control bit */
#define LPC176X_SCB_PCONP_CAN_1 BSP_BIT32( 13 )
/* CAN Controller 2 power/clock control bit */
#define LPC176X_SCB_PCONP_CAN_2 BSP_BIT32( 14 )
/* Power/clock control bit for IOCON, GPIO, and GPIO interrupts*/
#define LPC176X_SCB_PCONP_GPIO BSP_BIT32( 15 )
/* Repetitive Interrupt Timer power/clock control bit */
#define LPC176X_SCB_PCONP_RIT BSP_BIT32( 16 )
/* Motor Control PWM */
#define LPC176X_SCB_PCONP_MCPWM BSP_BIT32( 17 )
/* Quadrate Encoder Interface power/clock control bit */
#define LPC176X_SCB_PCONP_QEI BSP_BIT32( 18 )
/* The IC21 interface power/clock control bit */
#define LPC176X_SCB_PCONP_I2C_1 BSP_BIT32( 19 )
/* 20 - Reserved */
/* The SSP0 interface power/clock control bit */
#define LPC176X_SCB_PCONP_SSP_0 BSP_BIT32( 21 )
/* Timer 2 power/clock control bit */
#define LPC176X_SCB_PCONP_TIMER_2 BSP_BIT32( 22 )
/* Timer 3 power/clock control bit */
#define LPC176X_SCB_PCONP_TIMER_3 BSP_BIT32( 23 )
/* UART 2 power/clock control bit */
#define LPC176X_SCB_PCONP_UART_2 BSP_BIT32( 24 )
/* UART 3 power/clock control bit */
#define LPC176X_SCB_PCONP_UART_3 BSP_BIT32( 25 )
/* I2C interface 2 power/clock control bit */
#define LPC176X_SCB_PCONP_I2C_2 BSP_BIT32( 26 )
/* I2S interface power/clock control bit */
#define LPC176X_SCB_PCONP_I2S BSP_BIT32( 27 )
/* 28 - Reserved */
/* GPDMA function power/clock control bit */
#define LPC176X_SCB_PCONP_GPDMA BSP_BIT32( 29 )
/* Ethernet block power/clock control bit */
#define LPC176X_SCB_PCONP_ENET BSP_BIT32( 30 )
/* USB interface power/clock control bit */
#define LPC176X_SCB_PCONP_USB BSP_BIT32( 31 )
#define LPC176X_SCB_CCLKSEL_CCLKDIV( val ) BSP_FLD32( val, 0, 7 )
#define LPC176X_SCB_CCLKSEL_CCLKDIV_GET( reg ) BSP_FLD32GET( reg, 0, 7 )
#define LPC176X_SCB_CCLKSEL_CCLKDIV_SET( reg, val ) BSP_FLD32SET( reg, \
  val, \
  0, \
  7 )
#define LPC176X_SCB_CCLKSEL_CCLKSEL BSP_BIT32( 8 )
#define LPC176X_SCB_USBCLKSEL_USBDIV( val ) BSP_FLD32( val, 0, 4 )
#define LPC176X_SCB_USBCLKSEL_USBDIV_GET( reg ) BSP_FLD32GET( reg, 0, 4 )
#define LPC176X_SCB_USBCLKSEL_USBDIV_SET( reg, val ) BSP_FLD32SET( reg, \
  val, \
  0, \
  4 )
#define LPC176X_SCB_USBCLKSEL_USBSEL( val ) BSP_FLD32( val, 8, 9 )
#define LPC176X_SCB_USBCLKSEL_USBSEL_GET( reg ) BSP_FLD32GET( reg, 8, 9 )
#define LPC176X_SCB_USBCLKSEL_USBSEL_SET( reg, val ) BSP_FLD32SET( reg, \
  val, \
  8, \
  9 )
#define LPC176X_SCB_CLKSRCSEL_CLKSRC BSP_BIT32( 0 )
#define LPC176X_SCB_SCS_MCIPWR BSP_BIT32( 3 )
#define LPC176X_SCB_SCS_OSC_RANGE_SEL BSP_BIT32( 4 )
#define LPC176X_SCB_SCS_OSC_ENABLE BSP_BIT32( 5 )
#define LPC176X_SCB_SCS_OSC_STATUS BSP_BIT32( 6 )
#define LPC176X_SCB_PCLKSEL_PCLKDIV( val ) BSP_FLD32( val, 0, 4 )
#define LPC176X_SCB_PCLKSEL_PCLKDIV_GET( reg ) BSP_FLD32GET( reg, 0, 4 )
#define LPC176X_SCB_PCLKSEL_PCLKDIV_SET( reg, val ) BSP_FLD32SET( reg, \
  val, \
  0, \
  4 )
#define LPC176X_SCB_PBOOST_BOOST BSP_BIT32( 0 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTSEL( val ) BSP_FLD32( val, 3, 0 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTSEL_GET( reg ) BSP_FLD32GET( reg, 3, 0 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTSEL_SET( reg, val ) BSP_FLD32SET( reg, \
  val, 3, 0 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTDIV( val ) BSP_FLD32( val, 7, 4 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTDIV_GET( reg ) BSP_FLD32GET( reg, 7, 4 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUTDIV_SET( reg, val ) BSP_FLD32SET( reg, \
  val, 7, 4 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUT_EN BSP_BIT32( 8 )
#define LPC176X_SCB_CLKOUTCFG_CLKOUT_ACT BSP_BIT32( 9 )

/**
 * @brief System Control Block representation.
 */
typedef struct {
  /**
   * @brief Flash Accelerator Configuration Register.
   */
  uint32_t flashcfg;
  uint32_t reserved_04[ 15 ];
  /**
   * @brief Memopry Map Control.
   */
  uint32_t memmap;
  uint32_t reserved_44[ 15 ];
  /**
   * @brief Phase-Locked Loop 0.
   */
  lpc176x_pll pll_0;
  uint32_t reserved_90[ 4 ];
  /**
   * @brief Phase-Locked Loop 1.
   */
  lpc176x_pll pll_1;
  uint32_t reserved_b0[ 4 ];
  /**
   * @brief Power Mode Control register.
   */
  uint32_t pcon;
  /**
   * @brief Power Control for Peripherals register.
   */
  uint32_t pconp;
  uint32_t reserved_c8[ 15 ];
  /**
   *@brief Selects the divide valuefor creating the CPU clock from the
   *       PPL0 output.
   */
  uint32_t cclksel;
  /**
   * @brief Selects the divide value for creating the USB clock from the
   *        PPL0 output.
   */
  uint32_t usbclksel;
  /**
   * @brief Clock Source Select register.
   */
  uint32_t clksrcsel;
  uint32_t reserved_110[ 12 ];
  /**
   * @brief External Interrupt flag register.
   */
  uint32_t extint;
  uint32_t reserved_144;
  /**
   * @brief  External Interrupt Mode register.
   */
  uint32_t extmode;
  /**
   * @brief  External Interrupt Polarity register.
   */
  uint32_t extpolar;
  uint32_t reserved_150[ 12 ];
  /**
   * @brief Reset Source Identification Register.
   */
  uint32_t rsid;
  uint32_t reserved_184[ 7 ];
  /**
   * @brief System Controls and Status Register.
   */
  uint32_t scs;
  uint32_t reserved_1a4;
  /**
   * @brief Peripheral Clock Selection registers 0 and 1.
   */
  uint32_t pclksel[ 2 ];
  /**
   * @brief Peripheral boost.
   */
  uint32_t pboost;
  uint32_t reserved_1b4[ 5 ];
  /**
   * @brief Clock Output Configuration Register.
   */
  uint32_t clkoutcfg;
  /**
   * @brief Reset Control bit 0 and 1.
   */
  uint32_t rstcon[ 2 ];
  uint32_t reserved_1d4[ 4 ];
} lpc176x_scb;

#define LPC176X_BASE 0x00u
#define LPC176X_SCB ( *(volatile lpc176x_scb *) ( LPC176X_SCB_BASE_ADDR ) )

/* Fast I/O setup */
#define LPC176X_FIO_BASE_ADDR 0x2009C000U
#define LPC176X_FIO ( (volatile lpc176x_fio *) LPC176X_FIO_BASE_ADDR )

#define FIO0DIR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x00U ) )
#define FIO0MASK ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x10U ) )
#define FIO0PIN ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x14U ) )
#define FIO0SET ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x18U ) )
#define FIO0CLR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x1CU ) )

#define FIO1DIR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x20U ) )
#define FIO1MASK ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x30U ) )
#define FIO1PIN ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x34U ) )
#define FIO1SET ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x38U ) )
#define FIO1CLR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x3CU ) )

#define FIO2DIR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x40U ) )
#define FIO2MASK ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x50U ) )
#define FIO2PIN ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x54U ) )
#define FIO2SET ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x58U ) )
#define FIO2CLR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x5CU ) )

#define FIO3DIR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x60U ) )
#define FIO3MASK ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x70U ) )
#define FIO3PIN ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x74U ) )
#define FIO3SET ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x78U ) )
#define FIO3CLR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x7CU ) )

#define FIO4DIR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x80U ) )
#define FIO4MASK ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x90U ) )
#define FIO4PIN ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x94U ) )
#define FIO4SET ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x98U ) )
#define FIO4CLR ( *(volatile uint32_t *) ( LPC176X_FIO_BASE_ADDR + 0x9CU ) )

/* FIOs can be accessed through WORD, HALF-WORD or BYTE. */
#define FIO0DIR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x00U ) )
#define FIO1DIR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x20U ) )
#define FIO2DIR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x40U ) )
#define FIO3DIR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x60U ) )
#define FIO4DIR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x80U ) )

#define FIO0DIR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x01U ) )
#define FIO1DIR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x21U ) )
#define FIO2DIR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x41U ) )
#define FIO3DIR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x61U ) )
#define FIO4DIR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x81U ) )

#define FIO0DIR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x02U ) )
#define FIO1DIR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x22U ) )
#define FIO2DIR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x42U ) )
#define FIO3DIR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x62U ) )
#define FIO4DIR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x82U ) )

#define FIO0DIR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x03U ) )
#define FIO1DIR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x23U ) )
#define FIO2DIR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x43U ) )
#define FIO3DIR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x63U ) )
#define FIO4DIR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x83U ) )

#define FIO0DIRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x00U ) )
#define FIO1DIRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x20U ) )
#define FIO2DIRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x40U ) )
#define FIO3DIRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x60U ) )
#define FIO4DIRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x80U ) )

#define FIO0DIRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x02U ) )
#define FIO1DIRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x22U ) )
#define FIO2DIRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x42U ) )
#define FIO3DIRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x62U ) )
#define FIO4DIRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x82U ) )

#define FIO0MASK0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x10U ) )
#define FIO1MASK0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x30U ) )
#define FIO2MASK0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x50U ) )
#define FIO3MASK0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x70U ) )
#define FIO4MASK0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x90U ) )

#define FIO0MASK1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x11U ) )
#define FIO1MASK1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x21U ) )
#define FIO2MASK1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x51U ) )
#define FIO3MASK1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x71U ) )
#define FIO4MASK1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x91U ) )

#define FIO0MASK2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x12U ) )
#define FIO1MASK2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x32U ) )
#define FIO2MASK2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x52U ) )
#define FIO3MASK2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x72U ) )
#define FIO4MASK2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x92U ) )

#define FIO0MASK3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x13U ) )
#define FIO1MASK3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x33U ) )
#define FIO2MASK3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x53U ) )
#define FIO3MASK3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x73U ) )
#define FIO4MASK3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x93U ) )

#define FIO0MASKL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x10U ) )
#define FIO1MASKL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x30U ) )
#define FIO2MASKL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x50U ) )
#define FIO3MASKL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x70U ) )
#define FIO4MASKL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x90U ) )

#define FIO0MASKU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x12U ) )
#define FIO1MASKU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x32U ) )
#define FIO2MASKU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x52U ) )
#define FIO3MASKU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x72U ) )
#define FIO4MASKU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x92U ) )

#define FIO0PIN0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x14U ) )
#define FIO1PIN0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x34U ) )
#define FIO2PIN0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x54U ) )
#define FIO3PIN0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x74U ) )
#define FIO4PIN0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x94U ) )

#define FIO0PIN1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x15U ) )
#define FIO1PIN1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x25U ) )
#define FIO2PIN1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x55U ) )
#define FIO3PIN1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x75U ) )
#define FIO4PIN1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x95U ) )

#define FIO0PIN2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x16U ) )
#define FIO1PIN2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x36U ) )
#define FIO2PIN2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x56U ) )
#define FIO3PIN2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x76U ) )
#define FIO4PIN2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x96U ) )

#define FIO0PIN3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x17U ) )
#define FIO1PIN3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x37U ) )
#define FIO2PIN3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x57U ) )
#define FIO3PIN3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x77U ) )
#define FIO4PIN3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x97U ) )

#define FIO0PINL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x14U ) )
#define FIO1PINL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x34U ) )
#define FIO2PINL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x54U ) )
#define FIO3PINL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x74U ) )
#define FIO4PINL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x94U ) )

#define FIO0PINU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x16U ) )
#define FIO1PINU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x36U ) )
#define FIO2PINU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x56U ) )
#define FIO3PINU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x76U ) )
#define FIO4PINU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x96U ) )

#define FIO0SET0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x18U ) )
#define FIO1SET0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x38U ) )
#define FIO2SET0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x58U ) )
#define FIO3SET0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x78U ) )
#define FIO4SET0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x98U ) )

#define FIO0SET1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x19U ) )
#define FIO1SET1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x29U ) )
#define FIO2SET1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x59U ) )
#define FIO3SET1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x79U ) )
#define FIO4SET1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x99U ) )

#define FIO0SET2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1AU ) )
#define FIO1SET2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x3AU ) )
#define FIO2SET2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5AU ) )
#define FIO3SET2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7AU ) )
#define FIO4SET2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9AU ) )

#define FIO0SET3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1BU ) )
#define FIO1SET3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x3BU ) )
#define FIO2SET3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5BU ) )
#define FIO3SET3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7BU ) )
#define FIO4SET3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9BU ) )

#define FIO0SETL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x18U ) )
#define FIO1SETL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x38U ) )
#define FIO2SETL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x58U ) )
#define FIO3SETL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x78U ) )
#define FIO4SETL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x98U ) )

#define FIO0SETU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x1AU ) )
#define FIO1SETU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x3AU ) )
#define FIO2SETU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x5AU ) )
#define FIO3SETU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x7AU ) )
#define FIO4SETU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x9AU ) )

#define FIO0CLR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1CU ) )
#define FIO1CLR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x3CU ) )
#define FIO2CLR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5CU ) )
#define FIO3CLR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7CU ) )
#define FIO4CLR0 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9CU ) )

#define FIO0CLR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1DU ) )
#define FIO1CLR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x2DU ) )
#define FIO2CLR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5DU ) )
#define FIO3CLR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7DU ) )
#define FIO4CLR1 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9DU ) )

#define FIO0CLR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1EU ) )
#define FIO1CLR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x3EU ) )
#define FIO2CLR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5EU ) )
#define FIO3CLR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7EU ) )
#define FIO4CLR2 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9EU ) )

#define FIO0CLR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x1FU ) )
#define FIO1CLR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x3FU ) )
#define FIO2CLR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x5FU ) )
#define FIO3CLR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x7FU ) )
#define FIO4CLR3 ( *(volatile uint8_t *) ( LPC176X_FIO_BASE_ADDR + 0x9FU ) )

#define FIO0CLRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x1CU ) )
#define FIO1CLRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x3CU ) )
#define FIO2CLRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x5CU ) )
#define FIO3CLRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x7CU ) )
#define FIO4CLRL ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x9CU ) )

#define FIO0CLRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x1EU ) )
#define FIO1CLRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x3EU ) )
#define FIO2CLRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x5EU ) )
#define FIO3CLRU ( *(volatile uint16_t *) ( LPC176X_FIO_BASE_ADDR + 0x7EU ) )
#define FIO4CLRU ( *(volatile uint16_t *) ( FIO_BASE_ADDR + 0x9EU ) )

#define LPC176X_USB_CLOCK 48000000U
#define LPC176X_MODULE_CLOCK_MASK 0x3U

/* Pin Connect Block */
#define PINSEL_BASE_ADDR 0x4002C000U

#define PINSEL0 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x00U ) )
#define PINSEL1 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x04U ) )
#define PINSEL2 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x08U ) )
#define PINSEL3 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x0CU ) )
#define PINSEL4 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x10U ) )
#define PINSEL5 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x14U ) )
#define PINSEL6 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x18U ) )
#define PINSEL7 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x1CU ) )
#define PINSEL8 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x20U ) )
#define PINSEL9 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x24U ) )
#define PINSEL10 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x28U ) )
#define PINSEL11 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x2CU ) )

#define PINMODE0 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x40U ) )
#define PINMODE1 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x44U ) )
#define PINMODE2 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x48U ) )
#define PINMODE3 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x4CU ) )
#define PINMODE4 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x50U ) )
#define PINMODE5 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x54U ) )
#define PINMODE6 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x58U ) )
#define PINMODE7 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x5CU ) )
#define PINMODE8 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x60U ) )
#define PINMODE9 ( *(volatile uint32_t *) ( PINSEL_BASE_ADDR + 0x64U ) )

/* Pulse Width Modulator (PWM) */
#define PWM0_BASE_ADDR 0x40014000

#define PWM0IR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x00 ) )
#define PWM0TCR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x04 ) )
#define PWM0TC ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x08 ) )
#define PWM0PR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x0C ) )
#define PWM0PC ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x10 ) )
#define PWM0MCR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x14 ) )
#define PWM0MR0 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x18 ) )
#define PWM0MR1 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x1C ) )
#define PWM0MR2 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x20 ) )
#define PWM0MR3 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x24 ) )
#define PWM0CCR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x28 ) )
#define PWM0CR0 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x2C ) )
#define PWM0CR1 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x30 ) )
#define PWM0CR2 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x34 ) )
#define PWM0CR3 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x38 ) )
#define PWM0EMR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x3C ) )
#define PWM0MR4 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x40 ) )
#define PWM0MR5 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x44 ) )
#define PWM0MR6 ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x48 ) )
#define PWM0PCR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x4C ) )
#define PWM0LER ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x50 ) )
#define PWM0CTCR ( *(volatile uint32_t *) ( PWM0_BASE_ADDR + 0x70 ) )

#define PWM1_BASE_ADDR 0x40018000

#define PWM1IR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x00 ) )
#define PWM1TCR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x04 ) )
#define PWM1TC ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x08 ) )
#define PWM1PR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x0C ) )
#define PWM1PC ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x10 ) )
#define PWM1MCR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x14 ) )
#define PWM1MR0 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x18 ) )
#define PWM1MR1 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x1C ) )
#define PWM1MR2 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x20 ) )
#define PWM1MR3 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x24 ) )
#define PWM1CCR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x28 ) )
#define PWM1CR0 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x2C ) )
#define PWM1CR1 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x30 ) )
#define PWM1CR2 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x34 ) )
#define PWM1CR3 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x38 ) )
#define PWM1EMR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x3C ) )
#define PWM1MR4 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x40 ) )
#define PWM1MR5 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x44 ) )
#define PWM1MR6 ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x48 ) )
#define PWM1PCR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x4C ) )
#define PWM1LER ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x50 ) )
#define PWM1CTCR ( *(volatile uint32_t *) ( PWM1_BASE_ADDR + 0x70 ) )

/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define UART0_BASE_ADDR 0x4000C000

#define U0RBR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x00 ) )
#define U0THR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x00 ) )
#define U0DLL ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x00 ) )
#define U0DLM ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x04 ) )
#define U0IER ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x04 ) )
#define U0IIR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x08 ) )
#define U0FCR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x08 ) )
#define U0LCR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x0C ) )
#define U0LSR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x14 ) )
#define U0SCR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x1C ) )
#define U0ACR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x20 ) )
#define U0ICR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x24 ) )
#define U0FDR ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x28 ) )
#define U0TER ( *(volatile uint32_t *) ( UART0_BASE_ADDR + 0x30 ) )

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define UART1_BASE_ADDR 0x40010000

#define U1RBR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x00 ) )
#define U1THR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x00 ) )
#define U1DLL ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x00 ) )
#define U1DLM ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x04 ) )
#define U1IER ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x04 ) )
#define U1IIR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x08 ) )
#define U1FCR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x08 ) )
#define U1LCR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x0C ) )
#define U1MCR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x10 ) )
#define U1LSR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x14 ) )
#define U1MSR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x18 ) )
#define U1SCR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x1C ) )
#define U1ACR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x20 ) )
#define U1FDR ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x28 ) )
#define U1TER ( *(volatile uint32_t *) ( UART1_BASE_ADDR + 0x30 ) )

/* Universal Asynchronous Receiver Transmitter 2 (UART2) */
#define UART2_BASE_ADDR 0x40098000

#define U2RBR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x00 ) )
#define U2THR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x00 ) )
#define U2DLL ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x00 ) )
#define U2DLM ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x04 ) )
#define U2IER ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x04 ) )
#define U2IIR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x08 ) )
#define U2FCR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x08 ) )
#define U2LCR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x0C ) )
#define U2LSR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x14 ) )
#define U2SCR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x1C ) )
#define U2ACR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x20 ) )
#define U2ICR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x24 ) )
#define U2FDR ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x28 ) )
#define U2TER ( *(volatile uint32_t *) ( UART2_BASE_ADDR + 0x30 ) )

/* Universal Asynchronous Receiver Transmitter 3 (UART3) */
#define UART3_BASE_ADDR 0x4009C000

#define U3RBR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x00 ) )
#define U3THR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x00 ) )
#define U3DLL ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x00 ) )
#define U3DLM ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x04 ) )
#define U3IER ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x04 ) )
#define U3IIR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x08 ) )
#define U3FCR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x08 ) )
#define U3LCR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x0C ) )
#define U3LSR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x14 ) )
#define U3SCR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x1C ) )
#define U3ACR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x20 ) )
#define U3ICR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x24 ) )
#define U3FDR ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x28 ) )
#define U3TER ( *(volatile uint32_t *) ( UART3_BASE_ADDR + 0x30 ) )

/* SPI0 (Serial Peripheral Interface 0) */
#define SPI0_BASE_ADDR 0xE0020000
#define S0SPCR ( *(volatile uint32_t *) ( SPI0_BASE_ADDR + 0x00 ) )
#define S0SPSR ( *(volatile uint32_t *) ( SPI0_BASE_ADDR + 0x04 ) )
#define S0SPDR ( *(volatile uint32_t *) ( SPI0_BASE_ADDR + 0x08 ) )
#define S0SPCCR ( *(volatile uint32_t *) ( SPI0_BASE_ADDR + 0x0C ) )
#define S0SPINT ( *(volatile uint32_t *) ( SPI0_BASE_ADDR + 0x1C ) )

/* Real Time Clock */
#define RTC_BASE_ADDR 0x40024000

#define RTC_ILR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x00 ) )
#define RTC_CTC ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x04 ) )
#define RTC_CCR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x08 ) )
#define RTC_CIIR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x0C ) )
#define RTC_AMR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x10 ) )
#define RTC_CTIME0 ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x14 ) )
#define RTC_CTIME1 ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x18 ) )
#define RTC_CTIME2 ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x1C ) )
#define RTC_SEC ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x20 ) )
#define RTC_MIN ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x24 ) )
#define RTC_HOUR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x28 ) )
#define RTC_DOM ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x2C ) )
#define RTC_DOW ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x30 ) )
#define RTC_DOY ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x34 ) )
#define RTC_MONTH ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x38 ) )
#define RTC_YEAR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x3C ) )
#define RTC_CISS ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x40 ) )
#define RTC_ALSEC ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x60 ) )
#define RTC_ALMIN ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x64 ) )
#define RTC_ALHOUR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x68 ) )
#define RTC_ALDOM ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x6C ) )
#define RTC_ALDOW ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x70 ) )
#define RTC_ALDOY ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x74 ) )
#define RTC_ALMON ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x78 ) )
#define RTC_ALYEAR ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x7C ) )
#define RTC_PREINT ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x80 ) )
#define RTC_PREFRAC ( *(volatile uint32_t *) ( RTC_BASE_ADDR + 0x84 ) )

/* A/D Converter 0 (AD0) */
#define AD0_BASE_ADDR 0x40034000

#define AD0CR ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x00 ) )
#define AD0GDR ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x04 ) )
#define AD0INTEN ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x0C ) )
#define AD0_DATA_START ( (volatile uint32_t *) ( AD0_BASE_ADDR + 0x10 ) )
#define AD0DR0 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x10 ) )
#define AD0DR1 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x14 ) )
#define AD0DR2 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x18 ) )
#define AD0DR3 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x1C ) )
#define AD0DR4 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x20 ) )
#define AD0DR5 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x24 ) )
#define AD0DR6 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x28 ) )
#define AD0DR7 ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x2C ) )
#define AD0STAT ( *(volatile uint32_t *) ( AD0_BASE_ADDR + 0x30 ) )

/* D/A Converter */
#define DAC_BASE_ADDR 0x4008C000

#define DACR ( *(volatile uint32_t *) ( DAC_BASE_ADDR + 0x00 ) )

/* CAN CONTROLLERS AND ACCEPTANCE FILTER */
#define CAN_ACCEPT_BASE_ADDR 0x4003C000

#define CAN_AFMR ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + 0x00 ) )
#define CAN_SFF_SA ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + 0x04 ) )
#define CAN_SFF_GRP_SA ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + \
                                                  0x08 ) )
#define CAN_EFF_SA ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + 0x0C ) )
#define CAN_EFF_GRP_SA ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + \
                                                  0x10 ) )
#define CAN_EOT ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + 0x14 ) )
#define CAN_LUT_ERR_ADR ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + \
                                                   0x18 ) )
#define CAN_LUT_ERR ( *(volatile uint32_t *) ( CAN_ACCEPT_BASE_ADDR + 0x1C ) )

#define CAN_CENTRAL_BASE_ADDR 0x40040000

#define CAN_TX_SR ( *(volatile uint32_t *) ( CAN_CENTRAL_BASE_ADDR + 0x00 ) )
#define CAN_RX_SR ( *(volatile uint32_t *) ( CAN_CENTRAL_BASE_ADDR + 0x04 ) )
#define CAN_MSR ( *(volatile uint32_t *) ( CAN_CENTRAL_BASE_ADDR + 0x08 ) )

#define CAN1_BASE_ADDR 0x40044000

#define CAN1MOD ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x00 ) )
#define CAN1CMR ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x04 ) )
#define CAN1GSR ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x08 ) )
#define CAN1ICR ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x0C ) )
#define CAN1IER ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x10 ) )
#define CAN1BTR ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x14 ) )
#define CAN1EWL ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x18 ) )
#define CAN1SR ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x1C ) )
#define CAN1RFS ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x20 ) )
#define CAN1RID ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x24 ) )
#define CAN1RDA ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x28 ) )
#define CAN1RDB ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x2C ) )

#define CAN1TFI1 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x30 ) )
#define CAN1TID1 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x34 ) )
#define CAN1TDA1 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x38 ) )
#define CAN1TDB1 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x3C ) )
#define CAN1TFI2 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x40 ) )
#define CAN1TID2 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x44 ) )
#define CAN1TDA2 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x48 ) )
#define CAN1TDB2 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x4C ) )
#define CAN1TFI3 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x50 ) )
#define CAN1TID3 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x54 ) )
#define CAN1TDA3 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x58 ) )
#define CAN1TDB3 ( *(volatile uint32_t *) ( CAN1_BASE_ADDR + 0x5C ) )

#define CAN2_BASE_ADDR 0x40048000

#define CAN2MOD ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x00 ) )
#define CAN2CMR ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x04 ) )
#define CAN2GSR ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x08 ) )
#define CAN2ICR ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x0C ) )
#define CAN2IER ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x10 ) )
#define CAN2BTR ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x14 ) )
#define CAN2EWL ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x18 ) )
#define CAN2SR ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x1C ) )
#define CAN2RFS ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x20 ) )
#define CAN2RID ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x24 ) )
#define CAN2RDA ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x28 ) )
#define CAN2RDB ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x2C ) )

#define CAN2TFI1 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x30 ) )
#define CAN2TID1 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x34 ) )
#define CAN2TDA1 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x38 ) )
#define CAN2TDB1 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x3C ) )
#define CAN2TFI2 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x40 ) )
#define CAN2TID2 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x44 ) )
#define CAN2TDA2 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x48 ) )
#define CAN2TDB2 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x4C ) )
#define CAN2TFI3 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x50 ) )
#define CAN2TID3 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x54 ) )
#define CAN2TDA3 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x58 ) )
#define CAN2TDB3 ( *(volatile uint32_t *) ( CAN2_BASE_ADDR + 0x5C ) )

/* MultiMedia Card Interface(MCI) Controller */
#define MCI_BASE_ADDR 0x400c0000

#define MCI_POWER ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x00 ) )
#define MCI_CLOCK ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x04 ) )
#define MCI_ARGUMENT ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x08 ) )
#define MCI_COMMAND ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x0C ) )
#define MCI_RESP_CMD ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x10 ) )
#define MCI_RESP0 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x14 ) )
#define MCI_RESP1 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x18 ) )
#define MCI_RESP2 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x1C ) )
#define MCI_RESP3 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x20 ) )
#define MCI_DATA_TMR ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x24 ) )
#define MCI_DATA_LEN ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x28 ) )
#define MCI_DATA_CTRL ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x2C ) )
#define MCI_DATA_CNT ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x30 ) )
#define MCI_STATUS ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x34 ) )
#define MCI_CLEAR ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x38 ) )
#define MCI_MASK0 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x3C ) )
#define MCI_MASK1 ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x40 ) )
#define MCI_FIFO_CNT ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x48 ) )
#define MCI_FIFO ( *(volatile uint32_t *) ( MCI_BASE_ADDR + 0x80 ) )

/* I2S Interface Controller (I2S) */
#define I2S_BASE_ADDR 0x400a8000

#define I2S_DAO ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x00 ) )
#define I2S_DAI ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x04 ) )
#define I2S_TX_FIFO ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x08 ) )
#define I2S_RX_FIFO ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x0C ) )
#define I2S_STATE ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x10 ) )
#define I2S_DMA1 ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x14 ) )
#define I2S_DMA2 ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x18 ) )
#define I2S_IRQ ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x1C ) )
#define I2S_TXRATE ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x20 ) )
#define I2S_RXRATE ( *(volatile uint32_t *) ( I2S_BASE_ADDR + 0x24 ) )

/* General-purpose DMA Controller */
#define DMA_BASE_ADDR 0x50004000

#define GPDMA_INT_STAT ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x000 ) )
#define GPDMA_INT_TCSTAT ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x004 ) )
#define GPDMA_INT_TCCLR ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x008 ) )
#define GPDMA_INT_ERR_STAT ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x00C ) )
#define GPDMA_INT_ERR_CLR ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x010 ) )
#define GPDMA_RAW_INT_TCSTAT ( *(volatile uint32_t *) ( DMA_BASE_ADDR + \
                                                        0x014 ) )
#define GPDMA_RAW_INT_ERR_STAT ( *(volatile uint32_t *) ( DMA_BASE_ADDR + \
                                                          0x018 ) )
#define GPDMA_ENABLED_CHNS ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x01C ) )
#define GPDMA_SOFT_BREQ ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x020 ) )
#define GPDMA_SOFT_SREQ ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x024 ) )
#define GPDMA_SOFT_LBREQ ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x028 ) )
#define GPDMA_SOFT_LSREQ ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x02C ) )
#define GPDMA_CONFIG ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x030 ) )
#define GPDMA_SYNC ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x034 ) )

/* DMA channel 0 registers */
#define GPDMA_CH0_BASE_ADDR ( DMA_BASE_ADDR + 0x100 )
#define GPDMA_CH0_SRC ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x100 ) )
#define GPDMA_CH0_DEST ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x104 ) )
#define GPDMA_CH0_LLI ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x108 ) )
#define GPDMA_CH0_CTRL ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x10C ) )
#define GPDMA_CH0_CFG ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x110 ) )

/* DMA channel 1 registers */
#define GPDMA_CH1_BASE_ADDR ( DMA_BASE_ADDR + 0x120 )
#define GPDMA_CH1_SRC ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x120 ) )
#define GPDMA_CH1_DEST ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x124 ) )
#define GPDMA_CH1_LLI ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x128 ) )
#define GPDMA_CH1_CTRL ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x12C ) )
#define GPDMA_CH1_CFG ( *(volatile uint32_t *) ( DMA_BASE_ADDR + 0x130 ) )

/* USB Controller */
#define USB_INT_BASE_ADDR 0x400fc1c0
#define USB_BASE_ADDR 0x2008c200

#define USB_INT_STAT ( *(volatile uint32_t *) ( USB_INT_BASE_ADDR + 0x00 ) )

/* USB Device Interrupt Registers */
#define DEV_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x00 ) )
#define DEV_INT_EN ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x04 ) )
#define DEV_INT_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x08 ) )
#define DEV_INT_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x0C ) )
#define DEV_INT_PRIO ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x2C ) )

/* USB Device Endpoint Interrupt Registers */
#define EP_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x30 ) )
#define EP_INT_EN ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x34 ) )
#define EP_INT_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x38 ) )
#define EP_INT_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x3C ) )
#define EP_INT_PRIO ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x40 ) )

/* USB Device Endpoint Realization Registers */
#define REALIZE_EP ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x44 ) )
#define EP_INDEX ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x48 ) )
#define MAXPACKET_SIZE ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x4C ) )

/* USB Device Command Reagisters */
#define CMD_CODE ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x10 ) )
#define CMD_DATA ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x14 ) )

/* USB Device Data Transfer Registers */
#define RX_DATA ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x18 ) )
#define TX_DATA ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x1C ) )
#define RX_PLENGTH ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x20 ) )
#define TX_PLENGTH ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x24 ) )
#define USB_CTRL ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x28 ) )

/* USB Device DMA Registers */
#define DMA_REQ_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x50 ) )
#define DMA_REQ_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x54 ) )
#define DMA_REQ_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x58 ) )
#define UDCA_HEAD ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x80 ) )
#define EP_DMA_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x84 ) )
#define EP_DMA_EN ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x88 ) )
#define EP_DMA_DIS ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x8C ) )
#define DMA_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x90 ) )
#define DMA_INT_EN ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0x94 ) )
#define EOT_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xA0 ) )
#define EOT_INT_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xA4 ) )
#define EOT_INT_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xA8 ) )
#define NDD_REQ_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xAC ) )
#define NDD_REQ_INT_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xB0 ) )
#define NDD_REQ_INT_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xB4 ) )
#define SYS_ERR_INT_STAT ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xB8 ) )
#define SYS_ERR_INT_CLR ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xBC ) )
#define SYS_ERR_INT_SET ( *(volatile uint32_t *) ( USB_BASE_ADDR + 0xC0 ) )

/* USB Host Controller */
#define USBHC_BASE_ADDR 0x2008c000

#define HC_REVISION ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x00 ) )
#define HC_CONTROL ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x04 ) )
#define HC_CMD_STAT ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x08 ) )
#define HC_INT_STAT ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x0C ) )
#define HC_INT_EN ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x10 ) )
#define HC_INT_DIS ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x14 ) )
#define HC_HCCA ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x18 ) )
#define HC_PERIOD_CUR_ED ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x1C ) )
#define HC_CTRL_HEAD_ED ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x20 ) )
#define HC_CTRL_CUR_ED ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x24 ) )
#define HC_BULK_HEAD_ED ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x28 ) )
#define HC_BULK_CUR_ED ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x2C ) )
#define HC_DONE_HEAD ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x30 ) )
#define HC_FM_INTERVAL ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x34 ) )
#define HC_FM_REMAINING ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x38 ) )
#define HC_FM_NUMBER ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x3C ) )
#define HC_PERIOD_START ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x40 ) )
#define HC_LS_THRHLD ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x44 ) )
#define HC_RH_DESCA ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x48 ) )
#define HC_RH_DESCB ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x4C ) )
#define HC_RH_STAT ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x50 ) )
#define HC_RH_PORT_STAT1 ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x54 ) )
#define HC_RH_PORT_STAT2 ( *(volatile uint32_t *) ( USBHC_BASE_ADDR + 0x58 ) )

/* USB OTG Controller */
#define USBOTG_BASE_ADDR 0x2008c100

#define OTG_INT_STAT ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x00 ) )
#define OTG_INT_EN ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x04 ) )
#define OTG_INT_SET ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x08 ) )
#define OTG_INT_CLR ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x0C ) )
#define OTG_STAT_CTRL ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x10 ) )
#define OTG_TIMER ( *(volatile uint32_t *) ( USBOTG_BASE_ADDR + 0x14 ) )

#define USBOTG_I2C_BASE_ADDR 0x2008c300

#define OTG_I2C_RX ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + 0x00 ) )
#define OTG_I2C_TX ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + 0x00 ) )
#define OTG_I2C_STS ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + 0x04 ) )
#define OTG_I2C_CTL ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + 0x08 ) )
#define OTG_I2C_CLKHI ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + \
                                                 0x0C ) )
#define OTG_I2C_CLKLO ( *(volatile uint32_t *) ( USBOTG_I2C_BASE_ADDR + \
                                                 0x10 ) )

#define USBOTG_CLK_BASE_ADDR 0x2008CFF0U

#define OTG_CLK_CTRL ( *(volatile uint32_t *) ( USBOTG_CLK_BASE_ADDR + \
                                                0x04U ) )
#define OTG_CLK_STAT ( *(volatile uint32_t *) ( USBOTG_CLK_BASE_ADDR + \
                                                0x08U ) )

/* Register Fields */
#define GET_FIELD( val, mask, shift ) \
  ( ( ( val ) & ( mask ) ) >> ( shift ) )

#define SET_FIELD( val, field, mask, shift ) \
  ( ( ( val ) & ~( mask ) ) | ( ( ( field ) << ( shift ) ) & ( mask ) ) )

/* CLKSRCSEL */
#define CLKSRCSEL_CLKSRC_MASK 0x00000003U

#define GET_CLKSRCSEL_CLKSRC( reg ) \
  GET_FIELD( reg, CLKSRCSEL_CLKSRC_MASK, 0 )

#define SET_CLKSRCSEL_CLKSRC( reg, val ) \
  SET_FIELD( reg, val, CLKSRCSEL_CLKSRC_MASK, 0 )

/* PLLCON */
#define PLLCON_PLLE 0x00000001U

#define PLLCON_PLLC 0x00000002U

/* PLLCFG */
#define PLLCFG_MSEL_MASK 0x00007fffU

#define GET_PLLCFG_MSEL( reg ) \
  GET_FIELD( reg, PLLCFG_MSEL_MASK, 0 )

#define SET_PLLCFG_MSEL( reg, val ) \
  SET_FIELD( reg, val, PLLCFG_MSEL_MASK, 0 )

#define PLLCFG_NSEL_MASK 0x00ff0000U

#define GET_PLLCFG_NSEL( reg ) \
  GET_FIELD( reg, PLLCFG_NSEL_MASK, 16 )

#define SET_PLLCFG_NSEL( reg, val ) \
  SET_FIELD( reg, val, PLLCFG_NSEL_MASK, 16 )

/* PLLSTAT */
#define PLLSTAT_MSEL_MASK 0x00007fffU

#define GET_PLLSTAT_MSEL( reg ) \
  GET_FIELD( reg, PLLSTAT_MSEL_MASK, 0 )

#define SET_PLLSTAT_MSEL( reg, val ) \
  SET_FIELD( reg, val, PLLSTAT_MSEL_MASK, 0 )

#define PLLSTAT_NSEL_MASK 0x00ff0000U

#define GET_PLLSTAT_NSEL( reg ) \
  GET_FIELD( reg, PLLSTAT_NSEL_MASK, 16 )

#define SET_PLLSTAT_NSEL( reg, val ) \
  SET_FIELD( reg, val, PLLSTAT_NSEL_MASK, 16 )

#define PLLSTAT_PLLE 0x01000000U

#define PLLSTAT_PLLC 0x02000000U

#define PLLSTAT_PLOCK 0x04000000U

/* CCLKCFG */
#define CCLKCFG_CCLKSEL_MASK 0x000000ffU

#define GET_CCLKCFG_CCLKSEL( reg ) \
  GET_FIELD( reg, CCLKCFG_CCLKSEL_MASK, 0 )

#define SET_CCLKCFG_CCLKSEL( reg, val ) \
  SET_FIELD( reg, val, CCLKCFG_CCLKSEL_MASK, 0 )

/* MEMMAP */
#define MEMMAP_MAP_MASK 0x00000003U

#define GET_MEMMAP_MAP( reg ) \
  GET_FIELD( reg, MEMMAP_MAP_MASK, 0 )

#define SET_MEMMAP_MAP( reg, val ) \
  SET_FIELD( reg, val, MEMMAP_MAP_MASK, 0 )

/* TIR */
#define TIR_MR0 0x00000001U
#define TIR_MR1 0x00000002U
#define TIR_MR2 0x00000004U
#define TIR_MR3 0x00000008U
#define TIR_CR0 0x00000010U
#define TIR_CR1 0x00000020U
#define TIR_CR2 0x00000040U
#define TIR_CR3 0x00000080U

/* TCR */
#define TCR_EN 0x00000001U
#define TCR_RST 0x00000002U

/* TMCR */
#define TMCR_MR0I 0x00000001U
#define TMCR_MR0R 0x00000002U
#define TMCR_MR0S 0x00000004U
#define TMCR_MR1I 0x00000008U
#define TMCR_MR1R 0x00000010U
#define TMCR_MR1S 0x00000020U
#define TMCR_MR2I 0x00000040U
#define TMCR_MR2R 0x00000080U
#define TMCR_MR2S 0x00000100U
#define TMCR_MR3I 0x00000200U
#define TMCR_MR3R 0x00000400U
#define TMCR_MR3S 0x00000800U

/* PCLKSEL0 */
#define PCLKSEL0_PCLK_WDT_MASK 0x00000003U

#define GET_PCLKSEL0_PCLK_WDT( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_WDT_MASK, 0 )

#define SET_PCLKSEL0_PCLK_WDT( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_WDT_MASK, 0 )

#define PCLKSEL0_PCLK_TIMER0_MASK 0x0000000cU

#define GET_PCLKSEL0_PCLK_TIMER0( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_TIMER0_MASK, 2 )

#define SET_PCLKSEL0_PCLK_TIMER0( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_TIMER0_MASK, 2 )

#define PCLKSEL0_PCLK_TIMER1_MASK 0x00000030U

#define GET_PCLKSEL0_PCLK_TIMER1( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_TIMER1_MASK, 4 )

#define SET_PCLKSEL0_PCLK_TIMER1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_TIMER1_MASK, 4 )

#define PCLKSEL0_PCLK_UART0_MASK 0x000000c0U

#define GET_PCLKSEL0_PCLK_UART0( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_UART0_MASK, 6 )

#define SET_PCLKSEL0_PCLK_UART0( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_UART0_MASK, 6 )

#define PCLKSEL0_PCLK_UART1_MASK 0x00000300U

#define GET_PCLKSEL0_PCLK_UART1( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_UART1_MASK, 8 )

#define SET_PCLKSEL0_PCLK_UART1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_UART1_MASK, 8 )

#define PCLKSEL0_PCLK_PWM0_MASK 0x00000c00U

#define GET_PCLKSEL0_PCLK_PWM0( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_PWM0_MASK, 10 )

#define SET_PCLKSEL0_PCLK_PWM0( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_PWM0_MASK, 10 )

#define PCLKSEL0_PCLK_PWM1_MASK 0x00003000U

#define GET_PCLKSEL0_PCLK_PWM1( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_PWM1_MASK, 12 )

#define SET_PCLKSEL0_PCLK_PWM1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_PWM1_MASK, 12 )

#define PCLKSEL0_PCLK_I2C0_MASK 0x0000c000U

#define GET_PCLKSEL0_PCLK_I2C0( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_I2C0_MASK, 14 )

#define SET_PCLKSEL0_PCLK_I2C0( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_I2C0_MASK, 14 )

#define PCLKSEL0_PCLK_SPI_MASK 0x00030000U

#define GET_PCLKSEL0_PCLK_SPI( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_SPI_MASK, 16 )

#define SET_PCLKSEL0_PCLK_SPI( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_SPI_MASK, 16 )

#define PCLKSEL0_PCLK_RTC_MASK 0x000c0000U

#define GET_PCLKSEL0_PCLK_RTC( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_RTC_MASK, 18 )

#define SET_PCLKSEL0_PCLK_RTC( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_RTC_MASK, 18 )

#define PCLKSEL0_PCLK_SSP1_MASK 0x00300000U

#define GET_PCLKSEL0_PCLK_SSP1( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_SSP1_MASK, 20 )

#define SET_PCLKSEL0_PCLK_SSP1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_SSP1_MASK, 20 )

#define PCLKSEL0_PCLK_DAC_MASK 0x00c00000U

#define GET_PCLKSEL0_PCLK_DAC( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_DAC_MASK, 22 )

#define SET_PCLKSEL0_PCLK_DAC( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_DAC_MASK, 22 )

#define PCLKSEL0_PCLK_ADC_MASK 0x03000000U

#define GET_PCLKSEL0_PCLK_ADC( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_ADC_MASK, 24 )

#define SET_PCLKSEL0_PCLK_ADC( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_ADC_MASK, 24 )

#define PCLKSEL0_PCLK_CAN1_MASK 0x0c000000U

#define GET_PCLKSEL0_PCLK_CAN1( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_CAN1_MASK, 26 )

#define SET_PCLKSEL0_PCLK_CAN1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_CAN1_MASK, 26 )

#define PCLKSEL0_PCLK_CAN2_MASK 0x30000000U

#define GET_PCLKSEL0_PCLK_CAN2( reg ) \
  GET_FIELD( reg, PCLKSEL0_PCLK_CAN2_MASK, 28 )

#define SET_PCLKSEL0_PCLK_CAN2( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL0_PCLK_CAN2_MASK, 28 )

/* PCLKSEL1 */
#define PCLKSEL1_PCLK_BAT_RAM_MASK 0x00000003U

#define GET_PCLKSEL1_PCLK_BAT_RAM( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_BAT_RAM_MASK, 0 )

#define SET_PCLKSEL1_PCLK_BAT_RAM( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_BAT_RAM_MASK, 0 )

#define PCLKSEL1_PCLK_GPIO_MASK 0x0000000cU

#define GET_PCLKSEL1_PCLK_GPIO( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_GPIO_MASK, 2 )

#define SET_PCLKSEL1_PCLK_GPIO( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_GPIO_MASK, 2 )

#define PCLKSEL1_PCLK_PCB_MASK 0x00000030U

#define GET_PCLKSEL1_PCLK_PCB( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_PCB_MASK, 4 )

#define SET_PCLKSEL1_PCLK_PCB( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_PCB_MASK, 4 )

#define PCLKSEL1_PCLK_I2C1_MASK 0x000000c0U

#define GET_PCLKSEL1_PCLK_I2C1( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_I2C1_MASK, 6 )

#define SET_PCLKSEL1_PCLK_I2C1( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_I2C1_MASK, 6 )

#define PCLKSEL1_PCLK_SSP0_MASK 0x00000c00U

#define GET_PCLKSEL1_PCLK_SSP0( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_SSP0_MASK, 10 )

#define SET_PCLKSEL1_PCLK_SSP0( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_SSP0_MASK, 10 )

#define PCLKSEL1_PCLK_TIMER2_MASK 0x00003000U

#define GET_PCLKSEL1_PCLK_TIMER2( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_TIMER2_MASK, 12 )

#define SET_PCLKSEL1_PCLK_TIMER2( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_TIMER2_MASK, 12 )

#define PCLKSEL1_PCLK_TIMER3_MASK 0x0000c000U

#define GET_PCLKSEL1_PCLK_TIMER3( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_TIMER3_MASK, 14 )

#define SET_PCLKSEL1_PCLK_TIMER3( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_TIMER3_MASK, 14 )

#define PCLKSEL1_PCLK_UART2_MASK 0x00030000U

#define GET_PCLKSEL1_PCLK_UART2( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_UART2_MASK, 16 )

#define SET_PCLKSEL1_PCLK_UART2( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_UART2_MASK, 16 )

#define PCLKSEL1_PCLK_UART3_MASK 0x000c0000U

#define GET_PCLKSEL1_PCLK_UART3( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_UART3_MASK, 18 )

#define SET_PCLKSEL1_PCLK_UART3( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_UART3_MASK, 18 )

#define PCLKSEL1_PCLK_I2C2_MASK 0x00300000U

#define GET_PCLKSEL1_PCLK_I2C2( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_I2C2_MASK, 20 )

#define SET_PCLKSEL1_PCLK_I2C2( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_I2C2_MASK, 20 )

#define PCLKSEL1_PCLK_I2S_MASK 0x00c00000U

#define GET_PCLKSEL1_PCLK_I2S( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_I2S_MASK, 22 )

#define SET_PCLKSEL1_PCLK_I2S( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_I2S_MASK, 22 )

#define PCLKSEL1_PCLK_MCI_MASK 0x03000000U

#define GET_PCLKSEL1_PCLK_MCI( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_MCI_MASK, 24 )

#define SET_PCLKSEL1_PCLK_MCI( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_MCI_MASK, 24 )

#define PCLKSEL1_PCLK_SYSCON_MASK 0x30000000U

#define GET_PCLKSEL1_PCLK_SYSCON( reg ) \
  GET_FIELD( reg, PCLKSEL1_PCLK_SYSCON_MASK, 28 )

#define SET_PCLKSEL1_PCLK_SYSCON( reg, val ) \
  SET_FIELD( reg, val, PCLKSEL1_PCLK_SYSCON_MASK, 28 )

/* RTC_ILR */
#define RTC_ILR_RTCCIF 0x00000001U
#define RTC_ILR_RTCALF 0x00000002U
#define RTC_ILR_RTSSF 0x00000004U

/* RTC_CCR */
#define RTC_CCR_CLKEN 0x00000001U
#define RTC_CCR_CTCRST 0x00000002U
#define RTC_CCR_CLKSRC 0x00000010U

/* GPDMA */
typedef struct {
  uint32_t src;
  uint32_t dest;
  uint32_t lli;
  uint32_t ctrl;
} lpc176x_dma_descriptor;

typedef struct {
  lpc176x_dma_descriptor desc;
  uint32_t cfg;
} lpc176x_dma_channel;

#define GPDMA_CH_NUMBER 2
#define GPDMA_STATUS_CH_0 0x00000001U
#define GPDMA_STATUS_CH_1 0x00000002U
#define GPDMA_CH_BASE_ADDR( i ) \
  ( (volatile lpc176x_dma_channel *) \
    ( ( i ) ? GPDMA_CH1_BASE_ADDR : GPDMA_CH0_BASE_ADDR ) )

/* GPDMA_CONFIG */
#define GPDMA_CONFIG_EN 0x00000001U
#define GPDMA_CONFIG_MODE 0x00000002U

/* GPDMA_ENABLED_CHNS */
#define GPDMA_ENABLED_CHNS_CH0 0x00000001U
#define GPDMA_ENABLED_CHNS_CH1 0x00000002U

/* GPDMA_CH_CTRL */
#define GPDMA_CH_CTRL_TSZ_MASK 0x00000fffU

#define GET_GPDMA_CH_CTRL_TSZ( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_TSZ_MASK, 0 )

#define SET_GPDMA_CH_CTRL_TSZ( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_TSZ_MASK, 0 )

#define GPDMA_CH_CTRL_TSZ_MAX 0x00000fffU

#define GPDMA_CH_CTRL_SBSZ_MASK 0x00007000U

#define GET_GPDMA_CH_CTRL_SBSZ( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_SBSZ_MASK, 12 )

#define SET_GPDMA_CH_CTRL_SBSZ( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_SBSZ_MASK, 12 )

#define GPDMA_CH_CTRL_DBSZ_MASK 0x00038000U

#define GET_GPDMA_CH_CTRL_DBSZ( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_DBSZ_MASK, 15 )

#define SET_GPDMA_CH_CTRL_DBSZ( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_DBSZ_MASK, 15 )

#define GPDMA_CH_CTRL_BSZ_1 0x00000000U

#define GPDMA_CH_CTRL_BSZ_4 0x00000001U

#define GPDMA_CH_CTRL_BSZ_8 0x00000002U

#define GPDMA_CH_CTRL_BSZ_16 0x00000003U

#define GPDMA_CH_CTRL_BSZ_32 0x00000004U

#define GPDMA_CH_CTRL_BSZ_64 0x00000005U

#define GPDMA_CH_CTRL_BSZ_128 0x00000006U

#define GPDMA_CH_CTRL_BSZ_256 0x00000007U

#define GPDMA_CH_CTRL_SW_MASK 0x001c0000U

#define GET_GPDMA_CH_CTRL_SW( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_SW_MASK, 18 )

#define SET_GPDMA_CH_CTRL_SW( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_SW_MASK, 18 )

#define GPDMA_CH_CTRL_DW_MASK 0x00e00000U

#define GET_GPDMA_CH_CTRL_DW( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_DW_MASK, 21 )

#define SET_GPDMA_CH_CTRL_DW( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_DW_MASK, 21 )

#define GPDMA_CH_CTRL_W_8 0x00000000U

#define GPDMA_CH_CTRL_W_16 0x00000001U

#define GPDMA_CH_CTRL_W_32 0x00000002U

#define GPDMA_CH_CTRL_SI 0x04000000U

#define GPDMA_CH_CTRL_DI 0x08000000U

#define GPDMA_CH_CTRL_PROT_MASK 0x70000000U

#define GET_GPDMA_CH_CTRL_PROT( reg ) \
  GET_FIELD( reg, GPDMA_CH_CTRL_PROT_MASK, 28 )

#define SET_GPDMA_CH_CTRL_PROT( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CTRL_PROT_MASK, 28 )

#define GPDMA_CH_CTRL_ITC 0x80000000U

/* GPDMA_CH_CFG */
#define GPDMA_CH_CFG_EN 0x00000001U

#define GPDMA_CH_CFG_SRCPER_MASK 0x0000001eU

#define GET_GPDMA_CH_CFG_SRCPER( reg ) \
  GET_FIELD( reg, GPDMA_CH_CFG_SRCPER_MASK, 1 )

#define SET_GPDMA_CH_CFG_SRCPER( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CFG_SRCPER_MASK, 1 )

#define GPDMA_CH_CFG_DESTPER_MASK 0x000003c0U

#define GET_GPDMA_CH_CFG_DESTPER( reg ) \
  GET_FIELD( reg, GPDMA_CH_CFG_DESTPER_MASK, 6 )

#define SET_GPDMA_CH_CFG_DESTPER( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CFG_DESTPER_MASK, 6 )

#define GPDMA_CH_CFG_PER_SSP0_TX 0x00000000U

#define GPDMA_CH_CFG_PER_SSP0_RX 0x00000001U

#define GPDMA_CH_CFG_PER_SSP1_TX 0x00000002U

#define GPDMA_CH_CFG_PER_SSP1_RX 0x00000003U

#define GPDMA_CH_CFG_PER_SD_MMC 0x00000004U

#define GPDMA_CH_CFG_PER_I2S_CH0 0x00000005U

#define GPDMA_CH_CFG_PER_I2S_CH1 0x00000006U

#define GPDMA_CH_CFG_FLOW_MASK 0x00003800U

#define GET_GPDMA_CH_CFG_FLOW( reg ) \
  GET_FIELD( reg, GPDMA_CH_CFG_FLOW_MASK, 11 )

#define SET_GPDMA_CH_CFG_FLOW( reg, val ) \
  SET_FIELD( reg, val, GPDMA_CH_CFG_FLOW_MASK, 11 )

#define GPDMA_CH_CFG_FLOW_MEM_TO_MEM_DMA 0x00000000U

#define GPDMA_CH_CFG_FLOW_MEM_TO_PER_DMA 0x00000001U

#define GPDMA_CH_CFG_FLOW_PER_TO_MEM_DMA 0x00000002U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_DMA 0x00000003U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_DEST 0x00000004U

#define GPDMA_CH_CFG_FLOW_MEM_TO_PER_PER 0x00000005U

#define GPDMA_CH_CFG_FLOW_PER_TO_MEM_PER 0x00000006U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_SRC 0x00000007U

#define GPDMA_CH_CFG_IE 0x00004000U

#define GPDMA_CH_CFG_ITC 0x00008000U

#define GPDMA_CH_CFG_LOCK 0x00010000U

#define GPDMA_CH_CFG_ACTIVE 0x00020000U

#define GPDMA_CH_CFG_HALT 0x00040000U

/* AHBCFG */
#define AHBCFG_SCHEDULER_UNIFORM 0x00000001U

#define AHBCFG_BREAK_BURST_MASK 0x00000006U

#define GET_AHBCFG_BREAK_BURST( reg ) \
  GET_FIELD( reg, AHBCFG_BREAK_BURST_MASK, 1 )

#define SET_AHBCFG_BREAK_BURST( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_BREAK_BURST_MASK, 1 )

#define AHBCFG_QUANTUM_BUS_CYCLE 0x00000008U

#define AHBCFG_QUANTUM_SIZE_MASK 0x000000f0U

#define GET_AHBCFG_QUANTUM_SIZE( reg ) \
  GET_FIELD( reg, AHBCFG_QUANTUM_SIZE_MASK, 4 )

#define SET_AHBCFG_QUANTUM_SIZE( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_QUANTUM_SIZE_MASK, 4 )

#define AHBCFG_DEFAULT_MASTER_MASK 0x00000700U

#define GET_AHBCFG_DEFAULT_MASTER( reg ) \
  GET_FIELD( reg, AHBCFG_DEFAULT_MASTER_MASK, 8 )

#define SET_AHBCFG_DEFAULT_MASTER( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_DEFAULT_MASTER_MASK, 8 )

#define AHBCFG_EP1_MASK 0x00007000U

#define GET_AHBCFG_EP1( reg ) \
  GET_FIELD( reg, AHBCFG_EP1_MASK, 12 )

#define SET_AHBCFG_EP1( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_EP1_MASK, 12 )

#define AHBCFG_EP2_MASK 0x00070000U

#define GET_AHBCFG_EP2( reg ) \
  GET_FIELD( reg, AHBCFG_EP2_MASK, 16 )

#define SET_AHBCFG_EP2( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_EP2_MASK, 16 )

#define AHBCFG_EP3_MASK 0x00700000U

#define GET_AHBCFG_EP3( reg ) \
  GET_FIELD( reg, AHBCFG_EP3_MASK, 20 )

#define SET_AHBCFG_EP3( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_EP3_MASK, 20 )

#define AHBCFG_EP4_MASK 0x07000000U

#define GET_AHBCFG_EP4( reg ) \
  GET_FIELD( reg, AHBCFG_EP4_MASK, 24 )

#define SET_AHBCFG_EP4( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_EP4_MASK, 24 )

#define AHBCFG_EP5_MASK 0x70000000U

#define GET_AHBCFG_EP5( reg ) \
  GET_FIELD( reg, AHBCFG_EP5_MASK, 28 )

#define SET_AHBCFG_EP5( reg, val ) \
  SET_FIELD( reg, val, AHBCFG_EP5_MASK, 28 )

/* I2S */
static volatile lpc_i2s * const lpc176x_i2s = (lpc_i2s *) I2S_BASE_ADDR;

/* ADC */
#define ADC_CR_SEL( val ) BSP_FLD32( val, 0, 7 )
#define ADC_CR_SEL_GET( val ) BSP_FLD32GET( val, 0, 7 )
#define ADC_CR_SEL_SET( reg, val ) BSP_FLD32SET( reg, val, 0, 7 )
#define ADC_CR_CLKDIV( val ) BSP_FLD32( val, 8, 15 )
#define ADC_CR_CLKDIV_GET( reg ) BSP_FLD32GET( reg, 8, 15 )
#define ADC_CR_CLKDIV_SET( reg, val ) BSP_FLD32SET( reg, val, 8, 15 )
#define ADC_CR_BURST BSP_BIT32( 16 )
#define ADC_CR_CLKS( val ) BSP_FLD32( val, 17, 19 )
#define ADC_CR_PDN BSP_BIT32( 21 )
#define ADC_CR_START( val ) BSP_FLD32( val, 24, 26 )
#define ADC_CR_EDGE BSP_BIT32( 27 )

#define ADC_DR_VALUE( reg ) BSP_FLD32GET( reg, 6, 15 )
#define ADC_DR_OVERRUN BSP_BIT32( 30 )
#define ADC_DR_DONE BSP_BIT32( 31 )

/* DAC */
#define DAC_STEPS 1024
#define DAC_VALUE( val ) BSP_FLD32( val, 6, 15 )
#define DAC_BIAS BSP_BIT32( 16 )

#endif /* LIBBSP_ARM_LPC176X_H */
