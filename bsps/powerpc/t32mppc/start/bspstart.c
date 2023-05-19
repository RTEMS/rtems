/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2017 embedded brains GmbH & Co. KG
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
