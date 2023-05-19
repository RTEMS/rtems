/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBasedefsValBasedefs
 *
 * @brief Helper file to verify the requirements towards the basedefs.
 *
 * The specification items in
 * (rtems-central/)spec/rtems/basedefs/req/[*] represent
 * requirements towards the basedefs in
 * (rtems/)cpukit/include/rtems/score/basedefs.h.
 * There are automated verification tests generated from
 * (rtems-central/)spec/rtems/basedefs/val/[*] and placed
 * in (rtems/)testsuites/validation/tc-basedefs-0.c.
 *
 * Yet, not every test can be placed in the same compilation unit
 * (tc-basedefs-0.c). Hence, the code which must be placed
 * in an extra compilation unit is put here and this file
 * is linked to tc-basedefs-0.c to be accessible from the tests.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include "tc-basedefs-pendant.h"

/* For the RTEMS_SYMBOL_NAME tests */
#undef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ SyMbOl_
#define SyMbOl_symbol_id prefix_symbol_id
#define SYMBOL_NAME symbol_name

uintptr_t basedefs_get_global_symbol( void )
{
  return (uintptr_t) global_symbol;
}

int basedefs_use_prefixed_symbol_name( void )
{
  const int SyMbOl_symbol_name = 124;
  return RTEMS_SYMBOL_NAME( symbol_name );
}

int basedefs_use_prefixed_upper_symbol_name( void )
{
  const int SyMbOl_symbol_name = 125;
  return RTEMS_SYMBOL_NAME( SYMBOL_NAME );
}

int basedefs_use_prefixed_symbol_id( void )
{
  const int prefix_symbol_id = 126;
  return RTEMS_SYMBOL_NAME( symbol_id );
}

int basedefs_weak_alias_1_func( int i )
{
  return 10 * i;
}

const volatile int basedefs_weak_1_var = 62;

int basedefs_weak_1_func( void )
{
  return 65;
}

static RTEMS_ALIGNED( 64 ) int allocated_memory_dummy;

void *basedefs_malloclike_func( size_t size )
{
  return &allocated_memory_dummy;
}

void *basedefs_alloc_align_func( size_t size, void **p, size_t alignment )
{
  *p = &allocated_memory_dummy;
  return &allocated_memory_dummy;
}

void *basedefs_alloc_size_func( size_t size )
{
  return &allocated_memory_dummy;
}

void *basedefs_alloc_size_2_func( size_t size0, size_t size1 )
{
  return &allocated_memory_dummy;
}

void basedefs_free( void *ptr )
{
  (void) ptr;
}
