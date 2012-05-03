/*
 *  mount()
 *
 *  Mange the mount table. You can iterate on mounts and file systems, as well
 *  as add and remove file systems not in the file system confiration table.
 *
 *  COPYRIGHT (c) Chris Johns <chrisj@rtems.org> 2010.
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#include <rtems/libio_.h>

typedef struct {
  rtems_chain_node node;
  rtems_filesystem_table_t entry;
} filesystem_node;

static RTEMS_CHAIN_DEFINE_EMPTY(filesystem_chain);

bool rtems_filesystem_iterate(
  rtems_per_filesystem_routine routine,
  void *routine_arg
)
{
  const rtems_filesystem_table_t *table_entry = &rtems_filesystem_table [0];
  rtems_chain_node *node = NULL;
  bool stop = false;

  while ( table_entry->type && !stop ) {
    stop = (*routine)( table_entry, routine_arg );
    ++table_entry;
  }

  if ( !stop ) {
    rtems_libio_lock();
    for (
      node = rtems_chain_first( &filesystem_chain );
      !rtems_chain_is_tail( &filesystem_chain, node ) && !stop;
      node = rtems_chain_next( node )
    ) {
      const filesystem_node *fsn = (filesystem_node *) node;

      stop = (*routine)( &fsn->entry, routine_arg );
    }
    rtems_libio_unlock();
  }

  return stop;
}

typedef struct {
  const char *type;
  rtems_filesystem_fsmount_me_t mount_h;
} find_arg;

static bool find_handler(const rtems_filesystem_table_t *entry, void *arg)
{
  find_arg *fa = arg;

  if ( strcmp( entry->type, fa->type ) != 0 ) {
    return false;
  } else {
    fa->mount_h = entry->mount_h;

    return true;
  }
}

rtems_filesystem_fsmount_me_t
rtems_filesystem_get_mount_handler(
  const char *type
)
{
  find_arg fa = {
    .type = type,
    .mount_h = NULL
  };

  if ( type != NULL ) {
    rtems_filesystem_iterate( find_handler, &fa );
  }

  return fa.mount_h;
}

int
rtems_filesystem_register(
  const char                    *type,
  rtems_filesystem_fsmount_me_t  mount_h
)
{
  size_t type_size = strlen(type) + 1;
  size_t fsn_size = sizeof( filesystem_node ) + type_size;
  filesystem_node *fsn = malloc( fsn_size );
  char *type_storage = (char *) fsn + sizeof( *fsn );

  if ( fsn == NULL )
    rtems_set_errno_and_return_minus_one( ENOMEM );

  memcpy(type_storage, type, type_size);
  fsn->entry.type = type_storage;
  fsn->entry.mount_h = mount_h;

  rtems_libio_lock();
  if ( rtems_filesystem_get_mount_handler( type ) == NULL ) {
    rtems_chain_append( &filesystem_chain, &fsn->node );
  } else {
    rtems_libio_unlock();
    free( fsn );

    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  rtems_libio_unlock();

  return 0;
}

int
rtems_filesystem_unregister(
  const char *type
)
{
  rtems_chain_node *node = NULL;

  if ( type == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  rtems_libio_lock();
  for (
    node = rtems_chain_first( &filesystem_chain );
    !rtems_chain_is_tail( &filesystem_chain, node );
    node = rtems_chain_next( node )
  ) {
    filesystem_node *fsn = (filesystem_node *) node;

    if ( strcmp( fsn->entry.type, type ) == 0 ) {
      rtems_chain_extract( node );
      free( fsn );
      rtems_libio_unlock();

      return 0;
    }
  }
  rtems_libio_unlock();

  rtems_set_errno_and_return_minus_one( ENOENT );
}
