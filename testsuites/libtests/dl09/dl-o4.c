/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-load.h"
#include "dl-o4.h"
#include "dl-o5.h"

#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

int         dl04_unresolv_1;
float       dl04_unresolv_2;
char        dl04_unresolv_3;
char*       dl04_unresolv_4;
const int   dl04_unresolv_5 = 4;
const char* dl04_unresolv_6 = "dl-O4";

#define DL_NAME       "dlo4"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

int rtems_main_o4 (void)
{
  printf (DL_NAME ": module: %s\n", dl_localise_file (__FILE__));
  printf (DL_NAME ":   dl04_unresolv_1: %4u: %p: %d\n",   PAINT_VAR (dl04_unresolv_1));
  printf (DL_NAME ":   dl04_unresolv_2: %4u: %p: %f\n",   PAINT_VAR (dl04_unresolv_2));
  printf (DL_NAME ":   dl04_unresolv_3: %4u: %p: %02x\n", PAINT_VAR (dl04_unresolv_3));
  printf (DL_NAME ":   dl04_unresolv_4: %4u: %p: %p\n",   PAINT_VAR (dl04_unresolv_4));
  printf (DL_NAME ":   dl04_unresolv_5: %4u: %p: %d\n",   PAINT_VAR (dl04_unresolv_5));
  printf (DL_NAME ":   dl04_unresolv_6: %4u: %p: %s\n",   PAINT_VAR (dl04_unresolv_6));

  rtems_main_o5 ();

  return 0;
}
