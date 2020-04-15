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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/profiling.h>

#ifdef RTEMS_PROFILING

#include <inttypes.h>

typedef struct {
  const rtems_printer *printer;
  uint32_t indentation_level;
  const char *indentation;
  int retval;
} context;

static void update_retval(context *ctx, int rv)
{
  if (rv > 0 && ctx->retval >= 0) {
    ctx->retval += rv;
  }
}

static void indent(context *ctx, uint32_t indentation_level)
{
  uint32_t n = ctx->indentation_level + indentation_level;
  uint32_t i;

  for (i = 0; i < n; ++i) {
    int rv = rtems_printf(ctx->printer, "%s", ctx->indentation);

    update_retval(ctx, rv);
  }
}

static uint64_t arithmetic_mean(uint64_t total, uint64_t count)
{
  return count != 0 ? total / count : 0;
}

static void report_per_cpu(context *ctx, const rtems_profiling_per_cpu *per_cpu)
{
  int rv;

  indent(ctx, 1);
  rv = rtems_printf(
    ctx->printer,
    "<PerCPUProfilingReport processorIndex=\"%" PRIu32 "\">\n",
    per_cpu->processor_index
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MaxThreadDispatchDisabledTime unit=\"ns\">%" PRIu32
      "</MaxThreadDispatchDisabledTime>\n",
    per_cpu->max_thread_dispatch_disabled_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MeanThreadDispatchDisabledTime unit=\"ns\">%" PRIu64
      "</MeanThreadDispatchDisabledTime>\n",
    arithmetic_mean(
      per_cpu->total_thread_dispatch_disabled_time,
      per_cpu->thread_dispatch_disabled_count
    )
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<TotalThreadDispatchDisabledTime unit=\"ns\">%" PRIu64
      "</TotalThreadDispatchDisabledTime>\n",
    per_cpu->total_thread_dispatch_disabled_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<ThreadDispatchDisabledCount>%" PRIu64 "</ThreadDispatchDisabledCount>\n",
    per_cpu->thread_dispatch_disabled_count
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MaxInterruptDelay unit=\"ns\">%" PRIu32 "</MaxInterruptDelay>\n",
    per_cpu->max_interrupt_delay
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MaxInterruptTime unit=\"ns\">%" PRIu32
      "</MaxInterruptTime>\n",
    per_cpu->max_interrupt_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MeanInterruptTime unit=\"ns\">%" PRIu64
      "</MeanInterruptTime>\n",
    arithmetic_mean(
      per_cpu->total_interrupt_time,
      per_cpu->interrupt_count
    )
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<TotalInterruptTime unit=\"ns\">%" PRIu64 "</TotalInterruptTime>\n",
    per_cpu->total_interrupt_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<InterruptCount>%" PRIu64 "</InterruptCount>\n",
    per_cpu->interrupt_count
  );
  update_retval(ctx, rv);

  indent(ctx, 1);
  rv = rtems_printf(
    ctx->printer,
    "</PerCPUProfilingReport>\n"
  );
  update_retval(ctx, rv);
}

static void report_smp_lock(context *ctx, const rtems_profiling_smp_lock *smp_lock)
{
  int rv;
  uint32_t i;

  indent(ctx, 1);
  rv = rtems_printf(
    ctx->printer,
    "<SMPLockProfilingReport name=\"%s\">\n",
    smp_lock->name
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MaxAcquireTime unit=\"ns\">%" PRIu32 "</MaxAcquireTime>\n",
    smp_lock->max_acquire_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MaxSectionTime unit=\"ns\">%" PRIu32 "</MaxSectionTime>\n",
    smp_lock->max_section_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MeanAcquireTime unit=\"ns\">%" PRIu64
      "</MeanAcquireTime>\n",
    arithmetic_mean(
      smp_lock->total_acquire_time,
      smp_lock->usage_count
    )
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<MeanSectionTime unit=\"ns\">%" PRIu64
      "</MeanSectionTime>\n",
    arithmetic_mean(
      smp_lock->total_section_time,
      smp_lock->usage_count
    )
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<TotalAcquireTime unit=\"ns\">%" PRIu64 "</TotalAcquireTime>\n",
    smp_lock->total_acquire_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<TotalSectionTime unit=\"ns\">%" PRIu64 "</TotalSectionTime>\n",
    smp_lock->total_section_time
  );
  update_retval(ctx, rv);

  indent(ctx, 2);
  rv = rtems_printf(
    ctx->printer,
    "<UsageCount>%" PRIu64 "</UsageCount>\n",
    smp_lock->usage_count
  );
  update_retval(ctx, rv);

  for (i = 0; i < RTEMS_PROFILING_SMP_LOCK_CONTENTION_COUNTS; ++i) {
    indent(ctx, 2);
    rv = rtems_printf(
      ctx->printer,
      "<ContentionCount initialQueueLength=\"%" PRIu32 "\">%"
        PRIu64 "</ContentionCount>\n",
      i,
      smp_lock->contention_counts[i]
    );
    update_retval(ctx, rv);
  }

  indent(ctx, 1);
  rv = rtems_printf(
    ctx->printer,
    "</SMPLockProfilingReport>\n"
  );
  update_retval(ctx, rv);
}

static void report(void *arg, const rtems_profiling_data *data)
{
  context *ctx = arg;

  switch (data->header.type) {
    case RTEMS_PROFILING_PER_CPU:
      report_per_cpu(ctx, &data->per_cpu);
      break;
    case RTEMS_PROFILING_SMP_LOCK:
      report_smp_lock(ctx, &data->smp_lock);
      break;
  }
}

#endif /* RTEMS_PROFILING */

int rtems_profiling_report_xml(
  const char *name,
  const rtems_printer *printer,
  uint32_t indentation_level,
  const char *indentation
)
{
#ifdef RTEMS_PROFILING
  context ctx_instance = {
    .printer = printer,
    .indentation_level = indentation_level,
    .indentation = indentation,
    .retval = 0
  };
  context *ctx = &ctx_instance;
  int rv;

  indent(ctx, 0);
  rv = rtems_printf(printer, "<ProfilingReport name=\"%s\">\n", name);
  update_retval(ctx, rv);

  rtems_profiling_iterate(report, ctx);

  indent(ctx, 0);
  rv = rtems_printf(printer, "</ProfilingReport>\n");
  update_retval(ctx, rv);

  return ctx->retval;
#else /* RTEMS_PROFILING */
  (void) name;
  (void) printer;
  (void) indentation_level;
  (void) indentation;

  return 0;
#endif /* RTEMS_PROFILING */
}
