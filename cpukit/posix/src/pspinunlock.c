/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function Unlocks a Spin Lock Object
 */

/*
 *  POSIX Spinlock Manager -- Wait at a Spinlock
 *
 *  COPYRIGHT (c) 1989-2007.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/spinlockimpl.h>

int pthread_spin_unlock( pthread_spinlock_t *lock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_Level               level;
#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  SMP_lock_Stats          unused_stats;
  SMP_lock_Stats_context  unused_context;
#endif

  the_spinlock = _POSIX_Spinlock_Get( lock );
  level = the_spinlock->interrupt_state;
#if defined(RTEMS_SMP)
#if defined(RTEMS_PROFILING)
  /* This is a hack to get around the lock profiling statistics */
  unused_stats.total_section_time = 0;
  unused_stats.max_section_time = UINT32_MAX;
  unused_context.stats = &unused_stats;
  unused_context.acquire_instant = 0;
#endif
  _SMP_ticket_lock_Release(
    &the_spinlock->Lock,
    &unused_context
  );
#endif
  _ISR_Local_enable( level );
  return 0;
}
