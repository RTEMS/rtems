/*
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  NOTE:  This port uses a C file with inline assembler instructions
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" cpu_asm file.  An
 *  implementation in assembly should include "cpu_asm.h"
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/sh.h>

#include <rtems/score/ispsh7032.h>
#include <rtems/score/iosh7032.h>
#include <rtems/score/sh_io.h>

/* from cpu_isps.c */
extern proc_ptr         _Hardware_isr_Table[];

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr asm("r15"); 

/* 
 * sh_set_irq_priority
 * 
 * this function sets the interrupt level of the specified interrupt
 *
 * parameters:
 *             - irq : interrupt number 
 *             - prio: priority to set for this interrupt number
 *
 * returns:    0 if ok
 *             -1 on error
 */

unsigned int sh_set_irq_priority( 
  unsigned int irq, 
  unsigned int prio )
{
  unsigned32 shiftcount;
  unsigned32 prioreg;
  unsigned16 temp16;
  unsigned32 level;

  /*
   * first check for valid interrupt
   */
  if(( irq > 113) || (_Hardware_isr_Table[irq] == _dummy_isp))
    return -1;
  /*
   * check for valid irq priority
   */
  if( prio > 15 )
    return -1;

  /*
   * look up appropriate interrupt priority register
   */
  if( irq > 71)
    {
      irq = irq - 72;
      shiftcount = 12 - ((irq & ~0x03) % 16);
      
      switch( irq / 16)
	{
	case 0: { prioreg = INTC_IPRC; break;}
	case 1: { prioreg = INTC_IPRD; break;}
	case 2: { prioreg = INTC_IPRE; break;}
	default: return -1;
	}
    }
  else
    {
      shiftcount = 12 - 4 * ( irq % 4);
      if( irq > 67)
	prioreg = INTC_IPRB;
      else
	prioreg = INTC_IPRA;
    }

  /*
   * Set the interrupt priority register
   */
  _CPU_ISR_Disable( level );

  temp16 = read16( prioreg);
  temp16 &= ~( 15 << shiftcount);
  temp16 |= prio << shiftcount;
  write16( temp16, prioreg);

  _CPU_ISR_Enable( level );

  return 0;
}

/*
 *  _CPU_Context_save_fp_context
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
)
{
}

/*
 *  _CPU_Context_restore_fp_context
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
)
{
}

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 */

/*  within __CPU_Context_switch:
 *  _CPU_Context_switch
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 * NOTE: It should be safe not to store r4, r5
 *
 * NOTE: It is doubtful if r0 is really needed to be stored
 *
 * NOTE: gbr is added, but should not be necessary, as it is
 *	only used globally in this port.
 */

/*
 * FIXME: This is an ugly hack, but we wanted to avoid recalculating 
 *        the offset each time Context_Control is changed
 */
void __CPU_Context_switch(
  Context_Control  *run,	/* r4 */
  Context_Control  *heir	/* r5 */
)
{

asm volatile(
	".global __CPU_Context_switch\n"
"__CPU_Context_switch:\n"

"	add	%0,r4\n"
  
"	stc.l   sr,@-r4\n"
"	stc.l	gbr,@-r4\n"
"	mov.l   r0,@-r4\n"
"	mov.l   r1,@-r4\n"
"	mov.l   r2,@-r4\n"
"	mov.l   r3,@-r4\n"

"	mov.l   r6,@-r4\n"
"	mov.l   r7,@-r4\n"
"	mov.l	r8,@-r4\n"
"	mov.l	r9,@-r4\n"
"	mov.l	r10,@-r4\n"
"	mov.l	r11,@-r4\n"
"	mov.l	r12,@-r4\n"
"	mov.l	r13,@-r4\n"
"	mov.l   r14,@-r4\n"
"	sts.l   pr,@-r4\n"
"	sts.l   mach,@-r4\n"
"	sts.l   macl,@-r4\n"
"	mov.l   r15,@-r4\n"

"	mov     r5, r4\n"
  :: "I" (sizeof(Context_Control))
  );

  asm volatile(
	".global __CPU_Context_restore\n"
"__CPU_Context_restore:\n"
"	mov.l   @r4+,r15\n"
"	lds.l   @r4+,macl\n"
"	lds.l   @r4+,mach\n"
"	lds.l   @r4+,pr\n"
"	mov.l   @r4+,r14\n"
"	mov.l   @r4+,r13\n"
"	mov.l   @r4+,r12\n"
"	mov.l   @r4+,r11\n"
"	mov.l   @r4+,r10\n"
"	mov.l   @r4+,r9\n"
"	mov.l   @r4+,r8\n"
"	mov.l   @r4+,r7\n"
"	mov.l   @r4+,r6\n"

"	mov.l   @r4+,r3\n"
"	mov.l   @r4+,r2\n"
"	mov.l   @r4+,r1\n"
"	mov.l   @r4+,r0\n"
"	ldc.l   @r4+,gbr\n"
"	ldc.l   @r4+,sr\n"

"	rts\n"
"	nop\n" );
}

/*  
 *  This routine provides the RTEMS interrupt management.
 */
 
void __ISR_Handler( unsigned32 vector)
{
  register unsigned32 level;

  _CPU_ISR_Disable( level );

  _Thread_Dispatch_disable_level++;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  if( _ISR_Nest_level == 0 )
    {
      /* Install irq stack */
      _old_stack_ptr = stack_ptr;
      stack_ptr = _CPU_Interrupt_stack_high;
    }

#endif

  _ISR_Nest_level++;

  _CPU_ISR_Enable( level );

  /* call isp */
  if( _ISR_Vector_table[ vector])
    (*_ISR_Vector_table[ vector ])( vector );

  _CPU_ISR_Disable( level );

  _Thread_Dispatch_disable_level--;

  _ISR_Nest_level--;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)

  if( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;  
#endif

  _CPU_ISR_Enable( level );

  if ( _ISR_Nest_level )
    return;

  if ( _Thread_Dispatch_disable_level ) {
    _ISR_Signals_to_thread_executing = FALSE;
    return;
  }

  if ( _Context_Switch_necessary || _ISR_Signals_to_thread_executing ) {
    _ISR_Signals_to_thread_executing = FALSE;
    _Thread_Dispatch();
  }
}
