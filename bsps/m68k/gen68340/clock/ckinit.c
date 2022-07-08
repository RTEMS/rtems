/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This routine initializes the MC68340/349 Periodic Interval Timer
 */

/*
 * Based on the `gen68360' board support package, and covered by the
 * original distribution terms.
 *
 * Geoffroy Montel
 * France Telecom - CNET/DSM/TAM/CAT
 * 4, rue du Clos Courtel
 * 35512 CESSON-SEVIGNE
 * FRANCE
 *
 * e-mail: g_montel@yahoo.com
 */

/*
 * COPYRIGHT (c) 1989-2014.
 * On-Line Applications Research Corporation (OAR).
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

#include <stdlib.h>       /* for atexit() */
#include <bsp.h>
#include <m68340.h>
#include <rtems/clockdrv.h>

#define CLOCK_VECTOR     120 /* clock isr routine vector in the vbr */
#define CLOCK_IRQ_LEVEL  6   /* clock isr level */

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile uint32_t         Clock_driver_ticks;

/*
 * Periodic interval timer interrupt handler
 */
static rtems_isr
Clock_isr (rtems_vector_number vector)
{
  /*
   * Announce the clock tick
   */
  Clock_driver_ticks++;
  rtems_clock_tick();
}

static void
Clock_exit (void)
{
  /*
   * Turn off periodic interval timer
   */
  SIMPITR = 0;
}

static void
Install_clock (rtems_isr_entry clock_isr)
{
  uint32_t   pitr_tmp;
  uint32_t   usecs_per_tick;

  Clock_driver_ticks = 0;

  set_vector (clock_isr, CLOCK_VECTOR, 1);

  /* sets the Periodic Interrupt Control Register PICR */
  SIMPICR = ( CLOCK_IRQ_LEVEL << 8 ) | ( CLOCK_VECTOR );

  /* sets the PITR count value */
  /* this assumes a 32.765 kHz crystal */

  usecs_per_tick = rtems_configuration_get_microseconds_per_tick();
  /* find out whether prescaler should be enabled or not */
  if ( usecs_per_tick <= 31128 ) {
     pitr_tmp = ( usecs_per_tick * 8192 ) / 1000000 ;
  } else {
     pitr_tmp = ( usecs_per_tick / 1000000 ) * 16;
     /* enable it */
     pitr_tmp |= 0x100;
  }

  SIMPITR = (unsigned char) pitr_tmp;

  atexit (Clock_exit);
}

void _Clock_Initialize( void )
{
  Install_clock (Clock_isr);
}
