/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/assert.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>

#if defined( RTEMS_DEBUG )
  void _Assert_Thread_dispatching_repressed( void )
  {
    bool dispatch_is_disabled;
    ISR_Level level;
    Per_CPU_Control *per_cpu;

    _ISR_Disable( level );
    per_cpu = _Per_CPU_Get_by_index( _SMP_Get_current_processor() );
    dispatch_is_disabled = per_cpu->thread_dispatch_disable_level != 0;
    _ISR_Enable( level );

    _Assert( dispatch_is_disabled || _ISR_Get_level() != 0 );
  }
#endif
