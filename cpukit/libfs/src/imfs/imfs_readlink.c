/*
 *  IMFS_readlink
 *
 *  The following rouine puts the symblic links destination name into 
 *  buff.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

int IMFS_readlink(
  rtems_filesystem_location_info_t  *loc,
 char                               *buf,         /* OUT */
 size_t                             bufsize    
)
{
  IMFS_jnode_t      *node;
  int                i;
  
  node = loc->node_access;

  if ( node->type != IMFS_SYM_LINK )
    rtems_set_errno_and_return_minus_one( EINVAL );

  for( i=0; ((i<bufsize) && (node->info.sym_link.name[i] != '\0')); i++ )
    buf[i] = node->info.sym_link.name[i];

  return i;
}
