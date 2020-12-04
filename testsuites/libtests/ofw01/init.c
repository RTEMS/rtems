/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) <2020> Niteesh G S <niteesh.gs@gmail.com>
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

#include <tmacros.h>
#include <stdio.h>
#include <stdlib.h>
#include <libfdt.h>
#include <sys/endian.h>
#include <bsp/fdt.h>
#include <ofw/ofw.h>
#include <ofw/ofw_test.h>

#include "some.h"

#define BUF_SIZE 100

const char rtems_test_name[] = "OFW 01";
static const void *test_bin = NULL;

const void *__wrap_bsp_fdt_get(void);
const void *__real_bsp_fdt_get(void);

const void *__wrap_bsp_fdt_get(void)
{
  if (test_bin != NULL) {
    return test_bin;
  }

#ifdef BSP_FDT_IS_SUPPORTED
  return __real_bsp_fdt_get();
#else
  return some_bin;
#endif
}

static void Init(rtems_task_argument arg)
{
  int rv;
  phandle_t d;
  phandle_t l;
  phandle_t t;
  phandle_t c;
  phandle_t a;
  phandle_t b;
  phandle_t q;
  phandle_t root;
  phandle_t temp;
  uint32_t *arr;
  char buf[BUF_SIZE];
  char *bufp;
  ssize_t buf_len;
  rtems_ofw_memory_area reg;
  rtems_ofw_memory_area regs[2];
  rtems_vector_number intr;
  rtems_vector_number intrs[2];
  TEST_BEGIN();
  buf_len = sizeof(buf);

  /*
   * Reinitializing the OFW API to use the test
   * FDT instead of the original FDT.
   */
  test_bin = some_bin;
  rtems_ofw_init();

  /*
   * Cannot use fdt_path_offset to compare because
   * the OF interface uses the offset from the ftdp
   * to the node as phandle.
   */
  root = rtems_ofw_find_device("/");
  rtems_test_assert(root == 56);

  root = rtems_ofw_peer(0);
  rtems_test_assert(root == 56);

  d = rtems_ofw_child(root);
  temp = rtems_ofw_find_device("/d");
  rtems_test_assert(d == temp);

  temp = rtems_ofw_parent(d);
  rtems_test_assert(root == temp);

  rv = rtems_ofw_get_prop(d, "e", buf, buf_len);
  rtems_test_assert(rv != -1);
  rtems_test_assert(strcmp(buf, "f") == 0);

  rv = rtems_ofw_has_prop(d, "g");
  rtems_test_assert(rv == 1);

  rv = rtems_ofw_get_prop_len(root, "model");
  rtems_test_assert(rv == 2);

  rv = rtems_ofw_next_prop(d, "e", buf, buf_len);
  rtems_test_assert(rv == 1);
  rtems_test_assert(strcmp(buf, "g") == 0);

  l = rtems_ofw_find_device("/m@1248");
  rv = rtems_ofw_search_prop(l, "model", buf, buf_len);
  rtems_test_assert(rv != -1);
  rtems_test_assert(strcmp(buf, "c") == 0);

  rv = rtems_ofw_get_prop_alloc(root, "compatible", (void **)&bufp);
  rtems_test_assert(rv != -1);
  rtems_test_assert(strcmp(bufp, "a,b") == 0);

  rtems_ofw_free(bufp);
  rv = rtems_ofw_get_prop_alloc_multi(l, "n", sizeof(*arr), (void **)&arr);
  rtems_test_assert(rv == 2);
  rtems_test_assert(arr[0] == htobe32(0xdeadbeef));
  rtems_test_assert(arr[1] == htobe32(0x12345678));

  rv = rtems_ofw_get_enc_prop_alloc_multi(l, "n", sizeof(*arr), (void **)&arr);
  rtems_test_assert(rv == 2);
  rtems_test_assert(arr[0] == 0xdeadbeef);
  rtems_test_assert(arr[1] == 0x12345678);

  t = rtems_ofw_find_device("/t");
  rv = rtems_ofw_next_prop(t, "u", buf, buf_len);
  rtems_test_assert(rv == 0);

  rv = rtems_ofw_next_prop(d, "e", buf, buf_len);
  rtems_test_assert(rv == 1);

  a = rtems_ofw_find_device("/a");
  rv = rtems_ofw_get_reg(a, &reg, sizeof(reg));
  rtems_test_assert(rv == 1);
  rtems_test_assert(reg.start == 0x1234);
  rtems_test_assert(reg.size == 0x10);

  b = rtems_ofw_find_device("/a/b");
  rv = rtems_ofw_get_reg(b, &regs[0], sizeof(regs));
  rtems_test_assert(rv == 2);
  rtems_test_assert(regs[0].start == 0x8234);
  rtems_test_assert(regs[0].size == 0x10);
  rtems_test_assert(regs[1].start == 0xf468);
  rtems_test_assert(regs[1].size == 0x10);

  c = rtems_ofw_find_device("/c");
  rv = rtems_ofw_get_reg(c, &reg, sizeof(reg));
  rtems_test_assert(rv == -1);

  a = rtems_ofw_find_device("/a");
  rv = rtems_ofw_get_interrupts(a, &intr, sizeof(intr));
  rtems_test_assert(rv == 1);
  rtems_test_assert(intr == 0x1);

  c = rtems_ofw_find_device("/c");
  rv = rtems_ofw_get_interrupts(c, &intrs[0], sizeof(intrs));
  rtems_test_assert(rv == 2);
  rtems_test_assert(intrs[0] == 0x1);
  rtems_test_assert(intrs[1] == 0x2);

  q = rtems_ofw_find_device("/c/q");
  rv = rtems_ofw_node_status(q);
  rtems_test_assert(rv == true);

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
