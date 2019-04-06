/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/shmimpl.h>

int _POSIX_Shm_Object_create_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t size
)
{
  shm_obj->handle = _Workspace_Allocate( size );
  if ( shm_obj->handle == NULL ) {
    return ENOMEM;
  }

  memset( shm_obj->handle, 0, size );
  shm_obj->size = size;
  return 0;
}

int _POSIX_Shm_Object_delete_from_workspace( POSIX_Shm_Object *shm_obj )
{
  /* zero out memory before releasing it. */
  memset( shm_obj->handle, 0, shm_obj->size );
  _Workspace_Free( shm_obj->handle );
  shm_obj->handle = NULL;
  shm_obj->size = 0;
  return 0;
}

int _POSIX_Shm_Object_resize_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t size
)
{
  int err;

  if ( size == 0 ) {
    err = _POSIX_Shm_Object_delete_from_workspace( shm_obj );
  } else if ( shm_obj->handle == NULL && shm_obj->size == 0 ) {
    err = _POSIX_Shm_Object_create_from_workspace( shm_obj, size );
  } else {
    /* Refuse to resize a workspace object. */
    err = EIO;
  }
  return err;
}

int _POSIX_Shm_Object_read_from_workspace(
  POSIX_Shm_Object *shm_obj,
  void *buf,
  size_t count
)
{
  if ( shm_obj == NULL || shm_obj->handle == NULL )
    return 0;

  if ( shm_obj->size < count ) {
    count = shm_obj->size;
  }

  memcpy( buf, shm_obj->handle, count );

  return count;
}

void * _POSIX_Shm_Object_mmap_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t len,
  int prot,
  off_t off
)
{
  if ( shm_obj == NULL || shm_obj->handle == NULL )
    return 0;

  /* This is already checked by mmap. Maybe make it a debug assert? */
  if ( shm_obj->size < len + off ) {
    return NULL;
  }

  return (char*)shm_obj->handle + off;
}

