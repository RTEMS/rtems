/*
 * /dev/gdbsci[0|1] for gdb's simulator's SH sci emulation
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 2001, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  $Id$
 */

#include <rtems.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <iosupp.h>
#include <rtems/score/sh_io.h>
/* HACK: There must be something better than this :) */
#if defined(__sh1__)
#include <rtems/score/ispsh7032.h>
#include <rtems/score/iosh7032.h>
#elif defined(__sh2__)
#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>
#else
#error unsupported sh model
#endif
#include <sh/sh7_sci.h>
#include <sh/sh7_pfc.h>
#include <sh/sci.h>

/* 
 * gdb assumes area 5/char access (base address & 0x0500000), 
 * the RTEMS's sh7045 code however defaults to area 5/int/short/char access
 * [Very likely a bug in the sh7045 code, RC.]
 */
 
#define GDBSCI_BASE 0x05ffffff

#define GDBSCI0_SMR (SCI0_SMR & GDBSCI_BASE)
#define GDBSCI0_BRR (SCI0_BRR & GDBSCI_BASE)
#define GDBSCI0_SCR (SCI0_SCR & GDBSCI_BASE)
#define GDBSCI0_TDR (SCI0_TDR & GDBSCI_BASE)
#define GDBSCI0_SSR (SCI0_SSR & GDBSCI_BASE)
#define GDBSCI0_RDR (SCI0_RDR & GDBSCI_BASE)

#define GDBSCI1_SMR (SCI1_SMR & GDBSCI_BASE)
#define GDBSCI1_BRR (SCI1_BRR & GDBSCI_BASE)
#define GDBSCI1_SCR (SCI1_SCR & GDBSCI_BASE)
#define GDBSCI1_TDR (SCI1_TDR & GDBSCI_BASE)
#define GDBSCI1_SSR (SCI1_SSR & GDBSCI_BASE)
#define GDBSCI1_RDG (SCI1_RDR & GDBSCI_BASE)

/*
 * NOTE: Only device 1 is valid for the simulator
 */
  
#define SH_GDBSCI_MINOR_DEVICES       2

/* Force SIGBUS by using an unsupported address for /dev/gdbsci0 */
#define SH_GDBSCI_BASE_0   SCI0_SMR
#define SH_GDBSCI_BASE_1   GDBSCI1_SMR

struct scidev_t {
  char *			name ;
  unsigned32			addr ;
  rtems_device_minor_number	minor ;
  unsigned short		opened ;
  tcflag_t			cflags ;
} sci_device[SH_GDBSCI_MINOR_DEVICES] =
{
  { "/dev/gdbsci0", SH_GDBSCI_BASE_0, 0, 0, B9600 | CS8 },
  { "/dev/gdbsci1", SH_GDBSCI_BASE_1, 1, 0, B9600 | CS8 }
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
  unsigned8	smr ;
  unsigned8	brr ;
  
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
#if NOT_SUPPORTED_BY_GDB
  unsigned16	temp16 ;

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
#endif
}

static void _sci_tx_polled(
  int minor,
  const char buf )
{
  struct scidev_t *scidev = &sci_device[minor] ;
#if NOT_SUPPORTED_BY_GDB                
  signed8         ssr ;

  while ( !inb((scidev->addr + SCI_SSR) & SCI_TDRE ))
      ;
#endif
  write8(buf,scidev->addr+SCI_TDR);

#if NOT_SUPPORTED_BY_GDB                
  ssr = inb(scidev->addr+SCI_SSR);
  ssr &= ~SCI_TDRE ;
  write8(ssr,scidev->addr+SCI_SSR);
#endif
} 

static int _sci_rx_polled (
  int minor)
{
  struct scidev_t *scidev = &sci_device[minor] ;
	
  unsigned char c;
#if NOT_SUPPORTED_BY_GDB
  char ssr ;
  ssr = read8(scidev->addr + SCI_SSR) ;

  if (ssr & (SCI_PER | SCI_FER | SCI_ORER))
    write8(ssr & ~(SCI_PER | SCI_FER | SCI_ORER), scidev->addr+SCI_SSR);

  if ( !(ssr & SCI_RDRF) )
    return -1;
#endif		
  c = read8(scidev->addr + SCI_RDR) ;
#if NOT_SUPPORTED_BY_GDB  
  write8(ssr & ~SCI_RDRF,scidev->addr + SCI_SSR);
#endif
  return c;
}

/*
 * sci_initialize
 */

rtems_device_driver sh_gdbsci_initialize(
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

  for ( i = 0 ; i < SH_GDBSCI_MINOR_DEVICES ; i++ )
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

rtems_device_driver sh_gdbsci_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg )
{
#if NOT_SUPPORTED_BY_GDB
  unsigned8 temp8;
#endif
 /* check for valid minor number */
   if(( minor > ( SH_GDBSCI_MINOR_DEVICES -1 )) || ( minor < 0 ))
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

#if NOT_SUPPORTED_BY_GDB
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
#endif

  sci_device[minor].opened++ ;

  return RTEMS_SUCCESSFUL ;
}
 
/*
 *  Close entry point
 */

rtems_device_driver sh_gdbsci_close(
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

rtems_device_driver sh_gdbsci_read(
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

rtems_device_driver sh_gdbsci_write(
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
    _sci_tx_polled( minor, buffer[ count ] );
  }

  rw_args->bytes_moved = maximum;
  return 0;
}

/*
 *  IO Control entry point
 */

rtems_device_driver sh_gdbsci_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  /* Not yet supported */
  return RTEMS_SUCCESSFUL ;
}
