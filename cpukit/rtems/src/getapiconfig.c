/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/rtems/config.h>
#include <rtems/rtems/tasksdata.h>

static rtems_api_configuration_table config;

const rtems_api_configuration_table *
rtems_configuration_get_rtems_api_configuration( void )
{
  /*
   * Always initialize the configuration table and do not bother with
   * synchronization issues.  This function is not performance critical.  The
   * configuration values are all constants.
   */

  config.notepads_enabled = false;
  config.maximum_tasks = rtems_configuration_get_maximum_tasks();
  config.maximum_timers = rtems_configuration_get_maximum_timers();
  config.maximum_semaphores = rtems_configuration_get_maximum_semaphores();
  config.maximum_message_queues =
    rtems_configuration_get_maximum_message_queues();
  config.maximum_partitions = rtems_configuration_get_maximum_partitions();
  config.maximum_regions = rtems_configuration_get_maximum_regions();
  config.maximum_ports = rtems_configuration_get_maximum_ports();
  config.maximum_periods = rtems_configuration_get_maximum_periods();
  config.maximum_barriers = rtems_configuration_get_maximum_barriers();

  if ( _RTEMS_tasks_User_task_table.entry_point != NULL ) {
    config.number_of_initialization_tasks = 1;
    config.User_initialization_tasks_table = &_RTEMS_tasks_User_task_table;
  }

  return &config;
}
