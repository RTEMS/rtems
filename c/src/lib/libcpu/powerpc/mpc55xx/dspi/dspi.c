/**
 * @file
 *
 * @ingroup mpc55xx_dspi
 *
 * @brief Source file for the LibI2C bus driver for the Deserial Serial Peripheral Interface (DSPI).
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
#include <mpc55xx/dspi.h>
#include <mpc55xx/mpc55xx.h>

#include <bsp/irq.h>

#include <libcpu/powerpc-utility.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define MPC55XX_DSPI_FIFO_SIZE 4

#define MPC55XX_DSPI_CTAR_NUMBER 8

#define MPC55XX_DSPI_CTAR_DEFAULT 0

#define MPC55XX_DSPI_EDMA_MAGIC_SIZE 128

#define MPC55XX_DSPI_BAUDRATE_SCALER_TABLE_SIZE 63

typedef struct {
	uint32_t scaler : 26;
	uint32_t pbr : 2;
	uint32_t br : 4;
} mpc55xx_dspi_baudrate_scaler_entry;

static const mpc55xx_dspi_baudrate_scaler_entry mpc55xx_dspi_baudrate_scaler_table [MPC55XX_DSPI_BAUDRATE_SCALER_TABLE_SIZE] = {
	{ 4, 0, 0 },
	{ 6, 1, 0 },
	{ 8, 0, 1 },
	{ 10, 2, 0 },
	{ 12, 1, 1 },
	{ 14, 3, 0 },
	{ 16, 0, 3 },
	{ 18, 1, 2 },
	{ 20, 2, 1 },
	{ 24, 1, 3 },
	{ 28, 3, 1 },
	{ 30, 2, 2 },
	{ 32, 0, 4 },
	{ 40, 2, 3 },
	{ 42, 3, 2 },
	{ 48, 1, 4 },
	{ 56, 3, 3 },
	{ 64, 0, 5 },
	{ 80, 2, 4 },
	{ 96, 1, 5 },
	{ 112, 3, 4 },
	{ 128, 0, 6 },
	{ 160, 2, 5 },
	{ 192, 1, 6 },
	{ 224, 3, 5 },
	{ 256, 0, 7 },
	{ 320, 2, 6 },
	{ 384, 1, 7 },
	{ 448, 3, 6 },
	{ 512, 0, 8 },
	{ 640, 2, 7 },
	{ 768, 1, 8 },
	{ 896, 3, 7 },
	{ 1024, 0, 9 },
	{ 1280, 2, 8 },
	{ 1536, 1, 9 },
	{ 1792, 3, 8 },
	{ 2048, 0, 10 },
	{ 2560, 2, 9 },
	{ 3072, 1, 10 },
	{ 3584, 3, 9 },
	{ 4096, 0, 11 },
	{ 5120, 2, 10 },
	{ 6144, 1, 11 },
	{ 7168, 3, 10 },
	{ 8192, 0, 12 },
	{ 10240, 2, 11 },
	{ 12288, 1, 12 },
	{ 14336, 3, 11 },
	{ 16384, 0, 13 },
	{ 20480, 2, 12 },
	{ 24576, 1, 13 },
	{ 28672, 3, 12 },
	{ 32768, 0, 14 },
	{ 40960, 2, 13 },
	{ 49152, 1, 14 },
	{ 57344, 3, 13 },
	{ 65536, 0, 15 },
	{ 81920, 2, 14 },
	{ 98304, 1, 15 },
	{ 114688, 3, 14 },
	{ 163840, 2, 15 },
	{ 229376, 3, 15 },
};

static void mpc55xx_dspi_edma_done( mpc55xx_edma_channel_entry *e, uint32_t error_status)
{
	rtems_semaphore_release( e->id);

	if (error_status != 0) {
		RTEMS_SYSLOG_ERROR( "eDMA error: 0x%08x\n", error_status);
	}
}

static mpc55xx_dspi_baudrate_scaler_entry mpc55xx_dspi_search_baudrate_scaler( uint32_t scaler, int min, int mid, int max)
{
	if (scaler <= mpc55xx_dspi_baudrate_scaler_table [mid].scaler) {
		max = mid;
	} else {
		min = mid;
	}
	mid = (min + max) / 2;
	if (mid == min) {
		return mpc55xx_dspi_baudrate_scaler_table [max];
	} else {
		return mpc55xx_dspi_search_baudrate_scaler( scaler, min, mid, max);
	}
}

static uint32_t mpc55xx_dspi_push_data [8 * MPC55XX_DSPI_NUMBER] __attribute__ ((aligned (32)));

static inline void mpc55xx_dspi_store_push_data( mpc55xx_dspi_bus_entry *e)
{
	mpc55xx_dspi_push_data [e->table_index * 8] = e->push_data.R;
	rtems_cache_flush_multiple_data_lines( &mpc55xx_dspi_push_data [e->table_index * 8], 4);
}

static inline uint32_t mpc55xx_dspi_push_data_address( mpc55xx_dspi_bus_entry *e)
{
	return (uint32_t) &mpc55xx_dspi_push_data [e->table_index * 8];
}

static inline uint32_t mpc55xx_dspi_nirvana_address( mpc55xx_dspi_bus_entry *e)
{
	return (uint32_t) &mpc55xx_dspi_push_data [e->table_index * 8 + 1];
}

static rtems_status_code mpc55xx_dspi_init( rtems_libi2c_bus_t *bus)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	mpc55xx_dspi_bus_entry *e = (mpc55xx_dspi_bus_entry *) bus;
	union DSPI_MCR_tag mcr = MPC55XX_ZERO_FLAGS;
	union DSPI_CTAR_tag ctar = MPC55XX_ZERO_FLAGS;
	union DSPI_RSER_tag rser = MPC55XX_ZERO_FLAGS;
	struct tcd_t tcd_push = EDMA_TCD_DEFAULT;
	int i = 0;

	/* eDMA receive */
	sc = rtems_semaphore_create (
		rtems_build_name ( 'S', 'P', 'I', 'R'),
		0,
		RTEMS_SIMPLE_BINARY_SEMAPHORE,
		0,
		&e->edma_receive.id
	);
	RTEMS_CHECK_SC( sc, "create receive update semaphore");

	sc = mpc55xx_edma_obtain_channel( &e->edma_receive, MPC55XX_INTC_DEFAULT_PRIORITY);
	RTEMS_CHECK_SC( sc, "obtain receive eDMA channel");

	/* eDMA transmit */
	sc = rtems_semaphore_create (
		rtems_build_name ( 'S', 'P', 'I', 'T'),
		0,
		RTEMS_SIMPLE_BINARY_SEMAPHORE,
		0,
		&e->edma_transmit.id
	);
	RTEMS_CHECK_SC( sc, "create transmit update semaphore");

	sc = mpc55xx_edma_obtain_channel( &e->edma_transmit, MPC55XX_INTC_DEFAULT_PRIORITY);
	RTEMS_CHECK_SC( sc, "obtain transmit eDMA channel");

	sc = mpc55xx_edma_obtain_channel( &e->edma_push, MPC55XX_INTC_DEFAULT_PRIORITY);
	RTEMS_CHECK_SC( sc, "obtain push eDMA channel");

	tcd_push.SADDR = mpc55xx_dspi_push_data_address( e);
	tcd_push.SDF.B.SSIZE = 2;
	tcd_push.SDF.B.SOFF = 0;
	tcd_push.DADDR = (uint32_t) &e->regs->PUSHR.R;
	tcd_push.SDF.B.DSIZE = 2;
	tcd_push.CDF.B.DOFF = 0;
	tcd_push.NBYTES = 4;
	tcd_push.CDF.B.CITER = 1;
	tcd_push.BMF.B.BITER = 1;

	EDMA.TCD [e->edma_push.channel] = tcd_push;

	/* Module Control Register */
	mcr.B.MSTR = e->master ? 1 : 0;
	mcr.B.CONT_SCKE = 0;
	mcr.B.DCONF = 0;
	mcr.B.FRZ = 0;
	mcr.B.MTFE = 0;
	mcr.B.PCSSE = 0;
	mcr.B.ROOE = 0;
	mcr.B.PCSIS0 = 1;
	mcr.B.PCSIS1 = 1;
	mcr.B.PCSIS2 = 1;
	mcr.B.PCSIS3 = 1;
	mcr.B.PCSIS5 = 1;
	mcr.B.MDIS = 0;
	mcr.B.DIS_TXF = 0;
	mcr.B.DIS_RXF = 0;
	mcr.B.CLR_TXF = 0;
	mcr.B.CLR_RXF = 0;
	mcr.B.SMPL_PT = 0;
	mcr.B.HALT = 0;

	e->regs->MCR.R = mcr.R;

	/* Clock and Transfer Attributes Register */
	ctar.B.DBR = 0;
	ctar.B.FMSZ = 0x7;
	ctar.B.CPOL = 0;
	ctar.B.CPHA = 0;
	ctar.B.LSBFE = 0;
	ctar.B.PCSSCK = 0;
	ctar.B.PASC = 0;
	ctar.B.PDT = 0;
	ctar.B.PBR = 0;
	ctar.B.CSSCK = 0;
	ctar.B.ASC = 0;
	ctar.B.DT = 0;
	ctar.B.BR = 0;

	for (i = 0; i < MPC55XX_DSPI_CTAR_NUMBER; ++i) {
		e->regs->CTAR [i].R = ctar.R;
	}

        /* DMA/Interrupt Request Select and Enable Register */
	rser.B.TFFFRE = 1;
	rser.B.TFFFDIRS = 1;
	rser.B.RFDFRE = 1;
	rser.B.RFDFDIRS = 1;

	e->regs->RSER.R = rser.R;

	return RTEMS_SUCCESSFUL;
}

static rtems_status_code mpc55xx_dspi_send_start( rtems_libi2c_bus_t *bus)
{
	mpc55xx_dspi_bus_entry *e = (mpc55xx_dspi_bus_entry *) bus;

	/* Reset chip selects */
	e->push_data.B.PCS0 = 0;
	e->push_data.B.PCS1 = 0;
	e->push_data.B.PCS2 = 0;
	e->push_data.B.PCS3 = 0;
	e->push_data.B.PCS4 = 0;
	e->push_data.B.PCS5 = 0;
	mpc55xx_dspi_store_push_data( e);

	return RTEMS_SUCCESSFUL;
}

static rtems_status_code mpc55xx_dspi_send_stop( rtems_libi2c_bus_t *bus)
{
	return RTEMS_SUCCESSFUL;
}

static rtems_status_code mpc55xx_dspi_send_addr( rtems_libi2c_bus_t *bus, uint32_t addr, int rw)
{
	mpc55xx_dspi_bus_entry *e = (mpc55xx_dspi_bus_entry *) bus;
	union DSPI_SR_tag sr = MPC55XX_ZERO_FLAGS;

	/* Flush transmit and receive FIFO */
	e->regs->MCR.B.CLR_TXF = 1;
	e->regs->MCR.B.CLR_RXF = 1;

	/* Clear status flags */
	sr.B.EOQF = 1;
	sr.B.TFFF = 1;
	sr.B.RFDF = 1;
	e->regs->SR.R = sr.R;

	/* Frame command */
	e->push_data.R = 0;
	e->push_data.B.CONT = 0;
	e->push_data.B.CTAS = MPC55XX_DSPI_CTAR_DEFAULT;
	e->push_data.B.EOQ = 0;
	e->push_data.B.CTCNT = 0;
	switch (addr) {
		case 0:
			e->push_data.B.PCS0 = 1;
			break;
		case 1:
			e->push_data.B.PCS1 = 1;
			break;
		case 2:
			e->push_data.B.PCS2 = 1;
			break;
		case 3:
			e->push_data.B.PCS3 = 1;
			break;
		case 4:
			e->push_data.B.PCS4 = 1;
			break;
		case 5:
			e->push_data.B.PCS5 = 1;
			break;
		default:
			return -RTEMS_INVALID_ADDRESS;
	}
	mpc55xx_dspi_store_push_data( e);

	return RTEMS_SUCCESSFUL;
}

/* FIXME, TODO */
extern uint32_t bsp_clock_speed;

static int mpc55xx_dspi_set_transfer_mode( rtems_libi2c_bus_t *bus, const rtems_libi2c_tfr_mode_t *mode)
{
	mpc55xx_dspi_bus_entry *e = (mpc55xx_dspi_bus_entry *) bus;
	union DSPI_CTAR_tag ctar = MPC55XX_ZERO_FLAGS;

	if (mode->bits_per_char != 8) {
		return -RTEMS_INVALID_NUMBER;
	}

	e->idle_char = mode->idle_char;

	ctar.R = e->regs->CTAR [MPC55XX_DSPI_CTAR_DEFAULT].R;

	ctar.B.PCSSCK = 0;
	ctar.B.CSSCK = 0;
	ctar.B.PASC = 0;
	ctar.B.ASC = 0;

	ctar.B.LSBFE = mode->lsb_first ? 1 : 0;
	ctar.B.CPOL = mode->clock_inv ? 1 : 0;
	ctar.B.CPHA = mode->clock_phs ? 1 : 0;

	if (mode->baudrate != e->baud) {
		uint32_t scaler = bsp_clock_speed / mode->baudrate;
		mpc55xx_dspi_baudrate_scaler_entry bse = mpc55xx_dspi_search_baudrate_scaler( scaler, 0, MPC55XX_DSPI_BAUDRATE_SCALER_TABLE_SIZE / 2, MPC55XX_DSPI_BAUDRATE_SCALER_TABLE_SIZE);

		ctar.B.PBR = bse.pbr;
		ctar.B.BR = bse.br;

		e->baud = mode->baudrate;
	}

	e->regs->CTAR [MPC55XX_DSPI_CTAR_DEFAULT].R = ctar.R;

	return 0;
}

/**
 * @brief Writes @a n characters from @a out to bus @a bus and synchronously stores the received data in @a in.
 *
 * eDMA channel usage for transmission:
 * @dot
 * digraph push {
 *  push [label="Push Register"];
 *  push_data [label="Push Data"];
 *  idle_push_data [label="Idle Push Data"];
 *  out [shape=box,label="Output Buffer"];
 *  edge [color=red,fontcolor=red];
 *  push -> idle_push_data [label="Transmit Request",URL="\ref mpc55xx_dspi_bus_entry::edma_transmit"];
 *  push -> out [label="Transmit Request",URL="\ref mpc55xx_dspi_bus_entry::edma_transmit"];
 *  out -> push_data [label="Channel Link",URL="\ref mpc55xx_dspi_bus_entry::edma_push"];
 *  edge [color=blue,fontcolor=blue];
 *  out -> push_data [label="Data"];
 *  push_data -> push [label="Data"];
 *  idle_push_data -> push [label="Data"];
 * }
 * @enddot
 *
 * eDMA channel usage for receiving:
 * @dot
 * digraph pop {
 *  pop [label="Pop Register"];
 *  nirvana [label="Nirvana"];
 *  in [shape=box,label="Input Buffer"];
 *  edge [color=red,fontcolor=red];
 *  pop -> nirvana [label="Receive Request",URL="\ref mpc55xx_dspi_bus_entry::edma_receive"];
 *  pop -> in [label="Receive Request",URL="\ref mpc55xx_dspi_bus_entry::edma_receive"];
 *  edge [color=blue,fontcolor=blue];
 *  pop -> nirvana [label="Data"];
 *  pop -> in [label="Data"];
 * }
 * @enddot
 */
static int mpc55xx_dspi_read_write( rtems_libi2c_bus_t *bus, unsigned char *in, const unsigned char *out, int n)
{
	mpc55xx_dspi_bus_entry *e = (mpc55xx_dspi_bus_entry *) bus;

	/* Non cache aligned characters */
	int n_nc = n;

	/* Cache aligned characters */
	int n_c = 0;

	/* Register addresses */
	volatile void *push = &e->regs->PUSHR.R;
	volatile void *pop = &e->regs->POPR.R;
	volatile union DSPI_SR_tag *status = &e->regs->SR;

	/* Push and pop data */
	union DSPI_PUSHR_tag push_data = e->push_data;
	union DSPI_POPR_tag pop_data;

	/* Status register */
	union DSPI_SR_tag sr;

	/* Read and write indices */
	int r = 0;
	int w = 0;

	if (n == 0) {
		return 0;
	} else if (in == NULL && out == NULL) {
		return -RTEMS_INVALID_ADDRESS;
	}

	if (n > MPC55XX_DSPI_EDMA_MAGIC_SIZE) {
		n_nc = (int) mpc55xx_non_cache_aligned_size( in);
		n_c = (int) mpc55xx_cache_aligned_size( in, (size_t) n);
		if (n_c > EDMA_TCD_BITER_LINKED_SIZE) {
			RTEMS_SYSLOG_WARNING( "buffer size out of range, cannot use eDMA\n");
			n_nc = n;
			n_c = 0;
		} else if (n_nc + n_c != n) {
			RTEMS_SYSLOG_WARNING( "input buffer not proper cache aligned, cannot use eDMA\n");
			n_nc = n;
			n_c = 0;
		}
	}

#ifdef DEBUG
	if (e->regs->SR.B.TXCTR != e->regs->SR.B.RXCTR) {
		RTEMS_SYSLOG_WARNING( "FIFO counter not equal\n");
	}
#endif /* DEBUG */

	/* Direct IO */
	if (out == NULL) {
		push_data.B.TXDATA = e->idle_char;
		while (r < n_nc || w < n_nc) {
			/* Wait for available FIFO */
			do {
				sr.R = status->R;
			} while (sr.B.TXCTR == MPC55XX_DSPI_FIFO_SIZE && sr.B.RXCTR == 0);

			/* Write */
			if (w < n_nc && (w - r) < MPC55XX_DSPI_FIFO_SIZE && sr.B.TXCTR != MPC55XX_DSPI_FIFO_SIZE) {
				++w;
				ppc_write_word( push_data.R, push);
			}

			/* Read */
			if (r < n_nc && sr.B.RXCTR != 0) {
				pop_data.R = ppc_read_word( pop);
				in [r] = (unsigned char) pop_data.B.RXDATA;
				++r;
			}
		}
	} else if (in == NULL) {
		while (r < n_nc || w < n_nc) {
			/* Wait for available FIFO */
			do {
				sr.R = status->R;
			} while (sr.B.TXCTR == MPC55XX_DSPI_FIFO_SIZE && sr.B.RXCTR == 0);

			/* Write */
			if (w < n_nc && (w - r) < MPC55XX_DSPI_FIFO_SIZE && sr.B.TXCTR != MPC55XX_DSPI_FIFO_SIZE) {
				push_data.B.TXDATA = out [w];
				++w;
				ppc_write_word( push_data.R, push);
			}

			/* Read */
			if (r < n_nc && sr.B.RXCTR != 0) {
				pop_data.R = ppc_read_word( pop);
				++r;
			}
		}
	} else {
		while (r < n_nc || w < n_nc) {
			/* Wait for available FIFO */
			do {
				sr.R = status->R;
			} while (sr.B.TXCTR == MPC55XX_DSPI_FIFO_SIZE && sr.B.RXCTR == 0);

			/* Write */
			if (w < n_nc && (w - r) < MPC55XX_DSPI_FIFO_SIZE && sr.B.TXCTR != MPC55XX_DSPI_FIFO_SIZE) {
				push_data.B.TXDATA = out [w];
				++w;
				ppc_write_word( push_data.R, push);
			}

			/* Read */
			if (r < n_nc && sr.B.RXCTR != 0) {
				pop_data.R = ppc_read_word( pop);
				in [r] = (unsigned char) pop_data.B.RXDATA;
				++r;
			}
		}
	}

	/* eDMA transfers */
	if (n_c > 0) {
		rtems_status_code sc = RTEMS_SUCCESSFUL;
		unsigned char *in_c = in + n_nc;
		const unsigned char *out_c = out + n_nc;
		struct tcd_t tcd_transmit = EDMA_TCD_DEFAULT;
		struct tcd_t tcd_receive = EDMA_TCD_DEFAULT;

		/* Cache operations */
		rtems_cache_flush_multiple_data_lines( out_c, (size_t) n_c);
		rtems_cache_invalidate_multiple_data_lines( in_c, (size_t) n_c);

		/* Set transmit TCD */
		if (out == NULL) {
			e->push_data.B.TXDATA = e->idle_char;
			mpc55xx_dspi_store_push_data( e);
			tcd_transmit.SADDR = mpc55xx_dspi_push_data_address( e);
			tcd_transmit.SDF.B.SSIZE = 2;
			tcd_transmit.SDF.B.SOFF = 0;
			tcd_transmit.DADDR = (uint32_t) push;
			tcd_transmit.SDF.B.DSIZE = 2;
			tcd_transmit.CDF.B.DOFF = 0;
			tcd_transmit.NBYTES = 4;
			tcd_transmit.CDF.B.CITER = n_c;
			tcd_transmit.BMF.B.BITER = n_c;
		} else {
			EDMA.CDSBR.R = e->edma_transmit.channel;
			tcd_transmit.SADDR = (uint32_t) out_c;
			tcd_transmit.SDF.B.SSIZE = 0;
			tcd_transmit.SDF.B.SOFF = 1;
			tcd_transmit.DADDR = mpc55xx_dspi_push_data_address( e) + 3;
			tcd_transmit.SDF.B.DSIZE = 0;
			tcd_transmit.CDF.B.DOFF = 0;
			tcd_transmit.NBYTES = 1;
			tcd_transmit.CDF.B.CITERE_LINK = 1;
			tcd_transmit.BMF.B.BITERE_LINK = 1;
			tcd_transmit.BMF.B.MAJORLINKCH = e->edma_push.channel;
			tcd_transmit.CDF.B.CITER = EDMA_TCD_LINK_AND_BITER( e->edma_push.channel, n_c);
			tcd_transmit.BMF.B.BITER = EDMA_TCD_LINK_AND_BITER( e->edma_push.channel, n_c);
			tcd_transmit.BMF.B.MAJORE_LINK = 1;
		}
		tcd_transmit.BMF.B.D_REQ = 1;
		tcd_transmit.BMF.B.INT_MAJ = 1;
		EDMA.TCD [e->edma_transmit.channel] = tcd_transmit;

		/* Set receive TCD */
		if (in == NULL) {
			tcd_receive.CDF.B.DOFF = 0;
			tcd_receive.DADDR = mpc55xx_dspi_nirvana_address( e);
		} else {
			tcd_receive.CDF.B.DOFF = 1;
			tcd_receive.DADDR = (uint32_t) in_c;
		}
		tcd_receive.SADDR = (uint32_t) pop + 3;
		tcd_receive.SDF.B.SSIZE = 0;
		tcd_receive.SDF.B.SOFF = 0;
		tcd_receive.SDF.B.DSIZE = 0;
		tcd_receive.NBYTES = 1;
		tcd_receive.BMF.B.D_REQ = 1;
		tcd_receive.BMF.B.INT_MAJ = 1;
		tcd_receive.CDF.B.CITER = n_c;
		tcd_receive.BMF.B.BITER = n_c;
		EDMA.TCD [e->edma_receive.channel] = tcd_receive;

		/* Clear request flags */
		sr.R = 0;
		sr.B.TFFF = 1;
		sr.B.RFDF = 1;
		status->R = sr.R;

		/* Enable hardware requests */
		mpc55xx_edma_enable_hardware_requests( e->edma_receive.channel, true);
		mpc55xx_edma_enable_hardware_requests( e->edma_transmit.channel, true);

		/* Wait for transmit update */
		sc = rtems_semaphore_obtain( e->edma_transmit.id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		RTEMS_CHECK_SC_RV( sc, "transmit update");

		/* Wait for receive update */
		sc = rtems_semaphore_obtain( e->edma_receive.id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		RTEMS_CHECK_SC_RV( sc, "receive update");
	}

	return n;
}

/**
 * @brief Reads @a n characters from bus @a bus and stores it in @a in.
 *
 * Writes idle characters to receive data.
 *
 * @see mpc55xx_dspi_read_write().
 */
static int mpc55xx_dspi_read( rtems_libi2c_bus_t *bus, unsigned char *in, int n)
{
	return mpc55xx_dspi_read_write( bus, in, NULL, n);
}

/**
 * @brief Writes @a n characters from @a out to bus @a bus.
 *
 * Discards the synchronously received data.
 *
 * @see mpc55xx_dspi_read_write().
 */
static int mpc55xx_dspi_write( rtems_libi2c_bus_t *bus, unsigned char *out, int n)
{
	return mpc55xx_dspi_read_write( bus, NULL, out, n);
}

static int mpc55xx_dspi_ioctl( rtems_libi2c_bus_t *bus, int cmd, void *arg)
{
	int rv = -1;
	switch (cmd) {
		case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
			rv = mpc55xx_dspi_set_transfer_mode( bus, (const rtems_libi2c_tfr_mode_t *) arg);
			break;
		case RTEMS_LIBI2C_IOCTL_READ_WRITE:
			rv = mpc55xx_dspi_read_write(
				bus,
				((rtems_libi2c_read_write_t *) arg)->rd_buf,
				((rtems_libi2c_read_write_t *) arg)->wr_buf,
				((rtems_libi2c_read_write_t *) arg)->byte_cnt
			);
			break;
		default:
			rv = -RTEMS_NOT_DEFINED;
			break;
	}
	return rv;
}

static const rtems_libi2c_bus_ops_t mpc55xx_dspi_ops = {
	.init        = mpc55xx_dspi_init,
	.send_start  = mpc55xx_dspi_send_start,
	.send_stop   = mpc55xx_dspi_send_stop,
	.send_addr   = mpc55xx_dspi_send_addr,
	.read_bytes  = mpc55xx_dspi_read,
	.write_bytes = mpc55xx_dspi_write,
	.ioctl       = mpc55xx_dspi_ioctl
};

mpc55xx_dspi_bus_entry mpc55xx_dspi_bus_table [MPC55XX_DSPI_NUMBER] = {
	{
		/* DSPI A */
		.bus = {
			.ops = &mpc55xx_dspi_ops,
			.size = sizeof( mpc55xx_dspi_bus_entry)
		},
		.table_index = 0,
		.bus_number = 0,
		.regs = &DSPI_A,
		.master = true,
		.push_data = MPC55XX_ZERO_FLAGS,
		.edma_transmit = {
			.channel = 32,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_push = {
			.channel = 43,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_receive = {
			.channel = 33,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.idle_char = 0xffffffff,
		.baud = 0
	}, {
		/* DSPI B */
		.bus = {
			.ops = &mpc55xx_dspi_ops,
			.size = sizeof( mpc55xx_dspi_bus_entry)
		},
		.table_index = 1,
		.bus_number = 0,
		.regs = &DSPI_B,
		.master = true,
		.push_data = MPC55XX_ZERO_FLAGS,
		.edma_transmit = {
			.channel = 12,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_push = {
			.channel = 10,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_receive = {
			.channel = 13,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.idle_char = 0xffffffff,
		.baud = 0
	}, {
		/* DSPI C */
		.bus = {
			.ops = &mpc55xx_dspi_ops,
			.size = sizeof( mpc55xx_dspi_bus_entry)
		},
		.table_index = 2,
		.bus_number = 0,
		.regs = &DSPI_C,
		.master = true,
		.push_data = MPC55XX_ZERO_FLAGS,
		.edma_transmit = {
			.channel = 14,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_push = {
			.channel = 11,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_receive = {
			.channel = 15,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.idle_char = 0xffffffff,
		.baud = 0
#ifdef DSPI_D
	}, {
		/* DSPI D */
		.bus = {
			.ops = &mpc55xx_dspi_ops,
			.size = sizeof( mpc55xx_dspi_bus_entry)
		},
		.table_index = 3,
		.bus_number = 0,
		.regs = &DSPI_D,
		.master = true,
		.push_data = MPC55XX_ZERO_FLAGS,
		.edma_transmit = {
			.channel = 16,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_push = {
			.channel = 18,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.edma_receive = {
			.channel = 17,
			.done = mpc55xx_dspi_edma_done,
			.id = RTEMS_ID_NONE
		},
		.idle_char = 0xffffffff,
		.baud = 0
#endif
	}
};
