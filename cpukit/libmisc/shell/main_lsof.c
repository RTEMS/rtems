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

#include <stdio.h>

#include <rtems/libio_.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>

static void lsof(void)
{
  rtems_chain_control *mt_chain = &rtems_filesystem_mount_table;
  rtems_chain_node *mt_node = NULL;

  for (
    mt_node = rtems_chain_first( mt_chain );
    !rtems_chain_is_tail( mt_chain, mt_node );
    mt_node = rtems_chain_next( mt_node )
  ) {
    rtems_filesystem_mount_table_entry_t *mt_entry =
      (rtems_filesystem_mount_table_entry_t *) mt_node;
    rtems_chain_control *mt_entry_chain = &mt_entry->location_chain;
    rtems_chain_node *mt_entry_node = NULL;

    fprintf(
      stdout,
      "%c %c %s %s -> %s root 0x%08x -> 0x%08x\n",
      mt_entry->mounted ? 'M' : 'U',
      mt_entry->writeable ? 'W' : 'R',
      mt_entry->type,
      mt_entry->target,
      mt_entry->dev,
      mt_entry->mt_fs_root,
      mt_entry->mt_fs_root->location.node_access
    );

    for (
      mt_entry_node = rtems_chain_first( mt_entry_chain );
      !rtems_chain_is_tail( mt_entry_chain, mt_entry_node );
      mt_entry_node = rtems_chain_next( mt_entry_node )
    ) {
      const rtems_filesystem_location_info_t *loc =
        (rtems_filesystem_location_info_t *) mt_entry_node;

      fprintf(
        stdout,
        "\t0x%08x -> 0x%08x\n",
        loc,
        loc->node_access
      );
    }
  }
}

static int rtems_shell_main_lsof(int argc, char **argv)
{
  lsof();

  return 0;
}

rtems_shell_cmd_t rtems_shell_LSOF_Command = {
  .name = "lsof",
  .usage = "lsof",
  .topic = "files",
  .command = rtems_shell_main_lsof
};
