/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/posix/shmimpl.h>

int _POSIX_Shm_Object_create_from_heap(
  POSIX_Shm_Object *shm_obj,
  size_t size
)
{
  void *p = calloc( 1, size ); /* get zero'd memory */
  if ( p != NULL ) {
    shm_obj->handle = p;
    shm_obj->size = size;
  } else {
    errno = EIO;
  }
  return 0;
}

int _POSIX_Shm_Object_delete_from_heap( POSIX_Shm_Object *shm_obj )
{
  /* zero out memory before releasing it. */
  memset( shm_obj->handle, 0, shm_obj->size );
  free( shm_obj->handle );
  shm_obj->handle = NULL;
  shm_obj->size = 0;
  return 0;
}

int _POSIX_Shm_Object_resize_from_heap(
  POSIX_Shm_Object *shm_obj,
  size_t size
)
{
  void *p;
  int err = 0;

  if ( size < shm_obj->size ) {
    /* zero out if shrinking */
    p = (void*)((uintptr_t)shm_obj->handle + (uintptr_t)size);
    memset( p, 0, shm_obj->size - size );
  }
  p = realloc( shm_obj->handle, size );
  if ( p != NULL ) {
    shm_obj->handle = p;
    if ( size > shm_obj->size ) {
      /* initialize added memory */ 
      memset( p, 0, size - shm_obj->size );
    }
    shm_obj->size = size;
  } else {
    err = EIO;
  }
  return err;
}

/* This is identical to _POSIX_Shm_Object_read_from_wkspace */
int _POSIX_Shm_Object_read_from_heap(
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

/* This is identical to _POSIX_Shm_Object_mmap_from_wkspace */
void * _POSIX_Shm_Object_mmap_from_heap(
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

