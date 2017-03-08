/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief I2C support implementation.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>

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
    printk( "timed out in wait_for_pin: I2C_STAT = %x\n",
      readw( &i2c_base->stat ) );
    writew( 0xFFFF, &i2c_base->stat );  /* clear current interrupts...*/
    status = 0;
  }

  return status;
}

static void wait_for_bb( void ) {

  int timeout = I2C_TIMEOUT;
  unsigned short status;

  writew( 0xFFFF, &i2c_base->stat );  /* clear current interrupts...*/
  while( ( status = readw( &i2c_base->stat ) & I2C_STAT_BB ) && timeout-- ) {
    writew( status, &i2c_base->stat );
    udelay( 1000 );
  }

  if( timeout <= 0 ) {
    printk( "timed out in wait_for_bb: I2C_STAT = %x\n",
      readw( &i2c_base->stat ) );
  }
  writew( 0xFFFF, &i2c_base->stat );   /* clear delayed stuff*/
}

static void flush_fifo( void ) {

  unsigned short status;

  /* note: if you try and read data when its not there or ready
   * you get a bus error
   */
  while( 1 ) {
    status = readw( &i2c_base->stat );
    if( status == I2C_STAT_RRDY ) {
      readw( &i2c_base->data );
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
    printk( "Error : I2C unsupported speed %d\n", speed );
    return;
  }

  psc = I2C_IP_CLK / I2C_INTERNAL_SAMPLING_CLK;
  psc -= 1;
  if( psc < I2C_PSC_MIN ) {
    printk( "Error : I2C unsupported prescalar %d\n", psc );
    return;
  }

  if( speed == OMAP_I2C_HIGH_SPEED ) {
    // High speed

    // For first phase of HS mode
    fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * OMAP_I2C_FAST_MODE );

    fsscll -= I2C_HIGHSPEED_PHASE_ONE_SCLL_TRIM;
    fssclh -= I2C_HIGHSPEED_PHASE_ONE_SCLH_TRIM;
    if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) ||
      ( fssclh > 255 ) ) ) {
      printk( "Error : I2C initializing first phase clock\n" );
      return;
    }

    // For second phase of HS mode
    hsscll = hssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

    hsscll -= I2C_HIGHSPEED_PHASE_TWO_SCLL_TRIM;
    hssclh -= I2C_HIGHSPEED_PHASE_TWO_SCLH_TRIM;
    if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) ||
      ( fssclh > 255 ) ) ) {
      printk( "Error : I2C initializing second phase clock\n" );
      return;
    }

    scll = ( unsigned int ) hsscll << 8 | ( unsigned int ) fsscll;
    sclh = ( unsigned int ) hssclh << 8 | ( unsigned int ) fssclh;

  } else {
    // Standard and fast speed
    fsscll = fssclh = I2C_INTERNAL_SAMPLING_CLK / ( 2 * speed );

    fsscll -= I2C_FASTSPEED_SCLL_TRIM;
    fssclh -= I2C_FASTSPEED_SCLH_TRIM;
    if( ( ( fsscll < 0 ) || ( fssclh < 0 ) ) || ( ( fsscll > 255 ) ||
      ( fssclh > 255 ) ) ) {
      printk( "Error : I2C initializing clock\n" );
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
      printk( "ERROR: Timeout in soft-reset\n" );
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
  writew( I2C_IE_XRDY_IE | I2C_IE_RRDY_IE | I2C_IE_ARDY_IE | I2C_IE_NACK_IE |
          I2C_IE_AL_IE, &i2c_base->ie );
  udelay( 1000 );
  flush_fifo();
  writew( 0xFFFF, &i2c_base->stat );
  writew( 0, &i2c_base->cnt );

  //if( gd->flags & GD_FLG_RELOC ) bus_initialized[ current_bus ] = 1;
}

static int i2c_read_byte(
  unsigned char devaddr,
  unsigned char regoffset,
  unsigned char *value
)
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
      *value = readw(&i2c_base->data);
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

int i2c_write(
  unsigned char chip,
  unsigned int addr,
  int alen,
  unsigned char *buffer,
  int len
)
{
  int i;
  unsigned short status;
  int i2c_error = 0;

  if (alen > 1) {
    printk("I2C write: addr len %d not supported\n", alen);
    return 1;
  }

  if (addr + len > 256) {
    printk("I2C write: address 0x%x + 0x%x out of range\n",
        addr, len);
    return 1;
  }

  /* wait until bus not busy */
  wait_for_bb();

  /* start address phase - will write regoffset + len bytes data */
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
    printk("error waiting for i2c address ACK (status=0x%x)\n",
          status);
    goto write_exit;
  }

  if (status & I2C_STAT_XRDY) {
    writeb(addr & 0xFF, &i2c_base->data);
    writew(I2C_STAT_XRDY, &i2c_base->stat);
  } else {
    i2c_error = 1;
    printk("i2c bus not ready for transmit (status=0x%x)\n",
          status);
    goto write_exit;
  }

  /* address phase is over, now write data */
  for (i = 0; i < len; i++) {
    status = wait_for_pin();

    if (status == 0 || status & I2C_STAT_NACK) {
      i2c_error = 1;
      printk("i2c error waiting for data ACK (status=0x%x)\n",
          status);
      goto write_exit;
    }

    if (status & I2C_STAT_XRDY) {
      writeb(buffer[i], &i2c_base->data);
      writew(I2C_STAT_XRDY, &i2c_base->stat);
    } else {
      i2c_error = 1;
      printk("i2c bus not ready for Tx (i=%d)\n", i);
      goto write_exit;
    }
  }

write_exit:
  flush_fifo();
  writew(0xFFFF, &i2c_base->stat);
  return i2c_error;
}

int i2c_read(
  unsigned char chip,
  uint addr,
  int alen,
  unsigned char *buffer,
  int len
)
{
  int i;

  if (alen > 1) {
    printk("I2C read: addr len %d not supported\n", alen);
    return 1;
  }

  if (addr + len > 256) {
    printk("I2C read: address out of range\n");
    return 1;
  }

  for (i = 0; i < len; i++) {
    if (i2c_read_byte(chip, addr + i, &buffer[i])) {
      printk("I2C read: I/O error\n");
      i2c_init( CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE );
      return 1;
    }
  }

  return 0;
}

/* Write (fill) memory
 *
 * Syntax:
 *  i2c mw {i2c_chip} {addr}{.0, .1, .2} {data} [{count}]
 */
static int imw ( unsigned char  chip, unsigned long addr, unsigned char byte )
{

  unsigned int  alen;
  int count;

  alen = 1;
  count = 1;

  while (count-- > 0) {
    if (i2c_write(chip, addr++, alen, &byte, 1) != 0)
      printk("Error writing the chip.\n");
    /*
     * Wait for the write to complete.  The write can take
     * up to 10mSec (we allow a little more time).
     */
  }

  return (0);
}

/*
 * Syntax:
 *  i2c md {i2c_chip} {addr}{.0, .1, .2} {len}
 */
static int imd( unsigned char chip, unsigned int addr, unsigned int length )
{
  int j, nbytes, linebytes;

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
    unsigned char linebuf[DISP_LINE_LEN];
    unsigned char *cp;

    linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

    if (i2c_read(chip, addr, alen, linebuf, linebytes) != 0)
      printk ("Error reading the chip.\n");
    else {
      printk("%04x:", addr);
      cp = linebuf;
      for (j=0; j<linebytes; j++) {
        printk(" %02x", *cp++);
        addr++;
      }
      printk ("    ");
      cp = linebuf;
      for (j=0; j<linebytes; j++) {
        if ((*cp < 0x20) || (*cp > 0x7e))
          printk (".");
        else
          printk("%c", *cp);
        cp++;
      }
      printk ("\n");
    }
    nbytes -= linebytes;
  } while (nbytes > 0);

  return 0;
}
