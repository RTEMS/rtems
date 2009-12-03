/*	$NetBSD: gti2c.c,v 1.2 2005/02/27 00:27:21 perry Exp $	*/

/*
 * Copyright (c) 2005 Brocade Communcations, inc.
 * All rights reserved.
 *
 * Written by Matt Thomas for Brocade Communcations, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Brocade Communications, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BROCADE COMMUNICATIONS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL EITHER BROCADE COMMUNICATIONS, INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Fixed many things + ported to RTEMS by Till Straumann, 2005 */

#include <stdio.h>
#include <rtems.h>
#include <libcpu/io.h>
#include <sys/errno.h>
#include <rtems/bspIo.h>
#include <rtems/score/sysstate.h>
#include <bsp/irq.h>
#include <rtems/libi2c.h>

#include <sys/cdefs.h>

#include <bsp/gtintrreg.h>
#include <bsp/gti2creg.h>
#include <bsp/gti2c_busdrv.h>

#define ENABLE_IRQ_AT_PIC_HACK	/* workaround for a bad HW bug */
#undef  DEBUG

#ifndef BSP_IRQ_MIN_PRIO
#define BSP_IRQ_MIN_PRIO 1
#endif

struct gti2c_softc {
	uint32_t	sc_gt;
	uint32_t	sc_cntl;
	int			sc_inited;
	rtems_id	sc_sync;
	int			sc_irqs; /* statistics */
};

#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

typedef struct {
	rtems_libi2c_bus_t	bus_desc;	
	struct gti2c_softc	pvt;
} gti2c_desc_rec, *gti2c_desc;

STATIC rtems_status_code
gt_i2c_init(rtems_libi2c_bus_t *bh);
STATIC rtems_status_code
gt_i2c_send_start(rtems_libi2c_bus_t *bh);
STATIC rtems_status_code
gt_i2c_send_stop(rtems_libi2c_bus_t *bh);
STATIC rtems_status_code
gt_i2c_send_addr(rtems_libi2c_bus_t *bh, uint32_t addr, int rw);
STATIC int
gt_i2c_read_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len);
STATIC int
gt_i2c_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len);

static rtems_libi2c_bus_ops_t myops = {
	init:			gt_i2c_init,
	send_start:		gt_i2c_send_start,
	send_stop:		gt_i2c_send_stop,
	send_addr:		gt_i2c_send_addr,
	read_bytes:		gt_i2c_read_bytes,
	write_bytes:	gt_i2c_write_bytes,
};

static gti2c_desc_rec my_bus_tbl = {
	{
		ops:	&myops,
		size:	sizeof(my_bus_tbl),
	},/* public fields */
	{
		sc_gt:		BSP_MV64x60_BASE,
		sc_cntl:	I2C_Control_TWSIEn,
		sc_inited:	0,
		sc_sync:	0
	} /* our private fields */
};


static inline uint32_t
gt_read(uint32_t base, uint32_t off)
{
	return in_le32((volatile unsigned*)(base+off));
}

static inline void
gt_write(uint32_t base, uint32_t off, uint32_t val)
{
	out_le32((volatile unsigned*)(base+off), val);
}


static inline void
disable_irq(struct gti2c_softc *sc)
{
uint32_t v = gt_read(sc->sc_gt, I2C_REG_Control);
	gt_write(sc->sc_gt, I2C_REG_Control, v & ~I2C_Control_IntEn);
}


static rtems_status_code
gt_i2c_wait(struct gti2c_softc *sc, uint32_t control, uint32_t desired_status)
{
	uint32_t status;
	rtems_status_code rval;

	control |= I2C_Control_IntEn;

	gt_write(sc->sc_gt, I2C_REG_Control, control | sc->sc_cntl);

	if ( sc->sc_inited ) {

#ifdef ENABLE_IRQ_AT_PIC_HACK
		BSP_enable_irq_at_pic(BSP_IRQ_I2C);
#endif

		rval = rtems_semaphore_obtain(sc->sc_sync, RTEMS_WAIT, 100);

		if ( RTEMS_SUCCESSFUL != rval )
			return rval;
	} else {
		uint32_t then, now;

		/* run in polling mode - useful during init */
		if ( _System_state_Is_up(_System_state_Get()) ) {
			printk("WARNING: gti2c running in polled mode -- should initialize properly!\n");
		}

		asm volatile("mftb %0":"=r"(then));

		do {
			asm volatile("mftb %0":"=r"(now));
			/* poll timebase for .2 seconds assuming a bus clock of 100MHz */
			if ( now - then > (uint32_t)100000000/4/5 )
				return RTEMS_TIMEOUT;
		} while ( ! (I2C_Control_IFlg & gt_read(sc->sc_gt, I2C_REG_Control)) );
	}

	status = gt_read(sc->sc_gt, I2C_REG_Status);

	if ( status != desired_status && (status!=I2C_Status_ReStarted || desired_status!=I2C_Status_Started) )
		return RTEMS_IO_ERROR;

	return RTEMS_SUCCESSFUL;
}

static void
gt_i2c_intr(void *arg)
{
struct gti2c_softc * const sc = &my_bus_tbl.pvt;
	uint32_t v;

	v = gt_read(sc->sc_gt, I2C_REG_Control);
	if ((v & I2C_Control_IFlg) == 0) {
		printk("gt_i2c_intr: IRQ but IFlg not set??\n");
		return;
	}
	gt_write(sc->sc_gt, I2C_REG_Control, v & ~(I2C_Control_IntEn));
#if 0
	gt_read(sc->sc_gt, I2C_REG_Control);
	asm volatile("sync");
/* This is how bad it is: after turning off the IntEn bit, the line
 * still remains asserted! (shame on you.)
 *
 * The test below (on MVME6100; the MVME5500 has the same problem
 * but the main cause register address is different; substitute
 * 0xf100000c for 0xf1000c68 on a 5500).
 * 
 * The skew was 101 TB ticks or ~3us (bus freq 133MHz) which
 * really sucks.
 *
 * Therefore, we must disable the interrupt at the PIC 
 */
{unsigned from,to;
	asm volatile("mftb %0":"=r"(from));
	while ( in_le32((volatile unsigned*)0xf100000c) & 0x20 )
		;
	asm volatile("mftb %0":"=r"(to));
	printk("I2C IRQ remained asserted for %i TB ticks!\n",to-from);
}
#endif
#ifdef ENABLE_IRQ_AT_PIC_HACK
	BSP_disable_irq_at_pic(BSP_IRQ_I2C);
#endif

	sc->sc_irqs++;

	rtems_semaphore_release(sc->sc_sync);
}

STATIC rtems_status_code
gt_i2c_init(rtems_libi2c_bus_t *bh)
{
struct gti2c_softc * const	sc = &((gti2c_desc)bh)->pvt;
unsigned					m,n,N;

	disable_irq(sc);

	/* reset */
	gt_write(sc->sc_gt, I2C_REG_SoftReset, 0);
	gt_write(sc->sc_gt, I2C_REG_SlaveAddr, 0);
	gt_write(sc->sc_gt, I2C_REG_ExtSlaveAddr, 0);

	/* Set baud rate; I don't know the details
	 * but have to assume that it has to fit into 7 bits
	 * (as indicated by some experiment)
	 */
	n = 0, N=1<<n;
	do {
		n++, N<<=1;
		/* increase 2^n until m becomes small enough */
		m = BSP_bus_frequency / 10 / 62500 / N;
	} while ( m > 16 );

	/* n is at least 1 */
	if ( n > 8 ) {
		n = 8; m = 16;	/* nothing else we can do */
	}
	if ( 0 == m )
		m = 1; /* nothing we can do */

	gt_write(sc->sc_gt, I2C_REG_BaudRate, I2C_BaudRate(m-1, n-1));

	if ( !sc->sc_inited ) { 

		if ( _System_state_Is_up(_System_state_Get()) ) {
			rtems_irq_connect_data ii = {
				name:	BSP_IRQ_I2C,
				hdl:	gt_i2c_intr,
				on:		0,
				off:	0,
				isOn:	0
			};
			rtems_status_code err;
			/* synchronization semaphore */
			err = rtems_semaphore_create(
					rtems_build_name('g','i','2','c'),
					0, 
					RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_LOCAL,
					0,
					&sc->sc_sync);
			if ( err ) {
				sc->sc_sync = 0;
				return err;
			}
			if ( !BSP_install_rtems_irq_handler(&ii) ) {
				fprintf(stderr,"Unable to install interrupt handler\n");
				rtems_semaphore_delete(sc->sc_sync);
				return RTEMS_INTERNAL_ERROR;
			}
			BSP_irq_set_priority(BSP_IRQ_I2C, BSP_IRQ_MIN_PRIO);
			sc->sc_inited = 1;
		} else {
		}
	} else {
		rtems_semaphore_flush(sc->sc_sync);
	}
	return RTEMS_SUCCESSFUL;
}

STATIC rtems_status_code
gt_i2c_send_start(rtems_libi2c_bus_t *bh)
{
struct gti2c_softc * const sc = &((gti2c_desc)bh)->pvt;

	return gt_i2c_wait(sc, I2C_Control_Start, I2C_Status_Started);
}

STATIC rtems_status_code
gt_i2c_send_stop(rtems_libi2c_bus_t *bh)
{
struct gti2c_softc * const sc = &((gti2c_desc)bh)->pvt;
uint32_t	data;

	data = gt_read(sc->sc_gt, I2C_REG_Status);
	if ( I2C_Status_Started == data || I2C_Status_ReStarted == data ) {
		/* According to the spec, a void message (start - stop sequence)
		 * is illegal and indeed, the chip plays bad tricks with us, i.e.,
		 * sometimes it hangs the bus so that it remains idle forever.
		 * so we have to address someone...
		 */
		gt_i2c_send_addr(bh, /*just something... */ 8, 1);
		data = gt_read(sc->sc_gt, I2C_REG_Status);
	}

	if ( I2C_Status_AddrReadAck == data ) {
		/* Another thing: spec says that the master generates stop only after
		 * not acknowledging the last byte. Again, the chip doesn't like
		 * to be stopped in this condition - hence we just do it the favor
		 * and read a single byte...
		 */
		gt_i2c_read_bytes(bh, (unsigned char *)&data, 1);
	}

	gt_write(sc->sc_gt, I2C_REG_Control, I2C_Control_Stop | sc->sc_cntl);

	/* should we poll for idle? There seems to be in IRQ when this completes */
	return RTEMS_SUCCESSFUL;
}

STATIC rtems_status_code
gt_i2c_send_addr(rtems_libi2c_bus_t *bh, uint32_t addr, int rw)
{
struct gti2c_softc * const sc = &((gti2c_desc)bh)->pvt;
uint32_t data, wanted_status;
uint8_t read_mask = rw ? 1 : 0;
rtems_status_code error;

	if (read_mask) {
		wanted_status = I2C_Status_AddrReadAck;
	} else {
		wanted_status = I2C_Status_AddrWriteAck;
	}
	/*
	 * First byte contains whether this xfer is a read or write.
	 */
	data = read_mask;
	if (addr > 0x7f) {
		/*
		 * If this is a 10bit request, the first address byte is
		 * 0b11110<b9><b8><r/w>.
		 */
		data |= 0xf0 | ((addr & 0x300) >> 7);
		gt_write(sc->sc_gt, I2C_REG_Data, data);
		error = gt_i2c_wait(sc, 0, wanted_status);
		if (error)
			return error;
		/*
		 * The first address byte has been sent, now to send
		 * the second one.
		 */
		if (read_mask) {
			wanted_status = I2C_Status_2ndAddrReadAck;
		} else {
			wanted_status = I2C_Status_2ndAddrWriteAck;
		}
		data = (uint8_t) addr;
	} else {
		data |= (addr << 1);
	}

	gt_write(sc->sc_gt, I2C_REG_Data, data);
	return gt_i2c_wait(sc, 0, wanted_status);
}

STATIC int
gt_i2c_read_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len)
{
struct gti2c_softc * const sc = &((gti2c_desc)bh)->pvt;
rtems_status_code error;
register unsigned char *p=buf;

	while ( len-- > 0 ) {
		error = gt_i2c_wait(
					sc,
					len ? I2C_Control_ACK : 0,
					len ? I2C_Status_MasterReadAck : I2C_Status_MasterReadNoAck);
		if ( error ) {
			return -error;
		}
		*p++ = gt_read(sc->sc_gt, I2C_REG_Data);
	}

	return p-buf;
}

STATIC int
gt_i2c_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len)
{
struct gti2c_softc * const sc = &((gti2c_desc)bh)->pvt;
int rval = 0;
rtems_status_code error;

	while ( len-- > 0 ) {
		gt_write(sc->sc_gt, I2C_REG_Data, buf[rval]);
		error = gt_i2c_wait(sc, 0, I2C_Status_MasterWriteAck);
		if ( error ) {
			return -error;
		}
		rval++;
	}

	return rval;
}

rtems_libi2c_bus_t *gt64260_i2c_bus_descriptor = &my_bus_tbl.bus_desc;

#ifdef DEBUG_MODULAR

void
_cexpModuleInitialize(void *arg)
{
	gt_i2c_init(&gt64260_i2c_bus_descriptor->bus_desc);
}

int
_cexpModuleFinalize(void * arg)
{
struct gti2c_softc * const sc = &gt64260_i2c_bus_descriptor->pvt;

	rtems_irq_connect_data ii = {
			name:	BSP_IRQ_I2C,
			hdl:	gt_i2c_intr,
			on:		noop,
			off:	noop,
			isOn:	inoop
	};

	rtems_semaphore_delete(sc->sc_sync);

	return !BSP_remove_rtems_irq_handler(&ii);
}

#endif
