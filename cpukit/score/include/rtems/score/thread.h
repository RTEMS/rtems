/**
 *  @file  rtems/score/thread.h
 *
 *  @brief Constants and Structures Related with the Thread Control Block
 *
 *  This include file contains all constants and structures associated
 *  with the thread control block.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREAD_H
#define _RTEMS_SCORE_THREAD_H

#include <rtems/score/atomic.h>
#include <rtems/score/context.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mppkt.h>
#endif
#include <rtems/score/isrlock.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/resource.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/threadq.h>
#include <rtems/score/watchdog.h>

#if defined(RTEMS_SMP)
  #include <rtems/score/cpuset.h>
#endif

struct Per_CPU_Control;

struct Scheduler_Control;

struct Scheduler_Node;

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
#include <rtems/score/timestamp.h>

typedef Timestamp_Control Thread_CPU_usage_t;

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
typedef CPU_Uint32ptr Thread_Entry_numeric_type;

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

#if !defined(RTEMS_SMP)
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
#endif

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
  /** The thread-local storage (TLS) area */
  void                                *tls_area;
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
 * @brief This type is able to contain several flags used to control the wait
 * class and state of a thread.
 *
 * The mutually exclusive wait class flags are
 * - @ref THREAD_WAIT_CLASS_EVENT,
 * - @ref THREAD_WAIT_CLASS_SYSTEM_EVENT, and
 * - @ref THREAD_WAIT_CLASS_OBJECT.
 *
 * The mutually exclusive wait state flags are
 * - @ref THREAD_WAIT_STATE_INTEND_TO_BLOCK,
 * - @ref THREAD_WAIT_STATE_BLOCKED, and
 * - @ref THREAD_WAIT_STATE_READY_AGAIN.
 */
typedef unsigned int Thread_Wait_flags;

/**
 *  @brief Information required to manage a thread while it is blocked.
 *
 *  This contains the information required to manage a thread while it is
 *  blocked and to return information to it.
 */
typedef struct {
  /**
   * @brief Node for thread queues.
   */
  union {
    /**
     * @brief A node for chains.
     */
    Chain_Node Chain;

    /**
     * @brief A node for red-black trees.
     */
    RBTree_Node RBTree;
  } Node;

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

  /**
   * @brief Code to set the timeout return code in _Thread_Timeout().
   */
  uint32_t timeout_code;

  /**
   * @brief The current thread queue.
   *
   * In case this field is @c NULL, then the thread is not blocked on a thread
   * queue.  This field is protected by the thread lock.
   *
   * @see _Thread_Lock_set() and _Thread_Wait_set_queue().
   */
  Thread_queue_Control *queue;

  /**
   * @brief This field contains several flags used to control the wait class
   * and state of a thread in case fine-grained locking is used.
   */
#if defined(RTEMS_SMP)
  Atomic_Uint           flags;
#else
  Thread_Wait_flags     flags;
#endif

  /**
   * @brief The current thread queue operations.
   *
   * This field is protected by the thread lock.
   *
   * @see _Thread_Lock_set() and _Thread_Wait_set_operations().
   */
  const Thread_queue_Operations *operations;
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

  /**
   * @brief This field is the current priority state of this thread.
   *
   * Writes to this field are only allowed in _Thread_Initialize() or via
   * _Thread_Change_priority().
   */
  Priority_Control         current_priority;

  /**
   * @brief This field is the base priority of this thread.
   *
   * Writes to this field are only allowed in _Thread_Initialize() or via
   * _Thread_Change_priority().
   */
  Priority_Control         real_priority;

  /**
   * @brief Generation of the current priority value.
   *
   * It is used in _Thread_Change_priority() to serialize the update of
   * priority related data structures.
   */
  uint32_t                 priority_generation;

  /**
   * @brief Hints if a priority restore is necessary once the resource count
   * changes from one to zero.
   *
   * This is an optimization to speed up the mutex surrender sequence in case
   * no attempt to change the priority was made during the mutex ownership.  On
   * SMP configurations atomic fences must synchronize writes to
   * Thread_Control::priority_restore_hint and Thread_Control::resource_count.
   */
  bool                     priority_restore_hint;

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

typedef struct Thread_Action Thread_Action;

/**
 * @brief Thread action handler.
 *
 * The thread action handler will be called with interrupts disabled and the
 * thread action lock acquired.  The handler must release the thread action
 * lock with _Thread_Action_release_and_ISR_enable().  So the thread action
 * lock can be used to protect private data fields of the particular action.
 *
 * Since the action is passed to the handler private data fields can be added
 * below the common thread action fields.
 *
 * @param[in] thread The thread performing the action.
 * @param[in] action The thread action.
 * @param[in] cpu The processor of the thread.
 * @param[in] level The ISR level for _Thread_Action_release_and_ISR_enable().
 */
typedef void ( *Thread_Action_handler )(
  Thread_Control         *thread,
  Thread_Action          *action,
  struct Per_CPU_Control *cpu,
  ISR_Level               level
);

/**
 * @brief Thread action.
 *
 * Thread actions can be chained together to trigger a set of actions on
 * particular events like for example a thread post-switch.  Use
 * _Thread_Action_initialize() to initialize this structure.
 *
 * Thread actions are the building block for efficient implementation of
 * - Classic signals delivery,
 * - POSIX signals delivery,
 * - thread restart notification,
 * - thread delete notification,
 * - forced thread migration on SMP configurations, and
 * - the Multiprocessor Resource Sharing Protocol (MrsP).
 *
 * @see _Thread_Run_post_switch_actions().
 */
struct Thread_Action {
  Chain_Node            Node;
  Thread_Action_handler handler;
};

/**
 * @brief Control block to manage thread actions.
 *
 * Use _Thread_Action_control_initialize() to initialize this structure.
 */
typedef struct {
  Chain_Control Chain;
} Thread_Action_control;

/**
 * @brief Thread life states.
 *
 * The thread life states are orthogonal to the thread states used for
 * synchronization primitives and blocking operations.  They reflect the state
 * changes triggered with thread restart and delete requests.
 */
typedef enum {
  THREAD_LIFE_NORMAL = 0x0,
  THREAD_LIFE_PROTECTED = 0x1,
  THREAD_LIFE_RESTARTING = 0x2,
  THREAD_LIFE_PROTECTED_RESTARTING = 0x3,
  THREAD_LIFE_TERMINATING = 0x4,
  THREAD_LIFE_PROTECTED_TERMINATING = 0x5,
  THREAD_LIFE_RESTARTING_TERMINATING = 0x6,
  THREAD_LIFE_PROTECTED_RESTARTING_TERMINATING = 0x7
} Thread_Life_state;

/**
 * @brief Thread life control.
 */
typedef struct {
  /**
   * @brief Thread life action used to react upon thread restart and delete
   * requests.
   */
  Thread_Action      Action;

  /**
   * @brief The current thread life state.
   */
  Thread_Life_state  state;

  /**
   * @brief The terminator thread of this thread.
   *
   * In case the thread is terminated and another thread (the terminator) waits
   * for the actual termination completion, then this field references the
   * terminator thread.
   */
  Thread_Control    *terminator;
} Thread_Life_control;

#if defined(RTEMS_SMP)
/**
 * @brief The thread state with respect to the scheduler.
 */
typedef enum {
  /**
   * @brief This thread is blocked with respect to the scheduler.
   *
   * This thread uses no scheduler nodes.
   */
  THREAD_SCHEDULER_BLOCKED,

  /**
   * @brief This thread is scheduled with respect to the scheduler.
   *
   * This thread executes using one of its scheduler nodes.  This could be its
   * own scheduler node or in case it owns resources taking part in the
   * scheduler helping protocol a scheduler node of another thread.
   */
  THREAD_SCHEDULER_SCHEDULED,

  /**
   * @brief This thread is ready with respect to the scheduler.
   *
   * None of the scheduler nodes of this thread is scheduled.
   */
  THREAD_SCHEDULER_READY
} Thread_Scheduler_state;
#endif

/**
 * @brief Thread scheduler control.
 */
typedef struct {
#if defined(RTEMS_SMP)
  /**
   * @brief The current scheduler state of this thread.
   */
  Thread_Scheduler_state state;

  /**
   * @brief The own scheduler control of this thread.
   *
   * This field is constant after initialization.
   */
  const struct Scheduler_Control *own_control;

  /**
   * @brief The scheduler control of this thread.
   *
   * The scheduler helping protocol may change this field.
   */
  const struct Scheduler_Control *control;

  /**
   * @brief The own scheduler node of this thread.
   *
   * This field is constant after initialization.  It is used by change
   * priority and ask for help operations.
   */
  struct Scheduler_Node *own_node;
#endif

  /**
   * @brief The scheduler node of this thread.
   *
   * On uni-processor configurations this field is constant after
   * initialization.
   *
   * On SMP configurations the scheduler helping protocol may change this
   * field.
   */
  struct Scheduler_Node *node;

#if defined(RTEMS_SMP)
  /**
   * @brief The processor assigned by the current scheduler.
   */
  struct Per_CPU_Control *cpu;

#if defined(RTEMS_DEBUG)
  /**
   * @brief The processor on which this thread executed the last time or is
   * executing.
   */
  struct Per_CPU_Control *debug_real_cpu;
#endif
#endif
} Thread_Scheduler_control;

typedef struct  {
  uint32_t      flags;
  void *        control;
}Thread_Capture_control;

#if defined(RTEMS_SMP)
/**
 * @brief Thread lock control.
 *
 * The thread lock is either the default lock or the lock of the resource on
 * which the thread is currently blocked.  The generation number takes care
 * that the up to date lock is used.  Only resources using fine grained locking
 * provide their own lock.
 *
 * The thread lock protects the following thread variables
 *  - Thread_Control::current_priority,
 *  - Thread_Control::Wait::queue, and
 *  - Thread_Control::Wait::operations.
 *
 * @see _Thread_Lock_acquire(), _Thread_Lock_release(), _Thread_Lock_set() and
 * _Thread_Lock_restore_default().
 */
typedef struct {
  /**
   * @brief The current thread lock.
   */
  ISR_lock_Control *current;

  /**
   * @brief The default thread lock in case the thread is not blocked on a
   * resource.
   */
  ISR_lock_Control Default;

  /**
   * @brief Generation number to invalidate stale locks.
   */
  Atomic_Uint generation;
} Thread_Lock_control;
#endif

/**
 *  This structure defines the Thread Control Block (TCB).
 */
struct Thread_Control {
  /** This field is the object management structure for each thread. */
  Objects_Control          Object;
  /** This field is the current execution state of this thread. */
  States_Control           current_state;

  /**
   * @brief This field is the current priority state of this thread.
   *
   * Writes to this field are only allowed in _Thread_Initialize() or via
   * _Thread_Change_priority().
   */
  Priority_Control         current_priority;

  /**
   * @brief This field is the base priority of this thread.
   *
   * Writes to this field are only allowed in _Thread_Initialize() or via
   * _Thread_Change_priority().
   */
  Priority_Control         real_priority;

  /**
   * @brief Generation of the current priority value.
   *
   * It is used in _Thread_Change_priority() to serialize the update of
   * priority related data structures.
   */
  uint32_t                 priority_generation;

  /**
   * @brief Hints if a priority restore is necessary once the resource count
   * changes from one to zero.
   *
   * This is an optimization to speed up the mutex surrender sequence in case
   * no attempt to change the priority was made during the mutex ownership.  On
   * SMP configurations atomic fences must synchronize writes to
   * Thread_Control::priority_restore_hint and Thread_Control::resource_count.
   */
  bool                     priority_restore_hint;

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
     /*================= end of common block =================*/

#if defined(RTEMS_SMP)
  /**
   * @brief Thread lock control.
   */
  Thread_Lock_control Lock;
#endif

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  /** This field is the head of queue of priority inheritance mutex
   *  held by the thread.
   */
  Chain_Control            lock_mutex;
#endif
#if defined(RTEMS_SMP)
  /**
   * @brief Resource node to build a dependency tree in case this thread owns
   * resources or depends on a resource.
   */
  Resource_Node            Resource_node;
#endif
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is true if the thread is offered globally */
  bool                                  is_global;
#endif
  /** This field is true if the thread is preemptible. */
  bool                                  is_preemptible;
  /** This field is true if the thread uses the floating point unit. */
  bool                                  is_fp;

  /**
   * @brief Scheduler related control.
   */
  Thread_Scheduler_control              Scheduler;

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

  /** This field contains information about the starting state of
   *  this thread.
   */
  Thread_Start_information              Start;

  Thread_Action_control                 Post_switch_actions;

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

#if !defined(RTEMS_SMP)
  /** This field points to the set of per task variables. */
  rtems_task_variable_t                *task_variables;
#endif

  /**
   * This is the thread key value chain's control, which is used
   * to track all key value for specific thread, and when thread
   * exits, we can remove all key value for specific thread by
   * iterating this chain, or we have to search a whole rbtree,
   * which is inefficient.
   */
  Chain_Control           Key_Chain;

  /**
   * @brief Thread life-cycle control.
   *
   * Control state changes triggered by thread restart and delete requests.
   */
  Thread_Life_control                   Life;

  Thread_Capture_control                Capture;

  /**
   * @brief Variable length array of user extension pointers.
   *
   * The length is defined by the application via <rtems/confdefs.h>.
   */
  void                                 *extensions[ RTEMS_ZERO_LENGTH_ARRAY ];
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
 * @brief Thread control add-on.
 */
typedef struct {
  /**
   * @brief Offset of the pointer field in Thread_Control referencing an
   * application configuration dependent memory area in the thread control
   * block.
   */
  size_t destination_offset;

  /**
   * @brief Offset relative to the thread control block begin to an application
   * configuration dependent memory area.
   */
  size_t source_offset;
} Thread_Control_add_on;

/**
 * @brief Thread control add-ons.
 *
 * The thread control block contains fields that point to application
 * configuration dependent memory areas, like the scheduler information, the
 * API control blocks, the user extension context table, the RTEMS notepads and
 * the Newlib re-entrancy support.  Account for these areas in the
 * configuration and avoid extra workspace allocations for these areas.
 *
 * This array is provided via <rtems/confdefs.h>.
 *
 * @see _Thread_Control_add_on_count and _Thread_Control_size.
 */
extern const Thread_Control_add_on _Thread_Control_add_ons[];

/**
 * @brief Thread control add-on count.
 *
 * Count of entries in _Thread_Control_add_ons.
 *
 * This value is provided via <rtems/confdefs.h>.
 */
extern const size_t _Thread_Control_add_on_count;

/**
 * @brief Size of the thread control block of a particular application.
 *
 * This value is provided via <rtems/confdefs.h>.
 *
 * @see _Thread_Control_add_ons.
 */
extern const size_t _Thread_Control_size;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
