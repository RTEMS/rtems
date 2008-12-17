/** 
 *  @file  rtems/score/userext.h
 *
 *  This include file contains all information about user extensions.  This
 *  Handler provides mechanisms which can be used to initialize and manipulate
 *  all user extensions.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_USEREXT_H
#define _RTEMS_SCORE_USEREXT_H

/**
 *  @defgroup ScoreUserExt User Extension Handler
 *
 *  This handler encapsulates functionality related to the management of
 *  the user extensions to the SuperCore that are available to the user.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/interr.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

/*@}*/

/** @defgroup ScoreUserExtStruct User Extension Handler Structures
 *
 *  The following records defines the User Extension Table.
 *  This table defines the application dependent routines which
 *  are invoked at critical points in the life of each thread and
 *  the system as a whole.
 */
/*@{*/

/**
 *  This type indicates the return type of a user extension method.
 */
typedef void User_extensions_routine;

/**
 *  This type defines the prototype of a thread creation extension handler.
 *  The handler is passed the thread executing and the thread being created.
 */
typedef bool    ( *User_extensions_thread_create_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  This type defines the prototype of a thread deletion extension handler.
 *  The handler is passed the thread executing and the thread being deleted.
 */
typedef User_extensions_routine ( *User_extensions_thread_delete_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  This type defines the prototype of thread starting extension handler.
 *  The handler is passed the thread executing and the thread being started.
 */
typedef User_extensions_routine ( *User_extensions_thread_start_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  This type defines the prototype of a thread restarting extension handler.
 *  The handler is passed the thread executing and the thread being restarted.
 */
typedef User_extensions_routine ( *User_extensions_thread_restart_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  This type defines the prototype of thread context switch extension handler.
 *  The handler is passed the thread currently executing and the thread being 
 *  switched to.
 */
typedef User_extensions_routine ( *User_extensions_thread_switch_extension )(
                 Thread_Control *,
                 Thread_Control *
             );

/**
 *  This type defines the prototype of a post context switch extension handler.
 *  The handler is passed the thread thread being switched to.
 */
typedef User_extensions_routine (*User_extensions_thread_post_switch_extension)(
                 Thread_Control *
             );

/**
 *  This type defines the prototype of a thread beginning to execute
 *  extension handler.  The handler is passed the thread executing.  This
 *  extension is executed in the context of the beginning thread.
 */
typedef User_extensions_routine ( *User_extensions_thread_begin_extension )(
                 Thread_Control *
             );

/**
 *  This type defines the prototype of a thread exiting extension handler.
 *  The handler is passed the thread exiting.
 */
typedef User_extensions_routine ( *User_extensions_thread_exitted_extension )(
                 Thread_Control *
             );

/**
 *  This type defines the prototype of the fatal error extension handler.
 *  The handler is passed an indicator of the source of the fatal error,
 *  whether it is internal to RTEMS and an error code.
 */
typedef User_extensions_routine ( *User_extensions_fatal_extension )(
                 Internal_errors_Source  /* the_source  */,
                 bool                    /* is_internal */,
                 uint32_t                /* the_error   */
             );

/**
 *  This type defines a set of user extensions.
 */
typedef struct {
  /** This field is the thread creation handler. */
  User_extensions_thread_create_extension       thread_create;
  /** This field is the thread starting handler. */
  User_extensions_thread_start_extension        thread_start;
  /** This field is the thread restarting handler. */
  User_extensions_thread_restart_extension      thread_restart;
  /** This field is the thread deleting handler */
  User_extensions_thread_delete_extension       thread_delete;
  /** This field is thread context switch handler. */
  User_extensions_thread_switch_extension       thread_switch;
  /** This field is the thread beginning handler. */
  User_extensions_thread_begin_extension        thread_begin;
  /** This field is thread exiting handler. */
  User_extensions_thread_exitted_extension      thread_exitted;
  /** This field is the fatal error extension. */
  User_extensions_fatal_extension               fatal;
}   User_extensions_Table;

/**
 *  This is used to manage the list of switch handlers.  They are managed
 *  separately from other extensions for performance reasons.
 */
typedef struct {
  /** This field is a Chain Node structure and allows this to be placed on
   *  chains for set management.
   */
  Chain_Node                              Node;
  /** This field is the thread switch extension. */
  User_extensions_thread_switch_extension thread_switch;
}   User_extensions_Switch_control;

/**
 *  This is used to manage each user extension set.
 *  The switch control is part of the extensions control even
 *  if not used due to the extension not having a switch
 *  handler.
 */
typedef struct {
  /** This field is a Chain Node structure and allows this to be placed on
   *  chains for set management.
   */
  Chain_Node                     Node;
  /** This field is the thread switch user extension. */
  User_extensions_Switch_control Switch;
  /** This field is the rest of this user extension's entry points.  */
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

/*@}*/
/** @addtogroup ScoreUserExt */

/*@{*/

/** @brief User extensions Handler Initialization
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _User_extensions_Handler_initialization(void);

/** @brief User extensions Add to API extension set
 *
 *  This routine is used to add an API extension set to the active list.
 *
 *  @param[in] the_extension is the extension set to add
 */
void _User_extensions_Add_API_set (
  User_extensions_Control *the_extension
);

/** @brief User extensions Add extension set
 *
 *  This routine is used to add a user extension set to the active list.
 *
 *  @param[in] the_extension is the extension set to add
 *  @param[in] extension_table is the user's extension set
 */
void _User_extensions_Add_set (
  User_extensions_Control *the_extension,
  User_extensions_Table   *extension_table
);

/**
 *  This routine is used to remove a user extension set from the active list.
 */
void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
);

/** @brief  User extensions Thread create
 *
 *  This routine is used to invoke the user extension for
 *  the thread creation operate.
 *
 *  @param[in] the_thread is the thread being created.
 *
 *  @return This method returns TRUE if the user extension executed
 *          successfully.
 */
bool _User_extensions_Thread_create (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread delete
 *
 *  This routine is used to invoke the user extension for
 *  the thread deletion operation.
 *
 *  @param[in] the_thread is the thread being deleted.
 */
void _User_extensions_Thread_delete (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread start
 *
 *  This routine is used to invoke the user extension for
 *  the thread start operation.
 *
 *  @param[in] the_thread is the thread being started.
 */
void _User_extensions_Thread_start (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread restart
 *
 *  This routine is used to invoke the user extension for
 *  the thread restart operation.
 *
 *  @param[in] the_thread is the thread being restarted.
 */
void _User_extensions_Thread_restart (
  Thread_Control *the_thread
);

/** @brief  User extensions Thread begin
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread begins.
 *
 *  @param[in] executing is the thread beginning to execute.
 */
void _User_extensions_Thread_begin (
  Thread_Control *executing
);


/** @brief  User extensions Thread switch
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a context switch occurs.
 *
 *  @param[in] executing is the thread currently executing.
 *  @param[in] heir is the thread which will execute.
 */
void _User_extensions_Thread_switch (
  Thread_Control *executing,
  Thread_Control *heir
);

/** @brief  User extensions Thread exitted
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread exits.
 *
 *  @param[in] executing is the thread voluntarily exiting.
 */
void _User_extensions_Thread_exitted (
  Thread_Control *executing
);

/** @brief  User extensions Fatal
 *
 *  This routine is used to invoke the user extension invoked
 *  when a fatal error occurs.
 *
 *  @param[in] the_source is the source of the fatal error.
 *  @param[in] is_internal is TRUE if the error originated inside RTEMS.
 *  @param[in] the_error is an indication of the actual error.
 */
void _User_extensions_Fatal (
  Internal_errors_Source  the_source,
  bool                    is_internal,
  uint32_t                the_error
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
