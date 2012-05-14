/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Adapted for the mvme3100 BSP by T. Straumann, 2007.
 */
#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <libcpu/io.h>
#include <rtems/clockdrv.h>
#include <bsp/vectors.h>

/*
 *  confdefs.h overrides for this BSP:
 */

#define BSP_INTERRUPT_STACK_SIZE          (16 * 1024)

/*
 * diagram illustrating the role of the configuration
 * constants
 *  PCI_MEM_WIN0:        CPU starting addr where PCI memory space is visible
 *  PCI_MEM_BASE:        CPU address of PCI mem addr. zero. (regardless of this
 *                       address being 'visible' or not!).
 * _VME_A32_WIN0_ON_PCI: PCI starting addr of the 1st window to VME
 * _VME_A32_WIN0_ON_VME: VME address of that same window
 *
 * AFAIK, only PreP boards have a non-zero PCI_MEM_BASE (i.e., an offset between
 * CPU and PCI addresses). The mvme2300 'ppcbug' firmware configures the PCI
 * bus using PCI base addresses! I.e., drivers need to add PCI_MEM_BASE to
 * the base address read from PCI config.space in order to translate that
 * into a CPU address.
 *
 * NOTE: VME addresses should NEVER be translated using these constants!
 *       they are strictly for BSP internal use. Drivers etc. should use
 *       the translation routines int VME.h (BSP_vme2local_adrs/BSP_local2vme_adrs).
 *
 *           CPU ADDR                  PCI_ADDR                                VME ADDR
 *
 *           00000000                  XXXXXXXX                                XXXXXXXX
 *    ^  ^   ........
 *    |  |
 *    |  |  e.g., RAM                  XXXXXXXX
 *    |  |                                                                     00000000
 *    |  |  .........                                                          ^
 *    |  |            (possible offset                                         |
 *    |  |             between pci and XXXXXXXX                                | ......
 *    |  |             cpu addresses)                                          |
 *    |  v                                                                     |
 *    |  PCI_MEM_BASE  ------------->  00000000 ---------------                |
 *    |     ........                   ........               ^                |
 *    |                                invisible              |                |
 *    |     ........                   from CPU               |                |
 *    v                                                       |                |
 *       PCI_MEM_WIN0 =============  first visible PCI addr   |                |
 *                                                            |                |
 *        pci devices   pci window                            |                |
 *       visible here                                         v                v
 *                      mapped by   ========== _VME_A32_WIN0_ON_PCI =======  _VME_A32_WIN0_ON_VME
 *                                                 vme window
 *        VME devices   hostbridge                 mapped by
 *       visible here                              universe
 *                    =====================================================
 *
 */

/* fundamental addresses for BSP (CHRPxxx and PREPxxx are from libcpu/io.h) */
#define	_IO_BASE            0xe0000000 /* Motload's PCI IO base */
#define	_ISA_MEM_BASE		CHRP_ISA_MEM_BASE
/* address of our ram on the PCI bus   */
#define	PCI_DRAM_OFFSET		CHRP_PCI_DRAM_OFFSET
/* offset of pci memory as seen from the CPU */
#define PCI_MEM_BASE		0
/* where (in CPU addr. space) does the PCI window start */
#define PCI_MEM_WIN0		0x80000000

/*
 *  Base address definitions for several devices
 */

#define BSP_OPEN_PIC_BASE_OFFSET 0x40000
#define BSP_OPEN_PIC_BIG_ENDIAN

#define BSP_8540_CCSR_BASE   (0xe1000000)

#define BSP_UART_IOBASE_COM1 (BSP_8540_CCSR_BASE+0x4500)
#define BSP_UART_IOBASE_COM2 (BSP_8540_CCSR_BASE+0x4600)
#define PCI_CONFIG_ADDR      (BSP_8540_CCSR_BASE+0x8000)
#define PCI_CONFIG_DATA      (BSP_8540_CCSR_BASE+0x8004)
#define PCI_CONFIG_WR_ADDR( addr, val ) out_be32((unsigned int*)(addr), (val))

#define BSP_CONSOLE_PORT	BSP_UART_COM1
#define BSP_UART_BAUD_BASE	(-9600) /* use existing divisor to determine clock rate */
#define BSP_UART_USE_SHARED_IRQS

#define BSP_MVME3100_IRQ_DETECT_REG ((volatile uint8_t *)0xe2000007)

/* I2C Devices */
/* Note that the i2c addresses stated in the manual are
 * left-shifted by one bit.
 */
#define BSP_VPD_I2C_ADDR			(0xA8>>1)		/* the VPD EEPROM  */
#define BSP_USR0_I2C_ADDR			(0xA4>>1)		/* the 1st user EEPROM */
#define BSP_USR1_I2C_ADDR			(0xA6>>1)		/* the 2nd user EEPROM */
#define BSP_THM_I2C_ADDR			(0x90>>1)		/* the DS1621 temperature sensor & thermostat */
#define BSP_RTC_I2C_ADDR			(0xD0>>1)		/* the DS1375 wall-clock */

#define BSP_I2C_BUS_DESCRIPTOR		mpc8540_i2c_bus_descriptor

#define BSP_I2C_BUS0_NAME             "/dev/i2c0"

#define BSP_I2C_VPD_EEPROM_NAME       "vpd-eeprom"
#define BSP_I2C_USR_EEPROM_NAME       "usr-eeprom"
#define BSP_I2C_USR1_EEPROM_NAME      "usr1-eeprom"
#define BSP_I2C_DS1621_NAME           "ds1621"
#define BSP_I2C_THM_NAME              BSP_I2C_DS1621_NAME
#define BSP_I2C_DS1621_RAW_NAME       "ds1621-raw"
#define BSP_I2C_DS1375_RAW_NAME       "ds1375-raw"
#define BSP_I2C_RTC_RAW_NAME          BSP_I2C_DS1375_RAW_NAME

#define	BSP_I2C_VPD_EEPROM_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_VPD_EEPROM_NAME)
#define	BSP_I2C_USR_EEPROM_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_USR_EEPROM_NAME)
#define	BSP_I2C_USR1_EEPROM_DEV_NAME     (BSP_I2C_BUS0_NAME"."BSP_I2C_USR1_EEPROM_NAME)
#define	BSP_I2C_DS1621_DEV_NAME          (BSP_I2C_BUS0_NAME"."BSP_I2C_DS1621_NAME)
#define BSP_I2C_THM_DEV_NAME              BSP_I2C_DS1621_DEV_NAME
#define	BSP_I2C_DS1621_RAW_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_DS1621_RAW_NAME)
#define	BSP_I2C_DS1375_RAW_DEV_NAME      (BSP_I2C_BUS0_NAME"."BSP_I2C_DS1375_RAW_NAME)

/* Definitions useful for bootloader (netboot); where to find
 * boot/'environment' parameters.
 */
#define BSP_EEPROM_BOOTPARMS_NAME        BSP_I2C_USR1_EEPROM_DEV_NAME
#define BSP_EEPROM_BOOTPARMS_SIZE        1024
#define BSP_EEPROM_BOOTPARMS_OFFSET      0
#define BSP_BOOTPARMS_WRITE_ENABLE()     do { BSP_eeprom_write_enable(); } while (0)
#define BSP_BOOTPARMS_WRITE_DISABLE()    do { BSP_eeprom_write_protect();} while (0)


#ifdef __cplusplus
extern "C" {
#endif
/* Initialize the I2C driver and register all devices
 * RETURNS 0 on success, -1 on error.
 *
 * Access to the VPD and user EEPROMS as well
 * as the ds1621 temperature sensor is possible
 * by means of file nodes
 *
 *   /dev/i2c0.vpd-eeprom   (read-only)
 *   /dev/i2c0.usr-eeprom   (read-write)
 *   /dev/i2c0.usr1-eeprom  (read-write)
 *   /dev/i2c0.ds1621       (read-only; one byte: board-temp in degC)
 *   /dev/i2c0.ds1621-raw   (read-write; transfer bytes to/from the ds1621)
 *   /dev/i2c0.ds1375-raw   (read-write; transfer bytes to/from the ds1375)
 *
 */
int
BSP_i2c_initialize();

/* System Control Register */
#define BSP_MVME3100_SYS_CR				((volatile uint8_t *)0xe2000001)
#define BSP_MVME3100_SYS_CR_RESET_MSK		(7<<5)
#define BSP_MVME3100_SYS_CR_RESET			(5<<5)
#define BSP_MVME3100_SYS_CR_EEPROM_WP		(1<<1)
#define BSP_MVME3100_SYS_CR_TSTAT_MSK		(1<<0)

/* LED support */
#define BSP_MVME3100_SYS_IND_REG		((volatile uint8_t *)0xe2000002)
#define BSP_LED_BRD_FAIL					(1<<0)
#define BSP_LED_USR1						(1<<1)
#define BSP_LED_USR2						(1<<2)
#define BSP_LED_USR3						(1<<3)

/* Flash CSR   */
#define BSP_MVME3100_FLASH_CSR			((volatile uint8_t *)0xe2000003)
#define BSP_MVME3100_FLASH_CSR_FLASH_RDY	(1<<0)
#define BSP_MVME3100_FLASH_CSR_FBT_BLK_SEL	(1<<1)
#define BSP_MVME3100_FLASH_CSR_F_WP_HW		(1<<2)
#define BSP_MVME3100_FLASH_CSR_F_WP_SW		(1<<3)
#define BSP_MVME3100_FLASH_CSR_MAP_SEL		(1<<4)

/* Phy interrupt detect */
#define BSP_MVME3100_IRQ_DETECT_REG		((volatile uint8_t *)0xe2000007)

/* Atomically set bits in a sys-register; The bits set in 'mask'
 * are set in the register others; are left unmodified.
 *
 * RETURNS: old state.
 *
 * NOTE   : since BSP_setSysReg( reg, 0 ) does not make
 *          any changes this call may be used
 *          to read the current status w/o modifying it.
 */
uint8_t
BSP_setSysReg(volatile uint8_t *r, uint8_t mask);

/* Atomically clear bits in a sys-register; The bits set in 'mask'
 * are cleared in the register; others are left unmodified.
 *
 * RETURNS: old state.
 *
 * NOTE   : since BSP_clrSysReg( reg, 0 ) does not make
 *          any changes this call may be used
 *          to read the current status w/o modifying it.
 */

uint8_t
BSP_clrSysReg(volatile uint8_t *r, uint8_t mask);

/* Convenience wrappers around BSP_setSysReg()/BSP_clrSysReg() */

/* Set write-protection for all EEPROM devices
 * RETURNS: old status
 */
uint8_t
BSP_eeprom_write_protect();

/* Disengage write-protection for all EEPROM devices
 * RETURNS: old status
 */
uint8_t
BSP_eeprom_write_enable();

/* Set LEDs that have their bit set in the mask
 *
 * RETURNS: old status.
 *
 * NOTE   : since BSP_setLEDs( 0 ) does not make
 *          any changes this call may be used
 *          to read the current status w/o modifying it.
 */
uint8_t
BSP_setLEDs(uint8_t mask);

/* Clear LEDs that have their bit set in the mask
 *
 * RETURNS: old status
 *
 * NOTE:  : see above (BSP_setLEDs)
 */
uint8_t
BSP_clrLEDs(uint8_t mask);

#if 0
#define outport_byte(port,value) outb(value,port)
#define outport_word(port,value) outw(value,port)
#define outport_long(port,value) outl(value,port)

#define inport_byte(port,value) (value = inb(port))
#define inport_word(port,value) (value = inw(port))
#define inport_long(port,value) (value = inl(port))
#endif

/*
 * Total memory using RESIDUAL DATA
 */
extern unsigned int BSP_mem_size;
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
/*
 * The commandline as passed from the bootloader.
 */
extern char *BSP_commandline_string;

#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

extern rtems_configuration_table  BSP_Configuration;
extern void BSP_panic(char *s);
/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2))); */
extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);

/* clear hostbridge errors
 *
 * NOTE: The routine returns always (-1) if 'enableMCP==1'
 *       [semantics needed by libbspExt] if the MCP input is not wired.
 *       It returns and clears the error bits of the PCI status register.
 *       MCP support is disabled because:
 *         a) the 2100 has no raven chip
 *         b) the raven (2300) would raise machine check interrupts
 *            on PCI config space access to empty slots.
 */
extern unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet);
extern void BSP_motload_pci_fixup();

struct rtems_bsdnet_ifconfig;

int
rtems_tsec_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME   "tse1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_tsec_attach

#ifdef __cplusplus
}
#endif

#endif
