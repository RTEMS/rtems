/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/telnetd.h>

rtems_status_code rtems_telnetd_initialize( void )
{
  return rtems_telnetd_start( &rtems_telnetd_config );
}
