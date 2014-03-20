/**
 * @file
 *
 * @brief Convert 64-bit Timestamp to struct timeval
 *
 * @ingroup SuperCore
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <rtems/score/timestamp.h>

#if CPU_TIMESTAMP_USE_INT64 == TRUE
void _Timestamp64_To_timeval(
  const Timestamp64_Control *_timestamp,
  struct timeval            *_timeval
)
{
  _Timestamp64_implementation_To_timeval( _timestamp, _timeval );
}
#endif
