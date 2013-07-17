/**
 *  @file
 *
 *  @brief Trylock CORE Mutex Seize Interrupt
 *  @ingroup ScoreMutex
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
int _CORE_mutex_Seize_interrupt_trylock(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  ISR_Level            level
)
{
  return _CORE_mutex_Seize_interrupt_trylock_body(
    the_mutex,
    executing,
    level
  );
}
#endif
