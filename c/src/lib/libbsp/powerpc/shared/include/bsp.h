/*
 *  bsp.h  -- contain BSP API definition.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */
#ifndef LIBBSP_POWERPC_MOTOROLA_SHARED_BSP_H
#define LIBBSP_POWERPC_MOTOROLA_SHARED_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <console.h>
#include <libcpu/io.h>
#include <clockdrv.h>
#include <bsp/vectors.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

/* fundamental addresses for this BSP (PREPxxx are from libcpu/io.h) */
#define	_IO_BASE			PREP_ISA_IO_BASE
#define	_ISA_MEM_BASE		PREP_ISA_MEM_BASE
/* address of our ram on the PCI bus   */
#define	PCI_DRAM_OFFSET		PREP_PCI_DRAM_OFFSET
/* offset of pci memory as seen from the CPU */
#define PCI_MEM_BASE		PREP_ISA_MEM_BASE

/*
 *  base address definitions for several devices
 *
 */
#define BSP_UART_IOBASE_COM1 ((_IO_BASE)+0x3f8)
#define BSP_UART_IOBASE_COM2 ((_IO_BASE)+0x2f8)
#define BSP_KBD_IOBASE       ((_IO_BASE)+0x60)
#define BSP_VGA_IOBASE       ((_IO_BASE)+0x3c0)

#define BSP_CONSOLE_PORT	BSP_UART_COM1
#define BSP_UART_BAUD_BASE	115200
  
#include <bsp/openpic.h>
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

/*
 *  TM27 stuff
 */

#if defined(USE_ENHANCED_INTR_API) && defined(RTEMS_TM27)

#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

void nullFunc() {}
static rtems_irq_connect_data clockIrqData = {BSP_DECREMENTER,
					      0,
					      (rtems_irq_enable)nullFunc,
					      (rtems_irq_disable)nullFunc,
					      (rtems_irq_is_enabled) nullFunc};
void Install_tm27_vector(void (*_handler)())
{
  clockIrqData.hdl = _handler;
  if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
	printk("Error installing clock interrupt handler!\n");
	rtems_fatal_error_occurred(1);
  }
}

#define Cause_tm27_intr()  \
  do { \
    unsigned32 _clicks = 8; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)


#define Clear_tm27_intr() \
  do { \
    unsigned32 _clicks = 0xffffffff; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
  } while (0)
#endif


#endif

#endif
