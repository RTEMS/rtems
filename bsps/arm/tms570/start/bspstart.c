/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the bsp_start() implementation.
 */

/*
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <bsp/tms570-pom.h>
#include <bsp/irq-generic.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

void bsp_start( void )
{
  void *need_remap_ptr;
  unsigned int need_remap_int;

  tms570_pom_initialize_and_clear();

  /*
   * If RTEMS image does not start at address 0x00000000
   * then first level exception table at memory begin has
   * to be replaced to point to RTEMS handlers addresses.
   *
   * There is no VBAR or other option because Cortex-R
   * does provides only fixed address 0x00000000 for exceptions
   * (0xFFFF0000-0xFFFF001C alternative SCTLR.V = 1 cannot
   * be used because target area corersponds to PMM peripheral
   * registers on TMS570).
   *
   * Alternative is to use jumps over SRAM based trampolines
   * but that is not compatible with
   *   Check TCRAM1 ECC error detection logic
   * which intentionally introduces data abort during startup
   * to check SRAM and if exception processing goes through
   * SRAM then it leads to CPU error halt.
   *
   * So use of POM to replace jumps to vectors target
   * addresses seems to be the best option for now.
   *
   * The passing of linker symbol (represented as start address
   * of global array) through dummy asm block ensures that C compiler
   * cannot optimize comparison out on premise that reference cannot
   * evaluate to NULL definition in standard.
   */
  need_remap_ptr = bsp_start_vector_table_begin;
  asm volatile ("\n": "=r" (need_remap_int): "0" (need_remap_ptr));
  if ( need_remap_int != 0 ) {
    tms570_pom_remap();
  }

  /* Interrupts */
  bsp_interrupt_initialize();

}
