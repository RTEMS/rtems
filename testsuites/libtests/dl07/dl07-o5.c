/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "dl-load.h"
#include "dl-o1.h"
#include "dl-o5.h"

#include <inttypes.h>
#include <rtems/test-printer.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);


uint64_t dl05_unresolv_1;
uint16_t dl05_unresolv_2;
uint32_t dl05_unresolv_3;
uint8_t  dl05_unresolv_4;
int64_t  dl05_unresolv_5;

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main_o5 (int argc, const char* argv[]);

#define DL_NAME       "dlo5"
#define PAINT_VAR(_v) sizeof(_v), &_v, _v

int rtems_main_o5 (int argc, const char* argv[])
{
  (void) argc;
  (void) argv;

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
  printf (DL_NAME ":   dl05_unresolv_1: %4zu: %p: %" PRIu64 "\n", PAINT_VAR (dl05_unresolv_1));
  printf (DL_NAME ":   dl05_unresolv_2: %4zu: %p: %" PRIu16 "\n", PAINT_VAR (dl05_unresolv_2));
  printf (DL_NAME ":   dl05_unresolv_3: %4zu: %p: %" PRIu32 "\n", PAINT_VAR (dl05_unresolv_3));
  printf (DL_NAME ":   dl05_unresolv_4: %4zu: %p: %" PRIu8  "\n", PAINT_VAR (dl05_unresolv_4));
  printf (DL_NAME ":   dl05_unresolv_5: %4zu: %p: %" PRIi64 "\n", PAINT_VAR (dl05_unresolv_5));
  return 0;
}
