/**
 * @file
 *
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This source file contains the implementation of vprintk().
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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
