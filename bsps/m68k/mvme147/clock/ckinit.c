/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine initializes the Tick Timer 2 on the MVME147 board.
 *  The tick frequency is 1 millisecond.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#include <stdlib.h>

#include <bsp.h>
#include <rtems/clockdrv.h>

#define MS_COUNT          65376    /* 1ms */
/* MS_COUNT = 0x10000 - 1e-3/6.25e-6 */
#define CLOCK_INT_LEVEL   6               /* T2's interrupt level */

uint32_t         Clock_isrs;                  /* ISRs until next tick */
volatile uint32_t         Clock_driver_ticks; /* ticks since initialization */
rtems_isr_entry  Old_ticker;

static void Clock_exit( void );

/*
 *  ISR Handler
 */
static rtems_isr Clock_isr(rtems_vector_number vector)
{
  Clock_driver_ticks += 1;
  pcc->timer2_int_control |= 0x80; /* Acknowledge interr. */

  if (Clock_isrs == 1) {
    rtems_clock_tick();
    Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;
  }
  else
    Clock_isrs -= 1;
}

static void Install_clock(rtems_isr_entry clock_isr )
{

  Clock_driver_ticks = 0;
  Clock_isrs = rtems_configuration_get_microseconds_per_tick() / 1000;

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, TIMER_2_VECTOR, 1 );

  pcc->timer2_int_control = 0x00; /* Disable T2 Interr. */
  pcc->timer2_preload = MS_COUNT;
  /* write preload value */
  pcc->timer2_control = 0x07; /* clear T2 overflow counter, enable counter */
  pcc->timer2_int_control = CLOCK_INT_LEVEL|0x08;
  /* Enable Timer 2 and set its int. level */

  atexit( Clock_exit );
}

void Clock_exit( void )
{
  pcc->timer2_int_control = 0x00; /* Disable T2 Interr. */
}

void _Clock_Initialize( void )
{
  Install_clock( Clock_isr );
}
