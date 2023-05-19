/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Source file for timer functions.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#include <rtems/bspIo.h>

#include <mpc83xx/mpc83xx.h>
#include <mpc83xx/gtm.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define MPC83XX_GTM_CHECK_INDEX( timer) \
	if (( timer) < 0 || ( timer) >= MPC83XX_GTM_NUMBER) { \
		return RTEMS_INVALID_NUMBER; \
	}

#define GTM_MODULE(timer)       ((timer)/4)
#define GTM_MODULE_TIMER(timer) ((timer)%4)
#define GTM_HIGH(timer)         (GTM_MODULE_TIMER(timer)/2)
#define GTM_LOW(timer)          (GTM_MODULE_TIMER(timer)%2)

#define MPC83XX_GTM_CLOCK_MASK MPC83XX_GTM_CLOCK_EXTERN

static const uint8_t mpc83xx_gmt_interrupt_vector_table [MPC83XX_GTM_NUMBER] = { 90, 78, 84, 72, 91, 79, 85, 73 };

rtems_status_code mpc83xx_gtm_initialize( int timer, int clock)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_interrupt_level level;

	unsigned mask = 0xfU << (GTM_LOW(timer) * 4);
	unsigned flags = 0x3U << (GTM_LOW(timer) * 4);
	uint8_t reg = 0;

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);

	reg = mpc83xx.gtm [GTM_MODULE(timer)].gtcfr [GTM_HIGH(timer)].reg;
	mpc83xx.gtm [GTM_MODULE(timer)].gtcfr [GTM_HIGH(timer)].reg = 
	  (uint8_t) ((reg & ~mask) | flags);

	mpc83xx.gtm [GTM_MODULE(timer)]
	  .gt_tim_regs [GTM_HIGH(timer)]
	  .gtmdr [GTM_LOW(timer)] = 0;

	rtems_interrupt_enable( level);

	sc = mpc83xx_gtm_set_clock( timer, clock);
	RTEMS_CHECK_SC( sc, "Set clock");

	sc = mpc83xx_gtm_set_value( timer, 0);
	RTEMS_CHECK_SC( sc, "Set value");

	sc = mpc83xx_gtm_set_reference( timer, 0);
	RTEMS_CHECK_SC( sc, "Set reference");

	sc = mpc83xx_gtm_set_prescale( timer, 0);
	RTEMS_CHECK_SC( sc, "Set prescale");

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_enable_restart( int timer, bool enable)
{
	rtems_interrupt_level level;
	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);

	if (enable) {
		mpc83xx.gtm [GTM_MODULE(timer)]
		  .gt_tim_regs [GTM_HIGH(timer)]
		  .gtmdr [GTM_LOW(timer)] |= 0x0008;
	} else {
		mpc83xx.gtm [GTM_MODULE(timer)]
		  .gt_tim_regs [GTM_HIGH(timer)]
		  .gtmdr [GTM_LOW(timer)] &= ~0x0008;
	}

	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_clock( int timer, int clock)
{
	rtems_interrupt_level level;
	uint16_t reg = 0;

	MPC83XX_GTM_CHECK_INDEX( timer);

	if (clock & ~MPC83XX_GTM_CLOCK_MASK) {
		return RTEMS_INVALID_CLOCK;
	}

	rtems_interrupt_disable( level);

	reg = mpc83xx.gtm [GTM_MODULE(timer)]
	  .gt_tim_regs [GTM_HIGH(timer)]
	  .gtmdr [GTM_LOW(timer)];
	mpc83xx.gtm [GTM_MODULE(timer)]
	  .gt_tim_regs [GTM_HIGH(timer)]
	  .gtmdr [GTM_LOW(timer)] = (reg & ~MPC83XX_GTM_CLOCK_MASK) | clock;

	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_clock( int timer, int *clock)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	*clock = mpc83xx.gtm [GTM_MODULE(timer)]
	  .gt_tim_regs [GTM_HIGH(timer)]
	  .gtmdr [GTM_LOW(timer)] & MPC83XX_GTM_CLOCK_MASK;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_start( int timer)
{
	rtems_interrupt_level level;
	uint8_t flags = 0x2 << (GTM_LOW(timer) * 4);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [GTM_MODULE(timer)]
.gtcfr [GTM_HIGH(timer)].reg &= ~flags;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_stop( int timer)
{
	rtems_interrupt_level level;
	uint8_t flags = 0x2 << (GTM_LOW(timer) * 4);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [GTM_MODULE(timer)].gtcfr [GTM_HIGH(timer)].reg |= flags;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_value( int timer, uint16_t value)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtcnr [GTM_LOW(timer)] = value;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_value( int timer, uint16_t *value)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	*value = mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtcnr [GTM_LOW(timer)];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_reference( int timer, uint16_t reference)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtrfr [GTM_LOW(timer)] = reference;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_reference( int timer, uint16_t *reference)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	*reference = mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtrfr [GTM_LOW(timer)];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_prescale( int timer, uint8_t prescale)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [GTM_MODULE(timer)].gtpsr [GTM_MODULE_TIMER(timer)] = prescale;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_prescale( int timer, uint8_t *prescale)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	*prescale = mpc83xx.gtm [GTM_MODULE(timer)].gtpsr [GTM_MODULE_TIMER(timer)];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_get_vector( int timer, rtems_vector_number *vector)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	*vector = mpc83xx_gmt_interrupt_vector_table [timer];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_enable( int timer)
{
	rtems_interrupt_level level;
	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtmdr [GTM_LOW(timer)] |= 0x0010;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_disable( int timer)
{
	rtems_interrupt_level level;
	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [GTM_MODULE(timer)].gt_tim_regs [GTM_HIGH(timer)].gtmdr [GTM_LOW(timer)] &= ~0x0010;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_clear( int timer)
{
	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [GTM_MODULE(timer)].gtevr [GTM_MODULE_TIMER(timer)] = 0x0002;

	return RTEMS_SUCCESSFUL;
}
