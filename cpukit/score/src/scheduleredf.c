/**
 * @file
 *
 * @brief Scheduler EDF Initialize and Support
 * @ingroup RTEMSScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );

  _RBTree_Initialize_empty( &context->Ready );
}
