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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/profiling.h>

#ifdef RTEMS_PROFILING

typedef struct {
  rtems_profiling_printf printf_func;
  void *printf_arg;
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
    int rv = (*ctx->printf_func)(ctx->printf_arg, "%s", ctx->indentation);

    update_retval(ctx, rv);
  }
}

static void report(void *arg, const rtems_profiling_data *data)
{
  context *ctx = arg;
}

#endif /* RTEMS_PROFILING */

int rtems_profiling_report_xml(
  const char *name,
  rtems_profiling_printf printf_func,
  void *printf_arg,
  uint32_t indentation_level,
  const char *indentation
)
{
#ifdef RTEMS_PROFILING
  context ctx_instance = {
    .printf_func = printf_func,
    .printf_arg = printf_arg,
    .indentation_level = indentation_level,
    .indentation = indentation,
    .retval = 0
  };
  context *ctx = &ctx_instance;
  int rv;

  indent(ctx, 0);
  rv = (*printf_func)(printf_arg, "<ProfilingReport name=\"%s\">\n", name);
  update_retval(ctx, rv);

  rtems_profiling_iterate(report, ctx);

  indent(ctx, 0);
  rv = (*printf_func)(printf_arg, "</ProfilingReport>\n");
  update_retval(ctx, rv);

  return ctx->retval;
#else /* RTEMS_PROFILING */
  (void) name;
  (void) printf_func;
  (void) printf_arg;
  (void) indentation_level;
  (void) indentation;

  return 0;
#endif /* RTEMS_PROFILING */
}
