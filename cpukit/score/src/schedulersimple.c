/**
 * @file
 *
 * @brief Scheduler Simple Functions
 *
 * @ingroup RTEMSScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/score/chainimpl.h>

void _Scheduler_simple_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );

  _Chain_Initialize_empty( &context->Ready );
}
