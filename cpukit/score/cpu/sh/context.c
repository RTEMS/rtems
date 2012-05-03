/*
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/sh.h>

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
  Context_Control_fp **fp_context_ptr
)
{
#if SH_HAS_FPU

asm volatile("\n\
    mov.l   @%0,r4    \n\
    add     %1,r4\n\
    sts.l   fpscr,@-r4\n\
    sts.l   fpul,@-r4\n\
    lds     %2,fpscr\n\
    fmov    dr14,@-r4\n\
    fmov    dr12,@-r4\n\
    fmov    dr10,@-r4\n\
    fmov    dr8,@-r4\n\
    fmov    dr6,@-r4\n\
    fmov    dr4,@-r4\n\
    fmov    dr2,@-r4\n\
    fmov    dr0,@-r4\n\
    "
#ifdef SH4_USE_X_REGISTERS
    "\
    lds     %3,fpscr\n\
    fmov    xd14,@-r4\n\
    fmov    xd12,@-r4\n\
    fmov    xd10,@-r4\n\
    fmov    xd8,@-r4\n\
    fmov    xd6,@-r4\n\
    fmov    xd4,@-r4\n\
    fmov    xd2,@-r4\n\
    fmov    xd0,@-r4\n\
    "
#endif
   "lds     %4,fpscr\n\
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
  Context_Control_fp **fp_context_ptr
)
{

#if SH_HAS_FPU

asm volatile("\n\
    mov.l   @%0,r4    \n\
    "
#ifdef SH4_USE_X_REGISTERS
    "\n\
    lds     %1,fpscr\n\
    fmov    @r4+,xd0\n\
    fmov    @r4+,xd2\n\
    fmov    @r4+,xd4\n\
    fmov    @r4+,xd6\n\
    fmov    @r4+,xd8\n\
    fmov    @r4+,xd10\n\
    fmov    @r4+,xd12\n\
    fmov    @r4+,xd14\n\
    "
#endif
    "\n\
    lds     %2,fpscr\n\
    fmov    @r4+,dr0\n\
    fmov    @r4+,dr2\n\
    fmov    @r4+,dr4\n\
    fmov    @r4+,dr6\n\
    fmov    @r4+,dr8\n\
    fmov    @r4+,dr10\n\
    fmov    @r4+,dr12\n\
    fmov    @r4+,dr14\n\
    lds.l   @r4+,fpul\n\
    lds.l   @r4+,fpscr\n\
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

asm volatile("\n\
	.global __CPU_Context_switch\n\
__CPU_Context_switch:\n\
\n\
	add	%0,r4\n\
  \n\
	stc.l   sr,@-r4\n\
	stc.l	gbr,@-r4\n\
	mov.l   r0,@-r4\n\
	mov.l   r1,@-r4\n\
	mov.l   r2,@-r4\n\
	mov.l   r3,@-r4\n\
\n\
	mov.l   r6,@-r4\n\
	mov.l   r7,@-r4\n\
	mov.l	r8,@-r4\n\
	mov.l	r9,@-r4\n\
	mov.l	r10,@-r4\n\
	mov.l	r11,@-r4\n\
	mov.l	r12,@-r4\n\
	mov.l	r13,@-r4\n\
	mov.l   r14,@-r4\n\
	sts.l   pr,@-r4\n\
	sts.l   mach,@-r4\n\
	sts.l   macl,@-r4\n\
	mov.l   r15,@-r4\n\
\n\
	mov     r5, r4"
  :: "i" (sizeof(Context_Control))
  );

  __asm__ volatile("\n\
	.global __CPU_Context_restore\n\
__CPU_Context_restore:\n\
	mov.l   @r4+,r15\n\
	lds.l   @r4+,macl\n\
	lds.l   @r4+,mach\n\
	lds.l   @r4+,pr\n\
	mov.l   @r4+,r14\n\
	mov.l   @r4+,r13\n\
	mov.l   @r4+,r12\n\
	mov.l   @r4+,r11\n\
	mov.l   @r4+,r10\n\
	mov.l   @r4+,r9\n\
	mov.l   @r4+,r8\n\
	mov.l   @r4+,r7\n\
	mov.l   @r4+,r6\n\
\n\
	mov.l   @r4+,r3\n\
	mov.l   @r4+,r2\n\
	mov.l   @r4+,r1\n\
	mov.l   @r4+,r0\n\
	ldc.l   @r4+,gbr\n\
	ldc.l   @r4+,sr\n\
\n\
	rts\n\
	nop" );
}
