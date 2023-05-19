/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreAtomic
 *
 * @brief This source file contains the implementation of
 *   _Libatomic_Protect_start() and _Libatomic_Protect_end().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <machine/_libatomic.h>

#include <rtems/score/apimutex.h>
#include <rtems/score/atomic.h>
#include <rtems/score/isrlevel.h>

#if defined(RTEMS_SMP)
/*
 * For real SMP targets this is not useful.  The main purpose is support for
 * testing on simulators like SIS.
 */
static Atomic_Flag _Libatomic_The_one_lock = ATOMIC_INITIALIZER_FLAG;
#endif

__uint32_t _Libatomic_Protect_start( void *ptr )
{
  ISR_Level isr_level;

  (void) ptr;
  _ISR_Local_disable( isr_level );

#if defined(RTEMS_SMP)
  while (
    _Atomic_Flag_test_and_set( &_Libatomic_The_one_lock, ATOMIC_ORDER_SEQ_CST )
  ) {
    /* Next try.  Yes, a TAS spin lock implementation is stupid. */
  }
#endif

  return isr_level;
}

void _Libatomic_Protect_end( void *ptr, __uint32_t isr_level )
{
  (void) ptr;

#if defined(RTEMS_SMP)
  _Atomic_Flag_clear( &_Libatomic_The_one_lock, ATOMIC_ORDER_SEQ_CST );
#endif

  _ISR_Local_enable( isr_level );
}

static API_Mutex_Control _Libatomic_Mutex =
  API_MUTEX_INITIALIZER( "_Libatomic" );

void _Libatomic_Lock_n( void *ptr, __size_t n )
{
  (void) ptr;
  (void) n;
  _API_Mutex_Lock( &_Libatomic_Mutex );
}

void _Libatomic_Unlock_n( void *ptr, __size_t n )
{
  (void) ptr;
  (void) n;
  _API_Mutex_Unlock( &_Libatomic_Mutex );
}
