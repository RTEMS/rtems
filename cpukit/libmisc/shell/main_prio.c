/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief The prio shell command
 */

/*
 * Copyright (c) 2025 Contemporary Software (chris@contemporary.software)
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

#include <pthread.h>

#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include <rtems/printer.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

struct prio_data {
  rtems_printer printer;
  int           thread_name_len;
  int           classic_pri_len;
  int           posix_pri_len;
};
typedef struct prio_data prio_data;

static size_t number_len( int num )
{
  size_t count = 0;
  while ( num > 0 ) {
    ++count;
    num /= 10;
  }
  return count;
}

static bool PRIO_sizes_thread_handler( Thread_Control *the_thread, void *arg )
{
  prio_data         *data = (prio_data *) arg;
  const rtems_id     classic_id = the_thread->Object.id;
  const pthread_t    posix_id = the_thread->Object.id;
  char               name[ 40 ];
  rtems_id           sched_id;
  int                sched_policy;
  struct sched_param sched_param;
  int                len;
  rtems_status_code  sc;
  int                rc;
  _Thread_Get_name( the_thread, name, sizeof( name ) );
  len = strnlen( name, sizeof( name ) );
  if ( len > data->thread_name_len ) {
    data->thread_name_len = len;
  }
  sc = rtems_task_get_scheduler( classic_id, &sched_id );
  if ( sc == RTEMS_SUCCESSFUL ) {
    rtems_task_priority classic_pri_max;
    sc = rtems_scheduler_get_maximum_priority( sched_id, &classic_pri_max );
    if ( sc == RTEMS_SUCCESSFUL ) {
      len = number_len( classic_pri_max );
      if ( len > data->classic_pri_len ) {
        data->classic_pri_len = len;
      }
    }
  }
  rc = pthread_getschedparam( posix_id, &sched_policy, &sched_param );
  if ( rc == 0 ) {
    len = number_len( sched_get_priority_min( sched_policy ) );
    if ( len > data->posix_pri_len ) {
      data->posix_pri_len = len;
    }
    len = number_len( sched_get_priority_max( sched_policy ) );
    if ( len > data->posix_pri_len ) {
      data->posix_pri_len = len;
    }
  }
  return false;
}

static bool PRIO_thread_handler( Thread_Control *the_thread, void *arg )
{
  const prio_data        *data = (prio_data *) arg;
  const rtems_id          classic_id = the_thread->Object.id;
  const pthread_t         posix_id = the_thread->Object.id;
  char                    name[ 40 ];
  char                    sched_name[ 8 ];
  Objects_Name            oname;
  rtems_id                sched_id;
  const rtems_status_code sched_sc = rtems_task_get_scheduler(
    classic_id,
    &sched_id
  );
  rtems_task_priority     classic_pri;
  const rtems_status_code classic_get_prio_sc = rtems_task_get_priority(
    classic_id,
    sched_id,
    &classic_pri
  );
  int                     sched_policy;
  rtems_task_priority     classic_pri_max;
  const rtems_status_code classic_get_prio_max_sc =
    rtems_scheduler_get_maximum_priority( sched_id, &classic_pri_max );
  struct sched_param sched_param;
  const int          get_sched_param_rc = pthread_getschedparam(
    posix_id,
    &sched_policy,
    &sched_param
  );
  const int posix_pri_min = sched_get_priority_min( sched_policy );
  const int posix_pri_max = sched_get_priority_max( sched_policy );
  _Thread_Get_name( the_thread, name, sizeof( name ) );
  oname.name_u32 = _Thread_Scheduler_get_home( the_thread )->name;
  _Objects_Name_to_string( oname, false, sched_name, sizeof( sched_name ) );
  (void) sched_sc; /* add error handling if needed */
  rtems_printf(
    &data->printer,
    "%08x %*s %4s ",
    classic_id,
    data->thread_name_len,
    name,
    sched_name
  );
  if ( classic_get_prio_sc == RTEMS_SUCCESSFUL ) {
    rtems_printf( &data->printer, "%*u ", data->classic_pri_len, classic_pri );
  } else {
    rtems_printf(
      &data->printer,
      "E:%*i ",
      data->classic_pri_len - 2,
      (int) classic_get_prio_sc
    );
  }
  if ( get_sched_param_rc == 0 ) {
    rtems_printf(
      &data->printer,
      "%*i ",
      data->posix_pri_len,
      sched_param.sched_priority
    );
  } else {
    rtems_printf(
      &data->printer,
      "E:%*i ",
      data->posix_pri_len - 2,
      get_sched_param_rc
    );
  }
  (void) classic_get_prio_max_sc; /* add error handling if needed */
  rtems_printf(
    &data->printer,
    "%*u %*u %*i %*i\n",
    data->classic_pri_len,
    classic_pri_max,
    data->classic_pri_len,
    1,
    data->posix_pri_len,
    posix_pri_min,
    data->posix_pri_len,
    posix_pri_max
  );
  return false;
}

static int rtems_shell_main_prio( int argc, char **argv )
{
  prio_data data;
  (void) argc;
  (void) argv;
  rtems_print_printer_fprintf( &data.printer, stdout );
  data.thread_name_len = 0;
  data.classic_pri_len = 5;
  data.posix_pri_len = 5;
  rtems_task_iterate( PRIO_sizes_thread_handler, &data );
  rtems_printf(
    &data.printer,
    "%-*s %-*s SCHD %-*s %-*s %-*s %-*s %-*s %-*s\n",
    8,
    "ID",
    data.thread_name_len,
    "THREAD",
    data.classic_pri_len,
    "CAPI",
    data.posix_pri_len,
    "PAPI",
    data.classic_pri_len,
    "CLOW",
    data.classic_pri_len,
    "CHIGH",
    data.posix_pri_len,
    "PLOW",
    data.posix_pri_len,
    "PHIGH"
  );
  rtems_task_iterate( PRIO_thread_handler, &data );
  return 0;
}

rtems_shell_cmd_t rtems_shell_PRIO_Command = {
  .name = "prio",
  .usage = "prio",
  .topic = "rtems",
  .command = rtems_shell_main_prio
};
