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
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
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
#include <rtems/score/ispsh7750.h>
#include <rtems/score/iosh7750.h>
#include <rtems/score/sh4_regs.h>
#include <rtems/score/sh_io.h>

/* from cpu_isps.c */
extern proc_ptr         _Hardware_isr_Table[];

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  unsigned long    *_old_stack_ptr;
#endif

register unsigned long  *stack_ptr asm("r15"); 

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
  void **fp_context_ptr     /* r4 */
)
{
#if SH_HAS_FPU
 
asm volatile("
    mov.l   @%0,r4    
    add     %1,r4
    sts.l   fpscr,@-r4
    sts.l   fpul,@-r4
    lds     %2,fpscr
    fmov    dr14,@-r4
    fmov    dr12,@-r4
    fmov    dr10,@-r4
    fmov    dr8,@-r4
    fmov    dr6,@-r4
    fmov    dr4,@-r4
    fmov    dr2,@-r4
    fmov    dr0,@-r4
    "
#ifdef SH4_USE_X_REGISTERS
    "
    lds     %3,fpscr
    fmov    xd14,@-r4
    fmov    xd12,@-r4
    fmov    xd10,@-r4
    fmov    xd8,@-r4
    fmov    xd6,@-r4
    fmov    xd4,@-r4
    fmov    xd2,@-r4
    fmov    xd0,@-r4
    " 
#endif
   "lds     %4,fpscr
   "
    : 
    : "r"(fp_context_ptr), "r"(sizeof(Context_Control_fp)), 
      "r"(SH4_FPSCR_SZ), "r"(SH4_FPSCR_PR | SH4_FPSCR_SZ), "r"(SH4_FPSCR_PR)
    : "r4", "r0");

#endif
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
  void **fp_context_ptr     /* r4 */
)
{
#if SH_HAS_FPU

asm volatile("
    mov.l   @%0,r4    
    "
#ifdef SH4_USE_X_REGISTERS
    "
    lds     %1,fpscr
    fmov    @r4+,xd0
    fmov    @r4+,xd2
    fmov    @r4+,xd4
    fmov    @r4+,xd6
    fmov    @r4+,xd8
    fmov    @r4+,xd10
    fmov    @r4+,xd12
    fmov    @r4+,xd14
    "
#endif
    "
    lds     %2,fpscr
    fmov    @r4+,dr0
    fmov    @r4+,dr2
    fmov    @r4+,dr4
    fmov    @r4+,dr6
    fmov    @r4+,dr8
    fmov    @r4+,dr10
    fmov    @r4+,dr12
    fmov    @r4+,dr14
    lds.l   @r4+,fpul
    lds.l   @r4+,fpscr
    " : 
    : "r"(fp_context_ptr), "r"(SH4_FPSCR_PR | SH4_FPSCR_SZ), "r"(SH4_FPSCR_SZ)
    : "r4", "r0");

#endif
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

asm volatile("
	.global __CPU_Context_switch
__CPU_Context_switch:

	add	%0,r4
  
	stc.l   sr,@-r4
	stc.l	gbr,@-r4
	mov.l   r0,@-r4
	mov.l   r1,@-r4
	mov.l   r2,@-r4
	mov.l   r3,@-r4

	mov.l   r6,@-r4
	mov.l   r7,@-r4
	mov.l	r8,@-r4
	mov.l	r9,@-r4
	mov.l	r10,@-r4
	mov.l	r11,@-r4
	mov.l	r12,@-r4
	mov.l	r13,@-r4
	mov.l   r14,@-r4
	sts.l   pr,@-r4
	sts.l   mach,@-r4
	sts.l   macl,@-r4
	mov.l   r15,@-r4

	mov     r5, r4"
  :: "I" (sizeof(Context_Control))
  );

  asm volatile("
	.global __CPU_Context_restore
__CPU_Context_restore:
	mov.l   @r4+,r15
	lds.l   @r4+,macl
	lds.l   @r4+,mach
	lds.l   @r4+,pr
	mov.l   @r4+,r14
	mov.l   @r4+,r13
	mov.l   @r4+,r12
	mov.l   @r4+,r11
	mov.l   @r4+,r10
	mov.l   @r4+,r9
	mov.l   @r4+,r8
	mov.l   @r4+,r7
	mov.l   @r4+,r6

	mov.l   @r4+,r3
	mov.l   @r4+,r2
	mov.l   @r4+,r1
	mov.l   @r4+,r0
	ldc.l   @r4+,gbr
	ldc.l   @r4+,sr

	rts
	nop" );
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

  _ISR_Nest_level--;

#if( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)

  if( _ISR_Nest_level == 0 )
    /* restore old stack pointer */
    stack_ptr = _old_stack_ptr;  
#endif

  _Thread_Dispatch_disable_level--;

  _CPU_ISR_Enable( level );

  if ( _Thread_Dispatch_disable_level == 0 )
    {
      if(( _Context_Switch_necessary) || (! _ISR_Signals_to_thread_executing))
	{
	  _ISR_Signals_to_thread_executing = FALSE;
	  _Thread_Dispatch();
	}
  }
}
