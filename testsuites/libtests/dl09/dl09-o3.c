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
#include "dl-o3.h"
#include "dl-o4.h"
#include "dl-o5.h"

#include <inttypes.h>
#include <rtems/test-printer.h>

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
