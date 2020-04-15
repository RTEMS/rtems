/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/ftpd.h>

rtems_status_code rtems_initialize_ftpd( void )
{
  rtems_ftpd_configuration.verbose = true;
  return rtems_ftpd_start( &rtems_ftpd_configuration );
}
