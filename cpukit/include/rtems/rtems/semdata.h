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
  /** This field is the object management portion of a Semaphore instance. */
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

  /**
   * @brief The semaphore variant.
   *
   * @see Semaphore_Variant.
   */
  unsigned int variant : 3;

  /**
   * @brief The semaphore thread queue discipline.
   *
   * @see Semaphore_Discipline.
   */
  unsigned int discipline : 1;

#if defined(RTEMS_MULTIPROCESSING)
  unsigned int is_global : 1;
#endif
}   Semaphore_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
