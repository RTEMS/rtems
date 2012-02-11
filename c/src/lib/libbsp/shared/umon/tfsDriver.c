/*
 *  tfsDriver.c - MicroMonitor TFS Hookup to RTEMS FS
 *
 *  Initial release: Oct 1, 2004   by Ed Sutter
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
 *
 *  $Id$
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

/*
 * Root node_access value
 * By using the address of a local static variable
 * we ensure a unique value for this identifier.
 */
#define ROOT_NODE_ACCESS    (&tfs_mutex)

/* Number of streams open at the same time
 */
static rtems_id tfs_mutex;

extern rtems_filesystem_operations_table  rtems_tfs_ops;
extern rtems_filesystem_file_handlers_r   rtems_tfs_handlers;

/* Direct copy from the IMFS.  Look at this.
 */

rtems_filesystem_limits_and_options_t rtems_tfs_limits_and_options = {
  5,   /* link_max */
  6,   /* max_canon */
  7,   /* max_input */
  255, /* name_max */
  255, /* path_max */
  2,   /* pipe_buf */
  1,   /* posix_async_io */
  2,   /* posix_chown_restrictions */
  3,   /* posix_no_trunc */
  4,   /* posix_prio_io */
  5,   /* posix_sync_io */
  6    /* posix_vdisable */
};

static int rtems_tfs_mount_me(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  rtems_status_code  sc;

  mt_entry->mt_fs_root.handlers = &rtems_tfs_handlers;
  mt_entry->mt_fs_root.ops      = &rtems_tfs_ops;

  /* We have no TFS specific data to maintain.  This filesystem
   * may only be mounted once. And we maintain no real filesystem
   * nodes, so there is no real root.
   */

  mt_entry->fs_info                = NULL;
  mt_entry->mt_fs_root.node_access = ROOT_NODE_ACCESS;

  /* These need to be looked at for full POSIX semantics.
   */

  mt_entry->pathconf_limits_and_options = rtems_tfs_limits_and_options;


  /*  Now allocate a semaphore for mutual exclusion.
   *  NOTE:  This could be in an fsinfo for this filesystem type.
   */

  sc = rtems_semaphore_create (
    rtems_build_name('U', 'M', 'O', 'N'),
    1,
    RTEMS_FIFO |
      RTEMS_BINARY_SEMAPHORE |
      RTEMS_NO_INHERIT_PRIORITY |
      RTEMS_NO_PRIORITY_CEILING |
      RTEMS_LOCAL,
    0,
    &tfs_mutex);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_set_errno_and_return_minus_one( ENOMEM );

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

static int rtems_tfs_evaluate_for_make(
  const char                         *path,
  rtems_filesystem_location_info_t   *pathloc,
  const char                        **name
)
{
  pathloc->node_access = NULL;
  rtems_set_errno_and_return_minus_one( EIO );
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

static int rtems_tfs_eval_path(
  const char                        *pathname,
  size_t                             pathnamelen,
  int                                flags,
  rtems_filesystem_location_info_t  *pathloc
)
{
  pathloc->handlers    = &rtems_tfs_handlers;

  /*
   * Hack to provide the illusion of directories inside the TFS file system.
   * Paths ending in a / are assumed to be directories.
   */
  if (pathname[strlen(pathname)-1] == '/') {
    int isRelative = (pathloc->node_access != ROOT_NODE_ACCESS);
    char *cp;

    /*
     * Reject attempts to open() directories
     */
    if (flags & RTEMS_LIBIO_PERMS_RDWR)
      rtems_set_errno_and_return_minus_one( EISDIR );
    if (isRelative) {
      cp = malloc (strlen(pathloc->node_access)+strlen(pathname)+1);
      if (cp == NULL)
        rtems_set_errno_and_return_minus_one( ENOMEM );
      strcpy (cp, pathloc->node_access);
      strcat (cp, pathname);
    } else {
      cp = strdup (pathname);
      if (cp == NULL)
        rtems_set_errno_and_return_minus_one( ENOMEM );
    }
    fixPath (cp);
    pathloc->node_access = cp;
    return 0;
  }
  if (pathloc->node_access != ROOT_NODE_ACCESS)
    pathloc->node_access = 0;

  /*
   * Reject it if it's not read-only or write-only.
   */
  flags &= RTEMS_LIBIO_PERMS_READ | RTEMS_LIBIO_PERMS_WRITE;
  if ((flags != RTEMS_LIBIO_PERMS_READ) && (flags != RTEMS_LIBIO_PERMS_WRITE) )
    rtems_set_errno_and_return_minus_one( EINVAL );
  return 0;
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
  uint32_t       flags,
  uint32_t       mode
)
{
  static int beenhere = 0;
  long flagmode;
  int  tfdidx, tfd;
  struct tfdinfo *tip;
  char *buf, *fstr, *istr, *bstr, pathcopy[TFSNAMESIZE*3+1];

  if (RTEMS_TFS_DEBUG)
    printk("_open_r(%s,0x%" PRIx32 ",0x%" PRIx32 ")\n",path,flags,mode);

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
  if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
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
  switch(flags & O_ACCMODE) {
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
      printk("_open_r(): flag 0x%" PRIx32 " not supported\n",flags);
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
  uint32_t       flags,
  uint32_t       mode
)
{
  char *full_path_name;
  char *s1;
  int err;

  if (RTEMS_TFS_DEBUG)
    printk("rtems_tfs_open(%s)\n",new_name);

  /*
   * Tack the `current directory' on to relative paths.
   * We know that the current directory ends in a / character.
   */
  if (*new_name == '/') {
    /*
     * Skip the TFS filesystem prefix.
     */
    int len = strlen (TFS_PATHNAME_PREFIX);
    if (strncmp (new_name, TFS_PATHNAME_PREFIX, len))
      return ENOENT;
    new_name += len;
    s1 = "";
  } else {
    s1 = rtems_filesystem_current.node_access;
  }
  full_path_name = malloc (strlen (s1) + strlen (new_name) + 1);
  if (full_path_name == NULL)
    return ENOMEM;
  strcpy (full_path_name, s1);
  strcat (full_path_name, new_name);
  fixPath (full_path_name);
  err = rtems_tfs_open_worker (iop, full_path_name, flags, mode);
  free (full_path_name);
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
  int  fd, ret;

  fd = (int) iop->data0;
  ret = mon_tfsctrl(cmd,(long)buf,0);
  return(-1);
}

static rtems_filesystem_node_types_t rtems_tfs_node_type(
  rtems_filesystem_location_info_t *pathloc
)
{
  if ((pathloc->node_access == NULL) ||
      (pathloc->node_access == ROOT_NODE_ACCESS))
    return RTEMS_FILESYSTEM_MEMORY_FILE;
  return RTEMS_FILESYSTEM_DIRECTORY;
}

static int rtems_tfs_free_node_info(

rtems_filesystem_location_info_t *pathloc)
{
  if (pathloc->node_access && (pathloc->node_access != ROOT_NODE_ACCESS)) {
    free (pathloc->node_access);
    pathloc->node_access = NULL;
  }
  return 0;
}


rtems_filesystem_operations_table  rtems_tfs_ops = {
  rtems_tfs_eval_path,                    /* eval_path */
  rtems_tfs_evaluate_for_make,            /* evaluate_for_make */
  rtems_filesystem_default_link,          /* link */
  rtems_filesystem_default_unlink,        /* unlink */
  rtems_tfs_node_type,                    /* node_type */
  rtems_filesystem_default_mknod,         /* mknod */
  rtems_filesystem_default_chown,         /* chown */
  rtems_tfs_free_node_info,               /* freenodinfo */
  rtems_filesystem_default_mount,         /* mount */
  rtems_tfs_mount_me,                     /* initialize */
  rtems_filesystem_default_unmount,       /* unmount */
  rtems_filesystem_default_fsunmount,     /* fsunmount */
  rtems_filesystem_default_utime,         /* utime */
  rtems_filesystem_default_evaluate_link, /* evaluate_link */
  rtems_filesystem_default_symlink,       /* symlink */
  rtems_filesystem_default_readlink,      /* readlin */
  rtems_filesystem_default_rename,        /* rename */
  rtems_filesystem_default_statvfs        /* statvfs */
};

rtems_filesystem_file_handlers_r rtems_tfs_handlers = {
  rtems_tfs_open,                     /* open */
  rtems_tfs_close,                    /* close */
  rtems_tfs_read,                     /* read */
  rtems_tfs_write,                    /* write */
  rtems_tfs_ioctl,                    /* ioctl */
  rtems_tfs_lseek,                    /* lseek */
  rtems_filesystem_default_fstat,     /* fstat */
  rtems_filesystem_default_fchmod,    /* fchmod */
  rtems_tfs_ftruncate,                /* ftruncate */
  rtems_filesystem_default_fsync,     /* fsync */
  rtems_filesystem_default_fdatasync, /* fdatasync */
  rtems_filesystem_default_fcntl,     /* fcntl */
  rtems_filesystem_default_rmnod      /* rmnod */
};
