/**
 * @file
 *
 * @ingroup RTEMSBSPsARMRTL22XX
 *
 * @brief Global BSP definitions.
 */

/*
 * Philips LPC22XX/LPC21xx BSP header file
 *
 * by Ray,Xu <Rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/
#ifndef LIBBSP_ARM_RTL22XX_BSP_H
#define LIBBSP_ARM_RTL22XX_BSP_H

/**
 * @defgroup RTEMSBSPsARMRTL22XX RTL22XX
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief RTL22XX Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

#define CONFIG_ARM_CLK 60000000L
/* cclk=cco/(2*P) */
/* cco = cclk*2*P 	*/

/** @brief system clk frequecy,<=60Mhz, defined in system configuration */
#define	LPC22xx_Fcclk	CONFIG_ARM_CLK

/* Fcco 156M~320Mhz*/
/** @brief system clk frequecy,<=60Mhz, defined in system configuration */
#define	LPC22xx_Fcclk	CONFIG_ARM_CLK
#define LPC22xx_Fcco	LPC22xx_Fcclk * 4
/** @brief VPB clk frequency,1,1/2,1/4 times of Fcclk */
#define LPC22xx_Fpclk	(LPC22xx_Fcclk /4) *1



/**
 * @name Fcclk range: 10MHz ~ MCU allowed frequency
 * @{
 */

#define Fcclk_MIN	    10000000L
#define Fcclk_MAX	    60000000L

/** @} */

/**
 * @name Fcco range: 156MHz ~ 320MHz
 * @{
 */

#define Fcco_MIN            156000000L
#define Fcco_MAX            320000000L

/** @} */

#define PLLFEED_DATA1       0xAA
#define PLLFEED_DATA2       0x55

/**
 * @name PLL PLLCON register bit descriptions
 * @{
 */

#define PLLCON_ENABLE_BIT   0
#define PLLCON_CONNECT_BIT  1

/** @} */

/**
 * @name PLL PLLSTAT register bit descriptions
 * @{
 */

#define PLLSTAT_ENABLE_BIT  8
#define PLLSTAT_CONNECT_BIT 9
#define PLLSTAT_LOCK_BIT    10

/** @} */

/**
 * @name PM Peripheral Type
 * @{
 */

#define PC_TIMER0	    0x2
#define PC_TIMER1	    0x4
#define PC_UART0	    0x8
#define PC_UART1	    0x10
#define PC_PWM0		    0x20
#define PC_I2C		    0x80
#define PC_SPI0		    0x100
#define PC_RTC		    0x200

/** @} */

/** @brief OSC [Hz] */
#define FOSC 	          11059200
/** @brief Core clk [Hz] */
#define FCCLK	          FOSC<<2

/**
 * @name System Configure
 * @{
 */

/** @brief osc freq,10MHz~25MHz, change to a real one if needed */
#define Fosc    11059200
/** @brief system freq 2^n time of  Fosc(1~32) <=60MHZ */
#define Fcclk   (Fosc << 2)
/** @brief CCO freq 2,4,8,16 time of Fcclk 156MHz~320MHz */
#define Fcco    (Fcclk <<2)
/** @brief VPB freq only(Fcclk / 4) 1~4 */
#define Fpclk   (Fcclk >>2) * 1
/* This was M.  That is a BAD BAD public constant.  I renamed it to
 * JOEL_M so it wouldn't conflict with user code.  If you can find
 * a better name, fix this.  But nothing I found uses it.
 */

/** @} */

#define JOEL_M       Fcclk / Fosc
#define P_min   Fcco_MIN / (2*Fcclk) + 1;
#define P_max   Fcco_MAX / (2*Fcclk);

#define  UART_BPS	115200

/** @brief Time Precision time [us] */
#define TIMER_PRECISION   10

/** @brief I2C Speed [bit/s] */
#define I2CSPEED          20000         // 20 Kbit/s

/**
 * @name Uarts buffers size
 * @{
 */

#define RXBUFSIZE         32
#define TXBUFSIZE         32

/** @} */

/** @brief SPI Speed [bit/s] */
#define SPISPEED          1500000       // 1.5 Mbit/s
/** @brief SPI EEPROM CS pin
 *
 *  (SSEL is not suitable for CS, because is used by SPI module for multi master SPI interface)
 */
#define SPI_CS_PIN        P0_13
#define SPI_CS_PIN_FUNC   PINSEL0_bit.SPI_CS_PIN

/**
 * @name Flash definition
 * @{
 */

//#define RTL22XX_FLASH_SIZE      (0x200000-RTL22XX_FLASH_BOOT)   // Total area of Flash region in words 8 bit
/** @brief Total area of Flash region in words 8 bit */
#define RTL22XX_FLASH_SIZE        (0x80000-RTL22XX_FLASH_BOOT)
//#define RTL22XX_FLASH_SIZE      (0x80000-RTL22XX_FLASH_BOOT)      // Total area of Flash region in words 8 bit
#define RTL22XX_FLASH_BEGIN       0x80000000
/** @brief First 0x8000 bytes reserved for boot loader etc. */
#define RTL22XX_FLASH_BASE        (RTL22XX_FLASH_BEGIN+RTL22XX_FLASH_BOOT)

/** @} */

/**
 * @name SRAM definition
 * @{
 */

/** @brief Total area of Flash region in words 8 bit */
#define SRAM_SIZE         0x100000
/** @brief First 0x8000 bytes reserved for boot loader etc. */
#define SRAM_BASE         0x81000000

/** @} */

/** @brief CS8900A definition */
#define CS8900A_BASE      0x82000000
/** @brief RTL8019AS definition */
#define RTL8019AS_BASE    0x82000000

struct rtems_bsdnet_ifconfig;
int cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config,
                          int                          attaching);

/**
 * @name Network driver configuration
 * @{
 */

#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	cs8900_driver_attach

/** @} */

/*
 * Prototypes for methods used across file boundaries in the BSP.
 */
extern void  UART0_Ini(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BSP_H */
