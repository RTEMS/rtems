/**
 *  @file
 *
 *  @brief Get As Nanoseconds
 *  @ingroup Timespec
 */

/*
 *  COPYRIGHT (c) 2013 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/timespec.h>
#include <rtems/score/todimpl.h>

uint64_t _Timespec_Get_as_nanoseconds(
  const struct timespec *time
)
{
  return ( ((uint64_t) time->tv_sec) * 1000000000ULL ) + time->tv_nsec;
}
