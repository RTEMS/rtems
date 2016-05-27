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

/**
 * @brief The POSIX mutex control.
 */
typedef struct {
  /**
   * @brief The object control.
   */
  Objects_Control Object;

  /**
   * The most general mutex variant supported by a POSIX mutex.
   *
   * The priority inheritance or no protocol variants will use only parts of
   * this structure.
   */
  CORE_ceiling_mutex_Control Mutex;

  /**
   * @brief The protocol variant.
   *
   * @see POSIX_Mutex_Protocol.
   */
  unsigned int protocol : 2;

  /**
   * @brief Indicates if this is a non-recursive or recursive mutex.
   */
  unsigned int is_recursive : 1;
} POSIX_Mutex_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
