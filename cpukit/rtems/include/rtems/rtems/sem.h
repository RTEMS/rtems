/**
 * @file rtems/rtems/sem.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Semaphore Manager.  This manager utilizes standard Dijkstra
 *  counting semaphores to provide synchronization and mutual exclusion
 *  capabilities.
 *
 *  Directives provided are:
 *
 *     - create a semaphore
 *     - get an ID of a semaphore
 *     - delete a semaphore
 *     - acquire a semaphore
 *     - release a semaphore
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SEM_H
#define _RTEMS_RTEMS_SEM_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_SEM_EXTERN
#define RTEMS_SEM_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/object.h>
#include <rtems/score/coresem.h>

/**
 *  @defgroup ClassicSem Semaphores
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API
 *  Semaphore Manager.
 */
/**@{*/

/**
 *  The following defines the control block used to manage each semaphore.
 */
typedef struct {
  /** This field is the object management portion of a Semaphore instance. */
  Objects_Control          Object;

  /**
   *  This is the Classic API attribute provided to the create directive.
   *  It is translated into behavioral attributes on the SuperCore Semaphore
   *  or Mutex instance.
   */
  rtems_attribute          attribute_set;

  /**
   *  This contains the memory associated with the SuperCore Semaphore or
   *  Mutex instance that provides the primary functionality of each
   *  Classic API Semaphore instance.  The structure used is dependent
   *  on the attributes specified by the user on the create directive.
   *
   *  @note Only one of these has meaning in a particular Classic API
   *        Semaphore instance.
   */
  union {
    /**
     *  This is the SuperCore Mutex instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_mutex_Control     mutex;

    /**
     *  This is the SuperCore Semaphore instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_semaphore_Control semaphore;
  } Core_control;
}   Semaphore_Control;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
RTEMS_SEM_EXTERN Objects_Information  _Semaphore_Information;

/**
 *  @brief Semaphore_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Semaphore_Manager_initialization(void);

/**
 *  @brief rtems_semaphore_create
 *
 *  This routine implements the rtems_semaphore_create directive.  The
 *  semaphore will have the name name.  The starting count for
 *  the semaphore is count.  The attribute_set determines if
 *  the semaphore is global or local and the thread queue
 *  discipline.  It returns the id of the created semaphore in ID.
 */
rtems_status_code rtems_semaphore_create(
  rtems_name           name,
  uint32_t             count,
  rtems_attribute      attribute_set,
  rtems_task_priority  priority_ceiling,
  rtems_id            *id
);

/**
 *  @brief rtems_semaphore_ident
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
  uint32_t      node,
  rtems_id     *id
);

/**
 *  @brief rtems_semaphore_delete
 *
 *  This routine implements the rtems_semaphore_delete directive.  The
 *  semaphore indicated by ID is deleted.
 */
rtems_status_code rtems_semaphore_delete(
  rtems_id   id
);

/**
 *  @brief rtems_semaphore_obtain
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
  rtems_option   option_set,
  rtems_interval timeout
);

/**
 *  @brief rtems_semaphore_release
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

/**
 *  @brief rtems_semaphore_flush
 *  pending on the semaphore.
 */
rtems_status_code rtems_semaphore_flush(
  rtems_id	   id
);

/**
 *  @brief _Semaphore_Seize
 *
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the RTEMS_NO_WAIT option is enabled in
 *  option_set, then the routine returns.  Otherwise, the calling task
 *  is blocked until a unit becomes available.
 */
bool _Semaphore_Seize(
  Semaphore_Control *the_semaphore,
  uint32_t           option_set
);

/**
 *  @brief _Semaphore_Translate_core_mutex_return_code
 *
 *  This function returns a RTEMS status code based on the mutex
 *  status code specified.
 */
rtems_status_code _Semaphore_Translate_core_mutex_return_code (
  uint32_t   the_mutex_status
);

/**
 *  @brief _Semaphore_Translate_core_semaphore_return_code
 *
 *  This function returns a RTEMS status code based on the semaphore
 *  status code specified.
 */
rtems_status_code _Semaphore_Translate_core_semaphore_return_code (
  uint32_t   the_mutex_status
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

/**@}*/

#endif
/*  end of include file */
