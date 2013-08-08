/**
 *  @file  rtems/score/thread.h
 *
 *  @brief Constants and Structures Related with the Thread Control Block
 *
 *  This include file contains all constants and structures associated
 *  with the thread control block.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREAD_H
#define _RTEMS_SCORE_THREAD_H

#include <rtems/score/context.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mppkt.h>
#endif
#include <rtems/score/object.h>
#include <rtems/score/percpu.h>
#include <rtems/score/priority.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/threadq.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreThread Thread Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the management of
 *  threads.  This includes the creation, deletion, and scheduling of threads.
 *
 *  The following variables are maintained as part of the per cpu data
 *  structure.
 *
 *  + Idle thread pointer
 *  + Executing thread pointer
 *  + Heir thread pointer
 */
/**@{*/

#if defined(RTEMS_POSIX_API)
  #define RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE
#endif

/*
 * With the addition of the Constant Block Scheduler (CBS),
 * this feature is needed even when POSIX is disabled.
 */
#define RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT

#if defined(RTEMS_POSIX_API)
  #define RTEMS_SCORE_THREAD_ENABLE_USER_PROVIDED_STACK_VIA_API
#endif

/*
 *  The user can define this at configure time and go back to ticks
 *  resolution.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timestamp.h>

  typedef Timestamp_Control Thread_CPU_usage_t;
#else
  typedef uint32_t Thread_CPU_usage_t;
#endif

/**
 *  The following defines the "return type" of a thread.
 *
 *  @note  This cannot always be right.  Some APIs have void
 *         tasks/threads, others return pointers, others may
 *         return a numeric value.  Hopefully a pointer is
 *         always at least as big as an uint32_t  . :)
 */
typedef void *Thread;

/**
 *  @brief Type of the numeric argument of a thread entry function with at
 *  least one numeric argument.
 *
 *  This numeric argument type designates an unsigned integer type with the
 *  property that any valid pointer to void can be converted to this type and
 *  then converted back to a pointer to void.  The result will compare equal to
 *  the original pointer.
 */
typedef uintptr_t Thread_Entry_numeric_type;

/**
 *  The following defines the ways in which the entry point for a
 *  thread can be invoked.  Basically, it can be passed any
 *  combination/permutation of a pointer and an uint32_t   value.
 *
 *  @note For now, we are ignoring the return type.
 */
typedef enum {
  THREAD_START_NUMERIC,
  THREAD_START_POINTER,
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    THREAD_START_BOTH_POINTER_FIRST,
    THREAD_START_BOTH_NUMERIC_FIRST
  #endif
} Thread_Start_types;

/** This type corresponds to a very simple style thread entry point. */
typedef Thread ( *Thread_Entry )( void );   /* basic type */

/** This type corresponds to a thread entry point which takes a single
 *  unsigned thirty-two bit integer as an argument.
 */
typedef Thread ( *Thread_Entry_numeric )( Thread_Entry_numeric_type );

/** This type corresponds to a thread entry point which takes a single
 *  untyped pointer as an argument.
 */
typedef Thread ( *Thread_Entry_pointer )( void * );

/** This type corresponds to a thread entry point which takes a single
 *  untyped pointer and an unsigned thirty-two bit integer as arguments.
 */
typedef Thread ( *Thread_Entry_both_pointer_first )( void *, Thread_Entry_numeric_type );

/** This type corresponds to a thread entry point which takes a single
 *  unsigned thirty-two bit integer and an untyped pointer and an
 *  as arguments.
 */
typedef Thread ( *Thread_Entry_both_numeric_first )( Thread_Entry_numeric_type, void * );

/**
 *  The following lists the algorithms used to manage the thread cpu budget.
 *
 *  Reset Timeslice:   At each context switch, reset the time quantum.
 *  Exhaust Timeslice: Only reset the quantum once it is consumed.
 *  Callout:           Execute routine when budget is consumed.
 */
typedef enum {
  THREAD_CPU_BUDGET_ALGORITHM_NONE,
  THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE,
  #if defined(RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE)
    THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE,
  #endif
  #if defined(RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT)
    THREAD_CPU_BUDGET_ALGORITHM_CALLOUT
  #endif
}  Thread_CPU_budget_algorithms;

/**  This defines thes the entry point for the thread specific timeslice
 *   budget management algorithm.
 */
typedef void (*Thread_CPU_budget_algorithm_callout )( Thread_Control * );

/**
 *  @brief Forward reference to the per task variable structure..
 *
 *  Forward reference to the per task variable structure.
 */
struct rtems_task_variable_tt;

/**
 *  @brief Internal structure used to manager per task variables.
 *
 *  This is the internal structure used to manager per Task Variables.
 */
typedef struct {
  /** This field points to the next per task variable for this task. */
  struct rtems_task_variable_tt  *next;
  /** This field points to the physical memory location of this per
   *  task variable.
   */
  void                          **ptr;
  /** This field is to the global value for this per task variable. */
  void                           *gval;
  /** This field is to this thread's value for this per task variable. */
  void                           *tval;
  /** This field points to the destructor for this per task variable. */
  void                          (*dtor)(void *);
} rtems_task_variable_t;

/**
 *  The following structure contains the information which defines
 *  the starting state of a thread.
 */
typedef struct {
  /** This field is the starting address for the thread. */
  Thread_Entry                         entry_point;
  /** This field indicates the how task is invoked. */
  Thread_Start_types                   prototype;
  /** This field is the pointer argument passed at thread start. */
  void                                *pointer_argument;
  /** This field is the numeric argument passed at thread start. */
  Thread_Entry_numeric_type            numeric_argument;
  /*-------------- initial execution modes ----------------- */
  /** This field indicates whether the thread was preemptible when
    * it started.
    */
  bool                                 is_preemptible;
  /** This field indicates the CPU budget algorith. */
  Thread_CPU_budget_algorithms         budget_algorithm;
  /** This field is the routine to invoke when the CPU allotment is
   *  consumed.
   */
  Thread_CPU_budget_algorithm_callout  budget_callout;
  /** This field is the initial ISR disable level of this thread. */
  uint32_t                             isr_level;
  /** This field is the initial priority. */
  Priority_Control                     initial_priority;
  #if defined(RTEMS_SCORE_THREAD_ENABLE_USER_PROVIDED_STACK_VIA_API)
    /** This field indicates whether the SuperCore allocated the stack. */
    bool                                 core_allocated_stack;
  #endif
  /** This field is the stack information. */
  Stack_Control                        Initial_stack;
  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    /** This field is the initial FP context area address. */
    Context_Control_fp                  *fp_context;
  #endif
  /** This field is the initial stack area address. */
  void                                *stack;
} Thread_Start_information;

/**
 *  @brief Union type to hold a pointer to an immutable or a mutable object.
 *
 *  The main purpose is to enable passing of pointers to read-only send buffers
 *  in the message passing subsystem.  This approach is somewhat fragile since
 *  it prevents the compiler to check if the operations on objects are valid
 *  with respect to the constant qualifier.  An alternative would be to add a
 *  third pointer argument for immutable objects, but this would increase the
 *  structure size.
 */
typedef union {
  void       *mutable_object;
  const void *immutable_object;
} Thread_Wait_information_Object_argument_type;

/**
 *  @brief Information required to manage a thread while it is blocked.
 *
 *  This contains the information required to manage a thread while it is
 *  blocked and to return information to it.
 */
typedef struct {
  /** This field is the Id of the object this thread is waiting upon. */
  Objects_Id            id;
  /** This field is used to return an integer while when blocked. */
  uint32_t              count;
  /** This field is for a pointer to a user return argument. */
  void                 *return_argument;
  /** This field is for a pointer to a second user return argument. */
  Thread_Wait_information_Object_argument_type
                        return_argument_second;
  /** This field contains any options in effect on this blocking operation. */
  uint32_t              option;
  /** This field will contain the return status from a blocking operation.
   *
   *  @note The following assumes that all API return codes can be
   *        treated as an uint32_t.
   */
  uint32_t              return_code;

  /** This field is the chain header for the second through Nth tasks
   *  of the same priority blocked waiting on the same object.
   */
  Chain_Control         Block2n;
  /** This field points to the thread queue on which this thread is blocked. */
  Thread_queue_Control *queue;
}   Thread_Wait_information;

/**
 *  The following defines the control block used to manage
 *  each thread proxy.
 *
 *  @note It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */
typedef struct {
  /** This field is the object management structure for each proxy. */
  Objects_Control          Object;
  /** This field is the current execution state of this proxy. */
  States_Control           current_state;
  /** This field is the current priority state of this proxy. */
  Priority_Control         current_priority;
  /** This field is the base priority of this proxy. */
  Priority_Control         real_priority;
  /** This field is the number of mutexes currently held by this proxy. */
  uint32_t                 resource_count;

  /** This field is the blocking information for this proxy. */
  Thread_Wait_information  Wait;
  /** This field is the Watchdog used to manage proxy delays and timeouts. */
  Watchdog_Control         Timer;
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is the received response packet in an MP system. */
  MP_packet_Prefix        *receive_packet;
#endif
     /****************** end of common block ********************/
  /** This field is used to manage the set of proxies in the system. */
  Chain_Node               Active;
}   Thread_Proxy_control;

/**
 *  The following record defines the control block used
 *  to manage each thread.
 *
 *  @note It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */
typedef enum {
  /** This value is for the Classic RTEMS API. */
  THREAD_API_RTEMS,
  /** This value is for the POSIX API. */
  THREAD_API_POSIX
}  Thread_APIs;

/** This macro defines the first API which has threads. */
#define THREAD_API_FIRST THREAD_API_RTEMS

/** This macro defines the last API which has threads. */
#define THREAD_API_LAST  THREAD_API_POSIX

/**
 *  This structure defines the Thread Control Block (TCB).
 */
struct Thread_Control_struct {
  /** This field is the object management structure for each thread. */
  Objects_Control          Object;
  /** This field is the current execution state of this thread. */
  States_Control           current_state;
  /** This field is the current priority state of this thread. */
  Priority_Control         current_priority;
  /** This field is the base priority of this thread. */
  Priority_Control         real_priority;
  /** This field is the number of mutexes currently held by this thread. */
  uint32_t                 resource_count;
  /** This field is the blocking information for this thread. */
  Thread_Wait_information  Wait;
  /** This field is the Watchdog used to manage thread delays and timeouts. */
  Watchdog_Control         Timer;
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is the received response packet in an MP system. */
  MP_packet_Prefix        *receive_packet;
#endif
#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  /** This field is the head of queue of priority inheritance mutex
   *  held by the thread.
   */
  Chain_Control            lock_mutex;
#endif
     /*================= end of common block =================*/
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is true if the thread is offered globally */
  bool                                  is_global;
#endif
  /** This field is true if the thread is preemptible. */
  bool                                  is_preemptible;
#if defined(RTEMS_SMP)
  /**
   * @brief This field is true if the thread is scheduled.
   *
   * A thread is scheduled if it is ready and the scheduler allocated a
   * processor for it.  A scheduled thread is assigned to exactly one
   * processor.  There are exactly processor count scheduled threads in the
   * system.
   */
  bool                                  is_scheduled;

  /**
   * @brief This field is true if the thread is in the air.
   *
   * A thread is in the air if it has an allocated processor (it is an
   * executing or heir thread on exactly one processor) and it is not a member
   * of the scheduled chain.  The extract operation on a scheduled thread will
   * produce threads in the air (see also _Thread_Set_transient()).  The next
   * enqueue or schedule operation will decide what to do based on this state
   * indication.  It can either place the thread back on the scheduled chain
   * and the thread can keep its allocated processor, or it can take the
   * processor away from the thread and give the processor to another thread of
   * higher priority.
   */
  bool                                  is_in_the_air;

  /**
   * @brief This field is true if the thread is executing.
   *
   * A thread is executing if it executes on a processor.  An executing thread
   * executes on exactly one processor.  There are exactly processor count
   * executing threads in the system.  An executing thread may have a heir
   * thread and thread dispatching is necessary.  On SMP a thread dispatch on a
   * remote processor needs help from an inter-processor interrupt, thus it
   * will take some time to complete the state change.  A lot of things can
   * happen in the meantime.
   */
  bool                                  is_executing;
#endif
#if __RTEMS_ADA__
  /** This field is the GNAT self context pointer. */
  void                                 *rtems_ada_self;
#endif
  /** This field is the length of the time quantum that this thread is
   *  allowed to consume.  The algorithm used to manage limits on CPU usage
   *  is specified by budget_algorithm.
   */
  uint32_t                              cpu_time_budget;
  /** This field is the algorithm used to manage this thread's time
   *  quantum.  The algorithm may be specified as none which case,
   *  no limit is in place.
   */
  Thread_CPU_budget_algorithms          budget_algorithm;
  /** This field is the method invoked with the budgeted time is consumed. */
  Thread_CPU_budget_algorithm_callout   budget_callout;
  /** This field is the amount of CPU time consumed by this thread
   *  since it was created.
   */
  Thread_CPU_usage_t                    cpu_time_used;

  /** This pointer holds per-thread data for the scheduler and ready queue. */
  void                                 *scheduler_info;

#ifdef RTEMS_SMP
  Per_CPU_Control                      *cpu;
#endif

  /** This field contains information about the starting state of
   *  this thread.
   */
  Thread_Start_information              Start;
  /** This field contains the context of this thread. */
  Context_Control                       Registers;
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  /** This field points to the floating point context for this thread.
   *  If NULL, the thread is integer only.
   */
  Context_Control_fp                   *fp_context;
#endif
  /** This field points to the newlib reentrancy structure for this thread. */
  struct _reent                        *libc_reent;
  /** This array contains the API extension area pointers. */
  void                                 *API_Extensions[ THREAD_API_LAST + 1 ];
  /** This field points to the user extension pointers. */
  void                                **extensions;
  /** This field points to the set of per task variables. */
  rtems_task_variable_t                *task_variables;
};

#if (CPU_PROVIDES_IDLE_THREAD_BODY == FALSE)
/**
 *  This routine is the body of the system idle thread.
 *
 *  NOTE: This routine is actually instantiated by confdefs.h when needed.
 */
void *_Thread_Idle_body(
  uintptr_t  ignored
);
#endif

/**  This defines the type for a method which operates on a single thread.
 */
typedef void (*rtems_per_thread_routine)( Thread_Control * );

/**
 *  @brief Iterates over all threads.
 *  This routine iterates over all threads regardless of API and
 *  invokes the specified routine.
 */
void rtems_iterate_over_all_threads(
  rtems_per_thread_routine routine
);

/**
 * @brief Returns the thread control block of the executing thread.
 *
 * This function can be called in any context.  On SMP configurations
 * interrupts are disabled to ensure that the processor index is used
 * consistently.
 *
 * @return The thread control block of the executing thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Get_executing( void )
{
  Thread_Control *executing;

  #if defined( RTEMS_SMP )
    ISR_Level level;

    _ISR_Disable( level );
  #endif

  executing = _Thread_Executing;

  #if defined( RTEMS_SMP )
    _ISR_Enable( level );
  #endif

  return executing;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
