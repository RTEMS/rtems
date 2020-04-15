/**
 * @file
 *
 * @brief Invokes the internal error handler with a source of
 *        INTERNAL_ERROR_RTEMS_API and is internal set to false.
 *
 * @ingroup ClassicFatal
 */

/*
 *  Fatal Error Manager
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/fatal.h>
#include <rtems/score/interr.h>

void rtems_fatal_error_occurred(
  uint32_t   the_error
)
{
  _Terminate( INTERNAL_ERROR_RTEMS_API, the_error );

/* will not return from this routine */
}
