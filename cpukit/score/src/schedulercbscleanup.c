/**
 *  @file
 *
 *  @brief Scheduler CBS Cleanup
 *  @ingroup RTEMSScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbs.h>
#include <rtems/score/wkspace.h>

int _Scheduler_CBS_Cleanup (void)
{
  unsigned int i;

  for ( i = 0; i<_Scheduler_CBS_Maximum_servers; i++ ) {
    if ( _Scheduler_CBS_Server_list[ i ].initialized )
      _Scheduler_CBS_Destroy_server( i );
  }
  return SCHEDULER_CBS_OK;
}
