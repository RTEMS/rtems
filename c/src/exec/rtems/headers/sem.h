/*  semaphore.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Semaphore Manager.  This manager utilizes standard Dijkstra
 *  counting semaphores to provide synchronization and mutual exclusion
 *  capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
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

#ifndef __RTEMS_SEMAPHORE_h
#define __RTEMS_SEMAPHORE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/attr.h>
#include <rtems/object.h>
#include <rtems/threadq.h>

/*
 *  The following defines the control block used to manage each semaphore.
 */

typedef struct {
  Objects_Control       Object;
  Thread_queue_Control  Wait_queue;
  rtems_attribute    attribute_set;
  unsigned32            count;
  unsigned32            nest_count;
  Thread_Control       *holder;
  Objects_Id            holder_id;
}   Semaphore_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

EXTERN Objects_Information  _Semaphore_Information;

/*
 *  _Semaphore_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
);

/*
 *  rtems_semaphore_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_semaphore_create directive.  The
 *  semaphore will have the name name.  The starting count for
 *  the semaphore is count.  The attribute_set determines if
 *  the semaphore is global or local and the thread queue
 *  discipline.  It returns the id of the created semaphore in ID.
 */

rtems_status_code rtems_semaphore_create(
  Objects_Name        name,
  unsigned32          count,
  rtems_attribute  attribute_set,
  Objects_Id         *id
);

/*
 *  rtems_semaphore_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_semaphore_ident directive.
 *  This directive returns the semaphore ID associated with name.
 *  If more than one semaphore is named name, then the semaphore
 *  to which the ID belongs is arbitrary.  node indicates the
 *  extent of the search for the ID of the semaphore named name.
 *  The search can be limited to a particular node or allowed to
 *  encompass all nodes.
 */

rtems_status_code rtems_semaphore_ident(
  Objects_Name  name,
  unsigned32    node,
  Objects_Id   *id
);

/*
 *  rtems_semaphore_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_semaphore_delete directive.  The
 *  semaphore indicated by ID is deleted.
 */

rtems_status_code rtems_semaphore_delete(
  Objects_Id id
);

/*
 *  rtems_semaphore_obtain
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_semaphore_obtain directive.  It
 *  attempts to obtain a unit from the semaphore associated with ID.
 *  If a unit can be allocated, the calling task will return immediately.
 *  If no unit is available, then the task may return immediately or
 *  block waiting for a unit with an optional timeout of timeout
 *  clock ticks.  Whether the task blocks or returns immediately
 *  is based on the RTEMS_NO_WAIT option in the option_set.
 */

rtems_status_code rtems_semaphore_obtain(
  Objects_Id        id,
  unsigned32        option_set,
  rtems_interval timeout
);

/*
 *  rtems_semaphore_release
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_semaphore_release directive.  It
 *  frees a unit to the semaphore associated with ID.  If a task was
 *  blocked waiting for a unit from this semaphore, then that task will
 *  be readied and the unit given to that task.  Otherwise, the unit
 *  will be returned to the semaphore.
 */

rtems_status_code rtems_semaphore_release(
  Objects_Id id
);

/*
 *  _Semaphore_Seize
 *
 *  DESCRIPTION:
 *
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the RTEMS_NO_WAIT option is enabled in
 *  option_set, then the routine returns.  Otherwise, the calling task
 *  is blocked until a unit becomes available.
 */

boolean _Semaphore_Seize(
  Semaphore_Control *the_semaphore,
  unsigned32         option_set
);

/*
 *  _Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */

STATIC INLINE Semaphore_Control *_Semaphore_Allocate( void );

/*
 *  _Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */

STATIC INLINE void _Semaphore_Free (
  Semaphore_Control *the_semaphore
);

/*
 *  _Semaphore_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 */

STATIC INLINE Semaphore_Control *_Semaphore_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _Semaphore_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_semaphore is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Semaphore_Is_null (
  Semaphore_Control *the_semaphore
);

#include <rtems/sem.inl>
#include <rtems/semmp.h>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
