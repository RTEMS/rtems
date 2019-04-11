/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief BSP restart.
 */

/*
 * Copyright (c) 2016, 2018 embedded brains GmbH.  All rights reserved.
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
