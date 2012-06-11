/**
 * @file
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0
/**
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  v850 Specific Information:
 *
 *  The v850 appears to always have soft float.
 */
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
)
{
}

/**
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  v850 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
)
{
}
#endif

/**
 *  This routine performs a normal non-FP context switch.
 *
 *  v850 Specific Information:
 *
 *  + r6 - running thread
 *  + r7 - heir thread
 */
#define V850_CONTEXT_CONTROL_R1_OFFSET   0
#define V850_CONTEXT_CONTROL_R3_OFFSET   4
#define V850_CONTEXT_CONTROL_R20_OFFSET  8
#define V850_CONTEXT_CONTROL_R21_OFFSET 12
#define V850_CONTEXT_CONTROL_R22_OFFSET 16
#define V850_CONTEXT_CONTROL_R23_OFFSET 20
#define V850_CONTEXT_CONTROL_R24_OFFSET 24
#define V850_CONTEXT_CONTROL_R25_OFFSET 28
#define V850_CONTEXT_CONTROL_R26_OFFSET 32
#define V850_CONTEXT_CONTROL_R27_OFFSET 36
#define V850_CONTEXT_CONTROL_R28_OFFSET 40
#define V850_CONTEXT_CONTROL_R29_OFFSET 44
#define V850_CONTEXT_CONTROL_R31_OFFSET 48
#define V850_CONTEXT_CONTROL_PSW_OFFSET 52

        .section .text
        .global  __CPU_Context_switch
        .type    __CPU_Context_switch, @function
__CPU_Context_switch:
        st.w     r1,V850_CONTEXT_CONTROL_R1_OFFSET[r6]
        st.w     r3,V850_CONTEXT_CONTROL_R3_OFFSET[r6]
        st.w     r20,V850_CONTEXT_CONTROL_R20_OFFSET[r6]
        st.w     r21,V850_CONTEXT_CONTROL_R21_OFFSET[r6]
        st.w     r22,V850_CONTEXT_CONTROL_R22_OFFSET[r6]
        st.w     r23,V850_CONTEXT_CONTROL_R23_OFFSET[r6]
        st.w     r24,V850_CONTEXT_CONTROL_R24_OFFSET[r6]
        st.w     r25,V850_CONTEXT_CONTROL_R25_OFFSET[r6]
        st.w     r26,V850_CONTEXT_CONTROL_R27_OFFSET[r6]
        st.w     r27,V850_CONTEXT_CONTROL_R27_OFFSET[r6]
        st.w     r28,V850_CONTEXT_CONTROL_R28_OFFSET[r6]
        st.w     r29,V850_CONTEXT_CONTROL_R29_OFFSET[r6]
        st.w     r31,V850_CONTEXT_CONTROL_R31_OFFSET[r6]
	stsr     psw,r21
        st.w     r21,V850_CONTEXT_CONTROL_PSW_OFFSET[r6]
restore:
        ld.w     V850_CONTEXT_CONTROL_R1_OFFSET[r7],r1
        ld.w     V850_CONTEXT_CONTROL_R3_OFFSET[r7],r3
        ld.w     V850_CONTEXT_CONTROL_R20_OFFSET[r7],r20
        ld.w     V850_CONTEXT_CONTROL_R21_OFFSET[r7],r21
        ld.w     V850_CONTEXT_CONTROL_R22_OFFSET[r7],r22
        ld.w     V850_CONTEXT_CONTROL_R23_OFFSET[r7],r23
        ld.w     V850_CONTEXT_CONTROL_R24_OFFSET[r7],r24
        ld.w     V850_CONTEXT_CONTROL_R25_OFFSET[r7],r25
        ld.w     V850_CONTEXT_CONTROL_R27_OFFSET[r7],r26
        ld.w     V850_CONTEXT_CONTROL_R27_OFFSET[r7],r27
        ld.w     V850_CONTEXT_CONTROL_R28_OFFSET[r7],r28
        ld.w     V850_CONTEXT_CONTROL_R29_OFFSET[r7],r29
        ld.w     V850_CONTEXT_CONTROL_R31_OFFSET[r7],r31
        ld.w     V850_CONTEXT_CONTROL_PSW_OFFSET[r7],r7
	ldsr     r7,psw
        jmp      [r31]


/**
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 *  v850 Specific Information:
 *
 *  Move second parameter to first and jump to normal restore
 */
        .section .text
        .global  __CPU_Context_restore
        .type    __CPU_Context_restore, @function
__CPU_Context_restore:
        mov      r6, r7         /* move to second parameter register */
        br       restore

#if 0
/**
 *  This routine provides the RTEMS interrupt management.
 *
 *  v850 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
void _ISR_Handler(void); /* C warning avoidance */
void _ISR_Handler(void)
{
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
   *  _Thread_Dispatch_disable_level--;
   *
   *  --_ISR_Nest_level;
   *
   *  if ( _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  if ( _Thread_Dispatch_disable_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  if ( _Thread_Dispatch_necessary ) {
   *    call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   *    prepare to get out of interrupt
   *    return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if outermost interrupt
   *      restore stack
   *  #endif
   *  prepare to get out of interrupt
   *  return from interrupt
   */
}
#endif
