/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Monitor partition support
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
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/partdata.h>
#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_part_canonical(
    rtems_monitor_part_t  *canonical_part,
    const void            *part_void
)
{
    const Partition_Control *rtems_part = (const Partition_Control *) part_void;

    canonical_part->attribute = rtems_part->attribute_set;
    canonical_part->start_addr = rtems_part->base_address;
    canonical_part->length = (uint32_t) ( (uintptr_t)
        rtems_part->limit_address + 1 - (uintptr_t) rtems_part->base_address );
    canonical_part->buf_size = rtems_part->buffer_size;
    canonical_part->used_blocks = rtems_part->number_of_used_blocks;
}


void
rtems_monitor_part_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    printf("\
  ID       NAME   ATTR        STARTADDR LENGTH    BUF_SIZE  USED_BLOCKS\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_part_dump(
    rtems_monitor_part_t *monitor_part,
    bool  verbose RTEMS_UNUSED
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_part->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_part->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_part->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_addr(monitor_part->start_addr);
    length += rtems_monitor_pad(40, length);
    length += rtems_monitor_dump_hex(monitor_part->length);
    length += rtems_monitor_pad(50, length);
    length += rtems_monitor_dump_hex(monitor_part->buf_size);
    length += rtems_monitor_pad(60, length);
    length += rtems_monitor_dump_hex(monitor_part->used_blocks);
    printf("\n");
}

