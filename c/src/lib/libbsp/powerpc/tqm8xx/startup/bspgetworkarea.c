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

void bsp_work_area_initialize(void)
{
  char *ram_end = (char *) (TQM_BD_INFO.sdram_size - (uint32_t)TopRamReserved);
  void *area_start = bsp_section_work_begin;
  uintptr_t area_size = (uintptr_t) ram_end - (uintptr_t) area_start;

  bsp_work_area_initialize_default( area_start, area_size );
}
