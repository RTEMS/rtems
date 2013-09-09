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

#include <rtems/score/todimpl.h>

uint32_t _TOD_Seconds_since_epoch( void )
{
  TOD_Control      *tod = &_TOD;
  ISR_Level         level;
  Timestamp_Control now;

  _TOD_Acquire( tod, level );
   now = tod->now;
  _TOD_Release( tod, level );

  return _Timestamp_Get_seconds( &now );
}
