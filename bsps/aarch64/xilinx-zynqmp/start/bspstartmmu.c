/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the default MMU tables and setup.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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
#include <bsp/aarch64-mmu.h>
#include <libcpu/mmu-vmsav8-64.h>

#include <rtems/malloc.h>
#include <rtems/sysinit.h>

/*
 * Create an MMU table to get the R1 base and end. This avoids
 * relocation errors as the R1 addresses are in the upper A64 address
 * space.
 *
 * The aarch64_mmu_config_table table cannot be used because the regions
 * in that table have no identifiers to indicate which region is the
 * the DDRMC_region1_mem region.
 */
static const struct mem_region {
  uintptr_t begin;
  uintptr_t end;
} bsp_r1_region[] = {
  { /* DDRMC_region1_mem, if not used size is 0 and ignored */
    .begin = (uintptr_t) bsp_r1_ram_base,
    .end = (uintptr_t) bsp_r1_ram_end,
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void
zynqmp_setup_mmu_and_cache( void ) __attribute__ ((weak));

BSP_START_TEXT_SECTION void
zynqmp_setup_mmu_and_cache( void )
{
  aarch64_mmu_control *control = &aarch64_mmu_instance;

  aarch64_mmu_setup();

  aarch64_mmu_setup_translation_table(
    control,
    &aarch64_mmu_config_table[ 0 ],
    aarch64_mmu_config_table_size
  );

  aarch64_mmu_enable( control );
}

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_secondary_cpu_mmu_and_cache( void )
__attribute__ ( ( weak ) );

BSP_START_TEXT_SECTION void zynqmp_setup_secondary_cpu_mmu_and_cache( void )
{
  aarch64_mmu_control *control = &aarch64_mmu_instance;

  /* Perform basic MMU setup */
  aarch64_mmu_setup();
  aarch64_mmu_enable( control );
}

void bsp_r1_heap_extend(void);
void bsp_r1_heap_extend(void)
{
  const struct mem_region* r1 = &bsp_r1_region[0];
  if (r1->begin != r1->end) {
    rtems_status_code sc =
      rtems_heap_extend((void*) r1->begin, r1->end - r1->begin);
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_fatal(BSP_FATAL_HEAP_EXTEND_ERROR);
    }
  }
}

/*
 * Initialise after the IDLE thread exists so the protected heap
 * extend call has a valid context.
 */
RTEMS_SYSINIT_ITEM(
  bsp_r1_heap_extend,
  RTEMS_SYSINIT_IDLE_THREADS,
  RTEMS_SYSINIT_ORDER_LAST
);
