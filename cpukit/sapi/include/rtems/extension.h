/*  extension.h
 *
 *  This include file contains all the constants, structures, and
 *  prototypes associated with the User Extension Manager.  This manager
 *  provides a mechanism for manipulating sets of user-defined extensions.
 *
 *  Directives provided are:
 *
 *     + create user extension set
 *     + get ID of user extension set
 *     + delete user extension set
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

#ifndef __RTEMS_EXTENSION_MANAGER_h
#define __RTEMS_EXTENSION_MANAGER_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/userext.h>
#include <rtems/rtems/status.h>  /* XXX */
#include <rtems/rtems/types.h>  /* XXX */

/*
 *  Extension related types
 */

typedef User_extensions_routine                   rtems_extension;
typedef User_extensions_thread_create_extension   rtems_task_create_extension;
typedef User_extensions_thread_delete_extension   rtems_task_delete_extension;
typedef User_extensions_thread_start_extension    rtems_task_start_extension;
typedef User_extensions_thread_restart_extension  rtems_task_restart_extension;
typedef User_extensions_thread_switch_extension   rtems_task_switch_extension;
typedef User_extensions_thread_post_switch_extension  
  rtems_task_post_switch_extension;
typedef User_extensions_thread_begin_extension    rtems_task_begin_extension;
typedef User_extensions_thread_exitted_extension  rtems_task_exitted_extension;
typedef User_extensions_fatal_extension           rtems_fatal_extension;

typedef User_extensions_Table                     rtems_extensions_table;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

EXTERN Objects_Information  _Extension_Information;

/*
 *  The following records define the control block used to manage
 *  each extension.
 */

typedef struct {
  Objects_Control          Object;
  User_extensions_Control  Extension;
}   Extension_Control;

/*
 *  _Extension_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Extension_Manager_initialization(
  unsigned32 maximum_extensions
);

/*
 *  rtems_extension_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_extension_create directive.  The
 *  extension will have the name name.   The entry points of the
 *  routines which constitute this extension set are in EXTENSION_TABLE.
 *  It returns the id of the created extension in ID.
 */

rtems_status_code rtems_extension_create(
  rtems_name              name,
  rtems_extensions_table *extension_table,
  Objects_Id              *id
);

/*
 *  rtems_extension_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_extension_ident directive.
 *  This directive returns the extension ID associated with name.
 *  If more than one extension is named name, then the extension
 *  to which the ID belongs is arbitrary.
 */

rtems_status_code rtems_extension_ident(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_extension_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_extension_delete directive.  The
 *  extension indicated by ID is deleted.
 */

rtems_status_code rtems_extension_delete(
  Objects_Id id
);

/*
 *  _Extension_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a extension control block from
 *  the inactive chain of free extension control blocks.
 */

STATIC INLINE Extension_Control *_Extension_Allocate( void );

/*
 *  _Extension_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a extension control block to the
 *  inactive chain of free extension control blocks.
 */

STATIC INLINE void _Extension_Free (
  Extension_Control *the_extension
);

/*
 *  _Extension_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps extension IDs to extension control blocks.
 *  If ID corresponds to a local extension, then it returns
 *  the extension control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */

STATIC INLINE Extension_Control *_Extension_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _Extension_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_extension is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Extension_Is_null(
  Extension_Control *the_extension
);

#include <rtems/extension.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
