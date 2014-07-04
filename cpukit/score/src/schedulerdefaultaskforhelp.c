/*
 * Copyright (c) 2014 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/scheduler.h>

Thread_Control *_Scheduler_default_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *offers_help,
  Thread_Control          *needs_help
)
{
  (void) scheduler;
  (void) offers_help;
  (void) needs_help;

  return NULL;
}
