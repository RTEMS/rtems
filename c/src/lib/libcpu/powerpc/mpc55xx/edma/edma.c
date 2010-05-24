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

#include <string.h>

#include <bsp/irq.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#if   ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))
#define MPC55XX_EDMA_CHANNEL_COUNT 16U
#else /* ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517)) */
#define MPC55XX_EDMA_CHANNEL_COUNT 64U
#endif /* ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517)) */

#define MPC55XX_EDMA_INVALID_CHANNEL MPC55XX_EDMA_CHANNEL_COUNT

#define MPC55XX_EDMA_IS_CHANNEL_INVALID( i) ((unsigned) (i) >= MPC55XX_EDMA_CHANNEL_COUNT)

#define MPC55XX_EDMA_IS_CHANNEL_VALID( i) ((unsigned) (i) < MPC55XX_EDMA_CHANNEL_COUNT)

#define MPC55XX_EDMA_IRQ_PRIORITY MPC55XX_INTC_DEFAULT_PRIORITY

#define MPC55XX_EDMA_CHANNEL_FLAG( channel) ((uint64_t) 1 << (channel))

static uint64_t mpc55xx_edma_channel_occupation = 0;

static rtems_chain_control mpc55xx_edma_channel_chain;

static void mpc55xx_edma_interrupt_handler( void *arg)
{
	mpc55xx_edma_channel_entry *e = (mpc55xx_edma_channel_entry *) arg;

#ifdef DEBUG
	uint32_t citer = EDMA.TCD [e->channel].CITERE_LINK ? EDMA.TCD [e->channel].CITER & EDMA_TCD_BITER_LINKED_MASK : EDMA.TCD [e->channel].CITER;
	RTEMS_DEBUG_PRINT( "channel %i (CITER = %i)\n", e->channel, citer);
#endif /* DEBUG */

	/* Clear interrupt */
	EDMA.CIRQR.R = (uint8_t) e->channel;

	/* Notify user */
	e->done( e, 0);
}

static void mpc55xx_edma_interrupt_error_handler( void *arg)
{
	rtems_chain_control *chain = &mpc55xx_edma_channel_chain;
	rtems_chain_node *node = chain->first;
	unsigned i = 0;
	uint64_t error_status = EDMA.ESR.R;
	uint64_t error_channels = ((uint64_t) EDMA.ERH.R << 32) | EDMA.ERL.R;
	uint64_t error_channels_update = 0;

	RTEMS_DEBUG_PRINT( "error channels: %08x %08x\n", (unsigned) (error_channels >> 32), (unsigned) error_channels);

	/* Mark all channels that are linked to a channel with errors */
	do {
		error_channels_update = 0;

		for (i = 0; i < MPC55XX_EDMA_CHANNEL_COUNT; ++i) {
			uint64_t channel_flags = 0;
			unsigned minor_link = i;
			unsigned major_link = i;

			/* Check if we have linked channels */
			if (EDMA.TCD [i].BMF.B.BITERE_LINK) {
				minor_link = EDMA_TCD_BITER_LINK( i);
			}
			if (EDMA.TCD [i].BMF.B.MAJORE_LINK) {
				major_link = EDMA.TCD [i].BMF.B.MAJORLINKCH;
			}

			/* Set flags related to this channel */
			channel_flags = MPC55XX_EDMA_CHANNEL_FLAG( i) | MPC55XX_EDMA_CHANNEL_FLAG( minor_link) | MPC55XX_EDMA_CHANNEL_FLAG( major_link);

			/* Any errors in these channels? */
			if ( error_channels & channel_flags ) {
				/* Get new error channels */
				uint64_t update = (error_channels & channel_flags) ^ channel_flags;

				/* Update error channels */
				error_channels |= channel_flags;

				/* Contribute to the update of this round */
				error_channels_update |=  update;
			}
		}
	} while (error_channels_update != 0);

	RTEMS_DEBUG_PRINT( "error channels (all): %08x %08x\n", (unsigned) (error_channels >> 32), (unsigned) error_channels);

	/* Process the channels related to errors */
	while (!rtems_chain_is_tail( chain, node)) {
		mpc55xx_edma_channel_entry *e = (mpc55xx_edma_channel_entry *) node;

		if ( error_channels & MPC55XX_EDMA_CHANNEL_FLAG( e->channel)) {
			mpc55xx_edma_enable_hardware_requests( e->channel, false);

			/* Notify user */
			e->done( e, error_status);
		}

		node = node->next;
	}

	/* Clear the error interrupt requests */
	for (i = 0; i < MPC55XX_EDMA_CHANNEL_COUNT; ++i) {
		if ( error_channels & MPC55XX_EDMA_CHANNEL_FLAG( i)) {
			EDMA.CER.R = (uint8_t) i;
		}
	}
}

void mpc55xx_edma_enable_hardware_requests( unsigned channel, bool enable)
{
	if (MPC55XX_EDMA_IS_CHANNEL_VALID( channel)) {
		if (enable) {
			EDMA.SERQR.R = (uint8_t) channel;
		} else {
			EDMA.CERQR.R = (uint8_t) channel;
		}
	} else {
		RTEMS_SYSLOG_ERROR( "invalid channel number\n");
	}
}

void mpc55xx_edma_enable_error_interrupts( unsigned channel, bool enable)
{
	if (MPC55XX_EDMA_IS_CHANNEL_VALID( channel)) {
		if (enable) {
			EDMA.SEEIR.R = (uint8_t) channel;
		} else {
			EDMA.CEEIR.R = (uint8_t) channel;
		}
	} else {
		RTEMS_SYSLOG_ERROR( "invalid channel number\n");
	}
}

rtems_status_code mpc55xx_edma_init(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	/* Initialize channel chain */
	rtems_chain_initialize_empty( &mpc55xx_edma_channel_chain);

	/* Arbitration mode: round robin */
	EDMA.CR.B.ERCA = 1;
	EDMA.CR.B.ERGA = 1;

	/* Clear TCDs */
	memset( (void *)&EDMA.TCD [0], 0, 
		MPC55XX_EDMA_CHANNEL_COUNT * sizeof( EDMA.TCD[0]));

	/* Error interrupt handlers */
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_ERROR_LOW,
		"eDMA Error (Low)",
		RTEMS_INTERRUPT_UNIQUE,
		MPC55XX_EDMA_IRQ_PRIORITY,
		mpc55xx_edma_interrupt_error_handler,
		NULL
	);
	RTEMS_CHECK_SC( sc, "install low error interrupt handler");

#if defined(MPC55XX_IRQ_EDMA_ERROR_HIGH)
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_ERROR_HIGH,
		"eDMA Error (High)",
		RTEMS_INTERRUPT_UNIQUE,
		MPC55XX_EDMA_IRQ_PRIORITY,
		mpc55xx_edma_interrupt_error_handler,
		NULL
	);
	RTEMS_CHECK_SC( sc, "install high error interrupt handler");
#endif /* defined(MPC55XX_IRQ_EDMA_ERROR_HIGH) */

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_obtain_channel( mpc55xx_edma_channel_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_interrupt_level level;
	uint64_t channel_occupation = 0;

	if (MPC55XX_EDMA_IS_CHANNEL_INVALID( e->channel)) {
		return RTEMS_INVALID_NUMBER;
	}

	/* Test and set channel occupation flag */
	rtems_interrupt_disable( level);
	channel_occupation = mpc55xx_edma_channel_occupation;
	if ( (channel_occupation & MPC55XX_EDMA_CHANNEL_FLAG( e->channel)) == 0 ) {
		mpc55xx_edma_channel_occupation = channel_occupation | MPC55XX_EDMA_CHANNEL_FLAG( e->channel);
	}
	rtems_interrupt_enable( level);

	/* Check channel occupation flag */
	if ( channel_occupation & MPC55XX_EDMA_CHANNEL_FLAG( e->channel)) {
		return RTEMS_RESOURCE_IN_USE;
	}

	/* Interrupt handler */
	sc = mpc55xx_interrupt_handler_install(
		MPC55XX_IRQ_EDMA_GET_REQUEST( e->channel),
		"eDMA Channel",
		RTEMS_INTERRUPT_SHARED,
		MPC55XX_EDMA_IRQ_PRIORITY,
		mpc55xx_edma_interrupt_handler,
		e
	);
	RTEMS_CHECK_SC( sc, "install channel interrupt handler");

	/* Enable error interrupts */
	mpc55xx_edma_enable_error_interrupts( e->channel, true);

	/* Prepend channel entry to channel list */
	rtems_chain_prepend( &mpc55xx_edma_channel_chain, &e->node);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code mpc55xx_edma_release_channel( mpc55xx_edma_channel_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_interrupt_level level;

	/* Clear channel occupation flag */
	rtems_interrupt_disable( level);
	mpc55xx_edma_channel_occupation &= ~MPC55XX_EDMA_CHANNEL_FLAG( e->channel);
	rtems_interrupt_enable( level);

	/* Disable hardware requests */
	mpc55xx_edma_enable_hardware_requests( e->channel, false);

	/* Disable error interrupts */
	mpc55xx_edma_enable_error_interrupts( e->channel, false);

	/* Extract channel entry from channel chain */
	rtems_chain_extract( &e->node);

	/* Remove interrupt handler */
	sc = rtems_interrupt_handler_remove(
		MPC55XX_IRQ_EDMA_GET_REQUEST( e->channel),
		mpc55xx_edma_interrupt_handler,
		e
	);
	RTEMS_CHECK_SC( sc, "remove channel interrupt handler");

	/* Notify user */
	e->done( e, 0);

	return RTEMS_SUCCESSFUL;
}
