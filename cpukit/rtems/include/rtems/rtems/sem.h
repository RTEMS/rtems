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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_SEMAPHORE_h
#define __RTEMS_SEMAPHORE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/object.h>
#include <rtems/score/coresem.h>
#include <rtems/score/threadq.h>

/*
 *  The following defines the control block used to manage each semaphore.
 */

typedef struct {
  Objects_Control          Object;
  rtems_attribute          attribute_set;
  union {
    CORE_mutex_Control     mutex;
    CORE_semaphore_Control semaphore;
  } Core_control;
}   Semaphore_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

RTEMS_EXTERN Objects_Information  _Semaphore_Information;

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
  rtems_name           name,
  unsigned32           count,
  rtems_attribute      attribute_set,
  rtems_task_priority  priority_ceiling,
  rtems_id            *id
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
  rtems_name    name,
  unsigned32    node,
  rtems_id     *id
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
  rtems_id   id
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
  rtems_id       id,
  unsigned32     option_set,
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
  rtems_id   id
);

/*
 *  rtems_semaphore_flush
 *
 *  This directive allows a thread to flush the threads
 *  pending on the semaphore.
 */

rtems_status_code rtems_semaphore_flush(
  Objects_Id      id
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
 *  _Semaphore_Translate_core_mutex_return_code
 *
 *  DESCRIPTION:
 *
 *  This function returns a RTEMS status code based on the mutex
 *  status code specified.
 */
 
rtems_status_code _Semaphore_Translate_core_mutex_return_code (
  unsigned32 the_mutex_status
);

/*
 *  _Semaphore_Translate_core_semaphore_return_code
 *
 *  DESCRIPTION:
 *
 *  This function returns a RTEMS status code based on the semaphore
 *  status code specified.
 */
 
rtems_status_code _Semaphore_Translate_core_semaphore_return_code (
  unsigned32 the_mutex_status
);
 
/*PAGE
 *
 *  _Semaphore_Core_mutex_mp_support
 *
 *  DESCRIPTION:
 *
 *  This function processes the global actions necessary for remote
 *  accesses to a global semaphore based on a core mutex.  This function
 *  is called by the core.
 */

#if defined(RTEMS_MULTIPROCESSING)
void  _Semaphore_Core_mutex_mp_support (
  Thread_Control *the_thread,
  rtems_id        id
);
#endif

/*PAGE
 *
 *  _Semaphore_Core_mp_support
 *
 *  DESCRIPTION:
 *
 *  This function processes the global actions necessary for remote 
 *  accesses to a global semaphore based on a core semaphore.  This function
 *  is called by the core.
 */
 
void  _Semaphore_Core_semaphore_mp_support (
  Thread_Control *the_thread,
  rtems_id        id
);

/*PAGE
 *
 *  _POSIX_Semaphore_MP_support
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

void _POSIX_Semaphore_MP_support(
  Thread_Control *the_thread,
  Objects_Id      id
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/sem.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/semmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
