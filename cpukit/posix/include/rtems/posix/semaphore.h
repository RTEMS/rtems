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
#include <rtems/score/object.h>
#include <rtems/score/coresem.h>

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
   Objects_Control         Object;
   int                     process_shared;
   bool                    named;
   bool                    linked;
   uint32_t                open_count;
   CORE_semaphore_Control  Semaphore;
   /*
    *  sem_t is 32-bit.  If Object_Id is 16-bit, then they are not
    *  interchangeable.  We have to be able to return a pointer to
    *  a 32-bit form of the 16-bit Id.
    */
   #if defined(RTEMS_USE_16_BIT_OBJECT)
     sem_t                 Semaphore_id;
   #endif
}  POSIX_Semaphore_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
