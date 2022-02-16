/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief POSIX Threads Private Support
 *
 * This include file contains all the private support information for
 * POSIX threads.
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

#ifndef _RTEMS_POSIX_PTHREAD_H
#define _RTEMS_POSIX_PTHREAD_H

#include <rtems/posix/threadsup.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_PTHREAD POSIX Threads Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Private Support Information for POSIX Threads
 *
 */
/**@{**/

/**
 *  For now, we are only allowing the user to specify the entry point
 *  and stack size for POSIX initialization threads.
 */
typedef struct {
  /** This is the entry point for a POSIX initialization thread. */
  void       *(*thread_entry)(void *);
  /** This is the stack size for a POSIX initialization thread. */
  int       stack_size;
} posix_initialization_threads_table;

extern const size_t _POSIX_Threads_Minimum_stack_size;

/**
 * @brief Initialization table for the first user POSIX thread.
 *
 * This table is used by _POSIX_Threads_Initialize_user_thread() and
 * initialized via <rtems/confdefs.h>.
 */
extern const posix_initialization_threads_table
  _POSIX_Threads_User_thread_table;

/**
 * @brief System initialization handler to create the first user POSIX thread.
 */
extern void _POSIX_Threads_Initialize_user_thread( void );

/**
 * The following defines the information control block used to manage
 * this class of objects.
 */
extern Thread_Information _POSIX_Threads_Information;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
