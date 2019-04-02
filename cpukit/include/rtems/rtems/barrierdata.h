/**
 * @file
 *
 * @ingroup ClassicBarrierImpl
 *
 * @brief Classic Barrier Manager Data Structures
 */


/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_BARRIERDATA_H
#define _RTEMS_RTEMS_BARRIERDATA_H

#include <rtems/rtems/barrier.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/corebarrier.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicBarrierImpl
 *
 * @{
 */

/**
 *  This type defines the control block used to manage each barrier.
 */
typedef struct {
  /** This is used to manage a barrier as an object. */
  Objects_Control          Object;
  /** This is used to implement the barrier. */
  CORE_barrier_Control     Barrier;
  /** This is used to specify the attributes of a barrier. */
  rtems_attribute          attribute_set;
}   Barrier_Control;

/**
 * @brief The Classic Barrier objects information.
 */
extern Objects_Information _Barrier_Information;

/**
 * @brief Macro to define the objects information for the Classic Barrier
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define BARRIER_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Barrier, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_BARRIERS, \
    Barrier_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
