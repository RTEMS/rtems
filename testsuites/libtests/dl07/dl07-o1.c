/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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

#include "dl-o1.h"

#include <rtems/test-printer.h>
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
