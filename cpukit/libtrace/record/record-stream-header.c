/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2019 embedded brains GmbH & Co. KG
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

#include <rtems/record.h>
#include <rtems/version.h>
#include <rtems.h>

#include <sys/endian.h>
#include <string.h>

size_t _Record_Stream_header_initialize( Record_Stream_header *header )
{
  rtems_record_item *items;
  size_t             available;
  size_t             used;
  const char        *str;

#if BYTE_ORDER == LITTLE_ENDIAN
#if __INTPTR_WIDTH__ == 32
  header->format = RTEMS_RECORD_FORMAT_LE_32,
#elif __INTPTR_WIDTH__ == 64
  header->format = RTEMS_RECORD_FORMAT_LE_64,
#else
#error "unexpected __INTPTR_WIDTH__"
#endif
#elif BYTE_ORDER == BIG_ENDIAN
#if __INTPTR_WIDTH__ == 32
  header->format = RTEMS_RECORD_FORMAT_BE_32,
#elif __INTPTR_WIDTH__ == 64
  header->format = RTEMS_RECORD_FORMAT_BE_64,
#else
#error "unexpected __INTPTR_WIDTH__"
#endif
#else
#error "unexpected BYTE_ORDER"
#endif

  header->magic = RTEMS_RECORD_MAGIC;

  header->Version.event = RTEMS_RECORD_TIME_EVENT( 0, RTEMS_RECORD_VERSION );
  header->Version.data = RTEMS_RECORD_THE_VERSION;

  header->Processor_maximum.event =
    RTEMS_RECORD_TIME_EVENT( 0, RTEMS_RECORD_PROCESSOR_MAXIMUM );
  header->Processor_maximum.data = rtems_scheduler_get_processor_maximum() - 1;

  header->Count.event = RTEMS_RECORD_TIME_EVENT( 0, RTEMS_RECORD_PER_CPU_COUNT );
  header->Count.data = _Record_Configuration.item_count;

  header->Frequency.event =
    RTEMS_RECORD_TIME_EVENT( 0, RTEMS_RECORD_FREQUENCY );
  header->Frequency.data = rtems_counter_frequency();

  items = header->Info;
  available = RTEMS_ARRAY_SIZE( header->Info );

  str = CPU_NAME;
  used = _Record_String_to_items(
    RTEMS_RECORD_ARCH,
    str,
    strlen( str ),
    items,
    available
  );
  items += used;
  available -= used;

  str = CPU_MODEL_NAME;
  used = _Record_String_to_items(
    RTEMS_RECORD_MULTILIB,
    str,
    strlen( str ),
    items,
    available
  );
  items += used;
  available -= used;

  str = rtems_board_support_package();
  used = _Record_String_to_items(
    RTEMS_RECORD_BSP,
    str,
    strlen( str ),
    items,
    available
  );
  items += used;
  available -= used;

  str = rtems_version_control_key();
  used = _Record_String_to_items(
    RTEMS_RECORD_VERSION_CONTROL_KEY,
    str,
    strlen( str ),
    items,
    available
  );
  items += used;
  available -= used;

  str = __VERSION__;
  used = _Record_String_to_items(
    RTEMS_RECORD_TOOLS,
    str,
    strlen( str ),
    items,
    available
  );
  items += used;

  return (size_t) ( (char *) items - (char *) header );
}
