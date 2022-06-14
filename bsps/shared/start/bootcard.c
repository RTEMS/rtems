/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedStartup
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <rtems/sysinit.h>

/*
 *  At most a single pointer to the cmdline for those target
 *  short on memory and not supporting a command line.
 */
const char *bsp_boot_cmdline;

RTEMS_SYSINIT_ITEM(
  bsp_start,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/*
 *  This is the initialization framework routine that weaves together
 *  calls to RTEMS and the BSP in the proper sequence to initialize
 *  the system while maximizing shared code and keeping BSP code in C
 *  as much as possible.
 */
void boot_card(
  const char *cmdline
)
{
  ISR_Level bsp_isr_level;

  /*
   * Make sure interrupts are disabled.  Directly use the CPU port API to allow
   * tracing of the higher level interrupt disable/enable routines, e.g.
   * _ISR_Local_disable() and _ISR_Local_enable().
   */
  _CPU_ISR_Disable( bsp_isr_level );
  (void) bsp_isr_level;

  bsp_boot_cmdline = cmdline;

  rtems_initialize_executive();

  /***************************************************************
   ***************************************************************
   *  APPLICATION RUNS NOW!!!  We will not return to here!!!     *
   ***************************************************************
   ***************************************************************/
}
