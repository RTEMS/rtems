/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Once
 *
 * This include file contains the static inline implementation of the private
 * inlined routines for POSIX once.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  COPYRIGHT (c) 2013.
 *  Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/objectimpl.h>
#include <rtems/score/percpu.h>

#ifndef _RTEMS_POSIX_ONCEIMPL_H
#define _RTEMS_POSIX_ONCEIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_ONCE
 *
 * @{
 */

/**
 * @brief Lock to allow once mutex's to be initialized.
 */
POSIX_EXTERN pthread_mutex_t _POSIX_Once_Lock;

/**
 * @brief POSIX once manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */
void _POSIX_Once_Manager_initialization(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
