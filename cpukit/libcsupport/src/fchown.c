/**
 *  @file
 *
 *  @brief Change Owner and Group of a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

int rtems_filesystem_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
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
        rv = (*mt_entry->ops->chown_h)( loc, owner, group );
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
 *  POSIX 1003.1b 5.6.5 - Change Owner and Group of a File
 */
int fchown( int fd, uid_t owner, gid_t group )
{
  int rv;
  rtems_libio_t *iop;

  rtems_libio_check_fd( fd );
  iop = rtems_libio_iop( fd );
  rtems_libio_check_is_open(iop);

  rtems_filesystem_instance_lock( &iop->pathinfo );

  rv = rtems_filesystem_chown( &iop->pathinfo, owner, group );

  rtems_filesystem_instance_unlock( &iop->pathinfo );

  return rv;
}
