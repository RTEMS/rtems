/*
 *  FD extenstions to the GRCAN driver
 *
 *  COPYRIGHT (c) 2007-2019.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#include <grlib/grcan.h>
#include <grlib/canbtrs.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>

#include <grlib/grlib_impl.h>
#include "grcan_internal.h"

/* Uncomment for debug output */
/****************** DEBUG Definitions ********************/
#define DBG_TX 2
#define DBG_RX 4
#define DBG_STATE 8

#define DEBUG_FLAGS (DBG_STATE | DBG_RX | DBG_TX )
/*
#define DEBUG
#define DEBUGFUNCS
*/
#include <grlib/debug_defs.h>

/*********************************************************/

struct grcanfd_bd0 {
	uint32_t head[2];
	uint64_t data0; /* variable size, from 1 to 8 dwords */
};

struct grcanfd_bd1 {
	unsigned long long data[2];
};

static uint8_t dlc2len[16] = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	8, 12, 16, 20,
	24, 32, 48, 64
};

static uint8_t len2fddlc[14] = {
 /* 12,13 */	0x9,
 /* 16,17 */	0xA,
 /* 20,21 */	0xB,
 /* 24,25 */	0xC,
 /* 28,29 */	-1,
 /* 32,33 */	0xD,
 /* 36,37 */	-1,
 /* 40,41 */	-1,
 /* 44,45 */	-1,
 /* 48,49 */	0xE,
 /* 52,53 */	-1,
 /* 56,57 */	-1,
 /* 60,61 */	-1,
 /* 64,65 */	0xF,
};

/* Convert length in bytes to descriptor length field */
static inline uint8_t grcan_len2dlc(int len)
{
	if (len <= 8)
		return len;
	if (len > 64)
		return -1;
	if (len & 0x3)
		return -1;
	return len2fddlc[(len - 12) >> 2];
}

static inline int grcan_numbds(int len)
{
	return 1 + ((len + 7) >> 4);
}

static int grcan_hw_read_try_fd(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANFDMsg * buffer,
	int max)
{
	int j;
	CANFDMsg *dest;
	struct grcanfd_bd0 *source, tmp, *rxmax;
	unsigned int wp, rp, size, addr;
	int bds_hw_avail, bds_tot, bds, ret, dlc;
	uint64_t *dp;
	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

	wp = READ_REG(&regs->rx0wr);
	rp = READ_REG(&regs->rx0rd);

	/*
	 * Due to hardware wrap around simplification write pointer will
	 * never reach the read pointer, at least a gap of 8 bytes.
	 * The only time they are equal is when the read pointer has
	 * reached the write pointer (empty buffer)
	 *
	 */
	if (wp != rp) {
		/* Not empty, we have received chars...
		 * Read as much as possible from DMA buffer
		 */
		size = READ_REG(&regs->rx0size);

		/* Get number of bytes available in RX buffer */
		bds_hw_avail = grcan_hw_rxavail(rp, wp, size);

		addr = (unsigned int)pDev->rx;
		source = (struct grcanfd_bd0 *)(addr + rp);
		dest = buffer;
		rxmax = (struct grcanfd_bd0 *)(addr + size);
		ret = bds_tot = 0;

		/* Read as many can messages as possible */
		while ((ret < max) && (bds_tot < bds_hw_avail)) {
			/* Read CAN message from DMA buffer */
			*(uint64_t *)&tmp = READ_DMA_DOUBLE(source);
			if (tmp.head[1] & 0x4) {
				DBGC(DBG_RX, "overrun\n");
			}
			if (tmp.head[1] & 0x2) {
				DBGC(DBG_RX, "bus-off mode\n");
			}
			if (tmp.head[1] & 0x1) {
				DBGC(DBG_RX, "error-passive mode\n");
			}
			/* Convert one grcan CAN message to one "software" CAN message */
			dest->extended = tmp.head[0] >> 31;
			dest->rtr = (tmp.head[0] >> 30) & 0x1;
			if (dest->extended) {
				dest->id = tmp.head[0] & 0x3fffffff;
			} else {
				dest->id = (tmp.head[0] >> 18) & 0xfff;
			}
			dest->fdopts = (tmp.head[1] >> 25) & GRCAN_FDMASK;
			dlc = tmp.head[1] >> 28;
			if (dest->fdopts & GRCAN_FDOPT_FDFRM) {
				dest->len = dlc2len[dlc];
			} else {
				dest->len = dlc;
				if (dlc > 8)
					dest->len = 8;
			}

			dp = (uint64_t *)&source->data0;
			for (j = 0; j < ((dest->len + 7) / 8); j++) {
				dest->data.dwords[j] = READ_DMA_DOUBLE(dp);
				if (++dp >= (uint64_t *)rxmax)
					dp = (uint64_t *)addr; /* wrap around */
			}

			/* wrap around if neccessary */
			bds = grcan_numbds(dest->len);
			source += bds;
			if (source >= rxmax) {
				source = (struct grcanfd_bd0 *)
					 ((void *)source - size);
			}
			dest++;	/* straight user buffer */
			ret++;
			bds_tot += bds;
		}

		/* A bus off interrupt may have occured after checking pDev->started */
		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		if (pDev->started == STATE_STARTED) {
			regs->rx0rd = (unsigned int) source - addr;
			regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
		} else {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			ret = state2err[pDev->started];
		}
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

		return ret;
	}
	return 0;
}

int grcanfd_read(void *d, CANFDMsg *msg, size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANFDMsg *dest;
	unsigned int count, left;
	int nread;
	int req_cnt;

	FUNCDBG();

	dest = msg;
	req_cnt = ucount;

	if ( (!dest) || (req_cnt<1) )
		return GRCAN_RET_INVARG;

	if (pDev->started != STATE_STARTED) {
		return GRCAN_RET_NOTSTARTED;
	}

	DBGC(DBG_RX, "grcan_read [%p]: buf: %p len: %u\n", d, msg, (unsigned int) ucount);

	nread = grcan_hw_read_try_fd(pDev,pDev->regs,dest,req_cnt);
	if (nread < 0) {
		return nread;
	}
	count = nread;
	if ( !( pDev->rxblock && pDev->rxcomplete && (count!=req_cnt) ) ){
		if ( count > 0 ) {
			/* Successfully received messages (at least one) */
			return count;
		}

		/* nothing read, shall we block? */
		if ( !pDev->rxblock ) {
			/* non-blocking mode */
			return GRCAN_RET_TIMEOUT;
		}
	}

	while (count == 0 || (pDev->rxcomplete && (count!=req_cnt))) {
		if (!pDev->rxcomplete) {
			left = 1; /* return as soon as there is one message available */
		} else {
			left = req_cnt - count;     /* return as soon as all data are available */

			/* never wait for more than the half the maximum size of the receive buffer 
			 * Why? We need some time to copy buffer before to catch up with hw,
			 * otherwise we would have to copy everything when the data has been
			 * received.
			 */
			if (left > ((pDev->rxbuf_size/GRCAN_MSG_SIZE) / 2)){
				left = (pDev->rxbuf_size/GRCAN_MSG_SIZE) / 2;
			}
		}

		nread = grcan_wait_rxdata(pDev, left);
		if (nread) {
			/* The wait has been aborted, probably due to
			 * the device driver has been closed by another
			 * thread or a bus-off. Return error code.
			 */
			return nread;
		}

		/* Try read bytes from circular buffer */
		nread = grcan_hw_read_try_fd(
				pDev,
				pDev->regs,
				dest+count,
				req_cnt-count);

		if (nread < 0) {
			/* The read was aborted by bus-off. */
			return nread;
		}
		count += nread;
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */
	return count;
}

static int grcan_hw_write_try_fd(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANFDMsg *buffer,
	int count)
{
	unsigned int rp, wp, size, addr;
	int ret;
	struct grcanfd_bd0 *dest, *txmax;
	CANFDMsg *source = (CANFDMsg *) buffer;
	int space_left;
	unsigned int tmp;
	int i, bds;
	uint64_t *dp;
	uint8_t dlc;
	SPIN_IRQFLAGS(oldLevel);

	DBGC(DBG_TX, "\n");

	rp = READ_REG(&regs->tx0rd);
	wp = READ_REG(&regs->tx0wr);
	size = READ_REG(&regs->tx0size);
	space_left = grcan_hw_txspace(rp, wp, size);

	addr = (unsigned int)pDev->tx;
	dest = (struct grcanfd_bd0 *)(addr + wp);
	txmax = (struct grcanfd_bd0 *)(addr + size);
	ret = 0;

	while (source < &buffer[count]) {
		/* Get the number of descriptors to wait for */
		if (source->fdopts & GRCAN_FDOPT_FDFRM)
			bds = grcan_numbds(source->len); /* next msg's buffers */
		else
			bds = 1;
		if (space_left < bds)
			break;

		/* Convert and write CAN message to DMA buffer */
		dlc = grcan_len2dlc(source->len);
		if (dlc < 0) {
			/* Bad user input. Report the number of written messages
			 * or an error when non sent.
			 */
			if (ret <= 0)
				return GRCAN_RET_INVARG;
			break;
		}
		dest->head[1] = (dlc << 28) |
				((source->fdopts & GRCAN_FDMASK) << 25);
		dp = &dest->data0;
		for (i = 0; i < ((source->len + 7) / 8); i++) {
			*dp++ = source->data.dwords[i];
			if (dp >= (uint64_t *)txmax)
				dp = (uint64_t *)addr; /* wrap around */
		}
		if (source->extended) {
			tmp = (1 << 31) | (source->id & 0x3fffffff);
		} else {
			tmp = (source->id & 0xfff) << 18;
		}
		if (source->rtr)
			tmp |= (1 << 30);
		dest->head[0] = tmp;
		source++;	/* straight user buffer */
		dest += bds;
		if (dest >= txmax)
			dest = (struct grcanfd_bd0 *)((void *)dest - size);
		space_left -= bds;
		ret++;
	}

	/* A bus off interrupt may have occured after checking pDev->started */
	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	if (pDev->started == STATE_STARTED) {
		regs->tx0wr = (unsigned int) dest - addr;
		regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
	} else {
		DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
		ret = state2err[pDev->started];
	}
	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	return ret;
}

int grcanfd_write(
	void *d,
	CANFDMsg *msg,
	size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANFDMsg *source, *curr;
	unsigned int count, left;
	int nwritten;
	int req_cnt;

	DBGC(DBG_TX,"\n");

	if ((pDev->started != STATE_STARTED) || pDev->config.silent || pDev->flushing)
		return GRCAN_RET_NOTSTARTED;

	req_cnt = ucount;
	curr = source = (CANFDMsg *) msg;

	/* check proper length and buffer pointer */
	if (( req_cnt < 1) || (source == NULL) ){
		return GRCAN_RET_INVARG;
	}

	nwritten = grcan_hw_write_try_fd(pDev,pDev->regs,source,req_cnt);
	if (nwritten < 0) {
		return nwritten;
	}
	count = nwritten;
	if ( !(pDev->txblock && pDev->txcomplete && (count!=req_cnt)) ) {
		if ( count > 0 ) {
			/* Successfully transmitted chars (at least one char) */
			return count;
		}

		/* nothing written, shall we block? */
		if ( !pDev->txblock ) {
			/* non-blocking mode */
			return GRCAN_RET_TIMEOUT;
		}
	}

	/* if in txcomplete mode we need to transmit all chars */
	while((count == 0) || (pDev->txcomplete && (count!=req_cnt)) ){
		/*** block until room to fit all or as much of transmit buffer
		 * as possible before IRQ comes. Set up a valid IRQ point so
		 * that an IRQ is triggered when we can put a chunk of data
		 * into transmit fifo.
		 */

		/* Get the number of descriptors to wait for */
		curr = &source[count];
		if (curr->fdopts & GRCAN_FDOPT_FDFRM)
			left = grcan_numbds(curr->len); /* next msg's buffers */
		else
			left = 1;

		if (pDev->txcomplete) {
			/* Wait for all messages to fit into descriptor table.
			 * Assume all following msgs are single descriptors.
			 */
			left += req_cnt - count - 1;
			if (left > ((pDev->txbuf_size/GRCAN_MSG_SIZE)/2)) {
				left = (pDev->txbuf_size/GRCAN_MSG_SIZE)/2;
			}

		}

		nwritten = grcan_wait_txspace(pDev,left);
		/* Wait until more room in transmit buffer */
		if ( nwritten ) {
			/* The wait has been aborted, probably due to 
			 * the device driver has been closed by another
			 * thread. To avoid deadlock we return directly
			 * with error status.
			 */
			return nwritten;
		}

		/* Try read bytes from circular buffer */
		nwritten = grcan_hw_write_try_fd(
			pDev,
			pDev->regs,
			source+count,
			req_cnt-count);

		if (nwritten < 0) {
			/* Write was aborted by bus-off. */
			return nwritten;
		}
		count += nwritten;
	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */

	return count;
}

int grcanfd_set_speed(void *d, unsigned int nom_hz, unsigned int fd_hz)
{
	struct grcan_priv *pDev = d;
	struct grlib_canbtrs_timing nom, fd;
	int ret;

	FUNCDBG();

	/* cannot change speed during run mode */
	if ((pDev->started == STATE_STARTED) || !pDev->fd_capable)
		return -1;

	/* get speed rate from argument */
	ret = grlib_canbtrs_calc_timing(
		nom_hz, pDev->corefreq_hz, GRCAN_SAMPLING_POINT,
		&grcanfd_nom_btrs_ranges, &nom);
	if ( ret )
		return -2;
	ret = grlib_canbtrs_calc_timing(
		fd_hz, pDev->corefreq_hz, GRCAN_SAMPLING_POINT,
		&grcanfd_fd_btrs_ranges, &fd);
	if ( ret )
		return -2;

	/* save timing/speed */
	pDev->config.timing = *(struct grcan_timing *)&nom;
	pDev->config.timing_fd.scaler = fd.scaler;
	pDev->config.timing_fd.ps1 = fd.ps1;
	pDev->config.timing_fd.ps2 = fd.ps2;
	pDev->config.timing_fd.sjw = fd.rsj;
	pDev->config.timing_fd.resv_zero = 0;
	pDev->config_changed = 1;

	return 0;

}

int grcanfd_set_btrs(
	void *d,
	const struct grcanfd_timing *nominal,
	const struct grcanfd_timing *fd)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	/* Set BTR registers manually
	 * Read GRCAN/HurriCANe Manual.
	 */
	if ((pDev->started == STATE_STARTED) || !pDev->fd_capable)
		return -1;

	if (!nominal)
		return -2;

	pDev->config.timing.scaler = nominal->scaler;
	pDev->config.timing.ps1 = nominal->ps1;
	pDev->config.timing.ps2 = nominal->ps2;
	pDev->config.timing.rsj = nominal->sjw;
	pDev->config.timing.bpr = 0;
	if (fd) {
		pDev->config.timing_fd = *fd;
	} else {
		memset(&pDev->config.timing_fd, 0,
			sizeof(struct grcanfd_timing));
	}
	pDev->config_changed = 1;

	return 0;
}
