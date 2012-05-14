/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <libcpu/io.h>
#include <rtems/clockdrv.h>
#include <bsp/vectors.h>
  
#ifdef qemu
#include <rtems/bspcmdline.h>
#endif
  
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  confdefs.h overrides for this BSP:
 */

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
#if defined(mvme2100)
#define	_IO_BASE		CHRP_ISA_IO_BASE
#define	_ISA_MEM_BASE		CHRP_ISA_MEM_BASE
/* address of our ram on the PCI bus   */
#define	PCI_DRAM_OFFSET		CHRP_PCI_DRAM_OFFSET
/* offset of pci memory as seen from the CPU */
#define PCI_MEM_BASE		0
/* where (in CPU addr. space) does the PCI window start */
#define PCI_MEM_WIN0		0x80000000

#else
#define	_IO_BASE		PREP_ISA_IO_BASE
#define	_ISA_MEM_BASE		PREP_ISA_MEM_BASE
#ifndef qemu
/* address of our ram on the PCI bus   */
#define	PCI_DRAM_OFFSET		PREP_PCI_DRAM_OFFSET
/* offset of pci memory as seen from the CPU */
#define PCI_MEM_BASE		PREP_ISA_MEM_BASE
#define PCI_MEM_WIN0		0
#else
#define	PCI_DRAM_OFFSET		0
#define PCI_MEM_BASE		0
#define PCI_MEM_WIN0		PREP_ISA_MEM_BASE
#endif
#endif


/*
 *  Base address definitions for several devices
 *
 *  MVME2100 is very similar but has fewer devices and uses on-CPU EPIC
 *  implementation of OpenPIC controller.  It also cannot be probed to
 *  find out what it is which is VERY different from other Motorola boards.
 */

#if defined(mvme2100)
#define BSP_UART_IOBASE_COM1 ((_IO_BASE)+0x01e10000)
/* #define BSP_UART_IOBASE_COM1     (0xffe10000) */
#define BSP_OPEN_PIC_BASE_OFFSET 0x40000

#define MVME_HAS_DEC21140
#else
#define BSP_UART_IOBASE_COM1 ((_IO_BASE)+0x3f8)
#define BSP_UART_IOBASE_COM2 ((_IO_BASE)+0x2f8)

#if ! defined(qemu)
#define BSP_KBD_IOBASE       ((_IO_BASE)+0x60)
#define BSP_VGA_IOBASE       ((_IO_BASE)+0x3c0)
#endif

#if defined(mvme2300)
#define MVME_HAS_DEC21140
#endif
#endif

#define BSP_CONSOLE_PORT	BSP_UART_COM1
#define BSP_UART_BAUD_BASE	115200

#if defined(MVME_HAS_DEC21140)
struct rtems_bsdnet_ifconfig;
#define RTEMS_BSP_NETWORK_DRIVER_NAME "dc1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_dec21140_driver_attach
extern int rtems_dec21140_driver_attach();
#endif

#ifdef qemu
#define RTEMS_BSP_NETWORK_DRIVER_NAME "ne1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_ne_driver_attach
extern int rtems_ne_driver_attach();
#endif

#ifdef qemu
#define BSP_IDLE_TASK_BODY bsp_ppc_idle_task_body
extern void *bsp_ppc_idle_task_body(uintptr_t arg);
#endif

#include <bsp/openpic.h>
/* BSP_PIC_DO_EOI is optionally used by the 'vmeUniverse' driver
 * to implement VME IRQ priorities in software.
 * Note that this requires support by the interrupt controller
 * driver (cf. libbsp/shared/powerpc/irq/openpic_i8259_irq.c)
 * and the BSP-specific universe initialization/configuration
 * (cf. libbsp/shared/powerpc/vme/VMEConfig.h vme_universe.c)
 *
 * ********* IMPORTANT NOTE ********
 * When deriving from this file (new BSPs)
 * DO NOT define "BSP_PIC_DO_EOI" if you don't know what
 * you are doing i.e., w/o implementing the required pieces
 * mentioned above.
 * ********* IMPORTANT NOTE ********
 */
#define BSP_PIC_DO_EOI openpic_eoi(0)

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

/*
 * String passed by the bootloader.
 */
extern char *BSP_commandline_string;

#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

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

#endif

#ifdef __cplusplus
};
#endif

#endif
