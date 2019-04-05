/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 * This OpenMP micro benchmark is based on mailing list posts from Jakub
 * Jelinek.
 *
 * Subject: [gomp3] libgomp performance improvements
 * https://gcc.gnu.org/ml/gcc-patches/2008-03/msg00930.html
 *
 * Subject: [gomp3] Use private futexes if possible
 * https://gcc.gnu.org/ml/gcc-patches/2008-03/msg01126.html
 *
 * This file can be compiled on Linux, etc. using:
 *
 * cc -std=c11 -O2 -fopenmp init.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <omp.h>
#include <stdio.h>

#ifdef __rtems__
#include <pthread.h>
#include <tmacros.h>

const char rtems_test_name[] = "SMPOPENMP 1";

#define CPU_COUNT_MAX 32
#endif /* __rtems__ */

static void work(void)
{
  __asm__ volatile ("" : : : "memory");
}

static void barrier_bench(void)
{
  #pragma omp parallel
  for (int i = 0; i < 10000; ++i) {
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
    #pragma omp barrier
    work();
  }
}

static void parallel_bench(void)
{
  for (int i = 0; i < 20000; ++i) {
    #pragma omp parallel
    work();
  }
}

static void static_bench(void)
{
  for (int i = 0; i < 1000; ++i) {
    #pragma omp parallel for schedule (static)
    for (int j = 0; j < 100; ++j) {
      work();
    }
  }
}

static void dynamic_bench(void)
{
  #pragma omp parallel for schedule (dynamic)
  for (int i = 0; i < 100000; ++i) {
    work();
  }
}

static void guided_bench(void)
{
  #pragma omp parallel for schedule (guided)
  for (int i = 0; i < 100000; ++i) {
    work();
  }
}

static void runtime_bench(void)
{
  #pragma omp parallel for schedule (runtime)
  for (int i = 0; i < 100000; ++i) {
    work();
  }
}

static void single_bench(void)
{
  #pragma omp parallel
  for (int i = 0; i < 10000; ++i) {
    #pragma omp single
    work();
  }
}

static void all(void)
{
  barrier_bench();
  parallel_bench();
  static_bench();
  dynamic_bench();
  guided_bench();
  runtime_bench();
  single_bench();
}

static void do_bench(const char *name, void (*bench)(void), int n)
{
  double start;
  double delta;

  (*bench)();
  start = omp_get_wtime();
  for (int i = 0; i < n; ++i) {
    (*bench)();
  }
  delta = omp_get_wtime() - start;
  printf("\t\t<%sBench unit=\"s\">%f</%sBench>\n", name, delta, name);
}

static void microbench(int num_threads, int n)
{
  printf("\t<Microbench numThreads=\"%i\" majorLoopCount=\"%i\">\n", num_threads, n);
  omp_set_num_threads(num_threads);
  do_bench("Barrier", barrier_bench, n);
  do_bench("Parallel", parallel_bench, n);
  do_bench("Static", static_bench, n);
  do_bench("Dynamic", dynamic_bench, n);
  do_bench("Guided", guided_bench, n);
  do_bench("Runtime", runtime_bench, n);
  do_bench("Single", single_bench, n);
  printf("\t</Microbench>\n");
}

static int estimate_3s_runtime_with_one_proc(void)
{
  double start;
  double delta;
  int n;

  omp_set_num_threads(1);
  all();
  start = omp_get_wtime();
  all();
  delta = omp_get_wtime() - start;

  if (delta > 0.0 && delta <= 1.0) {
    n = (int) (3.0 / delta);
  } else {
    n = 1;
  }

  return n;
}

static void test(void)
{
  int num_procs;
  int n;

  printf("<SMPOpenMP01>\n");

  n = estimate_3s_runtime_with_one_proc();
  num_procs = omp_get_num_procs();
  omp_set_num_threads(num_procs);

  for (int i = 1; i <= num_procs; ++i) {
    microbench(i, n);
  }

  printf("</SMPOpenMP01>\n");
}

#ifdef __rtems__

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  cpu_set_t cpu_set;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  TEST_BEGIN();

  CPU_ZERO(&cpu_set);
  CPU_SET(0, &cpu_set);

  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpu_set), &cpu_set);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test();
  TEST_END();
  rtems_test_exit(0);
}

typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t cnd;
  bool cpus_used[CPU_COUNT_MAX];
} test_context;

static test_context test_instance;

static uint32_t find_free_cpu(test_context *ctx)
{
  uint32_t i;
  uint32_t n;

  n = rtems_scheduler_get_processor_maximum();

  pthread_mutex_lock(&ctx->mtx);

  do {
    for (i = 1; i < n; ++i) {
      if (!ctx->cpus_used[i]) {
        ctx->cpus_used[i] = true;
        break;
      }
    }

    if (i == n) {
      pthread_cond_wait(&ctx->cnd, &ctx->mtx);
    }
  } while (i == n);

  pthread_mutex_unlock(&ctx->mtx);

  return i;
}

static void begin_extension(Thread_Control *th)
{
  rtems_id th_id;

  th_id = th->Object.id;

  if (rtems_object_id_get_api(th_id) == OBJECTS_POSIX_API) {
    rtems_status_code sc;
    rtems_id sched_id;
    uint32_t cpu_index;
    cpu_set_t cpu_set;
    rtems_task_priority prio;

    cpu_index = find_free_cpu(&test_instance);

    sc = rtems_scheduler_ident_by_processor(cpu_index, &sched_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_priority(th_id, RTEMS_CURRENT_PRIORITY, &prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(th_id, sched_id, prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    CPU_ZERO(&cpu_set);
    CPU_SET((int) cpu_index, &cpu_set);

    sc = rtems_task_set_affinity(th_id, sizeof(cpu_set), &cpu_set);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void terminate_extension(Thread_Control *th)
{
  rtems_id th_id;

  th_id = th->Object.id;

  if (rtems_object_id_get_api(th_id) == OBJECTS_POSIX_API) {
    rtems_status_code sc;
    cpu_set_t cpu_set;
    uint32_t cpu_index;
    test_context *ctx;

    sc = rtems_task_get_affinity(th_id, sizeof(cpu_set), &cpu_set);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    cpu_index = CPU_FFS(&cpu_set) - 1;

    ctx = &test_instance;

    pthread_mutex_lock(&ctx->mtx);
    rtems_test_assert(ctx->cpus_used[cpu_index]);
    ctx->cpus_used[cpu_index] = false;
    pthread_cond_broadcast(&ctx->cnd);
    pthread_mutex_unlock(&ctx->mtx);
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT_MAX

#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    .thread_begin = begin_extension, \
    .thread_terminate = terminate_extension \
  }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#else /* __rtems__ */

int main(void)
{
  test();
  return 0;
}

#endif /* __rtems__ */
