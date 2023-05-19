/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePerCPUData
 *
 * @brief This source file contains the per-CPU data linker set and its system
 *   initialization handler.
 */

/*
 * Copyright (C) 2019, 2021 embedded brains GmbH & Co. KG
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

#include <rtems/score/percpudata.h>
#include <rtems/score/interr.h>
#include <rtems/score/memory.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

#include <string.h>

RTEMS_LINKER_RWSET(
  _Per_CPU_Data,
#if defined(RTEMS_SMP)
  /*
   * In SMP configurations, prevent false cache line sharing of per-processor
   * data with a proper alignment.
   */
  RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )
#endif
  char
);

#if defined(RTEMS_SMP)
static void _Per_CPU_Data_initialize( void )
{
  uintptr_t size;

  size = RTEMS_LINKER_SET_SIZE( _Per_CPU_Data );

  if ( size > 0 ) {
    char                     *data_begin;
    const Memory_Information *mem;
    Per_CPU_Control          *cpu;
    uint32_t                  cpu_index;
    uint32_t                  cpu_max;

    /* Prevent an out of bounds warning in the memcpy() below */
    data_begin = RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data );
    RTEMS_OBFUSCATE_VARIABLE( data_begin );

    mem = _Memory_Get();
    cpu = _Per_CPU_Get_by_index( 0 );
    cpu->data = data_begin;

    cpu_max = rtems_configuration_get_maximum_processors();

    for ( cpu_index = 1 ; cpu_index < cpu_max ; ++cpu_index ) {
      cpu = _Per_CPU_Get_by_index( cpu_index );
      cpu->data = _Memory_Allocate( mem, size, CPU_CACHE_LINE_BYTES );

      if( cpu->data == NULL ) {
        _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA );
      }

      memcpy( cpu->data, data_begin, size);
    }
  }
}

RTEMS_SYSINIT_ITEM(
  _Per_CPU_Data_initialize,
  RTEMS_SYSINIT_PER_CPU_DATA,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
