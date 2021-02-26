/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @ingroup tqm8xx
 *
 * @brief This source file contains the powerpc/tqm8xx implementation of
 *   _Memory_Get().
 */

/*
 * Copyright (C) 2008, 2019 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Copyright (C) 2008, 2009 On-Line Applications Research Corporation (OAR)
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
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

#include <rtems/sysinit.h>

static Memory_Area _Memory_Areas[1];

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

static void bsp_memory_initialize(void)
{
  void *end = (void *) (TQM_BD_INFO.sdram_size - (uintptr_t)TopRamReserved);

  _Memory_Initialize(&_Memory_Areas[0], bsp_section_work_begin, end);
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

const Memory_Information *_Memory_Get(void)
{
  return &_Memory_Information;
}
