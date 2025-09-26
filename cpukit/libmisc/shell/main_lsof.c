/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief lsof Shell Command Implementation
 */

/*
 * Copyright (C) 2012, 2014 embedded brains GmbH & Co. KG
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

#include <rtems/libio_.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>

static void print_location( const rtems_filesystem_location_info_t *loc )
{
  fprintf(
    stdout,
    "\tloc = 0x%08" PRIxPTR "\n\t\tnode_access = 0x%08" PRIxPTR
      ", node_access_2 = 0x%08" PRIxPTR ", handler = 0x%08" PRIxPTR "\n",
    (uintptr_t) loc,
    (uintptr_t) loc->node_access,
    (uintptr_t) loc->node_access_2,
    (uintptr_t) loc->handlers
  );
}

static void print_mt_entry_locations(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  const rtems_chain_control *mt_entry_chain = &mt_entry->location_chain;
  const rtems_chain_node *mt_entry_node;

  for (
    mt_entry_node = rtems_chain_immutable_first( mt_entry_chain );
    !rtems_chain_is_tail( mt_entry_chain, mt_entry_node );
    mt_entry_node = rtems_chain_immutable_next( mt_entry_node )
  ) {
    const rtems_filesystem_location_info_t *loc =
      (const rtems_filesystem_location_info_t *) mt_entry_node;

    print_location( loc );
  }
}

static void lsof(void)
{
  const rtems_chain_control *mt_chain = &rtems_filesystem_mount_table;
  const rtems_chain_node *mt_node;

  fprintf(
    stdout,
    "type = null, root loc = 0x%08" PRIxPTR "\n",
    (uintptr_t) rtems_filesystem_null_mt_entry.mt_fs_root
  );

  print_mt_entry_locations( &rtems_filesystem_null_mt_entry );

  for (
    mt_node = rtems_chain_immutable_first( mt_chain );
    !rtems_chain_is_tail( mt_chain, mt_node );
    mt_node = rtems_chain_immutable_next( mt_node )
  ) {
    rtems_filesystem_mount_table_entry_t *mt_entry =
      (rtems_filesystem_mount_table_entry_t *) mt_node;

    fprintf(
      stdout,
      "type = %s, %s, %s, target = %s, dev = %s, root loc = 0x%08" PRIxPTR "\n",
      mt_entry->type,
      mt_entry->mounted ? "mounted" : "unmounted",
      mt_entry->writeable ? "read-write" : "read-only",
      mt_entry->target,
      mt_entry->dev == NULL ? "NULL" : mt_entry->dev,
      (uintptr_t) mt_entry->mt_fs_root
    );

    print_mt_entry_locations( mt_entry );
  }
}

static int rtems_shell_main_lsof(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  lsof();

  return 0;
}

rtems_shell_cmd_t rtems_shell_LSOF_Command = {
  .name = "lsof",
  .usage = "lsof",
  .topic = "files",
  .command = rtems_shell_main_lsof
};
