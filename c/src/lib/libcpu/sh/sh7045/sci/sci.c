/*
 * /dev/sci[0|1] for Hitachi SH 704X
 *
 * The SH doesn't have a designated console device. Therefore we "alias" 
 * another device as /dev/console and revector all calls to /dev/console 
 * to this device.
 *
 * This approach is similar to installing a sym-link from one device to 
 * another device. If rtems once will support sym-links for devices files, 
 * this implementation could be dropped.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modified to reflect sh7045 processor:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 *
 *  $Id$
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <iosupp.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>
#include <sh/sh7_sci.h>
#include <sh/io_types.h>
#include <sh/sci.h>

struct scidev_t {
  char *			name ;
  rtems_device_minor_number	minor ;
  unsigned short		opened ;
  tcflag_t			cflags ;
} sci_device[2] =
{
  { "/dev/sci0", 0, 0, B9600 | CS8 },
  { "/dev/sci1", 1, 0, B9600 | CS8 }
} ;

/*  local data structures maintain hardware configuration */
extern int _sci_get_brparms(
  tcflag_t      cflag,
  unsigned char *smr,
  unsigned char *brr );

#if UNUSED
static sci_setup_t sio_param[2];
#endif

/* local functions operate SCI ports 0 and 1 */
/* called from polling routines or ISRs */
rtems_boolean wrtSCI0(unsigned char ch)
{
  unsigned8 temp;
  rtems_boolean result=FALSE;

  if ((read8(SCI_SSR0) & SCI_TDRE) != 0x00) {
    /* Write the character to the TDR */
    write8(ch, SCI_TDR0);
    /* Clear the TDRE bit */
    temp = read8(SCI_SSR0) & ~SCI_TDRE;
    write8(temp, SCI_SSR0);
    result = TRUE;
  }
  return result;
} /* wrtSCI0 */

rtems_boolean wrtSCI1(unsigned char ch)
{
  unsigned8 temp;
  rtems_boolean result=FALSE;

  if ((read8(SCI_SSR1) & SCI_TDRE) != 0x00) {
     /* Write the character to the TDR */
     write8(ch, SCI_TDR1);
     /* Clear the TDRE bit */
     temp = read8(SCI_SSR1) & ~SCI_TDRE;
     write8(temp, SCI_SSR1);
     result = TRUE;
  }
  return result;
} /* wrtSCI1 */

/* polled output steers byte to selected port */
void sh_sci_outbyte_polled(
  rtems_device_minor_number  minor,
  char ch )
{
	if (minor == 0) /* blocks until port ready */
		while (wrtSCI0(ch) != TRUE); /* SCI0*/
	else
		while (wrtSCI1(ch) != TRUE); /* SCI1*/
} /* sh_sci_outbyte_polled */

/* Initial version calls polled output driver and blocks */
void outbyte(
  rtems_device_minor_number  minor,
  char ch)
{
	sh_sci_outbyte_polled(minor, (unsigned char)ch);
} /* outbyte */

rtems_boolean rdSCI0(unsigned char *ch)
{
  unsigned8 temp;
  rtems_boolean result=FALSE;

  if ((read8(SCI_SSR0) & SCI_RDRF) != 0x00) {
    /* Write the character to the TDR */
    write8(*ch, SCI_RDR0);
    /* Clear the TDRE bit */
    temp = read8(SCI_SSR0) & ~SCI_RDRF;
    write8(temp, SCI_SSR0);
    result = TRUE;
  }
  return result;
} /* rdSCI0 */

rtems_boolean rdSCI1(unsigned char *ch)
{
  unsigned8 temp;
  rtems_boolean result=FALSE;

  if ((read8(SCI_SSR1) & SCI_RDRF) != 0x00) {
    /* Write the character to the TDR */
    write8(*ch, SCI_RDR1);
    /* Clear the TDRE bit */
    temp= read8(SCI_SSR1) & ~SCI_RDRF;
    write8(temp, SCI_SSR1);
    result = TRUE;
    }
  return result;
} /* rdSCI1 */


/* initial version pulls byte from selected port */
char sh_sci_inbyte_polled(
    rtems_device_minor_number  minor )
{
	char ch;
	
	if (minor == 0) /* blocks until char.ready */
		while (rdSCI0(&ch) != TRUE); /* SCI0 */
	else
		while (rdSCI1(&ch) != TRUE); /* SCI1 */
	return ch;
} /* sh_sci_inbyte_polled */

/* Initial version calls polled input driver */
char inbyte(
  rtems_device_minor_number  minor )
{
	char ch;

	ch = sh_sci_inbyte_polled(minor);
	return ch;
} /* inbyte */


/*  sh_sci_initialize
 *
 *  This routine initializes the sh_sci IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver sh_sci_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg )
{
  int a;
  unsigned16 temp16;
  rtems_device_driver status ;

  /* register devices */  
  for ( a = 0 ; a < 2 ; a++ )
  {
    status = rtems_io_register_name(
      sci_device[a].name,
      major,
      sci_device[a].minor );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }

  /* default hardware setup */

  /* general setup */
  temp16 = read16(PFC_PECR1) | 0x0800;	/* General I/O except pin 13 (reset) */
  write16(temp16, PFC_PECR1);
  write16(0x00, PFC_PECR2);	/* All I/O lines bits 7-0 */
  temp16 = read16(PFC_PEIOR) | 0x0020;	/* P5 to out, all other pins in */
  write16(temp16, PFC_PEIOR);

  temp16 = read16(PFC_PACRL2) | 0x0145;  /* PFC - pins for Tx0-1, Rx0-1 */
  write16(temp16, PFC_PACRL2);
  
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
  unsigned8 temp8;
  unsigned char	smr ;
  unsigned char brr ;
  
  unsigned 	a ;
  
  /* device already opened */
  if ( sci_device[minor].opened > 0 )
  {
    sci_device[minor].opened++ ;

    return RTEMS_SUCCESSFUL ;
  }
    
  /* retrieve brr and smr values */
  _sci_get_brparms( sci_device[minor].cflags, &smr, &brr );
  
  if (minor == 0) {
    write8(0x00, SCI_SCR0);	/* Clear SCR */
    write8(smr, SCI_SMR0);	/* Clear SMR */
    write8(brr, SCI_BRR0);	/* Default 9600 baud rate */
#if 0
    write8(0x1F, SCI_BRR0);    /* 28800 baud */
#endif
/* FIXME: Will get optimized away */
    for(a=0;a<10000L;a++);	/* One bit delay */
    write8(0x30, SCI_SCR0);	/* Enable clock output */
    temp8 = read8(SCI_RDR0);      /* Clear out old input */

  } else {
    write8(0x00, SCI_SCR1);	/* Clear SCR */
    write8(smr, SCI_SMR1);	/* Clear SMR */
    write8(brr, SCI_BRR1);	/* Default 9600 baud rate */
#if 0
    write8(0x1F, SCI_BRR1);    /* 28800 baud */
#endif
/* FIXME: Will get optimized away */
    for(a=0;a<10000L;a++);	/* One bit delay */
    write8(0x30, SCI_SCR1);	/* Enable clock output */
    temp8 = read8(SCI_RDR1);      /* Clear out old input */
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
  /* FIXME: Incomplete */
  if ( sci_device[minor].opened > 0 )
    sci_device[minor].opened-- ;
  else
    return RTEMS_INVALID_NUMBER ;
    
  return RTEMS_SUCCESSFUL ;
}

/*
 * read bytes from the serial port. We only have stdin.
 */

rtems_device_driver sh_sci_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte(minor);
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      break;
    }
  }

  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

/*
 * write bytes to the serial port. Stdout and stderr are the same. 
 */

rtems_device_driver sh_sci_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;

  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte(minor, '\r');
    }
    outbyte( minor, buffer[ count ] );
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
