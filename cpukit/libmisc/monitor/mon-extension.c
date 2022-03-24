/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Monitor extension support
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
#include <rtems/extensiondata.h>
#include <rtems/monitor.h>

#include <stdio.h>

void
rtems_monitor_extension_canonical(
    rtems_monitor_extension_t *canonical_extension,
    const void                *extension_void
)
{
    const Extension_Control *rtems_extension = (const Extension_Control *) extension_void;
    const rtems_extensions_table *e = &rtems_extension->Extension.Callouts;

    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_create,
                                            (void *) e->thread_create);

    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_start,
                                            (void *) e->thread_start);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_restart,
                                            (void *) e->thread_restart);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_delete,
                                            (void *) e->thread_delete);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_tswitch,
                                            (void *) e->thread_switch);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_begin,
                                            (void *) e->thread_begin);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_exitted,
                                            (void *) e->thread_exitted);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->e_fatal,
                                            (void *) e->fatal);
}

void
rtems_monitor_extension_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    fprintf(stdout,"\
  ID       NAME\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */

    rtems_monitor_separator();
}


/*
 * Dump out the canonical form
 */

void
rtems_monitor_extension_dump(
    rtems_monitor_extension_t *monitor_extension,
    bool                       verbose
)
{
    uint32_t             length = 0;

    length += rtems_monitor_dump_id(monitor_extension->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_extension->id);

    length += rtems_monitor_pad(18, length);
    length += fprintf(stdout,"create: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_create, verbose);
    length += fprintf(stdout,";  start: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_start, verbose);
    length += fprintf(stdout,";  restart: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_restart, verbose);
    length += fprintf(stdout,"\n");
    length = 0;

    length += rtems_monitor_pad(18, length);
    length += fprintf(stdout,"delete: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_delete, verbose);
    length += fprintf(stdout,";  switch: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_tswitch, verbose);
    length += fprintf(stdout,";  begin: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_begin, verbose);
    length += fprintf(stdout,"\n");
    length = 0;

    length += rtems_monitor_pad(18, length);
    length += fprintf(stdout,"exitted: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_exitted, verbose);
    length += fprintf(stdout,";  fatal: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->e_fatal, verbose);
    length += fprintf(stdout,"\n");
    length = 0;
    fprintf(stdout,"\n");
}
