/**
 *  @file  rtems/score/percpu.h
 *
 *  This include file defines the per CPU information required
 *  by RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_PERCPU_H
#define _RTEMS_PERCPU_H

#include <rtems/score/cpu.h>

#ifdef ASM
  #include <rtems/asm.h>
#else
  #include <rtems/score/isrlevel.h>
  #include <rtems/score/timestamp.h>
  #if defined(RTEMS_SMP)
    #include <rtems/score/smplock.h>
  #endif

  /*
   * NOTE: This file MUST be included on non-smp systems as well
   *       in order to define bsp_smp_processor_id.
   */
  #include <rtems/bspsmp.h>
#endif

/**
 *  @defgroup PerCPU RTEMS Per CPU Information
 *
 *  @ingroup Score
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
#include <rtems/score/timestamp.h>

#ifndef __THREAD_CONTROL_DEFINED__
#define __THREAD_CONTROL_DEFINED__
typedef struct Thread_Control_struct Thread_Control;
#endif

#if (CPU_ALLOCATE_INTERRUPT_STACK == FALSE) && defined(RTEMS_SMP)
  #error "RTEMS must allocate per CPU interrupt stack for SMP"
#endif

typedef enum {

  /**
   *  This defines the constant used to indicate that the cpu code is in
   *  its initial powered up start.
   */
   RTEMS_BSP_SMP_CPU_INITIAL_STATE = 1,

  /**
   *  This defines the constant used to indicate that the cpu code has
   *  completed basic initialization and awaits further commands.
   */
   RTEMS_BSP_SMP_CPU_INITIALIZED = 2,

  /**
   *  This defines the constant used to indicate that the cpu code has
   *  completed basic initialization and awaits further commands.
   */
  RTEMS_BSP_SMP_CPU_UP = 3,

  /**
   *  This defines the constant used to indicate that the cpu code has
   *  shut itself down.
   */
  RTEMS_BSP_SMP_CPU_SHUTDOWN = 4
} bsp_smp_cpu_state;

/**
 *  @brief Per CPU Core Structure
 *
 *  This structure is used to hold per core state information.
 */
typedef struct {
  #if defined(RTEMS_SMP)
    /** This element is used to lock this structure */
    SMP_lock_spinlock_simple_Control  lock;

    /** This indicates that the CPU is online. */
    uint32_t                          state;

    /**
     *  This is the request for the interrupt.
     *
     *  @note This may become a chain protected by atomic instructions.
     */
    uint32_t                          message;
  #endif

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
  volatile bool dispatch_necessary;

  /** This is the time of the last context switch on this CPU. */
  Timestamp_Control time_of_last_context_switch;
} Per_CPU_Control;
#endif

#ifdef ASM
#if defined(RTEMS_SMP)
  #define PER_CPU_LOCK     0
  #define PER_CPU_STATE    (1 * __RTEMS_SIZEOF_VOID_P__)
  #define PER_CPU_MESSAGE  (2 * __RTEMS_SIZEOF_VOID_P__)
  #define PER_CPU_END_SMP  (3 * __RTEMS_SIZEOF_VOID_P__)
#else
  #define PER_CPU_END_SMP  0
#endif

#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /*
   *  If this CPU target lets RTEMS allocates the interrupt stack, then
   *  we need to have places in the per cpu table to hold them.
   */
  #define PER_CPU_INTERRUPT_STACK_LOW   PER_CPU_END_SMP
  #define PER_CPU_INTERRUPT_STACK_HIGH  \
          PER_CPU_INTERRUPT_STACK_LOW + (1 * __RTEMS_SIZEOF_VOID_P__)
  #define PER_CPU_END_STACK             \
          PER_CPU_INTERRUPT_STACK_HIGH + (1 * __RTEMS_SIZEOF_VOID_P__)
#else
  #define PER_CPU_END_STACK             PER_CPU_END_SMP
#endif

/*
 *  These are the offsets of the required elements in the per CPU table.
 */
#define PER_CPU_ISR_NEST_LEVEL \
          PER_CPU_END_STACK + 0
#define PER_CPU_EXECUTING \
          PER_CPU_END_STACK + (1 * __RTEMS_SIZEOF_VOID_P__)
#define PER_CPU_HEIR \
          PER_CPU_END_STACK + (2 * __RTEMS_SIZEOF_VOID_P__)
#define PER_CPU_IDLE \
          PER_CPU_END_STACK + (3 * __RTEMS_SIZEOF_VOID_P__)
#define PER_CPU_DISPATCH_NEEDED \
	  PER_CPU_END_STACK + (4 * __RTEMS_SIZEOF_VOID_P__)

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
extern Per_CPU_Control _Per_CPU_Information[] CPU_STRUCTURE_ALIGNMENT;

#if defined(RTEMS_SMP)
/**
 *  @brief Set of Pointers to Per CPU Core Information
 *
 *  This is an array of pointers to each CPU's per CPU data structure.
 *  It should be simpler to retrieve this pointer in assembly language
 *  that to calculate the array offset.
 */
extern Per_CPU_Control *_Per_CPU_Information_p[];

/**
 *  @brief Initialize SMP Handler
 *
 *  This method initialize the SMP Handler.
 */
void _SMP_Handler_initialize(void);

/**
 *  @brief Allocate and Initialize Per CPU Structures
 *
 *  This method allocates and initialize the per CPU structure.
 */
void _Per_CPU_Initialize(void);

#endif

/*
 * On a non SMP system, the bsp_smp_processor_id is defined to 0.
 * Thus when built for non-SMP, there should be no performance penalty.
 */
#define _Thread_Heir \
  _Per_CPU_Information[bsp_smp_processor_id()].heir
#define _Thread_Executing \
  _Per_CPU_Information[bsp_smp_processor_id()].executing
#define _Thread_Idle \
  _Per_CPU_Information[bsp_smp_processor_id()].idle
#define _ISR_Nest_level \
  _Per_CPU_Information[bsp_smp_processor_id()].isr_nest_level
#define _CPU_Interrupt_stack_low \
  _Per_CPU_Information[bsp_smp_processor_id()].interrupt_stack_low
#define _CPU_Interrupt_stack_high \
  _Per_CPU_Information[bsp_smp_processor_id()].interrupt_stack_high
#define _Thread_Dispatch_necessary \
  _Per_CPU_Information[bsp_smp_processor_id()].dispatch_necessary
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #define _Thread_Time_of_last_context_switch \
    _Per_CPU_Information[bsp_smp_processor_id()].time_of_last_context_switch
#endif


#endif  /* ASM */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
