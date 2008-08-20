/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Enhanced Direct Memory Access (eDMA).
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

#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>
#include <mpc55xx/mpc55xx.h>

#include <bsp/irq.h>

#include <string.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define MPC55XX_EDMA_CHANNEL_NUMBER 64
#define MPC55XX_EDMA_INVALID_CHANNEL UINT8_MAX
#define MPC55XX_EDMA_IS_CHANNEL_INVALID( i) ((i) < 0 || (i) >= MPC55XX_EDMA_CHANNEL_NUMBER)

#define MPC55XX_EDMA_IRQ_PRIORITY MPC55XX_INTC_MIN_PRIORITY

typedef struct {
	uint8_t channel;
	rtems_id transfer_update;
	uint32_t *error_status;
} mpc55xx_edma_channel_entry;

static mpc55xx_edma_channel_entry mpc55xx_edma_channel_table [MPC55XX_EDMA_CHANNEL_NUMBER];

static uint32_t mpc55xx_edma_channel_occupation_low = 0;

static uint32_t mpc55xx_edma_channel_occupation_high = 0;

static rtems_id mpc55xx_edma_channel_occupation_mutex = RTEMS_ID_NONE;

static uint8_t mpc55xx_edma_irq_error_low_channel = 0;

static uint8_t mpc55xx_edma_irq_error_high_channel = 32;

static void mpc55xx_edma_irq_handler( rtems_vector_number vector, void *data)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	mpc55xx_edma_channel_entry *e = (mpc55xx_edma_channel_entry *) data;
#ifdef DEBUG
	uint32_t citer = EDMA.TCD [e->channel].CITERE_LINK ? EDMA.TCD [e->channel].CITER & EDMA_TCD_BITER_LINKED_MASK : EDMA.TCD [e->channel].CITER;
	DEBUG_PRINT( "Channel %i (CITER = %i)\n", e->channel, citer);
#endif /* DEBUG */
	EDMA.CIRQR.R = e->channel;
	sc = rtems_semaphore_release( e->transfer_update);
	SYSLOG_WARNING_SC( sc, "Transfer update semaphore release");
}

static void mpc55xx_edma_irq_update_error_table( uint8_t *link_table, uint8_t *error_table, int channel)
{
	int i = 0;
	error_table [channel] = 1;
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_NUMBER; ++i) {
		if (channel == link_table [i] && error_table [i] == 0) {
			mpc55xx_edma_irq_update_error_table( link_table, error_table, i);
		}
	}
}

static void mpc55xx_edma_irq_error_handler( rtems_vector_number vector, void *data)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	uint8_t channel_start = *((uint8_t *) data);
	uint8_t channel_end = (uint8_t) (channel_start + 32);
	int i = 0;
	uint32_t mask = 0x1;
	uint32_t error_register = 0;
	uint8_t channel_link_table [MPC55XX_EDMA_CHANNEL_NUMBER];
	uint8_t channel_error_table [MPC55XX_EDMA_CHANNEL_NUMBER];

	/* Error register */
	if (channel_start < 32) {
		error_register = EDMA.ERL.R;
	} else if (channel_start < 64) {
		error_register = EDMA.ERH.R;
	}
	DEBUG_PRINT( "Error register %s: 0x%08x\n", channel_start < 32 ? "low" : "high", error_register);

	/* Fill channel link table */
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_NUMBER; ++i) {
		if (EDMA.TCD [i].BITERE_LINK && EDMA.TCD [i].CITER != EDMA.TCD [i].BITER) {
			channel_link_table [i] = (uint8_t) EDMA_TCD_BITER_LINK( i);
		} else if (EDMA.TCD [i].MAJORE_LINK && EDMA.TCD [i].CITER == EDMA.TCD [i].BITER) {
			channel_link_table [i] = EDMA.TCD [i].MAJORLINKCH;
		} else {
			channel_link_table [i] = MPC55XX_EDMA_INVALID_CHANNEL;
		}
		channel_error_table [i] = 0;
	}

	/* Search for channels with errors */
	for (i = channel_start; i < channel_end; ++i) {
		if ((error_register & mask) != 0) {
			mpc55xx_edma_irq_update_error_table( channel_link_table, channel_error_table, i);
		}
		mask <<= 1;
	}

	/* Process the channels related to errors */
	error_register = EDMA.ESR.R;
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_NUMBER; ++i) {
		if (channel_error_table [i]) {
			mpc55xx_edma_channel_entry *e = &mpc55xx_edma_channel_table [i];
			if (e->error_status != NULL) {
				*e->error_status = error_register;
			}
			sc = mpc55xx_edma_enable_hardware_requests( i, false);
			SYSLOG_ERROR_SC( sc, "Disable hardware requests, channel = %i", i);
			sc = rtems_semaphore_release( e->transfer_update);
			SYSLOG_WARNING_SC( sc, "Transfer update semaphore release, channel = %i", i);
		}
	}

	/* Clear the error interrupt requests */
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_NUMBER; ++i) {
		if (channel_error_table [i]) {
			EDMA.CER.R = (uint8_t) i;
		}
	}
}

rtems_status_code mpc55xx_edma_enable_hardware_requests( int channel, bool enable)
{
	if (MPC55XX_EDMA_IS_CHANNEL_INVALID( channel)) {
		return RTEMS_INVALID_NUMBER;
	}
	if (enable) {
		EDMA.SERQR.R = (uint8_t) channel;
	} else {
		EDMA.CERQR.R = (uint8_t) channel;
	}
	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_enable_error_interrupts( int channel, bool enable)
{
	if (MPC55XX_EDMA_IS_CHANNEL_INVALID( channel)) {
		return RTEMS_INVALID_NUMBER;
	}
	if (enable) {
		EDMA.SEEIR.R = channel;
	} else {
		EDMA.CEEIR.R = channel;
	}
	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_init()
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int i = 0;

	/* Channel occupation mutex */
	sc = rtems_semaphore_create (
		rtems_build_name ( 'D', 'M', 'A', 'O'),
		1,
		RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
		RTEMS_NO_PRIORITY,
		&mpc55xx_edma_channel_occupation_mutex
	);
	CHECK_SC( sc, "Create channel occupation mutex");

	/* Arbitration mode: round robin */
	EDMA.CR.B.ERCA = 1;
	EDMA.CR.B.ERGA = 1;

	/* Clear TCDs */
	memset( &EDMA.TCD [0], 0, sizeof( EDMA.TCD));

	/* Channel table */
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_NUMBER; ++i) {
		mpc55xx_edma_channel_table [i].channel = i;
		mpc55xx_edma_channel_table [i].transfer_update = RTEMS_ID_NONE;
		mpc55xx_edma_channel_table [i].error_status = NULL;
	}

	/* Error interrupt handler */
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_ERROR_LOW,
		MPC55XX_EDMA_IRQ_PRIORITY,
		"eDMA Error (Low)",
		RTEMS_INTERRUPT_UNIQUE,
		mpc55xx_edma_irq_error_handler,
		&mpc55xx_edma_irq_error_low_channel
	);
	CHECK_SC( sc, "Install low error interrupt handler");
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_ERROR_HIGH,
		MPC55XX_EDMA_IRQ_PRIORITY,
		"eDMA Error (High)",
		RTEMS_INTERRUPT_UNIQUE,
		mpc55xx_edma_irq_error_handler,
		&mpc55xx_edma_irq_error_high_channel
	);
	CHECK_SC( sc, "Install high error interrupt handler");

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_obtain_channel( int channel, uint32_t *error_status, rtems_id transfer_update)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int channel_occupied = 1;

	if (MPC55XX_EDMA_IS_CHANNEL_INVALID( channel)) {
		return RTEMS_INVALID_NUMBER;
	}

	/* Check occupation */
	sc = rtems_semaphore_obtain( mpc55xx_edma_channel_occupation_mutex, RTEMS_WAIT, 0);
	CHECK_SC( sc, "Obtain channel occupation mutex");
	if (channel < 32) {
		channel_occupied = mpc55xx_edma_channel_occupation_low & (0x1 << channel);
		if (!channel_occupied) {
			mpc55xx_edma_channel_occupation_low |= 0x1 << channel;
		}
	} else if (channel < 64) {
		channel_occupied = mpc55xx_edma_channel_occupation_high & (0x1 << (channel - 32));
		if (!channel_occupied) {
			mpc55xx_edma_channel_occupation_high |= 0x1 << (channel - 32);
		}
	}
	if (channel_occupied) {
		sc = rtems_semaphore_release( mpc55xx_edma_channel_occupation_mutex);
		SYSLOG_WARNING_SC( sc, "Release occupation mutex");
		return RTEMS_RESOURCE_IN_USE;
	} else {
		sc = rtems_semaphore_release( mpc55xx_edma_channel_occupation_mutex);
		CHECK_SC( sc, "Release channel occupation mutex");
	}

	/* Channel data */
	mpc55xx_edma_channel_table [channel].transfer_update = transfer_update;
	mpc55xx_edma_channel_table [channel].error_status = error_status;

	/* Interrupt handler */
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_GET_REQUEST( channel),
		MPC55XX_EDMA_IRQ_PRIORITY,
		"eDMA Channel",
		RTEMS_INTERRUPT_SHARED,
		mpc55xx_edma_irq_handler,
		&mpc55xx_edma_channel_table [channel]
	);
	CHECK_SC( sc, "Install channel interrupt handler");

	/* Enable error interrupts */
	sc = mpc55xx_edma_enable_error_interrupts( channel, true);
	CHECK_SC( sc, "Enable error interrupts");

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_release_channel( int channel)
{
	// TODO
	return RTEMS_NOT_IMPLEMENTED;
}
