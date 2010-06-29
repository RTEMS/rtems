/**
 *  @file  rtems/score/percpu.h
 *
 *  This include file defines the per CPU information required
 *  by RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_PERCPU_H
#define _RTEMS_PERCPU_H

#ifdef ASM
  #include <rtems/asm.h>
#endif

/**
 *  @defgroup PerCPU RTEMS Per CPU Information
 *
 *  This defines the per CPU state information required by RTEMS
 *  and the BSP.  In an SMP configuration, there will be multiple
 *  instances of this data structure -- one per CPU -- and the
 *  current CPU number will be used as the index.
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM

/**
 * This forward defines the Thread Control Block structure.
 */
typedef struct Thread_Control_struct Thread_Control;

/**
 *  @brief Per CPU Core Structure
 *
 *  This structure is used to hold per core state information.
 */
typedef struct {
#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /**
   * This contains a pointer to the lower range of the interrupt stack for
   * this CPU.  This is the address allocated and freed.
   */
  void  *interrupt_stack_low;

  /**
   * This contains a pointer to the interrupt stack pointer for this CPU.
   * It will be loaded at the beginning on an ISR.
   */
  void  *interrupt_stack_high;
#endif

  /**
   *
   *  This contains the current interrupt nesting level on this
   *  CPU.
   */
  uint32_t isr_nest_level;

  /** This is the thread executing on this CPU. */
  Thread_Control *executing;

  /** This is the heir thread for this this CPU. */
  Thread_Control *heir;

  /** This is the idle thread for this CPU. */
  Thread_Control *idle;

  /** This is set to true when this CPU needs to run the dispatcher. */
  volatile bool dispatch_needed;

} Per_CPU_Control;
#endif

#ifdef ASM

#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /*
   *  If this CPU target lets RTEMS allocates the interrupt stack, then
   *  we need to have places in the per cpu table to hold them.
   */
  #define PER_CPU_INTERRUPT_STACK_LOW   0
  #define PER_CPU_INTERRUPT_STACK_HIGH  (1 * SIZEOF_VOID_P)
  #define PER_CPU_END_STACK             (2 * SIZEOF_VOID_P)
#else
  /*
   *  Otherwise, there are no interrupt stack addresses in the per CPU table.
   */
  #define PER_CPU_END_STACK             0
#endif

/*
 *  These are the offsets of the required elements in the per CPU table.
 */
#define PER_CPU_ISR_NEST_LEVEL   PER_CPU_END_STACK + 0
#define PER_CPU_EXECUTING        PER_CPU_END_STACK + (1 * SIZEOF_VOID_P)
#define PER_CPU_HEIR             PER_CPU_END_STACK + (2 * SIZEOF_VOID_P)
#define PER_CPU_IDLE             PER_CPU_END_STACK + (3 * SIZEOF_VOID_P)
#define PER_CPU_DISPATCH_NEEDED  PER_CPU_END_STACK + (4 * SIZEOF_VOID_P)
#define ISR_NEST_LEVEL \
    (SYM(_Per_CPU_Information) + PER_CPU_ISR_NEST_LEVEL)
#define DISPATCH_NEEDED \
    (SYM(_Per_CPU_Information) + PER_CPU_DISPATCH_NEEDED)

/*
 * Do not define these offsets if they are not in the table.
 */
#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  #define INTERRUPT_STACK_LOW \
      (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_LOW)
  #define INTERRUPT_STACK_HIGH \
      (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_HIGH)
#endif

#endif

#ifndef ASM

/**
 *  @brief Set of Per CPU Core Information
 *
 *  This is an array of per CPU core information.
 */
extern Per_CPU_Control _Per_CPU_Information;

/*
 * On an SMP system, these macros dereference the CPU core number.
 * But on a non-SMP system, these macros are simple references.
 * Thus when built for non-SMP, there should be no performance penalty.
 */
#define _Thread_Heir              _Per_CPU_Information.heir
#define _Thread_Executing         _Per_CPU_Information.executing
#define _Thread_Idle              _Per_CPU_Information.idle
#define _ISR_Nest_level           _Per_CPU_Information.isr_nest_level
#define _CPU_Interrupt_stack_low  _Per_CPU_Information.interrupt_stack_low
#define _CPU_Interrupt_stack_high _Per_CPU_Information.interrupt_stack_high
#define _Context_Switch_necessary _Per_CPU_Information.dispatch_needed

#endif  /* ASM */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
