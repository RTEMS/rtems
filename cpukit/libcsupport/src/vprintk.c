/**
 *  @file
 *
 *  @brief Print Formatted Output
 *  @ingroup libcsupport
 */

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

#include <rtems/bspIo.h>
#include <rtems/score/io.h>

int vprintk( const char *fmt, va_list ap )
{
  return _IO_Vprintf( rtems_put_char, NULL, fmt, ap );
}
