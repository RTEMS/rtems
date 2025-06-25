/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Xen
 *
 * @brief This source file contains the default MMU tables and setup.
 */

/*
 * Copyright (C) 2025 On-Line Applications Research Corporation (OAR)
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

BSP_START_TEXT_SECTION void
xen_setup_mmu_and_cache( void )
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

BSP_START_TEXT_SECTION void xen_setup_secondary_cpu_mmu_and_cache( void )
{
  aarch64_mmu_control *control = &aarch64_mmu_instance;

  /* Perform basic MMU setup */
  aarch64_mmu_setup();
  aarch64_mmu_enable( control );
}
