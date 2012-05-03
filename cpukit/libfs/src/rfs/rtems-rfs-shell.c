/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File Systems Shell Commands Support
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <string.h>

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-group.h>
#include <rtems/rfs/rtems-rfs-inode.h>
#include <rtems/rfs/rtems-rfs-dir.h>
#include <rtems/rtems-rfs-format.h>

#include <sys/statvfs.h>

#if __rtems__
#include "rtems-rfs-rtems.h"
#endif

/**
 * The type of the shell handlers we have.
 */
typedef int (*rtems_rfs_shell_handler) (rtems_rfs_file_system* fs, int argc, char *argv[]);

/**
 * Table of handlers we parse to invoke the command.
 */
typedef struct
{
  const char*             name;
  rtems_rfs_shell_handler handler;
  const char*             help;
} rtems_rfs_shell_cmd;

/**
 * Lock the file system.
 */
static void
rtems_rfs_shell_lock_rfs (rtems_rfs_file_system* fs)
{
#if __rtems__
  rtems_rfs_rtems_lock (fs);
#endif
}

/**
 * Unlock the file system.
 */
static void
rtems_rfs_shell_unlock_rfs (rtems_rfs_file_system* fs)
{
#if __rtems__
  rtems_rfs_rtems_unlock (fs);
#endif
}

/**
 * Get the file system data from the specific path. Checks to make sure the path is
 * pointing to a valid RFS file system.
 */
static int
rtems_rfs_get_fs (const char* path, rtems_rfs_file_system** fs)
{
  struct statvfs sb;
  int            rc;

  rc = statvfs (path, &sb);
  if (rc < 0)
  {
    printf ("error: cannot statvfs path: %s: (%d) %s\n",
            path, errno, strerror (errno));
    return -1;
  }

  if (sb.f_fsid != RTEMS_RFS_SB_MAGIC)
  {
    printf ("error: path '%s' is not on an RFS file system\n", path);
    return -1;
  }

#if __rtems__
  /*
   * Now find the path location on the file system. This will give the file
   * system data.
   */
  {
    rtems_filesystem_eval_path_context_t ctx;
    int eval_flags = RTEMS_FS_FOLLOW_LINK;
    const rtems_filesystem_location_info_t *currentloc =
      rtems_filesystem_eval_path_start (&ctx, path, eval_flags);
    *fs = rtems_rfs_rtems_pathloc_dev (currentloc);
    rtems_filesystem_eval_path_cleanup (&ctx);
  }
#endif

  return rc;
}

static int
rtems_rfs_shell_data (rtems_rfs_file_system* fs, int argc, char *argv[])
{
  size_t blocks;
  size_t inodes;
  int    bpcent;
  int    ipcent;

  printf ("RFS Filesystem Data\n");
  printf ("             flags: %08" PRIx32 "\n", fs->flags);
#if 0
  printf ("            device: %08lx\n",         rtems_rfs_fs_device (fs));
#endif
  printf ("            blocks: %zu\n",           rtems_rfs_fs_blocks (fs));
  printf ("        block size: %zu\n",           rtems_rfs_fs_block_size (fs));
  printf ("              size: %" PRIu64 "\n",   rtems_rfs_fs_size (fs));
  printf ("  media block size: %" PRIu32 "\n",   rtems_rfs_fs_media_block_size (fs));
  printf ("        media size: %" PRIu64 "\n",   rtems_rfs_fs_media_size (fs));
  printf ("            inodes: %" PRIu32 "\n",   rtems_rfs_fs_inodes (fs));
  printf ("        bad blocks: %" PRIu32 "\n",   fs->bad_blocks);
  printf ("  max. name length: %" PRIu32 "\n",   rtems_rfs_fs_max_name (fs));
  printf ("            groups: %d\n",            fs->group_count);
  printf ("      group blocks: %zd\n",           fs->group_blocks);
  printf ("      group inodes: %zd\n",           fs->group_inodes);
  printf ("  inodes per block: %zd\n",           fs->inodes_per_block);
  printf ("  blocks per block: %zd\n",           fs->blocks_per_block);
  printf ("     singly blocks: %zd\n",           fs->block_map_singly_blocks);
  printf ("    doublly blocks: %zd\n",           fs->block_map_doubly_blocks);
  printf (" max. held buffers: %" PRId32 "\n",   fs->max_held_buffers);

  rtems_rfs_shell_lock_rfs (fs);

  rtems_rfs_group_usage (fs, &blocks, &inodes);

  rtems_rfs_shell_unlock_rfs (fs);

  bpcent = (blocks * 1000) / rtems_rfs_fs_blocks (fs);
  ipcent = (inodes * 1000) / rtems_rfs_fs_inodes (fs);

  printf ("       blocks used: %zd (%d.%d%%)\n",
          blocks, bpcent / 10, bpcent % 10);
  printf ("       inodes used: %zd (%d.%d%%)\n",
          inodes, ipcent / 10, ipcent % 10);

  return 0;
}

static int
rtems_rfs_shell_block (rtems_rfs_file_system* fs, int argc, char *argv[])
{
  rtems_rfs_buffer_handle buffer;
  rtems_rfs_block_no      block;
  uint8_t*                data;
  bool                    state;
  int                     b;
  int                     rc;

  if (argc <= 1)
  {
    printf ("error: no block number provided\n");
    return 1;
  }

  block = strtoul (argv[1], 0, 0);

  rtems_rfs_shell_lock_rfs (fs);

  rc = rtems_rfs_group_bitmap_test (fs, false, block, &state);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: testing block state: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  printf (" %5" PRIu32 ": block %s\n", block, state ? "allocated" : "free");

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: opening buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, true);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &buffer);
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: requesting buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  for (b = 0, data = rtems_rfs_buffer_data (&buffer);
       b < rtems_rfs_fs_block_size (fs);
       b++, data++)
  {
    int mod = b % 16;
    if (mod == 0)
    {
      if (b)
        printf ("\n");
      printf ("%04x ", b);
    }
    if (mod == 8)
      printf (" ");
    printf ("%02x ", *data);
  }

  printf ("\n");

  rc = rtems_rfs_buffer_handle_close (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: closing buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  rtems_rfs_shell_unlock_rfs (fs);

  return 0;
}

static int
rtems_rfs_shell_inode (rtems_rfs_file_system* fs, int argc, char *argv[])
{
  rtems_rfs_ino start;
  rtems_rfs_ino end;
  rtems_rfs_ino total;
  rtems_rfs_ino ino;
  bool          show_all;
  bool          error_check_only;
  bool          forced;
  bool          have_start;
  bool          have_end;
  int           arg;
  int           b;
  int           rc;

  total = fs->group_inodes * fs->group_count;
  start = RTEMS_RFS_ROOT_INO;
  end = total - 1;
  show_all = false;
  error_check_only = false;
  forced = false;
  have_start = have_end = false;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'a':
          show_all = true;
          break;
        case 'e':
          error_check_only = true;
          break;
        case 'f':
          forced = true;
          break;
        default:
          printf ("warning: option ignored: %s\n", argv[arg]);
          break;
      }
    }
    else
    {
      if (have_end && have_start)
        printf ("warning: option ignored: %s\n", argv[arg]);
      else if (!have_start)
      {
        start = end = strtoul (argv[arg], 0, 0);
        have_start = true;
      }
      else
      {
        end = strtoul (argv[arg], 0, 0);
        have_end = true;
      }
    }
  }

  if ((start >= total) || (end >= total))
  {
    printf ("error: inode out of range (0->%" PRId32 ").\n", total - 1);
    return 1;
  }

  rtems_rfs_shell_lock_rfs (fs);

  for (ino = start; ino <= end; ino++)
  {
    rtems_rfs_inode_handle inode;
    bool                   allocated;

    rc = rtems_rfs_group_bitmap_test (fs, true, ino, &allocated);
    if (rc > 0)
    {
      rtems_rfs_shell_unlock_rfs (fs);
      printf ("error: testing inode state: ino=%" PRIu32 ": (%d) %s\n",
              ino, rc, strerror (rc));
      return 1;
    }

    if (show_all || allocated)
    {
      uint16_t mode;
      bool     error;

      rc = rtems_rfs_inode_open (fs, ino, &inode, true);
      if (rc > 0)
      {
        rtems_rfs_shell_unlock_rfs (fs);
        printf ("error: opening inode handle: ino=%" PRIu32 ": (%d) %s\n",
                ino, rc, strerror (rc));
        return 1;
      }

      error = false;

      mode = rtems_rfs_inode_get_mode (&inode);

      if (error_check_only)
      {
        if (!RTEMS_RFS_S_ISDIR (mode) &&
            !RTEMS_RFS_S_ISCHR (mode) &&
            !RTEMS_RFS_S_ISBLK (mode) &&
            !RTEMS_RFS_S_ISREG (mode) &&
            !RTEMS_RFS_S_ISLNK (mode))
          error = true;
        else
        {
#if NEED_TO_HANDLE_DIFFERENT_TYPES
          int b;
          for (b = 0; b < RTEMS_RFS_INODE_BLOCKS; b++)
          {
            uint32_t block;
            block = rtems_rfs_inode_get_block (&inode, b);
            if ((block <= RTEMS_RFS_SUPERBLOCK_SIZE) ||
                (block >= rtems_rfs_fs_blocks (fs)))
              error = true;
          }
#endif
        }
      }

      if (!error_check_only || error)
      {
        printf (" %5" PRIu32 ": pos=%06" PRIu32 ":%04zx %c ",
                ino, rtems_rfs_buffer_bnum (&inode.buffer),
                inode.offset * RTEMS_RFS_INODE_SIZE,
                allocated ? 'A' : 'F');

        if (!allocated && !forced)
          printf (" --\n");
        else
        {
          const char* type;
          type = "UKN";
          if (RTEMS_RFS_S_ISDIR (mode))
            type = "DIR";
          else if (RTEMS_RFS_S_ISCHR (mode))
            type = "CHR";
          else if (RTEMS_RFS_S_ISBLK (mode))
            type = "BLK";
          else if (RTEMS_RFS_S_ISREG (mode))
            type = "REG";
          else if (RTEMS_RFS_S_ISLNK (mode))
            type = "LNK";
          printf ("links=%03i mode=%04x (%s/%03o) bo=%04u bc=%04" PRIu32 " b=[",
                  rtems_rfs_inode_get_links (&inode),
                  mode, type, mode & ((1 << 10) - 1),
                  rtems_rfs_inode_get_block_offset (&inode),
                  rtems_rfs_inode_get_block_count (&inode));
          for (b = 0; b < (RTEMS_RFS_INODE_BLOCKS - 1); b++)
            printf ("%" PRIu32 " ", rtems_rfs_inode_get_block (&inode, b));
          printf ("%" PRIu32 "]\n", rtems_rfs_inode_get_block (&inode, b));
        }
      }

      rc = rtems_rfs_inode_close (fs, &inode);
      if (rc > 0)
      {
        rtems_rfs_shell_unlock_rfs (fs);
        printf ("error: closing inode handle: ino=%" PRIu32 ": (%d) %s\n",
                ino, rc, strerror (rc));
        return 1;
      }
    }
  }

  rtems_rfs_shell_unlock_rfs (fs);

  return 0;
}

static int
rtems_rfs_shell_dir (rtems_rfs_file_system* fs, int argc, char *argv[])
{
  rtems_rfs_buffer_handle buffer;
  rtems_rfs_block_no      block;
  uint8_t*                data;
  bool                    state;
  int                     entry;
  int                     b;
  int                     rc;

  if (argc <= 1)
  {
    printf ("error: no block number provided\n");
    return 1;
  }

  block = strtoul (argv[1], 0, 0);

  rtems_rfs_shell_lock_rfs (fs);

  rc = rtems_rfs_group_bitmap_test (fs, false, block, &state);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: testing block state: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  printf (" %5" PRIu32 ": block %s\n", block, state ? "allocated" : "free");

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: opening buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, true);
  if (rc > 0)
  {
    rtems_rfs_buffer_handle_close (fs, &buffer);
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: requesting buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  b = 0;
  entry = 1;
  data = rtems_rfs_buffer_data (&buffer);

  while (b < (rtems_rfs_fs_block_size (fs) - RTEMS_RFS_DIR_ENTRY_SIZE - 1))
  {
    rtems_rfs_ino eino;
    int           elength;
    int           length;
    int           c;

    eino    = rtems_rfs_dir_entry_ino (data);
    elength = rtems_rfs_dir_entry_length (data);

    if (elength == RTEMS_RFS_DIR_ENTRY_EMPTY)
      break;

    if ((elength < RTEMS_RFS_DIR_ENTRY_SIZE) ||
        (elength >= rtems_rfs_fs_max_name (fs)))
    {
      printf (" %5d: entry length appears corrupt: %d\n", entry, elength);
      break;
    }

    if ((eino < RTEMS_RFS_ROOT_INO) || (eino >= rtems_rfs_fs_inodes (fs)))
    {
      printf (" %5d: entry ino appears corrupt: ino=%" PRId32 "\n", entry, eino);
      break;
    }

    length = elength - RTEMS_RFS_DIR_ENTRY_SIZE;

    printf (" %5d: %04x inode=%-6" PRIu32 " hash=%08" PRIx32 " name[%03u]=",
            entry, b,
            rtems_rfs_dir_entry_ino (data),
            rtems_rfs_dir_entry_hash (data),
            length);

    if (length > 50)
      length = 50;

    for (c = 0; c < length; c++)
      printf ("%c", data[RTEMS_RFS_DIR_ENTRY_SIZE + c]);
    if (length < elength - RTEMS_RFS_DIR_ENTRY_SIZE)
      printf ("...");
    printf ("\n");

    b += elength;
    data += elength;
    entry++;
  }

  rc = rtems_rfs_buffer_handle_close (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_shell_unlock_rfs (fs);
    printf ("error: closing buffer handle: block=%" PRIu32 ": (%d) %s\n",
            block, rc, strerror (rc));
    return 1;
  }

  rtems_rfs_shell_unlock_rfs (fs);

  return 0;
}

static int
rtems_rfs_shell_group (rtems_rfs_file_system* fs, int argc, char *argv[])
{
  int start;
  int end;
  int g;

  start = 0;
  end = fs->group_count - 1;

  switch (argc)
  {
    case 1:
      break;
    case 2:
      start = end = strtoul (argv[1], 0, 0);
      break;
    case 3:
      start = strtoul (argv[1], 0, 0);
      end = strtoul (argv[2], 0, 0);
      break;
    default:
      printf ("error: too many arguments.\n");
      return 1;
  }

  if ((start < 0) || (end < 0) ||
      (start >= fs->group_count) || (end >= fs->group_count))
  {
    printf ("error: group out of range (0->%d).\n", fs->group_count);
    return 1;
  }

  rtems_rfs_shell_lock_rfs (fs);

  for (g = start; g <= end; g++)
  {
    rtems_rfs_group* group = &fs->groups[g];
    size_t           blocks;
    size_t           inodes;
    blocks = group->size - rtems_rfs_bitmap_map_free (&group->block_bitmap);
    inodes = fs->group_inodes - rtems_rfs_bitmap_map_free (&group->inode_bitmap);
    printf (" %4d: base=%-7" PRIu32 " size=%-6zu blocks=%-5zu (%3zu%%) inode=%-5zu (%3zu%%)\n",
            g, group->base, group->size,
            blocks, (blocks * 100)  / group->size,
            inodes, (inodes * 100) / fs->group_inodes);
  }

  rtems_rfs_shell_unlock_rfs (fs);

  return 0;
}


void
rtems_rfs_shell_usage (const char* arg)
{
  printf ("%s: RFS debugger\n", arg);
  printf ("  %s [-hl] <path> <command>\n", arg);
  printf ("   where:\n");
  printf ("     path:    Path to the mounted RFS file system\n");
  printf ("     command: A debugger command. See -l for a list plus help.\n");
  printf ("     -h:      This help\n");
  printf ("     -l:      The debugger command list.\n");
}

int
rtems_shell_debugrfs (int argc, char *argv[])
{
  const rtems_rfs_shell_cmd table[] =
  {
    { "block", rtems_rfs_shell_block,
      "Display the contents of a block, block <bno>, block <bno>..<bno>" },
    { "data", rtems_rfs_shell_data,
      "Display file system data, data" },
    { "dir", rtems_rfs_shell_dir,
      "Display a block as a table for directory entrie, dir <bno>" },
    { "group", rtems_rfs_shell_group,
      "Display the group data of a file system, group, group <group>, group <start> <end>" },
    { "inode", rtems_rfs_shell_inode,
      "Display an inode, inode <ino>, inode> <ino>..<ino>" }
  };

  int arg;
  int t;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] != '-')
      break;

    switch (argv[arg][1])
    {
      case 'h':
        rtems_rfs_shell_usage (argv[0]);
        return 0;
      case 'l':
        printf ("%s: commands are:\n", argv[0]);
        for (t = 0; t < (sizeof (table) / sizeof (const rtems_rfs_shell_cmd)); t++)
          printf ("  %s\t\t%s\n", table[t].name, table[t].help);
        return 0;
      default:
        printf ("error: unknown option: %s\n", argv[arg]);
        return 1;
    }
  }

  if ((argc - arg) < 2)
    printf ("error: you need at least a path and command, try %s -h\n", argv[0]);
  else
  {
    rtems_rfs_file_system* fs;
    if (rtems_rfs_get_fs (argv[arg], &fs) == 0)
    {
      for (t = 0; t < (sizeof (table) / sizeof (const rtems_rfs_shell_cmd)); t++)
        if (strcmp (argv[arg + 1], table[t].name) == 0)
          return table[t].handler (fs, argc - 2, argv + 2);
      printf ("error: command not found: %s\n", argv[arg + 1]);
    }
  }

  return 1;
}

int
rtems_shell_rfs_format (int argc, char* argv[])
{
  rtems_rfs_format_config config;
  const char*             driver = NULL;
  int                     arg;

  memset (&config, 0, sizeof (rtems_rfs_format_config));

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'v':
          config.verbose = true;
          break;

        case 's':
          arg++;
          if (arg >= argc)
          {
            printf ("error: block size needs an argument\n");
            return 1;
          }
          config.block_size = strtoul (argv[arg], 0, 0);
          break;

        case 'b':
          arg++;
          if (arg >= argc)
          {
            printf ("error: group block count needs an argument\n");
            return 1;
          }
          config.group_blocks = strtoul (argv[arg], 0, 0);
          break;

        case 'i':
          arg++;
          if (arg >= argc)
          {
            printf ("error: group inode count needs an argument\n");
            return 1;
          }
          config.group_inodes = strtoul (argv[arg], 0, 0);
          break;

        case 'I':
          config.initialise_inodes = true;
          break;

        case 'o':
          arg++;
          if (arg >= argc)
          {
            printf ("error: inode percentage overhead needs an argument\n");
            return 1;
          }
          config.inode_overhead = strtoul (argv[arg], 0, 0);
          break;

        default:
          printf ("error: invalid option: %s\n", argv[arg]);
          return 1;
      }
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else
      {
        printf ("error: only one driver name allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  if (!driver) {
    printf ("error: no driver name provided\n");
    return 1;
  }

  if (rtems_rfs_format (driver, &config) < 0)
  {
    printf ("error: format of %s failed: %s\n",
            driver, strerror (errno));
    return 1;
  }

  return 0;
}
