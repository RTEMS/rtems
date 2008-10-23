/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  S. Kate Feng 2003-2007 : Modified it to support the mvme5500 BSP.
 *
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libcpu/io.h>
#include <bsp/vectors.h>

#include <bsp/bspMvme5500.h>

/* fundamental addresses for this BSP (PREPxxx are from libcpu/io.h) */
#define _IO_BASE GT64260_REG_BASE

/* PCI0 Domain I/O space */
#define PCI0_IO_BASE            0xf0000000
#define PCI1_IO_BASE            0xf0800000

/* PCI 0 memory space as seen from the CPU */
#define PCI0_MEM_BASE		0x80000000
#define PCI_MEM_BASE            0  /* glue for vmeUniverse */
#define PCI_MEM_BASE_ADJUSTMENT        0

/* address of our ram on the PCI bus */
#define	PCI_DRAM_OFFSET	        0

/* PCI 1 memory space as seen from the CPU */
#define PCI1_MEM_BASE		0xe0000000
#define PCI1_MEM_SIZE           0x10000000

/* Needed for hot adding via PMCspan on the PCI0 local bus.
 * This is board dependent, only because mvme5500
 * supports hot adding and has more than one local PCI
 * bus.
 */
#define BSP_MAX_PCI_BUS_ON_PCI0 8
#define BSP_MAX_PCI_BUS_ON_PCI1 2
#define BSP_MAX_PCI_BUS  (BSP_MAX_PCI_BUS_ON_PCI0+BSP_MAX_PCI_BUS_ON_PCI1)


/* The glues to Till's vmeUniverse, although the name does not
 * actually reflect the relevant architect of the MVME5500.
 * Till TODO ? :  BSP_PCI_DO_EOI instead ? 
 * BSP_EXT_IRQ0 instead of BSP_PCI_IRQ0 ?
 *
 */
#define BSP_PIC_DO_EOI  inl(0xc34)  /* PCI IACK */
#define BSP_PCI_IRQ0 BSP_GPP_IRQ_LOWEST_OFFSET

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2

/* uart.c uses out_8 instead of outb  */
#define BSP_UART_IOBASE_COM1	GT64260_DEV1_BASE + 0x20000
#define BSP_UART_IOBASE_COM2	GT64260_DEV1_BASE + 0x21000

#define BSP_CONSOLE_PORT		BSP_UART_COM1  /* console */
#define BSP_UART_BAUD_BASE		115200

/*
 * Vital Board data Start using DATA RESIDUAL
 */
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

#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

extern void BSP_panic(char *s);
extern void bsp_reset(void);
/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2))); */
extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);

extern unsigned long _BSP_clear_hostbridge_errors();

extern unsigned int BSP_heap_start;

#if 0
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"gt1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_GT64260eth_driver_attach
#else
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"wmG1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_i82544EI_driver_attach
#endif

extern int
RTEMS_BSP_NETWORK_DRIVER_ATTACH(/* struct rtems_bsdnet_ifconfig * */);

/*
 *  BSP Configuration Default Overrides
 */
#define BSP_ZERO_WORKSPACE_AUTOMATICALLY TRUE

/* As per Linux, This should be in the ppc/system.h */

static inline void memBar()
{
    asm volatile("sync":::"memory");
}

static inline void ioBar()
{
    asm volatile("eieio");
}

#endif 
