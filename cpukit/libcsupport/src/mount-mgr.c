/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  mount()
 *
 *  Mange the mount table. You can iterate on mounts and file systems, as well
 *  as add and remove file systems not in the file system confiration table.
 *
 *  COPYRIGHT (c) Chris Johns <chrisj@rtems.org> 2010.
 *
 *  Copyright (c) 2010 embedded brains GmbH & Co. KG
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
  rtems_chain_control *chain = &filesystem_chain;
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
      node = rtems_chain_first( chain );
      !rtems_chain_is_tail( chain, node ) && !stop;
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
  rtems_chain_control *chain = &filesystem_chain;
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
    rtems_chain_initialize_node( &fsn->node );
    rtems_chain_append_unprotected( chain, &fsn->node );
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
  rtems_chain_control *chain = &filesystem_chain;
  rtems_chain_node *node = NULL;

  if ( type == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  rtems_libio_lock();
  for (
    node = rtems_chain_first( chain );
    !rtems_chain_is_tail( chain, node );
    node = rtems_chain_next( node )
  ) {
    filesystem_node *fsn = (filesystem_node *) node;

    if ( strcmp( fsn->entry.type, type ) == 0 ) {
      rtems_chain_extract_unprotected( node );
      free( fsn );
      rtems_libio_unlock();

      return 0;
    }
  }
  rtems_libio_unlock();

  rtems_set_errno_and_return_minus_one( ENOENT );
}
