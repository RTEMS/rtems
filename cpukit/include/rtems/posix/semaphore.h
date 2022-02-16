/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
