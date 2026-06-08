/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Gedare Bloom.
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

#include <bsp.h>
#include "tmacros.h"
#include <rtems/imfs.h>
#include <rtems/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libfdt.h>
#include <rtems/rtems-fdt.h>

#include <rtems/rtems-fdtimpl.h>

#include "fdt02-tar.h"

const char rtems_test_name[] = "FDT 02";

void test_mem_rsv(
  rtems_fdt_handle *handle,
  int               count,
  int               n,
  uint64_t          exp_address,
  uint64_t          exp_size
);

void test_entry_index(
  rtems_fdt_handle *handle,
  int               exp_entries,
  int               id,
  const char       *exp_name,
  int               exp_offset
);

void test_get_uint(
  rtems_fdt_handle *handle,
  int               offset,
  const char       *name,
  uint32_t          exp_value0,
  uint32_t          exp_value1,
  uint32_t          exp_value2
);

rtems_task Init( rtems_task_argument argument );

void test_mem_rsv(
  rtems_fdt_handle *handle,
  int               count,
  int               n,
  uint64_t          exp_address,
  uint64_t          exp_size
)
{
  int      rc, num;
  uint64_t address, size;

  num = rtems_fdt_num_mem_rsv( handle );
  rtems_test_assert( num == count );

  rc = rtems_fdt_get_mem_rsv( handle, n, &address, &size );

  if ( n < 0 ) {
    rtems_test_assert( rc < 0 );
  } else if ( n == num ) {
    rtems_test_assert( rc == 0 );
    rtems_test_assert( size == 0 );
  } else {
    rtems_test_assert( rc == 0 );
    rtems_test_assert( exp_address == address );
    rtems_test_assert( exp_size == size );
  }
}

void test_entry_index(
  rtems_fdt_handle *handle,
  int               exp_entries,
  int               id,
  const char       *exp_name,
  int               exp_offset
)
{
  int         entries;
  const char *name;
  const char *at;
  int         offset;

  entries = rtems_fdt_num_entries( handle );
  rtems_test_assert( entries == exp_entries );

  name = rtems_fdt_entry_name( handle, id );
  rtems_test_assert( name );
  if ( strchr( exp_name, '@' ) == NULL ) {
    at = strchr( name, '@' );
  } else {
    at = strchr( name, 0 );
  }
  rtems_test_assert( strncmp( name, exp_name, at - name ) == 0 );

  offset = rtems_fdt_entry_offset( handle, id );
  rtems_test_assert( offset == exp_offset );
}

void test_get_uint(
  rtems_fdt_handle *handle,
  int               offset,
  const char       *name,
  uint32_t          exp_value0,
  uint32_t          exp_value1,
  uint32_t          exp_value2
)
{
  int         len;
  const void *prop;
  uint32_t    v32;
  uint64_t    v64;
  uint64_t    exp64, exp64_1;
  uintptr_t   vptr;

  exp64 = ( (uint64_t) exp_value0 << 8 * sizeof( uint32_t ) ) | exp_value1;
  exp64_1 = ( (uint64_t) exp_value1 << 8 * sizeof( uint32_t ) ) | exp_value2;

  prop = rtems_fdt_getprop( handle, offset, name, &len );
  rtems_test_assert( prop != NULL );
  rtems_test_assert( (size_t) len >= sizeof( uint32_t ) );

  v32 = rtems_fdt_get_uint32( prop );
  rtems_test_assert( v32 == exp_value0 );

  if ( sizeof( uintptr_t ) == sizeof( uint32_t ) ) {
    vptr = rtems_fdt_get_uintptr( prop );
    rtems_test_assert( vptr == (uintptr_t) exp_value0 );
  }

  /* full read for 32-bit pointer, truncated to 4 bytes for 64-bit */
  vptr = rtems_fdt_get_offset_len_uintptr( prop, 0, sizeof( uint32_t ) );
  rtems_test_assert( vptr == (uintptr_t) exp_value0 );

  /* truncate to two bytes */
  vptr = rtems_fdt_get_offset_len_uintptr( prop, 0, 2 );
  rtems_test_assert( vptr == (uintptr_t) exp_value0 >> 16 );

  if ( (size_t) len >= sizeof( uint64_t ) ) {
    v32 = rtems_fdt_get_offset_uint32( prop, 1 );
    rtems_test_assert( v32 == exp_value1 );

    v64 = rtems_fdt_get_uint64( prop );
    rtems_test_assert( v64 == exp64 );

    if ( sizeof( uintptr_t ) == sizeof( uint64_t ) ) {
      vptr = rtems_fdt_get_uintptr( prop );
      rtems_test_assert( vptr == (uintptr_t) exp64 );

      vptr = rtems_fdt_get_offset_len_uintptr( prop, 0, sizeof( uint64_t ) );
      rtems_test_assert( vptr == (uintptr_t) exp64 );

      /* truncate to 6 bytes */
      vptr = rtems_fdt_get_offset_len_uintptr( prop, 0, 6 );
      rtems_test_assert( vptr == (uintptr_t) exp64 >> 16 );
    } else {
      vptr = rtems_fdt_get_offset_uintptr( prop, 1 );
      rtems_test_assert( vptr == (uintptr_t) exp_value1 );
    }

    vptr = rtems_fdt_get_offset_len_uintptr( prop, 4, sizeof( uint32_t ) );
    rtems_test_assert( vptr == (uintptr_t) exp_value1 );

    /* truncate to 1 byte */
    vptr = rtems_fdt_get_offset_len_uintptr( prop, 4, 1 );
    rtems_test_assert( vptr == (uintptr_t) exp_value1 >> 24 );

    /* check across 32-bit boundary */
    vptr = rtems_fdt_get_offset_len_uintptr( prop, 1, sizeof( uint32_t ) );
    rtems_test_assert(
      vptr == ( (uintptr_t) ( exp_value0 << 8 ) | ( exp_value1 >> 24 ) )
    );
  }

  if ( (size_t) len > sizeof( uint64_t ) + sizeof( uint32_t ) ) {
    v32 = rtems_fdt_get_offset_uint32( prop, 2 );
    rtems_test_assert( v32 == exp_value2 );

    v64 = rtems_fdt_get_offset_uint64( prop, 1 );
    rtems_test_assert( v64 == exp64_1 );

    if ( sizeof( uintptr_t ) == sizeof( uint64_t ) ) {
      vptr = rtems_fdt_get_offset_uintptr( prop, 1 );
      rtems_test_assert( vptr == (uintptr_t) exp64_1 );

      vptr = rtems_fdt_get_offset_len_uintptr( prop, 4, sizeof( uint64_t ) );
      rtems_test_assert( vptr == (uintptr_t) exp64_1 );
    } else {
      vptr = rtems_fdt_get_offset_uintptr( prop, 2 );
      rtems_test_assert( vptr == (uintptr_t) exp_value2 );
    }

    vptr = rtems_fdt_get_offset_len_uintptr( prop, 8, sizeof( uint32_t ) );
    rtems_test_assert( vptr == (uintptr_t) exp_value2 );

    /* truncate to 3 byte */
    vptr = rtems_fdt_get_offset_len_uintptr( prop, 8, 3 );
    rtems_test_assert( vptr == (uintptr_t) exp_value2 >> 8 );

    /* check across 64-bit boundary */
    vptr = rtems_fdt_get_offset_len_uintptr( prop, 6, 4 );
    rtems_test_assert(
      vptr == ( (uintptr_t) ( exp_value1 << 16 ) | ( exp_value2 >> 16 ) )
    );
  }
}

#define NUM_MEM_RSV ( 3 )

rtems_task Init( rtems_task_argument ignored )
{
  (void) ignored;
  rtems_status_code sc;
  rtems_fdt_handle  handle;
  int               rc;
  int               i;
  int               num_mem_rsv = NUM_MEM_RSV;
  int               idx;

  uint64_t addresses[] =
    { 0x1000000000000000, 0x2000000000000000, 0x0000000000000000 };
  uint64_t sizes[] =
    { 0x0000000002000000, 0x0100000000000000, 0x0000000000000014 };

  TEST_BEGIN();

  sc = rtems_tarfs_load( "/", (void *) fdt02_tar, fdt02_tar_size );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rc = rtems_fdt_load_blob_file_binary( "/test01.dtb", &handle );
  rtems_test_assert( rc == 0 );

  test_mem_rsv( &handle, num_mem_rsv, -1, 0, 0 );
  test_mem_rsv( &handle, num_mem_rsv, num_mem_rsv, 0, 0 );

  for ( i = 0; i < num_mem_rsv; i++ ) {
    test_mem_rsv( &handle, num_mem_rsv, i, addresses[ i ], sizes[ i ] );
  }

  test_entry_index( &handle, 6, 1, "/cpus", 108 );
  test_entry_index( &handle, 6, 4, "/memory", 548 );
  test_entry_index( &handle, 6, 4, "/memory@0", 548 );

  idx = rtems_fdt_find_path_offset( &handle, "/randomnode" );
  rtems_test_assert( idx >= 0 );
  test_get_uint( &handle, idx, "blob", 0x0a0b0c0d, 0xdeeaadbe, 0 );
  test_get_uint( &handle, idx, "string", 0xff007374, 0x75666673, 0x74756666 );

  rc = rtems_fdt_unload( &handle );
  rtems_test_assert( rc == 0 );

  TEST_END();
  rtems_test_exit( 0 );
}

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS ( 3 + 1 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
