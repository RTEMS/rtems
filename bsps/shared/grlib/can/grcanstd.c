/*
 *  non-FD specific function for GRCAN driver
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

static int grcan_hw_read_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg * buffer,
	int max
)
{
	int i, j;
	CANMsg *dest;
	struct grcan_msg *source, tmp;
	unsigned int wp, rp, size, rxmax, addr;
	int trunk_msg_cnt;

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
		trunk_msg_cnt = grcan_hw_rxavail(rp, wp, size);

		/* truncate size if user space buffer hasn't room for
		 * all received chars.
		 */
		if (trunk_msg_cnt > max)
			trunk_msg_cnt = max;

		/* Read until i is 0 */
		i = trunk_msg_cnt;

		addr = (unsigned int)pDev->rx;
		source = (struct grcan_msg *)(addr + rp);
		dest = buffer;
		rxmax = addr + (size - GRCAN_MSG_SIZE);

		/* Read as many can messages as possible */
		while (i > 0) {
			/* Read CAN message from DMA buffer */
			tmp.head[0] = READ_DMA_WORD(&source->head[0]);
			tmp.head[1] = READ_DMA_WORD(&source->head[1]);
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
			dest->len = tmp.head[1] >> 28;
			for (j = 0; j < dest->len; j++)
				dest->data[j] = READ_DMA_BYTE(&source->data[j]);

			/* wrap around if neccessary */
			source =
			    ((unsigned int)source >= rxmax) ?
			    (struct grcan_msg *)addr : source + 1;
			dest++;	/* straight user buffer */
			i--;
		}
		{
			/* A bus off interrupt may have occured after checking pDev->started */
			SPIN_IRQFLAGS(oldLevel);

			SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
			if (pDev->started == STATE_STARTED) {
				regs->rx0rd = (unsigned int) source - addr;
				regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
			} else {
				DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
				trunk_msg_cnt = state2err[pDev->started];
			}
			SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		}
		return trunk_msg_cnt;
	}
	return 0;
}

static int grcan_hw_write_try(
	struct grcan_priv *pDev,
	struct grcan_regs *regs,
	CANMsg * buffer,
	int count
)
{
	unsigned int rp, wp, size, txmax, addr;
	int ret;
	struct grcan_msg *dest;
	CANMsg *source;
	int space_left;
	unsigned int tmp;
	int i;

	DBGC(DBG_TX, "\n");
	/*FUNCDBG(); */

	rp = READ_REG(&regs->tx0rd);
	wp = READ_REG(&regs->tx0wr);
	size = READ_REG(&regs->tx0size);

	space_left = grcan_hw_txspace(rp, wp, size);

	/* is circular fifo full? */
	if (space_left < 1)
		return 0;

	/* Truncate size */
	if (space_left > count)
		space_left = count;
	ret = space_left;

	addr = (unsigned int)pDev->tx;

	dest = (struct grcan_msg *)(addr + wp);
	source = (CANMsg *) buffer;
	txmax = addr + (size - GRCAN_MSG_SIZE);

	while (space_left > 0) {
		/* Convert and write CAN message to DMA buffer */
		if (source->extended) {
			tmp = (1 << 31) | (source->id & 0x3fffffff);
		} else {
			tmp = (source->id & 0xfff) << 18;
		}
		if (source->rtr)
			tmp |= (1 << 30);
		dest->head[0] = tmp;
		dest->head[1] = source->len << 28;
		for (i = 0; i < source->len; i++)
			dest->data[i] = source->data[i];
		source++;	/* straight user buffer */
		dest =
		    ((unsigned int)dest >= txmax) ?
		    (struct grcan_msg *)addr : dest + 1;
		space_left--;
	}

	{
		/* A bus off interrupt may have occured after checking pDev->started */
		SPIN_IRQFLAGS(oldLevel);

		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		if (pDev->started == STATE_STARTED) {
			regs->tx0wr = (unsigned int) dest - addr;
			regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
		} else {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			ret = state2err[pDev->started];
		}
		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}
	return ret;
}


int grcan_read(void *d, CANMsg *msg, size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANMsg *dest;
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

	nread = grcan_hw_read_try(pDev,pDev->regs,dest,req_cnt);
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
		nread = grcan_hw_read_try(
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

int grcan_write(void *d, CANMsg *msg, size_t ucount)
{
	struct grcan_priv *pDev = d;
	CANMsg *source;
	unsigned int count, left;
	int nwritten;
	int req_cnt;

	DBGC(DBG_TX,"\n");

	if ((pDev->started != STATE_STARTED) || pDev->config.silent || pDev->flushing)
		return GRCAN_RET_NOTSTARTED;

	req_cnt = ucount;
	source = (CANMsg *) msg;

	/* check proper length and buffer pointer */
	if (( req_cnt < 1) || (source == NULL) ){
		return GRCAN_RET_INVARG;
	}

	nwritten = grcan_hw_write_try(pDev,pDev->regs,source,req_cnt);
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
		/*** block until room to fit all or as much of transmit buffer as possible
		 * IRQ comes. Set up a valid IRQ point so that an IRQ is received 
		 * when we can put a chunk of data into transmit fifo
		 */
		if ( !pDev->txcomplete ){
			left = 1; /* wait for anything to fit buffer */
		}else{
			left = req_cnt - count; /* wait for all data to fit in buffer */

			/* never wait for more than the half the maximum size of the transmit
			 * buffer 
			 * Why? We need some time to fill buffer before hw catches up.
			 */
			if ( left > ((pDev->txbuf_size/GRCAN_MSG_SIZE)/2) ){
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
		nwritten = grcan_hw_write_try(
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


int grcan_set_speed(void *d, unsigned int speed)
{
	struct grcan_priv *pDev = d;
	struct grcan_timing timing;
	int ret;

	FUNCDBG();

	/* cannot change speed during run mode */
	if ((pDev->started == STATE_STARTED) || pDev->fd_capable)
		return -1;

	/* get speed rate from argument */
	ret = grlib_canbtrs_calc_timing(
		speed, pDev->corefreq_hz, GRCAN_SAMPLING_POINT,
		&grcan_btrs_ranges, (struct grlib_canbtrs_timing *)&timing);
	if (ret)
		return -2;

	/* save timing/speed */
	pDev->config.timing = timing;
	pDev->config_changed = 1;

	return 0;
}

int grcan_set_btrs(void *d, const struct grcan_timing *timing)
{
	struct grcan_priv *pDev = d;

	FUNCDBG();

	/* Set BTR registers manually
	 * Read GRCAN/HurriCANe Manual.
	 */
	if ((pDev->started == STATE_STARTED) || pDev->fd_capable)
		return -1;

	if ( !timing )
		return -2;

	pDev->config.timing = *timing;
	pDev->config_changed = 1;

	return 0;
}
