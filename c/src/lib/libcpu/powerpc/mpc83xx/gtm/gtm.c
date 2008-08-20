/**
 * @file
 *
 * @brief Source file for timer functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
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

#define MPC83XX_GTM_VARIABLES( timer) \
	int module = (timer) / 4; \
	int module_timer = (timer) % 4; \
	int high = module_timer / 2; \
	int low = module_timer % 2; \
	rtems_interrupt_level level;

#define MPC83XX_GTM_CLOCK_MASK MPC83XX_GTM_CLOCK_EXTERN

static const uint8_t mpc83xx_gmt_interrupt_vector_table [MPC83XX_GTM_NUMBER] = { 90, 78, 84, 72, 91, 79, 85, 73 };

static const uint8_t mpc83xx_gmt_interrupt_priority_table [MPC83XX_GTM_NUMBER] = { 113, 77, 95, 59, 116, 80, 98, 62 };

rtems_status_code mpc83xx_gtm_initialize( int timer, int clock)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	MPC83XX_GTM_VARIABLES( timer);

	unsigned mask = 0xfU << (low * 4);
	unsigned flags = 0x3U << (low * 4);
	uint8_t reg = 0;

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);

	reg = mpc83xx.gtm [module].gtcfr [high].reg;
	mpc83xx.gtm [module].gtcfr [high].reg = (uint8_t) ((reg & ~mask) | flags);

	mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] = 0;

	rtems_interrupt_enable( level);

	sc = mpc83xx_gtm_set_clock( timer, clock);
	CHECK_SC( sc, "Set clock");

	sc = mpc83xx_gtm_set_value( timer, 0);
	CHECK_SC( sc, "Set value");

	sc = mpc83xx_gtm_set_reference( timer, 0);
	CHECK_SC( sc, "Set reference");

	sc = mpc83xx_gtm_set_prescale( timer, 0);
	CHECK_SC( sc, "Set prescale");

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_enable_restart( int timer, bool enable)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);

	if (enable) {
		mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] |= 0x0008;
	} else {
		mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] &= ~0x0008;
	}

	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_clock( int timer, int clock)
{
	MPC83XX_GTM_VARIABLES( timer);

	uint16_t reg = 0;

	MPC83XX_GTM_CHECK_INDEX( timer);

	if (clock & ~MPC83XX_GTM_CLOCK_MASK) {
		return RTEMS_INVALID_CLOCK;
	}

	rtems_interrupt_disable( level);

	reg = mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low];
	mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] = (reg & ~MPC83XX_GTM_CLOCK_MASK) | clock;

	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_clock( int timer, int *clock)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	*clock = mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] & MPC83XX_GTM_CLOCK_MASK;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_start( int timer)
{
	MPC83XX_GTM_VARIABLES( timer);

	uint8_t flags = 0x2 << (low * 4);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [module].gtcfr [high].reg &= ~flags;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_stop( int timer)
{
	MPC83XX_GTM_VARIABLES( timer);

	uint8_t flags = 0x2 << (low * 4);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [module].gtcfr [high].reg |= flags;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_value( int timer, uint16_t value)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [module].gt_tim_regs [high].gtcnr [low] = value;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_value( int timer, uint16_t *value)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	*value = mpc83xx.gtm [module].gt_tim_regs [high].gtcnr [low];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_reference( int timer, uint16_t reference)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [module].gt_tim_regs [high].gtrfr [low] = reference;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_reference( int timer, uint16_t *reference)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	*reference = mpc83xx.gtm [module].gt_tim_regs [high].gtrfr [low];

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_set_prescale( int timer, uint8_t prescale)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [module].gtpsr [module_timer] = prescale;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_get_prescale( int timer, uint8_t *prescale)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	*prescale = mpc83xx.gtm [module].gtpsr [module_timer];

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
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] |= 0x0010;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_disable( int timer)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	rtems_interrupt_disable( level);
	mpc83xx.gtm [module].gt_tim_regs [high].gtmdr [low] &= ~0x0010;
	rtems_interrupt_enable( level);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc83xx_gtm_interrupt_clear( int timer)
{
	MPC83XX_GTM_VARIABLES( timer);

	MPC83XX_GTM_CHECK_INDEX( timer);

	mpc83xx.gtm [module].gtevr [module_timer] = 0x0002;

	return RTEMS_SUCCESSFUL;
}
