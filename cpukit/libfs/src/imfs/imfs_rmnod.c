/*
 *  IMFS Node Removal Handler
 *
 *  This file contains the handler used to remove a node when a file type
 *  does not require special actions.
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

#include "imfs.h"

void IMFS_create_orphan( IMFS_jnode_t *jnode )
{
  if ( jnode->Parent != NULL ) {
    rtems_chain_extract( &jnode->Node );
    jnode->Parent = NULL;
  }

  --jnode->st_nlink;

  IMFS_update_ctime( jnode );
}

void IMFS_check_node_remove( IMFS_jnode_t *jnode )
{
  if ( !rtems_libio_is_file_open( jnode ) && jnode->st_nlink < 1 ) {
    if ( rtems_filesystem_current.node_access == jnode )
       rtems_filesystem_current.node_access = NULL;

    switch ( jnode->type ) {
      case IMFS_MEMORY_FILE:
        IMFS_memfile_remove( jnode );
        break;
      case IMFS_SYM_LINK:
        free( jnode->info.sym_link.name );
        break;
      default:
        break;
    }

    free( jnode );
  }
}

/*
 *  IMFS_rmnod
 */

int IMFS_rmnod(
  rtems_filesystem_location_info_t  *parent_pathloc, /* IN */
  rtems_filesystem_location_info_t  *pathloc         /* IN */
)
{
  IMFS_jnode_t *jnode = (IMFS_jnode_t *) pathloc->node_access;

  IMFS_create_orphan( jnode );
  IMFS_check_node_remove( jnode );

  return 0;
}
