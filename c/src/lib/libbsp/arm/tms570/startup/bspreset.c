/**
 * @file bspreset.c
 *
 * @ingroup tms570
 *
 * @brief Reset code.
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

#include <rtems.h>

#include <bsp/bootcard.h>
#include <bsp/tms570.h>
#include <bsp/start.h>

BSP_START_TEXT_SECTION __attribute__( ( flatten ) ) void bsp_reset( void )
{
  while ( true ) {
    /* Do nothing */
  }
}
