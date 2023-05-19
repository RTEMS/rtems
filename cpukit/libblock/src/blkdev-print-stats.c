/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_blkdev Block Device Management
 *
 * @brief Prints the Block Device Statistics
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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
     " READ AHEAD PEEKS     | %" PRIu32 "\n"
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
     stats->read_ahead_peeks,
     stats->read_blocks,
     stats->read_errors,
     stats->write_transfers,
     stats->write_blocks,
     stats->write_errors
  );
}
