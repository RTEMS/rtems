/** 
 *  @file  rtems/score/userext.h
 *
 *  This include file contains all information about user extensions.  This
 *  Handler provides mechanisms which can be used to initialize and manipulate
 *  all user extensions.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __USER_EXTENSIONS_h
#define __USER_EXTENSIONS_h

/**
 *  @defgroup ScoreUserExt User Extension Handler
 *
 *  This group contains functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/interr.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

/** @defgroup ScoreUserExtStruct User Extension Handler Structures
 *
 *  The following records defines the User Extension Table.
 *  This table defines the application dependent routines which
 *  are invoked at critical points in the life of each thread and
 *  the system as a whole.
 */
/*@{*/

/**
 *  XXX
 */
typedef void User_extensions_routine;

/**
 *  XXX
 */
typedef boolean ( *User_extensions_thread_create_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_delete_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_start_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_restart_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_switch_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine (
                                *User_extensions_thread_post_switch_extension )(
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_begin_extension )(
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_thread_exitted_extension )(
                 Thread_Control *
             );

/**
 *  XXX
 */
typedef User_extensions_routine ( *User_extensions_fatal_extension )(
                 Internal_errors_Source  /* the_source  */,
                 boolean                 /* is_internal */,
                 uint32_t                /* the_error   */
             );

/**
 *  XXX
 */
typedef struct {
  /** This field is XXX */
  User_extensions_thread_create_extension       thread_create;
  /** This field is XXX */
  User_extensions_thread_start_extension        thread_start;
  /** This field is XXX */
  User_extensions_thread_restart_extension      thread_restart;
  /** This field is XXX */
  User_extensions_thread_delete_extension       thread_delete;
  /** This field is XXX */
  User_extensions_thread_switch_extension       thread_switch;
  /** This field is XXX */
  User_extensions_thread_begin_extension        thread_begin;
  /** This field is XXX */
  User_extensions_thread_exitted_extension      thread_exitted;
  /** This field is XXX */
  User_extensions_fatal_extension               fatal;
}   User_extensions_Table;

/*@}*/

/**
 *  This is used to manage the list of switch handlers.
 */
typedef struct {
  /** This field is XXX */
  Chain_Node                              Node;
  /** This field is XXX */
  User_extensions_thread_switch_extension thread_switch;
}   User_extensions_Switch_control;

/**
 *  This is used to manage each user extension set.
 *  The switch control is part of the extensions control even
 *  if not used due to the extension not having a switch
 *  handler.
 */
typedef struct {
  /** This field is XXX */
  Chain_Node                     Node;
  /** This field is XXX */
  User_extensions_Switch_control Switch;
  /** This field is XXX */
  User_extensions_Table          Callouts;
}   User_extensions_Control;

/**
 *  This is used to manage the list of active extensions.
 */
SCORE_EXTERN Chain_Control _User_extensions_List;

/**
 *  This is used to manage a chain of user extension task
 *  switch nodes.
 */
SCORE_EXTERN Chain_Control _User_extensions_Switches_list;

/** @brief  User extensions Thread create
 *
 *  This routine is used to invoke the user extension for
 *  the thread creation operate.
 */
boolean _User_extensions_Thread_create (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread delete
 *
 *  This routine is used to invoke the user extension for
 *  the thread deletion operation.
 */
void _User_extensions_Thread_delete (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread start
 *
 *  This routine is used to invoke the user extension for
 *  the thread start operation.
 */
void _User_extensions_Thread_start (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread restart
 *
 *  This routine is used to invoke the user extension for
 *  the thread restart operation.
 */
void _User_extensions_Thread_restart (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread begin
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread begins.
 */
void _User_extensions_Thread_begin (
  Thread_Control *executing
);

/** @brief  User extensions Thread exitted
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread exits.
 */
void _User_extensions_Thread_exitted (
  Thread_Control *executing
);

/** @brief  User extensions Fatal
 *
 *  This routine is used to invoke the user extension invoked
 *  when a fatal error occurs.
 */
void _User_extensions_Fatal (
  Internal_errors_Source  the_source,
  boolean                 is_internal,
  uint32_t                the_error
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/userext.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
