/*
 *  IMFS Initialization
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

#include <sys/types.h>         /* for mkdir */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>

#include "imfs.h"
#include <rtems/libio_.h>

#if defined(IMFS_DEBUG)
#include <stdio.h>
#endif

/*
 *  IMFS_fsunmount
 */

#define jnode_get_control( jnode ) \
  (&jnode->info.directory.Entries)

#define jnode_has_no_children( jnode )  \
  Chain_Is_empty( jnode_get_control( jnode ) )

#define jnode_has_children( jnode ) \
  ( ! jnode_has_no_children( jnode ) )

#define jnode_get_first_child( jnode ) \
    ((IMFS_jnode_t *)( Chain_Head( jnode_get_control( jnode ) )->next))

int IMFS_fsunmount(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
   IMFS_jnode_t                     *jnode;
   IMFS_jnode_t                     *next;
   rtems_filesystem_location_info_t loc;       
   int                              result = 0;

   /* 
    * Traverse tree that starts at the mt_fs_root and deallocate memory 
    * associated memory space
    */
    
   jnode = (IMFS_jnode_t *)temp_mt_entry->mt_fs_root.node_access;
   loc = temp_mt_entry->mt_fs_root;
   
   /*
    *  Set this to null to indicate that it is being unmounted.
    */

   temp_mt_entry->mt_fs_root.node_access = NULL;

   do {
     next = jnode->Parent;
     loc.node_access = (void *)jnode;
     IMFS_Set_handlers( &loc );

     if ( jnode->type != IMFS_DIRECTORY ) {
        result = IMFS_unlink( &loc );
        if (result != 0)
          return -1;
        jnode = next;
     } else if ( jnode_has_no_children( jnode ) ) {
        result = IMFS_unlink( &loc );
        if (result != 0)
          return -1;
        jnode = next;
     }
     if ( jnode != NULL ) {
       if ( jnode->type == IMFS_DIRECTORY ) {
         if ( jnode_has_children( jnode ) )
           jnode = jnode_get_first_child( jnode );
       }
     }
   } while (jnode != NULL);

   return 0;
}




