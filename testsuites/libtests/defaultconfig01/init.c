/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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
