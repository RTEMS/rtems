/*
 *  IMFS Linear File Handlers
 *
 *  This file contains the set of handlers used to process operations on
 *  IMFS linear memory file nodes.  Linear memory files are contiguous
 *  blocks of memory created from a TAR or other filesystem image.
 *  The blocks are nonwriteable and nonresizeable.
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

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 * linearfile_read
 *
 *  This routine processes the read() system call.
 */

int linearfile_read(
  rtems_libio_t *iop,
  void          *buffer,
  unsigned32     count
)
{
  IMFS_jnode_t   *the_jnode;
  unsigned char  *dest;
  unsigned char  *file_ptr;
  int            file_offset;


  the_jnode = iop->file_info;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_LINEAR_FILE );
  if ( the_jnode->type != IMFS_LINEAR_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  /*
   *  Error checks on arguments
   */

  dest = (unsigned char *)buffer;
  assert( dest );
  if ( !dest )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  Perform a simple memory copy.
   */

  if (count == 0)
     return(0);

  the_jnode = iop->file_info;
  file_ptr    = (unsigned char *)the_jnode->info.linearfile.direct;
  file_offset = (unsigned long)iop->offset;

  if (count > (the_jnode->info.linearfile.size - file_offset))
     count = the_jnode->info.linearfile.size - file_offset;

  memcpy(dest, &file_ptr[file_offset], count);

  return(count);
}


/*
 *  linearfile_lseek
 *
 *  This routine processes the lseek() system call.
 */

int linearfile_lseek(
  rtems_libio_t   *iop,
  off_t            offset,
  int              whence
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  if (iop->offset > the_jnode->info.linearfile.size)
    iop->offset = the_jnode->info.linearfile.size;

  return iop->offset;
}

