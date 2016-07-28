/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/power.h>
#include <bsp/irq.h>

#include <libchip/chip.h>

#define ATSAM_ENABLE_ALARM_INTERRUPT (1u << 1)

static void set_rtc_alarm_interrupt(uint8_t interval)
{
	Rtc *rtc = RTC;
	rtems_time_of_day tod;

	/* Clear current status register */
	RTC_ClearSCCR(rtc, 0x3F);

	atsam_rtc_get_time(&tod);
	tod.second = (tod.second + interval) % 60;
	tod.second = (((tod.second / 10) << 4) | (tod.second % 10));

	rtc->RTC_TIMALR &= ~RTC_TIMALR_SECEN;
	rtc->RTC_TIMALR = tod.second;
	rtc->RTC_TIMALR |= RTC_TIMALR_SECEN;
	RTC_EnableIt(rtc, ATSAM_ENABLE_ALARM_INTERRUPT);
}

static void rtc_interrupt_handler(void *arg)
{
	atsam_power_data_rtc_driver *rtc_data;

	rtc_data = (atsam_power_data_rtc_driver *)arg;
	set_rtc_alarm_interrupt(rtc_data->interval);
}

static void rtc_alarm_handler(void *arg)
{
	Rtc *rtc = RTC;
	rtems_status_code sc;

	/* Clear current status register */
	RTC_ClearSCCR(rtc, 0x3F);

	/* Switch off all RTC interrupts */
	RTC_DisableIt(rtc, 0x1F);

	/* Install RTC interrupt handler */
	sc = rtems_interrupt_handler_install(RTC_IRQn,
	    "RTC",
	    RTEMS_INTERRUPT_UNIQUE,
	    rtc_interrupt_handler,
	    arg
	);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void set_time(void)
{
	rtems_time_of_day tod;
	rtems_status_code sc;

	atsam_rtc_get_time(&tod);
	sc = rtems_clock_set(&tod);
	assert(sc == RTEMS_SUCCESSFUL);
}

void atsam_power_handler_rtc_driver(
    const atsam_power_control *control,
    atsam_power_state state
)
{
	atsam_power_data_rtc_driver *rtc_data;
	Rtc *rtc = RTC;

	rtc_data = (atsam_power_data_rtc_driver *)control->data.arg;

	switch (state) {
		case ATSAM_POWER_ON:
			RTC_DisableIt(rtc, ATSAM_ENABLE_ALARM_INTERRUPT);
			set_time();
			break;
		case ATSAM_POWER_OFF:
			set_rtc_alarm_interrupt(rtc_data->interval);
			break;
		case ATSAM_POWER_INIT:
			rtc_alarm_handler(rtc_data);
			break;
		default:
			break;
	}
}
