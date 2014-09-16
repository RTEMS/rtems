/**
 * @file
 *
 * @ingroup or1ksim
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2014 by Hesham ALMatary
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <libcpu/cache.h>

void bsp_start( void )
{
  _CPU_cache_enable_instruction();
  _CPU_cache_enable_data();
}
