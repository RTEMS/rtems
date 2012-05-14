/*
 * /dev/sci[0|1] for Hitachi SH 703X
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1999, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/iosupp.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/ispsh7032.h>
#include <rtems/score/iosh7032.h>
#include <sh/sh7_sci.h>
#include <sh/sh7_pfc.h>
#include <sh/sci.h>

/*
 * NOTE: Some SH variants have 3 sci devices
 */

#define SCI_MINOR_DEVICES       2

#define SH_SCI_BASE_0   SCI0_SMR
#define SH_SCI_BASE_1   SCI1_SMR

struct scidev_t {
  char *			name ;
  uint32_t  			addr ;
  rtems_device_minor_number	minor ;
  unsigned short		opened ;
  tcflag_t			cflags ;
} sci_device[SCI_MINOR_DEVICES] =
{
  { "/dev/sci0", SH_SCI_BASE_0, 0, 0, B9600 | CS8 },
  { "/dev/sci1", SH_SCI_BASE_1, 1, 0, B9600 | CS8 }
} ;

/*  imported from scitab.rel */
extern int _sci_get_brparms(
  tcflag_t      cflag,
  unsigned char *smr,
  unsigned char *brr );

/* Translate termios' tcflag_t into sci settings */
static int _sci_set_cflags(
  struct scidev_t      *sci_dev,
  tcflag_t      c_cflag )
{
  uint8_t  	smr ;
  uint8_t  	brr ;

  if ( c_cflag & CBAUD )
  {
    if ( _sci_get_brparms( c_cflag, &smr, &brr ) != 0 )
      return -1 ;
  }

  if ( c_cflag & CSIZE )
  {
    if ( c_cflag & CS8 )
      smr &= ~SCI_SEVEN_BIT_DATA;
    else if ( c_cflag & CS7 )
      smr |= SCI_SEVEN_BIT_DATA;
    else
      return -1 ;
  }

  if ( c_cflag & CSTOPB )
    smr |= SCI_STOP_BITS_2;
  else
    smr &= ~SCI_STOP_BITS_2;

  if ( c_cflag & PARENB )
    smr |= SCI_PARITY_ON ;
  else
    smr &= ~SCI_PARITY_ON ;

  if ( c_cflag & PARODD )
    smr |= SCI_ODD_PARITY ;
  else
    smr &= ~SCI_ODD_PARITY;

  write8( smr, sci_dev->addr + SCI_SMR );
  write8( brr, sci_dev->addr + SCI_BRR );

  return 0 ;
}

static void _sci_init(
  rtems_device_minor_number minor )
{
  uint16_t  	temp16 ;

  /* Pin function controller initialisation for asynchronous mode */
  if( minor == 0)
    {
      temp16 = read16( PFC_PBCR1);
      temp16 &= ~( PB8MD | PB9MD );
      temp16 |= (PB_TXD0 | PB_RXD0);
      write16( temp16, PFC_PBCR1);
    }
  else
    {
      temp16 = read16( PFC_PBCR1);
      temp16 &= ~( PB10MD | PB11MD);
      temp16 |= (PB_TXD1 | PB_RXD1);
      write16( temp16, PFC_PBCR1);
    }

  /* disable sck-pin */
  if( minor == 0)
  {
	  temp16 = read16( PFC_PBCR1);
	  temp16 &= ~(PB12MD);
	  write16( temp16, PFC_PBCR1);
  }
  else
  {
	  temp16 = read16( PFC_PBCR1);
	  temp16 &= ~(PB13MD);
	  write16( temp16, PFC_PBCR1);
  }
}

static void _sci_tx_polled(
  int minor,
  const char buf )
{
  struct scidev_t *scidev = &sci_device[minor] ;
  int8_t           ssr ;

  while ( !inb((scidev->addr + SCI_SSR) & SCI_TDRE ))
      ;
  write8(buf,scidev->addr+SCI_TDR);

  ssr = inb(scidev->addr+SCI_SSR);
  ssr &= ~SCI_TDRE ;
  write8(ssr,scidev->addr+SCI_SSR);
}

static int _sci_rx_polled (
  int minor)
{
  struct scidev_t *scidev = &sci_device[minor] ;

  unsigned char c;
  char ssr ;
  ssr = read8(scidev->addr + SCI_SSR) ;

  if (ssr & (SCI_PER | SCI_FER | SCI_ORER))
    write8(ssr & ~(SCI_PER | SCI_FER | SCI_ORER), scidev->addr+SCI_SSR);

  if ( !(ssr & SCI_RDRF) )
    return -1;

  c = read8(scidev->addr + SCI_RDR) ;

  write8(ssr & ~SCI_RDRF,scidev->addr + SCI_SSR);
  return c;
}

/*
 * sci_initialize
 */

rtems_device_driver sh_sci_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg )
{
  rtems_device_driver status ;
  rtems_device_minor_number     i;

  /*
   * register all possible devices.
   * the initialization of the hardware is done by sci_open
   */

  for ( i = 0 ; i < SCI_MINOR_DEVICES ; i++ )
  {
    status = rtems_io_register_name(
      sci_device[i].name,
      major,
      sci_device[i].minor );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }

  /* default hardware setup */

  return RTEMS_SUCCESSFUL;
}


/*
 *  Open entry point
 */

rtems_device_driver sh_sci_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg )
{
  uint8_t   temp8;

 /* check for valid minor number */
   if(( minor > ( SCI_MINOR_DEVICES -1 )) || ( minor < 0 ))
   {
     return RTEMS_INVALID_NUMBER;
   }

 /* device already opened */
  if ( sci_device[minor].opened > 0 )
  {
    sci_device[minor].opened++ ;
    return RTEMS_SUCCESSFUL ;
  }

  _sci_init( minor );

  if (minor == 0) {
    temp8 = read8(sci_device[minor].addr + SCI_SCR);
    temp8 &= ~(SCI_TE | SCI_RE) ;
    write8(temp8, sci_device[minor].addr + SCI_SCR);	/* Clear SCR */
    _sci_set_cflags( &sci_device[minor], sci_device[minor].cflags );

/* FIXME: Should be one bit delay */
    CPU_delay(50000); /* microseconds */

    temp8 |= SCI_RE | SCI_TE;
    write8(temp8, sci_device[minor].addr + SCI_SCR);	/* Enable clock output */
  } else {
    temp8 = read8(sci_device[minor].addr + SCI_SCR);
    temp8 &= ~(SCI_TE | SCI_RE) ;
    write8(temp8, sci_device[minor].addr + SCI_SCR);	/* Clear SCR */
    _sci_set_cflags( &sci_device[minor], sci_device[minor].cflags );

/* FIXME: Should be one bit delay */
    CPU_delay(50000); /* microseconds */

    temp8 |= SCI_RE | SCI_TE;
    write8(temp8, sci_device[minor].addr + SCI_SCR);	/* Enable clock output */
  }

  sci_device[minor].opened++ ;

  return RTEMS_SUCCESSFUL ;
}

/*
 *  Close entry point
 */

rtems_device_driver sh_sci_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  if( sci_device[minor].opened == 0 )
    {
      return RTEMS_INVALID_NUMBER;
    }

  sci_device[minor].opened-- ;

  return RTEMS_SUCCESSFUL ;
}

/*
 * read bytes from the serial port.
 */

rtems_device_driver sh_sci_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count = 0;

  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  char * buffer = rw_args->buffer;
  int maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = _sci_rx_polled(minor);
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      break;
    }
  }

  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

/*
 * write bytes to the serial port.
 */

rtems_device_driver sh_sci_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;

  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  char *buffer = rw_args->buffer;
  int maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
#if 0
    if ( buffer[ count ] == '\n') {
      outbyte(minor, '\r');
    }
#endif
    _sci_tx_polled( minor, buffer[ count ] );
  }

  rw_args->bytes_moved = maximum;
  return 0;
}

/*
 *  IO Control entry point
 */

rtems_device_driver sh_sci_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  /* Not yet supported */
  return RTEMS_SUCCESSFUL ;
}
