/*
 *  open() - POSIX 1003.1 5.3.1 - Open a File
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <fcntl.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include <unistd.h>

/*
 *  Returns file descriptor on success or -1 and errno set to one of the
 *  following:
 *
 *    EACCESS  - Seach permission is denied on a component of the path prefix,
 *               or the file exists and the permissions specified by the
 *               flags are denied, or the file does not exist and write
 *               permission is denied for the parent directory of the file
 *               to be created, or O_TRUNC is specified and write permission
 *               is denied.
 *    EEXIST   - O_CREAT and O_EXCL are set and the named file exists.
 *    EINTR    - The open( operation was interrupted by a signal.
 *    EINVAL   - This implementation does not support synchronized IO for this
 *               file.
 *    EISDIR   - The named file is a directory and the flags argument 
 *               specified write or read/write access.
 *    EMFILE   - Too many file descriptors are in used by this process.
 *    ENAMETOOLONG - 
 *               The length of the path exceeds PATH_MAX or a pathname
 *               component is longer than NAME_MAX while POSIX_NO_TRUNC
 *               is in effect.
 *    ENFILE   - Too many files are open in the system.
 *    ENOENT   - O_CREAT is not set and and the anmed file does not exist,
 *               or O_CREAT is set and eitehr the path prefix does not exist
 *               or the path argument points to an empty string.
 *    ENOSPC   - The directory or file system that would contain the new file
 *               cannot be extended.
 *    ENOTDIR  - A component of the path prefix is not a directory.
 *    ENXIO    - O_NONBLOCK is set, the named file is a FIFO, O_WRONLY is
 *               set, and no process has the file open for reading.
 *    EROFS    - The named file resides on a read-only file system and either
 *               O_WRONLY, O_RDWR, O_CREAT (if the file does not exist), or
 *               O_TRUNC is set in the flags argument.
 */

int open(
  const char   *pathname,
  int           flags,
  ...
)
{
  va_list                             ap;
  int                                 mode;
  int                                 rc;
  rtems_libio_t                      *iop = 0;
  int                                 status;
  rtems_filesystem_location_info_t    loc;
  rtems_filesystem_location_info_t   *loc_to_free = NULL;
  int                                 eval_flags;


  /*
   * Set the Evaluation flags 
   */

  eval_flags = 0;
  status = flags + 1;
  if ( ( status & _FREAD ) == _FREAD )
    eval_flags |= RTEMS_LIBIO_PERMS_READ;
  if ( ( status & _FWRITE ) == _FWRITE )
    eval_flags |= RTEMS_LIBIO_PERMS_WRITE;

  
  va_start(ap, flags);

  mode = va_arg( ap, int );

  /*
   * NOTE: This comment is OBSOLETE.  The proper way to do this now
   *       would be to support a magic mounted file system.
   *
   *             Additional external I/O handlers would be supported by adding
   *             code to pick apart the pathname appropriately. The networking
   *             code does not require changes here since network file
   *             descriptors are obtained using socket(), not open().
   */

  /* allocate a file control block */
  iop = rtems_libio_allocate();
  if ( iop == 0 ) {
    rc = ENFILE;
    goto done;
  }

  /*
   *  See if the file exists.
   */

  status = rtems_filesystem_evaluate_path(
     pathname, eval_flags, &loc, TRUE );

  if ( status == -1 ) {
    if ( errno != ENOENT ) {
      rc = errno;
      goto done;
    }

    /* If the file does not exist and we are not trying to create it--> error */
    if ( !(flags & O_CREAT) ) {
      rc = ENOENT;
      goto done;
    }

    /* Create the node for the new regular file */
    rc = mknod( pathname, S_IFREG | mode, 0LL );
    if ( rc ) {
      rc = errno;
      goto done;
    }

    /* Sanity check to see if the file name exists after the mknod() */
    status = rtems_filesystem_evaluate_path( pathname, 0x0, &loc, TRUE );
    if ( status != 0 ) {   /* The file did not exist */
      rc = EACCES;
      goto done;
    }

  } else if ((flags & (O_EXCL|O_CREAT)) == (O_EXCL|O_CREAT)) {
    /* We were trying to create a file that already exists */
    rc = EEXIST;
    loc_to_free = &loc;
    goto done;
  }

  loc_to_free = &loc;

  /*
   *  Fill in the file control block based on the loc structure
   *  returned by successful path evaluation.
   */

  iop->handlers   = loc.handlers;
  iop->file_info  = loc.node_access;
  iop->flags     |= rtems_libio_fcntl_flags( flags );
  iop->pathinfo   = loc;

  if ( !iop->handlers->open_h ) {
    rc = ENOTSUP;
    goto done; 
  }

  rc = (*iop->handlers->open_h)( iop, pathname, flags, mode );
  if ( rc )
    goto done;

  /*
   *  Optionally truncate the file.
   */

  if ( (flags & O_TRUNC) == O_TRUNC ) {
    rc = ftruncate( iop - rtems_libio_iops, 0 );
  }
    
  /*
   *  Single exit and clean up path.
   */

done:
  va_end(ap);

  if ( rc ) {
    if ( iop )
      rtems_libio_free( iop );
    if ( loc_to_free )
      rtems_filesystem_freenode( loc_to_free );
    rtems_set_errno_and_return_minus_one( rc );
  }

  return iop - rtems_libio_iops;
}

/*
 *  _open_r
 *
 *  This is the Newlib dependent reentrant version of open().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _open_r(
  struct _reent *ptr,
  const char    *buf,
  int            flags,
  int            mode
)
{
  return open( buf, flags, mode );
}
#endif
