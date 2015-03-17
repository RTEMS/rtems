/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_TEST_H
#define _RTEMS_TEST_H

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/atomic.h>
#include <rtems/score/smpbarrier.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSTest Test Support
 *
 * @brief Test support functions.
 *
 * @{
 */

/**
 * @brief Each test must define a test name string.
 */
extern const char rtems_test_name[];

/**
 * @brief Fatal extension for tests.
 */
void rtems_test_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
);

/**
 * @brief Initial extension for tests.
 */
#define RTEMS_TEST_INITIAL_EXTENSION \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, rtems_test_fatal_extension }

/**
 * @brief Prints a begin of test message.
 *
 * @param[in] printf_func The formatted output function.
 * @param[in, out] printf_arg The formatted output function argument.
 *
 * @returns As specified by printf().
 */
int rtems_test_begin_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
);

/**
 * @brief Prints a begin of test message using printf().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_begin(void)
{
  return rtems_test_begin_with_plugin(rtems_printf_plugin, NULL);
}

/**
 * @brief Prints a begin of test message using printk().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_begink(void)
{
  return rtems_test_begin_with_plugin(printk_plugin, NULL);
}

/**
 * @brief Prints an end of test message.
 *
 * @param[in] printf_func The formatted output function.
 * @param[in, out] printf_arg The formatted output function argument.
 *
 * @returns As specified by printf().
 */
int rtems_test_end_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
);

/**
 * @brief Prints an end of test message using printf().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_end(void)
{
  return rtems_test_end_with_plugin(rtems_printf_plugin, NULL);
}

/**
 * @brief Prints an end of test message using printk().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_endk(void)
{
  return rtems_test_end_with_plugin(printk_plugin, NULL);
}

/**
 * @brief Internal context for parallel job execution.
 */
typedef struct {
  Atomic_Ulong stop;
  SMP_barrier_Control barrier;
  size_t worker_count;
  rtems_id worker_ids[32];
  rtems_id stop_worker_timer_id;
} rtems_test_parallel_context;

/**
 * @brief Worker task setup handler.
 *
 * Called during rtems_test_parallel() to optionally setup a worker task before
 * it is started.
 *
 * @param[in] ctx The parallel context.
 * @param[in] worker_index The worker index.
 * @param[in] worker_id The worker task identifier.
 */
typedef void (*rtems_test_parallel_worker_setup)(
  rtems_test_parallel_context *ctx,
  size_t worker_index,
  rtems_id worker_id
);

/**
 * @brief Basic parallel job description.
 */
typedef struct {
  /**
   * @brief Job initialization handler.
   *
   * This handler executes only in the context of the master worker before the
   * job body handler.
   *
   * @param[in] ctx The parallel context.
   * @param[in] arg The user specified argument.
   * @param[in] active_workers Count of active workers.  Depends on the cascade
   *   option.
   *
   * @return The desired job body execution time in clock ticks.  See
   *   rtems_test_parallel_stop_job().
   */
  rtems_interval (*init)(
    rtems_test_parallel_context *ctx,
    void *arg,
    size_t active_workers
  );

  /**
   * @brief Job body handler.
   *
   * @param[in] ctx The parallel context.
   * @param[in] arg The user specified argument.
   * @param[in] active_workers Count of active workers.  Depends on the cascade
   *   option.
   * @param[in] worker_index The worker index.  It ranges from 0 to the
   *   processor count minus one.
   */
  void (*body)(
    rtems_test_parallel_context *ctx,
    void *arg,
    size_t active_workers,
    size_t worker_index
  );

  /**
   * @brief Job finalization handler.
   *
   * This handler executes only in the context of the master worker after the
   * job body handler.
   *
   * @param[in] ctx The parallel context.
   * @param[in] arg The user specified argument.
   * @param[in] active_workers Count of active workers.  Depends on the cascade
   *   option.
   */
  void (*fini)(
    rtems_test_parallel_context *ctx,
    void *arg,
    size_t active_workers
  );

  /**
   * @brief Job specific argument.
   */
  void *arg;

  /**
   * @brief Job cascading flag.
   *
   * This flag indicates whether the job should be executed in a cascaded
   * manner (the job is executed on one processor first, two processors
   * afterwards and incremented step by step until all processors are used).
   */
  bool cascade;
} rtems_test_parallel_job;

/**
 * @brief Indicates if a job body should stop its work loop.
 *
 * @param[in] ctx The parallel context.
 *
 * @retval true The job body should stop its work loop and return to the caller.
 * @retval false Otherwise.
 */
static inline bool rtems_test_parallel_stop_job(
  const rtems_test_parallel_context *ctx
)
{
  return _Atomic_Load_ulong(&ctx->stop, ATOMIC_ORDER_RELAXED) != 0;
}

/**
 * @brief Indicates if a worker is the master worker.
 *
 * The master worker is the thread that called rtems_test_parallel().
 *
 * @param[in] worker_index The worker index.
 *
 * @retval true This is the master worker.
 * @retval false Otherwise.
 */
static inline bool rtems_test_parallel_is_master_worker(size_t worker_index)
{
  return worker_index == 0;
}

/**
 * @brief Returns the task identifier for a worker.
 *
 * @param[in] ctx The parallel context.
 * @param[in] worker_index The worker index.
 *
 * @return The task identifier of the worker.
 */
static inline rtems_id rtems_test_parallel_get_task_id(
  const rtems_test_parallel_context *ctx,
  size_t worker_index
)
{
  return ctx->worker_ids[worker_index];
}

/**
 * @brief Runs a bunch of jobs in parallel on all processors of the system.
 *
 * The worker tasks inherit the priority of the executing task.
 *
 * There are SMP barriers before and after the job body.
 *
 * @param[in] ctx The parallel context.
 * @param[in] worker_setup Optional handler to setup a worker task before it is
 *   started.
 * @param[in] jobs The table of jobs.
 * @param[in] job_count The count of jobs in the job table.
 */
void rtems_test_parallel(
  rtems_test_parallel_context *ctx,
  rtems_test_parallel_worker_setup worker_setup,
  const rtems_test_parallel_job *jobs,
  size_t job_count
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_TEST_H */
