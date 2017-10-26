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

#include <stdio.h>

#include <bsp.h>

#include "tmacros.h"

const char rtems_test_name[] = "DEFAULTCONFIG 1";

static void install_bsp_extension(void)
{
#ifdef BSP_INITIAL_EXTENSION
  static const rtems_extensions_table bsp_ext = BSP_INITIAL_EXTENSION;

  rtems_status_code sc;
  rtems_id id;

  sc = rtems_extension_create(
    rtems_build_name('B', 'S', 'P', ' '),
    &bsp_ext,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
#endif
}

int main(int argc, char **argv)
{
  int i;

  TEST_BEGIN();

  install_bsp_extension();

  for (i = 0; i < argc; ++i) {
    printf("argv[%i] = %s\n", i, argv[i]);
  }

  TEST_END();

  return 0;
}
