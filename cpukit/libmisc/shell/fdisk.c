/**
 * @file
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009
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
#include <stdlib.h>
#include <string.h>

#include <rtems/bdpart.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#define RTEMS_BDPART_SHELL_ERROR( fmt, ...) \
  do { \
    printf( "error: " fmt "\n", ##__VA_ARGS__); \
    return -1; \
  } while (0)

#define RTEMS_BDPART_SHELL_ERROR_SC( sc, fmt, ...) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    printf( "error: " fmt ": %s\n", ##__VA_ARGS__, rtems_status_text( sc)); \
    return -1; \
  }

typedef enum {
  RTEMS_BDPART_SHELL_FS,
  RTEMS_BDPART_SHELL_N,
  RTEMS_BDPART_SHELL_MBR,
  RTEMS_BDPART_SHELL_GPT
} rtems_bdpart_shell_state;

static const char rtems_bdpart_shell_usage [] =
  "disk format and utility functions\n"
  "\n"
  "fdisk DISK_NAME\n"
  "\tprints the partition table\n"
  "\n"
  "fdisk DISK_NAME [FS N1 [N2 ... ]] ... [write] [FORMAT]\n"
  "\tcreates a new partition table\n"
  "\n"
  "fdisk DISK_NAME register\n"
  "\tcreates a logical disk for each partition of the disk\n"
  "\n"
  "fdisk DISK_NAME unregister\n"
  "\tdeletes the logical disks associated with the partitions of the disk\n"
  "\n"
  "fdisk DISK_NAME mount\n"
  "\tmounts the file system of each partition of the disk\n"
  "\n"
  "fdisk DISK_NAME unmount\n"
  "\tunmounts the file system of each partition of the disk\n"
  "\n"
  "option values:\n"
  "\tDISK_NAME: absolute path to disk device like '/dev/hda'\n"
  "\tN*: weights of positive integers\n"
  "\tFS: 0x00 ... 0xff, fat12, fat16, fat32, data\n"
  "\twrite: write the new partition table to the disk\n"
  "\tFORMAT: mbr [[no]dos], gpt";

static int rtems_bdpart_shell_main( int argc, char **argv)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdpart_format format;
  rtems_bdpart_partition pt [RTEMS_BDPART_PARTITION_NUMBER_HINT];
  unsigned dist [RTEMS_BDPART_PARTITION_NUMBER_HINT];
  size_t count = RTEMS_BDPART_PARTITION_NUMBER_HINT;
  const char *disk_name = NULL;
  const char *mount_base = "/mnt";
  bool do_create = false;
  bool do_read = false;
  bool do_write = false;
  bool do_register = false;
  bool do_unregister = false;
  bool do_mount = false;
  bool do_unmount = false;
  bool do_dump = false;

  if (argc < 2) {
    puts( rtems_bdpart_shell_usage);
    return -1;
  }

  disk_name = argv [1];

  if (argc == 2) {
    do_read = true;
    do_dump = true;
  } else if (argc == 3) {
    /* Check option */
    if (strcmp( argv [2], "register") == 0) {
      do_read = true;
      do_register = true;
    } else if (strcmp( argv [2], "unregister") == 0) {
      do_read = true;
      do_unregister = true;
    } else if (strcmp( argv [2], "mount") == 0) {
      do_read = true;
      do_mount = true;
    } else if (strcmp( argv [2], "unmount") == 0) {
      do_read = true;
      do_unmount = true;
    } else {
      RTEMS_BDPART_SHELL_ERROR( "unexpected option: %s", argv [2]);
    }
  } else {
    rtems_bdpart_shell_state state = RTEMS_BDPART_SHELL_FS;
    uint8_t current_type = RTEMS_BDPART_MBR_FAT_32;
    size_t i = 0;
    int ai = 0;

    /* Clear partition table */
    memset( pt, 0, sizeof( pt));

    /* Default format */
    format.type = RTEMS_BDPART_FORMAT_MBR;
    format.mbr.disk_id = 0;
    format.mbr.dos_compatibility = true;

    for (ai = 2; ai < argc; ++ai) {
      char *s = argv [ai];
      unsigned long v = 0;
      char *end = NULL;

      if (strlen( s) == 0) {
        continue;
      } else if (strcmp( s, "write") == 0) {
        do_write = true;
        continue;
      } else if (strcmp( s, "mbr") == 0) {
        state = RTEMS_BDPART_SHELL_MBR;
        format.type = RTEMS_BDPART_FORMAT_MBR;
        continue;
      } else if (strcmp( s, "gpt") == 0) {
        state = RTEMS_BDPART_SHELL_GPT;
        format.type = RTEMS_BDPART_FORMAT_GPT;
        continue;
      }

      switch (state) {
        case RTEMS_BDPART_SHELL_FS:
          v = strtoul( s, &end, 16);
          if (*end == '\0') {
            if (v <= 0xffU) {
              current_type = (uint8_t) v;
            } else {
              RTEMS_BDPART_SHELL_ERROR( "type value out of range: %s", argv [ai]);
            }
          } else if (strcmp( s, "fat32") == 0) {
            current_type = RTEMS_BDPART_MBR_FAT_32;
          } else if (strcmp( s, "data") == 0) {
            current_type = RTEMS_BDPART_MBR_DATA;
          } else if (strcmp( s, "fat16") == 0) {
            current_type = RTEMS_BDPART_MBR_FAT_16;
          } else if (strcmp( s, "fat12") == 0) {
            current_type = RTEMS_BDPART_MBR_FAT_12;
          } else {
            RTEMS_BDPART_SHELL_ERROR( "unexpected option: %s", argv [ai]);
          }
          state = RTEMS_BDPART_SHELL_N;
          break;
        case RTEMS_BDPART_SHELL_N:
          v = strtoul( s, &end, 10);
          if (*end == '\0') {
            rtems_bdpart_to_partition_type( current_type, pt [i].type);
            dist [i] = v;
            if (i < count) {
              ++i;
            } else {
              RTEMS_BDPART_SHELL_ERROR( "too many partitions");
            }
          } else {
            --ai;
            state = RTEMS_BDPART_SHELL_FS;
          }
          break;
        case RTEMS_BDPART_SHELL_MBR:
          if (strcmp( s, "dos") == 0) {
            format.mbr.dos_compatibility = true;
          } else if (strcmp( s, "nodos") == 0) {
            format.mbr.dos_compatibility = false;
          } else {
            RTEMS_BDPART_SHELL_ERROR( "unexpected option: %s", argv [ai]);
          }
          break;
        case RTEMS_BDPART_SHELL_GPT:
          RTEMS_BDPART_SHELL_ERROR( "unexpected option: %s", argv [ai]);
        default:
          RTEMS_BDPART_SHELL_ERROR( "fdisk interal error");
      }
    }

    /* Partition number */
    count = i;

    /* Actions */
    do_create = true;
    do_dump = true;
    if (do_write) {
      do_read = true;
    }
  }

  if (do_create) {
    /* Create partitions */
    sc = rtems_bdpart_create( disk_name, &format, pt, dist, count);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot create partitions for '%s'", disk_name);
  }

  if (do_write) {
    /* Write partitions */
    sc = rtems_bdpart_write( disk_name, &format, pt, count);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot write partitions to '%s'", disk_name);
  }

  if (do_read) {
    /* Read partitions */
    count = RTEMS_BDPART_PARTITION_NUMBER_HINT;
    sc = rtems_bdpart_read( disk_name, &format, pt, &count);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot read partitions from '%s'", disk_name);
  }

  if (do_register) {
    /* Register partitions */
    sc = rtems_bdpart_register( disk_name, pt, count);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot register partitions of '%s'", disk_name);
  }

  if (do_unregister) {
    /* Unregister partitions */
    sc = rtems_bdpart_unregister( disk_name, pt, count);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot unregister partitions of '%s'", disk_name);
  }

  if (do_mount) {
    /* Mount partitions */
    sc = rtems_bdpart_mount( disk_name, pt, count, mount_base);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot mount partitions of '%s' to '%s'", disk_name, mount_base);
  }

  if (do_unmount) {
    /* Unmount partitions */
    sc = rtems_bdpart_unmount( disk_name, pt, count, mount_base);
    RTEMS_BDPART_SHELL_ERROR_SC( sc, "cannot unmount partitions of '%s'", disk_name);
  }

  if (do_dump) {
    /* Dump partitions */
    rtems_bdpart_dump( pt, count);
  }

  return 0;
}

struct rtems_shell_cmd_tt rtems_shell_FDISK_Command = {
  .name = "fdisk",
  .usage = rtems_bdpart_shell_usage,
  .topic = "files",
  .command = rtems_bdpart_shell_main,
  .alias = NULL,
  .next = NULL
};
