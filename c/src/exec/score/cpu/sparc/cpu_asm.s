/*  cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  $Id$
 */

#include <asm.h>
#include <rtems/score/cpu.h>

/*
 *  _CPU_Context_save_fp
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *  
 *  void _CPU_Context_save_fp(
 *    void **fp_context_ptr
 *  )
 *  {
 *  }
 */

        .align 4
        PUBLIC(_CPU_Context_save_fp)
SYM(_CPU_Context_save_fp):
        save    %sp,-104,%sp
        ld      [%i0],%l0
        std     %f0,[%l0+FO_F1_OFFSET]
        std     %f2,[%l0+F2_F3_OFFSET]
        std     %f4,[%l0+F4_F5_OFFSET]
        std     %f6,[%l0+F6_F7_OFFSET]
        std     %f8,[%l0+F8_F9_OFFSET]
        std     %f10,[%l0+F1O_F11_OFFSET]
        std     %f12,[%l0+F12_F13_OFFSET]
        std     %f14,[%l0+F14_F15_OFFSET]
        std     %f16,[%l0+F16_F17_OFFSET]
        std     %f18,[%l0+F18_F19_OFFSET]
        std     %f20,[%l0+F2O_F21_OFFSET]
        std     %f22,[%l0+F22_F23_OFFSET]
        std     %f24,[%l0+F24_F25_OFFSET]
        std     %f26,[%l0+F26_F27_OFFSET]
        std     %f28,[%l0+F28_F29_OFFSET]
        std     %f30,[%l0+F3O_F31_OFFSET]
        st      %fsr,[%l0+FSR_OFFSET]
        ret
        restore

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *  
 *  void _CPU_Context_restore_fp(
 *    void **fp_context_ptr
 *  )
 *  {
 *  }
 */

        .align 4
        PUBLIC(_CPU_Context_restore_fp)
SYM(_CPU_Context_restore_fp):
        save    %sp,-104,%sp
        ld      [%o0],%l0
        ldd     [%l0+FO_F1_OFFSET],%f0
        ldd     [%l0+F2_F3_OFFSET],%f2
        ldd     [%l0+F4_F5_OFFSET],%f4
        ldd     [%l0+F6_F7_OFFSET],%f6
        ldd     [%l0+F8_F9_OFFSET],%f8
        ldd     [%l0+F1O_F11_OFFSET],%f10
        ldd     [%l0+F12_F13_OFFSET],%f12
        ldd     [%l0+F14_F15_OFFSET],%f14
        ldd     [%l0+F16_F17_OFFSET],%f16
        ldd     [%l0+F18_F19_OFFSET],%f18
        ldd     [%l0+F2O_F21_OFFSET],%f20
        ldd     [%l0+F22_F23_OFFSET],%f22
        ldd     [%l0+F24_F25_OFFSET],%f24
        ldd     [%l0+F26_F27_OFFSET],%f26
        ldd     [%l0+F28_F29_OFFSET],%f28
        ldd     [%l0+F3O_F31_OFFSET],%f30
        ld      [%l0+FSR_OFFSET],%fsr
        ret
        restore

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 *  
 *  void _CPU_Context_switch(
 *    Context_Control  *run,
 *    Context_Control  *heir
 *  )
 *  {
 *  }
 */

/* from gcc-2.7.0/config/sparc/sparc.h on register usage */

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.
   g0 is used for the condition code and not to represent %g0, which is
   hardwired to 0, so reg 0 is *not* fixed.
   On non-v9 systems:
   g1 is free to use as temporary.
   g2-g4 are reserved for applications.  Gcc normally uses them as
   temporaries, but this can be disabled via the -mno-app-regs option.
   g5 through g7 are reserved for the operating system.
   On v9 systems:
   g1 and g5 are free to use as temporaries.
   g2-g4 are reserved for applications (the compiler will not normally use
   them, but they can be used as temporaries with -mapp-regs).
   g6-g7 are reserved for the operating system.
   ??? Register 1 is used as a temporary by the 64 bit sethi pattern, so must
   currently be a fixed register until this pattern is rewritten.
   Register 1 is also used when restoring call-preserved registers in large
   stack frames.  */


        .align 4
        PUBLIC(_CPU_Context_switch)
SYM(_CPU_Context_switch):
        ta      0x03                       /* flush registers */

        /* skip g0 */
        st      %g1,[%o0+G1_OFFSET]        /* globals */
        st      %g2,[%o0+G2_OFFSET]
        st      %g3,[%o0+G3_OFFSET]
        st      %g4,[%o0+G4_OFFSET]
        st      %g5,[%o0+G5_OFFSET]
        st      %g6,[%o0+G6_OFFSET]
        st      %g7,[%o0+G7_OFFSET]

        st      %l0,[%o0+L0_OFFSET]
        st      %l1,[%o0+L1_OFFSET]
        st      %l2,[%o0+L2_OFFSET]
        st      %l3,[%o0+L3_OFFSET]
        st      %l4,[%o0+L4_OFFSET]
        st      %l5,[%o0+L5_OFFSET]
        st      %l6,[%o0+L6_OFFSET]
        st      %l7,[%o0+L7_OFFSET]

        st      %i0,[%o0+I0_OFFSET]
        st      %i1,[%o0+I1_OFFSET]
        st      %i2,[%o0+I2_OFFSET]
        st      %i3,[%o0+I3_OFFSET]
        st      %i4,[%o0+I4_OFFSET]
        st      %i5,[%o0+I5_OFFSET]
        st      %i6,[%o0+I6_OFFSET]
        st      %i7,[%o0+I7_OFFSET]

        st      %o0,[%o0+O0_OFFSET]
        st      %o1,[%o0+O1_OFFSET]
        st      %o2,[%o0+O2_OFFSET]
        st      %o3,[%o0+O3_OFFSET]
        st      %o4,[%o0+O4_OFFSET]
        st      %o5,[%o0+O5_OFFSET]
        st      %o6,[%o0+O6_OFFSET]
        st      %o7,[%o0+O7_OFFSET]

        rd      %psr,%o2
        st      %o2,[%o0+PSR_OFFSET]        /* save status register */

        /* enter here with o1 = context to restore */
        /*                 o2 = psr */
restore:

        ld      [%o1+PSR_OFFSET],%o0
        and     %o2,31,%o2               /* g1 = cwp */
        and     %o0,-32,%o0                /* o0 = psr w/o cwp */
        or      %o0,%o2,%o2                /* o2 = new psr */
        wr      %o2,0,%psr                 /* restore status register */

        /* skip g0 */
        ld      [%o1+G1_OFFSET],%g1
        ld      [%o1+G2_OFFSET],%g2
        ld      [%o1+G3_OFFSET],%g3
        ld      [%o1+G4_OFFSET],%g4
        ld      [%o1+G5_OFFSET],%g5
        ld      [%o1+G6_OFFSET],%g6
        ld      [%o1+G7_OFFSET],%g7

        ld      [%o1+L0_OFFSET],%l0
        ld      [%o1+L1_OFFSET],%l1
        ld      [%o1+L2_OFFSET],%l2
        ld      [%o1+L3_OFFSET],%l3
        ld      [%o1+L4_OFFSET],%l4
        ld      [%o1+L5_OFFSET],%l5
        ld      [%o1+L6_OFFSET],%l6
        ld      [%o1+L7_OFFSET],%l7

        ld      [%o1+I0_OFFSET],%i0
        ld      [%o1+I1_OFFSET],%i1
        ld      [%o1+I2_OFFSET],%i2
        ld      [%o1+I3_OFFSET],%i3
        ld      [%o1+I4_OFFSET],%i4
        ld      [%o1+I5_OFFSET],%i5
        ld      [%o1+I6_OFFSET],%i6
        ld      [%o1+I7_OFFSET],%i7

        ld      [%o1+O0_OFFSET],%o0
        /* do o1 last to avoid destroying heir context pointer */
        ld      [%o1+O2_OFFSET],%o2
        ld      [%o1+O3_OFFSET],%o3
        ld      [%o1+O4_OFFSET],%o4
        ld      [%o1+O5_OFFSET],%o5
        ld      [%o1+O6_OFFSET],%o6
        ld      [%o1+O7_OFFSET],%o7

        ld      [%o1+O1_OFFSET],%o1   /* overwrite heir pointer */

        jmp     %o7 + 8                     /* return */
        nop                                 /* delay slot */
        

/*
 *  _CPU_Context_restore
 *
 *  This routine is generallu used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *  
 *  void _CPU_Context_restore(
 *    Context_Control *new_context
 *  )
 *  {
 *  }
 */

        .align 4
        PUBLIC(_CPU_Context_restore)
SYM(_CPU_Context_restore):
        save    %sp, -104, %sp              /* save a stack frame */
        ta      0x03                       /* flush registers */
        rd      %psr,%o2
        ba      restore 
        mov     %i0,%o1                    /* in the delay slot */

/*  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  void _ISR_Handler()
 *  {
 *  }
 */

        .align 4
        PUBLIC(_ISR_Handler)
SYM(_ISR_Handler):
        ret

  /*
   *  This discussion ignores a lot of the ugly details in a real
   *  implementation such as saving enough registers/state to be
   *  able to do something real.  Keep in mind that the goal is
   *  to invoke a user's ISR handler which is written in C and
   *  uses a certain set of registers.
   *
   *  Also note that the exact order is to a large extent flexible.
   *  Hardware will dictate a sequence for a certain subset of
   *  _ISR_Handler while requirements for setting
   */

 /*
  *  At entry to "common" _ISR_Handler, the vector number must be
  *  available.  On some CPUs the hardware puts either the vector
  *  number or the offset into the vector table for this ISR in a
  *  known place.  If the hardware does not give us this information,
  *  then the assembly portion of RTEMS for this port will contain
  *  a set of distinct interrupt entry points which somehow place
  *  the vector number in a known place (which is safe if another
  *  interrupt nests this one) and branches to _ISR_Handler.
  *
  *  save some or all context on stack
  *  may need to save some special interrupt information for exit
  *
  *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
  *    if ( _ISR_Nest_level == 0 )
  *      switch to software interrupt stack
  *  #endif
  *
  *  _ISR_Nest_level++;
  *
  *  _Thread_Dispatch_disable_level++;
  *
  *  (*_ISR_Vector_table[ vector ])( vector );
  *
  *  --_ISR_Nest_level;
  *
  *  if ( _ISR_Nest_level )
  *    goto the label "exit interrupt (simple case)"
  *
  *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
  *    restore stack
  *  #endif
  *  
  *  if ( !_Context_Switch_necessary )
  *    goto the label "exit interrupt (simple case)"
  *  
  *  if ( !_ISR_Signals_to_thread_executing )
  *    goto the label "exit interrupt (simple case)"
  *
  *  call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
  *
  *  prepare to get out of interrupt
  *  return from interrupt  (maybe to _ISR_Dispatch)
  *
  *  LABEL "exit interrupt (simple case):
  *  prepare to get out of interrupt
  *  return from interrupt
  */
