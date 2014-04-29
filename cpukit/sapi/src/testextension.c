/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/test.h>
#include <rtems/profiling.h>

void rtems_test_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  (void) source;
  (void) is_internal;
  (void) code;

#if defined(RTEMS_PROFILING)
  if (rtems_get_current_processor() == 0) {
    rtems_profiling_report_xml(
      rtems_test_name,
      printk_plugin,
      NULL,
      1,
      "  "
    );

    /* Add some stuff to help broken debuggers */
    printk(
      "*** PROFILING DONE %s ***\n",
      rtems_test_name
    );
  }
#endif
}
