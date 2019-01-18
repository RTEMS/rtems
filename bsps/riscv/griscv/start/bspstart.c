/*
 * Copyright (c) 2018 embedded brains GmbH
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <bsp/irq-generic.h>

#include <amba.h>
#include <rtems/sysinit.h>

#if defined(RTEMS_SMP) || defined(RTEMS_MULTIPROCESSING)
/* Irq used by shared memory driver and for inter-processor interrupts.
 * Can be overridden by being defined in the application.
 */
const unsigned char GRLIB_mp_irq __attribute__((weak)) = 14;
#endif

uint32_t GRLIB_Cpu_Index = 0;

#ifdef RTEMS_SMP
uint32_t riscv_hart_count = 1;
/* Index of the boot CPU. Set by the first CPU at boot to its CPU ID. */
int GRLIB_Boot_Cpu = -1;
#endif

void bsp_start(void)
{
  bsp_interrupt_initialize();
#ifdef RTEMS_SMP
  GRLIB_Cpu_Index = _CPU_SMP_Get_current_processor();
#endif
}

void amba_initialize(void);
struct ambapp_bus ambapp_plb;

void amba_initialize(void)
{
  ambapp_scan(&ambapp_plb, GRLIB_IO_AREA, NULL, NULL);
  gptimer_initialize();
  irqmp_initialize();
}

RTEMS_SYSINIT_ITEM(
  amba_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);
