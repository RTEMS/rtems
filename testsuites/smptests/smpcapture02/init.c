/*
 *  COPYRIGHT (c) 2015
 *  Cobham Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * SMP Capture Test 2
 *
 * This program tests the functionality to add custom entries to
 * the SMP capture trace.
 *
 */

/* #define VERBOSE 1 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <bsp/irq.h>
#include <rtems/captureimpl.h>
#include "tmacros.h"

const char rtems_test_name[] = "SMPCAPTURE 2";

#define MAX_CPUS       4
#define TASKS_PER_CPU  5
#define ITERATIONS     3
#define TASK_PRIO      30
#define CLOCK_TICKS    100

typedef struct  {
  rtems_id id;
  rtems_id task_sem;
  rtems_id prev_sem;
} per_cpu_task_data;

typedef struct {
  bool found;
  const char *info;
  rtems_option options;
  rtems_interrupt_handler handler;
  void *arg;
} clock_interrupt_handler;

static rtems_id finished_sem;
static per_cpu_task_data task_data[ TASKS_PER_CPU * TASKS_PER_CPU ];
static rtems_interrupt_handler org_clock_handler;

typedef enum {
  enter_add_number,
  exit_add_number,
  clock_tick
} cap_rec_type;

/*
 * These records define the data stored in the capture trace.
 * The records must be packed so alignment issues are not a factor.
 */
typedef struct {
  uint32_t a;
  uint32_t b;
} RTEMS_PACKED enter_add_number_record;

typedef struct {
  uint32_t res;
} RTEMS_PACKED exit_add_number_record;

typedef struct {
  void *arg;
} RTEMS_PACKED clock_tick_record;

/*
 * Create a printable set of char's from a name.
 */
#define PNAME(n) \
  (char) (n >> 24) & 0xff, (char) (n >> 16) & 0xff, \
  (char) (n >> 8) & 0xff, (char) (n >> 0) & 0xff

/*
 * The function that we want to trace
 */
static uint32_t add_number(uint32_t a, uint32_t b)
{
  return a + b;
}

/*
 * The wrapper for the function we want to trace. Records
 * input arguments and the result to the capture trace.
 */
static uint32_t add_number_wrapper(uint32_t a, uint32_t b)
{
  rtems_capture_record_lock_context lock;
  enter_add_number_record enter_rec;
  exit_add_number_record exit_rec;
  cap_rec_type id;
  uint32_t res;
  void* rec;

  id = enter_add_number;
  enter_rec.a = a;
  enter_rec.b = b;

  rec = rtems_capture_record_open(_Thread_Get_executing(),
                                  RTEMS_CAPTURE_TIMESTAMP,
                                  sizeof(id) + sizeof(enter_rec),
                                  &lock);
  rtems_test_assert(rec != NULL);
  rec = rtems_capture_record_append(rec, &id, sizeof(id));
  rtems_test_assert(rec != NULL);
  rec = rtems_capture_record_append(rec, &enter_rec, sizeof(enter_rec));
  rtems_test_assert(rec != NULL);
  rtems_capture_record_close(&lock);

  res = add_number(a, b);

  id = exit_add_number;
  exit_rec.res = res;

  rec = rtems_capture_record_open(_Thread_Get_executing(),
                                  RTEMS_CAPTURE_TIMESTAMP,
                                  sizeof(id) + sizeof(exit_rec),
                                  &lock);
  rtems_test_assert(rec != NULL);
  rec = rtems_capture_record_append(rec, &id, sizeof(id));
  rtems_test_assert(rec != NULL);
  rec = rtems_capture_record_append(rec, &exit_rec, sizeof(exit_rec));
  rtems_test_assert(rec != NULL);
  rtems_capture_record_close(&lock);

  return res;
}

/*
 * Task that calls the function we want to trace
 */
static void task(rtems_task_argument arg)
{
  rtems_status_code sc;
  uint32_t i;

  for ( i = 0; i < ITERATIONS; i++ ) {
    /*
     * Wait until the previous task in the task chain
     * has completed its operation.
     */
    sc = rtems_semaphore_obtain(task_data[arg].prev_sem, 0, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    add_number_wrapper(arg, i);

    /*
     * Signal the next task in the chain to continue
     */
    sc = rtems_semaphore_release(task_data[arg].task_sem);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* Signal the main task that this task has finished */
  sc = rtems_semaphore_release(finished_sem);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_task_suspend(rtems_task_self());
}

static void test(uint32_t cpu_count)
{
  rtems_status_code sc;
  uint32_t t;
  uint32_t c;
  rtems_task_argument idx;
  cpu_set_t cpu_set;

  /* Semaphore to signal end of test */
  sc = rtems_semaphore_create(rtems_build_name('D', 'o', 'n', 'e'), 0,
      RTEMS_LOCAL |
      RTEMS_NO_INHERIT_PRIORITY |
      RTEMS_NO_PRIORITY_CEILING |
      RTEMS_FIFO, 0, &finished_sem);

  /*
   * Create a set of tasks per CPU. Chain them together using
   * semaphores so that only one task can be active at any given
   * time.
   */
  for ( c = 0; c < cpu_count; c++ ) {
    for ( t = 0; t < TASKS_PER_CPU; t++ ) {
      idx = c * TASKS_PER_CPU + t;

      sc = rtems_task_create(rtems_build_name('T', 'A', '0' + c, '0' + t),
          TASK_PRIO,
          RTEMS_MINIMUM_STACK_SIZE,
          RTEMS_DEFAULT_MODES,
          RTEMS_DEFAULT_ATTRIBUTES,
          &task_data[idx].id);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_semaphore_create(rtems_build_name('S', 'E', '0' + c, '0' + t),
          0,
          RTEMS_LOCAL |
          RTEMS_SIMPLE_BINARY_SEMAPHORE |
          RTEMS_NO_INHERIT_PRIORITY |
          RTEMS_NO_PRIORITY_CEILING |
          RTEMS_FIFO, 0, &task_data[idx].task_sem);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      task_data[(idx + 1) % (cpu_count * TASKS_PER_CPU)].prev_sem =
          task_data[idx].task_sem;

      CPU_ZERO_S(sizeof(cpu_set_t), &cpu_set);
      CPU_SET_S(c, sizeof(cpu_set_t), &cpu_set);

      sc = rtems_task_set_affinity(task_data[idx].id, sizeof(cpu_set_t),
          &cpu_set);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  /* Start the tasks */
  for ( idx = 0; idx < cpu_count * TASKS_PER_CPU; idx++ ) {
    sc = rtems_task_start(task_data[idx].id, task, idx);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* Start chain */
  sc = rtems_semaphore_release(task_data[0].task_sem);

  /* Wait until chain has completed */
  for ( idx = 0; idx < cpu_count * TASKS_PER_CPU; idx++ ) {
    rtems_semaphore_obtain(finished_sem, 0, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

}

/* Writes an entry in the capture trace for every clock tick */
static void clock_tick_wrapper(void *arg)
{
  rtems_capture_record_lock_context lock;
  cap_rec_type id  = clock_tick;
  Thread_Control* tcb = _Thread_Get_executing();
  void* rec;

  rec = rtems_capture_record_open(tcb,
                                  RTEMS_CAPTURE_TIMESTAMP,
                                  sizeof(id),
                                  &lock);
  rtems_test_assert(rec != NULL);
  rec = rtems_capture_record_append(rec, &id, sizeof(id));
  rtems_test_assert(rec != NULL);
  rtems_capture_record_close(&lock);

  org_clock_handler(arg);
}

/* Tries to locate the clock interrupt handler by looking
 * for a handler with the name "Clock" or "clock" */
static void locate_clock_interrupt_handler(
    void *arg, const char *info, rtems_option options,
    rtems_interrupt_handler handler, void *handler_arg)
{
  clock_interrupt_handler *cih = (clock_interrupt_handler*)arg;
  if ( !strcmp(info, "clock") || !strcmp(info, "Clock") ) {
    cih->info = info;
    cih->options = options;
    cih->handler = handler;
    cih->arg = handler_arg;
    cih->found = true;
  }
}

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  uint32_t i;
  uint32_t cpu;
  uint32_t cpu_count;
  uint32_t enter_count;
  uint32_t exit_count;
  uint32_t clock_tick_count;
  uint32_t res_should_be;
  rtems_vector_number vec;
  size_t read;
  const void *recs;
  cap_rec_type id;
  rtems_capture_record rec;
  rtems_capture_record prev_rec;
  enter_add_number_record enter_rec;
  exit_add_number_record exit_rec;
  clock_interrupt_handler cih = {.found = 0};

#ifdef VERBOSE
  rtems_name name;
#endif

  TEST_BEGIN();

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();

  sc = rtems_capture_open(50000, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_watch_global(true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_set_control(true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Run main test */
  test(cpu_count);

  /* Try to find the clock interrupt handler */
  for ( vec=BSP_INTERRUPT_VECTOR_MIN; vec<BSP_INTERRUPT_VECTOR_MAX; vec++ ) {
    rtems_interrupt_handler_iterate(vec, locate_clock_interrupt_handler, &cih);
    if ( cih.found )
      break;
  }

  /* If we find the clock interrupt handler we replace it with
   * a wrapper and wait for a fixed number of ticks.
   */
  if ( cih.found ) {
#ifdef VERBOSE
    printf("Found a clock handler\n");
#endif
    org_clock_handler = cih.handler;
    rtems_interrupt_handler_install(vec, cih.info,
        cih.options | RTEMS_INTERRUPT_REPLACE, clock_tick_wrapper, cih.arg);

    rtems_task_wake_after(CLOCK_TICKS);
  }

  /* Disable capturing */
  sc = rtems_capture_set_control(false);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  clock_tick_count = 0;

  /* Read out the trace from all processors */
  for ( cpu = 0; cpu < cpu_count; cpu++ ) {
    sc = rtems_capture_read(cpu, &read, &recs);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(recs != NULL);

    memset(&rec, 0, sizeof(rec));
    prev_rec = rec;
    enter_count = 0;
    exit_count = 0;
    res_should_be = 0;

    for ( i = 0; i < read; i++ ) {

      recs = rtems_capture_record_extract(recs, &rec, sizeof(rec));
      rtems_test_assert(recs != NULL);

      /* Verify that time goes forward */
      rtems_test_assert(rec.time >= prev_rec.time);

      if ((rec.events & RTEMS_CAPTURE_TIMESTAMP) != 0) {
        recs = rtems_capture_record_extract(recs, &id, sizeof(id));
        rtems_test_assert(recs != NULL);

        switch (id) {
        case enter_add_number:
          rtems_test_assert(enter_count == exit_count);
          enter_count++;
          recs = rtems_capture_record_extract(recs,
                                              &enter_rec,
                                              sizeof(enter_rec));
          rtems_test_assert(recs != NULL);
          res_should_be = add_number(enter_rec.a, enter_rec.b);
#ifdef VERBOSE
          /* Print record */
          rtems_object_get_classic_name(rec.task_id, &name);
          printf("Time: %"PRIu64"us Task: %c%c%c%c => Add %"PRIu32" and %"PRIu32" is %"PRIu32"\n",
                 rec.time / 1000, PNAME(name), enter_rec.a, enter_rec.b, res_should_be);
#endif
          break;
        case exit_add_number:
          rtems_test_assert(enter_count == exit_count+1);
          exit_count++;
          recs = rtems_capture_record_extract(recs,
                                              &exit_rec,
                                              sizeof(exit_rec));
          rtems_test_assert(recs != NULL);
#ifdef VERBOSE
          /* Print record */
          rtems_object_get_classic_name(rec.task_id, &name);
          printf("Time: %"PRIu64"us Task: %c%c%c%c => Result is %"PRIu32"\n",
                 rec.time / 1000, PNAME(name), exit_rec.res);
#endif
          /* Verify that the result matches the expected result */
          rtems_test_assert(res_should_be == exit_rec.res);
          break;
        case clock_tick:
          clock_tick_count++;
#ifdef VERBOSE
          rtems_object_get_classic_name(rec.task_id, &name);
          printf("Time: %"PRIu64"us Task: %c%c%c%c => Clock tick\n",
                 rec.time/1000, PNAME(name));
#endif
          break;
        default:
          rtems_test_assert(0);
        }
      }

      prev_rec = rec;
    }

    rtems_test_assert(enter_count == exit_count);
    rtems_test_assert(enter_count == TASKS_PER_CPU * ITERATIONS);

    rtems_capture_release(cpu, read);
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_PROCESSORS MAX_CPUS
#define CONFIGURE_MAXIMUM_PROCESSORS MAX_CPUS
#define CONFIGURE_MAXIMUM_SEMAPHORES MAX_CPUS * TASKS_PER_CPU + 1
#define CONFIGURE_MAXIMUM_TASKS MAX_CPUS * TASKS_PER_CPU + 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION
#define CONFIGURE_INIT

#include <rtems/confdefs.h>
