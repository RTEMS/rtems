/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-load.h"
#include "dl-o1.h"
#include "dl-o2.h"
#include "dl-o4.h"
#include "dl-o5.h"

#include <inttypes.h>
#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main_o3 (int argc, const char* argv[]);

#define DL_NAME      "dlo3"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

int rtems_main_o3 (int argc, const char* argv[])
{
  printf (DL_NAME ": module: %s\n", dl_localise_file (__FILE__));
  printf (DL_NAME ":         dl01_bss1: %4zu: %p: %d\n",          PAINT_VAR (dl01_bss1));
  printf (DL_NAME ":         dl01_bss2: %4zu: %p: %f\n",          PAINT_VAR (dl01_bss2[0]));
  printf (DL_NAME ":         dl01_bss3: %4zu: %p: %02x\n",        PAINT_VAR (dl01_bss3[0]));
  printf (DL_NAME ":        dl01_data1: %4zu: %p: %d\n",          PAINT_VAR (dl01_data1));
  /* no  %f in the rtems test printer */
  printf (DL_NAME ":        dl01_data2: %4zu: %p: %f\n",          PAINT_VAR (dl01_data2));
  printf (DL_NAME ":       dl01_const1: %4zu: %p: %d\n",          PAINT_VAR (dl01_const1));
  printf (DL_NAME ":       dl01_const2: %4zu: %p: %f\n",          PAINT_VAR (dl01_const2));
  printf (DL_NAME ":        dl01_func1: %4zu: %p\n",              sizeof(dl01_func1), &dl01_func1);
  printf (DL_NAME ":         dl02_bss1: %4zu: %p: %d\n",          PAINT_VAR (dl02_bss1));
  printf (DL_NAME ":         dl02_bss2: %4zu: %p: %f\n",          PAINT_VAR (dl02_bss2[0]));
  printf (DL_NAME ":         dl02_bss3: %4zu: %p: %02x\n",        PAINT_VAR (dl02_bss3[0]));
  printf (DL_NAME ":        dl02_data1: %4zu: %p: %d\n",          PAINT_VAR (dl02_data1));
  /* no %f in the rtems test printer */
  printf (DL_NAME ":        dl02_data2: %4zu: %p: %f\n",          PAINT_VAR (dl02_data2));
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
  return 0;
}
