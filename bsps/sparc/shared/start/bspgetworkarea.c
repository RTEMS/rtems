/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @brief This source file contains an implementation of _Memory_Get().
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
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

#include <bsp.h>

#include <rtems/score/memory.h>

/* This function is called only by the start sequence defined in start.S */
void _SPARC_Memory_initialize( void *end_of_usable_ram );

static Memory_Area _Memory_Areas[ 1 ];

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

void _SPARC_Memory_initialize( void *end_of_usable_ram )
{
  _Memory_Initialize( &_Memory_Areas[ 0 ], &end, end_of_usable_ram );
}

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
