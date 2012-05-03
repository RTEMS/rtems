/*
  ------------------------------------------------------------------------

  Copyright Cybertec Pty Ltd, 2000
  All rights reserved Cybertec Pty Ltd, 2000

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at

  http://www.rtems.com/license/LICENSE.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  Set of helpers when creating a root file system. The root filesystem
  in RTEMS is the In Memory Filesystem (IMFS). We could copy an exiting
  filesystem to here, how-ever a number of files can have target
  specific initialisation info which we need to write.

 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <rtems/mkrootfs.h>
#include <rtems/libio.h>

/*
 * A table a list of names and their modes.
 */

typedef struct rtems_rootfs_dir_table
{
  const char *name;
  mode_t      mode;
} rtems_rootfs_dir_table;

/*
 * Table of directorys to make.
 */

static const rtems_rootfs_dir_table default_directories[] =
{
  { "/bin",     S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH },
  { "/etc",     S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH },
  { "/dev",     S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH },
  { "/usr/bin", S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH }
};

#define MKFILE_MODE (S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH)
#define MKDIR_MODE  (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

/*
 * Create enough files to support the networking stack.
 * Points to a table of strings.
 */

int
rtems_rootfs_file_append (const char *file,
                          mode_t     omode,
                          const int  line_cnt,
                          const char **lines)
{
  struct stat sb;
  int         fd;
  int         i;

  /*
   * See is a file exists. If it does not, create the
   * file and the path to the file.
   */

  fd = -1;

  if (stat(file, &sb))
  {
    if (errno == ENOENT)
    {
      /*
       * Get the path to the file if one exists and create the
       * path. If it exists nothing happens.
       */

      size_t i = strlen (file);

      while (i)
      {
        if (file[i] == '/')
        {
          char path[128];

          if (i >= sizeof path)
          {
            printf ("root fs, path too long `%s'\n", file);
            return -1;
          }

          strncpy (path, file, i);
          path[i] = '\0';

          if (rtems_mkdir (path, MKDIR_MODE))
            return -1;
          break;
        }
        i--;
      }

      if ((fd = open (file, O_CREAT | O_APPEND | O_WRONLY, omode)) < 0)
      {
        printf ("root fs, cannot create file `%s' : %s\n",
                file, strerror (errno));
        return -1;
      }
    }
  }

  if (fd < 0)
  {
    if ((fd = open (file, O_APPEND | O_WRONLY)) < 0)
    {
      printf ("root fs, cannot open file `%s' : %s\n",
              file, strerror (errno));
      return -1;
    }
  }

  for (i = 0; i < line_cnt; i++)
  {
    size_t len = strlen (lines[i]);

    if (len)
    {
      if (write (fd, lines[i], strlen (lines[i])) < 0)
      {
        close (fd);
        printf ("root fs, cannot write to `%s' : %s\n",
                file, strerror (errno));
        return -1;
      }
    }
  }

  return close (fd);
}

/*
 * Write hosts record.
 */

int
rtems_rootfs_append_host_rec (in_addr_t cip,
                              const char    *cname,
                              const char    *dname)
{
  char           buf[128];
  char           *bufp = buf;
  const char     *bufl[1];
  struct in_addr ip;

  ip.s_addr = cip;

  if (cname && strlen (cname))
  {
    snprintf (bufp, sizeof (buf), "%s\t\t%s", inet_ntoa (ip), cname);
    bufp += strlen (buf);

    if (dname && strlen (dname))
    {
      snprintf (bufp, sizeof (buf), "\t\t%s.%s", cname, dname);
      bufp += strlen (buf);
    }

    strcat (buf, "\n");

    bufl[0] = buf;

    if (rtems_rootfs_file_append ("/etc/hosts", MKFILE_MODE, 1, bufl) < 0)
      return -1;
  }
  else
  {
    printf ("rootfs hosts rec append, no cname supplied\n");
    return -1;
  }

  return 0;
}

/*
 * Create a root file system.
 */

int
rtems_create_root_fs (void)
{
  const char *lines[1];
  size_t      i;

  /*
   * Create the directories.
   */

  for (i = 0;
       i < (sizeof (default_directories) / sizeof (rtems_rootfs_dir_table));
       i++)
    if (rtems_mkdir (default_directories[i].name,
                            default_directories[i].mode))
      return -1;

  /*
   * The TCP/IP stack likes this one. If DNS does not work
   * use the host file.
   */

  lines[0] = "hosts,bind\n";

  if (rtems_rootfs_file_append ("/etc/host.conf", MKFILE_MODE, 1, lines))
    return -1;

  /*
   * Create a `/etc/hosts' file.
   */

  if (rtems_rootfs_append_host_rec (htonl (0x7f000001), "localhost", "localdomain"))
    return -1;

  return 0;
}
