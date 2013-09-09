/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <assert.h>

#include <rtems/bspsmp.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/mmu.h>
#include <bsp/qoriq.h>
#include <bsp/vectors.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

void _start_core_1(void);

#define CORE_COUNT 2

#define ONE_CORE(core) (1 << (core))

#define ALL_CORES ((1 << CORE_COUNT) - 1)

#define IPI_INDEX 0

#define TLB_BEGIN 8

#define TLB_END 16

#define TLB_COUNT (TLB_END - TLB_BEGIN)

/*
 * These values can be obtained with the debugger or a look into the
 * U-Boot sources (arch/powerpc/cpu/mpc85xx/release.S).
 */
#if 1
  #define BOOT_BEGIN 0x1fff0000
  #define BOOT_LAST  0x1fffffff
  #define SPIN_TABLE (BOOT_BEGIN + 0xf2a0)
#else
  #define BOOT_BEGIN 0x3fff0000
  #define BOOT_LAST  0x3fffffff
  #define SPIN_TABLE (BOOT_BEGIN + 0xf240)
#endif

#define TLB_BEGIN 8

#define TLB_END 16

#define TLB_COUNT (TLB_END - TLB_BEGIN)

typedef struct {
  uint32_t addr_upper;
  uint32_t addr_lower;
  uint32_t r3_upper;
  uint32_t r3_lower;
  uint32_t reserved;
  uint32_t pir;
  uint32_t r6_upper;
  uint32_t r6_lower;
} uboot_spin_table;

static uint32_t initial_core_1_stack[4096 / sizeof(uint32_t)];

static void mmu_config_undo(void)
{
  int i = 0;

  for (i = TLB_BEGIN; i < TLB_END; ++i) {
    qoriq_tlb1_invalidate(i);
  }
}

static void release_core_1(void)
{
  const Per_CPU_Control *second_cpu = _Per_CPU_Get_by_index(1);
  uboot_spin_table *spin_table = (uboot_spin_table *) SPIN_TABLE;
  qoriq_mmu_context mmu_context;

  qoriq_mmu_context_init(&mmu_context);
  qoriq_mmu_add(&mmu_context, BOOT_BEGIN, BOOT_LAST, 0, 0, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW);
  qoriq_mmu_partition(&mmu_context, TLB_COUNT);
  qoriq_mmu_write_to_tlb1(&mmu_context, TLB_BEGIN);

  spin_table->pir = 1;
  spin_table->r3_lower = (uint32_t) second_cpu->interrupt_stack_high;
  spin_table->addr_upper = 0;
  rtems_cache_flush_multiple_data_lines(spin_table, sizeof(*spin_table));
  ppc_synchronize_data();
  spin_table->addr_lower = (uint32_t) _start_core_1;
  rtems_cache_flush_multiple_data_lines(spin_table, sizeof(*spin_table));

  mmu_config_undo();
}

void qoriq_secondary_cpu_initialize(void)
{
  const Per_CPU_Control *second_cpu = _Per_CPU_Get_by_index(1);

  /* Disable decrementer */
  PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS(BOOKE_TCR, BOOKE_TCR_DIE);

  /* Initialize exception handler */
  ppc_exc_initialize_with_vector_base(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    (uintptr_t) second_cpu->interrupt_stack_low,
    rtems_configuration_get_interrupt_stack_size(),
    bsp_exc_vector_base
  );

  /* Now it is possible to make the code execute only */
  qoriq_mmu_change_perm(
    FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SX,
    FSL_EIS_MAS3_SR
  );

  /* Initialize interrupt support */
  bsp_interrupt_facility_initialize();

  bsp_interrupt_vector_enable(QORIQ_IRQ_IPI_0);

  rtems_smp_secondary_cpu_initialize();
}

static void ipi_handler(void *arg)
{
  rtems_smp_process_interrupt();
}

uint32_t bsp_smp_initialize(uint32_t configured_cpu_count)
{
  rtems_status_code sc;
  uint32_t cores = configured_cpu_count < CORE_COUNT ?
    configured_cpu_count : CORE_COUNT;

  sc = rtems_interrupt_handler_install(
    QORIQ_IRQ_IPI_0,
    "IPI",
    RTEMS_INTERRUPT_UNIQUE,
    ipi_handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  if (cores > 1) {
    release_core_1();
  }

  return cores;
}

void bsp_smp_broadcast_interrupt(void)
{
  uint32_t self = ppc_processor_id();
  qoriq.pic.per_cpu [self].ipidr [IPI_INDEX].reg = ALL_CORES;
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
  uint32_t self = ppc_processor_id();
  qoriq.pic.per_cpu [self].ipidr [IPI_INDEX].reg =
    ONE_CORE(target_processor_index);
}
