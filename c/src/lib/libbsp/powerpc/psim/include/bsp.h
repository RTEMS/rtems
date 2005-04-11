/*  bsp.h
 *
 *  This include file contains all Papyrus board IO definitions.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/include/bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __PAPYRUS_h
#define __PAPYRUS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (12 * 1024)
  
#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <console.h>
#include <libcpu/io.h>
#include <clockdrv.h>
#include <iosupp.h>
#include <bsp/vectors.h>

/*
 *  Stuff for Time Test 27
 */
#if defined(RTEMS_TM27)

#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

/* #define Install_tm27_vector( _handler ) \
   set_vector( (_handler), PPC_IRQ_DECREMENTER, 1 ) */

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
    unsigned32 _clicks = 1; \
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

/* Constants */

/*
 *  Device Driver Table Entries
 */
 
/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */
 

/*
 *  Information placed in the linkcmds file.
 */

extern int   RAM_END;
extern int   end;        /* last address in the program */


#define BSP_Convert_decrementer( _value ) ( (unsigned long long) _value ) 

/* macros */
#define Processor_Synchronize() \
  asm(" eieio ")

/* functions */

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

void bsp_cleanup( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
