/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine initailizes the periodic interrupt timer on
 *  the Motorola 68332.
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

#include <stdlib.h>
#include <bsp.h>
#include <mrm332.h>
#include <rtems/clockdrv.h>
#include <rtems/m68k/sim.h>

#define CLOCK_VECTOR   MRM_PIV

uint32_t                Clock_isrs;         /* ISRs until next tick */
volatile uint32_t       Clock_driver_ticks; /* ticks since initialization */
static rtems_isr_entry  Old_ticker;

static void Clock_exit( void );

static rtems_isr Clock_isr(rtems_vector_number vector)
{
  Clock_driver_ticks += 1;

  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;
  }
  else
    Clock_isrs -= 1;
}

static void Install_clock(rtems_isr_entry clock_isr)
{
  Clock_driver_ticks = 0;
  Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );

  /* enable 1mS interrupts */
  *PITR = (unsigned short int)( SAM(0x09,0,PITM) );/* load counter */
  *PICR = (unsigned short int)                     /* enable interrupt */
    ( SAM(ISRL_PIT,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );

  atexit( Clock_exit );
}

void Clock_exit( void )
{
  /* shutdown the periodic interrupt */
  *PICR = (unsigned short int)
    ( SAM(0,8,PIRQL) | SAM(CLOCK_VECTOR,0,PIV) );
  /*     ^^ zero disables interrupt */

  /* do not restore old vector */
}

void _Clock_Initialize( void )
{
  Install_clock( Clock_isr );
}
