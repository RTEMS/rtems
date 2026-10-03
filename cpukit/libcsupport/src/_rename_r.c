/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of _rename_r().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012, 2026 embedded brains GmbH & Co. KG
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

#if defined( RTEMS_NEWLIB ) && !defined( HAVE__RENAME_R )

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <rtems/libio_.h>

static void get_name( const char *path, const char **name, size_t *namelen )
{
  size_t pathlen = strlen( path );
  size_t i = pathlen;

  while ( i > 0 && !rtems_filesystem_is_delimiter( path[ i - 1 ] ) ) {
    --i;
  }

  *name = &path[ i ];
  *namelen = pathlen - i;
}

static bool is_dot_or_dotdot( const char *name, size_t namelen )
{
  return rtems_filesystem_is_current_directory( name, namelen ) ||
         rtems_filesystem_is_parent_directory( name, namelen );
}

static bool is_directory( const rtems_filesystem_location_info_t *loc )
{
  return S_ISDIR( rtems_filesystem_location_type( loc ) );
}

static bool are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  return a->mt_entry == b->mt_entry &&
         ( *a->mt_entry->ops->are_nodes_equal_h )( a, b );
}

static int is_same_file(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  struct stat st_a;
  struct stat st_b;
  int         rv;

  memset( &st_a, 0, sizeof( st_a ) );
  memset( &st_b, 0, sizeof( st_b ) );

  rv = ( *a->handlers->fstat_h )( a, &st_a );
  if ( rv == 0 ) {
    rv = ( *b->handlers->fstat_h )( b, &st_b );
  }

  if ( rv == 0 ) {
    rv = st_a.st_dev == st_b.st_dev && st_a.st_ino == st_b.st_ino;
  }

  return rv;
}

/*
 * Returns 1, if the directory is the location or one of its ancestors in the
 * file system instance, 0 if not, and -1 on error.
 */
static int is_directory_ancestor_of(
  const rtems_filesystem_location_info_t *dir,
  const rtems_filesystem_location_info_t *loc
)
{
  rtems_filesystem_location_info_t     copy;
  rtems_filesystem_global_location_t  *global;
  rtems_filesystem_eval_path_context_t ctx;
  int                                  rv;

  rtems_filesystem_location_clone( &copy, loc );
  global = rtems_filesystem_location_transform_to_global( &copy );
  rtems_filesystem_eval_path_start_with_root_and_current(
    &ctx,
    ".",
    1,
    0,
    &rtems_filesystem_root,
    &global
  );

  while ( true ) {
    const rtems_filesystem_location_info_t *currentloc = &ctx.currentloc;

    if ( rtems_filesystem_location_is_null( currentloc ) ) {
      rv = -1;
      break;
    }

    if ( are_nodes_equal( currentloc, dir ) ) {
      rv = 1;
      break;
    }

    if (
      currentloc->mt_entry != dir->mt_entry ||
      rtems_filesystem_location_is_instance_root( currentloc ) ||
      are_nodes_equal( currentloc, &ctx.rootloc->location )
    ) {
      rv = 0;
      break;
    }

    ctx.path = "..";
    ctx.pathlen = 2;
    rtems_filesystem_eval_path_continue( &ctx );
  }

  rtems_filesystem_eval_path_cleanup( &ctx );
  rtems_filesystem_global_location_release( global, false );

  return rv;
}

/*
 * A directory with the S_ISVTX flag set allows a process to remove or rename
 * an entry only if the process owns the entry or the directory.
 */
static bool may_remove_entry(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  struct stat parent_st;
  struct stat st;
  uid_t       uid;

  memset( &parent_st, 0, sizeof( parent_st ) );
  (void) ( *parentloc->handlers->fstat_h )( parentloc, &parent_st );

  if ( ( parent_st.st_mode & S_ISVTX ) == 0 ) {
    return true;
  }

  memset( &st, 0, sizeof( st ) );
  (void) ( *loc->handlers->fstat_h )( loc, &st );
  uid = rtems_current_user_env_get()->euid;

  return uid == st.st_uid || uid == parent_st.st_uid;
}

static int check_rename(
  const rtems_filesystem_location_info_t *old_parentloc,
  const rtems_filesystem_location_info_t *old_loc,
  const char                             *old_name,
  size_t                                  old_namelen,
  const rtems_filesystem_location_info_t *new_parentloc,
  const rtems_filesystem_location_info_t *new_loc,
  const char                             *new_name,
  size_t                                  new_namelen
)
{
  bool old_is_dir;
  int  rv;

  if (
    is_dot_or_dotdot( old_name, old_namelen ) || new_namelen == 0 ||
    is_dot_or_dotdot( new_name, new_namelen )
  ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( rtems_filesystem_location_is_instance_root( old_loc ) ) {
    rtems_set_errno_and_return_minus_one( EBUSY );
  }

  old_is_dir = is_directory( old_loc );

  if ( new_loc != NULL ) {
    bool new_is_dir;

    rv = is_same_file( old_loc, new_loc );
    if ( rv != 0 ) {
      return rv;
    }

    new_is_dir = is_directory( new_loc );

    if ( old_is_dir && !new_is_dir ) {
      rtems_set_errno_and_return_minus_one( ENOTDIR );
    }

    if ( !old_is_dir && new_is_dir ) {
      rtems_set_errno_and_return_minus_one( EISDIR );
    }

    if ( rtems_filesystem_location_is_instance_root( new_loc ) ) {
      rtems_set_errno_and_return_minus_one( EBUSY );
    }
  }

  if ( old_is_dir ) {
    rv = is_directory_ancestor_of( old_loc, new_parentloc );
    if ( rv != 0 ) {
      if ( rv > 0 ) {
        errno = EINVAL;
      }

      return -1;
    }
  }

  if (
    !may_remove_entry( old_parentloc, old_loc ) ||
    ( new_loc != NULL && !may_remove_entry( new_parentloc, new_loc ) )
  ) {
    rtems_set_errno_and_return_minus_one( EPERM );
  }

  return 0;
}

/**
 *  POSIX 1003.1b - 5.3.4 - Rename a file
 */
int _rename_r(
  struct _reent *ptr RTEMS_UNUSED,
  const char        *old,
  const char *new
)
{
  int                                  rv = 0;
  rtems_filesystem_eval_path_context_t old_ctx;
  int                                  old_eval_flags = 0;
  rtems_filesystem_location_info_t     old_parentloc;
  int old_parent_eval_flags = RTEMS_FS_PERMS_WRITE | RTEMS_FS_PERMS_EXEC |
                              RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t
    *old_currentloc = rtems_filesystem_eval_path_start_with_parent(
      &old_ctx,
      old,
      old_eval_flags,
      &old_parentloc,
      old_parent_eval_flags
    );
  rtems_filesystem_eval_path_context_t new_ctx;
  int                                  new_eval_flags = RTEMS_FS_MAKE;
  rtems_filesystem_location_info_t     new_parentloc;
  int new_parent_eval_flags = RTEMS_FS_PERMS_WRITE | RTEMS_FS_PERMS_EXEC |
                              RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t
    *new_currentloc = rtems_filesystem_eval_path_start_with_parent(
      &new_ctx,
      new,
      new_eval_flags,
      &new_parentloc,
      new_parent_eval_flags
    );

  rv = rtems_filesystem_location_exists_in_same_instance_as(
    old_currentloc,
    new_currentloc
  );
  if ( rv == 0 ) {
    const char                             *old_name;
    size_t                                  old_namelen;
    const char                             *new_name;
    size_t                                  new_namelen;
    const rtems_filesystem_location_info_t *new_loc;

    get_name( old, &old_name, &old_namelen );
    get_name( new, &new_name, &new_namelen );

    if ( rtems_filesystem_eval_path_has_token( &new_ctx ) ) {
      new_loc = NULL;
    } else {
      new_loc = new_currentloc;
    }

    rv = check_rename(
      &old_parentloc,
      old_currentloc,
      old_name,
      old_namelen,
      &new_parentloc,
      new_loc,
      new_name,
      new_namelen
    );

    if ( rv == 0 ) {
      rv = ( *new_currentloc->mt_entry->ops->rename_h )(
        &old_parentloc,
        old_currentloc,
        &new_parentloc,
        new_loc,
        new_name,
        new_namelen
      );
    } else if ( rv > 0 ) {
      rv = 0;
    }
  }

  rtems_filesystem_eval_path_cleanup_with_parent( &old_ctx, &old_parentloc );
  rtems_filesystem_eval_path_cleanup_with_parent( &new_ctx, &new_parentloc );

  return rv;
}
#endif
