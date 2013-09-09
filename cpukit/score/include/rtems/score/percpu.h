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

#if defined( ASM )
  #include <rtems/asm.h>
#else
  #include <rtems/score/assert.h>
  #include <rtems/score/isrlock.h>
  #include <rtems/score/timestamp.h>
  #include <rtems/score/smp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined( RTEMS_SMP )
  /*
   * This ensures that on SMP configurations the individual per-CPU controls
   * are on different cache lines to prevent false sharing.  This define can be
   * used in assembler code to easily get the per-CPU control for a particular
   * processor.
   */
  #define PER_CPU_CONTROL_SIZE_LOG2 7

  #define PER_CPU_CONTROL_SIZE ( 1 << PER_CPU_CONTROL_SIZE_LOG2 )
#endif

#if !defined( ASM )

#ifndef __THREAD_CONTROL_DEFINED__
#define __THREAD_CONTROL_DEFINED__
typedef struct Thread_Control_struct Thread_Control;
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

#if defined( RTEMS_SMP )

#if CPU_USE_DEFERRED_FP_SWITCH == TRUE
  #error "deferred FP switch not implemented for SMP"
#endif

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

#endif /* defined( RTEMS_SMP ) */

/**
 *  @brief Per CPU Core Structure
 *
 *  This structure is used to hold per core state information.
 */
typedef struct {
  /**
   * @brief CPU port specific control.
   */
  CPU_Per_CPU_control cpu_per_cpu;

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

  /**
   * @brief The thread dispatch critical section nesting counter which is used
   * to prevent context switches at inopportune moments.
   */
  volatile uint32_t thread_dispatch_disable_level;

  /** This is set to true when this CPU needs to run the dispatcher. */
  volatile bool dispatch_necessary;

  /** This is the thread executing on this CPU. */
  Thread_Control *executing;

  /** This is the heir thread for this this CPU. */
  Thread_Control *heir;

  /** This is the time of the last context switch on this CPU. */
  Timestamp_Control time_of_last_context_switch;

  /**
   * @brief This lock protects the dispatch_necessary, executing, heir and
   * message fields.
   */
  ISR_lock_Control lock;

  #if defined( RTEMS_SMP )
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

#if defined( RTEMS_SMP )
typedef struct {
  Per_CPU_Control per_cpu;
  char unused_space_for_cache_line_alignment
    [ PER_CPU_CONTROL_SIZE - sizeof( Per_CPU_Control ) ];
} Per_CPU_Control_envelope;
#else
typedef struct {
  Per_CPU_Control per_cpu;
} Per_CPU_Control_envelope;
#endif

/**
 *  @brief Set of Per CPU Core Information
 *
 *  This is an array of per CPU core information.
 */
extern Per_CPU_Control_envelope _Per_CPU_Information[] CPU_STRUCTURE_ALIGNMENT;

#define _Per_CPU_ISR_disable_and_acquire( per_cpu, isr_cookie ) \
  _ISR_lock_ISR_disable_and_acquire( &( per_cpu )->lock, isr_cookie )

#define _Per_CPU_Release_and_ISR_enable( per_cpu, isr_cookie ) \
  _ISR_lock_Release_and_ISR_enable( &( per_cpu )->lock, isr_cookie )

#define _Per_CPU_Acquire( per_cpu ) \
  _ISR_lock_Acquire( &( per_cpu )->lock )

#define _Per_CPU_Release( per_cpu ) \
  _ISR_lock_Release( &( per_cpu )->lock )

#if defined( RTEMS_SMP )
#define _Per_CPU_Acquire_all( isr_cookie ) \
  do { \
    uint32_t ncpus = _SMP_Get_processor_count(); \
    uint32_t cpu; \
    _ISR_Disable( isr_cookie ); \
    for ( cpu = 0 ; cpu < ncpus ; ++cpu ) { \
      _Per_CPU_Acquire( _Per_CPU_Get_by_index( cpu ) ); \
    } \
  } while ( 0 )
#else
#define _Per_CPU_Acquire_all( isr_cookie ) \
  _ISR_Disable( isr_cookie )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_Release_all( isr_cookie ) \
  do { \
    uint32_t ncpus = _SMP_Get_processor_count(); \
    uint32_t cpu; \
    for ( cpu = 0 ; cpu < ncpus ; ++cpu ) { \
      _Per_CPU_Release( _Per_CPU_Get_by_index( cpu ) ); \
    } \
    _ISR_Enable( isr_cookie ); \
  } while ( 0 )
#else
#define _Per_CPU_Release_all( isr_cookie ) \
  _ISR_Enable( isr_cookie )
#endif

#if defined( RTEMS_SMP )
static inline Per_CPU_Control *_Per_CPU_Get( void )
{
  Per_CPU_Control *per_cpu =
    &_Per_CPU_Information[ _SMP_Get_current_processor() ].per_cpu;

  _Assert(
    per_cpu->thread_dispatch_disable_level != 0 || _ISR_Get_level() != 0
  );

  return per_cpu;
}
#else
#define _Per_CPU_Get() ( &_Per_CPU_Information[ 0 ].per_cpu )
#endif

static inline Per_CPU_Control *_Per_CPU_Get_by_index( uint32_t index )
{
  return &_Per_CPU_Information[ index ].per_cpu;
}

static inline uint32_t _Per_CPU_Get_index( const Per_CPU_Control *per_cpu )
{
  const Per_CPU_Control_envelope *per_cpu_envelope =
    ( const Per_CPU_Control_envelope * ) per_cpu;

  return ( uint32_t ) ( per_cpu_envelope - &_Per_CPU_Information[ 0 ] );
}

#if defined( RTEMS_SMP )

static inline void _Per_CPU_Send_interrupt( const Per_CPU_Control *per_cpu )
{
  _CPU_SMP_Send_interrupt( _Per_CPU_Get_index( per_cpu ) );
}

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

#endif /* defined( RTEMS_SMP ) */

/*
 * On a non SMP system, the _SMP_Get_current_processor() is defined to 0.
 * Thus when built for non-SMP, there should be no performance penalty.
 */
#define _Thread_Dispatch_disable_level \
  _Per_CPU_Get()->thread_dispatch_disable_level
#define _Thread_Heir \
  _Per_CPU_Get()->heir
#define _Thread_Executing \
  _Per_CPU_Get()->executing
#define _ISR_Nest_level \
  _Per_CPU_Get()->isr_nest_level
#define _CPU_Interrupt_stack_low \
  _Per_CPU_Get()->interrupt_stack_low
#define _CPU_Interrupt_stack_high \
  _Per_CPU_Get()->interrupt_stack_high
#define _Thread_Dispatch_necessary \
  _Per_CPU_Get()->dispatch_necessary
#define _Thread_Time_of_last_context_switch \
  _Per_CPU_Get()->time_of_last_context_switch

/**@}*/

#endif /* !defined( ASM ) */

#if defined( ASM ) || defined( _RTEMS_PERCPU_DEFINE_OFFSETS )

#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /*
   *  If this CPU target lets RTEMS allocates the interrupt stack, then
   *  we need to have places in the per CPU table to hold them.
   */
  #define PER_CPU_INTERRUPT_STACK_LOW \
    CPU_PER_CPU_CONTROL_SIZE
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
    CPU_PER_CPU_CONTROL_SIZE
#endif

/*
 *  These are the offsets of the required elements in the per CPU table.
 */
#define PER_CPU_ISR_NEST_LEVEL \
  PER_CPU_END_STACK
#define PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL \
  PER_CPU_ISR_NEST_LEVEL + 4
#define PER_CPU_DISPATCH_NEEDED \
  PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL + 4

#define THREAD_DISPATCH_DISABLE_LEVEL \
  (SYM(_Per_CPU_Information) + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)
#define ISR_NEST_LEVEL \
  (SYM(_Per_CPU_Information) + PER_CPU_ISR_NEST_LEVEL)
#define DISPATCH_NEEDED \
  (SYM(_Per_CPU_Information) + PER_CPU_DISPATCH_NEEDED)

#endif /* defined( ASM ) || defined( _RTEMS_PERCPU_DEFINE_OFFSETS ) */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
