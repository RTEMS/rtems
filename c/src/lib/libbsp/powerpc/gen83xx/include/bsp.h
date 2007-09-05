/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains board specific definitions                   |
\*===============================================================*/

#ifndef __GEN83xx_BSP_h
#define __GEN83xx_BSP_h

/*
 * distinguish board characteristics
 */
/*
 * for Freescale MPC8349 EAMDS
 */
#if defined(MPC8349EAMDS)
/*
 * two DUART channels supported
 */
#define GEN83xx_DUART_AVAIL_MASK 0x03

/* we need the low level initialization in start.S*/
#define NEED_LOW_LEVEL_INIT
/*
 * clocking infos
 */
#define BSP_CLKIN_FRQ 66000000L
#define BSP_SYSPLL_MF 4 /* FIXME: derive from clock register */

/*
 * Reset configuration words
 */
#define RESET_CONF_WRD_L (RCWLR_LBIUCM_1_1 |	\
			  RCWLR_DDRCM_1_1  |	\
			  RCWLR_SPMF(4)    |	\
			  RCWLR_COREPLL(4))

#define RESET_CONF_WRD_H (RCWHR_PCI_HOST     |	\
			  RCWHR_PCI_32       |	\
			  RCWHR_PCI1ARB_EN   |	\
			  RCWHR_PCI2ARB_EN   |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_TSEC1M_GMII  |	\
			  RCWHR_TSEC2M_GMII  |	\
			  RCWHR_ENDIAN_BIG   |	\
			  RCWHR_LALE_NORM    |	\
			  RCWHR_LDP_PAR)
/*
 * for JPK HSC_CM01
 */
#elif defined(HSC_CM01)
/*
 * one DUART channel (UART1) supported
 */
#define GEN83xx_DUART_AVAIL_MASK 0x01

/* we need the low level initialization in start.S*/
#define NEED_LOW_LEVEL_INIT
/*
 * clocking infos
 */
#define BSP_CLKIN_FRQ 66000000L
#define BSP_SYSPLL_MF 4 /* FIXME: derive from clock register */

/*
 * Reset configuration words
 */
#define RESET_CONF_WRD_L (RCWLR_LBIUCM_1_1 |	\
			  RCWLR_DDRCM_1_1  |	\
			  RCWLR_SPMF(4)    |	\
			  RCWLR_COREPLL(4))

#define RESET_CONF_WRD_H (RCWHR_PCI_HOST     |	\
			  RCWHR_PCI_32       |	\
			  RCWHR_PCI1ARB_EN   |	\
			  RCWHR_PCI2ARB_EN   |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_TSEC1M_RGMII |	\
			  RCWHR_TSEC2M_GMII  |	\
			  RCWHR_ENDIAN_BIG   |	\
			  RCWHR_LALE_NORM    |	\
			  RCWHR_LDP_PAR)
#else
#error "board type not defined"
#endif

/*
 * for JPK HSC_CM01 and freescale MPC8349EAMDS
 */
#if defined(MPC8349EAMDS) || defined(HSC_CM01)
/*
 * address range definitions
 */
/* ROM definitions (8 MB, mirrored multiple times) */
#define	ROM_START	0xFE000000
#define ROM_SIZE        0x02000000
#define	ROM_END		(ROM_START+ROM_SIZE-1)
#define	BOOT_START      ROM_START
#define	BOOT_END        ROM_END

/* SDRAM definitions (256 MB) */
#define	RAM_START       0x00000000
#define RAM_SIZE        0x10000000
#define	RAM_END		(RAM_START+RAM_SIZE-1)


/* working internal memory map base address */
#define	IMMRBAR         0xE0000000

/*
 * working values for various registers, used in start/start.S
 */
/*
 * Local Access Windows
 * FIXME: decode bit settings 
 */
#define LBLAWBAR0_VAL  0xFE000000
#define LBLAWAR0_VAL   0x80000016
#define LBLAWBAR1_VAL  0xF8000000
#define LBLAWAR1_VAL   0x8000000E
#define LBLAWBAR2_VAL  0xF0000000
#define LBLAWAR2_VAL   0x80000019
#define DDRLAWBAR0_VAL 0x00000000
#define DDRLAWAR0_VAL  0x8000001B
/*
 * Local Bus (Memory) Controller
 * FIXME: decode bit settings 
 */
#define BR0_VAL 0xFE001001
#define OR0_VAL 0xFF806FF7
#define BR1_VAL 0xF8000801
#define OR1_VAL 0xFFFFE8F0
#define BR2_VAL 0xF0001861
#define OR2_VAL 0xFC006901
/*
 * SDRAM registers
 * FIXME: decode bit settings 
 */
#define MRPTR_VAL 0x20000000
#define LSRT_VAL  0x32000000
#define LSDMR_VAL 0x4062D733
#define LCRR_VAL  0x80000004

/*
 * DDR-SDRAM registers
 * FIXME: decode bit settings 
 */
#define CS2_BNDS_VAL                 0x00000007
#define CS3_BNDS_VAL                 0x0008000F
#define CS2_CONFIG_VAL               0x80000101
#define CS3_CONFIG_VAL               0x80000101
#define TIMING_CFG_1_VAL             0x36333321
#define TIMING_CFG_2_VAL             0x00000800
#define DDR_SDRAM_CFG_VAL            0xC2000000
#define DDR_SDRAM_MODE_VAL           0x00000022
#define DDR_SDRAM_INTTVL_VAL         0x045B0100
#define DDR_SDRAM_CLK_CNTL_VAL       0x00000000

#else
#error "board type not defined"
#endif

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#include "bspopts.h"

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;
/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
 *  Stuff for Time Test 27
 */
#define MUST_WAIT_FOR_INTERRUPT 0

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
#define BSP_UART1_MINOR 0
#define BSP_UART2_MINOR 1

/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * indicate, that BSP has no IDE driver
 */
#undef RTEMS_BSP_HAS_IDE_DRIVER

/*
 * How many libio files we want
 */
#define BSP_LIBIO_MAX_FDS       20

/* misc macros */
#define BSP_ARRAY_CNT(arr) (sizeof(arr)/sizeof(arr[0]))

/* functions */

void bsp_cleanup(void);
rtems_status_code bsp_register_i2c(void);

/* console modes (only termios) */
#ifdef  PRINTK_MINOR
#undef  PRINTK_MINOR
#endif
#define PRINTK_MINOR BSP_UART1_MINOR

#define SINGLE_CHAR_MODE
#define UARTS_USE_TERMIOS_INT   1

/*
 *  Convert decrement value to tenths of microsecnds (used by
 *  shared timer driver).
 *
 *    + CPU has a csb_clock bus,
 *    + There are 4 bus cycles per click
 *    + We return value in 1/10 microsecond units.
 *   Modified following equation to integer equation to remove
 *   floating point math.
 *   (int) ((float)(_value) / ((XLB_CLOCK/1000000 * 0.1) / 4.0))
 */
#define BSP_CSB_CLK_FRQ (BSP_CLKIN_FRQ * BSP_SYSPLL_MF)
#define BSP_Convert_decrementer( _value ) \
  (int) (((_value) * 4000) / (BSP_CSB_CLK_FRQ/10000))

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int BSP_tsec_attach(struct rtems_bsdnet_ifconfig *config,int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"tsec1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	BSP_tsec_attach

#define RTEMS_BSP_NETWORK_DRIVER_NAME2	"tsec2"

/*
 * i2c EEPROM device name
 */
#define RTEMS_BSP_I2C_EEPROM_DEVICE_NAME "eeprom"
#define RTEMS_BSP_I2C_EEPROM_DEVICE_PATH "/dev/i2c1.eeprom"

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* GEN83xx */
