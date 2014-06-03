/**
 * @file
 *
 * @brief Thread Yield
 *
 * @ingroup ScoreThread
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

void _Thread_Yield( Thread_Control *executing )
{
  ISR_Level level;

  _ISR_Disable( level );

  if ( _States_Is_ready( executing->current_state ) ) {
    _Scheduler_Yield( executing );
  }

  _ISR_Enable( level );
}
