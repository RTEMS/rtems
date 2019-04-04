/**
 * @file
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief ISR Is In Progress Default Implementation
 */

/*
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>

#if CPU_PROVIDES_ISR_IS_IN_PROGRESS == FALSE

bool _ISR_Is_in_progress( void )
{
  uint32_t isr_nest_level;

  #if defined( RTEMS_SMP )
    ISR_Level level;

    _ISR_Local_disable( level );
  #endif

  isr_nest_level = _ISR_Nest_level;

  #if defined( RTEMS_SMP )
    _ISR_Local_enable( level );
  #endif

  return isr_nest_level != 0;
}

#endif
