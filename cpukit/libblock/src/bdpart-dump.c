/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
    " BEGIN      | END        | TYPE\n"
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
      p->end,
      type
    );
  }

  puts( "------------+------------+-----------------------------------------------------");
}
