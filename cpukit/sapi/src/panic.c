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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/fatal.h>
#include <rtems/bspIo.h>

#include <stdarg.h>

void rtems_panic( const char *fmt, ... )
{
  va_list ap;

  va_start( ap, fmt );
  vprintk( fmt, ap );
  va_end( ap);

  _Terminate( RTEMS_FATAL_SOURCE_PANIC, (Internal_errors_t) fmt );
}
