/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  GRCAN driver
 *
 *  COPYRIGHT (c) 2007-2019.
 *  Cobham Gaisler AB.
 *
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

#ifndef GRCAN_DEFAULT_BAUD
 /* default to 500kbits/s */
 #define GRCAN_DEFAULT_BAUD 500000
#endif

#ifndef GRCAN_SAMPLING_POINT
 #define GRCAN_SAMPLING_POINT 80
#endif

#define WRAP_AROUND_TX_MSGS 1
#define WRAP_AROUND_RX_MSGS 2
#define GRCAN_MSG_SIZE sizeof(struct grcan_msg)

struct grcan_msg {
	unsigned int head[2];
	unsigned char data[8];
};

struct grcan_config {
	struct grcan_timing timing;
	struct grcanfd_timing timing_fd;
	struct grcan_selection selection;
	int abort;
	int silent;
};

struct grcan_priv {
	struct drvmgr_dev *dev;	/* Driver manager device */
	char devName[52];	/* Device Name */
	unsigned int baseaddr, ram_base;
	struct grcan_regs *regs;
	int irq;
	int minor;
	int open;
	int started;
	unsigned int channel;
	int flushing;
	unsigned int corefreq_hz;
	int fd_capable;

	/* Circular DMA buffers */
	void *_rx, *_rx_hw;
	void *_tx, *_tx_hw;
	void *txbuf_adr;
	void *rxbuf_adr;
	struct grcan_msg *rx;
	struct grcan_msg *tx;
	unsigned int rxbuf_size;    /* requested RX buf size in bytes */
	unsigned int txbuf_size;    /* requested TX buf size in bytes */

	int txblock, rxblock;
	int txcomplete, rxcomplete;

	struct grcan_filter sfilter;
	struct grcan_filter afilter;
	int config_changed; /* 0=no changes, 1=changes ==> a Core reset is needed */
	struct grcan_config config;
	struct grcan_stats stats;

	rtems_id rx_sem, tx_sem, txempty_sem, dev_sem;
	SPIN_DECLARE(devlock);
};

#ifdef GRCAN_REG_BYPASS_CACHE
#define READ_REG(address) grlib_read_uncached32((unsigned int)(address))
#else
#define READ_REG(address) (*(volatile unsigned int *)(address))
#endif

#ifdef GRCAN_DMA_BYPASS_CACHE
#define READ_DMA_DOUBLE(address) grlib_read_uncached64((uint64_t *)(address))
#define READ_DMA_WORD(address) grlib_read_uncached32((unsigned int)(address))
#define READ_DMA_BYTE(address) grlib_read_uncached8((unsigned int)(address))
#else
#define READ_DMA_DOUBLE(address) (*(volatile uint64_t *)(address))
#define READ_DMA_WORD(address) (*(volatile unsigned int *)(address))
#define READ_DMA_BYTE(address) (*(volatile unsigned char *)(address))
#endif

extern int state2err[4];
extern struct grlib_canbtrs_ranges grcan_btrs_ranges;
extern struct grlib_canbtrs_ranges grcanfd_nom_btrs_ranges;
extern struct grlib_canbtrs_ranges grcanfd_fd_btrs_ranges;

int grcan_wait_rxdata(struct grcan_priv *pDev, int min);
int grcan_wait_txspace(struct grcan_priv *pDev, int min);

static inline unsigned int grcan_hw_rxavail(
	unsigned int rp,
	unsigned int wp,
	unsigned int size)
{
	if (rp == wp) {
		/* read pointer and write pointer is equal only
		 * when RX buffer is empty.
		 */
		return 0;
	}

	if (wp > rp) {
		return (wp - rp) / GRCAN_MSG_SIZE;
	} else {
		return (size - (rp - wp)) / GRCAN_MSG_SIZE;
	}
}

static inline unsigned int grcan_hw_txspace(
	unsigned int rp,
	unsigned int wp,
	unsigned int size)
{
	unsigned int left;

	if (rp == wp) {
		/* read pointer and write pointer is equal only
		 * when TX buffer is empty.
		 */
		return size / GRCAN_MSG_SIZE - WRAP_AROUND_TX_MSGS;
	}

	/* size - 4 - abs(read-write) */
	if (wp > rp) {
		left = size - (wp - rp);
	} else {
		left = rp - wp;
	}

	return left / GRCAN_MSG_SIZE - WRAP_AROUND_TX_MSGS;
}
