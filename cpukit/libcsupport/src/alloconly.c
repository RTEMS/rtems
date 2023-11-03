/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementations of rtems_malloc() and
 *   posix_memalign() which do not support free().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <rtems/malloc.h>

#include <errno.h>
#include <stdlib.h>

#include <rtems/score/memory.h>

void *rtems_malloc( size_t size )
{
  if ( size == 0 ) {
    return NULL;
  }

  return _Memory_Allocate( _Memory_Get(), size, CPU_HEAP_ALIGNMENT );
}

int posix_memalign( void **memptr, size_t alignment, size_t size )
{
  void *p;

  RTEMS_OBFUSCATE_VARIABLE( memptr );

  if ( memptr == NULL ) {
    return EINVAL;
  }

  *memptr = NULL;

  if ( alignment < sizeof (void *) ) {
    return EINVAL;
  }

  if ( ( ( alignment - 1 ) & alignment ) != 0 ) {
    return EINVAL;
  }

  if ( size == 0 ) {
    return 0;
  }

  p = _Memory_Allocate( _Memory_Get(), size, alignment );

  if ( p == NULL ) {
    return ENOMEM;
  }

  *memptr = p;
  return 0;
}
