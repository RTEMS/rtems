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

#define ATSAM_ENABLE_ALARM_INTERRUPT RTC_IER_ALREN

static void set_rtc_alarm_interrupt(uint32_t interval)
{
	Rtc *rtc = RTC;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint32_t time;

	/* Clear current status register */
	RTC_ClearSCCR(rtc, 0x3F);

	RTC_GetTime(rtc, &hour, &minute, &second);

	time = UINT32_C(3600) * hour + UINT32_C(60) * minute + second;
	time = (time + interval) % (UINT32_C(24) * 3600);

	second = (uint8_t) (time % 60);
	minute = (uint8_t) ((time / 60) % 60);
	hour = (uint8_t) (time / 3600);

	if (interval < 60) {
		RTC_SetTimeAlarm(rtc, NULL, NULL, &second);
	} else if (interval < 3600) {
		RTC_SetTimeAlarm(rtc, NULL, &minute, &second);
	} else {
		RTC_SetTimeAlarm(rtc, &hour, &minute, &second);
	}

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
	rtems_interrupt_level level;
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
			/* Enable fast startup via RTC alarm */
			rtems_interrupt_disable(level);
			PMC->PMC_FSMR |= PMC_FSMR_RTCAL;
			rtems_interrupt_enable(level);

			rtc_alarm_handler(rtc_data);
			break;
		default:
			break;
	}
}
