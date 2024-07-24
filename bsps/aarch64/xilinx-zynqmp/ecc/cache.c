/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of zynqmp_ecc_init().
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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
#include <bsp/ecc_priv.h>
#include <bsp/irq.h>
#include <bsp/utility.h>
#include <rtems/score/aarch64-system-registers.h>

static Cache_Error_RAM_ID get_l1_ramid(uint64_t ramid)
{
  switch (ramid) {
  case 0x0:
    return RAM_ID_L1I_TAG;
  case 0x1:
    return RAM_ID_L1I_DATA;
  case 0x8:
    return RAM_ID_L1D_TAG;
  case 0x9:
    return RAM_ID_L1D_DATA;
  case 0xa:
    return RAM_ID_L1D_DIRTY;
  case 0x18:
    return RAM_ID_TLB;
  default:
    return RAM_ID_UNKNOWN;
  }
}

static Cache_Error_RAM_ID get_l2_ramid(uint64_t ramid)
{
  switch (ramid) {
  case 0x10:
    return RAM_ID_L2_TAG;
  case 0x11:
    return RAM_ID_L2_DATA;
  case 0x12:
    return RAM_ID_SCU;
  default:
    return RAM_ID_UNKNOWN;
  }
}

static void cache_handler(void *arg)
{
  uint64_t l1val = _AArch64_Read_cpumerrsr_el1();
  _AArch64_Write_cpumerrsr_el1(l1val);
  uint64_t l2val = _AArch64_Read_l2merrsr_el1();
  _AArch64_Write_l2merrsr_el1(l2val);

  (void) arg;

  if (l1val & AARCH64_CPUMERRSR_EL1_VALID) {
    /* parse L1 data */
    Cache_Error_Info cerr = {0, };
    cerr.abort = l1val & AARCH64_CPUMERRSR_EL1_FATAL;
    cerr.repeats = AARCH64_CPUMERRSR_EL1_REPEATERR_GET(l1val);
    cerr.other_errors = AARCH64_CPUMERRSR_EL1_OTHERERR_GET(l1val);
    cerr.ramid = get_l1_ramid(AARCH64_CPUMERRSR_EL1_RAMID_GET(l1val));
    cerr.segment = AARCH64_CPUMERRSR_EL1_CPUIDWAY_GET(l1val);
    cerr.address = AARCH64_CPUMERRSR_EL1_ADDR_GET(l1val);

    zynqmp_invoke_ecc_handler(L1_CACHE, &cerr);
  }

  if (l2val & AARCH64_L2MERRSR_EL1_VALID) {
    /* parse L2 data */
    Cache_Error_Info cerr = {0, };
    cerr.abort = l2val & AARCH64_L2MERRSR_EL1_FATAL;
    cerr.repeats = AARCH64_L2MERRSR_EL1_REPEATERR_GET(l2val);
    cerr.other_errors = AARCH64_L2MERRSR_EL1_OTHERERR_GET(l2val);
    cerr.ramid = get_l2_ramid(AARCH64_L2MERRSR_EL1_RAMID_GET(l2val));
    cerr.segment = AARCH64_L2MERRSR_EL1_CPUIDWAY_GET(l2val);
    cerr.address = AARCH64_L2MERRSR_EL1_ADDR_GET(l2val);

    zynqmp_invoke_ecc_handler(L2_CACHE, &cerr);
  }
}

static rtems_interrupt_entry zynqmp_cache_ecc_entry;

rtems_status_code zynqmp_configure_cache_ecc( void )
{
  rtems_interrupt_entry_initialize(
    &zynqmp_cache_ecc_entry,
    cache_handler,
    NULL,
    "L1/L2 Cache Errors"
  );

  return rtems_interrupt_entry_install(
    ZYNQMP_IRQ_CACHE,
    RTEMS_INTERRUPT_SHARED,
    &zynqmp_cache_ecc_entry
  );
}
