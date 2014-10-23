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

#if defined(RTEMS_SMP)
#include <rtems/score/smpimpl.h>
#endif

#if defined(RTEMS_PROFILING)
static bool should_report(
  rtems_fatal_source source,
  rtems_fatal_code code
)
{
#if defined(RTEMS_SMP)
  return source != RTEMS_FATAL_SOURCE_SMP
    || code != SMP_FATAL_SHUTDOWN_RESPONSE;
#else
  (void) source;
  (void) code;

  return true;
#endif
}
#endif

void rtems_test_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  (void) is_internal;

#if defined(RTEMS_PROFILING)
  if ( should_report( source, code ) ) {
    printk(
      "\n*** PROFILING REPORT BEGIN %s ***\n",
      rtems_test_name
    );

    rtems_profiling_report_xml(
      rtems_test_name,
      printk_plugin,
      NULL,
      1,
      "  "
    );

    printk(
      "*** PROFILING REPORT END %s ***\n",
      rtems_test_name
    );
  }
#endif
}
