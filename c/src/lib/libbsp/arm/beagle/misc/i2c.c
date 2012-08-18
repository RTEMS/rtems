/**
 * @file
 *
 * @ingroup beagle_i2c
 *
 * @brief I2C support implementation.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

#include <bsp.h>
#include <bsp/i2c.h>

static struct i2c *i2c_base = (struct i2c *)I2C_DEFAULT_BASE;

static unsigned short wait_for_pin( void ) {

	unsigned short status;
	int timeout = I2C_TIMEOUT;

	do {
		udelay( 1000 );
		status = readw( &i2c_base->stat );
	} while( !( status &
			  ( I2C_STAT_ROVR | I2C_STAT_XUDF | I2C_STAT_XRDY |
				I2C_STAT_RRDY | I2C_STAT_ARDY | I2C_STAT_NACK |
				I2C_STAT_AL ) ) && timeout-- );

	if( timeout <= 0 ) {
		printf( "timed out in wait_for_pin: I2C_STAT = %x\n", readw( &i2c_base->stat ) );
		writew( 0xFFFF, &i2c_base->stat );	/* clear current interrupts...*/
		status = 0;
	}

	return status;
}

static void wait_for_bb( void ) {

	int timeout = I2C_TIMEOUT;
	unsigned short status;

	writew( 0xFFFF, &i2c_base->stat );	/* clear current interrupts...*/
	while( ( status = readw( &i2c_base->stat ) & I2C_STAT_BB ) && timeout-- ) {
		writew( status, &i2c_base->stat );
		udelay( 1000 );
	}

	if( timeout <= 0 ) {
		printf( "timed out in wait_for_bb: I2C_STAT = %x\n", readw( &i2c_base->stat ) );
	}
	writew( 0xFFFF, &i2c_base->stat );	 /* clear delayed stuff*/
}

static void flush_fifo( void ) {

	unsigned short status;

	/* note: if you try and read data when its not there or ready
	 * you get a bus error
	 */
	while( 1 ) {
		status = readw( &i2c_base->stat );
		if( status == I2C_STAT_RRDY ) {
#if defined( CONFIG_OMAP243X ) || defined( CONFIG_OMAP34XX ) ||	defined( CONFIG_OMAP44XX )
			readb( &i2c_base->data );
#else
			readw( &i2c_base->data );
#endif
			writew( I2C_STAT_RRDY, &i2c_base->stat );
			udelay( 1000 );
		} else {
			break;
		}
	}
}

void i2c_init( int speed, int slaveadd ) {

	int psc, fsscll, fssclh;
	int hsscll = 0, hssclh = 0;
	unsigned int scll, sclh;
	int timeout = I2C_TIMEOUT;

	// Only handle standard, fast and high speeds
	if( ( speed != OMAP_I2C_STANDARD   ) &&
	    ( speed != OMAP_I2C_FAST_MODE  ) &&
	    ( speed != OMAP_I2C_HIGH_SPEED ) ) {
		printf( "Error : I2C unsupported speed %d\n", speed );
		return;
	}

	psc = I2C_IP_CLK / I2C_INTERNAL_SAMPLING_CLK;
	psc -= 1;
	if( psc < I2C_PSC_MIN ) {
		printf( "Error : I2C unsupported prescalar %d\n", psc );
		return;
	}

	if( speed == OMAP_I2C_HIGH_SPEED ) {
		// High speed

		// For first phase of HS mode
		fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * OMAP_I2C_FAST_MODE );

		fsscll -= I2C_HIGHSPEED_PHASE_ONE_SCLL_TRIM;
		fssclh -= I2C_HIGHSPEED_PHASE_ONE_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing first phase clock\n" );
			return;
		}

		// For second phase of HS mode
		hsscll = hssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

		hsscll -= I2C_HIGHSPEED_PHASE_TWO_SCLL_TRIM;
		hssclh -= I2C_HIGHSPEED_PHASE_TWO_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing second phase clock\n" );
			return;
		}

		scll = ( unsigned int ) hsscll << 8 | ( unsigned int ) fsscll;
		sclh = ( unsigned int ) hssclh << 8 | ( unsigned int ) fssclh;

	} else {
		// Standard and fast speed
		fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

		fsscll -= I2C_FASTSPEED_SCLL_TRIM;
		fssclh -= I2C_FASTSPEED_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing clock\n" );
			return;
		}

		scll = ( unsigned int ) fsscll;
		sclh = ( unsigned int ) fssclh;
	}

	if( readw( &i2c_base->con ) & I2C_CON_EN ) {
		writew( 0, &i2c_base->con );
		udelay( 50000  );
	}

	writew( 0x2, &i2c_base->sysc ); /* for ES2 after soft reset */
	udelay( 1000 );

	writew( I2C_CON_EN, &i2c_base->con );
	while( !( readw( &i2c_base->syss ) & I2C_SYSS_RDONE ) && timeout-- ) {
		if (timeout <= 0) {
			printf( "ERROR: Timeout in soft-reset\n" );
			return;
		}
		udelay( 1000 );
	}

	writew( 0, &i2c_base->con );
	writew( psc, &i2c_base->psc );
	writew( scll, &i2c_base->scll );
	writew( sclh, &i2c_base->sclh );

	/* own address */
	writew( slaveadd, &i2c_base->oa );
	writew( I2C_CON_EN, &i2c_base->con );

	/* have to enable intrrupts or OMAP i2c module doesn't work */
	writew( I2C_IE_XRDY_IE | I2C_IE_RRDY_IE | I2C_IE_ARDY_IE | I2C_IE_NACK_IE | I2C_IE_AL_IE, &i2c_base->ie );
	udelay( 1000 );
	flush_fifo();
	writew( 0xFFFF, &i2c_base->stat );
	writew( 0, &i2c_base->cnt );

	//if( gd->flags & GD_FLG_RELOC ) bus_initialized[ current_bus ] = 1;
}

static int i2c_read_byte( unsigned char devaddr, unsigned char regoffset, unsigned char *value)
{
	int i2c_error = 0;
	unsigned short status;

	/* wait until bus not busy */
	wait_for_bb();

	/* one byte only */
	writew(1, &i2c_base->cnt);
	/* set slave address */
	writew(devaddr, &i2c_base->sa);
	/* no stop bit needed here */
	writew(I2C_CON_EN | I2C_CON_MST | I2C_CON_STT |
	      I2C_CON_TRX, &i2c_base->con);

	/* send register offset */
	while (1) {
		status = wait_for_pin();
		if (status == 0 || status & I2C_STAT_NACK) {
			i2c_error = 1;
			goto read_exit;
		}
		if (status & I2C_STAT_XRDY) {
			/* Important: have to use byte access */
			writeb(regoffset, &i2c_base->data);
			writew(I2C_STAT_XRDY, &i2c_base->stat);
		}
		if (status & I2C_STAT_ARDY) {
			writew(I2C_STAT_ARDY, &i2c_base->stat);
			break;
		}
	}

	/* set slave address */
	writew(devaddr, &i2c_base->sa);
	/* read one byte from slave */
	writew(1, &i2c_base->cnt);
	/* need stop bit here */
	writew(I2C_CON_EN | I2C_CON_MST |
		I2C_CON_STT | I2C_CON_STP,
		&i2c_base->con);

	/* receive data */
	while (1) {
		status = wait_for_pin();
		if (status == 0 || status & I2C_STAT_NACK) {
			i2c_error = 1;
			goto read_exit;
		}
		if (status & I2C_STAT_RRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || \
	defined(CONFIG_OMAP44XX)
			*value = readb(&i2c_base->data);
#else
			*value = readw(&i2c_base->data);
#endif
			writew(I2C_STAT_RRDY, &i2c_base->stat);
		}
		if (status & I2C_STAT_ARDY) {
			writew(I2C_STAT_ARDY, &i2c_base->stat);
			break;
		}
	}

read_exit:
	flush_fifo();
	writew(0xFFFF, &i2c_base->stat);
	writew(0, &i2c_base->cnt);
	return i2c_error;
}

int i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	int i;
	unsigned short status;
	int i2c_error = 0;

	if (alen > 1) {
		printf("I2C write: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf("I2C write: address 0x%x + 0x%x out of range\n",
				addr, len);
		return 1;
	}

	/* wait until bus not busy */
	wait_for_bb();

	/* start address phase - will write regoffset + len bytes data */
	/* TODO consider case when !CONFIG_OMAP243X/34XX/44XX */
	writew(alen + len, &i2c_base->cnt);
	/* set slave address */
	writew(chip, &i2c_base->sa);
	/* stop bit needed here */
	writew(I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX |
		I2C_CON_STP, &i2c_base->con);

	/* Send address byte */
	status = wait_for_pin();

	if (status == 0 || status & I2C_STAT_NACK) {
		i2c_error = 1;
		printf("error waiting for i2c address ACK (status=0x%x)\n",
		      status);
		goto write_exit;
	}

	if (status & I2C_STAT_XRDY) {
		writeb(addr & 0xFF, &i2c_base->data);
		writew(I2C_STAT_XRDY, &i2c_base->stat);
	} else {
		i2c_error = 1;
		printf("i2c bus not ready for transmit (status=0x%x)\n",
		      status);
		goto write_exit;
	}

	/* address phase is over, now write data */
	for (i = 0; i < len; i++) {
		status = wait_for_pin();

		if (status == 0 || status & I2C_STAT_NACK) {
			i2c_error = 1;
			printf("i2c error waiting for data ACK (status=0x%x)\n",
					status);
			goto write_exit;
		}

		if (status & I2C_STAT_XRDY) {
			writeb(buffer[i], &i2c_base->data);
			writew(I2C_STAT_XRDY, &i2c_base->stat);
		} else {
			i2c_error = 1;
			printf("i2c bus not ready for Tx (i=%d)\n", i);
			goto write_exit;
		}
	}

write_exit:
	flush_fifo();
	writew(0xFFFF, &i2c_base->stat);
	return i2c_error;
}

int i2c_read(unsigned char chip, uint addr, int alen, unsigned char *buffer, int len)
{
	int i;

	if (alen > 1) {
		printf("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		puts("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_read_byte(chip, addr + i, &buffer[i])) {
			puts("I2C read: I/O error\n");
			i2c_init( CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE );
			return 1;
		}
	}

	return 0;
}

/* Write (fill) memory
 *
 * Syntax:
 *	i2c mw {i2c_chip} {addr}{.0, .1, .2} {data} [{count}]
 */
static int imw ( unsigned char	chip, unsigned long	addr, unsigned char	byte )
{

	unsigned int	alen;
	int	count;

	alen = 1;
	count = 1;

	while (count-- > 0) {
		if (i2c_write(chip, addr++, alen, &byte, 1) != 0)
			printf("Error writing the chip.\n");
		/*
		 * Wait for the write to complete.  The write can take
		 * up to 10mSec (we allow a little more time).
		 */
	}

	return (0);
}

/*
 * Syntax:
 *	i2c md {i2c_chip} {addr}{.0, .1, .2} {len}
 */
static int imd( unsigned char chip, unsigned int addr, unsigned int length )
{
	int	j, nbytes, linebytes;

	unsigned int alen = 0;
	if (alen > 3) return 0;

	/*
	 * Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once.
	 */
	nbytes = length;
	do {
		unsigned char	linebuf[DISP_LINE_LEN];
		unsigned char	*cp;

		linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

		if (i2c_read(chip, addr, alen, linebuf, linebytes) != 0)
			puts ("Error reading the chip.\n");
		else {
			printf("%04x:", addr);
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				printf(" %02x", *cp++);
				addr++;
			}
			puts ("    ");
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				if ((*cp < 0x20) || (*cp > 0x7e))
					puts (".");
				else
					printf("%c", *cp);
				cp++;
			}
			printf ("\n");
		}
		nbytes -= linebytes;
	} while (nbytes > 0);

	return 0;
}

/*
struct i2c_speed {
	unsigned short psc;		// 0x30
	unsigned short scll;	// 0x34
	unsigned short sclh;	// 0x38
};

void i2c_calc_speed( int setspeed, i2c_speed &out ) {
	i2c_speed spd;

	int psc, fsscll, fssclh;
	int hsscll = 0, hssclh = 0;
	unsigned int scll, sclh;
	int timeout = I2C_TIMEOUT;

	psc = I2C_IP_CLK / I2C_INTERNAL_SAMPLING_CLK;
	psc -= 1;
	if( psc < I2C_PSC_MIN ) {
		printf( "Error : I2C unsupported prescalar %d\n", psc );
		return;
	}

	if( speed == OMAP_I2C_HIGH_SPEED ) {
		// High speed
		// For first phase of HS mode
		fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * OMAP_I2C_FAST_MODE );

		fsscll -= I2C_HIGHSPEED_PHASE_ONE_SCLL_TRIM;
		fssclh -= I2C_HIGHSPEED_PHASE_ONE_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing first phase clock\n" );
			return;
		}
		// For second phase of HS mode
		hsscll = hssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

		hsscll -= I2C_HIGHSPEED_PHASE_TWO_SCLL_TRIM;
		hssclh -= I2C_HIGHSPEED_PHASE_TWO_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing second phase clock\n" );
			return;
		}

		scll = ( unsigned int ) hsscll << 8 | ( unsigned int ) fsscll;
		sclh = ( unsigned int ) hssclh << 8 | ( unsigned int ) fssclh;

	} else if( ( speed == OMAP_I2C_STANDARD ) || ( speed == OMAP_I2C_FAST_MODE  ) ) {
		// Standard and fast speed
		fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

		fsscll -= I2C_FASTSPEED_SCLL_TRIM;
		fssclh -= I2C_FASTSPEED_SCLH_TRIM;
		if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) || ( fssclh > 255 ) ) ) {
			printf( "Error : I2C initializing clock\n" );
			return;
		}

		scll = ( unsigned int ) fsscll;
		sclh = ( unsigned int ) fssclh;
	} else {
		// Only handle standard, fast and high speeds
		printf( "Error : I2C unsupported speed %d\n", speed );
		return;
	}

}
*/












/*




void beagle_i2c_reset(volatile beagle_i2c *i2c)
{
  i2c->ctrl = I2C_CTRL_RESET;
}

rtems_status_code beagle_i2c_init(
  volatile beagle_i2c *i2c,
  unsigned clock_in_hz
)
{
  uint32_t i2cclk = 0;

  //if (i2c == &beagle.i2c_1) {
    i2cclk |= I2CCLK_1_EN | I2CCLK_1_HIGH_DRIVE;
  //} else if (i2c == &beagle.i2c_2) {
  //  i2cclk |= I2CCLK_2_EN | I2CCLK_2_HIGH_DRIVE;
  //} else {
  //  return RTEMS_INVALID_ID;
  //}

  BEAGLE_I2CCLK_CTRL |= i2cclk;

  beagle_i2c_reset(i2c);

  return beagle_i2c_clock(i2c, clock_in_hz);
}

rtems_status_code beagle_i2c_clock(
  volatile beagle_i2c *i2c,
  unsigned clock_in_hz
)
{
  uint32_t clk_div = beagle_hclk() / clock_in_hz;
  uint32_t clk_lo = 0;
  uint32_t clk_hi = 0;

  switch (clock_in_hz) {
    case 100000:
      clk_lo = clk_div / 2;
      break;
    case 400000:
      clk_lo = (64 * clk_div) / 100;
      break;
    default:
      return RTEMS_INVALID_CLOCK;
  }

  clk_hi = clk_div - clk_lo;

  i2c->clk_lo = clk_lo;
  i2c->clk_hi = clk_hi;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code wait_for_transaction_done(volatile beagle_i2c *i2c)
{
  uint32_t stat = 0;

  do {
    stat = i2c->stat;
  } while ((stat & I2C_STAT_TDI) == 0);

  if ((stat & I2C_STAT_TFE) != 0) {
    i2c->stat = I2C_STAT_TDI;

    return RTEMS_SUCCESSFUL;
  } else {
    beagle_i2c_reset(i2c);

    return RTEMS_IO_ERROR;
  }
}

static rtems_status_code tx(volatile beagle_i2c *i2c, uint32_t data)
{
  uint32_t stat = 0;

  do {
    stat = i2c->stat;
  } while ((stat & (I2C_STAT_TFE | I2C_STAT_TDI)) == 0);

  if ((stat & I2C_STAT_TDI) == 0) {
    i2c->rx_or_tx = data;

    return RTEMS_SUCCESSFUL;
  } else {
    beagle_i2c_reset(i2c);

    return RTEMS_IO_ERROR;
  }
}

rtems_status_code beagle_i2c_write_start(
  volatile beagle_i2c *i2c,
  unsigned addr
)
{
  return tx(i2c, I2C_TX_ADDR(addr) | I2C_TX_START);
}

rtems_status_code beagle_i2c_read_start(
  volatile beagle_i2c *i2c,
  unsigned addr
)
{
  return tx(i2c, I2C_TX_ADDR(addr) | I2C_TX_START | I2C_TX_READ);
}

rtems_status_code beagle_i2c_write_with_optional_stop(
  volatile beagle_i2c *i2c,
  const uint8_t *out,
  size_t n,
  bool stop
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;

  for (i = 0; i < n - 1 && sc == RTEMS_SUCCESSFUL; ++i) {
    sc = tx(i2c, out [i]);
  }

  if (sc == RTEMS_SUCCESSFUL) {
    uint32_t stop_flag = stop ? I2C_TX_STOP : 0;

    sc = tx(i2c, out [n - 1] | stop_flag);
  }

  if (stop && sc == RTEMS_SUCCESSFUL) {
    sc = wait_for_transaction_done(i2c);
  }

  return sc;
}

static bool can_tx_for_rx(volatile beagle_i2c *i2c)
{
  return (i2c->stat & (I2C_STAT_TFF | I2C_STAT_RFF)) == 0;
}

static bool can_rx(volatile beagle_i2c *i2c)
{
  return (i2c->stat & I2C_STAT_RFE) == 0;
}

rtems_status_code beagle_i2c_read_with_optional_stop(
  volatile beagle_i2c *i2c,
  uint8_t *in,
  size_t n,
  bool stop
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t last = n - 1;
  size_t rx = 0;
  size_t tx = 0;

  if (!stop) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  while (rx <= last) {
    while (tx < last && can_tx_for_rx(i2c)) {
      i2c->rx_or_tx = 0;
      ++tx;
    }

    if (tx == last && can_tx_for_rx(i2c)) {
      uint32_t stop_flag = stop ? I2C_TX_STOP : 0;

      i2c->rx_or_tx = stop_flag;
      ++tx;
    }

    while (rx <= last && can_rx(i2c)) {
      in [rx] = (uint8_t) i2c->rx_or_tx;
      ++rx;
    }
  }

  if (stop) {
    sc = wait_for_transaction_done(i2c);
  }

  return sc;
}

rtems_status_code beagle_i2c_write_and_read(
  volatile beagle_i2c *i2c,
  unsigned addr,
  const uint8_t *out,
  size_t out_size,
  uint8_t *in,
  size_t in_size
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (out_size > 0) {
    bool stop = in_size == 0;

    sc = beagle_i2c_write_start(i2c, addr);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }

    sc = beagle_i2c_write_with_optional_stop(i2c, out, out_size, stop);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  if (in_size > 0) {
    sc = beagle_i2c_read_start(i2c, addr);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }

    beagle_i2c_read_with_optional_stop(i2c, in, in_size, true);
  }

  return RTEMS_SUCCESSFUL;
}
*/
