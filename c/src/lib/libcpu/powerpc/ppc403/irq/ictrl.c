/*  ictrl.c
 *
 *  This routine installs and handles external interrupt vectors for
 *  PowerPC 403 CPU built-in external interrupt controller
 *
 *  Author: Thomas Doerfler <td@imd.m.isar.de>
 *
 *  COPYRIGHT (c) 1998 by IMD, Puchheim, Germany
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of IMD not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      IMD makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 */

#include "ictrl.h"
#include <rtems.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */

/*
 *  ISR vector table to dispatch external interrupts
 */

rtems_isr_entry ictrl_vector_table[PPC_IRQ_EXT_MAX];

/*
 *
 *  some utilities to access the EXI* registers
 *
 */

/*
 *  clear bits in EXISR that have a bit set in mask
 */
#if defined(ppc405)
RTEMS_INLINE_ROUTINE void
clr_exisr(uint32_t   mask)
{
    __asm__ volatile ("mtdcr 0xC0,%0"::"r" (mask));/*EXISR*/
}

/*
 *  get value of EXISR
 */
RTEMS_INLINE_ROUTINE uint32_t
get_exisr(void)
{
    uint32_t   val;

    __asm__ volatile ("mfdcr %0,0xC0":"=r" (val));/*EXISR*/
    return val;
}

/*
 *  get value of EXIER
 */
RTEMS_INLINE_ROUTINE uint32_t
get_exier(void)
{
    uint32_t   val;
    __asm__ volatile ("mfdcr %0,0xC2":"=r" (val));/*EXIER*/
    return val;
}

/*
 *  set value of EXIER
 */
RTEMS_INLINE_ROUTINE void
set_exier(uint32_t   val)
{
    __asm__ volatile ("mtdcr 0xC2,%0"::"r" (val));/*EXIER*/
}

#else /* not ppc405 */

RTEMS_INLINE_ROUTINE void
clr_exisr(uint32_t   mask)
{
    __asm__ volatile ("mtdcr 0x40,%0"::"r" (mask));/*EXISR*/
}

/*
 *  get value of EXISR
 */
RTEMS_INLINE_ROUTINE uint32_t
get_exisr(void)
{
    uint32_t   val;

    __asm__ volatile ("mfdcr %0,0x40":"=r" (val));/*EXISR*/
    return val;
}

/*
 *  get value of EXIER
 */
RTEMS_INLINE_ROUTINE uint32_t
get_exier(void)
{
    uint32_t   val;
    __asm__ volatile ("mfdcr %0,0x42":"=r" (val));/*EXIER*/
    return val;
}

/*
 *  set value of EXIER
 */
RTEMS_INLINE_ROUTINE void
set_exier(uint32_t   val)
{
    __asm__ volatile ("mtdcr 0x42,%0"::"r" (val));/*EXIER*/
}
#endif /* ppc405 */
/*
 *  enable an external interrupt, make this interrupt consistent
 */
RTEMS_INLINE_ROUTINE void
enable_ext_irq( uint32_t   mask)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    set_exier(get_exier() | ((mask)&PPC_EXI_MASK));
  rtems_interrupt_enable(level);
}

/*
 *  disable an external interrupt, make this interrupt consistent
 */
RTEMS_INLINE_ROUTINE void
disable_ext_irq( uint32_t   mask)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
    set_exier(get_exier() & ~(mask) & PPC_EXI_MASK);
  rtems_interrupt_enable(level);
}

/*
 *
 *  this function is called, when a external interrupt is present and
 *  enabled but there is no handler installed. It will clear
 *  the corresponding enable bits and call the spurious handler
 *  present in the CPU Configuration Table, if any.
 *
 */
void
ictrl_spurious_handler(uint32_t   spurious_mask,
		       CPU_Interrupt_frame *cpu_frame)
{
  int v;
  extern void (*bsp_spurious_handler)(uint32_t   vector, CPU_Interrupt_frame *);

  for (v=0; v < PPC_IRQ_EXT_MAX; v++) {
    if (VEC_TO_EXMSK(v) & spurious_mask) {
      clr_exisr(VEC_TO_EXMSK(v));
      disable_ext_irq(VEC_TO_EXMSK(v));
#if 0
      printf("spurious external interrupt: %d at pc 0x%x; disabling\n",
	     vector, cpu_frame->Interrupt.pcoqfront);
#endif
      if (bsp_spurious_handler()) {
	bsp_spurious_handler(v + PPC_IRQ_EXT_BASE,cpu_frame);
      }
    }
  }
}


/*
 *  ISR Handler: this is called from the primary exception dispatcher
 */

void
ictrl_isr(rtems_vector_number vector,CPU_Interrupt_frame *cpu_frame)
{
  uint32_t          istat,
                    mask,
                    global_vec;
  int               exvec;
  rtems_isr_entry handler;

  istat = get_exisr() & get_exier() & PPC_EXI_MASK;

  /* FIXME: this may be speeded up using cntlzw instruction */
  for (exvec = 0;exvec < PPC_IRQ_EXT_MAX;exvec++) {
    mask = VEC_TO_EXMSK(exvec);
    if (0 != (istat & mask)) {
      /*clr_exisr(mask); too early to ack*/
      handler = ictrl_vector_table[exvec];
      if (handler) {
	istat &= ~mask;
	global_vec = exvec + PPC_IRQ_EXT_BASE;
	(handler)(global_vec);
      }
      clr_exisr(mask);/* now we can ack*/
    }
  }
  if (istat != 0) { /* anything left? then we have a spurious interrupt */
    ictrl_spurious_handler(istat,cpu_frame);
  }
}

/*
 *
 * the following functions form the user interface
 *
 */

/*
 *
 * install a user vector for one of the external interrupt sources
 *
 */
rtems_status_code
ictrl_set_vector(rtems_isr_entry   new_handler,
		 uint32_t          vector,
		 rtems_isr_entry   *old_handler
)
{
  /*
   *  We put the actual user ISR address in 'ictrl_vector_table'.  This will
   *  be used by the _ictrl_isr so the user gets control.
   */

  /* check for valid vector range */
  if ((vector >= PPC_IRQ_EXT_BASE) &&
      (vector <  PPC_IRQ_EXT_BASE + PPC_IRQ_EXT_MAX)) {
    /* return old handler entry */
    *old_handler = ictrl_vector_table[vector - PPC_IRQ_EXT_BASE];

    if (new_handler != NULL) {
      /* store handler function... */
      ictrl_vector_table[vector - PPC_IRQ_EXT_BASE] = new_handler;
      /* then enable it in EXIER register */
      enable_ext_irq(VEC_TO_EXMSK(vector - PPC_IRQ_EXT_BASE));
    }
    else { /* new_handler == NULL */
      /* then disable it in EXIER register */
      disable_ext_irq(VEC_TO_EXMSK(vector - PPC_IRQ_EXT_BASE));
      ictrl_vector_table[vector - PPC_IRQ_EXT_BASE] = NULL;
    }
    return RTEMS_SUCCESSFUL;
  }
  else {
    return RTEMS_INVALID_NUMBER;
  }
}

/*
 * Called via atexit()
 * deactivate the interrupt controller
 */

void
ictrl_exit(void)
{
  /* mark them all unused */
  disable_ext_irq(~0);
  clr_exisr(~0);

}

/*
 * activate the interrupt controller
 */

rtems_status_code
ictrl_init(void)
{
  proc_ptr dummy;

  /* mark them all unused */
  disable_ext_irq(~0);
  clr_exisr(~0);

  /* install the external interrupt handler */
  _CPU_ISR_install_vector(PPC_IRQ_EXTERNAL,
			  ictrl_isr,
			  &dummy);
  atexit(ictrl_exit);
  return RTEMS_SUCCESSFUL;
}
