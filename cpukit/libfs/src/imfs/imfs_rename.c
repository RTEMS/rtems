/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup IMFS
 *
 * @brief IMFS Rename
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <rtems/imfsimpl.h>

#include <string.h>
#include <stdlib.h>

typedef struct {
  IMFS_node_control Base;
  const IMFS_node_control *replaced;
  char name[ RTEMS_ZERO_LENGTH_ARRAY ];
} IMFS_renamed_control;

static void IMFS_restore_replaced_control( IMFS_jnode_t *node )
{
  const IMFS_node_control *base;
  IMFS_renamed_control    *control;

  base = RTEMS_DECONST( IMFS_node_control *, node->control );
  control = (IMFS_renamed_control *) base;
  node->control = control->replaced;
  free( control );
}

static void IMFS_renamed_destroy( IMFS_jnode_t *node )
{
  IMFS_restore_replaced_control( node );
  ( *node->control->node_destroy )( node );
}

int IMFS_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
)
{
  IMFS_jnode_t         *node;
  IMFS_jnode_t         *new_parent;
  IMFS_renamed_control *control;

  /*
   * FIXME: Due to insufficient checks we can create inaccessible nodes with
   * this operation.
   */

  node = oldloc->node_access;
  new_parent = newparentloc->node_access;

  if ( node->Parent == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( namelen >= IMFS_NAME_MAX ) {
    rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
  }

  control = malloc( sizeof( *control ) + namelen );
  if ( control == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  memcpy( control->name, name, namelen );

  if ( node->control->node_destroy == IMFS_renamed_destroy ) {
    IMFS_restore_replaced_control( node );
  }

  control->Base = *node->control;
  control->Base.node_destroy = IMFS_renamed_destroy;
  control->replaced = node->control;
  node->control = &control->Base;
  node->name = control->name;
  node->namelen = namelen;

  IMFS_remove_from_directory( node );
  IMFS_add_to_directory( new_parent, node );
  IMFS_update_ctime( node );

  return 0;
}
