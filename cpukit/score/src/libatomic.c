/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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

/*
 * FIXME: The once lock should be only a temporary solution. We need a
 * dedicated internal mutex for this.
 */

void _Libatomic_Lock_n( void *ptr, __size_t n )
{
  (void) ptr;
  (void) n;
  _Once_Lock();
}

void _Libatomic_Unlock_n( void *ptr, __size_t n )
{
  (void) ptr;
  (void) n;
  _Once_Unlock();
}
