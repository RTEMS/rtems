/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

#include "dl06-o2.h"

#include <dlfcn.h>
#include <math.h>

#include <rtems/test-printer.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

typedef void (*func1_t)(unsigned short s[7]);

static void* find_sym(const char* name)
{
  void* sym = dlsym(RTLD_DEFAULT, name);
  if (sym == NULL)
    printf("dlsym failed: not found: %s\n", name);
  return sym;
}

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main (int argc, const char* argv[]);

int rtems_main (int argc, const char* argv[])
{
  func1_t        f1;
  double         d;
  unsigned short s[7] = { 12, 34, 56, 78, 90, 13, 57 };

  printf("Loaded module: argc:%d [%s]\n", argc, __FILE__);

  f1 = find_sym ("dl_o2_func1");
  if (f1 == NULL)
    return 0;

  f1 (s);

  d = dl_o2_func2 (7.1, 33.0);
  (void) d;

  d = dl_o2_func3 (0.778899);
  (void) d;


  return argc;
}
