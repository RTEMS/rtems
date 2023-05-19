/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * @brief Prints the Partition Table @a Partitions with @a Count Partitions
 */

/*
 * Copyright (C) 2009, 2010 embedded brains GmbH & Co. KG
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

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/bdpart.h>

static void rtems_bdpart_type_to_string(
  const uuid_t type,
  char str [37]
)
{
  uuid_unparse_lower( type, str);
}

void rtems_bdpart_dump( const rtems_bdpart_partition *pt, size_t count)
{
  size_t i = 0;

  printf(
    "-------------------------------------------------------------------------------\n"
    "                                PARTITION TABLE\n"
    "------------+------------+-----------------------------------------------------\n"
    " BEGIN      | LAST       | TYPE\n"
    "------------+------------+-----------------------------------------------------\n"
  );

  for (i = 0; i < count; ++i) {
    const rtems_bdpart_partition *p = pt + i;
    const char *type = NULL;
    char type_buffer [52];
    uint8_t type_mbr = 0;

    if (rtems_bdpart_to_mbr_partition_type( p->type, &type_mbr)) {
      switch (type_mbr) {
        case RTEMS_BDPART_MBR_FAT_12:
          type = "FAT 12";
          break;
        case RTEMS_BDPART_MBR_FAT_16:
          type = "FAT 16";
          break;
        case RTEMS_BDPART_MBR_FAT_16_LBA:
          type = "FAT 16 LBA";
          break;
        case RTEMS_BDPART_MBR_FAT_32:
          type = "FAT 32";
          break;
        case RTEMS_BDPART_MBR_FAT_32_LBA:
          type = "FAT 32 LBA";
          break;
        case RTEMS_BDPART_MBR_DATA:
          type = "DATA";
          break;
        default:
          snprintf( type_buffer, sizeof( type_buffer), "0x%02" PRIx8, type_mbr);
          type = type_buffer;
          break;
      }
    } else {
      rtems_bdpart_type_to_string( p->type, type_buffer);
      type = type_buffer;
    }

    printf(
      " %10" PRIu32 " | %10" PRIu32 " |%52s\n",
      p->begin,
      p->end - 1U,
      type
    );
  }

  puts( "------------+------------+-----------------------------------------------------");
}
