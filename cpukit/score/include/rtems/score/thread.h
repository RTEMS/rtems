/*  thread.h
 *
 *  This include file contains all constants and structures associated
 *  with the thread control block.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __THREAD_h
#define __THREAD_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/context.h>
#include <rtems/score/cpu.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/tod.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>

/*
 *  The following defines the "return type" of a thread.
 */

typedef void Thread;

/*
 *  The following defines the ways in which the entry point for a
 *  thread can be invoked.  Basically, it can be passed any
 *  combination/permutation of a pointer and an unsigned32 value.
 *
 *  NOTE: For now, we are ignoring the return type.
 */

typedef enum {
  THREAD_START_NUMERIC,
  THREAD_START_POINTER,
  THREAD_START_BOTH_POINTER_FIRST,
  THREAD_START_BOTH_NUMERIC_FIRST
} Thread_Start_types;

typedef Thread ( *Thread_Entry )( );

/*
 *  The following structure contains the information which defines
 *  the starting state of a thread.
 */

typedef struct {
  Thread_Entry         entry_point;      /* starting thread address         */
  Thread_Start_types   prototype;        /* how task is invoked             */
  void                *pointer_argument; /* pointer argument                */
  unsigned32           numeric_argument; /* numeric argument                */
                                         /* initial execution modes         */
  boolean              is_preemptible;
  boolean              is_timeslice;
  unsigned32           isr_level;
  Priority_Control     initial_priority; /* initial priority                */
  Stack_Control        Initial_stack;    /* stack information               */
  void                *fp_context;       /* initial FP context area address */
  void                *stack;            /* initial FP context area address */
}   Thread_Start_information;

/*
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */

#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

typedef struct {
  Objects_Id            id;              /* waiting on this object       */
  unsigned32            count;           /* "generic" fields to be used */
  void                 *return_argument; /*   when blocking */
  void                 *return_argument_1;
  unsigned32            option;

  /*
   *  NOTE: The following assumes that all API return codes can be
   *        treated as an unsigned32.  
   */
  unsigned32            return_code;     /* status for thread awakened   */

  Chain_Control         Block2n;         /* 2 - n priority blocked chain */
  Thread_queue_Control *queue;           /* pointer to thread queue      */
}   Thread_Wait_information;

/*
 *  The following defines the control block used to manage
 *  each thread proxy.
 *
 *  NOTE: It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */

typedef struct {
  Objects_Control          Object;
  States_Control           current_state;
  Priority_Control         current_priority;
  Priority_Control         real_priority;
  unsigned32               resource_count;
  Thread_Wait_information  Wait;
  Watchdog_Control         Timer;
  MP_packet_Prefix        *receive_packet;
     /****************** end of common block ********************/
  Chain_Node               Active;
}   Thread_Proxy_control;


/*
 *  The following record defines the control block used
 *  to manage each thread.
 *
 *  NOTE: It is critical that proxies and threads have identical
 *        memory images for the shared part.
 */

typedef enum {
  THREAD_API_RTEMS,
  THREAD_API_POSIX
}  Thread_APIs;

#define THREAD_API_FIRST THREAD_API_RTEMS
#define THREAD_API_LAST  THREAD_API_POSIX

typedef struct {
  Objects_Control           Object;
  States_Control            current_state;
  Priority_Control          current_priority;
  Priority_Control          real_priority;
  unsigned32                resource_count;
  Thread_Wait_information   Wait;
  Watchdog_Control          Timer;
  MP_packet_Prefix         *receive_packet;
     /****************** end of common block ********************/
  boolean                   is_global;
  boolean                   do_post_task_switch_extension;
  Chain_Control            *ready;
  Priority_Information      Priority_map;
  Thread_Start_information  Start;
  boolean                   is_preemptible;
  boolean                   is_timeslice;
  Context_Control           Registers;
  void                     *fp_context;
  void                     *API_Extensions[ THREAD_API_LAST + 1 ];
  void                    **extensions;
}   Thread_Control;

/*
 *  The following context area contains the context of the "thread"
 *  which invoked the start multitasking routine.  This context is 
 *  restored as the last action of the stop multitasking routine.  Thus
 *  control of the processor can be returned to the environment
 *  which initiated the system.
 */
 
EXTERN Context_Control _Thread_BSP_context;
 
/*
 *  The following declares the dispatch critical section nesting
 *  counter which is used to prevent context switches at inopportune
 *  moments.
 */

EXTERN unsigned32 _Thread_Dispatch_disable_level;

/*
 *  The following holds how many user extensions are in the system.  This
 *  is used to determine how many user extension data areas to allocate
 *  per thread.
 */

EXTERN unsigned32 _Thread_Maximum_extensions;

/*
 *  The following data items are used to manage timeslicing.
 */

EXTERN unsigned32 _Thread_Ticks_remaining_in_timeslice;
EXTERN unsigned32 _Thread_Ticks_per_timeslice;

/*
 *  The following points to the array of FIFOs used to manage the
 *  set of ready threads.
 */

EXTERN Chain_Control *_Thread_Ready_chain;

/*
 *  The following points to the thread which is currently executing.
 *  This thread is implicitly manipulated by numerous directives.
 */

EXTERN Thread_Control *_Thread_Executing;

/*
 *  The following points to the highest priority ready thread
 *  in the system.  Unless the current thread is not preemptibl,
 *  then this thread will be context switched to when the next
 *  dispatch occurs.
 */

EXTERN Thread_Control *_Thread_Heir;

/*
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */

EXTERN Thread_Control *_Thread_Allocated_fp;

/*
 *  _Thread_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _Thread_Handler_initialization (
  unsigned32   ticks_per_timeslice,
  unsigned32   maximum_extensions,
  unsigned32   maximum_proxies
);

/*
 *  _Thread_Start_multitasking
 *
 *  DESCRIPTION:
 *
 *  This routine initiates multitasking.  It is invoked only as
 *  part of initialization and its invocation is the last act of
 *  the non-multitasking part of the system initialization.
 */

void _Thread_Start_multitasking (
  Thread_Control *system_thread,
  Thread_Control *idle_thread
);

/*
 *  _Thread_Stop_multitasking
 *
 *  DESCRIPTION:
 *
 *  This routine halts multitasking and returns control to
 *  the "thread" (i.e. the BSP) which initially invoked the 
 *  routine which initialized the system.
 */

STATIC INLINE void _Thread_Stop_multitasking( void );

/*
 *  _Thread_Dispatch_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the thread dispatching subsystem.
 */

STATIC INLINE void _Thread_Dispatch_initialization( void );

/*
 *  _Thread_Dispatch
 *
 *  DESCRIPTION:
 *
 *  This routine is responsible for transferring control of the
 *  processor from the executing thread to the heir thread.  As part
 *  of this process, it is responsible for the following actions:
 *
 *     + saving the context of the executing thread
 *     + restoring the context of the heir thread
 *     + dispatching any signals for the resulting executing thread
 */

void _Thread_Dispatch( void );

/*
 *  _Thread_Initialize
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

boolean _Thread_Initialize(
  Objects_Information *information,
  Thread_Control      *the_thread,
  void                *stack_area,    /* NULL if to be allocated */
  unsigned32           stack_size,    /* insure it is >= min */
  boolean              is_fp,         /* TRUE if thread uses FP */
  Priority_Control     priority,
  boolean              is_preemptible,
  boolean              is_timeslice,
  unsigned32           isr_level,
  Objects_Name         name
 
);

/*
 *  _Thread_Start
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
boolean _Thread_Start(
  Thread_Control           *the_thread,
  Thread_Start_types        the_prototype,
  void                     *entry_point,
  void                     *pointer_argument,
  unsigned32                numeric_argument
);

/*
 *  _Thread_Restart
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
/* XXX multiple task arg profiles */
 
boolean _Thread_Restart(
  Thread_Control           *the_thread,
  void                     *pointer_argument,
  unsigned32                numeric_argument
);

/*
 *  _Thread_Close
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _Thread_Close(
  Objects_Information  *information,
  Thread_Control       *the_thread
);

/*
 *  _Thread_Ready
 *
 *  DESCRIPTION:
 *
 *  This routine removes any set states for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Ready(
  Thread_Control *the_thread
);

/*
 *  _Thread_Clear_state
 *
 *  DESCRIPTION:
 *
 *  This routine clears the indicated STATES for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

/*
 *  _Thread_Set_state
 *
 *  DESCRIPTION:
 *
 *  This routine sets the indicated states for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 */

void _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/*
 *  _Thread_Set_transient
 *
 *  DESCRIPTION:
 *
 *  This routine sets the TRANSIENT state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

void _Thread_Set_transient(
  Thread_Control *the_thread
);

/*
 *  _Thread_Reset_timeslice
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked upon expiration of the currently
 *  executing thread's timeslice.  If no other thread's are ready
 *  at the priority of the currently executing thread, then the
 *  executing thread's timeslice is reset.  Otherwise, the
 *  currently executing thread is placed at the rear of the
 *  FIFO for this priority and a new heir is selected.
 */

void _Thread_Reset_timeslice( void );

/*
 *  _Thread_Tickle_timeslice
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked as part of processing each clock tick.
 *  It is responsible for determining if the current thread allows
 *  timeslicing and, if so, when its timeslice expires.
 */

void _Thread_Tickle_timeslice( void );

/*
 *  _Thread_Yield_processor
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread of equal
 *  or greater priority.
 */

void _Thread_Yield_processor( void );

/*
 *  _Thread_Is_executing
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_thread is the currently executing
 *  thread, and FALSE otherwise.
 */

STATIC INLINE boolean _Thread_Is_executing (
  Thread_Control *the_thread
);

/*
 *  _Thread_Is_heir
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_thread is the heir
 *  thread, and FALSE otherwise.
 */

STATIC INLINE boolean _Thread_Is_executing (
  Thread_Control *the_thread
);

/*
 *  _Thread_Is_executing_also_the_heir
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the currently executing thread
 *  is also the heir thread, and FALSE otherwise.
 */

STATIC INLINE boolean _Thread_Is_executing_also_the_heir( void );

/*
 *  _Thread_Load_environment
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the context of the_thread to its
 *  appropriate starting state.
 */

void _Thread_Load_environment(
  Thread_Control *the_thread
);

/*
 *  _Thread_Handler
 *
 *  DESCRIPTION:
 *
 *  This routine is the wrapper function for all threads.  It is
 *  the starting point for all threads.  The user provided thread
 *  entry point is invoked by this routine.  Operations
 *  which must be performed immediately before and after the user's
 *  thread executes are found here.
 */

void _Thread_Handler( void );

/*
 *  _Thread_Delay_ended
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a thread must be unblocked at the
 *  end of a time based delay (i.e. wake after or wake when).
 */

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
);

/*
 *  _Thread_Change_priority
 *
 *  DESCRIPTION:
 *
 *  This routine changes the current priority of the_thread to
 *  new_priority.  It performs any necessary scheduling operations
 *  including the selection of a new heir thread.
 */

void _Thread_Change_priority (
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/*
 *  _Thread_Set_priority
 *
 *  DESCRIPTION:
 *
 *  This routine updates the priority related fields in the_thread
 *  control block to indicate the current priority is now new_priority.
 */

void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/*
 *  _Thread_Evaluate_mode
 *
 *  DESCRIPTION:
 *
 *  This routine XXX
 */

boolean _Thread_Evaluate_mode( void );

/*
 *  _Thread_Resume
 *
 *  DESCRIPTION:
 *
 *  This routine clears the SUSPENDED state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

STATIC INLINE void _Thread_Resume (
  Thread_Control *the_thread
);

/*
 *  _Thread_Unblock
 *
 *  DESCRIPTION:
 *
 *  This routine clears any blocking state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

STATIC INLINE void _Thread_Unblock (
  Thread_Control *the_thread
);

/*
 *  _Thread_Restart_self
 *
 *  DESCRIPTION:
 *
 *  This routine resets the current context of the calling thread
 *  to that of its initial state.
 */

STATIC INLINE void _Thread_Restart_self( void );

/*
 *  _Thread_Calculate_heir
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the highest priority
 *  ready thread.
 */

STATIC INLINE void _Thread_Calculate_heir( void );

/*
 *  _Thread_Is_allocated_fp
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the floating point context of
 *  the_thread is currently loaded in the floating point unit, and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _Thread_Is_allocated_fp (
  Thread_Control *the_thread
);

/*
 *  _Thread_Deallocate_fp
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when the currently loaded floating
 *  point context is now longer associated with an active thread.
 */

STATIC INLINE void _Thread_Deallocate_fp( void );

/*
 *  _Thread_Disable_dispatch
 *
 *  DESCRIPTION:
 *
 *  This routine prevents dispatching.
 */

STATIC INLINE void _Thread_Disable_dispatch( void );

/*
 *  _Thread_Enable_dispatch
 *
 *  DESCRIPTION:
 *
 *  This routine allows dispatching to occur again.  If this is
 *  the outer most dispatching critical section, then a dispatching
 *  operation will be performed and, if necessary, control of the
 *  processor will be transferred to the heir thread.
 */

#if ( CPU_INLINE_ENABLE_DISPATCH == TRUE )

STATIC INLINE void _Thread_Enable_dispatch();

#endif

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )

void _Thread_Enable_dispatch( void );

#endif

/*
 *  _Thread_Unnest_dispatch
 *
 *  DESCRIPTION:
 *
 *  This routine allows dispatching to occur again.  However,
 *  no dispatching operation is performed even if this is the outer
 *  most dispatching critical section.
 */

STATIC INLINE void _Thread_Unnest_dispatch( void );

/*
 *  _Thread_Is_dispatching_enabled
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */

STATIC INLINE boolean _Thread_Is_dispatching_enabled( void );

/*
 *  _Thread_Is_context_switch_necessary
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */

STATIC INLINE boolean _Thread_Is_context_switch_necessary( void );

/*
 *  _Thread_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_thread is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Thread_Is_null (
  Thread_Control *the_thread
);

/*
 *  _Thread_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps thread IDs to thread control
 *  blocks.  If ID corresponds to a local thread, then it
 *  returns the_thread control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the thread ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_thread is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_thread is undefined.
 */

STATIC INLINE Thread_Control *_Thread_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _Thread_Is_proxy_blocking
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the status code is equal to the
 *  status which indicates that a proxy is blocking, and FALSE otherwise.
 */
 
STATIC INLINE boolean _Thread_Is_proxy_blocking (
  unsigned32 code
);

#include <rtems/score/thread.inl>
#include <rtems/score/threadmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
