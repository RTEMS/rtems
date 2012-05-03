/*  clock.c
 *
 *  This routine initializes the interval timer on the
 *  PowerPC 405 CPU.  The tick frequency is specified by the bsp.
 *
 *  Author: Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1.1/clock/clock.c:
 *
 *  Modifications for deriving timer clock from cpu system clock by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 * Further modifications for PPC405GP/EX by Michael Hamel
 */

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>
#include <stdlib.h>                     /* for atexit() */
#include <rtems/bspIo.h>
#include <ppc405common.h>
#include <libcpu/cpuIdent.h>
#include <bsp/irq.h>



/* PPC405GP */
#define CPC0_CR1	0xB2
 #define CR1_CETE	0x00800000

/* PPC405EX */
#define	SDR0_C405	0x180
 #define SDR_CETE	0x02000000

volatile uint32_t   Clock_driver_ticks;
static uint32_t   pit_value, tick_time;

void Clock_exit( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/* Defined in bspstart.c */
extern uint32_t bsp_clicks_per_usec;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  ISR Handler
 */

void Clock_isr(void* handle)
{
	Clock_driver_ticks++;
    rtems_clock_tick();
}

int ClockIsOn(const rtems_irq_connect_data* unused)
{
    return ((mfspr(TCR) & PIE) != 0);
}


void ClockOff(const rtems_irq_connect_data* unused)
{
    register uint32_t   r;

	r = mfspr(TCR);
	mtspr(TCR, r & ~(PIE | ARE) );
}

void ClockOn(const rtems_irq_connect_data* unused)
{
    uint32_t   iocr, r;
	ppc_cpu_id_t cpu;
    Clock_driver_ticks = 0;

 	cpu = get_ppc_cpu_type();
	if (cpu==PPC_405GP) {
		iocr = mfdcr(CPC0_CR1);
		if (bsp_timer_internal_clock) iocr &= ~CR1_CETE	;/* timer clocked from system clock */
								 else iocr |=  CR1_CETE; /* select external timer clock */
		mtdcr(CPC0_CR1,iocr);
	} else if (cpu==PPC_405EX) {
		mfsdr(SDR0_C405,iocr);
		if (bsp_timer_internal_clock) iocr &= ~SDR_CETE	;/* timer clocked from system clock */
								 else iocr |=  SDR_CETE; /* select external timer clock */
		mtsdr(SDR0_C405,iocr);
	} else {
		printk("clock.c:unrecognised CPU");
		rtems_fatal_error_occurred(1);
	}

    pit_value = rtems_configuration_get_microseconds_per_tick() * bsp_clicks_per_usec;
	mtspr(PIT,pit_value);

	tick_time = mfspr(TBL) + pit_value;
	r = mfspr(TCR);
	mtspr(TCR, r | PIE | ARE);
}



void Install_clock(void (*clock_isr)(void *))
{

    /*
     * initialize the interval here
     * First tick is set to right amount of time in the future
     * Future ticks will be incremented over last value set
     * in order to provide consistent clicks in the face of
     * interrupt overhead
     */

	rtems_irq_connect_data clockIrqConnData;

	Clock_driver_ticks = 0;
	clockIrqConnData.on   = ClockOn;
	clockIrqConnData.off  = ClockOff;
	clockIrqConnData.isOn = ClockIsOn;
	clockIrqConnData.name = BSP_PIT;
	clockIrqConnData.hdl  = clock_isr;
	if ( ! BSP_install_rtems_irq_handler (&clockIrqConnData)) {
		 printk("Unable to connect Clock Irq handler\n");
		 rtems_fatal_error_occurred(1);
	}
    atexit(Clock_exit);
}

void
ReInstall_clock(void (*new_clock_isr)(void *))
{
	uint32_t   isrlevel = 0;
	rtems_irq_connect_data clockIrqConnData;

	rtems_interrupt_disable(isrlevel);
	clockIrqConnData.name = BSP_PIT;
	if ( ! BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
		printk("Unable to stop system clock\n");
		rtems_fatal_error_occurred(1);
	}

	BSP_remove_rtems_irq_handler (&clockIrqConnData);
	clockIrqConnData.on   = ClockOn;
	clockIrqConnData.off  = ClockOff;
	clockIrqConnData.isOn = ClockIsOn;
	clockIrqConnData.name = BSP_PIT;
	clockIrqConnData.hdl  = new_clock_isr;
	if (!BSP_install_rtems_irq_handler (&clockIrqConnData)) {
		printk("Unable to connect Clock Irq handler\n");
		rtems_fatal_error_occurred(1);
	}
	rtems_interrupt_enable(isrlevel);
}


/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 *
 * This will not work on the 405GP because
 * when bit's are set in TCR they can only be unset by a reset
 */

void Clock_exit(void)
{
    rtems_irq_connect_data clockIrqConnData;

    clockIrqConnData.name = BSP_PIT;
    if (!BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
      printk("Unable to stop system clock\n");
      rtems_fatal_error_occurred(1);
    }
    BSP_remove_rtems_irq_handler (&clockIrqConnData);
}


rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  /*
   * make major/minor avail to others such as shared memory driver
   */
  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
}
