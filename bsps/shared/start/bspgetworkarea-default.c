/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @ingroup RTEMSBSPsSharedStartup
 *
 * @brief This source file contains the default implementation of
 *   _Memory_Get().
 */

/*
 * Copyright (C) 2011, 2019 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Copyright (C) 1989, 2009 On-Line Applications Research Corporation (OAR)
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

#if defined(HAS_UBOOT) && !defined(BSP_DISABLE_UBOOT_WORK_AREA_CONFIG)
  #define USE_UBOOT
#endif

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];

/*
 *  We may get the size information from U-Boot or the linker scripts.
 */
#ifdef USE_UBOOT
#include <bsp/u-boot.h>
#include <rtems/sysinit.h>

static Memory_Area _Memory_Areas[ 1 ];

static void bsp_memory_initialize( void )
{
  char *end;

  end = (char *) bsp_uboot_board_info.bi_memstart
    + bsp_uboot_board_info.bi_memsize;
  _Memory_Initialize( &_Memory_Areas[ 0 ], WorkAreaBase, end );
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#else /* !USE_UBOOT */
extern char RamEnd[];

static Memory_Area _Memory_Areas[] = {
  MEMORY_INITIALIZER(WorkAreaBase, RamEnd)
};
#endif /* USE_UBOOT */

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
