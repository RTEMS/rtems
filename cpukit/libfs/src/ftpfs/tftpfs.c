/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplTFTPFS
 *
 * @brief This source file contains the implementation of
 *   the Trivial File Transfer Protocol (TFTP) file system.
 *
 * The code in this file handles the file system operations (such as
 * `mount()`, `open()`, `read()`, `write()`, `close()` etc.).
 * The networking part, i.e. the actual Trivial File Transfer Protocol
 * implementation, is realized in another file - the
 * @ref tftpDriver.c "TFTP client library".
 */

/*
 * Copyright (C) 1998 W. Eric Norum <eric@norum.ca>
 * Copyright (C) 2012, 2022 embedded brains GmbH & Co. KG
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
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <rtems/tftp.h>
#include <rtems/thread.h>

#include "tftp_driver.h"

/*
 * Flags for filesystem info.
 */
#define TFTPFS_VERBOSE (1 << 0)

/*
 * TFTP File system info.
 */
typedef struct tftpfs_info_s {
  uint32_t flags;
  rtems_mutex tftp_mutex;
  size_t nStreams;
  void ** volatile tftpStreams;
  tftp_net_config tftp_config;
} tftpfs_info_t;

#define tftpfs_info_mount_table(_mt) ((tftpfs_info_t*) ((_mt)->fs_info))
#define tftpfs_info_pathloc(_pl)     ((tftpfs_info_t*) ((_pl)->mt_entry->fs_info))
#define tftpfs_info_iop(_iop)        (tftpfs_info_pathloc (&((_iop)->pathinfo)))

/* Forward declarations */
static const rtems_filesystem_operations_table  rtems_tftp_ops;
static const rtems_filesystem_file_handlers_r   rtems_tftp_handlers;

static bool rtems_tftp_is_directory(
    const char *path,
    size_t pathlen
)
{
    return path [pathlen - 1] == '/';
}

/*
 * Return value:
 *   0   if options have been pracessed without error
 *   N+1 if parsing failed at position N
 */
ssize_t _Tftpfs_Parse_options(
  const char *option_str,
  tftp_net_config *tftp_config,
  uint32_t *flags
)
{
  const char *cur_pos = option_str;
  size_t verbose_len = strlen ("verbose");
  size_t rfc1350_len = strlen ("rfc1350");
  int len;

  while(cur_pos != NULL && *cur_pos != '\0') {
    if (strncmp (cur_pos, "verbose", verbose_len) == 0) {
      *flags |= TFTPFS_VERBOSE;
      len = (int) verbose_len;
    } else if (strncmp (cur_pos, "rfc1350", rfc1350_len) == 0) {
      tftp_config->options.block_size = TFTP_RFC1350_BLOCK_SIZE;
      tftp_config->options.window_size = TFTP_RFC1350_WINDOW_SIZE;
      len = (int) rfc1350_len;
    } else if (sscanf(
        cur_pos,
        "blocksize=%"SCNu16"%n",
        &tftp_config->options.block_size,
        &len
      ) == 1) {
    } else if (sscanf(
        cur_pos,
        "windowsize=%"SCNu16"%n",
        &tftp_config->options.window_size,
        &len
      ) == 1) {
    } else if (*cur_pos == ',') { /* skip surplus "," */
      len = 0;
    } else {
      return cur_pos - option_str + 1;
    }

    cur_pos += len;
    if (*cur_pos != ',' && *cur_pos != '\0') {
      return cur_pos - option_str + 1;
    }
    if (*cur_pos == ',') {
      cur_pos++;
    }
  }

  return 0;
}

int rtems_tftpfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  const char *device = mt_entry->dev;
  size_t devicelen = strlen (device);
  tftpfs_info_t *fs = NULL;
  char *root_path;
  size_t err_pos;
  int errno_store = ENOMEM;

  if (devicelen == 0) {
    root_path = malloc (1);
    if (root_path == NULL)
      goto error;
    root_path [0] = '\0';
  }
  else {
    root_path = malloc (devicelen + 2);
    if (root_path == NULL)
      goto error;

    root_path = memcpy (root_path, device, devicelen);
    root_path [devicelen] = '/';
    root_path [devicelen + 1] = '\0';
  }

  fs = malloc (sizeof (*fs));
  if (fs == NULL)
    goto error;
  fs->flags = 0;
  fs->nStreams = 0;
  fs->tftpStreams = 0;

  tftp_initialize_net_config (&fs->tftp_config);
  err_pos = _Tftpfs_Parse_options (data, &fs->tftp_config, &fs->flags);
  if (err_pos != 0) {
    printf(
      "TFTP FS: ERROR in mount options '%s'.\n"
        "TFTP FS: Cannot parse from this point: '%s'\n",
      ((char *) data),
      ((char *) data) + (err_pos - 1)
    );
    errno_store = EINVAL;
    goto error;
  }

  mt_entry->fs_info = fs;
  mt_entry->mt_fs_root->location.node_access = root_path;
  mt_entry->mt_fs_root->location.handlers = &rtems_tftp_handlers;
  mt_entry->ops = &rtems_tftp_ops;

  /*
   *  Now allocate a semaphore for mutual exclusion.
   *
   *  NOTE:  This could be in an fsinfo for this filesystem type.
   */

  rtems_mutex_init (&fs->tftp_mutex, "TFTPFS");

  return 0;

error:

  free (fs);
  free (root_path);

  rtems_set_errno_and_return_minus_one (errno_store);
}

/*
 * Clear the pointer to a stream
 */
static void
releaseStream (tftpfs_info_t *fs, size_t s)
{
    rtems_mutex_lock (&fs->tftp_mutex);
    fs->tftpStreams[s] = NULL;
    rtems_mutex_unlock (&fs->tftp_mutex);
}

static void
rtems_tftpfs_shutdown (rtems_filesystem_mount_table_entry_t* mt_entry)
{
  tftpfs_info_t *fs = tftpfs_info_mount_table (mt_entry);
  size_t         s;
  void          *tp;
  for (s = 0; s < fs->nStreams; s++) {
      tp = fs->tftpStreams[s];
      releaseStream (fs, s);
      _Tftp_Destroy(tp);
  }
  rtems_mutex_destroy (&fs->tftp_mutex);
  free (fs);
  free (mt_entry->mt_fs_root->location.node_access);
}

/*
 * Convert a path to canonical form
 */
static void
fixPath (char *path)
{
    char *inp, *outp, *base;

    outp = inp = path;
    base = NULL;
    for (;;) {
        if (inp[0] == '.') {
            if (inp[1] == '\0')
                break;
            if (inp[1] == '/') {
                inp += 2;
                continue;
            }
            if (inp[1] == '.') {
                if (inp[2] == '\0') {
                    if ((base != NULL) && (outp > base)) {
                        outp--;
                        while ((outp > base) && (outp[-1] != '/'))
                            outp--;
                    }
                    break;
                }
                if (inp[2] == '/') {
                    inp += 3;
                    if (base == NULL)
                        continue;
                    if (outp > base) {
                        outp--;
                        while ((outp > base) && (outp[-1] != '/'))
                            outp--;
                    }
                    continue;
                }
            }
        }
        if (base == NULL)
            base = inp;
        while (inp[0] != '/') {
            if ((*outp++ = *inp++) == '\0')
                return;
        }
        *outp++ = '/';
        while (inp[0] == '/')
            inp++;
    }
    *outp = '\0';
    return;
}

static void rtems_tftp_eval_path(rtems_filesystem_eval_path_context_t *self)
{
    int eval_flags = rtems_filesystem_eval_path_get_flags (self);

    if ((eval_flags & RTEMS_FS_MAKE) == 0) {
        int rw = RTEMS_FS_PERMS_READ | RTEMS_FS_PERMS_WRITE;

        if ((eval_flags & rw) != rw) {
            rtems_filesystem_location_info_t *currentloc =
                rtems_filesystem_eval_path_get_currentloc (self);
            char *current = currentloc->node_access;
            size_t currentlen = strlen (current);
            const char *path = rtems_filesystem_eval_path_get_path (self);
            size_t pathlen = rtems_filesystem_eval_path_get_pathlen (self);
            size_t len = currentlen + pathlen;

            rtems_filesystem_eval_path_clear_path (self);

            current = realloc (current, len + 1);
            if (current != NULL) {
                memcpy (current + currentlen, path, pathlen);
                current [len] = '\0';
                if (!rtems_tftp_is_directory (current, len)) {
                    fixPath (current);
                }
                currentloc->node_access = current;
            } else {
                rtems_filesystem_eval_path_error (self, ENOMEM);
            }
        } else {
            rtems_filesystem_eval_path_error (self, EINVAL);
        }
    } else {
        rtems_filesystem_eval_path_error (self, EIO);
    }
}

/*
 * The routine which does most of the work for the IMFS open handler
 */
static int rtems_tftp_open_worker(
    rtems_libio_t *iop,
    char          *full_path_name,
    int            oflag
)
{
    tftpfs_info_t        *fs;
    void                 *tp;
    size_t               s;
    char                 *cp1;
    char                 *remoteFilename;
    char                 *hostname;
    int                  err;

    /*
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);

    /*
     * Extract the host name component
     */
    if (*full_path_name == '/')
      full_path_name++;

    hostname = full_path_name;
    cp1 = strchr (full_path_name, ':');
    if (!cp1) {
        return EINVAL; /* No ':' in path: no hostname or no filename */
    } else {
        *cp1 = '\0';
        ++cp1;
    }

    /*
     * Extract file pathname component
     */
    if (*cp1 == '\0')
        return ENOENT;
    remoteFilename = cp1;

    /*
     * Establish the connection
     */
    err = tftp_open (
        hostname,
        remoteFilename,
        (oflag & O_ACCMODE) == O_RDONLY,
        &fs->tftp_config,
        &tp
    );
    if (err != 0) {
      return err;
    }

    /*
     * Find a free stream
     */
    rtems_mutex_lock (&fs->tftp_mutex);
    for (s = 0 ; s < fs->nStreams ; s++) {
        if (fs->tftpStreams[s] == NULL)
        break;
    }
    if (s == fs->nStreams) {
        /*
         * Reallocate stream pointers
         * Guard against the case where realloc() returns NULL.
         */
        void **np;

        np = realloc (fs->tftpStreams, ++fs->nStreams * sizeof *fs->tftpStreams);
        if (np == NULL) {
            rtems_mutex_unlock (&fs->tftp_mutex);
            tftp_close( tp );
            return ENOMEM;
        }
        fs->tftpStreams = np;
    }
    fs->tftpStreams[s] = tp;
    rtems_mutex_unlock (&fs->tftp_mutex);
    iop->data0 = s;
    iop->data1 = tp;

    return 0;
}

static int rtems_tftp_open(
    rtems_libio_t *iop,
    const char    *new_name,
    int            oflag,
    mode_t         mode
)
{
    tftpfs_info_t *fs;
    char          *full_path_name;
    int           err;

    full_path_name = iop->pathinfo.node_access;

    if (rtems_tftp_is_directory (full_path_name, strlen (full_path_name))) {
        rtems_set_errno_and_return_minus_one (ENOTSUP);
    }

    /*
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);

    if (fs->flags & TFTPFS_VERBOSE)
      printf ("TFTPFS: %s\n", full_path_name);

    err = rtems_tftp_open_worker (iop, full_path_name, oflag);
    if (err != 0) {
       rtems_set_errno_and_return_minus_one (err);
    }

    return 0;
}

/*
 * Read from a TFTP stream
 */
static ssize_t rtems_tftp_read(
    rtems_libio_t *iop,
    void          *buffer,
    size_t         count
)
{
    void *tp = iop->data1;
    ssize_t result = tftp_read (tp, buffer, count);

    if (result < 0) {
        rtems_set_errno_and_return_minus_one (-result);
    }
    return result;
}

/*
 * Close a TFTP stream
 */
static int rtems_tftp_close(
    rtems_libio_t *iop
)
{
    tftpfs_info_t     *fs;
    void              *tp = iop->data1;
    int                e = 0;

    /*
     * Get the file system info.
     */
    fs = tftpfs_info_iop (iop);

    if (!tp)
        rtems_set_errno_and_return_minus_one (EIO);

    releaseStream (fs, iop->data0);
    e = tftp_close (tp);
    if (e)
        rtems_set_errno_and_return_minus_one (e);
    return 0;
}

static ssize_t rtems_tftp_write(
    rtems_libio_t   *iop,
    const void      *buffer,
    size_t           count
)
{
    void *tp = iop->data1;
    ssize_t result = tftp_write (tp, buffer, count);

    if (result < 0) {
        rtems_set_errno_and_return_minus_one (-result);
    }
    return result;
}

/*
 * Dummy version to let fopen(xxxx,"w") work properly.
 */
static int rtems_tftp_ftruncate(
    rtems_libio_t   *iop RTEMS_UNUSED,
    off_t            count RTEMS_UNUSED
)
{
    return 0;
}

static int rtems_tftp_fstat(
    const rtems_filesystem_location_info_t *loc,
    struct stat                            *buf
)
{
    const char *path = loc->node_access;
    size_t pathlen = strlen (path);

    buf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO
        | (rtems_tftp_is_directory (path, pathlen) ? S_IFDIR : S_IFREG);

    return 0;
}

static int rtems_tftp_clone(
    rtems_filesystem_location_info_t *loc
)
{
    int rv = 0;

    loc->node_access = strdup (loc->node_access);

    if (loc->node_access == NULL) {
        errno = ENOMEM;
        rv = -1;
    }

    return rv;
}

static void rtems_tftp_free_node_info(
    const rtems_filesystem_location_info_t *loc
)
{
    free (loc->node_access);
}

static bool rtems_tftp_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  return strcmp (a->node_access, b->node_access) == 0;
}

static const rtems_filesystem_operations_table  rtems_tftp_ops = {
    .lock_h = rtems_filesystem_default_lock,
    .unlock_h = rtems_filesystem_default_unlock,
    .eval_path_h = rtems_tftp_eval_path,
    .link_h = rtems_filesystem_default_link,
    .are_nodes_equal_h = rtems_tftp_are_nodes_equal,
    .mknod_h = rtems_filesystem_default_mknod,
    .rmnod_h = rtems_filesystem_default_rmnod,
    .fchmod_h = rtems_filesystem_default_fchmod,
    .chown_h = rtems_filesystem_default_chown,
    .clonenod_h = rtems_tftp_clone,
    .freenod_h = rtems_tftp_free_node_info,
    .mount_h = rtems_filesystem_default_mount,
    .unmount_h = rtems_filesystem_default_unmount,
    .fsunmount_me_h = rtems_tftpfs_shutdown,
    .utimens_h = rtems_filesystem_default_utimens,
    .symlink_h = rtems_filesystem_default_symlink,
    .readlink_h = rtems_filesystem_default_readlink,
    .rename_h = rtems_filesystem_default_rename,
    .statvfs_h = rtems_filesystem_default_statvfs
};

static const rtems_filesystem_file_handlers_r rtems_tftp_handlers = {
   .open_h = rtems_tftp_open,
   .close_h = rtems_tftp_close,
   .read_h = rtems_tftp_read,
   .write_h = rtems_tftp_write,
   .ioctl_h = rtems_filesystem_default_ioctl,
   .lseek_h = rtems_filesystem_default_lseek,
   .fstat_h = rtems_tftp_fstat,
   .ftruncate_h = rtems_tftp_ftruncate,
   .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
   .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
   .fcntl_h = rtems_filesystem_default_fcntl,
   .kqfilter_h = rtems_filesystem_default_kqfilter,
   .mmap_h = rtems_filesystem_default_mmap,
   .poll_h = rtems_filesystem_default_poll,
   .readv_h = rtems_filesystem_default_readv,
   .writev_h = rtems_filesystem_default_writev
};
