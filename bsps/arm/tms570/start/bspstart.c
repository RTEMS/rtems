/**
 * @file bspstart.c
 *
 * @ingroup tms570
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/tms570-pom.h>
#include <bsp/irq-generic.h>
#include <bsp/start.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>
#include <rtems/endian.h>

void bsp_start( void )
{
  void *need_remap_ptr;
  unsigned int need_remap_int;

  tms570_initialize_and_clear();

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
