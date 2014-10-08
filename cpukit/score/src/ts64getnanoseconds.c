/**
 *  @file  score/src/ts64toticks.c
 *
 *  @brief Get Nanoseconds Portion of Timestamp
 *  @ingroup SuperCore
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/timestamp.h>

#if CPU_TIMESTAMP_USE_INT64 == TRUE
uint32_t _Timestamp64_Get_nanoseconds(
  const Timestamp64_Control *_time
)
{
  return _Timestamp64_implementation_Get_nanoseconds( _time );
}
#endif
