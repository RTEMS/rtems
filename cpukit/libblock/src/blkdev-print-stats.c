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
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/blkdev.h>

#include <inttypes.h>

void rtems_blkdev_print_stats(
  const rtems_blkdev_stats *stats,
  rtems_printk_plugin_t print,
  void *print_arg
)
{
  (*print)(
     print_arg,
     "-------------------------------------------------------------------------------\n"
     "                               DEVICE STATISTICS\n"
     "----------------------+--------------------------------------------------------\n"
     " READ HITS            | %" PRIu32 "\n"
     " READ MISSES          | %" PRIu32 "\n"
     " READ AHEAD TRANSFERS | %" PRIu32 "\n"
     " READ BLOCKS          | %" PRIu32 "\n"
     " READ ERRORS          | %" PRIu32 "\n"
     " WRITE TRANSFERS      | %" PRIu32 "\n"
     " WRITE BLOCKS         | %" PRIu32 "\n"
     " WRITE ERRORS         | %" PRIu32 "\n"
     "----------------------+--------------------------------------------------------\n",
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
