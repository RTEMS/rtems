/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief BSP Reset
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <bsp/bootcard.h>
#include <bspopts.h>

#include <rtems/rtems/intr.h>

#include <stdint.h>

#if BSP_MICROBLAZE_FPGA_WATCHDOG_BASE != 0
/*
 * The registers of the XPS and AXI Timebase Watchdog Timer.  The bit
 * EWDT2 is accessible through both control and status registers.
 */
#define WDT_TWCSR_EWDT2 0x1U
#define WDT_TWCSR0_EWDT1 0x2U
#define WDT_TWCSR0_WDS 0x4U
#define WDT_TWCSR0_WRS 0x8U

typedef struct {
  uint32_t twcsr0;
  uint32_t twcsr1;
  uint32_t tbr;
} microblaze_fpga_wdt;
#endif

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

#if BSP_MICROBLAZE_FPGA_WATCHDOG_BASE != 0
  volatile microblaze_fpga_wdt *wdt;
  rtems_interrupt_level         level;

  wdt = (volatile microblaze_fpga_wdt *) BSP_MICROBLAZE_FPGA_WATCHDOG_BASE;
  rtems_interrupt_local_disable( level );
  (void) level;

  /*
   * Writing a one to the watchdog state restarts the interval.  The first
   * expiry sets the watchdog state, the second one resets the system.
   */
  wdt->twcsr0 = WDT_TWCSR0_WRS | WDT_TWCSR0_WDS | WDT_TWCSR0_EWDT1;
  wdt->twcsr1 = WDT_TWCSR_EWDT2;

  while ( true ) {
    /* Wait for the reset */
  }
#else
  __asm__ volatile (
    "brai 0xFFFFFFFFFFFFFFFF"
  );
  RTEMS_UNREACHABLE();
#endif
}
