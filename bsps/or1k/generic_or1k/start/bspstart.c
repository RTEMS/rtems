/**
 * @file
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2014-2015 by Hesham ALMatary
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_start( void )
{
  rtems_cache_enable_instruction();
  rtems_cache_enable_data();
}
