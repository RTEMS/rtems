/*
 * Philips LPC22XX/LPC21xx BSP header file
 *
 * by Ray,Xu <Rayx.cn@gmail.com>
 *  
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
*/
#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>


#define CONFIG_ARM_CLK 60000000L 
/* cclk=cco/(2*P) */
/* cco = cclk*2*P 	*/

#define	LPC22xx_Fcclk	CONFIG_ARM_CLK  /* system clk frequecy,<=60Mhz, defined in system configuration */

/* Fcco 156M~320Mhz*/
#define	LPC22xx_Fcclk	CONFIG_ARM_CLK  /* system clk frequecy,<=60Mhz, defined in system configuration */
#define LPC22xx_Fcco	LPC22xx_Fcclk * 4
#define LPC22xx_Fpclk	(LPC22xx_Fcclk /4) *1  /*VPB clk frequency,1,1/2,1/4 times of Fcclk */



/* Fcclk range: 10MHz ~ MCU allowed frequency */
#define Fcclk_MIN	    10000000L
#define Fcclk_MAX	    60000000L

/* Fcco range: 156MHz ~ 320MHz */
#define Fcco_MIN            156000000L
#define Fcco_MAX            320000000L

#define PLLFEED_DATA1       0xAA
#define PLLFEED_DATA2       0x55

/* PLL PLLCON register bit descriptions */
#define PLLCON_ENABLE_BIT   0
#define PLLCON_CONNECT_BIT  1

/* PLL PLLSTAT register bit descriptions */
#define PLLSTAT_ENABLE_BIT  8
#define PLLSTAT_CONNECT_BIT 9
#define PLLSTAT_LOCK_BIT    10

/* PM Peripheral Type */
#define PC_TIMER0	    0x2
#define PC_TIMER1	    0x4
#define PC_UART0	    0x8
#define PC_UART1	    0x10
#define PC_PWM0		    0x20
#define PC_I2C		    0x80
#define PC_SPI0		    0x100
#define PC_RTC		    0x200

// OSC [Hz]
#define FOSC 	          11059200
// Core clk [Hz]
#define FCCLK	          FOSC<<2
/**
* help file 
*/
/* 系统设置, Fosc、Fcclk、Fcco、Fpclk必须定义*/
#define Fosc            11059200                    //晶振频率,10MHz~25MHz，应当与实际一至
#define Fcclk           (Fosc << 2)                  //系统频率，必须为Fosc的整数倍(1~32)，且<=60MHZ
#define Fcco            (Fcclk <<2)                 //CCO频率，必须为Fcclk的2、4、8、16倍，范围为156MHz~320MHz
#define Fpclk           (Fcclk >>2) * 1             //VPB时钟频率，只能为(Fcclk / 4)的1 ~ 4倍
#define M                Fcclk / Fosc;
#define P_min         Fcco_MIN / (2*Fcclk) + 1;
#define P_max        Fcco_MAX / (2*Fcclk);



#define  UART_BPS	115200

// Time Precision time [us]
#define TIMER_PRECISION   10

// I2C Speed [bit/s]
#define I2CSPEED          20000         // 20 Kbit/s

// Uarts buffers size
#define RXBUFSIZE         32
#define TXBUFSIZE         32

// SPI Speed [bit/s]
#define SPISPEED          1500000       // 1.5 Mbit/s
// SPI EEPROM CS pin (SSEL is not suitable for CS, because is used by SPI module for multi master SPI interface)
#define SPI_CS_PIN        P0_13
#define SPI_CS_PIN_FUNC   PINSEL0_bit.SPI_CS_PIN

// Flash definition
//#define FLASH_SIZE              (0x200000-FLASH_BOOT)   // Total area of Flash region in words 8 bit
#define FLASH_SIZE              (0x80000-FLASH_BOOT)   // Total area of Flash region in words 8 bit
//#define FLASH_SIZE              (0x80000-FLASH_BOOT)      // Total area of Flash region in words 8 bit
#define FLASH_BEGIN             0x80000000
#define FLASH_BASE              (FLASH_BEGIN+FLASH_BOOT)   //First 0x8000 bytes reserved for boot loader etc.

// SRAM definition
#define SRAM_SIZE               0x100000                  // Total area of Flash region in words 8 bit
#define SRAM_BASE               0x81000000                //First 0x8000 bytes reserved for boot loader etc.

// CS8900A definition
#define CS8900A_BASE            0x82000000                //
// RTL8019AS definition
#define RTL8019AS_BASE            0x82000000                //

struct rtems_bsdnet_ifconfig;
int cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config,
                          int                          attaching);

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2

extern rtems_configuration_table BSP_Configuration;

/*
 * Network driver configuration
 */
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	cs8900_driver_attach

/*
 *  BSP Configuration Default Overrides
 */
#define BSP_ZERO_WORKSPACE_AUTOMATICALLY TRUE

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */
