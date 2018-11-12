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

#include <semaphore.h>
#include <rtems/score/objectdata.h>

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

extern const uint32_t _Configuration_POSIX_Maximum_named_semaphores;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
