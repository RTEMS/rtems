/**
 * @file
 *
 * @brief Prints the Block Device Statistics
 * @ingroup rtems_blkdev Block Device Management
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/blkdev.h>

#include <inttypes.h>

void rtems_blkdev_print_stats(
  const rtems_blkdev_stats *stats,
  uint32_t media_block_size,
  uint32_t media_block_count,
  uint32_t block_size,
  const rtems_printer* printer
)
{
  rtems_printf(
     printer,
     "-------------------------------------------------------------------------------\n"
     "                               DEVICE STATISTICS\n"
     "----------------------+--------------------------------------------------------\n"
     " MEDIA BLOCK SIZE     | %" PRIu32 "\n"
     " MEDIA BLOCK COUNT    | %" PRIu32 "\n"
     " BLOCK SIZE           | %" PRIu32 "\n"
     " READ HITS            | %" PRIu32 "\n"
     " READ MISSES          | %" PRIu32 "\n"
     " READ AHEAD TRANSFERS | %" PRIu32 "\n"
     " READ BLOCKS          | %" PRIu32 "\n"
     " READ ERRORS          | %" PRIu32 "\n"
     " WRITE TRANSFERS      | %" PRIu32 "\n"
     " WRITE BLOCKS         | %" PRIu32 "\n"
     " WRITE ERRORS         | %" PRIu32 "\n"
     "----------------------+--------------------------------------------------------\n",
     media_block_size,
     media_block_count,
     block_size,
     stats->read_hits,
     stats->read_misses,
     stats->read_ahead_transfers,
     stats->read_blocks,
     stats->read_errors,
     stats->write_transfers,
     stats->write_blocks,
     stats->write_errors
  );
}
