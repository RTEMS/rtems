/*
 *  i386 interrupt macros.
 *
 *  Formerly contained in and extracted from libcpu/i386/cpu.h
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  $Id$
 *
 *  Applications must not include this file directly.
 */

#ifndef _rtems_score_interrupts_h
#define _rtems_score_interrupts_h

#ifndef ASM

struct 	__rtems_raw_irq_connect_data__;

typedef void (*rtems_raw_irq_hdl)		(void);
typedef void (*rtems_raw_irq_enable)		(const struct __rtems_raw_irq_connect_data__*);
typedef void (*rtems_raw_irq_disable)		(const struct __rtems_raw_irq_connect_data__*);
typedef int  (*rtems_raw_irq_is_enabled)	(const struct __rtems_raw_irq_connect_data__*);

/*
 *  Interrupt Level Macros
 */

#define i386_disable_interrupts( _level ) \
  { \
    asm volatile ( "pushf ; \
                    cli ; \
                    pop %0" \
                   : "=rm" ((_level)) \
    ); \
  }

#define i386_enable_interrupts( _level )  \
  { \
    asm volatile ( "push %0 ; \
                    popf" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    asm volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _eflags; \
    \
    asm volatile ( "pushf ; \
                    pop %0" \
                    : "=rm" ((_eflags)) \
    ); \
    \
    _level = (_eflags & EFLAGS_INTR_ENABLE) ? 0 : 1; \
  } while (0)

#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level ) i386_enable_interrupts( _level )
     
#endif
#endif
