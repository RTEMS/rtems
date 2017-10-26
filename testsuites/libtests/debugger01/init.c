/*
 *  Debugger test.
 *
 *  Copyright (c) 2016 Chris Johns (chrisj@rtems.org)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include "tmacros.h"

#include <rtems/rtems-debugger.h>

#include "system.h"

static void test(void)
{
  rtems_test_assert(rtems_debugger_start("test", "something",
                                         3, 10, &rtems_test_printer) < 0);
  rtems_test_assert(rtems_debugger_register_test_remote() == 0);
  rtems_test_assert(rtems_debugger_start("test", "something",
                                         3, 10, &rtems_test_printer) == 0);
  rtems_debugger_set_verbose(true);
  rtems_test_assert(rtems_debugger_remote_debug(true) == 0);

  /*
   * This thread is suspended when the debugger is active until the debugger
   * disconnects.
   */
  sleep(1);
}

const char rtems_test_name[] = "DEBUGGER01";

rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  test();

  TEST_END();
}
