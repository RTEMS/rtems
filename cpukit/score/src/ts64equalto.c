/**
 *  @file
 *
 *  @brief Timestamp equal to Operator
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
bool _Timestamp64_Equal_to(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs
)
{
  return _Timestamp64_implementation_Equal_to( _lhs, _rhs );
}
#endif
