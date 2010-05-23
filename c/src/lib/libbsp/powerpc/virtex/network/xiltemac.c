/*
 * Driver for Xilinx plb temac v3.00a
 *
 * Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 * Copyright (c) 2007 Linn Products Ltd, Scotland.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */
#define PPC_HAS_CLASSIC_EXCEPTIONS FALSE

#ifndef __INSIDE_RTEMS_BSD_TCPIP_STACK__
#define __INSIDE_RTEMS_BSD_TCPIP_STACK__
#endif

#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include <xiltemac.h>
#include <rtems/irq.h>

/* Reading/Writing memory mapped i/o */
#define IN32(aPtr)                ((uint32_t)( *((volatile uint32_t *)(aPtr))) )
#define OUT32(aPtr, aValue)     (*((volatile uint32_t *)(aPtr)) = (uint32_t)aValue)
#define NUM_XILTEMAC_UNITS 2

/* Why isn't this defined in stdio.h like it's supposed to be? */
extern int snprintf(char*, size_t, const char*, ...);

extern rtems_isr xilTemacIsr( void *handle );
extern void xilTemacIsrOn(const rtems_irq_connect_data *);
extern void xilTemacIsrOff(const rtems_irq_connect_data *);
extern int xilTemacIsrIsOn(const rtems_irq_connect_data *);

void xilTemacInit( void *voidptr );
void xilTemacReset(struct ifnet *ifp);
void xilTemacStop(struct ifnet *ifp);
void xilTemacSend(struct ifnet *ifp);
void xilTemacStart(struct ifnet *ifp);
void xilTemacSetMacAddress(struct ifnet *ifp, unsigned char* aAddr);
void xilTemacPrintStats(struct ifnet *ifp);

void xilTemacRxThread( void *ignore );
void xilTemacTxThread( void *ignore );

static struct XilTemac gXilTemac[ NUM_XILTEMAC_UNITS ];

static rtems_id       gXilRxThread = 0;
static rtems_id       gXilTxThread = 0;

/*
** Events, one per unit.  The event is sent to the rx task from the isr
** or from the stack to the tx task whenever a unit needs service.  The
** rx/tx tasks identify the requesting unit(s) by their particular
** events so only requesting units are serviced.
*/

static rtems_event_set gUnitSignals[ NUM_XILTEMAC_UNITS ]= { RTEMS_EVENT_1,
                                                            RTEMS_EVENT_2 };

uint32_t xilTemacTxFifoVacancyBytes(uint32_t aBaseAddr)
{
  uint32_t ipisr = IN32(aBaseAddr + XTE_IPISR_OFFSET);
  uint32_t bytes = 0;
  if(ipisr & XTE_IPXR_XMIT_LFIFO_FULL_MASK) {
    /* If there's no room in the transmit length fifo, then any room in the
     * data fifo is irrelevant, return 0 */
  } else {
    bytes = IN32(aBaseAddr + XTE_PFIFO_TX_VACANCY_OFFSET);
    bytes &= XTE_PFIFO_COUNT_MASK;
    bytes *= 8;
  }
  return bytes;
}

void xilTemacFifoRead64(uint32_t aBaseAddr, uint32_t* aBuf, uint32_t aBytes)
{
  uint32_t numqwords = aBytes / 8;
  uint32_t xtrabytes = aBytes % 8;
  uint32_t i;

  for(i = 0; i < numqwords; i++)
  {
    aBuf[ (i*2)   ] = IN32(aBaseAddr + XTE_PFIFO_RX_DATA_OFFSET);
    aBuf[ (i*2)+1 ] = IN32(aBaseAddr + XTE_PFIFO_RX_DATA_OFFSET + 4);
  }

  /* If there was a non qword sized read */
  if( xtrabytes != 0 )
  {
    uint32_t lastdwordMS = IN32(aBaseAddr + XTE_PFIFO_RX_DATA_OFFSET);
    uint32_t lastdwordLS = IN32(aBaseAddr + XTE_PFIFO_RX_DATA_OFFSET + 4);
    uint8_t* finalbytes = (uint8_t *)&aBuf[ (numqwords*2) ];
    uint8_t* ptr8;
    int32_t  offset = 0;

    ptr8 = (uint8_t *)&lastdwordMS;
    if( xtrabytes >= 4 )
    {
      finalbytes[ offset++ ] = ptr8[0];
      finalbytes[ offset++ ] = ptr8[1];
      finalbytes[ offset++ ] = ptr8[2];
      finalbytes[ offset++ ] = ptr8[3];

      xtrabytes -= 4;
      ptr8 = (uint8_t *)&lastdwordLS;
    }

    if( xtrabytes == 1 )
    {
      finalbytes[ offset++ ] = ptr8[0];
    }
    else if ( xtrabytes == 2 )
    {
      finalbytes[ offset++ ] = ptr8[0];
      finalbytes[ offset++ ] = ptr8[1];
    }
    else if ( xtrabytes == 3 )
    {
      finalbytes[ offset++ ] = ptr8[0];
      finalbytes[ offset++ ] = ptr8[1];
      finalbytes[ offset++ ] = ptr8[2];
    }
  }
}

void xilTemacFifoWrite64(uint32_t aBaseAddr, uint32_t* aBuf, uint32_t aBytes)
{
  uint32_t numqwords = aBytes / 8;
  uint32_t xtrabytes = aBytes % 8;
  uint32_t i;

  for(i = 0; i < numqwords; i++ ) {
    OUT32(aBaseAddr + XTE_PFIFO_TX_DATA_OFFSET    , aBuf[ (i*2)   ]);
    OUT32(aBaseAddr + XTE_PFIFO_TX_DATA_OFFSET + 4, aBuf[ (i*2)+1 ]);
  }

  /* If there was a non word sized write */
  if( xtrabytes != 0 ) {
    uint32_t lastdwordMS = 0;
    uint32_t lastdwordLS = 0;
    uint8_t* finalbytes = (uint8_t *)&aBuf[ (numqwords*2) ];
    uint8_t* ptr8;
    int32_t  offset = 0;

    ptr8 = (uint8_t *)&lastdwordMS;

    if( xtrabytes >= 4 ) {
      ptr8[0] = finalbytes[ offset++ ];
      ptr8[1] = finalbytes[ offset++ ];
      ptr8[2] = finalbytes[ offset++ ];
      ptr8[3] = finalbytes[ offset++ ];

      xtrabytes -= 4;

      ptr8 = (uint8_t *)&lastdwordLS;
    }

    if( xtrabytes == 1 ) {
      ptr8[0] = finalbytes[ offset++ ];
    }
    else if ( xtrabytes == 2 ) {
      ptr8[0] = finalbytes[ offset++ ];
      ptr8[1] = finalbytes[ offset++ ];
    }
    else if ( xtrabytes == 3 ) {
      ptr8[0] = finalbytes[ offset++ ];
      ptr8[1] = finalbytes[ offset++ ];
      ptr8[2] = finalbytes[ offset++ ];
    }

    OUT32(aBaseAddr + XTE_PFIFO_TX_DATA_OFFSET,     lastdwordMS);
    OUT32(aBaseAddr + XTE_PFIFO_TX_DATA_OFFSET + 4, lastdwordLS);
  }
}

void xilTemacStop(struct ifnet *ifp)
{
  struct XilTemac* xilTemac = ifp->if_softc;
  uint32_t base = xilTemac->iAddr;

  /* Disable ipif interrupts */
  OUT32(base + XTE_DGIE_OFFSET, 0);

  /* Disable the receiver */
  uint32_t rxc1 = IN32(base + XTE_ERXC1_OFFSET);
  rxc1 &= ~XTE_ERXC1_RXEN_MASK;
  OUT32(base + XTE_ERXC1_OFFSET, rxc1);

  /* If receiver was receiving a packet when we disabled it, it will be
   * rejected, clear appropriate status bit */
  uint32_t ipisr = IN32(base + XTE_IPISR_OFFSET);
  if( ipisr & XTE_IPXR_RECV_REJECT_MASK ) {
    OUT32(base + XTE_IPISR_OFFSET, XTE_IPXR_RECV_REJECT_MASK);
  }

#if PPC_HAS_CLASSIC_EXCEPTIONS
  if( xilTemac->iOldHandler )
  {
    opb_intc_set_vector( xilTemac->iOldHandler, xilTemac->iIsrVector, NULL );
    xilTemac->iOldHandler = 0;
  }
#else
  if( xilTemac->iOldHandler.name != 0)
  {
    BSP_install_rtems_irq_handler (&xilTemac->iOldHandler);
  }
#endif

  ifp->if_flags &= ~IFF_RUNNING;
}

void xilTemacStart(struct ifnet *ifp)
{
  if( (ifp->if_flags & IFF_RUNNING) == 0 )
  {
    struct XilTemac* xilTemac = ifp->if_softc;
    uint32_t base = xilTemac->iAddr;

    /* Reset plb temac */
    OUT32(base + XTE_DSR_OFFSET, XTE_DSR_RESET_MASK);
    /* Don't have usleep on rtems 4.6
    usleep(1);
    */
    /* @ fastest ppc clock of 500 MHz = 2ns clk */
    uint32_t i = 0;
    for( i = 0; i < 1 * 500; i++) {
    }

    /* Reset hard temac */
    OUT32(base + XTE_CR_OFFSET, XTE_CR_HTRST_MASK);
    /* Don't have usleep on rtems 4.6
    usleep(4);
    */
    for( i = 0; i < 4 * 500; i++) {
    }

    /* Disable the receiver -- no need to disable xmit as we control that ;) */
    uint32_t rxc1 = IN32(base + XTE_ERXC1_OFFSET);
    rxc1 &= ~XTE_ERXC1_RXEN_MASK;
    OUT32(base + XTE_ERXC1_OFFSET, rxc1);

    /* If receiver was receiving a packet when we disabled it, it will be
     * rejected, clear appropriate status bit */
    uint32_t ipisr = IN32(base + XTE_IPISR_OFFSET);
    if( ipisr & XTE_IPXR_RECV_REJECT_MASK ) {
      OUT32(base + XTE_IPISR_OFFSET, XTE_IPXR_RECV_REJECT_MASK);
    }

    /* Setup IPIF interrupt enables */
    uint32_t dier = XTE_DXR_CORE_MASK | XTE_DXR_DPTO_MASK | XTE_DXR_TERR_MASK;
    dier |= XTE_DXR_RECV_FIFO_MASK | XTE_DXR_SEND_FIFO_MASK;
    OUT32(base + XTE_DIER_OFFSET, dier);

    /* Set the mac address */
    xilTemacSetMacAddress( ifp, xilTemac->iArpcom.ac_enaddr);

    /* Set the link speed */
    uint32_t emcfg = IN32(base + XTE_ECFG_OFFSET);
    printk("xiltemacStart, default linkspeed: %08x\n", emcfg);
    emcfg = (emcfg & ~XTE_ECFG_LINKSPD_MASK) | XTE_ECFG_LINKSPD_100;
    OUT32(base + XTE_ECFG_OFFSET, emcfg);

    /* Set phy divisor and enable mdio.  For a plb bus freq of 150MHz (the
       maximum as of Virtex4 Fx), a divisor of 29 gives a mdio clk freq of
       2.5MHz (see Xilinx docs for equation), the maximum in the phy standard.
       For slower plb frequencies, slower mkdio clks will result.  They may not
       be optimal, but they should work.  */
    uint32_t divisor = 29;
    OUT32(base + XTE_EMC_OFFSET, divisor | XTE_EMC_MDIO_MASK);

#if PPC_HAS_CLASSIC_EXCEPTIONS /* old connect code */
    /* Connect isr vector */
    rtems_status_code   sc;
    extern rtems_isr xilTemacIsr( rtems_vector_number aVector );
    sc = opb_intc_set_vector( xilTemacIsr, xilTemac->iIsrVector, &xilTemac->iOldHandler );
    if( sc != RTEMS_SUCCESSFUL )
    {
      xilTemac->iOldHandler = 0;
      printk("%s: Could not set interrupt vector for interface '%s' opb_intc_set_vector ret: %d\n", DRIVER_PREFIX, xilTemac->iUnitName, sc );
      assert(0);
    }
#else
    {
      rtems_irq_connect_data IrqConnData;

      /*
       *get old irq handler
       */
      xilTemac->iOldHandler.name = xilTemac->iIsrVector;
      if (!BSP_get_current_rtems_irq_handler (&xilTemac->iOldHandler)) {
	xilTemac->iOldHandler.name = 0;
	printk("%s: Unable to detect previous Irq handler\n",DRIVER_PREFIX);
	rtems_fatal_error_occurred(1);
      }

      IrqConnData.on     = xilTemacIsrOn;
      IrqConnData.off    = xilTemacIsrOff;
      IrqConnData.isOn   = xilTemacIsrIsOn;
      IrqConnData.name   = xilTemac->iIsrVector;
      IrqConnData.hdl    = xilTemacIsr;
      IrqConnData.handle = xilTemac;

      if (!BSP_install_rtems_irq_handler (&IrqConnData)) {
	printk("%s: Unable to connect Irq handler\n",DRIVER_PREFIX);
	rtems_fatal_error_occurred(1);
      }
    }
#endif
    /* Enable promiscuous mode -- The temac only supports full duplex, which
       means we're plugged into a switch.  Thus promiscuous mode simply means
       we get all multicast addresses*/
    OUT32(base + XTE_EAFM_OFFSET, XTE_EAFM_EPPRM_MASK);

    /* Setup and enable receiver */
    rxc1 = XTE_ERXC1_RXFCS_MASK | XTE_ERXC1_RXEN_MASK | XTE_ERXC1_RXVLAN_MASK;
    OUT32(base + XTE_ERXC1_OFFSET, rxc1);

    /* Setup and enable transmitter */
    uint32_t txc = XTE_ETXC_TXEN_MASK | XTE_ETXC_TXVLAN_MASK;
    OUT32(base + XTE_ETXC_OFFSET, txc);

    /* Enable interrupts for temac */
    uint32_t ipier = IN32(base + XTE_IPIER_OFFSET);
    ipier |= (XTE_IPXR_XMIT_ERROR_MASK);
    ipier |= (XTE_IPXR_RECV_ERROR_MASK | XTE_IPXR_RECV_DONE_MASK);
    ipier |= (XTE_IPXR_AUTO_NEG_MASK);
    OUT32(base + XTE_IPIER_OFFSET, ipier);

    printk("%s: xiltemacStart, ipier: %08x\n",DRIVER_PREFIX, ipier);

    /* Enable device global interrutps */
    OUT32(base + XTE_DGIE_OFFSET, XTE_DGIE_ENABLE_MASK);
    ifp->if_flags |= IFF_RUNNING;
  }
}

void xilTemacInit( void *voidptr )
{
}

void xilTemacReset(struct ifnet *ifp)
{
   xilTemacStop( ifp );
   xilTemacStart( ifp );
}

void xilTemacSetMacAddress(struct ifnet *ifp, unsigned char* aAddr)
{
  struct XilTemac* xilTemac = ifp->if_softc;
  uint32_t base = xilTemac->iAddr;

  /* You can't change the mac address while the card is in operation */
  if( (ifp->if_flags & IFF_RUNNING) != 0 ) {
    printk("%s: attempted to change MAC while up, interface '%s'\n", DRIVER_PREFIX, xilTemac->iUnitName );
    assert(0);
  }
  uint32_t mac;
  mac  = aAddr[0] & 0x000000FF;
  mac |= aAddr[1] << 8;
  mac |= aAddr[2] << 16;
  mac |= aAddr[3] << 24;
  OUT32(base + XTE_EUAW0_OFFSET, mac);

  mac = IN32(base + XTE_EUAW1_OFFSET);
  mac &= ~XTE_EUAW1_MASK;
  mac |= aAddr[4] & 0x000000FF;
  mac |= aAddr[5] << 8;
  OUT32(base + XTE_EUAW1_OFFSET, mac);
}

void xilTemacPrintStats( struct ifnet *ifp )
{
   struct XilTemac* xilTemac = ifp->if_softc;

   printf("\n");
   printf("%s: Statistics for interface '%s'\n", DRIVER_PREFIX, xilTemac->iUnitName );

   printf("%s:        Ipif Interrupts: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iInterrupts);
   printf("%s:          Rx Interrupts: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxInterrupts);
   printf("%s: Rx Rejected Interrupts: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxRejectedInterrupts);
   printf("%s:   Rx Rej Invalid Frame: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxRejectedInvalidFrame);
   printf("%s:  Rx Rej Data Fifo Full: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxRejectedDataFifoFull);
   printf("%s:Rx Rej Length Fifo Full: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxRejectedLengthFifoFull);
   printf("%s:        Rx Stray Events: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxStrayEvents);
   printf("%s:         Rx Max Drained: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iRxMaxDrained);
   printf("%s:          Tx Interrupts: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iTxInterrupts);
   printf("%s:         Tx Max Drained: %lu\n", DRIVER_PREFIX, xilTemac->iStats.iTxMaxDrained);

   printf("\n");
}

void xilTemacIsrSingle(struct XilTemac* xilTemac)
{
  uint32_t base = xilTemac->iAddr;
  uint32_t disr = IN32( base + XTE_DISR_OFFSET );
  struct ifnet* ifp = xilTemac->iIfp;

  if( disr && (ifp->if_flags & IFF_RUNNING) == 0 ) {
    /* some interrupt status bits are asserted but card is down */
    printk("%s: Fatal error, disr 0 or this emac not running\n", DRIVER_PREFIX);
    /*assert(0);*/
  } else {
    /* Handle all error conditions first */
    if( disr & (XTE_DXR_DPTO_MASK | XTE_DXR_TERR_MASK |
                XTE_DXR_RECV_FIFO_MASK | XTE_DXR_SEND_FIFO_MASK) ) {
      printk("%s: Fatal Bus error, disr: %08x\n", DRIVER_PREFIX, disr);
      /*assert(0);*/
    }
    if( disr & XTE_DXR_CORE_MASK ) {
      /* Normal case, temac interrupt */
      uint32_t ipisr = IN32(base + XTE_IPISR_OFFSET);
      uint32_t ipier = IN32(base + XTE_IPIER_OFFSET);
      uint32_t newipier = ipier;
      uint32_t pending = ipisr & ipier;
      xilTemac->iStats.iInterrupts++;

      /* Check for all fatal errors, even if that error is not enabled in ipier */
      if(ipisr & XTE_IPXR_FIFO_FATAL_ERROR_MASK) {
        printk("%s: Fatal Fifo Error ipisr: %08x\n", DRIVER_PREFIX, ipisr);
        /*assert(0);*/
      }

      if(pending & XTE_IPXR_RECV_DONE_MASK) {
        /* We've received a packet
           - inc stats
           - disable rx interrupt
           - signal rx thread to empty out fifo
             (rx thread must renable interrupt)
        */
        xilTemac->iStats.iRxInterrupts++;

        newipier &= ~XTE_IPXR_RECV_DONE_MASK;

        rtems_event_send(gXilRxThread, xilTemac->iIoEvent);
      }
      if(pending & XTE_IPXR_XMIT_DONE_MASK) {
        /* We've transmitted a packet.  This interrupt is only ever enabled in
           the ipier if the tx thread didn't have enough space in the data fifo
           or the tplr fifo.  If that's the case, we:
           - inc stats
           - disable tx interrupt
           - signal tx thread that a transmit has completed and thus there is now
             room to send again.
        */
        xilTemac->iStats.iTxInterrupts++;

        newipier &= ~XTE_IPXR_XMIT_DONE_MASK;

        rtems_event_send(gXilTxThread, xilTemac->iIoEvent);
      }
      if(pending & XTE_IPXR_RECV_DROPPED_MASK) {
        /* A packet was dropped (because it was invalid, or receiving it
           have overflowed one of the rx fifo's).
           - Increment stats.
           - Clear interrupt condition.
        */
        uint32_t toggle = 0;
        if(pending & XTE_IPXR_RECV_REJECT_MASK) {
          xilTemac->iStats.iRxRejectedInvalidFrame++;
          toggle |= XTE_IPXR_RECV_REJECT_MASK;
        }
        if(pending & XTE_IPXR_RECV_PFIFO_ABORT_MASK) {
          xilTemac->iStats.iRxRejectedDataFifoFull++;
          toggle |= XTE_IPXR_RECV_PFIFO_ABORT_MASK;
        }
        if(pending & XTE_IPXR_RECV_LFIFO_ABORT_MASK) {
          xilTemac->iStats.iRxRejectedLengthFifoFull++;
          toggle |= XTE_IPXR_RECV_LFIFO_ABORT_MASK;
        }
        xilTemac->iStats.iRxRejectedInterrupts++;
        OUT32(base + XTE_IPISR_OFFSET, toggle);
      }
      if(pending & XTE_IPXR_AUTO_NEG_MASK) {
        printk("%s: Autonegotiation finished\n", DRIVER_PREFIX);
        OUT32(base + XTE_IPISR_OFFSET, XTE_IPXR_AUTO_NEG_MASK);
      }
      if(newipier != ipier) {
        OUT32(base + XTE_IPIER_OFFSET, newipier);
      }
    }
  }
}

#if PPC_HAS_CLASSIC_EXCEPTIONS
rtems_isr xilTemacIsr( rtems_vector_number aVector )
{
  struct XilTemac* xilTemac;
  int              i;

  for( i=0; i< NUM_XILTEMAC_UNITS; i++ ) {
    xilTemac = &gXilTemac[i];

    if( xilTemac->iIsPresent ) {
      xilTemacIsrSingle(xilTemac);
    }
  }
}
#else
rtems_isr xilTemacIsr(void *handle )
{
  struct XilTemac* xilTemac = (struct XilTemac*)handle;

  xilTemacIsrSingle(xilTemac);
}

void xilTemacIsrOn(const rtems_irq_connect_data *unused)
{
}

void xilTemacIsrOff(const rtems_irq_connect_data *unused)
{
}

int xilTemacIsrIsOn(const rtems_irq_connect_data *unused)
{
  return 1;
}
#endif


int32_t xilTemacSetMulticastFilter(struct ifnet *ifp)
{
  return 0;
}

int xilTemacIoctl(struct ifnet* ifp, ioctl_command_t   aCommand, caddr_t aData)
{
  struct XilTemac* xilTemac = ifp->if_softc;
  int32_t error = 0;

  switch(aCommand) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, aCommand, aData);
      break;

    case SIOCSIFFLAGS:
      switch(ifp->if_flags & (IFF_UP | IFF_RUNNING))
      {
        case IFF_RUNNING:
          xilTemacStop(ifp);
          break;

        case IFF_UP:
          xilTemacStart(ifp);
          break;

        case IFF_UP | IFF_RUNNING:
          xilTemacReset(ifp);
          break;

        default:
          break;
      }
      break;

    case SIOCADDMULTI:
    case SIOCDELMULTI: {
        struct ifreq* ifr = (struct ifreq*) aData;
        error = ((aCommand == SIOCADDMULTI) ?
                 ( ether_addmulti(ifr, &(xilTemac->iArpcom)) ) :
                 ( ether_delmulti(ifr, &(xilTemac->iArpcom)))
           );
        /* ENETRESET indicates that driver should update its multicast filters */
        if(error == ENETRESET)
        {
            error = xilTemacSetMulticastFilter( ifp );
        }
        break;
    }

    case SIO_RTEMS_SHOW_STATS:
      xilTemacPrintStats( ifp );
      break;

    default:
      error = EINVAL;
      break;
  }
  return error;
}

void xilTemacSend(struct ifnet* ifp)
{
  struct XilTemac* xilTemac = ifp->if_softc;

  /* wake up tx thread w/ outbound interface's signal */
  rtems_event_send( gXilTxThread, xilTemac->iIoEvent );

  ifp->if_flags |= IFF_OACTIVE;
}

/* align the tx buffer to 32 bytes just for kicks, should make it more
 * cache friendly */
static unsigned char gTxBuf[2048] __attribute__ ((aligned (32)));

void xilTemacSendPacket(struct ifnet *ifp, struct mbuf* aMbuf)
{
  struct XilTemac  *xilTemac = ifp->if_softc;
  struct mbuf     *n = aMbuf;
  uint32_t        len = 0;

#ifdef DEBUG
  printk("SendPacket\n");
  printk("TXD: 0x%08x\n", (int32_t) n->m_data);
#endif

  /* assemble the packet into the tx buffer */
  for(;;) {
#ifdef DEBUG
    uint32_t i = 0;
    printk("MBUF: 0x%08x : ", (int32_t) n->m_data);
    for (i=0;i<n->m_len;i+=2) {
      printk("%02x%02x ", mtod(n, unsigned char*)[i], mtod(n, unsigned char*)[i+1]);
    }
    printk("\n");
#endif

    if( n->m_len > 0 ) {
      memcpy( &gTxBuf[ len ], (char *)n->m_data, n->m_len);
      len += n->m_len;
    }
    if( (n = n->m_next) == 0) {
      break;
    }
  }

  xilTemacFifoWrite64( xilTemac->iAddr, (uint32_t*)gTxBuf, len );
  /* Set the Transmit Packet Length Register which registers the packet
  * length, enqueues the packet and signals the xmit unit to start
  * sending. */
  OUT32(xilTemac->iAddr + XTE_TPLR_OFFSET, len);

#ifdef DEBUG
  printk("%s: txpkt, len %d\n", DRIVER_PREFIX, len );
  memset(gTxBuf, 0, len);
#endif
}

void xilTemacTxThreadSingle(struct ifnet* ifp)
{
  struct XilTemac* xilTemac = ifp->if_softc;
  struct mbuf*     m;
  uint32_t base = xilTemac->iAddr;

#ifdef DEBUG
  printk("%s: tx send packet, interface '%s'\n", DRIVER_PREFIX, xilTemac->iUnitName );
#endif

  /* Send packets till mbuf queue empty or tx fifo full */
  for(;;) {
    uint32_t i = 0;

    /* 1) clear out any statuses from previously sent tx frames */
    while( IN32(base + XTE_IPISR_OFFSET) & XTE_IPXR_XMIT_DONE_MASK ) {
      IN32(base + XTE_TSR_OFFSET);
      OUT32(base + XTE_IPISR_OFFSET, XTE_IPXR_XMIT_DONE_MASK);
      i++;
    }
    if( i > xilTemac->iStats.iTxMaxDrained ) {
      xilTemac->iStats.iTxMaxDrained = i;
    }

    /* 2) Check if enough space in tx data fifo _and_ tx tplr for an entire
       ethernet frame */
    if( xilTemacTxFifoVacancyBytes( xilTemac->iAddr ) <= ifp->if_mtu ) {
      /* 2a) If not, enable transmit done interrupt and break out of loop to
         wait for space */
      uint32_t ipier = IN32(base + XTE_IPIER_OFFSET);
      ipier |= (XTE_IPXR_XMIT_DONE_MASK);
      OUT32(base + XTE_IPIER_OFFSET, ipier);
      break;
    }

    /* 3) Contuine to dequeue mbuf chains till none left */
    IF_DEQUEUE( &(ifp->if_snd), m);
    if( !m ) {
      break;
    }

    /* 4) Send dequeued mbuf chain */
    xilTemacSendPacket( ifp, m );

    /* 5) Free mbuf chain */
    m_freem( m );
  }
  ifp->if_flags &= ~IFF_OACTIVE;
}

void xilTemacTxThread( void *ignore )
{
  struct XilTemac  *xilTemac;
  struct ifnet     *ifp;

  rtems_event_set  events;
  int              i;

  for(;;) {
    /* Wait for:
       - notification from stack of packet to send OR
       - notification from interrupt handler that there is space available to
         send already queued packets
    */
    rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                RTEMS_EVENT_ANY | RTEMS_WAIT,
                                RTEMS_NO_TIMEOUT,
                                &events );

    for(i=0; i< NUM_XILTEMAC_UNITS; i++) {
      xilTemac = &gXilTemac[i];

      if( xilTemac->iIsPresent ) {
        ifp = xilTemac->iIfp;

        if( (ifp->if_flags & IFF_RUNNING) ) {

          if( events & xilTemac->iIoEvent ) {
            xilTemacTxThreadSingle(ifp);
          }

        } else {
          printk("%s: xilTemacTxThread: event received for device: %s, but device not active\n",
            DRIVER_PREFIX, xilTemac->iUnitName);
          assert(0);
        }
      }
    }
  }
}

void xilTemacRxThreadSingle(struct ifnet* ifp)
{
  struct XilTemac* xilTemac = ifp->if_softc;

  uint32_t npkts = 0;
#ifdef DEBUG
  printk("%s: rxthread, packet rx on interface %s\n", DRIVER_PREFIX, xilTemac->iUnitName );
#endif

  uint32_t base = xilTemac->iAddr;

  /* While RECV_DONE_MASK in ipisr stays set */
  while( IN32(base + XTE_IPISR_OFFSET) & XTE_IPXR_RECV_DONE_MASK ) {

    /* 1) Read the length of the packet */
    uint32_t bytes = IN32(base + XTE_RPLR_OFFSET);

    /* 2) Read the Read Status Register (which contains no information).  When
     * all of these in the fifo have been read, then XTE_IPXR_RECV_DONE_MASK
     * will stay turned off, after it's written to */
    IN32(base + XTE_RSR_OFFSET);
    npkts++;

    struct mbuf* m;
    struct ether_header* eh;

    /* 3) Get some memory from the ip stack to store the packet in */
    MGETHDR(m, M_WAIT, MT_DATA);
    MCLGET(m, M_WAIT);

    m->m_pkthdr.rcvif = ifp;

    /* 4) Copy the packet into the ip stack's memory */
    xilTemacFifoRead64( base, mtod(m, uint32_t*), bytes);

    m->m_len = bytes - sizeof(struct ether_header);
    m->m_pkthdr.len = bytes - sizeof(struct ether_header);

    eh = mtod(m, struct ether_header*);

    m->m_data += sizeof(struct ether_header);

    /* 5) Tell the ip stack about the received packet */
    ether_input(ifp, eh, m);

    /* 6) Try and turn off XTE_IPXR_RECV_DONE bit in the ipisr.  If there's
     * still more packets (ie RSR ! empty), then it will stay asserted.  If
     * there's no more packets, this will turn it off.
     */
    OUT32(base + XTE_IPISR_OFFSET, XTE_IPXR_RECV_DONE_MASK);
  }

  /* End) All Rx packets serviced, renable rx interrupt */
  uint32_t ipier = IN32(base + XTE_IPIER_OFFSET);
  ipier |= XTE_IPXR_RECV_DONE_MASK;
  OUT32(base + XTE_IPIER_OFFSET, ipier);

#ifdef DEBUG
  printk("%s: rxthread, retrieved %d packets\n", DRIVER_PREFIX, npkts );
#endif
  if(npkts > xilTemac->iStats.iRxMaxDrained) {
    xilTemac->iStats.iRxMaxDrained = npkts;
  }
  /* ??) Very very occasionally, under extremely high stress, I get a situation
   * where we process no packets.  That is, the rx thread was evented, but
   * there was no packet available.  I'm not sure how this happens.  Ideally,
   * it shouldn't ocurr, and I suspect a minor bug in the driver.  However, for
   * me it's happenning 3 times in several hunderd million interrupts.  Nothing
   * bad happens, as long as we don't read from the rx fifo's if nothing is
   * there.  It is just not as efficient as possible (rx thread being evented
   * pointlessly) and a bit disconcerting about how it's ocurring.
   * The best way to reproduce this is to have two clients run:
   * $ ping <host> -f -s 65507
   * This flood pings the device from two clients with the maximum size ping
   * packet.  It absolutely hammers the device under test.  Eventually, (if
   * you leave it running overnight for instance), you'll get a couple of these
   * stray rx events. */
  if(npkts == 0) {
    /*printk("%s: RxThreadSingle: fatal error: event received, but no packets available\n", DRIVER_PREFIX);
    assert(0); */
    xilTemac->iStats.iRxStrayEvents++;
  }
}

void xilTemacRxThread( void *ignore )
{
  struct XilTemac* xilTemac;
  struct ifnet*   ifp;
  int             i;
  rtems_event_set events;

#ifdef DEBUG
  printk("%s: xilTemacRxThread running\n", DRIVER_PREFIX );
#endif

  for(;;) {
    rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                RTEMS_WAIT | RTEMS_EVENT_ANY,
                                RTEMS_NO_TIMEOUT,
                                &events);

#ifdef DEBUG
    printk("%s: rxthread, wakeup\n", DRIVER_PREFIX );
#endif

    for(i=0; i< NUM_XILTEMAC_UNITS; i++) {
      xilTemac = &gXilTemac[i];

      if( xilTemac->iIsPresent ) {
        ifp = xilTemac->iIfp;

        if( (ifp->if_flags & IFF_RUNNING) != 0 ) {
          if( events & xilTemac->iIoEvent ) {
            xilTemacRxThreadSingle(ifp);
          }
        }
        else {
          printk("%s: rxthread, interface %s present but not running\n", DRIVER_PREFIX, xilTemac->iUnitName );
          assert(0);
        }
      }
    }
  }
}

int32_t xilTemac_driver_attach(struct rtems_bsdnet_ifconfig* aBsdConfig, int aDummy)
{
   struct ifnet*    ifp;
   int32_t          mtu;
   int32_t          unit;
   char*            unitName;
   struct XilTemac* xilTemac;

   unit = rtems_bsdnet_parse_driver_name(aBsdConfig, &unitName);
   if(unit < 0 )
   {
      printk("%s: Interface Unit number < 0\n", DRIVER_PREFIX );
      return 0;
   }

   if( aBsdConfig->bpar == 0 )
   {
      printk("%s: Did not specify base address for device '%s'", DRIVER_PREFIX, unitName );
      return 0;
   }

   if( aBsdConfig->hardware_address == NULL )
   {
      printk("%s: No MAC address given for interface '%s'\n", DRIVER_PREFIX, unitName );
      return 0;
   }

   xilTemac = &gXilTemac[ unit ];
   memset(xilTemac, 0, sizeof(struct XilTemac));

   xilTemac->iIsPresent = 1;

   snprintf( xilTemac->iUnitName, MAX_UNIT_BYTES, "%s%" PRId32, unitName, unit );

   xilTemac->iIfp        = &(xilTemac->iArpcom.ac_if);
   ifp                  = &(xilTemac->iArpcom.ac_if);
   xilTemac->iAddr      = aBsdConfig->bpar;
   xilTemac->iIoEvent   = gUnitSignals[ unit ];
   xilTemac->iIsrVector = aBsdConfig->irno;

   memcpy( xilTemac->iArpcom.ac_enaddr, aBsdConfig->hardware_address, ETHER_ADDR_LEN);

   if( aBsdConfig->mtu )
   {
      mtu = aBsdConfig->mtu;
   }
   else
   {
      mtu = ETHERMTU;
   }

   ifp->if_softc  = xilTemac;
   ifp->if_unit   = unit;
   ifp->if_name   = unitName;
   ifp->if_mtu    = mtu;
   ifp->if_init   = xilTemacInit;
   ifp->if_ioctl  = xilTemacIoctl;
   ifp->if_start  = xilTemacSend;
   ifp->if_output = ether_output;

   ifp->if_flags  =  IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;

   if(ifp->if_snd.ifq_maxlen == 0)
   {
      ifp->if_snd.ifq_maxlen = ifqmaxlen;
   }

   if_attach(ifp);
   ether_ifattach(ifp);

   /* create shared rx & tx threads */
   if( (gXilRxThread == 0) && (gXilTxThread == 0) )
   {
      printk("%s: Creating shared RX/TX threads\n", DRIVER_PREFIX );
      gXilRxThread = rtems_bsdnet_newproc("xerx", 4096, xilTemacRxThread, NULL );
      gXilTxThread = rtems_bsdnet_newproc("xetx", 4096, xilTemacTxThread, NULL );
   }

   printk("%s: Initializing driver for '%s'\n", DRIVER_PREFIX, xilTemac->iUnitName );

   printk("%s: base address 0x%08X, intnum 0x%02X, \n",
          DRIVER_PREFIX,
          aBsdConfig->bpar,
          aBsdConfig->irno );

   return 1;
}

