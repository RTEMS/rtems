/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  S. Kate Feng 12/03 : Modified it to support the MVME5500 board.
 *
 */

#ifndef LIBBSP_POWERPC_MVME5500_BSP_H
#define LIBBSP_POWERPC_MVME5500_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
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

/* address of our ram on the PCI bus */
#define	PCI_DRAM_OFFSET	        0

/* PCI 1 memory space as seen from the CPU */
#define PCI1_MEM_BASE		0xe0000000
#define PCI1_MEM_SIZE           0x10000000

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
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

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

extern rtems_configuration_table  BSP_Configuration;
extern void BSP_panic(char *s);
extern void rtemsReboot(void);
/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2))); */
extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);

extern unsigned long _BSP_clear_hostbridge_errors();

#define RTEMS_BSP_NETWORK_DRIVER_NAME	"gt1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_GT64260eth_driver_attach

extern int
RTEMS_BSP_NETWORK_DRIVER_ATTACH(/* struct rtems_bsdnet_ifconfig * */);


#endif 
