/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  S. Kate Feng 2003-2007 : Modified it to support the mvme5500 BSP.
 *
 *  Modified for the 'beatnik' BSP by T. Straumann, 2005-2007.
 */
#ifndef LIBBSP_BEATNIK_BSP_H
#define LIBBSP_BEATNIK_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <libcpu/io.h>
#include <rtems/clockdrv.h>
#include <bsp/vectors.h>

#ifdef __cplusplus
  extern "C" {
#endif

/* Board type */
typedef enum {
	Unknown = 0,
	MVME5500,
	MVME6100
} BSP_BoardType;

BSP_BoardType
BSP_getBoardType();

/* Discovery Version */

typedef enum {
	unknown    = 0,
	GT_64260_A,		/* Revision 0x10 */
	GT_64260_B,	    /* Revision 0x20 */
	MV_64360,
} DiscoveryVersion;

/* Determine the type of discovery chip on this board; info
 * is cached and repeated calls just return the cached value.
 *
 * If a non-zero argument is passed, the routine panics
 * (BSP_panic) if no recognized bridge is found;
 */
 
DiscoveryVersion
BSP_getDiscoveryVersion(int assertion);

/*
 *  confdefs.h overrides for this BSP:
 *   - Interrupt stack space is not minimum if defined.
 */
#define BSP_INTERRUPT_STACK_SIZE          (16 * 1024)

/*
 *  base address definitions for several devices
 */
#define BSP_MV64x60_BASE    		(0xf1000000)
#define BSP_MV64x60_DEV1_BASE		(0xf1100000)
#define BSP_UART_IOBASE_COM1 		((BSP_MV64x60_DEV1_BASE)+0x20000)
#define BSP_UART_IOBASE_COM2 		((BSP_MV64x60_DEV1_BASE)+0x21000)
#define BSP_UART_USE_SHARED_IRQS

#define BSP_NVRAM_BASE_ADDR			(0xf1110000)
#define BSP_NVRAM_END_ADDR			(0xf1117fff)
#define BSP_NVRAM_RTC_START			(0xf1117ff8)

#define BSP_NVRAM_BOOTPARMS_START	(0xf1111000)
#define BSP_NVRAM_BOOTPARMS_END		(0xf1111fff)


/* This is only active/used during early init. It defines
 * the hose0 base for the shared/generic pci code.
 * Our own BSP specific pci initialization will then
 * override the PCI configuration (see gt_pci_init.c:BSP_pci_initialize)
 */

#define PCI_CONFIG_ADDR				(BSP_MV64x60_BASE + 0xcf8)
#define PCI_CONFIG_DATA				(BSP_MV64x60_BASE + 0xcfc)

/* our wonderful PCI initialization remaps everything to CPU addresses
 * - before calling BSP_pci_initialize() this is NOT VALID, however
 * and the deprecated inl()/outl() etc won't work!
 */
#define _IO_BASE					0x00000000
/* wonderful MotLoad has the base address as seen from the CPU programmed into config space :-) */
#define PCI_MEM_BASE				0
#define PCI_MEM_BASE_ADJUSTMENT		0
#define PCI_DRAM_OFFSET				0

/* PCI <-> local address mapping - no sophisticated windows
 * (i.e., no support for cached regions etc. you read a BAR
 * from config space and that's 1:1 where the CPU sees it).
 * Our memory is mapped 1:1 to PCI also.
 */
#define BSP_PCI2LOCAL_ADDR(a) ((uint32_t)(a))
#define BSP_LOCAL2PCI_ADDR(a) ((uint32_t)(a))

#define BSP_CONFIG_NUM_PCI_CACHE_SLOTS	32

#define BSP_CONSOLE_PORT			BSP_UART_COM1
#define BSP_UART_BAUD_BASE			115200

/* I2C Devices */
/* Note that the i2c addresses stated in the manual are
 * left-shifted by one bit.
 */
#define BSP_VPD_I2C_ADDR			(0xA8>>1)		/* the VPD EEPROM  */
#define BSP_USR_I2C_ADDR			(0xAA>>1)		/* the user EEPROM */
#define BSP_THM_I2C_ADDR			(0x90>>1)		/* the DS1621 temperature sensor & thermostat */

#define BSP_I2C_BUS_DESCRIPTOR		gt64260_i2c_bus_descriptor

#define BSP_I2C_BUS0_NAME             "/dev/i2c0"

#define BSP_I2C_VPD_EEPROM_NAME       "vpd-eeprom"
#define BSP_I2C_USR_EEPROM_NAME       "usr-eeprom"
#define BSP_I2C_DS1621_NAME           "ds1621"
#define BSP_I2C_THM_NAME              BSP_I2C_DS1621_NAME
#define BSP_I2C_DS1621_RAW_NAME       "ds1621-raw"

#define	BSP_I2C_VPD_EEPROM_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_VPD_EEPROM_NAME)
#define	BSP_I2C_USR_EEPROM_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_USR_EEPROM_NAME)
#define	BSP_I2C_DS1621_DEV_NAME          (BSP_I2C_BUS0_NAME"."BSP_I2C_DS1621_NAME)
#define BSP_I2C_THM_DEV_NAME              BSP_I2C_DS1621_DEV_NAME
#define	BSP_I2C_DS1621_RAW_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_DS1621_RAW_NAME)


/* Initialize the I2C driver and register all devices 
 * RETURNS 0 on success, -1 on error.
 *
 * Access to the VPD and user EEPROMS as well
 * as the ds1621 temperature sensor is possible
 * by means of file nodes
 *
 *   /dev/i2c0.vpd-eeprom   (read-only)
 *   /dev/i2c0.usr-eeprom   (read-write)
 *   /dev/i2c0.ds1621       (read-only; one byte: board-temp in degC)
 *   /dev/i2c0.ds1621-raw   (read-write; transfer bytes to/from the ds1621)
 */
int
BSP_i2c_initialize();

/* Networking; */
#if defined(RTEMS_NETWORKING)
#include <bsp/bsp_bsdnet_attach.h>
#endif

/* NOT FOR PUBLIC USE BELOW HERE */
#define BSP_PCI_HOSE0_MEM_BASE		0x80000000	/* must be aligned to size */
#define BSP_PCI_HOSE0_MEM_SIZE		0x20000000

#define BSP_PCI_HOSE1_MEM_BASE		0xe0000000

#define BSP_DEV_AND_PCI_IO_BASE 	0xf0000000
#define BSP_DEV_AND_PCI_IO_SIZE 	0x10000000

/* maintain coherency between CPU and GT64340 ethernet (& possibly other discovery components) */
#define BSP_RW_PAGE_ATTRIBUTES	TRIV121_ATTR_M

extern unsigned BSP_pci_hose1_bus_base;

void BSP_pci_initialize();

/* Exception Handling */

/* Use a task notepad to attach user exception handler info;
 * may be changed by application startup code (EPICS uses 11)
 */
#define BSP_EXCEPTION_NOTEPAD		14
  
#ifndef ASM

#define outport_byte(port,value) outb(value,port)
#define outport_word(port,value) outw(value,port)
#define outport_long(port,value) outl(value,port)

#define inport_byte(port,value) (value = inb(port))
#define inport_word(port,value) (value = inw(port))
#define inport_long(port,value) (value = inl(port))
/*
 * Vital Board data Start using DATA RESIDUAL
 */
/*
 * Total memory using RESIDUAL DATA
 */
extern unsigned int BSP_mem_size;
/*
 * Start of the heap
 */
extern unsigned int BSP_heap_start;
/*
 * PCI Bus Frequency
 */
extern unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
extern unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
extern unsigned int BSP_time_base_divisor;

extern char BSP_productIdent[20];
extern char BSP_serialNumber[20];

extern char BSP_enetAddr0[7];
extern char BSP_enetAddr1[7];

/*
 * The commandline as passed from the bootloader.
 */
extern char *BSP_commandline_string;


#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

extern rtems_configuration_table  BSP_Configuration;
extern void BSP_panic(char *s);
extern void bsp_reset(void);
extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);

/* clear hostbridge errors
 *
 * enableMCP: whether to enable MCP checkstop / machine check interrupts
 *            on the hostbridge and in HID0.
 *
 *            NOTE: The 5500 and 6100 boards have NO PHYSICAL CONNECTION
 *                  to MCP so 'enableMCP' will always fail!
 *
 * quiet    : be silent
 *
 * RETURNS  : PCI status (hose 0 in byte 0, host 1 in byte 1) and
 *            VME bridge status (upper 16 bits).
 *            Zero if no errors were found.
 */
extern unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet);

/* clear vme bridge errors and return (bridge-dependent) 16-bit status
 *
 * quiet    : be silent
 *
 * RETURNS  : 0 if there were no errors, non-zero, bridge-dependent
 *            16-bit error status on error.
 *
 */
extern unsigned short
(*_BSP_clear_vmebridge_errors)(int);


#endif

#ifdef __cplusplus
  }
#endif

#endif
