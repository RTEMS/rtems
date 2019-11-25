/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-load.h"
#include "dl-o6.h"

#include <inttypes.h>
#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

uint64_t dl06_unresolv_1;
uint16_t dl06_unresolv_2;
uint32_t dl06_unresolv_3;
uint8_t  dl06_unresolv_4;
int64_t  dl06_unresolv_5;

#define DL_NAME       "dlo6"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

void dl06_constructor (void) __attribute__ ((constructor));
void dl06_destructor (void) __attribute__ ((destructor));

void dl06_constructor (void)
{
  printf (DL_NAME ": CONSTRUCTOR: %s\n", dl_localise_file (__FILE__));
  printf (DL_NAME ":   dl06_unresolv_1: %4zu: %p: %" PRIu64 "\n", PAINT_VAR (dl06_unresolv_1));
  printf (DL_NAME ":   dl06_unresolv_2: %4zu: %p: %" PRIu16 "\n", PAINT_VAR (dl06_unresolv_2));
  printf (DL_NAME ":   dl06_unresolv_3: %4zu: %p: %" PRIu32 "\n", PAINT_VAR (dl06_unresolv_3));
  printf (DL_NAME ":   dl06_unresolv_4: %4zu: %p: %" PRIu8  "\n", PAINT_VAR (dl06_unresolv_4));
  printf (DL_NAME ":   dl06_unresolv_5: %4zu: %p: %" PRIi64 "\n", PAINT_VAR (dl06_unresolv_5));
}

void dl06_destructor (void)
{
  printf (DL_NAME ": DESTRUCTOR: %s\n", dl_localise_file (__FILE__));
}

int rtems_main_o6 (void)
{
  printf (DL_NAME ": module: %s\n", dl_localise_file (__FILE__));
  printf (DL_NAME ":  long file name to test extended name loading\n");

  return 0;
}
