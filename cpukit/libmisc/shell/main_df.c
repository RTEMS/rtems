/* SPDX-License-Identifier: GPL-2.0-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup libfs_msdos MSDOS FileSystem
 *
 * @brief df Shell Command Implementation
 */

/*
 *  Copyright (c) 2013 Andrey Mozzhuhin
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/shell.h>

#define __need_getopt_newlib
#include <getopt.h>
#include <string.h>
#include <inttypes.h>

static const char suffixes[] =
{ 'B', 'K', 'M', 'G', 'T' };

struct df_context
{
  unsigned block_size;
};

static unsigned rtems_shell_df_parse_size(const char *str)
{
  unsigned result;
  char suffix;
  int i;

  if (sscanf(str, "%d%c", &result, &suffix) == 2)
  {
    for (i = 0; i < sizeof(suffixes) / sizeof(suffixes[0]); i++)
    {
      if (suffix == suffixes[i])
        break;
      result *= 1024;
    }
  }
  else if (sscanf(str, "%d", &result) != 1)
  {
    result = 0;
  }

  return result;
}

static char *rtems_shell_df_humanize_size(uint64_t block_size, char *buf,
    size_t size)
{
  int i = 0;

  while (block_size >= 1024 && i < sizeof(suffixes) / sizeof(suffixes[0]) - 1)
  {
    block_size /= 1024;
    i++;
  }

  snprintf(buf, size, "%"PRIu64"%c", block_size, suffixes[i]);
  return buf;
}

static bool rtems_shell_df_print_entry(
    const rtems_filesystem_mount_table_entry_t *mt_entry, void *arg)
{
  struct df_context *context = arg;

  struct statvfs svfs;
  int code;
  char f_buf[20], u_buf[20], a_buf[20];

  if ((code = statvfs(mt_entry->target, &svfs)))
    return false;

  if (context->block_size > 0)
  {
    printf(
        "%-15s %10" PRIu64 " %9" PRIu64 " %11" PRIu64 " %9" PRIu64 "%% %14s\n",
        mt_entry->dev == NULL ? "none" : mt_entry->dev,
        (svfs.f_blocks * svfs.f_frsize + (context->block_size - 1)) / context->block_size,
        ((svfs.f_blocks - svfs.f_bfree) * svfs.f_frsize + (context->block_size - 1)) / context->block_size,
        (svfs.f_bfree * svfs.f_frsize + (context->block_size - 1)) / context->block_size,
        ((svfs.f_blocks - svfs.f_bfree) * 100 / svfs.f_blocks),
        mt_entry->target == NULL ? "none" : mt_entry->target);
  }
  else
  {
    rtems_shell_df_humanize_size(svfs.f_blocks * svfs.f_frsize, f_buf,
        sizeof(f_buf));
    rtems_shell_df_humanize_size((svfs.f_blocks - svfs.f_bfree) * svfs.f_frsize,
        u_buf, sizeof(u_buf));
    rtems_shell_df_humanize_size(svfs.f_bfree * svfs.f_frsize, a_buf,
        sizeof(a_buf));
    printf("%-15s %10s %9s %11s %9" PRIu64 "%% %14s\n",
        mt_entry->dev == NULL ? "none" : mt_entry->dev, f_buf, u_buf, a_buf,
        (uint64_t)(svfs.f_blocks - svfs.f_bfree) * 100 / svfs.f_blocks,
        mt_entry->target == NULL ? "none" : mt_entry->target);
  }

  return false;
}

static int rtems_shell_main_df(int argc, char **argv)
{
  int c;
  struct getopt_data optdata;
  struct df_context context;
  char buf[32];

  memset(&optdata, 0, sizeof(optdata));
  context.block_size = 1024;

  while ((c = getopt_r(argc, (char**) argv, ":hB:", &optdata)) != -1)
  {
    switch (c)
    {
    case 'h':
      context.block_size = 0;
      break;
    case 'B':
      context.block_size = rtems_shell_df_parse_size(optdata.optarg);
      break;
    default:
      return -1;
    }
  }

  if (context.block_size == 0)
    printf(
        "Filesystem     Size             Used   Available       Use%%     Mounted on\n");
  else
    printf(
        "Filesystem     %s-blocks        Used   Available       Use%%     Mounted on\n",
        rtems_shell_df_humanize_size(context.block_size, buf, sizeof(buf)));

  rtems_filesystem_mount_iterate(rtems_shell_df_print_entry, &context);

  return 0;
}

rtems_shell_cmd_t rtems_shell_DF_Command =
{
    .name = "df",
    .usage = "[-hB]\n"
             " -h  human-readable output\n"
             " -B  scale sizes by SIZE before printing them\n",
    .topic = "files",
    .command = rtems_shell_main_df,
    .alias = NULL,
    .next = NULL
};
