/**
 * @file
 *
 * @ingroup tqm8xx
 *
 * @brief Source for BSP Get Work Area Memory
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_work_area_initialize(void)
{
  char *ram_end = (char *) (TQM_BD_INFO.sdram_size - (uint32_t)TopRamReserved);
  void *area_start = bsp_work_area_start;
  uintptr_t area_size = (uintptr_t) ram_end - (uintptr_t) bsp_work_area_start;

  bsp_work_area_initialize_default( area_start, area_size );
}
