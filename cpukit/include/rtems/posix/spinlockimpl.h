/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief This header file provides interfaces used by the POSIX Spinlock
 *   implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_POSIX_SPINLOCKIMPL_H
#define _RTEMS_POSIX_SPINLOCKIMPL_H

#include <pthread.h>

#include <rtems/score/isrlevel.h>

#if defined(RTEMS_SMP)
#include <rtems/score/percpu.h>
#include <rtems/score/smplockticket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
#if defined(RTEMS_SMP)
  SMP_ticket_lock_Control Lock;
#else
  unsigned int reserved[ 2 ];
#endif
  ISR_Level interrupt_state;
} POSIX_Spinlock_Control;

static inline POSIX_Spinlock_Control *_POSIX_Spinlock_Get(
  pthread_spinlock_t *lock
)
{
  return (POSIX_Spinlock_Control *) lock;
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
