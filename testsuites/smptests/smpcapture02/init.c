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
} task_data_t;

typedef struct {
  bool found;
  const char *info;
  rtems_option options;
  rtems_interrupt_handler handler;
  void *arg;
} clock_interrupt_handler;

static rtems_id finished_sem;
static task_data_t task_data[ TASKS_PER_CPU * TASKS_PER_CPU ];
static rtems_interrupt_handler org_clock_handler;

enum cap_rec_types {
  enter_add_number,
  exit_add_number,
  clock_tick
};

/*
 * These records define the data stored in the capture trace.
 * The records must be 64-bit aligned to make sure that the time
 * attribute in rtems_capture_record_t is correctly aligned.
 */
typedef struct {
  enum cap_rec_types id;
  uint32_t a;
  uint32_t b;
} __attribute__ ((aligned (8))) enter_add_number_record_t;

typedef struct {
  enum cap_rec_types id;
  uint32_t res;
} __attribute__ ((aligned (8))) exit_add_number_record_t;

typedef struct {
  enum cap_rec_types id;
  void *arg;
} __attribute__ ((aligned (8))) clock_tick_record_t;

typedef struct {
  enum cap_rec_types id;
} empty_record_t ;

/*
 * The function that we want to trace
 */
static uint32_t add_number(uint32_t a, uint32_t b)
{
  return a+b;
}

/*
 * The wrapper for the function we want to trace. Records
 * input arguments and the result to the capture trace.
 */
static uint32_t add_number_wrapper(uint32_t a, uint32_t b)
{
  enter_add_number_record_t enter_rec;
  exit_add_number_record_t exit_rec;
  uint32_t res;
  void* rec;

  enter_rec.id = enter_add_number;
  enter_rec.a = a;
  enter_rec.b = b;

  rtems_capture_begin_add_record(_Thread_Get_executing(),
      RTEMS_CAPTURE_TIMESTAMP, sizeof(rtems_capture_record_t)+
      sizeof(enter_add_number_record_t), &rec);
  rec = rtems_capture_append_to_record(rec, &enter_rec, sizeof(enter_rec));
  rtems_capture_end_add_record(rec);

  res = add_number(a, b);

  exit_rec.id = exit_add_number;
  exit_rec.res = res;

  rtems_capture_begin_add_record(_Thread_Get_executing(),
      RTEMS_CAPTURE_TIMESTAMP, sizeof(rtems_capture_record_t)+
      sizeof(exit_add_number_record_t), &rec);
  rec = rtems_capture_append_to_record(rec, &exit_rec, sizeof(exit_rec));
  rtems_capture_end_add_record(rec);

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
  void* rec;
  clock_tick_record_t clock_tick_record = {.id = clock_tick};
  Thread_Control* tcb = _Thread_Get_executing();

  rtems_capture_begin_add_record(tcb, RTEMS_CAPTURE_TIMESTAMP,
      sizeof(rtems_capture_record_t) + sizeof(clock_tick_record_t), &rec);
  rec = rtems_capture_append_to_record(rec, &clock_tick_record,
      sizeof(clock_tick_record));
  rtems_capture_end_add_record(rec);

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
  uint32_t read;
  uint32_t enter_count;
  uint32_t exit_count;
  uint32_t clock_tick_count;
  uint32_t res_should_be;
  rtems_name name;
  rtems_capture_record_t *recs;
  rtems_capture_record_t *prev_rec;
  empty_record_t *record;
  enter_add_number_record_t *enter_add_number_rec;
  exit_add_number_record_t *exit_add_number_rec;
  rtems_vector_number vec;
  clock_interrupt_handler cih = {.found = 0};

  TEST_BEGIN();

  /* Get the number of processors that we are using. */
  cpu_count = rtems_get_processor_count();

  sc = rtems_capture_open(50000, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_watch_global(true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_control(true);
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
  sc = rtems_capture_control(false);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  clock_tick_count = 0;

  /* Read out the trace from all processors */
  for ( cpu = 0; cpu < cpu_count; cpu++ ) {
    sc = rtems_capture_read(cpu, &read, &recs);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    prev_rec = recs;
    enter_count = 0;
    exit_count = 0;
    res_should_be = 0;

    for ( i = 0; i < read; i++ ) {

      /* Verify that time goes forward */
      rtems_test_assert(recs->time>=prev_rec->time);

      if ( recs->events & RTEMS_CAPTURE_TIMESTAMP ) {
        record = (empty_record_t*)((char*) recs +
            sizeof(rtems_capture_record_t));

        switch ( record->id ) {
        case enter_add_number:
          rtems_test_assert(enter_count==exit_count);
          enter_count++;
          enter_add_number_rec = (enter_add_number_record_t*)record;
          res_should_be = add_number(enter_add_number_rec->a,
              enter_add_number_rec->b);
          rtems_object_get_classic_name(recs->task_id, &name);

#ifdef VERBOSE
          /* Print record */
          printf("Time: %"PRIu64"us Task: %4s => Add %"PRIu32" and"
              " %"PRIu32"\n",
              recs->time/1000,
              (char*)&name,
              enter_add_number_rec->a,
              enter_add_number_rec->b);
#endif
          break;
        case exit_add_number:
          rtems_test_assert(enter_count==exit_count+1);
          exit_count++;
          exit_add_number_rec = (exit_add_number_record_t*)record;
          /* Verify that the result matches the expected result */
          rtems_test_assert(res_should_be == exit_add_number_rec->res);

#ifdef VERBOSE
          /* Print record */
          rtems_object_get_classic_name(recs->task_id, &name);
          printf("Time: %"PRIu64"us Task: %4s => Result is %"PRIu32"\n",
              recs->time/1000,
              (char*)&name,
              exit_add_number_rec->res);
#endif
          break;
        case clock_tick:
          clock_tick_count++;
#ifdef VERBOSE
          rtems_object_get_classic_name(recs->task_id, &name);
          printf("Time: %"PRIu64"us Task: %4s => Clock tick\n",
              recs->time/1000,
              (char*)&name);
#endif
          break;
        default:
          rtems_test_assert(0);
        }
      }

      prev_rec = recs;
      recs = (rtems_capture_record_t*) ((char*) recs + recs->size);
    }

    rtems_test_assert(enter_count == exit_count);
    rtems_test_assert(enter_count == TASKS_PER_CPU * ITERATIONS);

    rtems_capture_release(cpu, read);
  }

  if( cih.found )
    rtems_test_assert(clock_tick_count == cpu_count * CLOCK_TICKS);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION
#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS MAX_CPUS
#define CONFIGURE_MAXIMUM_PROCESSORS MAX_CPUS
#define CONFIGURE_MAXIMUM_SEMAPHORES MAX_CPUS * TASKS_PER_CPU + 1
#define CONFIGURE_MAXIMUM_TASKS MAX_CPUS * TASKS_PER_CPU + 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION
#define CONFIGURE_INIT

#include <rtems/confdefs.h>
