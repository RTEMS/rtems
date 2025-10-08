/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#include "tmacros.h"

#include <libfdt.h>
#include <string.h>

#include "some.h"

/*
 * To generate the FDT blob use:
 *
 * dtc some.dts -O asm > some.s
 * as some.s -o some.o
 * objcopy -O binary some.o some.bin
 * rtems-bin2c some.bin some.c
 */

const char rtems_test_name[] = "LIBFDT 1";

static void test(void)
{
  const void *fdt = some_bin;
  int status;
  uint32_t size;
  const char *alias;
  int root;
  int cells;
  const char *prop;
  fdt32_t *prop_32;
  int len;
  int d;
  int m;
  int t;
  int node;
  uint32_t phandle;
  uint32_t cell;

  status = fdt_check_header(fdt);
  rtems_test_assert(status == 0);

  size = fdt_totalsize(fdt);
  rtems_test_assert(size == some_bin_size);

  alias = fdt_get_alias(fdt, "nix");
  rtems_test_assert(alias == NULL);

  alias = fdt_get_alias(fdt, "k");
  rtems_test_assert(strcmp(alias, "/m@1248") == 0);

  root = fdt_path_offset(fdt, "nix");
  rtems_test_assert(root == -FDT_ERR_BADPATH);

  root = fdt_path_offset(fdt, "/");
  rtems_test_assert(root >= 0);

  cells = fdt_address_cells(fdt, root);
  rtems_test_assert(cells == 1);

  cells = fdt_size_cells(fdt, root);
  rtems_test_assert(cells == 2);

  status = fdt_node_check_compatible(fdt, root, "a,b");
  rtems_test_assert(status == 0);

  status = fdt_node_check_compatible(fdt, root, "blub");
  rtems_test_assert(status == 1);

  prop = fdt_getprop(fdt, root, "model", &len);
  rtems_test_assert(len == 2);
  rtems_test_assert(memcmp(prop, "c", 2) == 0);

  d = fdt_subnode_offset(fdt, root, "slurf");
  rtems_test_assert(d == -FDT_ERR_NOTFOUND);

  d = fdt_subnode_offset(fdt, root, "d");
  rtems_test_assert(d >= 0);

  prop = fdt_getprop(fdt, d, "e", &len);
  rtems_test_assert(len == 2);
  rtems_test_assert(memcmp(prop, "f", 2) == 0);

  prop = fdt_getprop(fdt, d, "g", &len);
  rtems_test_assert(len == 0);
  rtems_test_assert(prop != NULL);

  m = fdt_subnode_offset(fdt, root, "m@1248");
  rtems_test_assert(m >= 0);

  t = fdt_subnode_offset(fdt, root, "t");
  rtems_test_assert(t >= 0);

  prop_32 = (fdt32_t *) fdt_getprop(fdt, t, "u", &len);
  rtems_test_assert(len == 4);
  phandle = fdt32_to_cpu(*prop_32);

  node = fdt_node_offset_by_phandle(fdt, phandle);
  rtems_test_assert(node == m);

  prop_32 = (fdt32_t *) fdt_getprop(fdt, m, "n", &len);
  rtems_test_assert(len == 8);
  cell = fdt32_to_cpu(prop_32[0]);
  rtems_test_assert(cell == 0xdeadbeef);
  cell = fdt32_to_cpu(prop_32[1]);
  rtems_test_assert(cell == 0x12345678);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
