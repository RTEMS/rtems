/*
 *  tfsDriver.c - MicroMonitor TFS Hookup to RTEMS FS
 *
 *  Initial release: Oct 1, 2004   by Ed Sutter
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  This code was derived from the tftpDriver.c code written by
 *  W. Eric Norum, which was apparently derived from the IMFS driver.
 *
 *  This code was reformatted by Joel Sherrill from OAR Corporation and
 *  Fernando Nicodemos <fgnicodemos@terra.com.br> from NCB - Sistemas
 *  Embarcados Ltda. (Brazil) to be more compliant with RTEMS coding
 *  standards and to eliminate C++ style comments.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>
#include <rtems/bspIo.h>
#include <rtems/umon.h>

#include <umon/tfs.h>
#include <umon/monlib.h>

#ifdef RTEMS_TFS_DRIVER_DEBUG
#define RTEMS_TFS_DEBUG 1
#else
#define RTEMS_TFS_DEBUG 0
#endif

#define MAXFILESIZE 0x4000
#define MAXTFDS  15

/* Define these for thread safety...
 */
#ifndef newlib_tfdlock
#define newlib_tfdlock()
#endif

#ifndef newlib_tfdunlock
#define newlib_tfdunlock()
#endif

/* TFS file descriptor info:
 */
struct tfdinfo {
  int   inuse;
  int   tfd;
  char *buf;
  char  name[TFSNAMESIZE+1];
  char  info[TFSNAMESIZE+1];
} tfdtable[MAXTFDS];

/*
 * Pathname prefix
 */
char TFS_PATHNAME_PREFIX[128];

static const rtems_filesystem_operations_table  rtems_tfs_ops;
static const rtems_filesystem_file_handlers_r   rtems_tfs_handlers;

static bool rtems_tfs_is_directory(
    const char *path,
    size_t pathlen
)
{
    return path [pathlen - 1] == '/';
}

static int rtems_tfs_mount_me(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  char *root_path = strdup("/");

  if (root_path == NULL) {
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }

  mt_entry->ops = &rtems_tfs_ops;
  mt_entry->mt_fs_root->location.handlers = &rtems_tfs_handlers;
  mt_entry->mt_fs_root->location.node_access = root_path;

  return 0;
}

/* Initialize the TFS-RTEMS file system
 */
int rtems_initialize_tfs_filesystem(
  const char *path
)
{
  int status;

  if (!path) {
    printk( "TFS: No mount point specified\n" );
    return -1;
  }

  strncpy( TFS_PATHNAME_PREFIX, path, sizeof(TFS_PATHNAME_PREFIX) );

  status = mkdir( TFS_PATHNAME_PREFIX, S_IRWXU | S_IRWXG | S_IRWXO );
  if ( status == -1 ) {
    printk( "TFS: Unable to mkdir %s\n", TFS_PATHNAME_PREFIX );
    return status;
  }

  if (rtems_filesystem_register( "tfs", rtems_tfs_mount_me ) < 0)
    return -1;

  status = mount( "umon", TFS_PATHNAME_PREFIX, "tfs", RTEMS_FILESYSTEM_READ_WRITE, NULL);

  if (status < 0) {
    printk( "TFS: Unable to mount on %s\n", TFS_PATHNAME_PREFIX );
    perror("TFS mount failed");
  }

  return(status);
}

/*
 * Convert a path to canonical form
 */
static void fixPath(char *path)
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
}

static void rtems_tfs_eval_path(rtems_filesystem_eval_path_context_t *self)
{
  int eval_flags = rtems_filesystem_eval_path_get_flags(self);

  if ((eval_flags & RTEMS_FS_MAKE) == 0) {
    int rw = RTEMS_FS_PERMS_READ | RTEMS_FS_PERMS_WRITE;

    if ((eval_flags & rw) != rw) {
      rtems_filesystem_location_info_t *currentloc =
        rtems_filesystem_eval_path_get_currentloc(self);
      char *current = currentloc->node_access;
      size_t currentlen = strlen(current);
      const char *path = rtems_filesystem_eval_path_get_path(self);
      size_t pathlen = rtems_filesystem_eval_path_get_pathlen(self);
      size_t len = currentlen + pathlen;

      rtems_filesystem_eval_path_clear_path(self);

      current = realloc(current, len + 1);
      if (current != NULL) {
        memcpy(current + currentlen, path, pathlen);
        current [len] = '\0';
        if (!rtems_tfs_is_directory(current, len)) {
          fixPath (current);
        }
        currentloc->node_access = current;
      } else {
        rtems_filesystem_eval_path_error(self, ENOMEM);
      }
    } else {
      rtems_filesystem_eval_path_error(self, EINVAL);
    }
  } else {
    rtems_filesystem_eval_path_error(self, EIO);
  }
}

/*
 * The routine which does most of the work for the IMFS open handler
 * The full_path_name here is all text AFTER the TFS_PATHNAME_PREFIX
 * string, so if the filename is "/TFS/abc", the full_path_name string
 * is "abc"...
 *
 * Attempts to remap the incoming flags to TFS equivalent.
 * Its not a perfect mapping, but gets pretty close.
 * A comma-delimited path is supported to allow the user
 * to specify TFS-stuff (flag string, info string, and a buffer).
 * For example:
 *  abc,e,script,0x400000
 *  This is a file called "abc" that will have the TFS 'e' flag
 *  and the TFS info field of "script".  The storage buffer is
 *  supplied by the user at 0x400000.
 */
static int rtems_tfs_open_worker(
  rtems_libio_t *iop,
  char          *path,
  int            oflag,
  mode_t         mode
)
{
  static int beenhere = 0;
  long flagmode;
  int  tfdidx, tfd;
  struct tfdinfo *tip;
  char *buf, *fstr, *istr, *bstr, pathcopy[TFSNAMESIZE*3+1];

  if (RTEMS_TFS_DEBUG)
    printk("_open_r(%s,0x%" PRIx32 ",0x%" PRIx32 ")\n",path,oflag,mode);

  if (!beenhere) {
    newlib_tfdlock();
    for(tfdidx=0;tfdidx<MAXTFDS;tfdidx++)
      tfdtable[tfdidx].inuse = 0;

    tfdtable[0].inuse = 1;    /* fake entry for stdin */
    tfdtable[1].inuse = 1;    /* fake entry for stdout */
    tfdtable[2].inuse = 1;    /* fake entry for stderr */
    newlib_tfdunlock();
    beenhere = 1;
  }

  istr = fstr = bstr = buf = (char *)0;

  /* Copy the incoming path to a local array so that we can safely
   * modify the string...
    */
  if (strlen(path) > TFSNAMESIZE*3) {
    return(ENAMETOOLONG);
  }
  strcpy(pathcopy,path);

  /* The incoming string may have commas that are used to delimit the
   * name from the TFS flag string, TFS info string and buffer.
   * Check for the commas and test for maximum string length...
   */
  fstr = strchr(pathcopy,',');
  if (fstr)  {
    *fstr++ = 0;
    istr = strchr(fstr,',');
    if (istr) {
      *istr++ = 0;
      bstr = strchr(istr,',');
      if (bstr)
        *bstr++ = 0;
    }
  }
  if (strlen(pathcopy) > TFSNAMESIZE) {
    return(ENAMETOOLONG);
  }
  if (istr) {
    if (strlen(istr) > TFSNAMESIZE) {
      return(ENAMETOOLONG);
    }
  }

  /* If O_EXCL and O_CREAT are set, then fail if the file exists...
   */
  if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
    if (mon_tfsstat((char *)pathcopy)) {
      return(EEXIST);
    }
  }

  /* Only a few flag combinations are supported...
   * O_RDONLY             Simple read-only
   * O_WRONLY | O_APPEND       Each write starts at end of file
   * O_WRONLY | O_TRUNC       If file exists, truncate it
   * O_WRONLY | O_CREAT       Create if it doesn't exist
   * O_WRONLY | O_CREAT | O_EXCL   Fail if file exists
   */
  switch(oflag & O_ACCMODE) {
    case O_RDONLY:
      flagmode = TFS_RDONLY;
      break;
    case O_WRONLY|O_APPEND:
      flagmode = TFS_APPEND;
      break;
    case O_WRONLY|O_TRUNC:
    case O_WRONLY|O_CREAT|O_TRUNC:
      mon_tfsunlink((char *)pathcopy);
      flagmode = TFS_CREATE|TFS_APPEND;
      break;
    case O_WRONLY|O_CREAT:
    case O_WRONLY|O_CREAT|O_APPEND:
      flagmode = TFS_CREATE|TFS_APPEND;
      break;
    case O_RDWR:
    case O_WRONLY|O_CREAT|O_EXCL:
      flagmode = TFS_CREATE|TFS_APPEND;
      break;
    default:
      printk("_open_r(): flag 0x%i not supported\n",oflag);
      return(ENOTSUP);
  }

  /* Find an open slot in our tfd table:
   */
  newlib_tfdlock();
  for(tfdidx=0;tfdidx<MAXTFDS;tfdidx++) {
    if (tfdtable[tfdidx].inuse == 0)
      break;
  }
  if (tfdidx == MAXTFDS) {
    newlib_tfdunlock();
    return(EMFILE);
  }
  tip = &tfdtable[tfdidx];
  tip->inuse = 1;
  newlib_tfdunlock();

  /* If file is opened for something other than O_RDONLY, then
   * we need to allocate a buffer for the file..
   * WARNING: It is the user's responsibility to make sure that
   * the file size does not exceed this buffer.  Note that the
   * buffer may be specified as part of the comma-delimited path.
   */
  if (flagmode == TFS_RDONLY) {
    buf = (char *)0;
  } else {
    if (bstr)
      buf = (char *)strtol(bstr,0,0);
    else
      buf = malloc(MAXFILESIZE);
    if (!buf) {
      newlib_tfdlock();
      tip->inuse = 0;
      newlib_tfdunlock();
      return(ENOMEM);
    }
  }

  /* Deal with tfs flags and tfs info fields if necessary:
   */
  if (fstr) {
    long bflag;

    bflag = mon_tfsctrl(TFS_FATOB,(long)fstr,0);
    if (bflag == -1) {
      return(EINVAL);
    }
    flagmode |= bflag;
  }

  if (istr)
    strcpy(tip->info,istr);
  else
    tip->info[0] = 0;

  tfd = mon_tfsopen((char *)pathcopy,flagmode,buf);
  if (tfd >= 0) {
    tip->tfd = tfd;
    tip->buf = buf;
    strcpy(tip->name,pathcopy);
    iop->data0 = (uint32_t)tfdidx;
    return(0);
  } else {
    printk("%s: %s\n",pathcopy,
      (char *)mon_tfsctrl(TFS_ERRMSG,tfd,0));
  }

  if (buf)
    free(buf);

  newlib_tfdlock();
  tip->inuse = 0;
  newlib_tfdunlock();
  return(EINVAL);
}

/*
 * The IMFS open handler
 */
static int rtems_tfs_open(
  rtems_libio_t *iop,
  const char    *new_name,
  int            oflag,
  mode_t         mode
)
{
  char *full_path_name;
  int err;

  full_path_name = iop->pathinfo.node_access;

  if (RTEMS_TFS_DEBUG)
    printk("rtems_tfs_open(%s)\n",full_path_name);

  if (rtems_tfs_is_directory(full_path_name, strlen(full_path_name))) {
    rtems_set_errno_and_return_minus_one (ENOTSUP);
  }

  err = rtems_tfs_open_worker (iop, full_path_name, oflag, mode);
  if (err != 0) {
     rtems_set_errno_and_return_minus_one (err);
  }

  return err;
}

/*
 * Read from an open TFS file...
 */
static ssize_t rtems_tfs_read(
  rtems_libio_t *iop,
  void          *buffer,
  uint32_t       count
)
{
  int  ret, fd;

  fd = (int) iop->data0;

  if (RTEMS_TFS_DEBUG)
    printk("_read_r(%d,%" PRId32 ")\n",fd,count);

  if ((fd < 3) || (fd >= MAXTFDS))
    return(EBADF);

  ret = mon_tfsread(tfdtable[fd].tfd,buffer,count);
  if (ret == TFSERR_EOF)
    ret = 0;

  return(ret);
}

/*
 * Close the open tfs file.
 */
static int rtems_tfs_close(
  rtems_libio_t *iop
)
{
  int  fd;
  char *info;
  struct tfdinfo *tip;

  fd = (int)iop->data0;

  if (RTEMS_TFS_DEBUG)
    printk("rtems_tfs_close(%d)\n",fd);

  if ((fd < 3) || (fd >= MAXTFDS)) {
    rtems_set_errno_and_return_minus_one (EBADF);
  }

  tip = &tfdtable[fd];

  if (tip->info[0])
    info = tip->info;
  else
    info = (char *)0;

  mon_tfsclose(tip->tfd,info);

  if (tip->buf)
    free(tip->buf);

  newlib_tfdlock();
  tip->inuse = 0;
  newlib_tfdunlock();
  return RTEMS_SUCCESSFUL;
}

static ssize_t rtems_tfs_write(
  rtems_libio_t *iop,
  const void    *buffer,
  uint32_t       count
)
{
  int  ret, fd;

  fd = (int) iop->data0;

  if (RTEMS_TFS_DEBUG)
    printk("rtems_tfs_write(%d,%" PRId32" )\n",fd,count);

  if ((fd <= 0) || (fd >= MAXTFDS)) {
    rtems_set_errno_and_return_minus_one (EBADF);
  }

  ret = mon_tfswrite(tfdtable[fd].tfd,(char *)buffer,count);
  if (ret < 0)
    return(-1);

  return(ret);
}

static off_t rtems_tfs_lseek(
  rtems_libio_t *iop,
  off_t          offset,
  int            whence
)
{
  int ret, fd;

  fd = (int) iop->data0;

  if (RTEMS_TFS_DEBUG)
    printk("rtems_tfs_lseek(%d,%ld,%d)\n",fd,(long)offset,whence);

  switch (whence) {
    case SEEK_END:
      printk("rtems_tfs_lseek doesn't support SEEK_END\n");
      return(-1);
    case SEEK_CUR:
      whence = TFS_CURRENT;
      break;
    case SEEK_SET:
      whence = TFS_BEGIN;
      break;
  }
  ret = mon_tfsseek(tfdtable[fd].tfd,offset,whence);

  if (ret < 0)
    return(-1);

  return (off_t)ret;
}

/*
 *
 */
static int rtems_tfs_ftruncate(
  rtems_libio_t *iop,
  off_t          count
)
{
  int ret, fd;

  fd = (int) iop->data0;
  ret = mon_tfstruncate(tfdtable[fd].tfd,count);
  if (ret != TFS_OKAY)
    return(-1);

  return(0);
}

static int rtems_tfs_ioctl(
  rtems_libio_t *iop,
  uint32_t       cmd,
  void          *buf
)
{
  int ret;

  ret = mon_tfsctrl(cmd,(long)buf,0);
  if (ret != TFS_OKAY)
    return(-1);

  return(0);
}

static rtems_filesystem_node_types_t rtems_tfs_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
  const char *path = loc->node_access;
  size_t pathlen = strlen(path);

  return rtems_tfs_is_directory(path, pathlen) ?
    RTEMS_FILESYSTEM_DIRECTORY
      : RTEMS_FILESYSTEM_MEMORY_FILE;
}

static int rtems_tfs_clone_node_info(
  rtems_filesystem_location_info_t *loc
)
{
  int rv = 0;

  loc->node_access = strdup(loc->node_access);

  if (loc->node_access == NULL) {
    errno = ENOMEM;
    rv = -1;
  }

  return rv;
}

static void rtems_tfs_free_node_info(
  const rtems_filesystem_location_info_t *loc
)
{
  free(loc->node_access);
}

static bool rtems_tfs_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  return strcmp(a->node_access, b->node_access) == 0;
}

static const rtems_filesystem_operations_table  rtems_tfs_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = rtems_tfs_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_tfs_are_nodes_equal,
  .node_type_h = rtems_tfs_node_type,
  .mknod_h = rtems_filesystem_default_mknod,
  .rmnod_h = rtems_filesystem_default_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = rtems_tfs_clone_node_info,
  .freenod_h = rtems_tfs_free_node_info,
  .mount_h = rtems_filesystem_default_mount,
  .fsmount_me_h = rtems_tfs_mount_me,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_filesystem_default_fsunmount,
  .utime_h = rtems_filesystem_default_utime,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const rtems_filesystem_file_handlers_r rtems_tfs_handlers = {
  .open_h = rtems_tfs_open,
  .close_h = rtems_tfs_close,
  .read_h = rtems_tfs_read,
  .write_h = rtems_tfs_write,
  .ioctl_h = rtems_tfs_ioctl,
  .lseek_h = rtems_tfs_lseek,
  .fstat_h = rtems_filesystem_default_fstat,
  .ftruncate_h = rtems_tfs_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl
};
