/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
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

#include <rtems/config.h>
#include <rtems/counter.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/*
 * Configuration parameter for clock driver.  The Trace32 PowerPC simulator has
 * an odd decrementer frequency.  The time base frequency is one tick per
 * instruction.  The decrementer frequency is one tick per ten instructions.
 * The clock driver assumes that the time base and decrementer frequencies are
 * equal.  For now we simulate processor that issues 10000000 instructions per
 * second.
 */
uint32_t bsp_time_base_frequency = 10000000;

#define MTIVPR(base) \
  __asm__ volatile ("mtivpr %0" : : "r" (base))

#define VECTOR_TABLE_ENTRY_SIZE 16

#define MTIVOR(vec, offset) \
  do { \
    __asm__ volatile ("mtspr " RTEMS_XSTRING(vec) ", %0" : : "r" (offset)); \
    offset += VECTOR_TABLE_ENTRY_SIZE; \
  } while (0)

static void t32mppc_initialize_exceptions(void)
{
  uintptr_t addr;

  ppc_exc_initialize_interrupt_stack(
    (uintptr_t) _ISR_Stack_area_begin
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
  MTIVOR(BOOKE_IVOR7,  addr);
  MTIVOR(BOOKE_IVOR8,  addr);
  MTIVOR(BOOKE_IVOR9,  addr);
  MTIVOR(BOOKE_IVOR10, addr);
  MTIVOR(BOOKE_IVOR11, addr);
  MTIVOR(BOOKE_IVOR12, addr);
  MTIVOR(BOOKE_IVOR13, addr);
  MTIVOR(BOOKE_IVOR14, addr);
  MTIVOR(BOOKE_IVOR15, addr);
  MTIVOR(BOOKE_IVOR32, addr);
  MTIVOR(BOOKE_IVOR33, addr);
  MTIVOR(BOOKE_IVOR34, addr);
  MTIVOR(BOOKE_IVOR35, addr);
}

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_time_base_frequency;
}

void bsp_start(void)
{
  get_ppc_cpu_type();
  get_ppc_cpu_revision();
  t32mppc_initialize_exceptions();
  bsp_interrupt_initialize();
}
