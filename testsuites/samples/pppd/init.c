/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIGURE_INIT
#include "system.h"

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtemspppd.h>
#include "netconfig.h"

const char rtems_test_name[] = "PPPD";

rtems_task Init(rtems_task_argument argument)
{
  rtems_test_begin();

  /* initialize network */
  rtems_bsdnet_initialize_network();
  rtems_pppd_initialize();
  pppdapp_initialize();
  rtems_task_delete(RTEMS_SELF);
}
