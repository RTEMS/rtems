/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS File System Mutex
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Mutex.
 *
 * It may be suprising we abstract this for the RTEMS file system but this code
 * is designed to be run on host operating systems.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
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

#if !defined (_RTEMS_RFS_MUTEX_H_)
#define _RTEMS_RFS_MUTEX_H_

#include <errno.h>

#include <rtems/rfs/rtems-rfs-trace.h>

#if __rtems__
#include <rtems.h>
#include <rtems/error.h>
#include <rtems/thread.h>
#endif

/**
 * RFS Mutex type.
 */
#if __rtems__
typedef rtems_recursive_mutex rtems_rfs_mutex;
#else
typedef uint32_t rtems_rfs_mutex; /* place holder */
#endif

/**
 * @brief Create the mutex.
 *
 * @param [in] mutex is pointer to the mutex handle returned to the caller.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 *
 */
int rtems_rfs_mutex_create (rtems_rfs_mutex* mutex);

/**
 * @brief Destroy the mutex.
 *
 * @param[in] mutex Reference to the mutex handle returned to the caller.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
int rtems_rfs_mutex_destroy (rtems_rfs_mutex* mutex);

/**
 * @brief Lock the mutex.
 *
 * @param[in] mutex is a pointer to the mutex to lock.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
static inline int
rtems_rfs_mutex_lock (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_recursive_mutex_lock(mutex);
#endif
  return 0;
}

/**
 * @brief Unlock the mutex.
 *
 * @param[in] mutex is a pointer to the mutex to unlock.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
static inline int
rtems_rfs_mutex_unlock (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_recursive_mutex_unlock(mutex);
#endif
  return 0;
}

#endif
