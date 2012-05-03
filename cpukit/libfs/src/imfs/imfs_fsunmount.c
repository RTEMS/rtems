/*
 *  IMFS Initialization
 *
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

#include "imfs.h"

/*
 *  IMFS_fsunmount
 */

#define jnode_get_control( jnode ) \
  (&jnode->info.directory.Entries)

#define jnode_has_no_children( jnode )  \
  rtems_chain_is_empty( jnode_get_control( jnode ) )

#define jnode_has_children( jnode ) \
  ( ! jnode_has_no_children( jnode ) )

#define jnode_get_first_child( jnode ) \
    ((IMFS_jnode_t *)( rtems_chain_head( jnode_get_control( jnode ) )->next))

void IMFS_fsunmount(
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

   loc = temp_mt_entry->mt_fs_root->location;
   jnode = (IMFS_jnode_t *)loc.node_access;

   /*
    *  Set this to null to indicate that it is being unmounted.
    */

   temp_mt_entry->mt_fs_root->location.node_access = NULL;

   do {
     next = jnode->Parent;
     loc.node_access = (void *)jnode;
     IMFS_Set_handlers( &loc );

     if ( !IMFS_is_directory( jnode ) || jnode_has_no_children( jnode ) ) {
        result = IMFS_rmnod( NULL, &loc );
        if ( result != 0 )
	  rtems_fatal_error_occurred( 0xdeadbeef );
        IMFS_node_destroy( jnode );
        jnode = next;
     }
     if ( jnode != NULL ) {
       if ( IMFS_is_directory( jnode ) ) {
         if ( jnode_has_children( jnode ) )
           jnode = jnode_get_first_child( jnode );
       }
     }
   } while (jnode != NULL);
}
