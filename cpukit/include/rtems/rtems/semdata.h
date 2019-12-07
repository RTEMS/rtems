/**
 * @file
 *
 * @ingroup ClassicSemImpl
 *
 * @brief Classic Semaphore Manager Data Structures
 */

/*
 * COPYRIGHT (c) 1989-2008, 2016.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SEMDATA_H
#define _RTEMS_RTEMS_SEMDATA_H

#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/mrsp.h>
#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicSemImpl
 *
 * @{
 */

/**
 *  The following defines the control block used to manage each semaphore.
 */
typedef struct {
  /**
   * @brief The object management portion of a semaphore instance.
   *
   * A pointer of the node of active semaphores contains the semaphore flags,
   * see _Semaphore_Get_flags().  The rational for this optimization is a
   * reduction of the semaphore control size in general and the ability to
   * allow a configuration dependent size of the semaphore control block, e.g.
   * for the MrsP semaphores.
   */
  Objects_Control          Object;

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
     * @brief The thread queue present in all other variants.
     */
    Thread_queue_Control Wait_queue;

    /**
     *  This is the SuperCore Mutex instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_ceiling_mutex_Control Mutex;

    /**
     *  This is the SuperCore Semaphore instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_semaphore_Control Semaphore;

#if defined(RTEMS_SMP)
    MRSP_Control MRSP;
#endif
  } Core_control;
}   Semaphore_Control;

/**
 * @brief The Classic Semaphore objects information.
 */
extern Objects_Information _Semaphore_Information;

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief Semaphore MP Send Extract Proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Semaphore_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
);
#endif

/**
 * @brief Macro to define the objects information for the Classic Semaphore
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 *   may be set).
 * @param scheduler_count The configured scheduler count (only used in SMP
 *   configurations).
 */
#if defined(RTEMS_SMP)
#define SEMAPHORE_INFORMATION_DEFINE( max, scheduler_count ) \
  typedef union { \
    Objects_Control Object; \
    Semaphore_Control Semaphore; \
    struct { \
      Objects_Control Object; \
      MRSP_Control Control; \
      Priority_Control ceiling_priorities[ scheduler_count ]; \
    } MRSP; \
  } Semaphore_Configured_control; \
  OBJECTS_INFORMATION_DEFINE( \
    _Semaphore, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_SEMAPHORES, \
    Semaphore_Configured_control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Semaphore_MP_Send_extract_proxy \
  )
#else
#define SEMAPHORE_INFORMATION_DEFINE( max, scheduler_count ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Semaphore, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_SEMAPHORES, \
    Semaphore_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Semaphore_MP_Send_extract_proxy \
  )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
