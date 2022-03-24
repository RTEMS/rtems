/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS MPCI Config display support
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

#include <stdio.h>
#include <stdlib.h>             /* strtoul() */
#include <inttypes.h>

#define DATACOL 15

/*
 * Fill in entire monitor config table
 * for sending to a remote monitor or printing on the local system
 */

void
rtems_monitor_mpci_canonical(
    rtems_monitor_mpci_t *canonical_mpci,
    const void           *config_void
)
{
    const rtems_multiprocessing_table *m;
    const rtems_mpci_table *mt;

    m = rtems_configuration_get_user_multiprocessing_table();
    mt = m->User_mpci_table;

    canonical_mpci->node = m->node;
    canonical_mpci->maximum_nodes = m->maximum_nodes;
    canonical_mpci->maximum_global_objects = m->maximum_global_objects;
    canonical_mpci->maximum_proxies = m->maximum_proxies;

    canonical_mpci->default_timeout = mt->default_timeout;
    canonical_mpci->maximum_packet_size = mt->maximum_packet_size;

    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->initialization,
                                            (void *) mt->initialization);

    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->get_packet,
                                            (void *) mt->get_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->return_packet,
                                            (void *) mt->return_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->send_packet,
                                            (void *) mt->send_packet);
    rtems_monitor_symbol_canonical_by_value(&canonical_mpci->receive_packet,
                                            (void *) mt->receive_packet);
}

/*
 * This is easy, since there is only 1 (altho we could get them from
 *    other nodes...)
 */

const void *
rtems_monitor_mpci_next(
    void                  *object_info,
    rtems_monitor_mpci_t  *canonical_mpci,
    rtems_id              *next_id
)
{
    int n = rtems_object_id_get_index(*next_id);

    if (n >= 1)
        goto failed;

    _Objects_Allocator_lock();

    *next_id += 1;
    return (const void *) (uintptr_t) 1;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_mpci_dump_header(
    bool verbose
)
{
    fprintf(stdout,"\
           max     max     max     default    max\n\
   node   nodes  globals  proxies  timeout  pktsize\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */

    rtems_monitor_separator();
}


void
rtems_monitor_mpci_dump(
    rtems_monitor_mpci_t *monitor_mpci,
    bool                  verbose
)
{
    uint32_t     length = 0;

    length += rtems_monitor_pad(2, length);
    length += fprintf(stdout,"  %" PRId32 , monitor_mpci->node);
    length += rtems_monitor_pad(11, length);
    length += fprintf(stdout,"%" PRId32, monitor_mpci->maximum_nodes);

    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->maximum_global_objects);

    length += rtems_monitor_pad(28, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->maximum_proxies);

    length += rtems_monitor_pad(37, length);
    length += rtems_monitor_dump_decimal(monitor_mpci->default_timeout);

    length += rtems_monitor_pad(46, length);
    length += rtems_monitor_dump_decimal((uint32_t) monitor_mpci->maximum_packet_size);

    fprintf(stdout,"\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += fprintf(stdout,"init: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->initialization, verbose);

    fprintf(stdout,"\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += fprintf(stdout,"get: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->get_packet, verbose);
    length += fprintf(stdout,";  return: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->return_packet, verbose);

    fprintf(stdout,"\n");
    length = 0;
    length += rtems_monitor_pad(DATACOL, length);

    length += fprintf(stdout,"send: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->send_packet, verbose);
    length += fprintf(stdout,";  receive: ");
    length += rtems_monitor_symbol_dump(&monitor_mpci->receive_packet, verbose);

    fprintf(stdout,"\n");
    length = 0;
}
