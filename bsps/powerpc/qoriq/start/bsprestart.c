/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief BSP restart.
 */

/*
 * Copyright (C) 2016, 2018 embedded brains GmbH & Co. KG
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
#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>
#include <bsp/qoriq.h>

#include <libcpu/powerpc-utility.h>

#include <string.h>

static char fdt_copy[BSP_FDT_BLOB_SIZE_MAX];

static RTEMS_NO_RETURN void do_restart(void *addr)
{
  void (*restart)(uintptr_t);

  qoriq_reset_qman_and_bman();

  memcpy(fdt_copy, bsp_fdt_get(), sizeof(fdt_copy));
  rtems_cache_flush_multiple_data_lines(fdt_copy, sizeof(fdt_copy));

  restart = addr;
  (*restart)((uintptr_t) fdt_copy);
  bsp_fatal(QORIQ_FATAL_RESTART_FAILED);
}

#if defined(RTEMS_SMP) && !defined(QORIQ_IS_HYPERVISOR_GUEST)

#include <rtems/score/smpimpl.h>
#include <rtems/score/smpbarrier.h>

#define RESTART_IPI_INDEX 1

static SMP_barrier_Control restart_barrier = SMP_BARRIER_CONTROL_INITIALIZER;

static void restart_interrupt(void *arg)
{
  uint32_t cpu_self_index;
  uint32_t thread_index;
  rtems_interrupt_level level;
  SMP_barrier_State bs;

  rtems_interrupt_local_disable(level);
  (void) level;

  _SMP_barrier_State_initialize(&bs);
  _SMP_barrier_Wait(&restart_barrier, &bs, _SMP_Get_processor_maximum());

  cpu_self_index = rtems_scheduler_get_processor();
  thread_index = cpu_self_index % QORIQ_THREAD_COUNT;

  if (cpu_self_index == 0) {
    do_restart(arg);
  } else if (thread_index == 0) {
    uint32_t real_processor_index;
    const qoriq_start_spin_table *spin_table;

    real_processor_index = cpu_self_index / QORIQ_THREAD_COUNT;
    spin_table = qoriq_start_spin_table_addr[real_processor_index];

    qoriq_restart_secondary_processor(spin_table);
  } else {
    uint32_t pir_reset_value;

    /* Restore reset PIR value */
    pir_reset_value = (cpu_self_index & ~0x1U) << 2;
    PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_PIR, pir_reset_value);

    /* Thread Enable Clear (TENC) */
    PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_TENC, 1U << thread_index);

    RTEMS_UNREACHABLE();
  }
}

static void raise_restart_interrupt(void)
{
  qoriq.pic.ipidr[RESTART_IPI_INDEX].reg =
    _Processor_mask_To_uint32_t(_SMP_Get_online_processors(), 0);
  ppc_synchronize_data();
  ppc_synchronize_instructions();
}

void bsp_restart(void *addr)
{
  rtems_status_code sc;
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(qoriq_start_spin_table_addr); ++i) {
    qoriq_start_spin_table *spin_table;

    spin_table = qoriq_start_spin_table_addr[i];
    memset(spin_table, 0, sizeof(*spin_table));
    rtems_cache_flush_multiple_data_lines(spin_table, sizeof(*spin_table));
  }

  sc = rtems_interrupt_handler_install(
    QORIQ_IRQ_IPI_0 + RESTART_IPI_INDEX,
    "Restart",
    RTEMS_INTERRUPT_UNIQUE,
    restart_interrupt,
    addr
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(QORIQ_FATAL_RESTART_INSTALL_INTERRUPT);
  }

  raise_restart_interrupt();
  bsp_fatal(QORIQ_FATAL_RESTART_INTERRUPT_FAILED);
}

#else /* !RTEMS_SMP || QORIQ_IS_HYPERVISOR_GUEST */

void bsp_restart(void *addr)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;
  do_restart(addr);
}

#endif /* RTEMS_SMP && !QORIQ_IS_HYPERVISOR_GUEST */
