/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 embedded brains GmbH
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

#include <rtems/sysinit.h>
#include <rtems/score/memory.h>
#include <rtems/bspIo.h>

#include <inttypes.h>

RTEMS_LINKER_RWSET_ITEM_DECLARE(
  _Sysinit,
  rtems_sysinit_item,
  _Sysinit_Verbose
);

#define SYSINIT_VERBOSE(x) \
  static void _Sysinit_Verbose_##x( void ); \
  RTEMS_SYSINIT_ITEM( \
    _Sysinit_Verbose_##x, \
    RTEMS_SYSINIT_##x, \
    RTEMS_SYSINIT_ORDER_LAST \
  )

SYSINIT_VERBOSE( BSP_EARLY );
SYSINIT_VERBOSE( MEMORY );
SYSINIT_VERBOSE( DIRTY_MEMORY );
SYSINIT_VERBOSE( ZERO_MEMORY );
SYSINIT_VERBOSE( ISR_STACK );
SYSINIT_VERBOSE( PER_CPU_DATA );
SYSINIT_VERBOSE( SBRK );
SYSINIT_VERBOSE( WORKSPACE );
SYSINIT_VERBOSE( MALLOC );
SYSINIT_VERBOSE( BSP_START );
SYSINIT_VERBOSE( CPU_COUNTER );
SYSINIT_VERBOSE( INITIAL_EXTENSIONS );
SYSINIT_VERBOSE( MP_EARLY );
SYSINIT_VERBOSE( DATA_STRUCTURES );
SYSINIT_VERBOSE( MP );
SYSINIT_VERBOSE( USER_EXTENSIONS );
SYSINIT_VERBOSE( CLASSIC_TASKS );
SYSINIT_VERBOSE( CLASSIC_TASKS_MP );
SYSINIT_VERBOSE( CLASSIC_TIMER );
SYSINIT_VERBOSE( CLASSIC_SIGNAL_MP );
SYSINIT_VERBOSE( CLASSIC_EVENT_MP );
SYSINIT_VERBOSE( CLASSIC_MESSAGE_QUEUE );
SYSINIT_VERBOSE( CLASSIC_MESSAGE_QUEUE_MP );
SYSINIT_VERBOSE( CLASSIC_SEMAPHORE );
SYSINIT_VERBOSE( CLASSIC_SEMAPHORE_MP );
SYSINIT_VERBOSE( CLASSIC_PARTITION );
SYSINIT_VERBOSE( CLASSIC_PARTITION_MP );
SYSINIT_VERBOSE( CLASSIC_REGION );
SYSINIT_VERBOSE( CLASSIC_DUAL_PORTED_MEMORY );
SYSINIT_VERBOSE( CLASSIC_RATE_MONOTONIC );
SYSINIT_VERBOSE( CLASSIC_BARRIER );
SYSINIT_VERBOSE( POSIX_SIGNALS );
SYSINIT_VERBOSE( POSIX_THREADS );
SYSINIT_VERBOSE( POSIX_MESSAGE_QUEUE );
SYSINIT_VERBOSE( POSIX_SEMAPHORE );
SYSINIT_VERBOSE( POSIX_TIMER );
SYSINIT_VERBOSE( POSIX_SHM );
SYSINIT_VERBOSE( POSIX_KEYS );
SYSINIT_VERBOSE( POSIX_CLEANUP );
SYSINIT_VERBOSE( IDLE_THREADS );
SYSINIT_VERBOSE( LIBIO );
SYSINIT_VERBOSE( USER_ENVIRONMENT );
SYSINIT_VERBOSE( ROOT_FILESYSTEM );
SYSINIT_VERBOSE( DRVMGR );
SYSINIT_VERBOSE( MP_SERVER );
SYSINIT_VERBOSE( BSP_PRE_DRIVERS );
SYSINIT_VERBOSE( DRVMGR_LEVEL_1 );
SYSINIT_VERBOSE( DEVICE_DRIVERS );
SYSINIT_VERBOSE( DRVMGR_LEVEL_2 );
SYSINIT_VERBOSE( DRVMGR_LEVEL_3 );
SYSINIT_VERBOSE( DRVMGR_LEVEL_4 );
SYSINIT_VERBOSE( MP_FINALIZE );
SYSINIT_VERBOSE( CLASSIC_USER_TASKS );
SYSINIT_VERBOSE( POSIX_USER_THREADS );
SYSINIT_VERBOSE( STD_FILE_DESCRIPTORS );
SYSINIT_VERBOSE( LAST );

static bool _Sysinit_Verbose_is_adjacent(
  const rtems_sysinit_item *a,
  const rtems_sysinit_item *b
)
{
  RTEMS_OBFUSCATE_VARIABLE( a );
  RTEMS_OBFUSCATE_VARIABLE( b );

  return a + 1 == b;
}

#define SYSINIT_IS_ADJACENT( x, y ) \
  _Sysinit_Verbose_is_adjacent( \
    &_Linker_set__Sysinit__Sysinit_Verbose_##x, \
    &_Linker_set__Sysinit__Sysinit_Verbose_##y \
  )

static void _Sysinit_Print_free_memory( void )
{
  const Memory_Information *mem;
  size_t                    i;

  mem = _Memory_Get();

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    const Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    printk(
      "sysinit: memory area %zu: free begin = 0x%08" PRIxPTR
        ", free size = 0x%08" PRIxPTR "\n",
      i,
      (uintptr_t) _Memory_Get_free_begin( area ),
      _Memory_Get_free_size( area )
    );
  }
}

void _Sysinit_Verbose( void )
{
  printk( "sysinit: begin\n" );
}

static void _Sysinit_Verbose_BSP_EARLY( void )
{
  if (
    !_Sysinit_Verbose_is_adjacent(
      &_Linker_set__Sysinit__Sysinit_Verbose,
      &_Linker_set__Sysinit__Sysinit_Verbose_BSP_EARLY
    )
  ) {
    printk( "sysinit: BSP early: done\n" );
  }
}

static void _Sysinit_Verbose_MEMORY( void )
{
  const Memory_Information *mem;
  size_t                    i;

  if ( !SYSINIT_IS_ADJACENT( BSP_EARLY, MEMORY ) ) {
    printk( "sysinit: memory: done\n" );
  }

  printk(
    "sysinit: stack pointer: 0x%08" PRIxPTR "\n",
    (uintptr_t) __builtin_frame_address( 0 )
  );

  mem = _Memory_Get();

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    const Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    printk(
      "sysinit: memory area %zu: begin = 0x%08" PRIxPTR
        ", size = 0x%08" PRIxPTR "\n",
      i,
      (uintptr_t) _Memory_Get_begin( area ),
      _Memory_Get_size( area )
    );
  }
}

static void _Sysinit_Verbose_DIRTY_MEMORY( void )
{
  if ( !SYSINIT_IS_ADJACENT( BSP_EARLY, DIRTY_MEMORY ) ) {
    printk( "sysinit: DIRTY_MEMORY: done\n" );
  }
}

static void _Sysinit_Verbose_ZERO_MEMORY( void )
{
  if ( !SYSINIT_IS_ADJACENT( DIRTY_MEMORY, ZERO_MEMORY ) ) {
    printk( "sysinit: ZERO_MEMORY: done\n" );
  }
}

static void _Sysinit_Verbose_ISR_STACK( void )
{
  if ( !SYSINIT_IS_ADJACENT( ZERO_MEMORY, ISR_STACK ) ) {
    printk( "sysinit: ISR_STACK: done\n" );
  }
}

static void _Sysinit_Verbose_PER_CPU_DATA( void )
{
  if ( !SYSINIT_IS_ADJACENT( ISR_STACK, PER_CPU_DATA ) ) {
    _Sysinit_Print_free_memory();
    printk( "sysinit: PER_CPU_DATA: done\n" );
  }
}

static void _Sysinit_Verbose_SBRK( void )
{
  if ( !SYSINIT_IS_ADJACENT( PER_CPU_DATA, SBRK ) ) {
    _Sysinit_Print_free_memory();
    printk( "sysinit: SBRK: done\n" );
  }
}

static void _Sysinit_Verbose_WORKSPACE( void )
{
  if ( !SYSINIT_IS_ADJACENT( SBRK, WORKSPACE ) ) {
    _Sysinit_Print_free_memory();
    printk( "sysinit: WORKSPACE: done\n" );
  }
}

static void _Sysinit_Verbose_MALLOC( void )
{
  if ( !SYSINIT_IS_ADJACENT( WORKSPACE, MALLOC ) ) {
    _Sysinit_Print_free_memory();
    printk( "sysinit: MALLOC: done\n" );
  }
}

static void _Sysinit_Verbose_BSP_START( void )
{
  if ( !SYSINIT_IS_ADJACENT( MALLOC, BSP_START ) ) {
    printk( "sysinit: BSP_START: done\n" );
  }
}

static void _Sysinit_Verbose_CPU_COUNTER( void )
{
  if ( !SYSINIT_IS_ADJACENT( BSP_START, CPU_COUNTER ) ) {
    printk( "sysinit: CPU_COUNTER: done\n" );
  }
}

static void _Sysinit_Verbose_INITIAL_EXTENSIONS( void )
{
  if ( !SYSINIT_IS_ADJACENT( CPU_COUNTER, INITIAL_EXTENSIONS ) ) {
    printk( "sysinit: INITIAL_EXTENSIONS: done\n" );
  }
}

static void _Sysinit_Verbose_MP_EARLY( void )
{
  if ( !SYSINIT_IS_ADJACENT( INITIAL_EXTENSIONS, MP_EARLY ) ) {
    printk( "sysinit: MP_EARLY: done\n" );
  }
}

static void _Sysinit_Verbose_DATA_STRUCTURES( void )
{
  if ( !SYSINIT_IS_ADJACENT( MP_EARLY, DATA_STRUCTURES ) ) {
    printk( "sysinit: DATA_STRUCTURES: done\n" );
  }
}

static void _Sysinit_Verbose_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( DATA_STRUCTURES, MP ) ) {
    printk( "sysinit: MP: done\n" );
  }
}

static void _Sysinit_Verbose_USER_EXTENSIONS( void )
{
  if ( !SYSINIT_IS_ADJACENT( MP, USER_EXTENSIONS ) ) {
    printk( "sysinit: USER_EXTENSIONS: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_TASKS( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_USER_TASKS, CLASSIC_TASKS ) ) {
    printk( "sysinit: CLASSIC_TASKS: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_TASKS_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_TASKS, CLASSIC_TASKS_MP ) ) {
    printk( "sysinit: CLASSIC_TASKS_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_TIMER( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_TASKS_MP, CLASSIC_TIMER ) ) {
    printk( "sysinit: CLASSIC_TIMER: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_SIGNAL_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_TIMER, CLASSIC_SIGNAL_MP ) ) {
    printk( "sysinit: CLASSIC_SIGNAL_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_EVENT_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_SIGNAL_MP, CLASSIC_EVENT_MP ) ) {
    printk( "sysinit: CLASSIC_EVENT_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_MESSAGE_QUEUE( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_EVENT_MP, CLASSIC_MESSAGE_QUEUE ) ) {
    printk( "sysinit: CLASSIC_MESSAGE_QUEUE: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_MESSAGE_QUEUE_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_MESSAGE_QUEUE, CLASSIC_MESSAGE_QUEUE_MP ) ) {
    printk( "sysinit: CLASSIC_MESSAGE_QUEUE_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_SEMAPHORE( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_MESSAGE_QUEUE_MP, CLASSIC_SEMAPHORE ) ) {
    printk( "sysinit: CLASSIC_SEMAPHORE: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_SEMAPHORE_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_SEMAPHORE, CLASSIC_SEMAPHORE_MP ) ) {
    printk( "sysinit: CLASSIC_SEMAPHORE_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_PARTITION( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_SEMAPHORE_MP, CLASSIC_PARTITION ) ) {
    printk( "sysinit: CLASSIC_PARTITION: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_PARTITION_MP( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_PARTITION, CLASSIC_PARTITION_MP ) ) {
    printk( "sysinit: CLASSIC_PARTITION_MP: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_REGION( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_PARTITION_MP, CLASSIC_REGION ) ) {
    printk( "sysinit: CLASSIC_REGION: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_DUAL_PORTED_MEMORY( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_REGION, CLASSIC_DUAL_PORTED_MEMORY ) ) {
    printk( "sysinit: CLASSIC_DUAL_PORTED_MEMORY: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_RATE_MONOTONIC( void )
{
  if (
    !SYSINIT_IS_ADJACENT(
      CLASSIC_DUAL_PORTED_MEMORY,
      CLASSIC_RATE_MONOTONIC
    )
  ) {
    printk( "sysinit: CLASSIC_RATE_MONOTONIC: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_BARRIER( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_RATE_MONOTONIC, CLASSIC_BARRIER ) ) {
    printk( "sysinit: CLASSIC_BARRIER: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_SIGNALS( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_BARRIER, POSIX_SIGNALS ) ) {
    printk( "sysinit: POSIX_SIGNALS: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_THREADS( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_SIGNALS, POSIX_THREADS ) ) {
    printk( "sysinit: POSIX_THREADS: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_MESSAGE_QUEUE( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_THREADS, POSIX_MESSAGE_QUEUE ) ) {
    printk( "sysinit: POSIX_MESSAGE_QUEUE: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_SEMAPHORE( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_MESSAGE_QUEUE, POSIX_SEMAPHORE ) ) {
    printk( "sysinit: POSIX_SEMAPHORE: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_TIMER( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_SEMAPHORE, POSIX_TIMER ) ) {
    printk( "sysinit: POSIX_TIMER: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_SHM( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_TIMER, POSIX_SHM ) ) {
    printk( "sysinit: POSIX_SHM: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_KEYS( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_SHM, POSIX_KEYS ) ) {
    printk( "sysinit: POSIX_KEYS: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_CLEANUP( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_KEYS, POSIX_CLEANUP ) ) {
    printk( "sysinit: POSIX_CLEANUP: done\n" );
  }
}

static void _Sysinit_Verbose_IDLE_THREADS( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_CLEANUP, IDLE_THREADS ) ) {
    printk( "sysinit: IDLE_THREADS: done\n" );
  }
}

static void _Sysinit_Verbose_LIBIO( void )
{
  if ( !SYSINIT_IS_ADJACENT( IDLE_THREADS, LIBIO ) ) {
    printk( "sysinit: LIBIO: done\n" );
  }
}

static void _Sysinit_Verbose_USER_ENVIRONMENT( void )
{
  if ( !SYSINIT_IS_ADJACENT( LIBIO, USER_ENVIRONMENT ) ) {
    printk( "sysinit: USER_ENVIRONMENT: done\n" );
  }
}

static void _Sysinit_Verbose_ROOT_FILESYSTEM( void )
{
  if ( !SYSINIT_IS_ADJACENT( USER_ENVIRONMENT, ROOT_FILESYSTEM ) ) {
    printk( "sysinit: ROOT_FILESYSTEM: done\n" );
  }
}

static void _Sysinit_Verbose_DRVMGR( void )
{
  if ( !SYSINIT_IS_ADJACENT( ROOT_FILESYSTEM, DRVMGR ) ) {
    printk( "sysinit: DRVMGR: done\n" );
  }
}

static void _Sysinit_Verbose_MP_SERVER( void )
{
  if ( !SYSINIT_IS_ADJACENT( DRVMGR, MP_SERVER ) ) {
    printk( "sysinit: MP_SERVER: done\n" );
  }
}

static void _Sysinit_Verbose_BSP_PRE_DRIVERS( void )
{
  if ( !SYSINIT_IS_ADJACENT( MP_SERVER, BSP_PRE_DRIVERS ) ) {
    printk( "sysinit: BSP_PRE_DRIVERS: done\n" );
  }
}

static void _Sysinit_Verbose_DRVMGR_LEVEL_1( void )
{
  if ( !SYSINIT_IS_ADJACENT( BSP_PRE_DRIVERS, DRVMGR_LEVEL_1 ) ) {
    printk( "sysinit: DRVMGR_LEVEL_1: done\n" );
  }
}

static void _Sysinit_Verbose_DEVICE_DRIVERS( void )
{
  if ( !SYSINIT_IS_ADJACENT( DRVMGR_LEVEL_1, DEVICE_DRIVERS ) ) {
    printk( "sysinit: DEVICE_DRIVERS: done\n" );
  }
}

static void _Sysinit_Verbose_DRVMGR_LEVEL_2( void )
{
  if ( !SYSINIT_IS_ADJACENT( DEVICE_DRIVERS, DRVMGR_LEVEL_2 ) ) {
    printk( "sysinit: DRVMGR_LEVEL_2: done\n" );
  }
}

static void _Sysinit_Verbose_DRVMGR_LEVEL_3( void )
{
  if ( !SYSINIT_IS_ADJACENT( DRVMGR_LEVEL_2, DRVMGR_LEVEL_3 ) ) {
    printk( "sysinit: DRVMGR_LEVEL_3: done\n" );
  }
}

static void _Sysinit_Verbose_DRVMGR_LEVEL_4( void )
{
  if ( !SYSINIT_IS_ADJACENT( DRVMGR_LEVEL_3, DRVMGR_LEVEL_4 ) ) {
    printk( "sysinit: DRVMGR_LEVEL_4: done\n" );
  }
}

static void _Sysinit_Verbose_MP_FINALIZE( void )
{
  if ( !SYSINIT_IS_ADJACENT( DRVMGR_LEVEL_4, MP_FINALIZE ) ) {
    printk( "sysinit: MP_FINALIZE: done\n" );
  }
}

static void _Sysinit_Verbose_CLASSIC_USER_TASKS( void )
{
  if ( !SYSINIT_IS_ADJACENT( MP_FINALIZE, CLASSIC_USER_TASKS ) ) {
    printk( "sysinit: CLASSIC_USER_TASKS: done\n" );
  }
}

static void _Sysinit_Verbose_POSIX_USER_THREADS( void )
{
  if ( !SYSINIT_IS_ADJACENT( CLASSIC_USER_TASKS, POSIX_USER_THREADS ) ) {
    printk( "sysinit: POSIX_USER_THREADS: done\n" );
  }
}

static void _Sysinit_Verbose_STD_FILE_DESCRIPTORS( void )
{
  if ( !SYSINIT_IS_ADJACENT( POSIX_USER_THREADS, STD_FILE_DESCRIPTORS ) ) {
    printk( "sysinit: STD_FILE_DESCRIPTORS: done\n" );
  }
}

static void _Sysinit_Verbose_LAST( void )
{
  printk( "sysinit: done\n" );
}
