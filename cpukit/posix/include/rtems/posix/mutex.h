/**
 * @file
 * 
 * @brief POSIX MUTEX Support
 *
 * This include file contains all the private support information for
 * POSIX mutex's.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MUTEX_H
#define _RTEMS_POSIX_MUTEX_H

#include <rtems/score/coremutex.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_MUTEX POSIX Mutex Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Private Support Information for POSIX Mutex
 * 
 */
/**@{**/

/*
 *  Data Structure used to manage a POSIX mutex
 */

typedef struct {
   Objects_Control     Object;
   int                 process_shared;
   CORE_mutex_Control  Mutex;
}  POSIX_Mutex_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
