/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief Unmount this Instance of IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

#define jnode_get_control( jnode ) \
  (&((IMFS_directory_t *) jnode)->Entries)

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
