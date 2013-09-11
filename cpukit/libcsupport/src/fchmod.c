/**
 *  @file
 *
 *  @brief Change File Modes
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>

#include <rtems/libio_.h>

int rtems_filesystem_chmod(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
)
{
  const rtems_filesystem_mount_table_entry_t *mt_entry = loc->mt_entry;
  int rv;

  if ( mt_entry->writeable || rtems_filesystem_location_is_null( loc ) ) {
    struct stat st;

    memset( &st, 0, sizeof(st) );

    rv = (*loc->handlers->fstat_h)( loc, &st );
    if ( rv == 0 ) {
      uid_t uid = geteuid();

      if ( uid == 0 || st.st_uid == uid ) {
        mode_t mask = S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX;

        mode = (st.st_mode & ~mask) | (mode & mask);

        rv = (*mt_entry->ops->fchmod_h)( loc, mode );
      } else {
        errno = EPERM;
        rv = -1;
      }
    }
  } else {
    errno = EROFS;
    rv = -1;
  }

  return rv;
}

/**
 *  POSIX 1003.1b 5.6.4 - Change File Modes
 */
int fchmod( int fd, mode_t mode )
{
  int rv;
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  rtems_filesystem_instance_lock( &iop->pathinfo );

  rv = rtems_filesystem_chmod( &iop->pathinfo, mode );

  rtems_filesystem_instance_unlock( &iop->pathinfo );

  return rv;
}
