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

#include "fdt01-tar.h"

#include "zynqmp-test-dtb.h"
#include "mpfs-test-dtb.h"
#include "microblaze-test-dtb.h"

rtems_task Init( rtems_task_argument argument );

void test_fdt_handle( rtems_fdt_handle *handle );

void test_compatible(
  rtems_fdt_handle *handle,
  const char       *compatible,
  bool              should_pass
);

void test_get_phandle( rtems_fdt_handle *handle, uint32_t phandle );

void test_getprop(
  rtems_fdt_handle *handle,
  const char       *pname,
  const void       *pval,
  int               plen
);

void test_subnode(
  rtems_fdt_handle *handle,
  int               offset,
  const char       *sname,
  int               slen
);

const char rtems_test_name[] = "FDT 01";

/* triplets of identical uncompressed/compressed dtbs */
const char *dtbs[] = {
  "/system.dtb",
  "/system.dtb.gz",
  (const char *) microblaze_test_dtb,
  "/zynqmp.dtb",
  "/zynqmp.dtb.gz",
  (const char *) zynqmp_test_dtb,
  "/mpfs.dtb",
  "/mpfs.dtb.gz",
  (const char *) mpfs_test_dtb,
};
#define NUM_DTB ( 3 * 3 )

void test_fdt_handle( rtems_fdt_handle *handle )
{
  rtems_fdt_handle test_handle;

  rtems_test_assert( rtems_fdt_valid_handle( handle ) );

  /* handle duplication */
  rtems_fdt_dup_handle( handle, &test_handle );
  rtems_test_assert( rtems_fdt_valid_handle( &test_handle ) );
  rtems_fdt_release_handle( &test_handle );
  rtems_test_assert( !rtems_fdt_valid_handle( &test_handle ) );
}

void test_compatible(
  rtems_fdt_handle *handle,
  const char       *compatible,
  bool              should_pass
)
{
  int idx;

  idx = rtems_fdt_node_offset_by_compatible( handle, -1, compatible );
  rtems_test_assert( should_pass ? idx >= 0 : idx < 0 );

  rtems_test_assert(
    !rtems_fdt_node_check_compatible( handle, idx, compatible ) == should_pass
  );
}

void test_get_phandle( rtems_fdt_handle *handle, uint32_t phandle )
{
  int      idx;
  uint32_t test_phandle;

  idx = rtems_fdt_node_offset_by_phandle( handle, phandle );
  rtems_test_assert( idx >= 0 );

  test_phandle = rtems_fdt_get_phandle( handle, idx );
  rtems_test_assert( test_phandle == phandle );
}

void test_getprop(
  rtems_fdt_handle *handle,
  const char       *pname,
  const void       *pval,
  int               plen
)
{
  int         idx;
  const void *prop;
  const char *namep;
  int         len;
  size_t      name_len = strlen( pname );

  idx = rtems_fdt_node_offset_by_prop_value( handle, -1, pname, pval, plen );
  rtems_test_assert( idx >= 0 );

  prop = rtems_fdt_getprop( handle, idx, pname, &len );
  rtems_test_assert( prop != NULL );
  rtems_test_assert( plen == len );

  prop = rtems_fdt_getprop_namelen( handle, idx, pname, name_len, &len );
  rtems_test_assert( prop != NULL );
  rtems_test_assert( plen == len );

  idx = rtems_fdt_first_prop_offset( handle, idx );
  rtems_test_assert( idx >= 0 );

  idx = rtems_fdt_next_prop_offset( handle, idx );
  rtems_test_assert( idx >= 0 );
  idx = rtems_fdt_next_prop_offset( handle, idx );
  rtems_test_assert( idx >= 0 );

  prop = rtems_fdt_getprop_by_offset( handle, idx, &namep, &len );
  rtems_test_assert( prop != NULL );
  rtems_test_assert( plen == len );
  rtems_test_assert( strncmp( namep, pname, name_len ) == 0 );
}

void test_subnode(
  rtems_fdt_handle *handle,
  int               offset,
  const char       *sname,
  int               slen
)
{
  int         idx1, idx2;
  const char *name;
  int         len = 0;
  int         parent_offset;

  name = rtems_fdt_get_name( handle, offset, &len );
  rtems_test_assert( strncmp( name, sname, slen ) == 0 );

  parent_offset = rtems_fdt_parent_offset( handle, offset );
  rtems_test_assert( parent_offset >= 0 );

  idx1 = rtems_fdt_subnode_offset( handle, parent_offset, sname );
  rtems_test_assert( idx1 >= 0 );
  rtems_test_assert( idx1 != parent_offset );

  idx2 = rtems_fdt_subnode_offset_namelen(
    handle,
    parent_offset,
    sname,
    slen
  );
  rtems_test_assert( idx2 >= 0 );
  rtems_test_assert( idx1 == idx2 );
}

rtems_task Init( rtems_task_argument ignored )
{
  (void) ignored;
  rtems_status_code sc;
  int               rc;
  size_t            i;
  uint32_t          val;
  int               idx;
  const char       *name = "interrupt-controller@c000000";
  rtems_fdt_handle  handles[ NUM_DTB ];

  TEST_BEGIN();

  RTEMS_STATIC_ASSERT( RTEMS_ARRAY_SIZE( dtbs ) == NUM_DTB, num_dtb );
  RTEMS_STATIC_ASSERT( NUM_DTB % 3 == 0, dtb_triplets );

  sc = rtems_tarfs_load( "/", (void *) fdt01_tar, fdt01_tar_size );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  /*
   * ensure that loading an FDT is the same whether it is a raw binary file,
   * a compressed file, or pre-compiled into the application image
   */
  for ( i = 0; i < NUM_DTB; i += 3 ) {
    struct stat sb;

    rc = rtems_fdt_load( dtbs[ i ], &handles[ i ] );
    if ( rc < 0 ) {
      printf( "error: %s\n", rtems_fdt_strerror( rc ) );
    }
    rtems_test_assert( rc == 0 );

    rc = stat( dtbs[ i ], &sb );
    rtems_test_assert( rc == 0 );

    rc = rtems_fdt_load( dtbs[ i + 1 ], &handles[ i + 1 ] );
    if ( rc < 0 ) {
      printf( "error: %s\n", rtems_fdt_strerror( rc ) );
    }
    rtems_test_assert( rc == 0 );

    rc = rtems_fdt_register( (void *) dtbs[ i + 2 ], &handles[ i + 2 ] );
    if ( rc < 0 ) {
      printf( "error: %s\n", rtems_fdt_strerror( rc ) );
    }
    rtems_test_assert( rc == 0 );

    rc = memcmp(
      handles[ i ].blob->blob,
      handles[ i + 1 ].blob->blob,
      sb.st_size
    );
    rtems_test_assert( rc == 0 );

    rc = memcmp(
      handles[ i ].blob->blob,
      handles[ i + 2 ].blob->blob,
      sb.st_size
    );
    rtems_test_assert( rc == 0 );
  }

  test_fdt_handle( &handles[ 0 ] );
  test_compatible( &handles[ 0 ], "fixed-clock", true );
  test_compatible( &handles[ 3 ], "fixed-clock", false );
  test_get_phandle( &handles[ 0 ], 0x7 );
  val = cpu_to_fdt32( 0x2 );
  test_getprop( &handles[ 0 ], "clocks", &val, sizeof( val ) );

  idx = rtems_fdt_node_offset_by_compatible(
    &handles[ 6 ],
    -1,
    "riscv,plic0"
  );
  rtems_test_assert( idx >= 0 );
  test_subnode( &handles[ 6 ], idx, name, strlen( name ) );
  /* try again with length truncated to the unit address */
  test_subnode(
    &handles[ 6 ],
    idx,
    name,
    (const char *) memchr( name, '@', strlen( name ) ) - name
  );

  for ( i = 0; i < NUM_DTB; i++ ) {
    rc = rtems_fdt_unload( &handles[ i ] );
    rtems_test_assert( rc == 0 );
  }

  TEST_END();
  rtems_test_exit( 0 );
}

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS ( 3 + ( NUM_DTB / 3 ) * 2 )

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
