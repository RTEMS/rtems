/*
 * Ported by Rosimildo da Silva.
 * ConnectTel,Inc.
 * e-mail: rdasilva@connecttel.com
 *
 * MODULE DESCRIPTION:
 * RTEMS driver for 3COM 3C509 Ethernet Card.
 * The driver has been tested on PC with a single network card.
 *
 *
 * This driver was based on the FreeBSD implementation( if_ep.c ) of the 3c5x9
 * family and on the network framework of the RTEMS network driver.
 * ( WD80x3 by  Eric Norum ).
 * See notes below:
 *
 ******************************************************************************
 * Copyright (c) 1994 Herb Peyerl <hpeyerl@novatel.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Herb Peyerl.
 * 4. The name of Herb Peyerl may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 * RTEMS driver for M68360 WD1 Ethernet
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <bsp.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/libkern.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <bsp/irq.h>

/* Local includes */
#include "3c509.h"
#include "elink.h"

/* #define	ET_MINLEN 60 */		/* minimum message length */

/*
 * Number of WDs supported by this driver
 */
#define NWDDRIVER	1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
/*
#define RX_BUF_COUNT     15
#define TX_BUF_COUNT     4
#define TX_BD_PER_BUF    4
*/

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT			RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */

/*
#define RBUF_SIZE	1520

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif
*/

/* network driver name */
#define NET_DRIVER_NAME		"ep"

/*
 * Per device structure.
 *
 * XXX Note:  id_conflicts should either become an array of things we're
 * specifically allowed to conflict with or be subsumed into some
 * more powerful mechanism for detecting and dealing with multiple types
 * of non-fatal conflict.  -jkh XXX
 */
struct isa_device
{
  int	id_id;		/* device id */
  int	id_unit;	/* unit number */
  int	id_iobase;	/* base i/o address */
  u_int	id_irq;		/* interrupt request */
};

struct ep_board
{
  int epb_addr;	        /* address of this board */
  char epb_used;		/* was this entry already used for configuring ? */
       					/* data from EEPROM for later use */
  u_short eth_addr[3];	/* Ethernet address */
  u_short prod_id;		/* product ID */
  u_short res_cfg;		/* resource configuration */
};

/*
 * Ethernet software status per interface.
 */
struct ep_softc
{
    struct arpcom arpcom;	/* Ethernet common part		 */
    int ep_io_addr;			/* i/o bus address		 	 */
    struct mbuf *top, *mcur;
    short cur_len;
    u_short ep_connectors;	/* Connectors on this card.	 */
    u_char ep_connector;	/* Configured connector.	 */
    int stat;				/* some flags 				 */
    struct ep_board *epb;
    int unit;

    rtems_irq_connect_data   irqInfo;
    rtems_id		     	 rxDaemonTid;
    rtems_id		     	 txDaemonTid;

    int              	 acceptBroadcast;

    short tx_underrun;
    short rx_no_first;
    short rx_no_mbuf;
    short rx_bpf_disc;
    short rx_overrunf;
    short rx_overrunl;
};

/*  static unsigned long loopc; */
static volatile unsigned long overrun;
static volatile unsigned long resend;
static struct ep_softc ep_softc[ NWDDRIVER ];
static struct isa_device isa_dev[ NWDDRIVER ] =
{
  { 0, 	   		/* device id 		  		*/
    0, 	   		/* unit number 		  		*/
    -1,	    	/* base i/o address   ???	*/
    0     		/* interrupt request  ???	*/
  }
};

static  u_long	ep_unit;
static  int	    ep_boards;
struct	ep_board ep_board[ EP_MAX_BOARDS + 1];
static	int ep_current_tag = EP_LAST_TAG + 1;
static	char *ep_conn_type[] = {"UTP", "AUI", "???", "BNC"};

#define ep_ftst(f) (sc->stat&(f))
#define ep_fset(f) (sc->stat|=(f))
#define ep_frst(f) (sc->stat&=~(f))

/* forward declarations for functions */
static int ep_attach( struct ep_softc *sc );
static int ep_isa_probe( struct isa_device *is );
static void epinit(  struct ep_softc *sc );
static void epread( register struct ep_softc *sc );
static void epstart( struct ifnet *ifp );
static void epread( register struct ep_softc *sc );
static int ep_isa_attach( struct isa_device *is );
static int get_eeprom_data( int id_port, int offset );
static void ep_intr( struct ep_softc *sc );

/* external functions */
extern void Wait_X_ms( unsigned int timeToWait );  /* timer.c ??? */

/**********************************************************************************
 *
 * DESCRIPTION: Writes a buffer of data to the I/O port. The data is sent to the
 *				port as 32 bits units( 4 bytes ).
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static __inline void outsl( unsigned short io_addr, uint8_t *out_data, int len )
{
   u_long *pl = ( u_long *)out_data;
   while( len-- )
   {
     outport_long( io_addr, *pl );
	 pl++;
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Writes a buffer of data to the I/O port. The data is sent to the
 *				port as 16 bits units( 2 bytes ).
 *
 * RETURNS:
 *
 **********************************************************************************/
static __inline void outsw( unsigned short io_addr, uint8_t *out_data, int len )
{
   u_short *ps = ( u_short *)out_data;
   while( len-- )
   {
     outport_word( io_addr, *ps );
	 ps++;
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Writes a buffer of data to the I/O port. The data is sent to the
 *				port as 8 bits units( 1 byte ).
 *
 * RETURNS: nothing
 *
 **********************************************************************************/
static __inline void outsb( unsigned short io_addr, uint8_t *out_data, int len )
{
   while( len-- )
   {
     outport_byte( io_addr, *out_data );
	 out_data++;
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Read a buffer of data from an I/O port. The data is read as 16 bits
 *			    units or 2 bytes.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static __inline void insw( unsigned short io_addr, uint8_t *in_data, int len )
{
   u_short *ps = ( u_short *)in_data;
   while( len-- )
   {
     inport_word( io_addr, *ps );
	 ps++;
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Read a buffer of data from an I/O port. The data is read as 32 bits
 *			    units or 4 bytes.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static __inline void insl( unsigned short io_addr, uint8_t *in_data, int len )
{
   u_long *pl = ( u_long *)in_data;
   while( len-- )
   {
     inport_long( io_addr, *pl );
	 pl++;
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Read a buffer of data from an I/O port. The data is read as 8 bits
 *			    units or 1 bytes.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static __inline void insb( unsigned short io_addr, uint8_t *in_data, int len )
{
   while( len-- )
   {
     inport_byte( io_addr, *in_data++ );
   }
}

/**********************************************************************************
 *
 * DESCRIPTION: Writes a word to the I/O port.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
/*
 * Routine to output a word as defined in FreeBSD.
 */
static __inline void outw( unsigned short io_addr, unsigned short out_data )
{
  outport_word( io_addr, out_data );
}

/**********************************************************************************
 *
 * DESCRIPTION:  Routine to read a word as defined in FreeBSD.
 *
 * RETURNS: nothing
 *
 **********************************************************************************/
static __inline unsigned short inw( unsigned short io_addr )
{
  unsigned short in_data;
  inport_word( io_addr, in_data );
  return in_data;
}

/**********************************************************************************
 *
 * DESCRIPTION: Routine to output a word as defined in FreeBSD.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
void __inline outb( unsigned short io_addr, uint8_t out_data )
{
  outport_byte( io_addr, out_data );
}

/**********************************************************************************
 *
 * DESCRIPTION: Routine to read a word as defined in FreeBSD.
 *
 * RETURNS: byte read.
 *
 **********************************************************************************/
static __inline uint8_t inb( unsigned short io_addr )
{
  uint8_t in_data;
  inport_byte( io_addr, in_data );
  return in_data;
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * We get eeprom data from the id_port given an offset into the eeprom.
 * Basically; after the ID_sequence is sent to all of the cards; they enter
 * the ID_CMD state where they will accept command requests. 0x80-0xbf loads
 * the eeprom data.  We then read the port 16 times and with every read; the
 * cards check for contention (ie: if one card writes a 0 bit and another
 * writes a 1 bit then the host sees a 0. At the end of the cycle; each card
 * compares the data on the bus; if there is a difference then that card goes
 * into ID_WAIT state again). In the meantime; one bit of data is returned in
 * the AX register which is conveniently returned to us by inb().  Hence; we
 * read 16 times getting one bit of data with each read.
 *
 * RETURNS: 16 bit word from the EEPROM
 *
 **********************************************************************************/
static int get_eeprom_data( int id_port, int offset )
{
    int i, data = 0;
    outb(id_port, 0x80 + offset);
    Wait_X_ms( 1 );
    for (i = 0; i < 16; i++)
	    data = (data << 1) | (inw(id_port) & 1);
    return( data );
}

/**********************************************************************************
 *
 * DESCRIPTION: Waits until the EEPROM of the card is ready to be accessed.
 *
 * RETURNS: 0 - not ready; 1 - ok
 *
 **********************************************************************************/
static int eeprom_rdy( struct ep_softc *sc )
{
    int i;

    for (i = 0; is_eeprom_busy(BASE) && i < MAX_EEPROMBUSY; i++)
	    continue;
    if (i >= MAX_EEPROMBUSY)
    {
	   printf("ep%d: eeprom failed to come ready.\n", sc->unit);
	   return (0);
    }
    return (1);
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * get_e: gets a 16 bits word from the EEPROM.
 * We must have set the window before call this routine.
 *
 * RETURNS: data from EEPROM
 *
 **********************************************************************************/
u_short get_e(  struct ep_softc *sc, int offset )
{
    if( !eeprom_rdy(sc) )
	   return (0xffff);
    outw(BASE + EP_W0_EEPROM_COMMAND, EEPROM_CMD_RD | offset );
    if( !eeprom_rdy(sc) )
	   return( 0xffff );
    return( inw( BASE + EP_W0_EEPROM_DATA ) );
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * Driver interrupt handler. This routine is called by the RTEMS kernel when this
 * interrupt is raised.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static rtems_isr ap_interrupt_handler( rtems_vector_number v )
{
  struct ep_softc *sc = (struct ep_softc *)&ep_softc[ 0 ];

  /* de-activate any pending interrrupt, and sent and event to interrupt task
   * to process all events required by this interrupt.
   */
  outw( BASE + EP_COMMAND, SET_INTR_MASK ); 	/* disable all Ints */
  rtems_event_send( sc->rxDaemonTid, INTERRUPT_EVENT );
}

/**********************************************************************************
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 **********************************************************************************/
static void nopOn(const rtems_irq_connect_data* notUsed)
{
  /* does nothing */
}

/**********************************************************************************
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *
 **********************************************************************************/
static int _3c509_IsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * Initializes the ethernet hardware.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void _3c509_initialize_hardware (struct ep_softc *sc)
{
  rtems_status_code st;

  epinit( sc );

  /*
   * Set up interrupts
   */
  sc->irqInfo.hdl = ( rtems_irq_hdl )ap_interrupt_handler;
  sc->irqInfo.on  = nopOn;
  sc->irqInfo.off = nopOn;
  sc->irqInfo.isOn = _3c509_IsOn;

  printf ("3c509: IRQ with Kernel: %d\n", sc->irqInfo.name );
  st = BSP_install_rtems_irq_handler( &sc->irqInfo );
  if( !st )
  {
    rtems_panic ("Can't attach WD interrupt handler for irq %d\n", sc->irqInfo.name );
  }
}

/**********************************************************************************
 *
 * DESCRIPTION: Driver interrupt daemon.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void _3c509_rxDaemon (void *arg)
{
  struct ep_softc *dp = (struct ep_softc *)&ep_softc[ 0 ];
  rtems_event_set events;

  printf ("3C509: RX Daemon is starting.\n");
  for( ;; )
  {
    /* printk( "R-" ); */
    rtems_bsdnet_event_receive( INTERRUPT_EVENT,
				                 RTEMS_WAIT | RTEMS_EVENT_ANY,
				                 RTEMS_NO_TIMEOUT,
				                 &events );
    /* printk( "R+" ); */
    ep_intr( dp );
    epstart( &dp->arpcom.ac_if );
  }
  printf ("3C509: RX Daemon is finishing.\n");
}

/**********************************************************************************
 *
 * DESCRIPTION: Driver transmit daemon
 *
 * RETURNS:
 *
 **********************************************************************************/
static void _3c509_txDaemon (void *arg)
{
    struct ep_softc *sc = (struct ep_softc *)&ep_softc[0];
	struct ifnet *ifp = &sc->arpcom.ac_if;
	rtems_event_set events;

    printf ("3C509: TX Daemon is starting.\n");
    for( ;; )
	{
		/*
		 * Wait for packet
		 */
	  /* printk( "T-\n" ); */
		rtems_bsdnet_event_receive( START_TRANSMIT_EVENT,
									RTEMS_EVENT_ANY | RTEMS_WAIT,
									RTEMS_NO_TIMEOUT,
									&events );
		/*	printk( "T+\n" ); */
      epstart( ifp );
      while( ifp->if_flags & IFF_OACTIVE )
          epstart( ifp );
	}
   printf ("3C509: TX Daemon is finishing.\n");
}

/**********************************************************************************
 *
 * DESCRIPTION: Activates the trabsmitter task...
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void _3c509_start (struct ifnet *ifp)
{
	struct ep_softc *sc = ifp->if_softc;
	/*    printk ("S");  */
	ifp->if_flags |= IFF_OACTIVE;
	rtems_event_send( sc->txDaemonTid, START_TRANSMIT_EVENT );
}

/**********************************************************************************
 *
 * DESCRIPTION: Initialize and start the device
 *
 * RETURNS:
 *
 **********************************************************************************/
static void _3c509_init (void *arg)
{
  struct ep_softc *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  printf ("3C509: Initialization called.\n");
  if (sc->txDaemonTid == 0) {

    /*
     * Set up WD hardware
     */
    _3c509_initialize_hardware (sc);
    printf ("3C509: starting network driver tasks..\n");
    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("APtx", 4096, _3c509_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("APrx", 4096, _3c509_rxDaemon, sc);
  }

  /*
   * Tell the world that we're running.
   */
   ifp->if_flags |= IFF_RUNNING;
}

/**********************************************************************************
 *
 * DESCRIPTION: Stop the device
 *
 * RETURNS:
 *
 **********************************************************************************/
static void _3c509_stop (struct ep_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  ifp->if_flags &= ~IFF_RUNNING;

  printf ("3C509: stop() called.\n");
  /*
   * Stop the transmitter
   */
  outw(BASE + EP_COMMAND, RX_DISABLE);
  outw(BASE + EP_COMMAND, RX_DISCARD_TOP_PACK);
  while (inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS);
  outw(BASE + EP_COMMAND, TX_DISABLE);
  outw(BASE + EP_COMMAND, STOP_TRANSCEIVER);
  outw(BASE + EP_COMMAND, RX_RESET);
  outw(BASE + EP_COMMAND, TX_RESET);
  while (inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS);
  outw(BASE + EP_COMMAND, C_INTR_LATCH);
  outw(BASE + EP_COMMAND, SET_RD_0_MASK);
  outw(BASE + EP_COMMAND, SET_INTR_MASK);
  outw(BASE + EP_COMMAND, SET_RX_FILTER);
}

/**********************************************************************************
 *
 * DESCRIPTION:  Show interface statistics
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void _3c509_stats (struct ep_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  printf ("3C509: stats() called.\n");
  printf("\tStat: %x\n", sc->stat);
  printf("\tIpackets=%ld, Opackets=%ld\n", ifp->if_ipackets, ifp->if_opackets);
  printf("\tNOF=%d, NOMB=%d, BPFD=%d, RXOF=%d, RXOL=%d, TXU=%d\n",
           sc->rx_no_first, sc->rx_no_mbuf, sc->rx_bpf_disc, sc->rx_overrunf,
           sc->rx_overrunl, sc->tx_underrun );
}

/**********************************************************************************
 *
 * DESCRIPTION: Driver ioctl handler
 *
 * RETURNS:
 *
 **********************************************************************************/
static int _3c509_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
	struct ep_softc *sc = ifp->if_softc;
	int error = 0;

    printf ("3C509: ioctl() called.\n");
	switch (command) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		ether_ioctl (ifp, command, data);
		break;

	case SIOCSIFFLAGS:
		switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
		case IFF_RUNNING:
			_3c509_stop (sc);
			break;

		case IFF_UP:
			_3c509_init (sc);
			break;

		case IFF_UP | IFF_RUNNING:
			_3c509_stop (sc);
			_3c509_init (sc);
			break;

		default:
			break;
		}
		break;

	case SIO_RTEMS_SHOW_STATS:
		_3c509_stats( sc );
		break;

	/*
	 * FIXME: All sorts of multicast commands need to be added here!
	 */
	default:
		error = EINVAL;
		break;
	}
	return error;
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * Attaches this network driver to the system. This function is called by the network
 * interface during the initialization of the system.
 *
 * RETURNS: - 1 - success; 0 - fail to initialize
 *
 **********************************************************************************/
int rtems_3c509_driver_attach (struct rtems_bsdnet_ifconfig *config )
{
	struct ep_softc *sc;
	struct ifnet *ifp;
	int mtu;
	int i;

    printf ("3C509: attach() called.\n");

	/*
	 * init some variables
	 */
	overrun = 0;
	resend = 0;
    ep_unit = 0;
    ep_boards = 0;

	/*
	 * Find a free driver
	 */
	for (i = 0 ; i < NWDDRIVER ; i++) {
		sc = &ep_softc[i];
		ifp = &sc->arpcom.ac_if;
		if (ifp->if_softc == NULL)
			break;
	}
	if (i >= NWDDRIVER)
	{
		printf ("Too many 3C509 drivers.\n");
		return 0;
	}

	/*
	 * Process options
	 */
	if( config->hardware_address )
	{
	  memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
	}
	else
	{
	  /* set it to something ... */
	  memset (sc->arpcom.ac_enaddr, 0x08,ETHER_ADDR_LEN);
	}
	if (config->mtu)
		mtu = config->mtu;
	else
		mtu = ETHERMTU;

	if (config->irno)
		sc->irqInfo.name = config->irno;
	else
		sc->irqInfo.name = 10;

	if (config->port)
		sc->ep_io_addr = config->port;
	else
		sc->ep_io_addr = 0x300;

	sc->acceptBroadcast = !config->ignore_broadcast;

    printf ("3C509: isa_probe() looking for a card...\n");
    if( !ep_isa_probe( &isa_dev[ 0 ] ) )
	{
	   printf ("3C509: isa_probe() fail to find a board.\n");
	   return 0;
	}

	/* A board has been found, so proceed with the installation of the driver */
    ep_isa_attach( &isa_dev[ 0 ] );
	/*
	 * Set up network interface values
	 */

	ifp->if_softc = sc;
	ifp->if_unit = i;
	ifp->if_name = NET_DRIVER_NAME;
	ifp->if_mtu = mtu;
	ifp->if_init = _3c509_init;
	ifp->if_ioctl = _3c509_ioctl;
	ifp->if_start = _3c509_start;
	ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	if( ifp->if_snd.ifq_maxlen == 0 )
	{
	   ifp->if_snd.ifq_maxlen = ifqmaxlen;
	}
 	/*
	 * Attach the interface
	 */
	if_attach (ifp);
	ether_ifattach (ifp);
    printf ("3C509: attach() is complete.\n");
	return 1;
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * This function looks for a 3COM card 3c5x9 in an isa bus. If a board is found, it
 * returns a structure describing the caracteristics of the card. It returns zero when
 * card can not be found.
 *
 * RETURNS:  	0 - fail - could not find a card...
 *				<>  description of the card.
 *
 **********************************************************************************/
static struct ep_board *ep_look_for_board_at( struct isa_device *is )
{
    int data, i, j, id_port = ELINK_ID_PORT;
    int count = 0;

    if(ep_current_tag == (EP_LAST_TAG + 1) )
    {
    	/* Come here just one time */
		ep_current_tag--;

        /* Look for the ISA boards. Init and leave them actived */
		outb(id_port, 0);
		outb(id_port, 0);

		elink_idseq(0xCF);
		elink_reset();
      Wait_X_ms( 10 );  /* RPS: assuming delay in miliseconds */
		for (i = 0; i < EP_MAX_BOARDS; i++)
        {
	    	outb(id_port, 0);
		    outb(id_port, 0);
		    elink_idseq(0xCF);

	    	data = get_eeprom_data(id_port, EEPROM_MFG_ID);
		    if (data != MFG_ID)
				break;

		    /* resolve contention using the Ethernet address */
		    for (j = 0; j < 3; j++)
		        get_eeprom_data(id_port, j);

		    /* and save this address for later use */

		    for (j = 0; j < 3; j++)
			   ep_board[ep_boards].eth_addr[j] = get_eeprom_data(id_port, j);

		    ep_board[ep_boards].res_cfg = get_eeprom_data(id_port, EEPROM_RESOURCE_CFG);
		    ep_board[ep_boards].prod_id = get_eeprom_data(id_port, EEPROM_PROD_ID);
		    ep_board[ep_boards].epb_used = 0;
#ifdef PC98
		    ep_board[ep_boards].epb_addr =
				(get_eeprom_data(id_port, EEPROM_ADDR_CFG) & 0x1f) * 0x100 + 0x40d0;
#else
		    ep_board[ep_boards].epb_addr =
				(get_eeprom_data(id_port, EEPROM_ADDR_CFG) & 0x1f) * 0x10 + 0x200;
		    if (ep_board[ep_boards].epb_addr > 0x3E0)
				/* Board in EISA configuration mode */
				continue;
#endif /* PC98 */

		    outb(id_port, ep_current_tag);	/* tags board */
		    outb(id_port, ACTIVATE_ADAPTER_TO_CONFIG);
		    ep_boards++;
	    	count++;
		    ep_current_tag--;
		}
		ep_board[ep_boards].epb_addr = 0;
		if( count )
		{
	    	printf("%d 3C5x9 board(s) on ISA found at", count);
		    for (j = 0; ep_board[j].epb_addr; j++)
			   if( ep_board[j].epb_addr <= 0x3E0 )
			       printf(" 0x%x", ep_board[j].epb_addr );
 		    printf("\n");
		}
    }

    /* we have two cases:
     *
     *  1. Device was configured with 'port ?'
     *      In this case we search for the first unused card in list
     *
     *  2. Device was configured with 'port xxx'
     *      In this case we search for the unused card with that address
     *
     */

    if (IS_BASE == -1)
    { /* port? */
		for (i = 0; ep_board[i].epb_addr && ep_board[i].epb_used; i++) ;
		if (ep_board[i].epb_addr == 0)
		      return 0;

		IS_BASE = ep_board[i].epb_addr;
		ep_board[i].epb_used = 1;
		return &ep_board[ i ];
    }
    else
    {
		for (i = 0; ep_board[i].epb_addr && ep_board[i].epb_addr != IS_BASE;  i++ ) ;
		if (ep_board[i].epb_used || ep_board[i].epb_addr != IS_BASE)
			    return 0;
		if (inw(IS_BASE + EP_W0_EEPROM_COMMAND) & EEPROM_TST_MODE)
		{
	    	printf("ep%d: 3c5x9 at 0x%x in PnP mode. Disable PnP mode!\n",
	    			is->id_unit, IS_BASE );
		}
		ep_board[i].epb_used = 1;
		return &ep_board[i];
    }
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * This routine checks if there card installed on the machine.
 *
 * RETURNS: 0 - no card founded.
 *			16 - size of the IO range for the card.
 *
 **********************************************************************************/
static int ep_isa_probe( struct isa_device *is )
{
    struct ep_softc *sc;
    struct ep_board *epb;
    u_short k;

    /* try to find a 3COM 3c5x9 .... */
    if( (epb = ep_look_for_board_at(is)) == 0 )
		return (0);

    sc = &ep_softc[ 0 ];
    sc->ep_io_addr = epb->epb_addr;
    sc->epb = epb;

    /*
     * The iobase was found and MFG_ID was 0x6d50. PROD_ID should be
     * 0x9[0-f]50	(IBM-PC)
     * 0x9[0-f]5[0-f]	(PC-98)
     */
    GO_WINDOW(0);
    k = sc->epb->prod_id;
#ifdef PC98
    if ((k & 0xf0f0) != (PROD_ID & 0xf0f0))
    {
#else
    if ((k & 0xf0ff) != (PROD_ID & 0xf0ff))
    {
#endif
	    printf("ep_isa_probe: ignoring model %04x\n", k );
/*        ep_unit--;  */
        return (0);
    }
    k = sc->epb->res_cfg;
    k >>= 12;

    /* Now we have two cases again:
     *
     *  1. Device was configured with 'irq?'
     *      In this case we use irq read from the board
     *
     *  2. Device was configured with 'irq xxx'
     *      In this case we set up the board to use specified interrupt
     *
     */

    if (is->id_irq == 0)
    {    /* irq? */
       is->id_irq = ( k == 2 ) ? 9 : k;
    }

    sc->stat = 0;	/* 16 bit access */

    /* By now, the adapter is already activated */

    return (EP_IOSIZE);	 /* 16 bytes of I/O space used. */
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * This routine attaches this network driver and the network interface routines.
 *
 * RETURNS: 0 - failed to attach
 *			1 - success
 *
 **********************************************************************************/
static int ep_isa_attach( struct isa_device *is )
{
    struct ep_softc *sc = &ep_softc[ 0 ];
    u_short config;
    int irq;

    sc->ep_connectors = 0;
    config = inw( IS_BASE + EP_W0_CONFIG_CTRL );
    if (config & IS_AUI)
    {
	   sc->ep_connectors |= AUI;
    }
    if (config & IS_BNC)
    {
	   sc->ep_connectors |= BNC;
    }
    if (config & IS_UTP)
    {
	   sc->ep_connectors |= UTP;
    }
    if( !(sc->ep_connectors & 7) )
       printf( "no connectors!" );
    sc->ep_connector = inw(BASE + EP_W0_ADDRESS_CFG) >> ACF_CONNECTOR_BITS;

    /*
     * Write IRQ value to board
     */

    irq = is->id_irq;
	/* update the interrupt line number to registered with kernel */
	sc->irqInfo.name = irq;

    GO_WINDOW( 0 );
    SET_IRQ( BASE, irq );

    printf( "3C509: I/O=0x%x, IRQ=%d, CONNECTOR=%s, ",
	    sc->ep_io_addr, sc->irqInfo.name,ep_conn_type[ sc->ep_connector ] );

    ep_attach( sc );
    return 1;
}

/**********************************************************************************
 *
 * DESCRIPTION: Completes the initialization/attachement of the driver.
 *
 * RETURNS: 0 - ok.
 *
 **********************************************************************************/
static int ep_attach( struct ep_softc *sc )
{
    u_short *p;
    int i;

    /*
     * Setup the station address
     */
    p = (u_short *) &sc->arpcom.ac_enaddr;
    GO_WINDOW(2);
    printf("ADDRESS=" );
    for (i = 0; i < 3; i++)
    {
	    p[i] = htons( sc->epb->eth_addr[i] );
        outw( BASE + EP_W2_ADDR_0 + (i * 2), ntohs( p[i] ) );
        printf("%04x ", (u_short)ntohs( p[i] ) );
    }
    printf("\n" );

    sc->rx_no_first = sc->rx_no_mbuf =
	sc->rx_bpf_disc = sc->rx_overrunf = sc->rx_overrunl =
	sc->tx_underrun = 0;

    ep_fset( F_RX_FIRST );
    sc->top = sc->mcur = 0;
    return 0;
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * Initializes the card.
 * The order in here seems important. Otherwise we may not receive interrupts. ?!
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void epinit( struct ep_softc *sc )
{
    register struct ifnet *ifp = &sc->arpcom.ac_if;
    int i, j;

    while( inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS ) ;
    GO_WINDOW(0);
    outw(BASE + EP_COMMAND, STOP_TRANSCEIVER);
    GO_WINDOW(4);
    outw(BASE + EP_W4_MEDIA_TYPE, DISABLE_UTP);
    GO_WINDOW(0);

    /* Disable the card */
    outw(BASE + EP_W0_CONFIG_CTRL, 0);

    /* Enable the card */
    outw(BASE + EP_W0_CONFIG_CTRL, ENABLE_DRQ_IRQ);

    GO_WINDOW(2);

    /* Reload the ether_addr. */
    for (i = 0; i < 6; i++)
       outb(BASE + EP_W2_ADDR_0 + i, sc->arpcom.ac_enaddr[i]);

    outw(BASE + EP_COMMAND, RX_RESET);
    outw(BASE + EP_COMMAND, TX_RESET);
    while (inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS);

    /* Window 1 is operating window */
    GO_WINDOW(1);
    for (i = 0; i < 31; i++)
       inb(BASE + EP_W1_TX_STATUS);

    /* get rid of stray intr's */
    outw(BASE + EP_COMMAND, ACK_INTR | 0xff);

    outw(BASE + EP_COMMAND, SET_RD_0_MASK | S_5_INTS);

    outw(BASE + EP_COMMAND, SET_INTR_MASK | S_5_INTS);

    if (ifp->if_flags & IFF_PROMISC)
	   outw(BASE + EP_COMMAND, SET_RX_FILTER | FIL_INDIVIDUAL |
	 						    FIL_GROUP | FIL_BRDCST | FIL_ALL);
    else
	   outw(BASE + EP_COMMAND, SET_RX_FILTER | FIL_INDIVIDUAL | FIL_GROUP | FIL_BRDCST);

     /*
      * S.B.
      *
      * Now behavior was slightly changed:
      *
      * if any of flags link[0-2] is used and its connector is
      * physically present the following connectors are used:
      *
      *   link0 - AUI * highest precedence
      *   link1 - BNC
      *   link2 - UTP * lowest precedence
      *
      * If none of them is specified then
      * connector specified in the EEPROM is used
      * (if present on card or AUI if not).
      *
      */

    /* Set the xcvr. */
    if (ifp->if_flags & IFF_LINK0 && sc->ep_connectors & AUI)
    {
	   i = ACF_CONNECTOR_AUI;
    }
    else if (ifp->if_flags & IFF_LINK1 && sc->ep_connectors & BNC)
    {
	   i = ACF_CONNECTOR_BNC;
    }
    else if (ifp->if_flags & IFF_LINK2 && sc->ep_connectors & UTP)
    {
	   i = ACF_CONNECTOR_UTP;
    }
    else
    {
	   i = sc->ep_connector;
    }
    GO_WINDOW(0);
    j = inw(BASE + EP_W0_ADDRESS_CFG) & 0x3fff;
    outw(BASE + EP_W0_ADDRESS_CFG, j | (i << ACF_CONNECTOR_BITS));

    switch(i)
    {
      case ACF_CONNECTOR_UTP:
	  if (sc->ep_connectors & UTP)
	  {
	    GO_WINDOW(4);
	    outw(BASE + EP_W4_MEDIA_TYPE, ENABLE_UTP);
	  }
	  break;

      case ACF_CONNECTOR_BNC:
	   if (sc->ep_connectors & BNC)
	   {
	    outw(BASE + EP_COMMAND, START_TRANSCEIVER);
	    Wait_X_ms( 1 );
	   }
       break;

      case ACF_CONNECTOR_AUI:
  	    /* nothing to do */
		break;

      default:
	   printf("ep%d: strange connector type in EEPROM: assuming AUI\n", sc->unit);
	   break;
    }

    outw(BASE + EP_COMMAND, RX_ENABLE);
    outw(BASE + EP_COMMAND, TX_ENABLE);

    ifp->if_flags |= IFF_RUNNING;
    ifp->if_flags &= ~IFF_OACTIVE;	/* just in case */

    sc->rx_no_first = sc->rx_no_mbuf =
	sc->rx_bpf_disc = sc->rx_overrunf = sc->rx_overrunl =
	sc->tx_underrun = 0;

    ep_fset(F_RX_FIRST);
    if( sc->top )
    {
	   m_freem( sc->top );
	   sc->top = sc->mcur = 0;
    }
    outw(BASE + EP_COMMAND, SET_RX_EARLY_THRESH | RX_INIT_EARLY_THRESH);
    outw(BASE + EP_COMMAND, SET_TX_START_THRESH | 16);

    /*
     * Store up a bunch of mbuf's for use later. (MAX_MBS). First we free up
     * any that we had in case we're being called from intr or somewhere
     * else.
     */

    GO_WINDOW(1);
}

static const char padmap[] = {0, 3, 2, 1};

/**********************************************************************************
 *
 * DESCRIPTION: Routine to transmit frames to the card.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void epstart( struct ifnet *ifp )
{
    register struct ep_softc *sc = ifp->if_softc;
    register u_int len;
    register struct mbuf *m;
    struct mbuf *top;
    int pad;

    while( inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS )
        ;
startagain:
    /*    printk( "S-" ); */

    /* Sneak a peek at the next packet */
    m = ifp->if_snd.ifq_head;
    if (m == 0)
    {
       ifp->if_flags &= ~IFF_OACTIVE;
       return;
    }

    for( len = 0, top = m; m; m = m->m_next )
         len += m->m_len;

    pad = padmap[ len & 3 ];

    /*
     * The 3c509 automatically pads short packets to minimum ethernet length,
     * but we drop packets that are too large. Perhaps we should truncate
     * them instead?
     */
    if( len + pad > ETHER_MAX_LEN )
    {
	   /* packet is obviously too large: toss it */
       ++ifp->if_oerrors;
       IF_DEQUEUE( &ifp->if_snd, m );
       m_freem( m );
   	   goto readcheck;
    }
    if (inw(BASE + EP_W1_FREE_TX) < len + pad + 4)
    {
	   /* no room in FIFO */
	   outw(BASE + EP_COMMAND, SET_TX_AVAIL_THRESH | (len + pad + 4));
       /* make sure */
	   if (inw(BASE + EP_W1_FREE_TX) < len + pad + 4)
	   {
	       ifp->if_flags |= IFF_OACTIVE;
	       return;
	   }
    }
    IF_DEQUEUE( &ifp->if_snd, m );
    outw(BASE + EP_W1_TX_PIO_WR_1, len);
    outw(BASE + EP_W1_TX_PIO_WR_1, 0x0);	/* Second dword meaningless */

    for (top = m; m != 0; m = m->m_next)
	{
       if( ep_ftst(F_ACCESS_32_BITS ) )
       {
	      outsl( BASE + EP_W1_TX_PIO_WR_1, mtod(m, uint8_t *), m->m_len / 4 );
	      if( m->m_len & 3 )
             outsb(BASE + EP_W1_TX_PIO_WR_1, mtod(m, uint8_t *) + (m->m_len & (~3)), m->m_len & 3 );
       }
       else
       {
	      outsw( BASE + EP_W1_TX_PIO_WR_1, mtod(m, uint8_t *), m->m_len / 2 );
	      if( m->m_len & 1 )
		  outb( BASE + EP_W1_TX_PIO_WR_1, *(mtod(m, uint8_t *) + m->m_len - 1) );
	   }
	}
    while( pad-- )
	{
  	   outb(BASE + EP_W1_TX_PIO_WR_1, 0);	/* Padding */
	}
    ifp->if_timer = 2;
    ifp->if_opackets++;
    m_freem(top);

/*    goto startagain;   */
    /*
     * Is another packet coming in? We don't want to overflow the tiny RX
     * fifo.
     */
readcheck:
    if( inw(BASE + EP_W1_RX_STATUS) & RX_BYTES_MASK )
    {
	/*
	 * we check if we have packets left, in that case we prepare to come
	 * back later
	 */
	   if( ifp->if_snd.ifq_head )
	   {
	      outw(BASE + EP_COMMAND, SET_TX_AVAIL_THRESH | 8);
       }
	   return;
    }
    goto startagain;
}

/**********************************************************************************
 *
 * DESCRIPTION: Routine to read frames from the card.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void epread( register struct ep_softc *sc )
{
    struct ether_header *eh;
    struct mbuf *top, *mcur, *m;
    struct ifnet *ifp;
    int lenthisone;

    short rx_fifo2, status;
    register short rx_fifo;

    ifp = &sc->arpcom.ac_if;
    status = inw( BASE + EP_W1_RX_STATUS );

read_again:

    if (status & ERR_RX)
    {
	   ++ifp->if_ierrors;
	   if( status & ERR_RX_OVERRUN )
	   {
	    /*
	     * we can think the rx latency is actually greather than we
	     * expect
	     */
	    if( ep_ftst(F_RX_FIRST) )
 		   sc->rx_overrunf++;
	    else
		   sc->rx_overrunl++;

	   }
	   goto out;
    }
    rx_fifo = rx_fifo2 = status & RX_BYTES_MASK;

    if( ep_ftst( F_RX_FIRST ) )
    {
	   MGETHDR( m, M_DONTWAIT, MT_DATA );
       if( !m )
	      goto out;
	   if( rx_fifo >= MINCLSIZE )
	      MCLGET( m, M_DONTWAIT );
	   sc->top = sc->mcur = top = m;
#define EROUND  ((sizeof(struct ether_header) + 3) & ~3)
#define EOFF    (EROUND - sizeof(struct ether_header))
	   top->m_data += EOFF;

  	   /* Read what should be the header. */
	   insw(BASE + EP_W1_RX_PIO_RD_1, mtod(top, uint8_t *), sizeof(struct ether_header) / 2);
	   top->m_len = sizeof(struct ether_header);
	   rx_fifo -= sizeof(struct ether_header);
	   sc->cur_len = rx_fifo2;
    }
    else
    {
	   /* come here if we didn't have a complete packet last time */
	   top = sc->top;
	   m = sc->mcur;
	   sc->cur_len += rx_fifo2;
    }

    /* Reads what is left in the RX FIFO */
    while (rx_fifo > 0)
    {
	   lenthisone = min( rx_fifo, M_TRAILINGSPACE(m) );
	   if( lenthisone == 0 )
	   {	/* no room in this one */
	       mcur = m;
	       MGET(m, M_WAIT, MT_DATA);
	       if (!m)
		      goto out;
	       if (rx_fifo >= MINCLSIZE)
		       MCLGET(m, M_WAIT);
	       m->m_len = 0;
	       mcur->m_next = m;
	       lenthisone = min(rx_fifo, M_TRAILINGSPACE(m));
	   }
	   if( ep_ftst( F_ACCESS_32_BITS ) )
	   { /* default for EISA configured cards*/
	      insl( BASE + EP_W1_RX_PIO_RD_1, mtod(m, uint8_t *) + m->m_len, lenthisone / 4);
	      m->m_len += (lenthisone & ~3);
	      if (lenthisone & 3)
		     insb(BASE + EP_W1_RX_PIO_RD_1, mtod(m, uint8_t *) + m->m_len, lenthisone & 3);
	      m->m_len += (lenthisone & 3);
	    }
	    else
	    {
	      insw(BASE + EP_W1_RX_PIO_RD_1, mtod(m, uint8_t *) + m->m_len, lenthisone / 2);
	      m->m_len += lenthisone;
	      if( lenthisone & 1 )
		     *(mtod(m, uint8_t *) + m->m_len - 1) = inb(BASE + EP_W1_RX_PIO_RD_1);
	   }
	   rx_fifo -= lenthisone;
    }

    if( status & ERR_RX_INCOMPLETE)
    {	/* we haven't received the complete packet */
	    sc->mcur = m;
     	sc->rx_no_first++;	/* to know how often we come here */
     	ep_frst( F_RX_FIRST );
	    if( !((status = inw(BASE + EP_W1_RX_STATUS)) & ERR_RX_INCOMPLETE) )
	    {
	      /* we see if by now, the packet has completly arrived */
	      goto read_again;
	    }
	    outw(BASE + EP_COMMAND, SET_RX_EARLY_THRESH | RX_NEXT_EARLY_THRESH);
	    return;
    }
    outw(BASE + EP_COMMAND, RX_DISCARD_TOP_PACK);
    ++ifp->if_ipackets;
    ep_fset(F_RX_FIRST);
    top->m_pkthdr.rcvif = &sc->arpcom.ac_if;
    top->m_pkthdr.len = sc->cur_len;

    eh = mtod(top, struct ether_header *);
    m_adj(top, sizeof(struct ether_header));
    ether_input(ifp, eh, top);
    sc->top = 0;
    while (inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS)
          ;
    outw(BASE + EP_COMMAND, SET_RX_EARLY_THRESH | RX_INIT_EARLY_THRESH);
    return;

out:
    outw(BASE + EP_COMMAND, RX_DISCARD_TOP_PACK);
    if (sc->top)
    {
	   m_freem(sc->top);
	   sc->top = 0;
       sc->rx_no_mbuf++;
    }
    ep_fset(F_RX_FIRST);
    while (inw(BASE + EP_STATUS) & S_COMMAND_IN_PROGRESS) ;
    outw(BASE + EP_COMMAND, SET_RX_EARLY_THRESH | RX_INIT_EARLY_THRESH);
}

/**********************************************************************************
 *
 * DESCRIPTION:
 * This routine handles interrupts. It is called from the "RX" task whenever
 * the ISR post an event to the task.
 * This is basically the "isr" from the FreeBSD driver.
 *
 * RETURNS: nothing.
 *
 **********************************************************************************/
static void ep_intr( struct ep_softc *sc )
{
  register int status;
  struct ifnet *ifp;
  ifp = &sc->arpcom.ac_if;

rescan:

  /*  printk( "I-" ); */
  while( ( status = inw(BASE + EP_STATUS)) & S_5_INTS )
  {
	/* first acknowledge all interrupt sources */
    outw( BASE + EP_COMMAND, ACK_INTR | ( status & S_MASK ) );

	if( status & ( S_RX_COMPLETE | S_RX_EARLY ) )
	{
	    epread( sc );
	    continue;
	}
	if (status & S_TX_AVAIL)
	{
	    /* we need ACK */
	    ifp->if_timer = 0;
	    ifp->if_flags &= ~IFF_OACTIVE;
	    GO_WINDOW(1);
	    inw(BASE + EP_W1_FREE_TX);
	    epstart(ifp);
	}
	if (status & S_CARD_FAILURE)
	{
	    ifp->if_timer = 0;
	    printf("\nep%d:\n\tStatus: %x\n", sc->unit, status);
	    GO_WINDOW(4);
	    printf("\tFIFO Diagnostic: %x\n", inw(BASE + EP_W4_FIFO_DIAG));
	    printf("\tStat: %x\n", sc->stat);
	    printf("\tIpackets=%ld, Opackets=%ld\n", ifp->if_ipackets, ifp->if_opackets);
	    printf("\tNOF=%d, NOMB=%d, BPFD=%d, RXOF=%d, RXOL=%d, TXU=%d\n",
		   sc->rx_no_first, sc->rx_no_mbuf, sc->rx_bpf_disc, sc->rx_overrunf,
		   sc->rx_overrunl, sc->tx_underrun);

	    printf("ep%d: Status: %x (input buffer overflow)\n", sc->unit, status);
	    ++ifp->if_ierrors;
	    epinit(sc);
	    return;
	}
	if (status & S_TX_COMPLETE)
	{
	    ifp->if_timer = 0;
	    /* we  need ACK. we do it at the end */
	    /*
	     * We need to read TX_STATUS until we get a 0 status in order to
	     * turn off the interrupt flag.
	     */
	    while ((status = inb(BASE + EP_W1_TX_STATUS)) & TXS_COMPLETE)
	    {
		   if (status & TXS_SUCCES_INTR_REQ)
		        ;
		   else if( status & (TXS_UNDERRUN | TXS_JABBER | TXS_MAX_COLLISION ) )
		   {
		      outw(BASE + EP_COMMAND, TX_RESET);
		      if (status & TXS_UNDERRUN)
		      {
			     sc->tx_underrun++;
		      }
		      else
		      {
			      if( status & TXS_JABBER )
			           ;
		          else	/* TXS_MAX_COLLISION - we shouldn't get here */
			           ++ifp->if_collisions;
		      }
		      ++ifp->if_oerrors;
		      outw(BASE + EP_COMMAND, TX_ENABLE);
		     /*
		      * To have a tx_avail_int but giving the chance to the
		      * Reception
		      */
		      if( ifp->if_snd.ifq_head )
		      {
			     outw(BASE + EP_COMMAND, SET_TX_AVAIL_THRESH | 8);
		      }
		   }
		   outb( BASE + EP_W1_TX_STATUS, 0x0 );	/* pops up the next status */
	    } /* while */
	    ifp->if_flags &= ~IFF_OACTIVE;
	    GO_WINDOW(1);
	    inw(BASE + EP_W1_FREE_TX);
	    epstart( ifp );
	}  /* end TX_COMPLETE */
  }
  outw(BASE + EP_COMMAND, C_INTR_LATCH);	/* ACK int Latch */
  if( (status = inw(BASE + EP_STATUS) ) & S_5_INTS )
      goto rescan;

  /* re-enable Ints */
  outw( BASE + EP_COMMAND, SET_INTR_MASK | S_5_INTS );
  /* printk( "I+" ); */
}
