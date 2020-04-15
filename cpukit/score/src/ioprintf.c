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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/io.h>

int _IO_Printf( IO_Put_char put_char, void *arg, char const  *fmt, ... )
{
  va_list ap;
  int     len;

  va_start( ap, fmt );
  len = _IO_Vprintf( put_char, arg, fmt, ap );
  va_end( ap );

  return len;
}
