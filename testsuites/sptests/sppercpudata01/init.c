/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/percpudata.h>

#include <tmacros.h>

#include "sppercpudata01.h"

const char rtems_test_name[] = "SPPERCPUDATA 1";

static RTEMS_ALIGNED(CPU_HEAP_ALIGNMENT)
  PER_CPU_DATA_ITEM(unsigned char, c) = 1;

static PER_CPU_DATA_ITEM(unsigned char, cz);

static PER_CPU_DATA_ITEM(unsigned short, s) = 2;

static PER_CPU_DATA_ITEM(unsigned short, sz);

static PER_CPU_DATA_ITEM(unsigned int, i) = 3;

static PER_CPU_DATA_ITEM(unsigned int, iz);

static PER_CPU_DATA_ITEM(unsigned long, l) = 4;

static PER_CPU_DATA_ITEM(unsigned long, lz);

static PER_CPU_DATA_ITEM(unsigned int, a[3]) = { 5, 6, 7 };

static PER_CPU_DATA_ITEM(unsigned int, az[3]);

typedef struct t {
  unsigned int a;
  unsigned int b;
  unsigned int c;
} t;

static PER_CPU_DATA_ITEM(t, t) = { .a = 8, .b = 9, .c = 10 };

static void set_affinity(uint32_t cpu_index)
{
  rtems_status_code sc;
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET((int) cpu_index, &set);
  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(set), &set);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_initial_values(void)
{
  uint32_t cpu_index;

  for (
    cpu_index = 0;
    cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Per_CPU_Control *cpu;
    unsigned char *c;
    unsigned short *s;
    unsigned int *i;
    unsigned long *l;
    uintptr_t off;
    t *pt;

    set_affinity(cpu_index);
    cpu = _Per_CPU_Get_by_index(cpu_index);

    off = PER_CPU_DATA_OFFSET(c);
    rtems_test_assert(off % CPU_HEAP_ALIGNMENT == 0);
    c = PER_CPU_DATA_GET_BY_OFFSET(cpu, unsigned char, off);
    rtems_test_assert(*c == 1);

    c = PER_CPU_DATA_GET(cpu, unsigned char, c);
    rtems_test_assert(*c == 1);

    c = PER_CPU_DATA_GET(cpu, unsigned char, cz);
    rtems_test_assert(*c == 0);

    s = PER_CPU_DATA_GET(cpu, unsigned short, s);
    rtems_test_assert(*s == 2);

    s = PER_CPU_DATA_GET(cpu, unsigned short, sz);
    rtems_test_assert(*s == 0);

    i = PER_CPU_DATA_GET(cpu, unsigned int, i);
    rtems_test_assert(*i == 3);

    i = PER_CPU_DATA_GET(cpu, unsigned int, iz);
    rtems_test_assert(*i == 0);

    l = PER_CPU_DATA_GET(cpu, unsigned long, l);
    rtems_test_assert(*l == 4);

    l = PER_CPU_DATA_GET(cpu, unsigned long, lz);
    rtems_test_assert(*l == 0);

    i = PER_CPU_DATA_GET(cpu, unsigned int, a[0]);
    rtems_test_assert(i[0] == 5);
    rtems_test_assert(i[1] == 6);
    rtems_test_assert(i[2] == 7);

    i = PER_CPU_DATA_GET(cpu, unsigned int, az[0]);
    rtems_test_assert(i[0] == 0);
    rtems_test_assert(i[1] == 0);
    rtems_test_assert(i[2] == 0);

    pt = PER_CPU_DATA_GET(cpu, t, t);
    rtems_test_assert(pt->a == 8);
    rtems_test_assert(pt->b == 9);
    rtems_test_assert(pt->c == 10);

    pt = PER_CPU_DATA_GET(cpu, struct t, t);
    rtems_test_assert(pt->a == 8);
    rtems_test_assert(pt->b == 9);
    rtems_test_assert(pt->c == 10);

    i = PER_CPU_DATA_GET(cpu, unsigned int, g);
    rtems_test_assert(*i == 11);
  }
}

static void set_unique_values(unsigned int v)
{
  uint32_t cpu_index;

  for (
    cpu_index = 0;
    cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Per_CPU_Control *cpu;
    unsigned char *c;
    unsigned short *s;
    unsigned int *i;
    unsigned long *l;
    t *pt;

    set_affinity(cpu_index);
    cpu = _Per_CPU_Get_by_index(cpu_index);

    c = PER_CPU_DATA_GET(cpu, unsigned char, c);
    *c = (unsigned char) ++v;

    c = PER_CPU_DATA_GET(cpu, unsigned char, cz);
    *c = (unsigned char) ++v;

    s = PER_CPU_DATA_GET(cpu, unsigned short, s);
    *s = (unsigned short) ++v;

    s = PER_CPU_DATA_GET(cpu, unsigned short, sz);
    *s = (unsigned short) ++v;

    i = PER_CPU_DATA_GET(cpu, unsigned int, i);
    *i = ++v;

    i = PER_CPU_DATA_GET(cpu, unsigned int, iz);
    *i = ++v;

    l = PER_CPU_DATA_GET(cpu, unsigned long, l);
    *l = ++v;

    l = PER_CPU_DATA_GET(cpu, unsigned long, lz);
    *l = ++v;

    i = PER_CPU_DATA_GET(cpu, unsigned int, a[0]);
    i[0] = ++v;
    i[1] = ++v;
    i[2] = ++v;

    i = PER_CPU_DATA_GET(cpu, unsigned int, az[0]);
    i[0] = ++v;
    i[1] = ++v;
    i[2] = ++v;

    pt = PER_CPU_DATA_GET(cpu, t, t);
    pt->a = ++v;
    pt->b = ++v;
    pt->c = ++v;

    i = PER_CPU_DATA_GET(cpu, unsigned int, g);
    *i = ++v;
  }
}

static void test_unique_values(unsigned int v)
{
  uint32_t cpu_index;

  for (
    cpu_index = 0;
    cpu_index < rtems_scheduler_get_processor_maximum();
    ++cpu_index
  ) {
    Per_CPU_Control *cpu;
    unsigned char *c;
    unsigned short *s;
    unsigned int *i;
    unsigned long *l;
    uintptr_t off;
    t *pt;

    set_affinity(cpu_index);
    cpu = _Per_CPU_Get_by_index(cpu_index);

    c = PER_CPU_DATA_GET(cpu, unsigned char, c);
    ++v;
    rtems_test_assert(*c == (unsigned char) v);

    off = PER_CPU_DATA_OFFSET(c);
    rtems_test_assert(off % CPU_HEAP_ALIGNMENT == 0);
    c = PER_CPU_DATA_GET_BY_OFFSET(cpu, unsigned char, off);
    rtems_test_assert(*c == (unsigned char) v);

    c = PER_CPU_DATA_GET(cpu, unsigned char, cz);
    ++v;
    rtems_test_assert(*c == (unsigned char) v);

    s = PER_CPU_DATA_GET(cpu, unsigned short, s);
    ++v;
    rtems_test_assert(*s == (unsigned short) v);

    s = PER_CPU_DATA_GET(cpu, unsigned short, sz);
    ++v;
    rtems_test_assert(*s == (unsigned short) v);

    i = PER_CPU_DATA_GET(cpu, unsigned int, i);
    ++v;
    rtems_test_assert(*i == v);

    i = PER_CPU_DATA_GET(cpu, unsigned int, iz);
    ++v;
    rtems_test_assert(*i == v);

    l = PER_CPU_DATA_GET(cpu, unsigned long, l);
    ++v;
    rtems_test_assert(*l == v);

    l = PER_CPU_DATA_GET(cpu, unsigned long, lz);
    ++v;
    rtems_test_assert(*l == v);

    i = PER_CPU_DATA_GET(cpu, unsigned int, a[0]);
    ++v;
    rtems_test_assert(i[0] == v);
    ++v;
    rtems_test_assert(i[1] == v);
    ++v;
    rtems_test_assert(i[2] == v);

    i = PER_CPU_DATA_GET(cpu, unsigned int, az[0]);
    ++v;
    rtems_test_assert(i[0] == v);
    ++v;
    rtems_test_assert(i[1] == v);
    ++v;
    rtems_test_assert(i[2] == v);

    pt = PER_CPU_DATA_GET(cpu, t, t);
    ++v;
    rtems_test_assert(pt->a == v);
    ++v;
    rtems_test_assert(pt->b == v);
    ++v;
    rtems_test_assert(pt->c == v);

    i = PER_CPU_DATA_GET(cpu, unsigned int, g);
    ++v;
    rtems_test_assert(*i == v);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  test_initial_values();
  set_unique_values(12);
  test_unique_values(12);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS 32

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
