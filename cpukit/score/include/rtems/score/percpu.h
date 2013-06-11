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
  #include <rtems/score/smplock.h>

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

#ifdef RTEMS_SMP

typedef enum {
  /**
   * @brief The per CPU controls are initialized to zero.
   *
   * In this state the only valid field of the per CPU controls for secondary
   * processors is the per CPU state.  The secondary processors should perform
   * their basic initialization now and change into the
   * PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING state once this is complete.
   *
   * The owner of the per CPU state field is the secondary processor in this
   * state.
   */
  PER_CPU_STATE_BEFORE_INITIALIZATION,

  /**
   * @brief Secondary processor is ready to begin multitasking.
   *
   * The secondary processor performed its basic initialization and is ready to
   * receive inter-processor interrupts.  Interrupt delivery must be disabled
   * in this state, but requested inter-processor interrupts must be recorded
   * and must be delivered once the secondary processor enables interrupts for
   * the first time.  The main processor will wait for all secondary processors
   * to change into this state.  In case a secondary processor does not reach
   * this state the system will not start.  The secondary processors wait now
   * for a change into the PER_CPU_STATE_BEGIN_MULTITASKING state set by the
   * main processor once all secondary processors reached the
   * PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING state.
   *
   * The owner of the per CPU state field is the main processor in this state.
   */
  PER_CPU_STATE_READY_TO_BEGIN_MULTITASKING,

  /**
   * @brief Multitasking begin of secondary processor is requested.
   *
   * The main processor completed system initialization and is about to perform
   * a context switch to its heir thread.  Secondary processors should now
   * issue a context switch to the heir thread.  This normally enables
   * interrupts on the processor for the first time.
   *
   * The owner of the per CPU state field is the secondary processor in this
   * state.
   */
  PER_CPU_STATE_BEGIN_MULTITASKING,

  /**
   * @brief Normal multitasking state.
   *
   * The owner of the per CPU state field is the secondary processor in this
   * state.
   */
  PER_CPU_STATE_UP,

  /**
   * @brief This is the terminal state.
   *
   * The owner of the per CPU state field is the secondary processor in this
   * state.
   */
  PER_CPU_STATE_SHUTDOWN
} Per_CPU_State;

#endif /* RTEMS_SMP */

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
   *  This contains the current interrupt nesting level on this
   *  CPU.
   */
  uint32_t isr_nest_level;

  /** This is set to true when this CPU needs to run the dispatcher. */
  volatile bool dispatch_necessary;

  /** This is the thread executing on this CPU. */
  Thread_Control *executing;

  /** This is the heir thread for this this CPU. */
  Thread_Control *heir;

  /** This is the time of the last context switch on this CPU. */
  Timestamp_Control time_of_last_context_switch;

  #if defined(RTEMS_SMP)
    /** This element is used to lock this structure */
    SMP_lock_Control lock;

    /**
     *  This is the request for the interrupt.
     *
     *  @note This may become a chain protected by atomic instructions.
     */
    uint32_t message;

    /**
     * @brief Indicates the current state of the CPU.
     *
     * This field is not protected by a lock.
     *
     * @see _Per_CPU_Change_state() and _Per_CPU_Wait_for_state().
     */
    Per_CPU_State state;
  #endif
} Per_CPU_Control;
#endif

#if defined(ASM) || defined(_RTEMS_PERCPU_DEFINE_OFFSETS)

#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /*
   *  If this CPU target lets RTEMS allocates the interrupt stack, then
   *  we need to have places in the per CPU table to hold them.
   */
  #define PER_CPU_INTERRUPT_STACK_LOW \
    0
  #define PER_CPU_INTERRUPT_STACK_HIGH \
    PER_CPU_INTERRUPT_STACK_LOW + CPU_SIZEOF_POINTER
  #define PER_CPU_END_STACK             \
    PER_CPU_INTERRUPT_STACK_HIGH + CPU_SIZEOF_POINTER

  #define INTERRUPT_STACK_LOW \
    (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_LOW)
  #define INTERRUPT_STACK_HIGH \
    (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_HIGH)
#else
  #define PER_CPU_END_STACK \
    0
#endif

/*
 *  These are the offsets of the required elements in the per CPU table.
 */
#define PER_CPU_ISR_NEST_LEVEL \
  PER_CPU_END_STACK
#define PER_CPU_DISPATCH_NEEDED \
  PER_CPU_ISR_NEST_LEVEL + 4

#define ISR_NEST_LEVEL \
  (SYM(_Per_CPU_Information) + PER_CPU_ISR_NEST_LEVEL)
#define DISPATCH_NEEDED \
  (SYM(_Per_CPU_Information) + PER_CPU_DISPATCH_NEEDED)

#endif /* defined(ASM) || defined(_RTEMS_PERCPU_DEFINE_OFFSETS) */

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

void _Per_CPU_Change_state(
  Per_CPU_Control *per_cpu,
  Per_CPU_State new_state
);

void _Per_CPU_Wait_for_state(
  const Per_CPU_Control *per_cpu,
  Per_CPU_State desired_state
);

#define _Per_CPU_Lock_acquire( per_cpu, isr_cookie ) \
  _SMP_lock_ISR_disable_and_acquire( &( per_cpu )->lock, isr_cookie )

#define _Per_CPU_Lock_release( per_cpu, isr_cookie ) \
  _SMP_lock_Release_and_ISR_enable( &( per_cpu )->lock, isr_cookie )

#endif

/*
 * On a non SMP system, the bsp_smp_processor_id is defined to 0.
 * Thus when built for non-SMP, there should be no performance penalty.
 */
#define _Thread_Heir \
  _Per_CPU_Information[bsp_smp_processor_id()].heir
#define _Thread_Executing \
  _Per_CPU_Information[bsp_smp_processor_id()].executing
#define _ISR_Nest_level \
  _Per_CPU_Information[bsp_smp_processor_id()].isr_nest_level
#define _CPU_Interrupt_stack_low \
  _Per_CPU_Information[bsp_smp_processor_id()].interrupt_stack_low
#define _CPU_Interrupt_stack_high \
  _Per_CPU_Information[bsp_smp_processor_id()].interrupt_stack_high
#define _Thread_Dispatch_necessary \
  _Per_CPU_Information[bsp_smp_processor_id()].dispatch_necessary
#define _Thread_Time_of_last_context_switch \
  _Per_CPU_Information[bsp_smp_processor_id()].time_of_last_context_switch

#endif  /* ASM */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
