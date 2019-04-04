/**
 * @file
 *
 * @brief Remove Watchdog
 * @ingroup RTEMSScoreWatchdog
 */

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

#include <rtems/score/watchdogimpl.h>

void _Watchdog_Remove(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog
)
{
  if ( _Watchdog_Is_scheduled( the_watchdog ) ) {
    if ( header->first == &the_watchdog->Node.RBTree ) {
      _Watchdog_Next_first( header, the_watchdog );
    }

    _RBTree_Extract( &header->Watchdogs, &the_watchdog->Node.RBTree );
    _Watchdog_Set_state( the_watchdog, WATCHDOG_INACTIVE );
  }
}
