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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

void
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

rtems_device_driver
Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock (Clock_isr);

  return RTEMS_SUCCESSFUL;
}
