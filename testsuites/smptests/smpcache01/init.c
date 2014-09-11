/*
 * Copyright (c) 2014 Aeroflex Gaisler AB.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/atomic.h>
#include <rtems/score/smpbarrier.h>
#include <rtems.h>
#include <limits.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPCACHE 1";

CPU_STRUCTURE_ALIGNMENT static int data_to_flush[1024];

#define CPU_COUNT 32

#define WORKER_PRIORITY 100

typedef void (*Cache_manager_Function_ptr)(const void *d_addr, size_t n_bytes);

void
_Cache_manager_Send_smp_msg(
    const size_t setsize,
    const cpu_set_t *set,
    Cache_manager_Function_ptr func,
    const void * addr,
    size_t size
  );

typedef struct {
  SMP_barrier_Control barrier;
  uint32_t count[CPU_COUNT];
} test_context;

static test_context ctx = {
  .barrier = SMP_BARRIER_CONTROL_INITIALIZER,
};

static void function_to_flush( void )
{
  /* Does nothing. Used to give a pointer to instruction address space. */
}

static void test_cache_message( const void *d_addr, size_t n_bytes )
{
  rtems_test_assert(n_bytes == 123);
  rtems_test_assert(d_addr == 0);

  ctx.count[rtems_get_current_processor()]++;
}

static void cache_manager_smp_functions( size_t set_size,
    cpu_set_t *cpu_set )
{
  rtems_cache_flush_multiple_data_lines_processor_set( &data_to_flush,
      sizeof(data_to_flush), set_size, cpu_set );
  rtems_cache_invalidate_multiple_data_lines_processor_set( &data_to_flush,
      sizeof(data_to_flush), set_size, cpu_set );
  rtems_cache_flush_entire_data_processor_set( set_size, cpu_set );
  rtems_cache_invalidate_entire_instruction();
  rtems_cache_invalidate_multiple_instruction_lines( &function_to_flush,
      4 /* arbitrary size */ );
}

static void standard_funcs_test( size_t set_size, cpu_set_t *cpu_set )
{
  cache_manager_smp_functions( set_size, cpu_set );
}

static void standard_funcs_isrdisabled_test( size_t set_size,
    cpu_set_t *cpu_set, SMP_barrier_State *bs  )
{
  ISR_Level isr_level;

  _ISR_Disable_without_giant( isr_level );

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  cache_manager_smp_functions( set_size, cpu_set );

  _ISR_Enable_without_giant( isr_level );
}

static void standard_funcs_giant_taken_test( size_t set_size,
    cpu_set_t *cpu_set, SMP_barrier_State *bs )
{
  if ( rtems_get_current_processor() == 0)
    _Giant_Acquire();

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  cache_manager_smp_functions( set_size, cpu_set );

  if ( rtems_get_current_processor() == 0)
    _Giant_Release();
}

static void test_func_test( size_t set_size, cpu_set_t *cpu_set,
    SMP_barrier_State *bs )
{
  ctx.count[rtems_get_current_processor()] = 0;
  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  _Cache_manager_Send_smp_msg( set_size, cpu_set, test_cache_message, 0, 123 );

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  rtems_test_assert( ctx.count[rtems_get_current_processor()] ==
      rtems_get_processor_count() );
}

static void test_func_isrdisabled_test( size_t set_size, cpu_set_t *cpu_set,
    SMP_barrier_State *bs )
{
  ISR_Level isr_level;

  ctx.count[rtems_get_current_processor()] = 0;
  _ISR_Disable_without_giant( isr_level );

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  _Cache_manager_Send_smp_msg( set_size, cpu_set, test_cache_message, 0, 123 );

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  rtems_test_assert( ctx.count[rtems_get_current_processor()] ==
      rtems_get_processor_count() );

  _ISR_Enable_without_giant( isr_level );
}

static void test_func_giant_taken_test( size_t set_size, cpu_set_t *cpu_set,
    SMP_barrier_State *bs )
{
  ctx.count[rtems_get_current_processor()] = 0;

  if ( rtems_get_current_processor() == 0)
    _Giant_Acquire();

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  _Cache_manager_Send_smp_msg( set_size, cpu_set, test_cache_message, 0, 123 );

  _SMP_barrier_Wait( &ctx.barrier, bs, rtems_get_processor_count() );

  rtems_test_assert( ctx.count[rtems_get_current_processor()] ==
      rtems_get_processor_count() );

  if ( rtems_get_current_processor() == 0)
    _Giant_Release();
}

static void cmlog(  const char* str )
{
  if ( rtems_get_current_processor() == 0 )
    printf( "%s", str );
}

static void all_tests( void )
{
  uint32_t cpu_count = rtems_get_processor_count();
  size_t set_size = CPU_ALLOC_SIZE( rtems_get_processor_count() );
  cpu_set_t *cpu_set = CPU_ALLOC( rtems_get_processor_count() );
  SMP_barrier_State bs = SMP_BARRIER_STATE_INITIALIZER;

  /* Send message to all available CPUs */
  CPU_FILL_S( set_size, cpu_set );

  /* Call SMP cache manager functions */
  cmlog( "Calling standard SMP cache functions. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  standard_funcs_test( set_size, cpu_set );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n");

  /* Call SMP cache manager functions with ISR disabled */
  cmlog( "Calling standard SMP cache functions with ISR disabled. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  standard_funcs_isrdisabled_test( set_size, cpu_set, &bs );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n" );

  /* Call SMP cache manager functions with core 0 holding the giant lock */
  cmlog( "Calling standard SMP cache functions with CPU0 holding "
      "the giant lock. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  standard_funcs_giant_taken_test( set_size, cpu_set, &bs );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n");

  /* Call a test function using SMP cache manager and verify that all
   * cores invoke the function */
  cmlog( "Calling a test function using the SMP cache manager to "
      "verify that all CPUs receive the SMP message. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  test_func_test( set_size, cpu_set, &bs );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n");

  /* Call a test function using SMP cache manager and verify that all
   * cores invoke the function. ISR disabled. */
  cmlog( "Calling a test function using the SMP cache manager to "
      "verify that all CPUs receive the SMP message. With ISR disabled. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  test_func_isrdisabled_test( set_size, cpu_set, &bs );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n" );

  /* Call a test function using SMP cache manager and verify that all
   * cores invoke the function. Core 0 holding giant lock. */
  cmlog( "Calling a test function using the SMP cache manager to "
      "verify that all CPUs receive the SMP message. With CPU0 "
      "holding the giant lock. " );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  test_func_giant_taken_test( set_size, cpu_set, &bs );
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count );
  cmlog( "Done!\n" );

  /* Done. Free up memory. */
  _SMP_barrier_Wait( &ctx.barrier, &bs, cpu_count);
  CPU_FREE( cpu_set );
}

static void worker_task(rtems_task_argument arg)
{
  rtems_status_code sc;

  all_tests();

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_smp_cache_manager( void )
{
  rtems_status_code sc;
  size_t worker_index;
  uint32_t cpu_count = rtems_get_processor_count();

  for (worker_index = 1; worker_index < cpu_count; ++worker_index) {
    rtems_id worker_id;

    sc = rtems_task_create(
      rtems_build_name('W', 'R', 'K', '0'+worker_index),
      WORKER_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &worker_id
    );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_task_start( worker_id, worker_task, 0 );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }

  all_tests();
}


static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_smp_cache_manager();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
