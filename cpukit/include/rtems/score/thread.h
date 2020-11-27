/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreThread which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014, 2016 embedded brains GmbH.
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
#include <rtems/score/freechain.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/priority.h>
#include <rtems/score/schedulernode.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/threadq.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/watchdog.h>

#if defined(RTEMS_SMP)
#include <rtems/score/processormask.h>
#endif

struct rtems_user_env_t;

struct _pthread_cleanup_context;

struct Per_CPU_Control;

struct _Scheduler_Control;

struct User_extensions_Iterator;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreThread Thread Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Thread Handler implementation.
 *
 * This handler encapsulates functionality related to the management of
 * threads.  This includes the creation, deletion, and scheduling of threads.
 *
 * The following variables are maintained as part of the per cpu data
 * structure.
 *
 * + Idle thread pointer
 * + Executing thread pointer
 * + Heir thread pointer
 *
 *@{
 */

#define RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE

/*
 * With the addition of the Constant Block Scheduler (CBS),
 * this feature is needed even when POSIX is disabled.
 */
#define RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT

#if defined(RTEMS_DEBUG)
#define RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT
#endif

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
 * @brief Data for idle thread entry.
 */
typedef struct {
  void *( *entry )( uintptr_t argument );
} Thread_Entry_idle;

/**
 * @brief Data for thread entry with one numeric argument and no return value.
 */
typedef struct {
  void ( *entry )( Thread_Entry_numeric_type argument );
  Thread_Entry_numeric_type argument;
} Thread_Entry_numeric;

/**
 * @brief Data for thread entry with one pointer argument and a pointer return
 * value.
 */
typedef struct {
  void *( *entry )( void *argument  );
  void *argument;
} Thread_Entry_pointer;

/**
 * @brief Thread entry information.
 */
typedef struct {
  /**
   * @brief Thread entry adaptor.
   *
   * Calls the corresponding thread entry with the right parameters.
   *
   * @param executing The executing thread.
   */
  void ( *adaptor )( Thread_Control *executing );

  /**
   * @brief Thread entry data used by the adaptor to call the thread entry
   * function with the right parameters.
   */
  union {
    Thread_Entry_idle Idle;
    Thread_Entry_numeric Numeric;
    Thread_Entry_pointer Pointer;
  } Kinds;
} Thread_Entry_information;

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
 *  The following structure contains the information which defines
 *  the starting state of a thread.
 */
typedef struct {
  /** This field contains the thread entry information. */
  Thread_Entry_information             Entry;
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
  /**
   * @brief This field points to the handler which should free the stack.
   */
  void                              ( *stack_free )( void * );
  /** This field is the stack information. */
  Stack_Control                        Initial_stack;
  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    /** This field is the initial FP context area address. */
    Context_Control_fp                  *fp_context;
  #endif
  /** The thread-local storage (TLS) area */
  void                                *tls_area;
} Thread_Start_information;

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
   * @brief Lock to protect the scheduler node change requests.
   */
  ISR_lock_Control Lock;

  /**
   * @brief The current scheduler state of this thread.
   */
  Thread_Scheduler_state state;

  /**
   * @brief The home scheduler of this thread.
   */
  const struct _Scheduler_Control *home_scheduler;

  /**
   * @brief The pinned scheduler of this thread.
   */
  const struct _Scheduler_Control *pinned_scheduler;

  /**
   * @brief The processor assigned by the current scheduler.
   */
  struct Per_CPU_Control *cpu;

  /**
   * @brief Scheduler nodes immediately available to the thread by its home
   * scheduler and due to thread queue ownerships.
   *
   * This chain is protected by the thread wait lock.
   *
   * This chain is never empty.  The first scheduler node on the chain is the
   * scheduler node of the home scheduler.
   */
  Chain_Control Wait_nodes;

  /**
   * @brief Scheduler nodes immediately available to the schedulers for this
   * thread.
   *
   * This chain is protected by the thread state lock.
   *
   * This chain is never empty for normal threads (the only exception are idle
   * threads associated with an online processor which is not used by a
   * scheduler).  In case a pinned scheduler is set for this thread, then the
   * first scheduler node of this chain belongs to the pinned scheduler,
   * otherwise the first scheduler node of this chain belongs to the home
   * scheduler.
   */
  Chain_Control Scheduler_nodes;

  /**
   * @brief Node for the Per_CPU_Control::Threads_in_need_for_help chain.
   *
   * This chain is protected by the Per_CPU_Control::Lock lock of the assigned
   * processor.
   */
  Chain_Node Help_node;

  /**
   * @brief Count of nodes scheduler nodes minus one.
   *
   * This chain is protected by the thread state lock.
   */
  size_t helping_nodes;

  /**
   * @brief List of pending scheduler node requests.
   *
   * This list is protected by the thread scheduler lock.
   */
  Scheduler_Node *requests;

  /**
   * @brief The thread pinning to current processor level.
   *
   * Must be touched only by the executing thread with thread dispatching
   * disabled.  If non-zero, then the thread is pinned to its current
   * processor.  The pin level is incremented and decremented by two.  The
   * least-significant bit indicates that the thread was pre-empted and must
   * undo the pinning with respect to the scheduler once the level changes from
   * three to one.
   *
   * The thread pinning may be used to access per-processor data structures in
   * critical sections with enabled thread dispatching, e.g. a pinned thread is
   * allowed to block.
   *
   * Thread pinning should be used only for short critical sections and not all
   * the time.  Thread pinning is a very low overhead operation in case the
   * thread is not preempted during the pinning.
   *
   * @see _Thread_Pin() and _Thread_Unpin().
   */
  int pin_level;

  /**
   * @brief The thread processor affinity set.
   */
  Processor_mask Affinity;
#endif

  /**
   * @brief The scheduler nodes of this thread.
   *
   * Each thread has a scheduler node for each scheduler instance.
   */
  Scheduler_Node *nodes;
} Thread_Scheduler_control;

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
#if defined(RTEMS_MULTIPROCESSING)
  /*
   * @brief This field is the identifier of the remote object this thread is
   * waiting upon.
   */
  Objects_Id            remote_id;
#endif
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
   * @brief This field contains several flags used to control the wait class
   * and state of a thread in case fine-grained locking is used.
   */
#if defined(RTEMS_SMP)
  Atomic_Uint           flags;
#else
  Thread_Wait_flags     flags;
#endif

#if defined(RTEMS_SMP)
  /**
   * @brief Thread wait lock control block.
   *
   * Parts of the thread wait information are protected by the thread wait
   * default lock and additionally a thread queue lock in case the thread
   * is enqueued on a thread queue.
   *
   * The thread wait lock mechanism protects the following thread variables
   *  - POSIX_API_Control::Attributes,
   *  - Scheduler_Node::Wait,
   *  - Thread_Control::Wait::Lock::Pending_requests,
   *  - Thread_Control::Wait::queue, and
   *  - Thread_Control::Wait::operations.
   *
   * @see _Thread_Wait_acquire(), _Thread_Wait_release(), _Thread_Wait_claim(),
   *   _Thread_Wait_restore_default() and _Thread_Wait_tranquilize().
   */
  struct {
    /**
     * @brief Thread wait default lock.
     */
    ISR_lock_Control Default;

    /**
     * @brief The pending thread wait lock acquire or tranquilize requests in
     * case the thread is enqueued on a thread queue.
     */
    Chain_Control Pending_requests;

    /**
     * @brief Tranquilizer gate used by _Thread_Wait_tranquilize().
     *
     * This gate is closed by _Thread_Wait_claim().  In case there are no
     * pending requests during a _Thread_Wait_restore_default(), then this gate
     * is opened immediately, otherwise it is placed on the pending request
     * chain and opened by _Thread_Wait_remove_request_locked() as the last
     * gate on the chain to signal overall request completion.
     */
    Thread_queue_Gate Tranquilizer;
  } Lock;

  /**
   * @brief Thread queue link provided for use by the thread wait lock owner to
   * build a thread queue path.
   */
  Thread_queue_Link Link;
#endif

  /**
   * @brief The current thread queue.
   *
   * If this field is NULL the thread is not enqueued on a thread queue.  This
   * field is protected by the thread wait default lock.
   *
   * @see _Thread_Wait_claim().
   */
  Thread_queue_Queue *queue;

  /**
   * @brief The current thread queue operations.
   *
   * This field is protected by the thread lock wait default lock.
   *
   * @see _Thread_Wait_claim().
   */
  const Thread_queue_Operations *operations;

  Thread_queue_Heads *spare_heads;
}   Thread_Wait_information;

/**
 * @brief Information required to manage a thread timer.
 */
typedef struct {
  ISR_LOCK_MEMBER( Lock )
  Watchdog_Header *header;
  Watchdog_Control Watchdog;
} Thread_Timer_information;

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

  /**
   * @see Thread_Control::Join_queue
   */
  Thread_queue_Control     Join_queue;

  /** This field is the current execution state of this proxy. */
  States_Control           current_state;

  /**
   * @brief The base priority of this thread in its home scheduler instance.
   */
  Priority_Node            Real_priority;

#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  /** This field is the number of mutexes currently held by this proxy. */
  uint32_t                 resource_count;
#endif

  /**
   * @brief Scheduler related control.
   */
  Thread_Scheduler_control Scheduler;

  /** This field is the blocking information for this proxy. */
  Thread_Wait_information  Wait;
  /** This field is the Watchdog used to manage proxy delays and timeouts. */
  Thread_Timer_information Timer;
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is the received response packet in an MP system. */
  MP_packet_Prefix        *receive_packet;
     /****************** end of common block ********************/

  /**
   * @brief Thread queue callout for _Thread_queue_Enqueue().
   */
  Thread_queue_MP_callout  thread_queue_callout;

  /**
   * @brief This field is used to manage the set of active proxies in the system.
   */
  RBTree_Node              Active;

  /**
   * @brief The scheduler node providing the thread wait nodes used to enqueue
   * this thread proxy on a thread queue.
   */
  Scheduler_Node           Scheduler_node;

  /**
   * @brief Provide thread queue heads for this thread proxy.
   *
   * The actual size of the thread queue heads depends on the application
   * configuration.  Since thread proxies are never destroyed we can use the
   * same storage place for the thread queue heads.
   */
  Thread_queue_Heads       Thread_queue_heads[ RTEMS_ZERO_LENGTH_ARRAY ];
#endif
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
 * The thread action handler will be called with interrupts disabled and a
 * corresponding lock acquired, e.g. _Thread_State_acquire().  The handler must
 * release the corresponding lock, e.g. _Thread_State_release().  So, the
 * corresponding lock may be used to protect private data used by the
 * particular action.
 *
 * Since the action is passed to the handler additional data may be accessed
 * via RTEMS_CONTAINER_OF().
 *
 * @param[in] the_thread The thread performing the action.
 * @param[in] action The thread action.
 * @param[in] lock_context The lock context to use for the lock release.
 */
typedef void ( *Thread_Action_handler )(
  Thread_Control   *the_thread,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
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
 * - POSIX signals delivery, and
 * - thread life-cycle changes.
 *
 * @see _Thread_Add_post_switch_action() and _Thread_Run_post_switch_actions().
 */
struct Thread_Action {
  Chain_Node            Node;
  Thread_Action_handler handler;
};

/**
 * @brief Per-thread information for POSIX Keys.
 */
typedef struct {
  /**
   * @brief Key value pairs registered for this thread.
   */
  RBTree_Control Key_value_pairs;

  /**
   * @brief Lock to protect the tree operations.
   */
  ISR_LOCK_MEMBER( Lock )
} Thread_Keys_information;

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
 *
 * The individual state values must be a power of two to allow use of bit
 * operations to manipulate and evaluate the thread life state.
 */
typedef enum {
  THREAD_LIFE_PROTECTED = 0x1,
  THREAD_LIFE_RESTARTING = 0x2,
  THREAD_LIFE_TERMINATING = 0x4,
  THREAD_LIFE_CHANGE_DEFERRED = 0x8,
  THREAD_LIFE_DETACHED = 0x10
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
   * @brief The count of pending life change requests.
   */
  uint32_t pending_life_change_requests;

  /**
   * @brief The thread exit value.
   *
   * It is,
   * - the value passed to pthread_exit(), or
   * - PTHREAD_CANCELED in case it is cancelled via pthread_cancel(), or
   * - NULL.
   */
  void *exit_value;
} Thread_Life_control;

typedef struct  {
  uint32_t      flags;
  void *        control;
}Thread_Capture_control;

/**
 *  This structure defines the Thread Control Block (TCB).
 *
 *  Uses a leading underscore in the structure name to allow forward
 *  declarations in standard header files provided by Newlib and GCC.
 *
 *  In case the second member changes (currently Join_queue), then the memset()
 *  in _Thread_Initialize() must be adjusted.
 */
struct _Thread_Control {
  /** This field is the object management structure for each thread. */
  Objects_Control          Object;

  /**
   * @brief Thread queue for thread join operations and multi-purpose lock.
   *
   * The lock of this thread queue is used for various purposes.  It protects
   * the following fields
   *
   * - RTEMS_API_Control::Signal,
   * - Thread_Control::budget_algorithm,
   * - Thread_Control::budget_callout,
   * - Thread_Control::cpu_time_budget,
   * - Thread_Control::current_state,
   * - Thread_Control::Post_switch_actions,
   * - Thread_Control::Scheduler::control, and
   * - Thread_Control::Scheduler::own_control.
   *
   * @see _Thread_State_acquire().
   */
  Thread_queue_Control     Join_queue;

  /** This field is the current execution state of this thread. */
  States_Control           current_state;

  /**
   * @brief The base priority of this thread in its home scheduler instance.
   */
  Priority_Node            Real_priority;

#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  /** This field is the number of mutexes currently held by this thread. */
  uint32_t                 resource_count;
#endif

  /**
   * @brief Scheduler related control.
   */
  Thread_Scheduler_control Scheduler;

  /** This field is the blocking information for this thread. */
  Thread_Wait_information  Wait;
  /** This field is the Watchdog used to manage thread delays and timeouts. */
  Thread_Timer_information Timer;
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is the received response packet in an MP system. */
  MP_packet_Prefix        *receive_packet;
#endif
     /*================= end of common block =================*/

#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  /**
   * @brief Potpourri lock statistics.
   *
   * These SMP lock statistics are used for all lock objects that lack a
   * storage space for the statistics.  Examples are lock objects used in
   * external libraries which are independent of the actual RTEMS build
   * configuration.
   */
  SMP_lock_Stats Potpourri_stats;
#endif

  /** This field is true if the thread is an idle thread. */
  bool                                  is_idle;
#if defined(RTEMS_MULTIPROCESSING)
  /** This field is true if the thread is offered globally */
  bool                                  is_global;
#endif
  /** This field is true if the thread is preemptible. */
  bool                                  is_preemptible;
  /** This field is true if the thread uses the floating point unit. */
  bool                                  is_fp;

  /**
   * @brief True, if the thread was created with an inherited scheduler
   * (PTHREAD_INHERIT_SCHED), and false otherwise.
   */
  bool was_created_with_inherited_scheduler;

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
  Timestamp_Control                     cpu_time_used;

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

  /**
   * @brief The POSIX Keys information.
   */
  Thread_Keys_information               Keys;

  /**
   * @brief Thread life-cycle control.
   *
   * Control state changes triggered by thread restart and delete requests.
   */
  Thread_Life_control                   Life;

  Thread_Capture_control                Capture;

  /**
   * @brief Pointer to an optional thread-specific POSIX user environment.
   */
  struct rtems_user_env_t *user_environment;

  /**
   * @brief LIFO list of POSIX cleanup contexts.
   */
  struct _pthread_cleanup_context *last_cleanup_context;

  /**
   * @brief LIFO list of user extensions iterators.
   */
  struct User_extensions_Iterator *last_user_extensions_iterator;

  /**
   * @brief Variable length array of user extension pointers.
   *
   * The length is defined by the application via <rtems/confdefs.h>.
   */
  void                                 *extensions[ RTEMS_ZERO_LENGTH_ARRAY ];
};

typedef void (*rtems_per_thread_routine)( Thread_Control * );

/**
 * @brief Deprecated, use rtems_task_iterate() instead.
 *
 * Use rtems_task_iterate() instead.
 */
void rtems_iterate_over_all_threads(
  rtems_per_thread_routine routine
) RTEMS_DEPRECATED;

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
 * API control blocks, the user extension context table, and the Newlib
 * re-entrancy support.  Account for these areas in the configuration and
 * avoid extra workspace allocations for these areas.
 *
 * This array is provided via <rtems/confdefs.h>.
 *
 * @see _Thread_Control_add_on_count.
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
 * @brief Count of configured threads.
 *
 * This value is provided via <rtems/confdefs.h>.
 */
extern const size_t _Thread_Initial_thread_count;

/**
 * @brief The default maximum size of a thread name in characters (including
 * the terminating '\0' character).
 *
 * This is the default value for the application configuration option
 * CONFIGURE_MAXIMUM_THREAD_NAME_SIZE.
 */
#define THREAD_DEFAULT_MAXIMUM_NAME_SIZE 16

/**
 * @brief Maximum size of a thread name in characters (including the
 * terminating '\0' character).
 *
 * This value is provided via <rtems/confdefs.h>.
 */
extern const size_t _Thread_Maximum_name_size;

/**
 * @brief If this constant is greater than zero, then it defines the maximum
 * thread-local storage size, otherwise the thread-local storage size is defined
 * by the linker depending on the thread-local storage objects used by the
 * application in the statically-linked executable.
 *
 * This value is provided via <rtems/confdefs.h>.
 */
extern const size_t _Thread_Maximum_TLS_size;

/**
 * @brief The configured thread control block.
 *
 * This type is defined in <rtems/confdefs.h> and depends on the application
 * configuration.
 */
typedef struct Thread_Configured_control Thread_Configured_control;

/**
 * @brief The configured thread queue heads.
 *
 * In SMP configurations, this type is defined in <rtems/confdefs.h> and depends
 * on the application configuration.
 */
#if defined(RTEMS_SMP)
typedef struct Thread_queue_Configured_heads Thread_queue_Configured_heads;
#else
typedef Thread_queue_Heads Thread_queue_Configured_heads;
#endif

/**
 * @brief Size of the thread queue heads of a particular application.
 *
 * In SMP configurations, this value is provided via <rtems/confdefs.h>.
 */
#if defined(RTEMS_SMP)
extern const size_t _Thread_queue_Heads_size;
#else
#define _Thread_queue_Heads_size sizeof(Thread_queue_Heads)
#endif

/**
 * @brief The thread object information.
 */
typedef struct {
  /**
   * @brief The object information.
   */
  Objects_Information Objects;

  /**
   * @brief Thread queue heads maintenance.
   */
  union {
    /**
     * @brief Contains the initial set of thread queue heads.
     *
     * This is set by <rtems/confdefs.h> via THREAD_INFORMATION_DEFINE().
     */
    Thread_queue_Configured_heads *initial;

    /**
     * @brief The free thread queue heads.
     *
     * This member is initialized by _Thread_Initialize_information().
     */
    Freechain_Control Free;
  } Thread_queue_heads;
} Thread_Information;

/**
 * @brief The internal thread  objects information.
 */
extern Thread_Information _Thread_Information;

#define THREAD_INFORMATION_DEFINE_ZERO( name, api, cls ) \
Thread_Information name##_Information = { \
  { \
    _Objects_Build_id( api, cls, 1, 0 ), \
    NULL, \
    _Objects_Allocate_none, \
    NULL, \
    0, \
    0, \
    0, \
    0, \
    CHAIN_INITIALIZER_EMPTY( name##_Information.Objects.Inactive ), \
    NULL, \
    NULL, \
    NULL \
    OBJECTS_INFORMATION_MP( name##_Information.Objects, NULL ), \
  }, { \
    NULL \
  } \
}

/**
 * @brief Return an inactive thread object or NULL.
 *
 * @retval NULL No inactive object is available.
 * @retval object An inactive object.
 */
Objects_Control *_Thread_Allocate_unlimited( Objects_Information *information );

#define THREAD_INFORMATION_DEFINE( name, api, cls, max ) \
static Objects_Control * \
name##_Local_table[ _Objects_Maximum_per_allocation( max ) ]; \
static Thread_Configured_control \
name##_Objects[ _Objects_Maximum_per_allocation( max ) ]; \
static Thread_queue_Configured_heads \
name##_Heads[ _Objects_Maximum_per_allocation( max ) ]; \
Thread_Information name##_Information = { \
  { \
    _Objects_Build_id( api, cls, 1, _Objects_Maximum_per_allocation( max ) ), \
    name##_Local_table, \
    _Objects_Is_unlimited( max ) ? \
      _Thread_Allocate_unlimited : _Objects_Allocate_static, \
    _Objects_Is_unlimited( max ) ? \
      _Objects_Free_unlimited : _Objects_Free_static, \
    0, \
    _Objects_Is_unlimited( max ) ? _Objects_Maximum_per_allocation( max ) : 0, \
    sizeof( Thread_Configured_control ), \
    OBJECTS_NO_STRING_NAME, \
    CHAIN_INITIALIZER_EMPTY( name##_Information.Objects.Inactive ), \
    NULL, \
    NULL, \
    &name##_Objects[ 0 ].Control.Object \
    OBJECTS_INFORMATION_MP( name##_Information.Objects, NULL ) \
  }, { \
    &name##_Heads[ 0 ] \
  } \
}

/**
 * @brief The idle thread stacks.
 *
 * Provided by the application via <rtems/confdefs.h>.
 */
extern char _Thread_Idle_stacks[];

#if defined(RTEMS_MULTIPROCESSING)
/**
 * @brief The configured thread control block.
 *
 * This type is defined in <rtems/confdefs.h> and depends on the application
 * configuration.
 */
typedef struct Thread_Configured_proxy_control Thread_Configured_proxy_control;

/**
 * @brief The configured proxies.
 *
 * Provided by the application via <rtems/confdefs.h>.
 */
extern Thread_Configured_proxy_control * const _Thread_MP_Proxies;
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
