/*
 *  SPICTRL SPI Driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __SPICTRL_H__
#define __SPICTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void spictrl_register_drv (void);

/*** REGISTER LAYOUT ***/
struct spictrl_regs {
	volatile unsigned int capability;	/* 0x00 */
	volatile unsigned int resv[7];		/* 0x04 */
	volatile unsigned int mode;		/* 0x20 */
	volatile unsigned int event;		/* 0x24 */
	volatile unsigned int mask;		/* 0x28 */
	volatile unsigned int command;		/* 0x2c */
	volatile unsigned int tx;		/* 0x30 */
	volatile unsigned int rx;		/* 0x34 */
	volatile unsigned int slvsel;		/* 0x38 */
	volatile unsigned int am_slvsel;	/* 0x3c */
	volatile unsigned int am_cfg;		/* 0x40 */
	volatile unsigned int am_period;	/* 0x44 */
	int reserved0[2];
	volatile unsigned int am_mask[4];	/* 0x50-0x5C */
	int reserved1[(0x200-0x60)/4];
	volatile unsigned int am_tx[128];	/* 0x200-0x3FC */
	volatile unsigned int am_rx[128];	/* 0x400-0x5FC */
};

/* -- About automated periodic transfer mode --
 *
 * Core must support this feature.
 *
 * The SPI core must be configured in periodic mode before
 * writing the data into the transfer FIFO which will be used
 * mutiple times in different transfers, it will also make
 * the receive FIFO to be updated.
 *
 * In periodic mode the following sequence is performed,
 *   1. start()
 *   2. ioctl(CONFIG, &config)    - Enable periodic mode
 *   3. set_address()
 *   4. write()                   - Fills TX FIFO, this has some constraints
 *   5. ioctl(START)              - Starts the periodic transmission of the TX FIFO
 *   6. read()                    - Read one response of the tranistted data. It will 
 *                                  hang until data is available. If hanging is not an
 *                                  options use ioctl(STATUS)
 *   7. go back to 6.
 *
 *   8. ioctl(STOP)               - Stop to set up a new periodic or normal transfer
 *   9. stop()
 *
 * Note that the the read length must equal the total write length.
 */

/* Custom SPICTRL driver ioctl commands */
#define SPICTRL_IOCTL_PERIOD_START	5000	/* Start automated periodic transfer mode */
#define SPICTRL_IOCTL_PERIOD_STOP	5001	/* Stop to SPI core from doing periodic transfers */
#define SPICTRL_IOCTL_CONFIG		5002	/* Configure Periodic transfer mode (before calling write() and START) */
#define SPICTRL_IOCTL_STATUS		5003	/* Get status */

#define SPICTRL_IOCTL_PERIOD_READ	5005	/* Write transmit registers and mask register 
						 * (only in automatic periodic mode) 
						 * Note that it is probably prefferred to read
						 * the received words using the read() using
						 * operations instead.
						 */
#define SPICTRL_IOCTL_PERIOD_WRITE	5006	/* Read receive registers and mask register 
						 * (only in automatic periodic mode) */
#define SPICTRL_IOCTL_REGS		5007	/* Get SPICTRL Register */

/* SPICTRL_IOCTL_CONFIG argument */
struct spictrl_ioctl_config {
	int		clock_gap;	/* Clock GAP between */
	unsigned int	flags;		/* Normal mode flags */
	int		periodic_mode;	/* 1=Enables Automated periodic transfers if supported by hardware */
	unsigned int	period;		/* Number of clocks between automated transfers are started */
	unsigned int	period_flags;	/* Options */
	unsigned int	period_slvsel;	/* Slave Select when transfer is not active, default is 0xffffffff */
};
#define SPICTRL_FLAGS_TAC		0x10

#define SPICTRL_PERIOD_FLAGS_ERPT	0x80	/* Trigger start-period from external signal */
#define SPICTRL_PERIOD_FLAGS_SEQ	0x40
#define SPICTRL_PERIOD_FLAGS_STRICT	0x20
#define SPICTRL_PERIOD_FLAGS_OVTB	0x10
#define SPICTRL_PERIOD_FLAGS_OVDB	0x08
#define SPICTRL_PERIOD_FLAGS_ASEL	0x04
#define SPICTRL_PERIOD_FLAGS_EACT	0x01

/* SPICTRL_IOCTL_PERIOD_READ and SPICTRL_IOCTL_PERIOD_WRITE Argument data structure 
 *
 * Note that the order of reading the mask registers are different for read/write
 *      operation. See options notes.
 */
struct spictrl_period_io {
	int options;	/* READ: bit0=Read Mask Registers into masks[].
			 *       bit1=Read Receive registers according to masks[]
			 *            (after reading masks).
			 *
			 * WRITE: bit0=Update Mask accoring to masks[].
			 *        bit1=Update Transmit registers according to masks[].
			 *             (before reading masks)
			 */
	unsigned int masks[4];

	void *data;	/* Data read sequentially according to masks[] bit. */
};

#ifdef __cplusplus
}
#endif

#endif
