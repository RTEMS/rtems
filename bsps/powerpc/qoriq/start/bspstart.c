/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief BSP startup.
 */

/*
 * Copyright (c) 2010, 2017 embedded brains GmbH.  All rights reserved.
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

#include <libfdt.h>

#include <rtems.h>
#include <rtems/config.h>
#include <rtems/counter.h>
#include <rtems/sysinit.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/console-termios.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/intercom.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>
#include <bsp/qoriq.h>
#include <bsp/vectors.h>

LINKER_SYMBOL(bsp_exc_vector_base);

qoriq_start_spin_table *
qoriq_start_spin_table_addr[QORIQ_CPU_COUNT / QORIQ_THREAD_COUNT];

/* Configuration parameters for console driver, ... */
unsigned int BSP_bus_frequency;

/* Configuration parameter for clock driver, ... */
uint32_t bsp_time_base_frequency;

uint32_t qoriq_clock_frequency;

uint32_t _CPU_Counter_frequency(void)
{
#ifdef __PPC_CPU_E6500__
  return qoriq_clock_frequency;
#else
  return bsp_time_base_frequency;
#endif
}

static void initialize_frequency_parameters(void)
{
  const void *fdt = bsp_fdt_get();
  int node;
  int len;
  fdt32_t *val_fdt;

  node = fdt_node_offset_by_prop_value(fdt, -1, "device_type", "cpu", 4);

  val_fdt = (fdt32_t *) fdt_getprop(fdt, node, "bus-frequency", &len);
  if (val_fdt == NULL || len != 4) {
    bsp_fatal(QORIQ_FATAL_FDT_NO_BUS_FREQUENCY);
  }
  BSP_bus_frequency = fdt32_to_cpu(*val_fdt) / QORIQ_BUS_CLOCK_DIVIDER;

  val_fdt = (fdt32_t *) fdt_getprop(fdt, node, "timebase-frequency", &len);
  if (val_fdt == NULL || len != 4) {
    bsp_fatal(QORIQ_FATAL_FDT_NO_BUS_FREQUENCY);
  }
  bsp_time_base_frequency = fdt32_to_cpu(*val_fdt);

  #ifdef __PPC_CPU_E6500__
    val_fdt = (fdt32_t *) fdt_getprop(fdt, node, "clock-frequency", &len);
    if (val_fdt == NULL || len != 4) {
      bsp_fatal(QORIQ_FATAL_FDT_NO_CLOCK_FREQUENCY);
    }
    qoriq_clock_frequency = fdt32_to_cpu(*val_fdt);
  #endif
}

#define MTIVPR(base) \
  __asm__ volatile ("mtivpr %0" : : "r" (base))

#ifdef __powerpc64__
#define VECTOR_TABLE_ENTRY_SIZE 32
#else
#define VECTOR_TABLE_ENTRY_SIZE 16
#endif

#define MTIVOR(vec, offset) \
  do { \
    __asm__ volatile ("mtspr " RTEMS_XSTRING(vec) ", %0" : : "r" (offset)); \
    offset += VECTOR_TABLE_ENTRY_SIZE; \
  } while (0)

void qoriq_initialize_exceptions(void *interrupt_stack_begin)
{
  uintptr_t addr;

  ppc_exc_initialize_interrupt_stack(
    (uintptr_t) interrupt_stack_begin
  );

  addr = (uintptr_t) bsp_exc_vector_base;
  MTIVPR(addr);
  MTIVOR(BOOKE_IVOR0,  addr);
  MTIVOR(BOOKE_IVOR1,  addr);
  MTIVOR(BOOKE_IVOR2,  addr);
  MTIVOR(BOOKE_IVOR3,  addr);
  MTIVOR(BOOKE_IVOR4,  addr);
  MTIVOR(BOOKE_IVOR5,  addr);
  MTIVOR(BOOKE_IVOR6,  addr);
#ifdef __PPC_CPU_E6500__
  MTIVOR(BOOKE_IVOR7,  addr);
#endif
  MTIVOR(BOOKE_IVOR8,  addr);
#ifdef __PPC_CPU_E6500__
  MTIVOR(BOOKE_IVOR9,  addr);
#endif
  MTIVOR(BOOKE_IVOR10, addr);
  MTIVOR(BOOKE_IVOR11, addr);
  MTIVOR(BOOKE_IVOR12, addr);
  MTIVOR(BOOKE_IVOR13, addr);
  MTIVOR(BOOKE_IVOR14, addr);
  MTIVOR(BOOKE_IVOR15, addr);
  MTIVOR(BOOKE_IVOR32, addr);
  MTIVOR(BOOKE_IVOR33, addr);
#ifndef __PPC_CPU_E6500__
  MTIVOR(BOOKE_IVOR34, addr);
#endif
  MTIVOR(BOOKE_IVOR35, addr);
#ifdef __PPC_CPU_E6500__
  MTIVOR(BOOKE_IVOR36, addr);
  MTIVOR(BOOKE_IVOR37, addr);
#ifndef QORIQ_IS_HYPERVISOR_GUEST
  MTIVOR(BOOKE_IVOR38, addr);
  MTIVOR(BOOKE_IVOR39, addr);
  MTIVOR(BOOKE_IVOR40, addr);
  MTIVOR(BOOKE_IVOR41, addr);
  MTIVOR(BOOKE_IVOR42, addr);
#endif
#endif
}

void bsp_start(void)
{
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  initialize_frequency_parameters();

  qoriq_initialize_exceptions(_ISR_Stack_area_begin);
  bsp_interrupt_initialize();

  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );

#ifndef QORIQ_IS_HYPERVISOR_GUEST
  /* Disable boot page translation */
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
  qoriq.lcc.bstar &= ~LCC_BSTAR_EN;
#else
  qoriq.lcc.bptr &= ~BPTR_EN;
#endif
#endif
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
#ifndef QORIQ_IS_HYPERVISOR_GUEST
  return intr[0] - 16;
#else
  return intr[0];
#endif
}

#ifdef RTEMS_MULTIPROCESSING
RTEMS_SYSINIT_ITEM(
  qoriq_intercom_init,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
