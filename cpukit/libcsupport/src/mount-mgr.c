/*
 *  mount()
 *
 *  Mange the mount table. You can iterate on mounts and file systems, as well
 *  as add and remove file systems not in the file system confiration table.
 *
 *  COPYRIGHT (c) Chris Johns <chrisj@rtems.org> 2010.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <rtems/chain.h>
#include <rtems/seterr.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <rtems/libio_.h>

/*
 * External defined by confdefs.h or the user.
 */
extern const rtems_filesystem_table_t configuration_filesystem_table[];

/*
 * Points to a list of filesystems added at runtime.
 */
extern rtems_chain_control *rtems_filesystem_table;

/*
 * Mount table list.
 */
extern rtems_chain_control rtems_filesystem_mount_table_control;
extern bool                rtems_filesystem_mount_table_control_init;

/*
 * Get the first entry in the filesystem table.
 */
const rtems_filesystem_table_t*
rtems_filesystem_table_first(
  void
)
{
  /*
   * We can assume this because it is the root file system.
   */
  return &configuration_filesystem_table[0];
}

/*
 * Get the next entry in the file system table.
 */
const rtems_filesystem_table_t*
rtems_filesystem_table_next(
  rtems_filesystem_table_t *entry
)
{
  const rtems_filesystem_table_t* fs;

  fs = rtems_filesystem_table_first( );
  
  while ( fs->type && ( fs != entry ) )
    ++fs;
  
  if ( fs->type ) {
    ++fs;
    if ( fs->type )
      return fs;
  }

  if ( rtems_filesystem_table ) {
    rtems_chain_node* node;
    for (node = rtems_chain_first( rtems_filesystem_table );
         !rtems_chain_is_tail( rtems_filesystem_table, node);
         node = rtems_chain_next( node )) {
      rtems_filesystem_table_node_t* tnode;
      tnode = (rtems_filesystem_table_node_t*) node;
      if ( entry == &tnode->entry ) {
        node = rtems_chain_next( node );
        if ( !rtems_chain_is_tail( rtems_filesystem_table, node ) ) {
          tnode = (rtems_filesystem_table_node_t*) node;
          return &tnode->entry;
        }
      }
    }
  }
  
  return NULL;
}

/*
 * Get the first entry in the mount table.
 */
rtems_filesystem_mount_table_entry_t*
rtems_filesystem_mounts_first(
  void
)
{
  rtems_filesystem_mount_table_entry_t* entry = NULL;
  if ( rtems_filesystem_mount_table_control_init ) {
    if ( !rtems_chain_is_empty( &rtems_filesystem_mount_table_control ) )
      entry = (rtems_filesystem_mount_table_entry_t*)
        rtems_chain_first( &rtems_filesystem_mount_table_control );
  }
  return entry;
}

/*
 * Get the next entry in the mount table.
 */
rtems_filesystem_mount_table_entry_t*
rtems_filesystem_mounts_next(
  rtems_filesystem_mount_table_entry_t *entry
)
{
  if ( !rtems_filesystem_mount_table_control_init || !entry )
    return NULL;
  return (rtems_filesystem_mount_table_entry_t*) rtems_chain_next( &entry->Node );
}

/*
 * Register a file system.
 */
int
rtems_filesystem_register(
  const char                    *type,
  rtems_filesystem_fsmount_me_t  mount_h
)
{
  rtems_filesystem_table_node_t *fs;
  if ( !rtems_filesystem_table ) {
    rtems_filesystem_table = malloc( sizeof( rtems_chain_control ) );
    if ( !rtems_filesystem_table )
      rtems_set_errno_and_return_minus_one( ENOMEM );
    rtems_chain_initialize_empty ( rtems_filesystem_table );
  }
  fs = malloc( sizeof( rtems_filesystem_table_node_t ) );
  if ( !fs )
    rtems_set_errno_and_return_minus_one( ENOMEM );
  fs->entry.type = strdup( type );
  if ( !fs->entry.type ) {
    free( fs );
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }    
  fs->entry.mount_h = mount_h;
  rtems_chain_append( rtems_filesystem_table, &fs->node );
  return 0;
}

/*
 * Unregister a file system.
 */
int
rtems_filesystem_unregister(
  const char *type
)
{
  if ( rtems_filesystem_table ) {
    rtems_chain_node *node;
    for (node = rtems_chain_first( rtems_filesystem_table );
         !rtems_chain_is_tail( rtems_filesystem_table, node );
         node = rtems_chain_next( node ) ) {
      rtems_filesystem_table_node_t *fs;
      fs = (rtems_filesystem_table_node_t*) node;
      if ( strcmp( fs->entry.type, type ) == 0 ) {
        rtems_chain_extract( node );
        free( (void*) fs->entry.type );
        free( fs );
        return 0;
      }
    }
  }
  rtems_set_errno_and_return_minus_one( ENOENT );
}
