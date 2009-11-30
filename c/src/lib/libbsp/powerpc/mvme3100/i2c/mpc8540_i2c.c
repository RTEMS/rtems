/* I2C bus driver for mpc8540-based boards */

/*
 * Authorship
 * ----------
 * This software ('mvme3100' RTEMS BSP) was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mvme3100' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

/* Note: We maintain base address, IRQ etc. statically and
 *       globally. We don't bother creating driver-specific
 *       data or using the bus handle but simply assume
 *       this is the only 8540/i2c bus in the system.
 *       Proper support for multiple instances would not
 *       be very hard to add but I don't see the point...
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/libi2c.h>
#include <bsp/irq.h>
#include <libcpu/spr.h>
#include <libcpu/io.h>
#include <rtems/bspIo.h>

#include "mpc8540_i2c_busdrv.h"

#define STATIC

/* I2C controller register definitions */
#define	I2CADR				0x3000
#define	I2CFDR				0x3004
#define	I2CCR				0x3008
#define	I2CCR_MEN				(1<<(7-0))
#define	I2CCR_MIEN				(1<<(7-1))
#define	I2CCR_MSTA				(1<<(7-2))
#define	I2CCR_MTX				(1<<(7-3))
#define	I2CCR_TXAK				(1<<(7-4))
#define	I2CCR_RSTA				(1<<(7-5))
#define	I2CCR_BCST				(1<<(7-7))
#define	I2CSR				0x300c
#define	I2CSR_MCF				(1<<(7-0))
#define	I2CSR_MAAS				(1<<(7-1))
#define	I2CSR_MBB				(1<<(7-2))
#define	I2CSR_MAL				(1<<(7-3))
#define	I2CSR_BCSTM				(1<<(7-4))
#define	I2CSR_SRW				(1<<(7-5))
#define	I2CSR_MIF				(1<<(7-6))
#define	I2CSR_RXAK				(1<<(7-7))
#define	I2CDR				0x3010
#define	I2CDFSRR			0x3014

SPR_RO(TBRL)

/********* Global Variables **********/

/*
 * Semaphore for synchronizing accessing task
 * with the (slow) hardware operation.
 * Task takes semaphore and blocks, ISR releases.
 */
static rtems_id syncsem = 0;

static inline int ok_to_block(void)
{
	return syncsem && _System_state_Is_up( _System_state_Get() );
}

/*
 * Wild guess for 0.2 s; this timeout is effective
 * in polling mode; during early init we don't know
 * the system clock rate yet - it's one of the things
 * we have to read from VPD -- via i2c.
 */

static uint32_t poll_timeout = 333333333/8/5;

/********* Primitives ****************/

static inline uint8_t
i2c_rd(unsigned reg)
{
	return in_8( (volatile uint8_t *)(BSP_8540_CCSR_BASE + reg) );
}

static inline void
i2c_wr(unsigned reg, uint8_t val)
{
	out_8( (volatile uint8_t *)(BSP_8540_CCSR_BASE + reg), val );
}

static inline void
i2c_set(unsigned reg, uint8_t val)
{
	i2c_wr( reg, i2c_rd( reg ) | val );
}

static inline void
i2c_clr(unsigned reg, uint8_t val)
{
	i2c_wr( reg, i2c_rd( reg ) & ~val );
}

/********* Helper Routines ***********/

/* Synchronize (wait) for a condition on the
 * i2c bus. Wait for START or STOP to be complete
 * or wait for a byte-transfer.
 * The latter is much slower (9 bit times vs. 1/2
 * in the former cases).
 *
 * If the system is up (and we may block) then
 * this routine attempts to block the current
 * task rather than busy-waiting.
 *
 * NOTE: waiting for START/STOP always requires
 *       polling.
 */

/* wait until i2c status reg AND mask == cond */
static rtems_status_code
i2c_wait( uint8_t msk, uint8_t cond )
{
uint32_t	      then;
rtems_status_code sc;
static int        warn = 0;

	if ( I2CSR_MIF == msk && ok_to_block() ) {
		/* block on semaphore only if system is up and sema initialized */
		sc = rtems_semaphore_obtain( syncsem, RTEMS_WAIT, 100 );
		if ( RTEMS_SUCCESSFUL != sc )
			return sc;
	} else {
		/* system not up (no SEMA yet ) or waiting on something other
		 * than MIF
		 */
		if ( I2CSR_MIF == msk && _System_state_Is_up( _System_state_Get() ) ) {
			if ( warn < 8 || ! (warn & 0x1f) )
				printk("WARNING: i2c bus driver running in polled mode -- should initialize properly!\n");
			warn++;
		}

		then = _read_TBRL();
		do {
			/* poll for .2 seconds */
			if ( (_read_TBRL() - then) > poll_timeout )
				return RTEMS_TIMEOUT;
		} while ( (msk & i2c_rd( I2CSR )) != cond );
	}

	return RTEMS_SUCCESSFUL;
}

/*
 * multi-byte transfer
 * - set transfer direction (master read or master write)
 * - transfer byte
 * - wait/synchronize
 * - check for ACK
 *
 * RETURNS: number of bytes transferred or negative error code.
 */

STATIC int
i2c_xfer(int rw, uint8_t *buf, int len)
{
int               i;
rtems_status_code sc;

	if ( rw ) {
		i2c_clr( I2CCR, I2CCR_MTX );
	} else {
		i2c_set( I2CCR, I2CCR_MTX );
	}

	for ( i = 0; i< len; i++ ) {
		i2c_clr( I2CSR, I2CSR_MIF );
		/* Enable interrupts if necessary */
		if ( ok_to_block() )
			i2c_set( I2CCR, I2CCR_MIEN );
		if ( rw ) {
			buf[i] = i2c_rd( I2CDR );
		} else {
			i2c_wr( I2CDR, buf[i] );
		}
		if ( RTEMS_SUCCESSFUL != (sc = i2c_wait( I2CSR_MIF, I2CSR_MIF )) )
			return -sc;
		if ( (I2CSR_RXAK & i2c_rd( I2CSR )) ) {
			/* NO ACK */
			return -RTEMS_IO_ERROR;
		}
	}

	return i;
}

/*
 * This bus controller gives us lagging data, i.e.,
 * when we read a byte from the data reg then that
 * issues a read cycle on the bus and gives us the
 * byte from the *previous* read cycle :-(
 *
 * This makes it impossible to properly terminate
 * a read transaction w/o knowing ahead of time
 * how many bytes are going to be read (API decouples
 * 'START'/'STOP' from 'READ') since we would have to
 * set TXAK when reading the next-to-last byte
 * (i.e., when the last byte is read on the i2c bus).
 *
 * Hence, (if we are reading) we must do a dummy
 * read-cycle here -- hopefully
 * that has no side-effects! (i.e., EEPROM drivers should
 * reposition file pointers after issuing STOP)
 *
 */

static void
rd1byte_noack(void)
{
uint8_t dum;
uint8_t ccr;

	/* If we are in reading state then read one more
	 * byte w/o acknowledge
	 */

	ccr = i2c_rd (I2CCR );

	if ( ! ( I2CCR_MTX & ccr ) ) {
		i2c_wr( I2CCR, ccr | I2CCR_TXAK );
		i2c_xfer(1, &dum, 1);
		/* restore original TXAK bit setting */
		i2c_clr( I2CCR, (I2CCR_TXAK & ccr) );
	}
}


/********* ISR ***********************/

static void i2c_isr(rtems_irq_hdl_param arg)
{
	/* disable irq */
	i2c_clr( I2CCR, I2CCR_MIEN );
	/* release task */
	rtems_semaphore_release( syncsem );
}

/********* IIC Bus Driver Ops ********/

STATIC rtems_status_code
i2c_init(rtems_libi2c_bus_t *bh)
{
rtems_status_code sc;

	/* compute more accurate timeout */
	if ( BSP_bus_frequency && BSP_time_base_divisor )
		poll_timeout = BSP_bus_frequency/BSP_time_base_divisor*1000/5;

	i2c_clr( I2CCR, I2CCR_MEN );
	i2c_set( I2CCR, I2CCR_MEN );

	i2c_wr( I2CADR, 0 );

	/* leave motload settings for divisor and filter registers */

	if ( SYSTEM_STATE_BEFORE_MULTITASKING <= _System_state_Get() && !syncsem ) {
		sc = rtems_semaphore_create(
				rtems_build_name('i','2','c','b'),
				0,
				RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_LOCAL,
				0,
				&syncsem);
		if ( RTEMS_SUCCESSFUL == sc ) {
			rtems_irq_connect_data xxx;
			xxx.name   = BSP_I2C_IRQ;
			xxx.on     = 0;
			xxx.off    = 0;
			xxx.isOn   = 0;
			xxx.hdl    = i2c_isr;
			xxx.handle = 0;
			if ( ! BSP_install_rtems_irq_handler( &xxx ) ) {
				printk("Unable to install i2c ISR -- falling back to polling mode\n");
				rtems_semaphore_delete( syncsem );
				/* fall back to polling mode */
				syncsem = 0;
			}
		} else {
			syncsem = 0;
		}
	}

	return RTEMS_SUCCESSFUL;
}

STATIC rtems_status_code
i2c_start(rtems_libi2c_bus_t *bh)
{
uint8_t           v;
rtems_status_code sc = RTEMS_SUCCESSFUL;

	v = i2c_rd( I2CCR );
	if ( I2CCR_MSTA & v ) {
		/* RESTART */
		rd1byte_noack();
		v |= I2CCR_RSTA;
	} else {
		v |= I2CCR_MSTA;
	}
	i2c_wr( I2CCR, v );

	/* On MBB we can only poll-wait (no IRQ is generated)
	 * and this is also much faster than reading a byte
	 * (1/2-bit time) so the overhead of an IRQ may not
	 * be justified.
	 * OTOH, we can put this off into the 'send_addr' routine
	 *

	sc = i2c_wait( I2CSR_MBB, I2CSR_MBB );
	 */

	return sc;
}

STATIC rtems_status_code
i2c_stop(rtems_libi2c_bus_t *bh)
{
	rd1byte_noack();

	/* STOP */
	i2c_clr( I2CCR, I2CCR_TXAK | I2CCR_MSTA );

	/* FIXME: should we really spend 1/2 bit-time polling
	 *        or should we just go ahead and hope noone
	 *        else will get a chance to do something to
	 *        the bus until the STOP completes?
	 */
	return i2c_wait( I2CSR_MBB, 0 );
}

STATIC rtems_status_code
i2c_send_addr(rtems_libi2c_bus_t *bh, uint32_t addr, int rw)
{
uint8_t           buf[2];
int               l = 0;
uint8_t           read_mask = rw ? 1 : 0;
rtems_status_code sc;

	/* Make sure we are started; (i2c_start() didn't bother to wait
	 * so we do it here - some time already has expired.
	 */
	sc = i2c_wait( I2CSR_MBB, I2CSR_MBB );

	if ( RTEMS_SUCCESSFUL != sc )
		return sc;

	if ( addr > 0x7f ) {
		/* 10-bit request; 1st address byte is 0b11110<b9><b8><r/w> */
		buf[l] = 0xf0 | ((addr >> 7) & 0x06) | read_mask;
		read_mask = 0;
		l++;
		buf[l] = addr & 0xff;
	} else {
		buf[l] = (addr << 1) | read_mask;
		l++;
	}

	/*
	 * After sending a an address for reading we must
	 * read a dummy byte (this actually clocks the first real
	 * byte on the i2c bus and makes it available in the
	 * data register so that the first 'read_bytes' operation
	 * obtains the byte we clock in here [and starts clocking
	 * the second byte]) to overcome the pipeline
	 * delay in the hardware (I don't like this design) :-(.
	 */
	sc = i2c_xfer( 0, buf, l );
	if ( rw && l == sc ) {
		sc = i2c_xfer( 1, buf, 1 );
	}
	return sc >=0 ? RTEMS_SUCCESSFUL : -sc;
}

STATIC int
i2c_read_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len)
{
	return i2c_xfer( 1, buf, len );
}

STATIC int
i2c_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf, int len)
{
	return i2c_xfer( 0, buf, len );
}

/********* Driver Glue Vars **********/

static rtems_libi2c_bus_ops_t  myops = {
	init:            i2c_init,
	send_start:      i2c_start,
	send_stop:       i2c_stop,
	send_addr:       i2c_send_addr,
	read_bytes:      i2c_read_bytes,
	write_bytes:     i2c_write_bytes,
};

static rtems_libi2c_bus_t my_bus_tbl = {
	ops:	&myops,
	size:   sizeof(my_bus_tbl),
};

/********* Global Driver Handle ******/

rtems_libi2c_bus_t *mpc8540_i2c_bus_descriptor = &my_bus_tbl;
