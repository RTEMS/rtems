/**
 * @file
 *
 * @brief Private Support Information for POSIX Semaphores
 *
 * This include file contains all the private support information for
 * POSIX Semaphores.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SEMAPHORE_H
#define _RTEMS_POSIX_SEMAPHORE_H

#include <rtems/score/objectdata.h>

#include <limits.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIXSemaphorePrivate POSIX Semaphore Private Support
 *
 * @ingroup POSIXAPI
 *
 * This defines the internal implementation support for POSIX semaphores.
 */
/**@{*/

/*
 *  Data Structure used to manage a POSIX semaphore
 */

typedef struct {
   Objects_Control Object;
   sem_t           Semaphore;
   bool            linked;
   uint32_t        open_count;
}  POSIX_Semaphore_Control;

/**
 * @brief The POSIX Semaphore objects information.
 */
extern Objects_Information _POSIX_Semaphore_Information;

/**
 * @brief Macro to define the objects information for the POSIX Semaphore
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define POSIX_SEMAPHORE_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _POSIX_Semaphore, \
    OBJECTS_POSIX_API, \
    OBJECTS_POSIX_SEMAPHORES, \
    POSIX_Semaphore_Control, \
    max, \
    _POSIX_PATH_MAX, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
