/*
 *  PMC901 specific configuration values
 *
 *  $Id$
 */

#ifndef  __PMC901_CONFIG__
#define __PMC901_CONFIG__
/* The following define the PMC960 bus regions */
/* Bus configuration */
#define RP_CONFIG_REGS     I960RP_BUS_WIDTH(32)
#define FLASH              I960RP_BUS_WIDTH(8)
#define DRAM               I960RP_BUS_WIDTH(32)
#define UART_LED           I960RP_BUS_WIDTH(8)
#define DEFAULT            I960RP_BUS_WIDTH(32)

/* Region Configuration */
#define  REGION_0_CONFIG      RP_CONFIG_REGS
#define  REGION_2_CONFIG      DEFAULT
#define  REGION_4_CONFIG      DEFAULT
#define  REGION_6_CONFIG      DEFAULT
#define  REGION_8_CONFIG      DEFAULT
#define  REGION_A_CONFIG      DRAM
#define  REGION_C_CONFIG      UART_LED
#define  REGION_E_CONFIG      DEFAULT
/* #define  REGION_BOOT_CONFIG   (FLASH | I960RP_ZBYTE_ORDER)*/
#define  REGION_BOOT_CONFIG   (DRAM)

/* #define  DRAM_BASE	0xfed00000 */
#define  DRAM_BASE	0xa0000000

#endif
