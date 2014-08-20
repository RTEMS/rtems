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

void bsp_start( void )
{
  /* set the cpu mode to supervisor and big endian */
  arm_cpu_mode = 0x213;

  tms570_pom_remap();

  /* Interrupts */
  bsp_interrupt_initialize();

}
