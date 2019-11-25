/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-load.h"
#include "dl-o3.h"
#include "dl-o4.h"
#include "dl-o5.h"

#include <inttypes.h>
#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

#define DL_NAME      "dlo3"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

int rtems_main_o3 ()
{
  printf (DL_NAME ": module: %s : %p\n",
	  dl_localise_file (__FILE__), rtems_main_o3);
  printf (DL_NAME ":   dl04_unresolv_1: %4zu: %p: %d\n",          PAINT_VAR (dl04_unresolv_1));
  printf (DL_NAME ":   dl04_unresolv_2: %4zu: %p: %f\n",          PAINT_VAR (dl04_unresolv_2));
  printf (DL_NAME ":   dl04_unresolv_3: %4zu: %p: %02x\n",        PAINT_VAR (dl04_unresolv_3));
  printf (DL_NAME ":   dl04_unresolv_4: %4zu: %p: %p\n",          PAINT_VAR (dl04_unresolv_4));
  printf (DL_NAME ":   dl04_unresolv_5: %4zu: %p: %d\n",          PAINT_VAR (dl04_unresolv_5));
  printf (DL_NAME ":   dl04_unresolv_6: %4zu: %p: %s\n",          PAINT_VAR (dl04_unresolv_6));
  printf (DL_NAME ":   dl05_unresolv_1: %4zu: %p: %" PRIu64 "\n", PAINT_VAR (dl05_unresolv_1));
  printf (DL_NAME ":   dl05_unresolv_2: %4zu: %p: %" PRIu16 "\n", PAINT_VAR (dl05_unresolv_2));
  printf (DL_NAME ":   dl05_unresolv_3: %4zu: %p: %" PRIu32 "\n", PAINT_VAR (dl05_unresolv_3));
  printf (DL_NAME ":   dl05_unresolv_4: %4zu: %p: %" PRIu8  "\n", PAINT_VAR (dl05_unresolv_4));
  printf (DL_NAME ":   dl05_unresolv_5: %4zu: %p: %" PRIi64 "\n", PAINT_VAR (dl05_unresolv_5));

  rtems_main_o4 ();

  return 0;
}
