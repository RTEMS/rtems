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

#ifndef __RTEMS_THREAD_h
#define __RTEMS_THREAD_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/asr.h>
#include <rtems/attr.h>
#include <rtems/context.h>
#include <rtems/cpu.h>
#include <rtems/eventset.h>
#include <rtems/modes.h>
#include <rtems/mppkt.h>
#include <rtems/object.h>
#include <rtems/options.h>
#include <rtems/priority.h>
#include <rtems/stack.h>
#include <rtems/states.h>
#include <rtems/tod.h>
#include <rtems/tqdata.h>
#include <rtems/watchdog.h>

/*
 *  Notepads constants (indices into notepad array)
 */

#define RTEMS_NOTEPAD_FIRST 0             /* lowest numbered notepad */
#define RTEMS_NOTEPAD_0    0              /* notepad location 0  */
#define RTEMS_NOTEPAD_1    1              /* notepad location 1  */
#define RTEMS_NOTEPAD_2    2              /* notepad location 2  */
#define RTEMS_NOTEPAD_3    3              /* notepad location 3  */
#define RTEMS_NOTEPAD_4    4              /* notepad location 4  */
#define RTEMS_NOTEPAD_5    5              /* notepad location 5  */
#define RTEMS_NOTEPAD_6    6              /* notepad location 6  */
#define RTEMS_NOTEPAD_7    7              /* notepad location 7  */
#define RTEMS_NOTEPAD_8    8              /* notepad location 8  */
#define RTEMS_NOTEPAD_9    9              /* notepad location 9  */
#define RTEMS_NOTEPAD_10   10             /* notepad location 10 */
#define RTEMS_NOTEPAD_11   11             /* notepad location 11 */
#define RTEMS_NOTEPAD_12   12             /* notepad location 12 */
#define RTEMS_NOTEPAD_13   13             /* notepad location 13 */
#define RTEMS_NOTEPAD_14   14             /* notepad location 14 */
#define RTEMS_NOTEPAD_15   15             /* notepad location 15 */
#define RTEMS_NOTEPAD_LAST RTEMS_NOTEPAD_15     /* highest numbered notepad */

#define RTEMS_NUMBER_NOTEPADS  (RTEMS_NOTEPAD_LAST+1)

/*
 *  The following defines the "return type" of an RTEMS thread.
 *
 *  NOTE:  Keep both types for internal threads.
 */

typedef void rtems_task;
typedef void Thread;

/*
 *  The following defines the argument to an RTEMS thread.
 */

typedef unsigned32 rtems_task_argument;
typedef unsigned32 Thread_Argument;

/*
 *  The following defines the type for the entry point of an RTEMS thread.
 */

typedef rtems_task ( *rtems_task_entry )(
                      rtems_task_argument
                   );

typedef Thread ( *Thread_Entry )(
                      Thread_Argument
                   );

/*
 *  The following structure contains the information which defines
 *  the starting state of a thread.
 */

typedef struct {
  Thread_Entry         entry_point;      /* starting thread address         */
  unsigned32           initial_argument; /* initial argument                */
  rtems_mode           initial_modes;    /* initial mode                    */
  rtems_task_priority  initial_priority; /* initial priority                */
  void                *fp_context;       /* initial FP context area address */
  Stack_Control        Initial_stack;    /* stack information               */
}   Thread_Start_information;

/*
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */

typedef struct {
  Objects_Id            id;              /* waiting on this object       */
  rtems_option          option_set;      /* wait mode                    */
  union {
    unsigned32          segment_size;    /* size of segment requested    */
    rtems_event_set     event_condition;
  } Extra;
  void                 *return_argument; /* address of user return param */
  rtems_status_code     return_code;     /* status for thread awakened   */
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
  Objects_Name             name;
  States_Control           current_state;
  rtems_task_priority         current_priority;
  rtems_task_priority         real_priority;
  unsigned32               resource_count;
  Thread_Wait_information  Wait;
  Watchdog_Control         Timer;
  rtems_packet_prefix        *receive_packet;
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

typedef struct {
  Objects_Control           Object;
  Objects_Name              name;
  States_Control            current_state;
  rtems_task_priority       current_priority;
  rtems_task_priority       real_priority;
  unsigned32                resource_count;
  Thread_Wait_information   Wait;
  Watchdog_Control          Timer;
  rtems_packet_prefix      *receive_packet;
     /****************** end of common block ********************/
  Chain_Control            *ready;
  Priority_Information      Priority_map;
  rtems_event_set           pending_events;
  rtems_event_set           events_out;
  Thread_Start_information  Start;
  ASR_Information           Signal;
  rtems_mode                current_modes;
  rtems_attribute           attribute_set;
  Context_Control           Registers;
  void                     *fp_context;
  unsigned32                Notepads[ RTEMS_NUMBER_NOTEPADS ];
  void                     *extension;
}   Thread_Control;

/*
 *  External API name for Thread_Control
 */

typedef Thread_Control rtems_tcb;

/*
 *  The following declares the dispatch critical section nesting
 *  counter which is used to prevent context switches at inopportune
 *  moments.
 */

EXTERN unsigned32 _Thread_Dispatch_disable_level;

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
 *  in the system.  Unless the current thread is RTEMS_NO_PREEMPT,
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
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

EXTERN Objects_Information _Thread_Information;

/*
 *  The following context area contains the context of the "thread"
 *  which invoked rtems_initialize_executive.  This context is restored
 *  as the last action of the rtems_shutdown_executive directive.  Thus
 *  control of the processor can be returned to the environment
 *  which initiated RTEMS.
 */

EXTERN Context_Control _Thread_BSP_context;

/*
 *  _Thread_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _Thread_Handler_initialization (
  unsigned32 maximum_tasks,
  unsigned32 ticks_per_timeslice,
  unsigned32 maximum_proxies
);

/*
 *  _Thread_Start_multitasking
 *
 *  DESCRIPTION:
 *
 *  This routine initiates multitasking.  It is invoked only as
 *  part of initialization and its invocation is the last act of
 *  the rtems_initialize_executive directive.
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
 *  the "thread" which initially invoked the rtems_initialize_executive
 *  directive.
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
 *  RTEMS_FIFO for this priority and a new heir is selected.
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
 *  end of a delay such as the rtems_task_wake_after and rtems_task_wake_when
 *  directives.
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
  rtems_task_priority  new_priority
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
  rtems_task_priority  new_priority
);

/*
 *  _Thread_Change_mode
 *
 *  DESCRIPTION:
 *
 *  This routine changes the current values of the modes
 *  indicated by mask of the calling thread are changed to that
 *  indicated in mode_set.  The former mode of the thread is
 *  returned in mode_set.  If the changes in the current mode
 *  indicate that a thread dispatch operation may be necessary,
 *  then need_dispatch is TRUE, otherwise it is FALSE.
 */

boolean _Thread_Change_mode(
  rtems_mode  new_mode_set,
  rtems_mode  mask,
  rtems_mode *old_mode_set
);

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

#include <rtems/thread.inl>
#include <rtems/threadmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
