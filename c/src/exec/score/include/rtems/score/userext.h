/*  userext.h
 *
 *  This include file contains all information about user extensions.  This
 *  Handler provides mechanisms which can be used to initialize and manipulate
 *  all RTEMS user extensions.
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

#ifndef __RTEMS_USER_EXTENSIONS_h
#define __RTEMS_USER_EXTENSIONS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/config.h>
#include <rtems/thread.h>

/*
 *  The following is used to manage each user extension set.
 */

typedef struct {
  Chain_Node                     Node;
  rtems_extensions_table  Callouts;
}   User_extensions_Control;

/*
 *  The following contains the static extension set which may be
 *  configured by the application.
 */

EXTERN User_extensions_Control _User_extensions_Initial;

/*
 *  The following is used to manage the list of active extensions.
 */

EXTERN Chain_Control _User_extensions_List;


/*
 *  _User_extensions_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

STATIC INLINE void _User_extensions_Handler_initialization (
    rtems_extensions_table *initial_extensions
);

/*
 *  _User_extensions_Add_set
 *
 *  DESCRIPTION:
 *
 *  This routine is used to add a user extension set to the active list.
 */

STATIC INLINE void _User_extensions_Add_set (
  User_extensions_Control  *the_extension,
  rtems_extensions_table   *extension_table
);

/*
 *  _User_extensions_Remove_set
 *
 *  DESCRIPTION:
 *
 *  This routine is used to remove a user extension set from the active list.
 */

STATIC INLINE void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
);

/*
 *  _User_extensions_Task_create
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the rtems_task_create directive.
 */

STATIC INLINE void _User_extensions_Task_create (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Task_delete
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the rtems_task_delete directive.
 */

STATIC INLINE void _User_extensions_Task_delete (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Task_start
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the rtems_task_start directive.
 */

STATIC INLINE void _User_extensions_Task_start (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Task_restart
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the rtems_task_restart directive.
 */

STATIC INLINE void _User_extensions_Task_restart (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Task_switch
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a context switch occurs.
 */

STATIC INLINE void _User_extensions_Task_switch (
  Thread_Control *executing,
  Thread_Control *heir
);

/*
 *  _User_extensions_Task_begin
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a task begins.
 */

STATIC INLINE void _User_extensions_Task_begin (
  Thread_Control *executing
);

/*
 *  _User_extensions_Task_exitted
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a task exits.
 */

STATIC INLINE void _User_extensions_Task_exitted (
  Thread_Control *executing
);

/*
 *  _User_extensions_Task_exitted
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a task exits.
 */

STATIC INLINE void _User_extensions_Task_exitted (
  Thread_Control *executing
);

/*
 *  _User_extensions_Fatal
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the rtems_fatal_error_occurred directive.
 */

STATIC INLINE void _User_extensions_Fatal (
  unsigned32 the_error
);

#include <rtems/userext.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
