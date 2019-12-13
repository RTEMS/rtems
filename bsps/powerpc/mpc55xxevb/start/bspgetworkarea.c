/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

LINKER_SYMBOL(bsp_section_work_bonus_begin);
LINKER_SYMBOL(bsp_section_work_bonus_end);

static Memory_Area _Memory_Areas[] = {
  MEMORY_INITIALIZER(bsp_section_work_begin, bsp_section_work_end),
  MEMORY_INITIALIZER(bsp_section_work_bonus_begin, bsp_section_work_bonus_end)
};

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

const Memory_Information *_Memory_Get(void)
{
  return &_Memory_Information;
}
