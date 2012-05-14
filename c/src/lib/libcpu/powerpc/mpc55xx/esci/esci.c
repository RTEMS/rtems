/**
 * @file
 *
 * @ingroup mpc55xx_esci
 *
 * @brief Source file for the Enhanced Serial Communication Interface (eSCI).
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/* Include order is important */
#include <mpc55xx/regs.h>
#include <mpc55xx/esci.h>
#include <bsp/irq.h>

#include <assert.h>
#include <unistd.h>
#include <termios.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/bspIo.h>

#include <bspopts.h>

/* Evil define conflicts */
#define TERMIOS_CR1 CR1
#undef CR1
#define TERMIOS_CR2 CR2
#undef CR2

#define MPC55XX_ESCI_IRQ_PRIORITY MPC55XX_INTC_DEFAULT_PRIORITY

#define MPC55XX_ESCI_IS_MINOR_INVALD(minor) ((minor) < 0 || (minor) >= MPC55XX_ESCI_NUMBER)

/**
 * @brief eSCI driver table.
 */
mpc55xx_esci_driver_entry mpc55xx_esci_driver_table [MPC55XX_ESCI_NUMBER] = { {
		.regs = &ESCI_A,
		.device_name = "/dev/ttyS0",
		.use_termios = 1,
		.use_interrupts = MPC55XX_ESCI_USE_INTERRUPTS,
		.tty = NULL,
		.irq_number = MPC55XX_IRQ_ESCI(0)
	}, {
		.regs = &ESCI_B,
		.device_name = "/dev/ttyS1",
		.use_termios = 1,
		.use_interrupts = MPC55XX_ESCI_USE_INTERRUPTS,
		.tty = NULL,
		.irq_number = MPC55XX_IRQ_ESCI(1)
	}
};

/**
 * @brief Default termios configuration.
 */
static const struct termios mpc55xx_esci_termios_default = {
	.c_cflag = CS8 | CREAD | CLOCAL | B115200
};

/**
 * @name Low-Level
 * @{
 */

/**
 * @brief Reads one character from the receive register.
 *
 * @note Waits for the receive data register full flag.
 */
static inline uint8_t mpc55xx_esci_read_char( mpc55xx_esci_driver_entry *e)
{
	volatile union ESCI_SR_tag *status = &e->regs->SR;
	volatile union ESCI_DR_tag *data = &e->regs->DR;
	union ESCI_SR_tag sr = MPC55XX_ZERO_FLAGS;

	while (status->B.RDRF == 0) {
		/* Wait */
	}

	/* Clear flag */
	sr.B.RDRF = 1;
	status->R = sr.R;

	/* Read */
	return data->B.D;
}

static inline void mpc55xx_esci_write_char(mpc55xx_esci_driver_entry *e, char c)
{
	static const union ESCI_SR_tag clear_tdre = { .B = { .TDRE = 1 } };
	volatile struct ESCI_tag *esci = e->regs;
	rtems_interrupt_level level;
	bool done = false;

	rtems_interrupt_disable(level);
	if (e->transmit_nest_level == 0) {
		union ESCI_CR1_tag cr1 = { .R = esci->CR1.R };

		if (cr1.B.TIE != 0) {
			cr1.B.TIE = 0;
			e->transmit_nest_level = 1;
			esci->CR1.R = cr1.R;
		}
	} else {
		++e->transmit_nest_level;
	}
	rtems_interrupt_enable(level);

	while (!done) {
		rtems_interrupt_disable(level);
		bool tx = e->transmit_in_progress;
		if (!tx || (tx && esci->SR.B.TDRE)) {
			esci->SR.R = clear_tdre.R;
			esci->DR.B.D = c;
			e->transmit_in_progress = true;
			done = true;
		}
		rtems_interrupt_enable(level);
	}

	rtems_interrupt_disable(level);
	if (e->transmit_nest_level > 0) {
		--e->transmit_nest_level;

		if (e->transmit_nest_level == 0) {
			union ESCI_CR1_tag cr1 = { .R = esci->CR1.R };

			cr1.B.TIE = 1;
			esci->CR1.R = cr1.R;
		}
	}
	rtems_interrupt_enable(level);
}

static inline void mpc55xx_esci_interrupts_enable( mpc55xx_esci_driver_entry *e)
{
	union ESCI_CR1_tag cr1 = MPC55XX_ZERO_FLAGS;
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);
	cr1.R = e->regs->CR1.R;
	cr1.B.RIE = 1;
	cr1.B.TIE = 1;
	e->regs->CR1.R = cr1.R;
	rtems_interrupt_enable( level);
}

static inline void mpc55xx_esci_interrupts_disable( mpc55xx_esci_driver_entry *e)
{
	union ESCI_CR1_tag cr1 = MPC55XX_ZERO_FLAGS;
	rtems_interrupt_level level;

	rtems_interrupt_disable( level);
	cr1.R = e->regs->CR1.R;
	cr1.B.RIE = 0;
	cr1.B.TIE = 0;
	e->regs->CR1.R = cr1.R;
	rtems_interrupt_enable( level);
}

/** @} */

/**
 * @name Termios Support
 * @{
 */

/**
 * @brief Opens port @a minor.
 *
 * @return Status code.
 */
static int mpc55xx_esci_termios_first_open( int major, int minor, void *arg)
{
	int rv = 0;
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];
	struct rtems_termios_tty *tty = ((rtems_libio_open_close_args_t *) arg)->iop->data1;

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	/* Connect TTY */
	e->tty = tty;

	/* Enable interrupts */
	if (e->use_interrupts) {
		mpc55xx_esci_interrupts_enable( e);
	}

	rv = rtems_termios_set_initial_baud( e->tty, 115200);
	assert(rv == 0);

	return 0;
}

/**
 * @brief Closes port @a minor.
 *
 * @return Status code.
 */
static int mpc55xx_esci_termios_last_close( int major, int minor, void* arg)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	/* Disable interrupts */
	mpc55xx_esci_interrupts_disable( e);

	/* Disconnect TTY */
	e->tty = NULL;

	return RTEMS_SUCCESSFUL;
}

/**
 * @brief Reads one character from port @a minor.
 *
 * @return Returns the character or -1 on error.
 */
static int mpc55xx_esci_termios_poll_read( int minor)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];
	volatile union ESCI_SR_tag *status = &e->regs->SR;
	volatile union ESCI_DR_tag *data = &e->regs->DR;
	union ESCI_SR_tag sr = MPC55XX_ZERO_FLAGS;

	if (status->B.RDRF == 0) {
		return -1;
	}

	/* Clear flag */
	sr.B.RDRF = 1;
	status->R = sr.R;

	/* Read */
	return data->B.D;
}

/**
 * @brief Writes @a n characters from @a out to port @a minor.
 *
 * @return Returns number of chars sent on success or -1 otherwise.
 */
static int mpc55xx_esci_termios_poll_write( int minor, const char *out,
					    size_t n)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];
	size_t i = 0;

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return -1;
	}

	/* Write */
	for (i = 0; i < n; ++i) {
		mpc55xx_esci_write_char( e, out [i]);
	}

	return n;
}

/**
 * @brief Writes one character from @a out to port @a minor.
 *
 * @return (always 0).
 *
 * @note The buffer @a out has to provide at least one character.
 * This function assumes that the transmit data register is empty.
 */
static int mpc55xx_esci_termios_write( int minor, const char *out, size_t n)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	e->regs->DR.B.D = out [0];
	e->transmit_in_progress = true;
	rtems_interrupt_enable(level);

	return 0;
}

/* FIXME, TODO */
extern uint32_t bsp_clock_speed;

/**
 * @brief Sets attributes of port @a minor according to termios attributes @a t.
 *
 * @return Status code.
 */
static int mpc55xx_esci_termios_set_attributes( int minor, const struct termios *t)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];
	volatile struct ESCI_tag *regs = e->regs;
	union ESCI_CR1_tag cr1 = MPC55XX_ZERO_FLAGS;
	union ESCI_CR2_tag cr2 = MPC55XX_ZERO_FLAGS;
	unsigned br = 0;

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	/* Enable module */
	cr2.B.MDIS = 0;

	/* Interrupts */
	if (e->use_interrupts && e->tty != NULL) {
		cr1.B.RIE = 1;
		cr1.B.TIE = 1;
	} else {
		cr1.B.RIE = 0;
		cr1.B.TIE = 0;
	}
	cr1.B.TCIE = 0;
	cr1.B.ILIE = 0;
	cr2.B.IEBERR = 0;
	cr2.B.ORIE = 0;
	cr2.B.NFIE = 0;
	cr2.B.FEIE = 0;
	cr2.B.PFIE = 0;

	/* Disable receiver wake-up standby */
	cr1.B.RWU = 0;

	/* Disable DMA channels */
	cr2.B.RXDMA = 0;
	cr2.B.TXDMA = 0;

	/* Idle line type */
	cr1.B.ILT = 0;

	/* Disable loops */
	cr1.B.LOOPS = 0;

	/* Enable or disable receiver */
	cr1.B.RE = (t->c_cflag & CREAD) ? 1 : 0;

	/* Enable transmitter */
	cr1.B.TE = 1;

	/* Baud rate */
	switch (t->c_cflag & CBAUD) {
		case B50: br = 50; break;
		case B75: br = 75; break;
		case B110: br = 110; break;
		case B134: br = 134; break;
		case B150: br = 150; break;
		case B200: br = 200; break;
		case B300: br = 300; break;
		case B600: br = 600; break;
		case B1200: br = 1200; break;
		case B1800: br = 1800; break;
		case B2400: br = 2400; break;
		case B4800: br = 4800; break;
		case B9600: br = 9600; break;
		case B19200: br = 19200; break;
		case B38400: br = 38400; break;
		case B57600: br = 57600; break;
		case B115200: br = 115200; break;
		case B230400: br = 230400; break;
		case B460800: br = 460800; break;
		default: br = 0; break;
	}
	if (br > 0) {
		br = bsp_clock_speed / (16 * br);
		br = (br > 8191) ? 8191 : br;
	} else {
		br = 0;
	}
	cr1.B.SBR = br;

	/* Number of data bits */
	if ((t->c_cflag & CSIZE) != CS8) {
		return RTEMS_IO_ERROR;
	}
	cr1.B.M = 0;

	/* Parity */
	cr1.B.PE = (t->c_cflag & PARENB) ? 1 : 0;
	cr1.B.PT = (t->c_cflag & PARODD) ? 1 : 0;

	/* Stop bits */
	if ( t->c_cflag & CSTOPB ) {
		/* Two stop bits */
		return RTEMS_IO_ERROR;
	}

	/* Disable LIN */
	regs->LCR.R = 0;

	/* Set control registers */
	regs->CR2.R = cr2.R;
	regs->CR1.R = cr1.R;

	return RTEMS_SUCCESSFUL;
}

/**
 * @brief Interrupt handler.
 */
static void mpc55xx_esci_termios_interrupt_handler( void *arg)
{
	mpc55xx_esci_driver_entry *e = (mpc55xx_esci_driver_entry *) arg;
	volatile union ESCI_SR_tag *status = &e->regs->SR;
	volatile union ESCI_DR_tag *data = &e->regs->DR;
	union ESCI_SR_tag sr = MPC55XX_ZERO_FLAGS;
	union ESCI_SR_tag active = MPC55XX_ZERO_FLAGS;
	rtems_interrupt_level level;

	/* Status */
	sr.R = status->R;

	/* Receive data register full? */
	if (sr.B.RDRF != 0) {
		active.B.RDRF = 1;
	}

	/* Transmit data register empty? */
	if (sr.B.TDRE != 0) {
		active.B.TDRE = 1;
	}

	/* Clear flags */
	rtems_interrupt_disable(level);
	status->R = active.R;
	e->transmit_in_progress = false;
	rtems_interrupt_enable(level);

	/* Enqueue */
	if (active.B.RDRF != 0) {
		char c = data->B.D;
		rtems_termios_enqueue_raw_characters( e->tty, &c, 1);
	}

	/* Dequeue */
	if (active.B.TDRE != 0) {
		rtems_termios_dequeue_characters( e->tty, 1);
	}
}

/** @} */

/**
 * @brief Termios callbacks with interrupt support.
 */
static const rtems_termios_callbacks mpc55xx_esci_termios_callbacks = {
	.firstOpen = mpc55xx_esci_termios_first_open,
	.lastClose = mpc55xx_esci_termios_last_close,
	.pollRead = NULL,
	.write = mpc55xx_esci_termios_write,
	.setAttributes = mpc55xx_esci_termios_set_attributes,
	.stopRemoteTx = NULL,
	.startRemoteTx = NULL,
	.outputUsesInterrupts = TERMIOS_IRQ_DRIVEN
};

/**
 * @brief Termios callbacks with polling functions (no interrupts).
 */
static const rtems_termios_callbacks mpc55xx_esci_termios_callbacks_polled = {
	.firstOpen = mpc55xx_esci_termios_first_open,
	.lastClose = mpc55xx_esci_termios_last_close,
	.pollRead = mpc55xx_esci_termios_poll_read,
	.write = mpc55xx_esci_termios_poll_write,
	.setAttributes = mpc55xx_esci_termios_set_attributes,
	.stopRemoteTx = NULL,
	.startRemoteTx = NULL,
	.outputUsesInterrupts = TERMIOS_POLLED
};

/**
 * @name Console Driver
 * @{
 */

rtems_device_driver console_initialize( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int termios_do_init = 1;
	rtems_device_minor_number i = 0;
	mpc55xx_esci_driver_entry *e = NULL;

	for (i = 0; i < MPC55XX_ESCI_NUMBER; ++i) {
		e = &mpc55xx_esci_driver_table [i];
		sc = rtems_io_register_name ( e->device_name, major, i);
		if (sc != RTEMS_SUCCESSFUL) {
			/* FIXME */
			rtems_fatal_error_occurred(0xdeadbeef);
		}
		if (i == MPC55XX_ESCI_CONSOLE_MINOR) {
			sc = rtems_io_register_name( CONSOLE_DEVICE_NAME, major, i);
			if (sc != RTEMS_SUCCESSFUL) {
				/* FIXME */
				rtems_fatal_error_occurred(0xdeadbeef);
			}
		}
		if (e->use_termios && termios_do_init) {
			if (termios_do_init) {
				termios_do_init = 0;
				rtems_termios_initialize();
			}
			if (e->use_interrupts) {
				sc = mpc55xx_interrupt_handler_install(
					e->irq_number,
					"eSCI",
					RTEMS_INTERRUPT_UNIQUE,
					MPC55XX_ESCI_IRQ_PRIORITY,
					mpc55xx_esci_termios_interrupt_handler,
					e
				);
				if (sc != RTEMS_SUCCESSFUL) {
					/* FIXME */
					rtems_fatal_error_occurred(0xdeadbeef);
				}
			}
		}
		mpc55xx_esci_termios_set_attributes( (int) i, &mpc55xx_esci_termios_default);
	}

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	if (e->use_termios) {
		if (e->use_interrupts) {
			sc =  rtems_termios_open( major, minor, arg, &mpc55xx_esci_termios_callbacks);
		} else {
			sc =  rtems_termios_open( major, minor, arg, &mpc55xx_esci_termios_callbacks_polled);
		}
	}

	return sc;
}

rtems_device_driver console_close( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	if (e->use_termios) {
		return rtems_termios_close( arg);
	}

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_read( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	if (e->use_termios) {
		return rtems_termios_read( arg);
	} else {
		rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *) arg;
		uint32_t i = 0;
		while (i < rw->count) {
			rw->buffer [i] = mpc55xx_esci_read_char( e);
			++i;
		}
		rw->bytes_moved = i;
	}

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_write( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	if (e->use_termios) {
		return rtems_termios_write( arg);
	} else {
		rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *) arg;
		uint32_t i = 0;
		while (i < rw->count) {
			if (rw->buffer [i] == '\n') {
				mpc55xx_esci_write_char( e, '\r');
			}
			mpc55xx_esci_write_char( e, rw->buffer [i]);
			++i;
		}
		rw->bytes_moved = i;
	}

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_control( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [minor];

	/* Check minor number */
	if (MPC55XX_ESCI_IS_MINOR_INVALD( minor)) {
		return RTEMS_INVALID_ID;
	}

	if (e->use_termios) {
		return rtems_termios_ioctl( arg);
	}

	return RTEMS_NOT_DEFINED;
}

/** @} */

/**
 * @name BSP Character Output
 * @{
 */

static void mpc55xx_esci_output_char( char c)
{
	mpc55xx_esci_driver_entry *e = &mpc55xx_esci_driver_table [MPC55XX_ESCI_CONSOLE_MINOR];

	mpc55xx_esci_interrupts_disable( e);
	if (c == '\n') {
		mpc55xx_esci_write_char( e, '\r');
	}
	mpc55xx_esci_write_char( e, c);
	mpc55xx_esci_interrupts_enable( e);
}

static void mpc55xx_esci_output_char_init( char c)
{
	mpc55xx_esci_termios_set_attributes( MPC55XX_ESCI_CONSOLE_MINOR, &mpc55xx_esci_termios_default);
	mpc55xx_esci_output_char( c);
	BSP_output_char = mpc55xx_esci_output_char;
}

/** @} */

BSP_output_char_function_type BSP_output_char = mpc55xx_esci_output_char_init;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
