/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-o1.h"

#include <rtems/test.h>
#include "dl-load.h"

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

/*
 * Create some symbols. The uninitialised will be in the common section with
 * separated text and data and this means there is no actual section in the ELF
 * file, the details for this are in the symbols.
 */
int         dl01_bss1;            /* unitialised, .bss */
float       dl01_bss2[30];        /* unitialised, .bss */
char        dl01_bss3[10];        /* unitialised, .bss */
int         dl01_data1 = 1;       /* initialised, .data */
float       dl01_data2 = 0.3333;  /* initialised, .data */
const int   dl01_const1 = 3;      /* read-only, .const */
const float dl01_const2 = 0.666;  /* read-only, .const */
int dl01_func1(void)              /* code, .text */
{
  return 4;
}

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main_o1 (void);

#define DL_NAME       "dlo1"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

int rtems_main_o1 (void)
{
  printf (DL_NAME ": module: %s\n", dl_localise_file (__FILE__));
  printf (DL_NAME ":         dl01_bss1: %4zu: %p: %d\n",   PAINT_VAR (dl01_bss1));
  printf (DL_NAME ":         dl01_bss2: %4zu: %p: %f\n",   PAINT_VAR (dl01_bss2[0]));
  printf (DL_NAME ":         dl01_bss3: %4zu: %p: %02x\n", PAINT_VAR (dl01_bss3[0]));
  printf (DL_NAME ":        dl01_data1: %4zu: %p: %d\n",   PAINT_VAR (dl01_data1));
  /* no  %f in the rtems test printer */
  printf (DL_NAME ":        dl01_data2: %4zu: %p: %f\n",   PAINT_VAR (dl01_data2));
  printf (DL_NAME ":       dl01_const1: %4zu: %p: %d\n",   PAINT_VAR (dl01_const1));
  printf (DL_NAME ":       dl01_const2: %4zu: %p: %f\n",   PAINT_VAR (dl01_const2));
  printf (DL_NAME ":        dl01_func1: %4zu: %p\n",       sizeof(dl01_func1), &dl01_func1);
  return 0;
}
