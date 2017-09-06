/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <time.h>

#include <rtems/score/basedefs.h>
#include <rtems/score/timecounter.h>

RTEMS_STATIC_ASSERT( CLOCKS_PER_SEC == 1000000, clocks_per_sec );

clock_t clock( void )
{
  struct timeval tv;

  _Timecounter_Microuptime( &tv );

  return (clock_t) (tv.tv_sec - 1) * CLOCKS_PER_SEC + tv.tv_usec;
}
