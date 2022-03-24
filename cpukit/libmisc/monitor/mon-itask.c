/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Monitor init task support
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <rtems/monitor.h>

#include <inttypes.h>
#include <stdio.h>

/*
 * As above, but just for init tasks
 */
void
rtems_monitor_init_task_canonical(
    rtems_monitor_init_task_t *canonical_itask,
    const void                *itask_void
)
{
    const rtems_initialization_tasks_table *rtems_itask = itask_void;

    rtems_monitor_symbol_canonical_by_value(&canonical_itask->entry,
                                            (void *) rtems_itask->entry_point);

    canonical_itask->argument = rtems_itask->argument;
    canonical_itask->stack_size = rtems_itask->stack_size;
    canonical_itask->priority = rtems_itask->initial_priority;
    canonical_itask->modes = rtems_itask->mode_set;
    canonical_itask->attributes = rtems_itask->attribute_set;
}

const void *
rtems_monitor_init_task_next(
    void                  *object_info RTEMS_UNUSED,
    rtems_monitor_init_task_t *canonical_init_task,
    rtems_id              *next_id
)
{
    const rtems_api_configuration_table *config;
    const rtems_initialization_tasks_table *itask;
    uint32_t   n = rtems_object_id_get_index(*next_id);

    config = rtems_configuration_get_rtems_api_configuration();
    if (n >= config->number_of_initialization_tasks)
        goto failed;

    _Objects_Allocator_lock();

    itask = config->User_initialization_tasks_table + n;

    /*
     * dummy up a fake id and name for this item
     */

    canonical_init_task->id = n;
    canonical_init_task->name = itask->name;

    *next_id += 1;
    return (const void *) itask;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_init_task_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    fprintf(stdout,"\
  #    NAME   ENTRY        ARGUMENT    PRIO   MODES  ATTRIBUTES   STACK SIZE\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_init_task_dump(
    rtems_monitor_init_task_t *monitor_itask,
    bool  verbose
)
{
    int length = 0;

    length += rtems_monitor_dump_decimal(monitor_itask->id);

    length += rtems_monitor_pad(7, length);
    length += rtems_monitor_dump_name(monitor_itask->id);

    length += rtems_monitor_pad(14, length);
    length += rtems_monitor_symbol_dump(&monitor_itask->entry, verbose);

    length += rtems_monitor_pad(25, length);
    length += fprintf(stdout,"%" PRId32 " [0x%" PRIx32 "]",
      monitor_itask->argument, monitor_itask->argument);

    length += rtems_monitor_pad(39, length);
    length += rtems_monitor_dump_priority(monitor_itask->priority);

    length += rtems_monitor_pad(46, length);
    length += rtems_monitor_dump_modes(monitor_itask->modes);

    length += rtems_monitor_pad(54, length);
    length += rtems_monitor_dump_attributes(monitor_itask->attributes);

    length += rtems_monitor_pad(66, length);
    length += fprintf(stdout,"%" PRId32 " [0x%" PRIx32 "]",
      monitor_itask->stack_size, monitor_itask->stack_size);

    fprintf(stdout,"\n");
}
