/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief  MALLOC_INFO Shell Command Implementation
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#include <inttypes.h>
#include <string.h>

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>
#include <rtems/shellconfig.h>

#include "internal.h"

static int rtems_shell_main_malloc_info(
  int   argc,
  char *argv[]
)
{
  if ( argc == 2 && strcmp( argv[ 1 ], "walk" ) == 0 ) {
    malloc_walk( 0, true );
  } else {
    Heap_Information_block info;

    rtems_shell_print_unified_work_area_message();
    malloc_info( &info );
    rtems_shell_print_heap_info( "free", &info.Free );
    rtems_shell_print_heap_info( "used", &info.Used );
    rtems_shell_print_heap_stats( &info.Stats );
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_MALLOC_INFO_Command = {
  .name = "malloc",
  .usage = "malloc [walk]",
  .topic = "mem",
  .command = rtems_shell_main_malloc_info,
  .alias = NULL,
  .next = NULL
};

