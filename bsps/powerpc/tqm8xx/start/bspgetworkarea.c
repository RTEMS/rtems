/**
 * @file
 *
 * @ingroup tqm8xx
 *
 * @brief Source for BSP Get Work Area Memory
 */

/*
 * Copyright (c) 2008, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
