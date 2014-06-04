/**
 * @file
 *
 * @brief Initiates system termination
 * @ingroup ScoreIntErr
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/interr.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>

System_state_Codes _System_state_Current;

Internal_errors_Information _Internal_errors_What_happened;

void _Terminate(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  Internal_errors_t       the_error
)
{
  ISR_Level level;

  _ISR_Disable_without_giant( level );
  (void) level;

  _SMP_Request_shutdown();

  _User_extensions_Fatal( the_source, is_internal, the_error );

  _Internal_errors_What_happened.the_source  = the_source;
  _Internal_errors_What_happened.is_internal = is_internal;
  _Internal_errors_What_happened.the_error   = the_error;

  _System_state_Set( SYSTEM_STATE_TERMINATED );

  _CPU_Fatal_halt( the_source, the_error );

  /* will not return from this routine */
  while (true);
}
