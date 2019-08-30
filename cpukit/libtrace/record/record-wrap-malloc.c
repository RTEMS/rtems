/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/record.h>

#include <stdlib.h>

typedef void *Record_Aligned_alloc(size_t, size_t);

Record_Aligned_alloc aligned_alloc;
Record_Aligned_alloc __real_aligned_alloc;
Record_Aligned_alloc __wrap_aligned_alloc;

void *__wrap_aligned_alloc( size_t alignment, size_t size )
{
  void *ptr;

  rtems_record_entry_2(
    RTEMS_RECORD_ALIGNED_ALLOC_ENTRY,
    alignment,
    size
  );
  ptr = __real_aligned_alloc( alignment, size );
  rtems_record_exit_1(
    RTEMS_RECORD_ALIGNED_ALLOC_EXIT,
    (rtems_record_data) ptr
  );
  return ptr;
}

typedef void *Record_Calloc( size_t, size_t );

Record_Calloc calloc;
Record_Calloc __real_calloc;
Record_Calloc __wrap_calloc;

void *__wrap_calloc( size_t nelem, size_t elsize )
{
  void *ptr;

  rtems_record_entry_2( RTEMS_RECORD_CALLOC_ENTRY, nelem, elsize );
  ptr = __real_calloc( nelem, elsize );
  rtems_record_exit_1( RTEMS_RECORD_CALLOC_EXIT, (rtems_record_data) ptr );
  return ptr;
}

typedef void *Record_Malloc( size_t );

Record_Malloc malloc;
Record_Malloc __real_malloc;
Record_Malloc __wrap_malloc;

void *__wrap_malloc( size_t size )
{
  void *ptr;

  rtems_record_entry_1( RTEMS_RECORD_MALLOC_ENTRY, size );
  ptr = __real_malloc( size );
  rtems_record_exit_1( RTEMS_RECORD_MALLOC_EXIT, (rtems_record_data) ptr );
  return ptr;
}

typedef void Record_Free( void *ptr );

Record_Free free;
Record_Free __real_free;
Record_Free __wrap_free;

void __wrap_free( void *ptr )
{
  rtems_record_entry_1( RTEMS_RECORD_FREE_ENTRY, (rtems_record_data) ptr );
  __real_free( ptr );
  rtems_record_entry( RTEMS_RECORD_FREE_EXIT );
}

typedef int Record_POSIX_memalign( void **, size_t, size_t );

Record_POSIX_memalign posix_memalign;
Record_POSIX_memalign __real_posix_memalign;
Record_POSIX_memalign __wrap_posix_memalign;

int __wrap_posix_memalign( void **memptr, size_t alignment, size_t size )
{
  int eno;

  rtems_record_entry_3(
    RTEMS_RECORD_POSIX_MEMALIGN_ENTRY,
    (rtems_record_data) memptr,
    alignment,
    size
  );
  eno = __real_posix_memalign( memptr, alignment, size );
  rtems_record_exit_2(
    RTEMS_RECORD_POSIX_MEMALIGN_EXIT,
    eno,
    (rtems_record_data) ( memptr != NULL ? *memptr : NULL )
  );
  return eno;
}

typedef void *Record_Realloc( void *, size_t );

Record_Realloc realloc;
Record_Realloc __real_realloc;
Record_Realloc __wrap_realloc;

void *__wrap_realloc( void *ptr, size_t size )
{
  rtems_record_entry_2(
    RTEMS_RECORD_REALLOC_ENTRY,
    (rtems_record_data) ptr,
    size
  );
  ptr = __real_realloc( ptr, size );
  rtems_record_exit_1( RTEMS_RECORD_REALLOC_EXIT, (rtems_record_data) ptr );
  return ptr;
}
