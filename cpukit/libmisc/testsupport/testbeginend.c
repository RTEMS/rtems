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

int rtems_test_begin_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
)
{
  return (*printf_func)(
    printf_arg,
    "\n\n*** BEGIN OF TEST %s ***\n",
    rtems_test_name
  );
}

int rtems_test_end_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
)
{
  return (*printf_func)(
    printf_arg,
    "*** END OF TEST %s ***\n",
    rtems_test_name
  );
}
