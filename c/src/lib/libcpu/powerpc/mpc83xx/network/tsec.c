/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the MPC83xx TSEC networking driver           |
\*===============================================================*/

#include <stdlib.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/tsec.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>
#include <errno.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <stdio.h>

#define CLREVENT_IN_IRQ

#define TSEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */

#ifdef DEBUG
  #define PF(fmt, ...) printk("%s: " fmt, __func__, ##__VA_ARGS__)
#else
  #define PF(...)
#endif

/*
 * Device data
 */
struct tsec_struct {
  struct arpcom           arpcom;
  int                     acceptBroadcast;

  /*
   * HW links: (filled from rtems_bsdnet_ifconfig
   */
  volatile tsec_registers *reg_ptr;    /* pointer to TSEC register block */
  volatile tsec_registers *mdio_ptr;   /* pointer to TSEC register block which is responsible for MDIO communication */
  rtems_irq_number irq_num_tx;
  rtems_irq_number irq_num_rx;
  rtems_irq_number irq_num_err;

  /*
   * BD management
   */
  int                     rxBdCount;
  int                     txBdCount;
  PQBufferDescriptor_t    *Rx_Frst_BD;
  PQBufferDescriptor_t    *Rx_Last_BD;
  PQBufferDescriptor_t    *Rx_NxtUsed_BD; /* First BD, which is in Use */
  PQBufferDescriptor_t    *Rx_NxtFill_BD; /* BD to be filled next      */
  struct mbuf             **Rx_mBuf_Ptr;  /* Storage for mbufs         */

  PQBufferDescriptor_t    *Tx_Frst_BD;
  PQBufferDescriptor_t    *Tx_Last_BD;
  PQBufferDescriptor_t    *Tx_NxtUsed_BD; /* First BD, which is in Use */
  PQBufferDescriptor_t    *Tx_NxtFill_BD; /* BD to be filled next      */
  struct mbuf             **Tx_mBuf_Ptr;  /* Storage for mbufs         */
  /*
   * Daemon IDs
   */
  rtems_id                rxDaemonTid;
  rtems_id                txDaemonTid;

  /*
   * MDIO/Phy info
   */
  struct rtems_mdio_info mdio_info;
  int phy_default;
  int media_state; /* (last detected) state of media */
  /*
   * statistic counters Rx
   */
  unsigned long           rxInterrupts;
  unsigned long           rxErrors;
  /*
   * statistic counters Tx
   */
  unsigned long           txInterrupts;
  unsigned long           txErrors;
  };

static struct tsec_struct tsec_driver[TSEC_COUNT];

/*
 * default numbers for buffers
 */
#define RX_BUF_COUNT 64
#define TX_BUF_COUNT 64

/*
 * mask for all Tx interrupts
 */
#define IEVENT_TXALL (TSEC_IEVENT_GTSC	\
			    | TSEC_IEVENT_TXC 	\
			    /*| TSEC_IEVENT_TXB*/	\
			    | TSEC_IEVENT_TXF )

/*
 * mask for all Rx interrupts
 */
#define IEVENT_RXALL (TSEC_IEVENT_RXC  	\
			    /* | TSEC_IEVENT_RXB */	\
			    | TSEC_IEVENT_GRSC 	\
			    | TSEC_IEVENT_RXF  )

/*
 * mask for all Error interrupts
 */
#define IEVENT_ERRALL (TSEC_IEVENT_BABR   		\
			    | TSEC_IEVENT_BSY    		\
			    | TSEC_IEVENT_EBERR  		\
			    | TSEC_IEVENT_MSRO   		\
			    | TSEC_IEVENT_BABT   		\
			    | TSEC_IEVENT_TXE    		\
			    | TSEC_IEVENT_LC     		\
			    | TSEC_IEVENT_CRL_XDA		\
			    | TSEC_IEVENT_XFUN   )

#define TSEC_IMASK_SET(reg,mask,val) {	\
  rtems_interrupt_level level;			\
  						\
  rtems_interrupt_disable(level);		\
  (reg) = (((reg) & ~(mask)) |			\
	   ((val) &  (mask)));			\
  rtems_interrupt_enable(level);		\
}

#define TSEC_ALIGN_BUFFER(buf,align)		\
  ((void *)( (((uint32_t)(buf))+(align)-1)		\
	     -(((uint32_t)(buf))+(align)-1)%align))

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1
#define FATAL_INT_EVENT RTEMS_EVENT_3

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

static int tsec_ioctl
(
 struct ifnet *ifp,                    /* interface information            */
 ioctl_command_t command,              /* ioctl command code               */
 caddr_t data                          /* optional data                    */
 );

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_hwinit
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize hardware register                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  volatile tsec_registers *reg_ptr = sc->reg_ptr; /* pointer to TSEC registers*/
  uint8_t *mac_addr;
  size_t i;

  /* Clear interrupt mask and all pending events */
  reg_ptr->imask = 0;
  reg_ptr->ievent = 0xffffffff;

  /*
   * init ECNTL register
   * - clear statistics counters
   * - enable statistics
   * NOTE: do not clear bits set in BSP init function
   */
  reg_ptr->ecntrl = ((reg_ptr->ecntrl & ~TSEC_ECNTRL_AUTOZ)
		     | TSEC_ECNTRL_CLRCNT
		     | TSEC_ECNTRL_STEN
		     | TSEC_ECNTRL_R100M);

  /*
   * init DMA control register:
   * - enable snooping
   * - write BD status before interrupt request
   * - do not poll TxBD, but wait for TSTAT[THLT] to be written
   */
  reg_ptr->dmactrl = (TSEC_DMACTL_TDSEN
		      | TSEC_DMACTL_TBDSEN
		      | TSEC_DMACTL_WWR
		      | TSEC_DMACTL_WOP);

  /*
   * init Attribute register:
   * - enable read snooping for data and BD
   */
  reg_ptr->attr = (TSEC_ATTR_RDSEN
		   | TSEC_ATTR_RBDSEN);


  reg_ptr->mrblr  = MCLBYTES-64; /* take care of buffer size lost
				  * due to alignment              */

  /*
   * init EDIS register: disable all error reportings
   */
  reg_ptr->edis = (TSEC_EDIS_BSYDIS    |
		   TSEC_EDIS_EBERRDIS  |
		   TSEC_EDIS_TXEDIS    |
		   TSEC_EDIS_LCDIS     |
		   TSEC_EDIS_CRLXDADIS |
		   TSEC_EDIS_FUNDIS);
  /*
   * init minimum frame length register
   */
  reg_ptr->minflr = 64;
  /*
   * init maximum frame length register
   */
  reg_ptr->maxfrm = 1536;
  /*
   * define physical address of TBI
   */
  reg_ptr->tbipa = 0x1e;
  /*
   * init transmit interrupt coalescing register
   */
  reg_ptr->txic = (TSEC_TXIC_ICEN
		   | TSEC_TXIC_ICFCT(2)
		   | TSEC_TXIC_ICTT(32));
  /*
   * init receive interrupt coalescing register
   */
#if 0
  reg_ptr->rxic = (TSEC_RXIC_ICEN
		   | TSEC_RXIC_ICFCT(2)
		   | TSEC_RXIC_ICTT(32));
#else
  reg_ptr->rxic = 0;
#endif
  /*
   * init MACCFG1 register
   */
  reg_ptr->maccfg1 = (TSEC_MACCFG1_RX_FLOW
		      | TSEC_MACCFG1_TX_FLOW);

  /*
   * init MACCFG2 register
   */
  reg_ptr->maccfg2 = ((reg_ptr->maccfg2 & TSEC_MACCFG2_IFMODE_MSK)
		      | TSEC_MACCFG2_IFMODE_BYT
		      | TSEC_MACCFG2_PRELEN( 7)
		      | TSEC_MACCFG2_FULLDUPLEX);

  /*
   * init station address register
   */
  mac_addr = sc->arpcom.ac_enaddr;

  reg_ptr->macstnaddr[0] = ((mac_addr[5] << 24)
			    | (mac_addr[4] << 16)
			    | (mac_addr[3] <<  8)
			    | (mac_addr[2] <<  0));
  reg_ptr->macstnaddr[1] = ((mac_addr[1] << 24)
			    | (mac_addr[0] << 16));
  /*
   * clear hash filters
   */
  for (i = 0;
       i < sizeof(reg_ptr->iaddr)/sizeof(reg_ptr->iaddr[0]);
       i++) {
    reg_ptr->iaddr[i] = 0;
  }
  for (i = 0;
       i < sizeof(reg_ptr->gaddr)/sizeof(reg_ptr->gaddr[0]);
       i++) {
    reg_ptr->gaddr[i] = 0;
  }
}

/***************************************************************************\
|  MII Management access functions                                          |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_mdio_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the MIIM interface                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  static const uint8_t divider [] = { 32, 32, 48, 64, 80, 112, 160, 224 };
  size_t n = sizeof(divider) / sizeof(divider [0]);
  size_t i = 0;
  uint32_t mii_clock = UINT32_MAX;
  uint32_t tsec_system_clock = BSP_bus_frequency / 2;

  /* Set TSEC registers for MDIO communication */

  /*
   * set clock divider
   */
  for (i = 0; i < n && mii_clock > 2500000; ++i) {
    mii_clock = tsec_system_clock / divider [i];
  }

  sc->mdio_ptr->miimcfg = i;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int tsec_mdio_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read register of a phy                                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int phy,                              /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned reg,                         /* register address                 */
 uint32_t *pval                        /* ptr to read buffer               */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if ok, else error                                                   |
\*=========================================================================*/
{
  struct tsec_struct *sc = uarg;/* control structure                */

  /* pointer to TSEC registers */
  volatile tsec_registers *reg_ptr = sc->mdio_ptr;
  PF("%u\n", reg);

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return EINVAL;
  }
  /*
   * set PHY/reg address
   */
  reg_ptr->miimadd = (TSEC_MIIMADD_PHY(phy)
		      | TSEC_MIIMADD_REGADDR(reg));
  /*
   * start read cycle
   */
  reg_ptr->miimcom = 0;
  reg_ptr->miimcom = TSEC_MIIMCOM_READ;

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(2);
  }  while (0 != (reg_ptr->miimind & TSEC_MIIMIND_BUSY));
  reg_ptr->miimcom = 0;
  /*
   * fetch read data, if available
   */
  if (pval != NULL) {
    *pval = reg_ptr->miimstat;
  }
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int tsec_mdio_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write register of a phy                                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int phy,                              /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned reg,                         /* register address                 */
 uint32_t val                          /* write value                      */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if ok, else error                                                   |
\*=========================================================================*/
{
  struct tsec_struct *sc = uarg;/* control structure                */

  /* pointer to TSEC registers */
  volatile tsec_registers *reg_ptr = sc->mdio_ptr;
  PF("%u\n", reg);

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    /*
     * set default phy number: 0 for TSEC1, 1 for TSEC2
     */
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return EINVAL;
  }
  /*
   * set PHY/reg address
   */
  reg_ptr->miimadd = (TSEC_MIIMADD_PHY(phy)
		      | TSEC_MIIMADD_REGADDR(reg));
  /*
   * start write cycle
   */
  reg_ptr->miimcon = val;

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(2);
  }  while (0 != (reg_ptr->miimind & TSEC_MIIMIND_BUSY));
  reg_ptr->miimcom = 0;
  return 0;
}


/***************************************************************************\
|  RX receive functions                                                     |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_event_set tsec_rx_wait_for_events
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all rx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc,       /* control structure                */
 rtems_event_set event_mask            /* events to wait for               */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    event set received                                                     |
\*=========================================================================*/
{
 rtems_event_set events;            /* events received                     */
 /*
  * enable Rx interrupts, make sure this is not interrupted :-)
  */
 TSEC_IMASK_SET(sc->reg_ptr->imask,IEVENT_RXALL,~0);

 /*
  * wait for events to come in
  */
 rtems_bsdnet_event_receive(event_mask,
			    RTEMS_EVENT_ANY | RTEMS_WAIT,
			    RTEMS_NO_TIMEOUT,
			    &events);
 return events;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_rxbd_alloc_clear
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   allocate space for Rx BDs, clear them                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  char *alloc_ptr;
  PQBufferDescriptor_t *BD_ptr;
  /*
   * allocate proper space for Rx BDs
   */
  alloc_ptr = calloc((sc->rxBdCount+1),sizeof(PQBufferDescriptor_t));
  if (alloc_ptr == NULL) {
    rtems_panic("TSEC: cannot allocate space for Rx BDs");
  }
  alloc_ptr = (void *)((uint32_t )((alloc_ptr + (sizeof(PQBufferDescriptor_t)-1)))
		       & ~(sizeof(PQBufferDescriptor_t)-1));
  /*
   * store pointers to certain positions in BD chain
   */
  sc->Rx_Last_BD = ((PQBufferDescriptor_t *)alloc_ptr)+sc->rxBdCount-1;
  sc->Rx_Frst_BD = (PQBufferDescriptor_t *)alloc_ptr;
  sc->Rx_NxtUsed_BD = sc->Rx_Frst_BD;
  sc->Rx_NxtFill_BD = sc->Rx_Frst_BD;

  /*
   * clear all BDs
   */
  for (BD_ptr  = sc->Rx_Frst_BD;
       BD_ptr <= sc->Rx_Last_BD;
       BD_ptr++) {
    BD_ptr->status = 0;
  }
  /*
   * Init BD chain registers
   */
  sc->reg_ptr->rbase = (uint32_t) (sc->Rx_Frst_BD);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_receive_packets
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process any received packets                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  PQBufferDescriptor_t *BD_ptr;
  struct mbuf *m,*n;
  bool finished = false;
  uint16_t status;
  struct ether_header *eh;
  int bd_idx;

  BD_ptr = sc->Rx_NxtUsed_BD;

  while ((0 == ((status = BD_ptr->status) & BD_EMPTY)) &&
	 !finished &&
	 (BD_ptr->buffer != NULL)) {
    /*
     * get mbuf associated with BD
     */
    bd_idx = BD_ptr - sc->Rx_Frst_BD;
    m = sc->Rx_mBuf_Ptr[bd_idx];
    sc->Rx_mBuf_Ptr[bd_idx] = NULL;

    /*
     * Check that packet is valid
     */
    if ((status & (BD_LAST |
		   BD_FIRST_IN_FRAME |
		   BD_LONG |
		   BD_NONALIGNED |
		   BD_CRC_ERROR |
		   BD_OVERRUN ))
	== (BD_LAST |
	    BD_FIRST_IN_FRAME ) ) {
      /*
       * send mbuf of this buffer to ether_input()
       */
      m->m_len   = m->m_pkthdr.len = (BD_ptr->length
				    - sizeof(uint32_t)
				    - sizeof(struct ether_header));
      eh         = mtod(m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      PF("RX[%08x] (%i)\n", BD_ptr, m->m_len);
      ether_input(&sc->arpcom.ac_if,eh,m);
    }
    else {
      /*
       * throw away mbuf
       */
      MFREE(m,n);
    }
    /*
     * mark buffer as non-allocated (for refill)
     */
    BD_ptr->buffer = NULL;
    /*
     * Advance BD_ptr to next BD
     */
    BD_ptr = ((BD_ptr == sc->Rx_Last_BD)
	     ? sc->Rx_Frst_BD
	     : BD_ptr+1);
  }
  sc->Rx_NxtUsed_BD = BD_ptr;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_refill_rxbds
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   link new buffers to rx BDs                                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  PQBufferDescriptor_t *BD_ptr;
  struct mbuf *m,*n;
  bool finished = false;
  int bd_idx;

  BD_ptr = sc->Rx_NxtFill_BD;
  while ((BD_ptr->buffer == NULL) &&
	 !finished) {
    /*
     * get new mbuf and attach a cluster
     */
    MGETHDR(m,M_DONTWAIT,MT_DATA);
    if (m != NULL) {
      MCLGET(m,M_DONTWAIT);
      if ((m->m_flags & M_EXT) == 0) {
	MFREE(m,n);
	m = NULL;
      }
    }
    if (m == NULL) {
      finished = true;
    }
    else {
      bd_idx = BD_ptr - sc->Rx_Frst_BD;
      sc->Rx_mBuf_Ptr[bd_idx] = m;

      m->m_pkthdr.rcvif= &sc->arpcom.ac_if;
      m->m_data        = TSEC_ALIGN_BUFFER(m->m_ext.ext_buf,64);
      BD_ptr->buffer   = m->m_data;
      BD_ptr->length   = 0;
      BD_ptr->status   = (BD_EMPTY
			  | BD_INTERRUPT
			  | ((BD_ptr == sc->Rx_Last_BD)
			     ? BD_WRAP
			     : 0));
      /*
       * Advance BD_ptr to next BD
       */
      BD_ptr = ((BD_ptr == sc->Rx_Last_BD)
		? sc->Rx_Frst_BD
		: BD_ptr+1);
    }
  }
  sc->Rx_NxtFill_BD = BD_ptr;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_rxDaemon
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all rx buffers and events                                        |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void * arg                            /* argument, is sc structure ptr    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)arg;
  bool finished = false;
  rtems_event_set events;
#if !defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif
  /*
   * enable Rx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 |= TSEC_MACCFG1_RXEN;
  while (!finished) {
    /*
     * fetch MBufs, associate them to RxBDs
     */
    tsec_refill_rxbds(sc);
    /*
     * wait for events to come in
     */
    events = tsec_rx_wait_for_events(sc,INTERRUPT_EVENT);
#if !defined(CLREVENT_IN_IRQ)
    /*
     * clear any pending RX events
     */
    irq_events = sc->reg_ptr->ievent & IEVENT_RXALL;
    sc->reg_ptr->ievent = irq_events;
#endif
    /*
     * fetch any completed buffers/packets received
     * and stuff them into the TCP/IP Stack
     */
    tsec_receive_packets(sc);
  }
  /*
   * disable Rx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 &= ~TSEC_MACCFG1_RXEN;
  /*
   * terminate daemon
   */
  sc->rxDaemonTid = 0;
  rtems_task_delete(RTEMS_SELF);
}

/***************************************************************************\
|  TX Transmit functions                                                    |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_txbd_alloc_clear
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   allocate space for Tx BDs, clear them                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  char *alloc_ptr;
  PQBufferDescriptor_t *BD_ptr;
  /*
   * allocate proper space for Tx BDs
   */
  alloc_ptr = calloc((sc->txBdCount+1),sizeof(PQBufferDescriptor_t));
  if (alloc_ptr == NULL) {
    rtems_panic("TSEC: cannot allocate space for Tx BDs");
  }
  alloc_ptr = (void *)((uint32_t )((alloc_ptr + (sizeof(PQBufferDescriptor_t)-1)))
		       & ~(sizeof(PQBufferDescriptor_t)-1));
  /*
   * store pointers to certain positions in BD chain
   */
  sc->Tx_Last_BD = ((PQBufferDescriptor_t *)alloc_ptr)+sc->txBdCount-1;
  sc->Tx_Frst_BD = (PQBufferDescriptor_t *)alloc_ptr;
  sc->Tx_NxtUsed_BD = sc->Tx_Frst_BD;
  sc->Tx_NxtFill_BD = sc->Tx_Frst_BD;

  /*
   * clear all BDs
   */
  for (BD_ptr  = sc->Tx_Frst_BD;
       BD_ptr <= sc->Tx_Last_BD;
       BD_ptr++) {
    BD_ptr->status = 0;
  }
  /*
   * Init BD chain registers
   */
  sc->reg_ptr->tbase = (uint32_t)(sc->Tx_Frst_BD);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_tx_start
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   start transmission                                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
struct ifnet *ifp
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_event_set tsec_tx_wait_for_events
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc,       /* control structure                */
 rtems_event_set event_mask            /* events to wait for               */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    event set received                                                     |
\*=========================================================================*/
{
 rtems_event_set events;            /* events received                     */
 /*
  * enable Tx interrupts, make sure this is not interrupted :-)
  */
 TSEC_IMASK_SET(sc->reg_ptr->imask,IEVENT_TXALL,~0);

 /*
  * wait for events to come in
  */
 rtems_bsdnet_event_receive(event_mask,
			    RTEMS_EVENT_ANY | RTEMS_WAIT,
			    RTEMS_NO_TIMEOUT,
			    &events);
 return events;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_tx_retire
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  PQBufferDescriptor_t *RetBD;
  RetBD = sc->Tx_NxtUsed_BD;
  int bd_idx;
  struct mbuf *m,*n;
  /*
   * check next BDs to be empty
   */
  while ((RetBD->buffer != NULL)                       /* BD is filled      */
	 && (0 == (RetBD->status & BD_READY ))) {/* BD no longer ready*/

    bd_idx = RetBD - sc->Tx_Frst_BD;
    m = sc->Tx_mBuf_Ptr[bd_idx];
    sc->Tx_mBuf_Ptr[bd_idx] = NULL;

    MFREE(m,n);
    RetBD->buffer = NULL;
    /*
     * Advance CurrBD to next BD
     */
    RetBD = ((RetBD == sc->Tx_Last_BD)
	     ? sc->Tx_Frst_BD
	     : RetBD+1);
  }
  sc->Tx_NxtUsed_BD = RetBD;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_sendpacket
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc,       /* control structure                */
 struct mbuf *m                        /* start of packet to send          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  PQBufferDescriptor_t *FrstBD = NULL;
  PQBufferDescriptor_t *CurrBD;
  uint16_t status;
  struct mbuf *l = NULL; /* ptr to last non-freed (non-empty) mbuf */
  int bd_idx;
  /*
   * get next Tx BD
   */
  CurrBD = sc->Tx_NxtFill_BD;
  while (m) {
    if(m->m_len == 0) {
      /*
       * Just toss empty mbufs
       */
      struct mbuf *n;
      MFREE(m, n);
      m = n;
      if(l != NULL) {
        l->m_next = m;
      }
    }
    else {
      /*
       * this mbuf is non-empty, so send it
       */
      /*
       * Is CurrBD still in Use/not yet retired?
       */
      while (CurrBD->buffer != NULL) {
	/*
	 * Then try to retire it
	 * and to return its mbuf
	 */
	tsec_tx_retire(sc);
	if (CurrBD->buffer != NULL) {
	  /*
	   * Wait for anything to happen...
	   */
	  tsec_tx_wait_for_events(sc,INTERRUPT_EVENT);
	}
      }
      status = ((BD_PAD_CRC | BD_TX_CRC)
		| ((m->m_next == NULL)
		   ? BD_LAST | BD_INTERRUPT
		   : 0)
		| ((CurrBD == sc->Tx_Last_BD) ? BD_WRAP : 0));

      /*
       * link buffer to BD
       */
      CurrBD->buffer = mtod(m, void *);
      CurrBD->length = (uint32_t)m->m_len;
      l = m;       /* remember: we use this mbuf          */
      PF("TX[%08x] (%i)\n", CurrBD, m->m_len);

      bd_idx = CurrBD - sc->Tx_Frst_BD;
      sc->Tx_mBuf_Ptr[bd_idx] = m;

      m = m->m_next; /* advance to next mbuf of this packet */
      /*
       * is this the first BD of the packet?
       * then don't set it to "READY" state,
       * and remember this BD position
       */
      if (FrstBD == NULL) {
	FrstBD = CurrBD;
      }
      else {
	status |= BD_READY;
      }
      CurrBD->status = status;
      /*
       * Advance CurrBD to next BD
       */
      CurrBD = ((CurrBD == sc->Tx_Last_BD)
		? sc->Tx_Frst_BD
		: CurrBD+1);
    }
  }
  /*
   * mbuf chain of this packet
   * has been translated
   * to BD chain, so set first BD ready now
   */
  if (FrstBD != NULL) {
    FrstBD->status |= BD_READY;
  }
  sc->Tx_NxtFill_BD = CurrBD;
  /*
   * wake up transmitter (clear TSTAT[THLT])
   */
  sc->reg_ptr->tstat = TSEC_TSTAT_THLT;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_txDaemon
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void * arg                            /* argument, is sc structure ptr    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  bool finished = false;
  rtems_event_set events;
#if !defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif

  /*
   * enable Tx in MACCFG1 register
   * FIXME: make this irq save
   */
  sc->reg_ptr->maccfg1 |= TSEC_MACCFG1_TXEN;
  while (!finished) {
    /*
     * wait for events to come in
     */
    events = tsec_tx_wait_for_events(sc,
					     START_TRANSMIT_EVENT
					     | INTERRUPT_EVENT);
#if !defined(CLREVENT_IN_IRQ)
    /*
     * clear any pending TX events
     */
    irq_events = sc->reg_ptr->ievent & IEVENT_TXALL;
    sc->reg_ptr->ievent = irq_events;
#endif
    /*
     * retire any sent tx BDs
     */
    tsec_tx_retire(sc);
    /*
     * Send packets till queue is empty
     */
    do {
      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);

      if (m) {
	tsec_sendpacket(sc,m);
      }
    } while (m != NULL);

    ifp->if_flags &= ~IFF_OACTIVE;
  }
  /*
   * disable Tx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 &= ~TSEC_MACCFG1_TXEN;
  /*
   * terminate daemon
   */
  sc->txDaemonTid = 0;
  rtems_task_delete(RTEMS_SELF);
}

/***************************************************************************\
|  Interrupt handlers and management routines                               |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_tx_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle tx interrupts                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_irq_hdl_param handle            /* handle, is sc structure ptr      */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)handle;
#if defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif

  PF("TXIRQ\n");
  sc->txInterrupts++;
  /*
   * disable tx interrupts
   */
  TSEC_IMASK_SET(sc->reg_ptr->imask,IEVENT_TXALL,0);

#if defined(CLREVENT_IN_IRQ)
  /*
   * clear any pending TX events
   */
  irq_events = sc->reg_ptr->ievent & IEVENT_TXALL;
  sc->reg_ptr->ievent = irq_events;
#endif
  /*
   * wake up tx Daemon
   */
  rtems_event_send(sc->txDaemonTid, INTERRUPT_EVENT);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_rx_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle rx interrupts                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_irq_hdl_param handle            /* handle, is sc structure          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)handle;
#if defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif

  sc->rxInterrupts++;
  PF("RXIRQ\n");
  /*
   * disable rx interrupts
   */
  TSEC_IMASK_SET(sc->reg_ptr->imask,IEVENT_RXALL,0);
#if defined(CLREVENT_IN_IRQ)
  /*
   * clear any pending RX events
   */
  irq_events = sc->reg_ptr->ievent & IEVENT_RXALL;
  sc->reg_ptr->ievent = irq_events;
#endif
  /*
   * wake up rx Daemon<
   */
  rtems_event_send(sc->rxDaemonTid, INTERRUPT_EVENT);
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_err_irq_handler
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle error interrupts                                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_irq_hdl_param handle            /* handle, is sc structure          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)handle;
  PF("ERIRQ\n");
  /*
   * clear error events in IEVENT
   */
  sc->reg_ptr->ievent = IEVENT_ERRALL;
  /*
   * has Rx been stopped? then restart it
   */
  if (0 != (sc->reg_ptr->rstat & TSEC_RSTAT_QHLT)) {
    sc->rxErrors++;
    sc->reg_ptr->rstat = TSEC_RSTAT_QHLT;
  }
  /*
   * has Tx been stopped? then restart it
   */
  if (0 != (sc->reg_ptr->tstat & TSEC_TSTAT_THLT)) {
    sc->txErrors++;
    sc->reg_ptr->tstat = TSEC_TSTAT_THLT;
  }
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static uint32_t tsec_irq_mask
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine irq mask for given interrupt number                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int irqnum,
 struct tsec_struct *sc
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    interrupt mask (for ievent/imask register)                             |
\*=========================================================================*/
{
  return ((irqnum == sc->irq_num_tx)
	  ? IEVENT_TXALL
	  : ((irqnum == sc->irq_num_rx)
	     ? IEVENT_RXALL
	     : ((irqnum == sc->irq_num_err)
		? IEVENT_ERRALL
		: 0)));
}
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_irq_on
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupts in TSEC mask register                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const
 rtems_irq_connect_data *irq_conn_data   /* irq connect data                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)(irq_conn_data->handle);

  TSEC_IMASK_SET(sc->reg_ptr->imask,
		       tsec_irq_mask(irq_conn_data->name,sc),
		       ~0);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_irq_off
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable TX interrupts in TSEC mask register                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const
 rtems_irq_connect_data *irq_conn_data   /* irq connect data                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)irq_conn_data->handle;

  TSEC_IMASK_SET(sc->reg_ptr->imask,
		       tsec_irq_mask(irq_conn_data->name,sc),
		       0);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int tsec_irq_isOn
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   check state of interrupts in TSEC mask register                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const
 rtems_irq_connect_data *irq_conn_data  /* irq connect data                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  struct tsec_struct *sc =
    (struct tsec_struct *)irq_conn_data->handle;

  return (0 != (sc->reg_ptr->imask
		& tsec_irq_mask(irq_conn_data->name,sc)));
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_install_irq_handlers
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   (un-)install the interrupt handlers                                     |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc,        /* ptr to control structure        */
 bool   install                         /* true: install, false: remove    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  size_t i;

  rtems_irq_connect_data irq_conn_data[3] = {
    {
      sc->irq_num_tx,
      tsec_tx_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,     /* (rtems_irq_hdl_param)   */
      tsec_irq_on,         /* (rtems_irq_enable)      */
      tsec_irq_off,        /* (rtems_irq_disable)     */
      tsec_irq_isOn        /* (rtems_irq_is_enabled)  */
    },{
      sc->irq_num_rx,
      tsec_rx_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,     /* (rtems_irq_hdl_param)   */
      tsec_irq_on,         /* (rtems_irq_enable)      */
      tsec_irq_off,        /* (rtems_irq_disable)     */
      tsec_irq_isOn        /* (rtems_irq_is_enabled)  */
    },{
      sc->irq_num_err,
      tsec_err_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,      /* (rtems_irq_hdl_param)   */
      tsec_irq_on,          /* (rtems_irq_enable)      */
      tsec_irq_off,         /* (rtems_irq_disable)     */
      tsec_irq_isOn         /* (rtems_irq_is_enabled)  */
    }
  };

  /*
   * (un-)install handler for Tx/Rx/Error
   */
  for (i = 0;
       i < sizeof(irq_conn_data)/sizeof(irq_conn_data[0]);
       i++) {
    if (install) {
      if (!BSP_install_rtems_irq_handler (&irq_conn_data[i])) {
	rtems_panic("TSEC: cannot install IRQ handler");
      }
    }
    else {
      if (!BSP_remove_rtems_irq_handler (&irq_conn_data[i])) {
	rtems_panic("TSEC: cannot uninstall IRQ handler");
      }
    }
  }
}

/***************************************************************************\
|  Initialization and interface routines                                    |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the driver and the hardware                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void *arg                              /* argument pointer, contains *sc  */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    zero, if success                                                       |
\*=========================================================================*/
{
  struct tsec_struct *sc = (struct tsec_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  /*
   * check, whether device is not yet running
   */
  if (0 == sc->rxDaemonTid) {
    /*
     * allocate rx/tx BDs
     */
    mpc83xx_rxbd_alloc_clear(sc);
    mpc83xx_txbd_alloc_clear(sc);
    /*
     * allocate storage for mbuf ptrs
     */
    sc->Rx_mBuf_Ptr = calloc(sc->rxBdCount,sizeof(struct mbuf *));
    sc->Tx_mBuf_Ptr = calloc(sc->txBdCount,sizeof(struct mbuf *));
    if ((sc->Rx_mBuf_Ptr == NULL) ||
	(sc->Tx_mBuf_Ptr == NULL)) {
	rtems_panic("TSEC: cannot allocate buffers for mbuf management");

    }

    /*
     * initialize TSEC hardware:
     * - set interrupt coalescing to BDCount/8, Time of 8 frames
     * - enable DMA snooping
     */
    tsec_hwinit(sc);
    /*
     * init access to phys
     */
    tsec_mdio_init(sc);
    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc("TStx",
					   4096,
					   tsec_txDaemon,
					   sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc("TSrx", 4096,
					   tsec_rxDaemon,
					   sc);
    /*
     * install interrupt handlers
     */
    tsec_install_irq_handlers(sc,true);
  }
  /*
   * Set flags appropriately
   */
  if(ifp->if_flags & IFF_PROMISC) {
    sc->reg_ptr->rctrl |=  TSEC_RCTRL_PROM;
  }
  else {
    sc->reg_ptr->rctrl &= ~TSEC_RCTRL_PROM;
  }

#if defined(MPC83XX_BOARD_HSC_CM01)
  /*
   * for HSC CM01: we need to configure the PHY to use maximum skew adjust
   */

  tsec_mdio_write(-1,sc,23,0x0100);
#endif

  /*
   * init timer so the "watchdog function gets called periodically
   */
  ifp->if_timer    = 1;
  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_off
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   deinitialize the driver and the hardware                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc         /* ptr to control structure        */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  /*
   * deinitialize driver?
   */
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_stats
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   print statistics                                                        |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct tsec_struct *sc         /* ptr to control structure        */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  if (sc->phy_default >= 0) {
    int media;
    int result;
    /*
     * fetch/print media info
     */
    media = IFM_MAKEWORD(0,0,0,sc->phy_default); /* fetch from default phy */
 
    result = tsec_ioctl(&(sc->arpcom.ac_if),
          		      SIOCGIFMEDIA,
          		      (caddr_t)&media);
    if (result == 0) {
      rtems_ifmedia2str(media,NULL,0);
      printf ("\n");
    } else {
      printf ("PHY communication error\n");
    }
  }
#if 0 /* print all PHY registers */
  {
    int reg;
    uint32_t reg_val;
    printf("****** PHY register values****\n");
    for (reg = 0;reg <= 31;reg++) {
      tsec_mdio_read(-1,sc,reg,&reg_val);
      printf("%02d:0x%04x%c",reg,reg_val,
	     (((reg % 4) == 3) ? '\n' : ' '));
    }
  }
#endif
  /*
   * print some statistics
   */
  printf ("   Rx Interrupts:%-8lu",   sc->rxInterrupts);
  printf ("       Rx Errors:%-8lu",   sc->rxErrors);
  printf ("      Rx packets:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RPKT]);
  printf ("   Rx broadcasts:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RBCA]);
  printf ("   Rx multicasts:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RMCA]);
  printf ("           Giant:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_ROVR]);
  printf ("       Non-octet:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RALN]);
  printf ("         Bad CRC:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RFCS]);
  printf ("         Overrun:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_RDRP]);

  printf ("   Tx Interrupts:%-8lu",   sc->txInterrupts);
  printf ("       Tx Errors:%-8lu",   sc->txErrors);
  printf ("      Tx packets:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_TPKT]);
  printf ("        Deferred:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_TDFR]);
  printf ("  Late Collision:%-8lu",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_TLCL]);
  printf ("Retransmit Limit:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_TEDF]);
  printf ("        Underrun:%-8lu\n",
	  sc->reg_ptr->rmon_mib[TSEC_RMON_TUND]);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int tsec_ioctl
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   perform io control functions                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct ifnet *ifp,                    /* interface information            */
 ioctl_command_t command,              /* ioctl command code               */
 caddr_t data                          /* optional data                    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    zero, if success                                                       |
\*=========================================================================*/
{
  struct tsec_struct *sc = ifp->if_softc;
  int error = 0;

  switch(command)  {
    /*
     * access PHY via MII
     */
  case SIOCGIFMEDIA:
  case SIOCSIFMEDIA:
    rtems_mii_ioctl (&(sc->mdio_info),sc,command,(void *)data);
    break;
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    /*
     * pass through to general ether_ioctl
     */
    ether_ioctl(ifp, command, data);
    break;

  case SIOCSIFFLAGS:
    /*
     * adjust active state
     */
    if (ifp->if_flags & IFF_RUNNING) {
      tsec_off(sc);
    }
    if (ifp->if_flags & IFF_UP) {
      tsec_init(sc);
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    /*
     * show interface statistics
     */
    tsec_stats(sc);
    break;

    /*
     * All sorts of multicast commands need to be added here!
     */
  default:
    error = EINVAL;
    break;
  }

  return error;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int tsec_mode_adapt
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   init the PHY and adapt TSEC settings                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct ifnet *ifp
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if success                                                       |
\*=========================================================================*/
{
  int result = 0;
  struct tsec_struct *sc = ifp->if_softc;
  int media = IFM_MAKEWORD( 0, 0, 0, sc->phy_default);

  /* In case no PHY is available stop now */
  if (sc->phy_default < 0) {
    return 0;
  }

  /*
   * fetch media status
   */
  result = tsec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
  if (result != 0) {
    return result;
  }

  /*
   * status is unchanged? then do nothing
   */
  if (media == sc->media_state) {
    return 0;
  }
  /*
   * otherwise: for the first call, try to negotiate mode
   */
  if (sc->media_state == 0) {
    /*
     * set media status: set auto negotiation -> start auto-negotiation
     */
    media = IFM_MAKEWORD(0,IFM_AUTO,0,sc->phy_default);
    result = tsec_ioctl(ifp,SIOCSIFMEDIA,(caddr_t)&media);
    if (result != 0) {
      return result;
    }
    /*
     * check auto-negotiation status
     */
    media = IFM_MAKEWORD(0,0,0,sc->phy_default);
    result = tsec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
    if (result != 0 || IFM_NONE == IFM_SUBTYPE(media)) {
      return result;
    }
  }

  /*
   * now set HW according to media results:
   */
  /*
   * if we are 1000MBit, then switch IF to byte mode
   */
  if (IFM_1000_T == IFM_SUBTYPE(media)) {
    sc->reg_ptr->maccfg2 =
      ((sc->reg_ptr->maccfg2 & ~TSEC_MACCFG2_IFMODE_MSK)
       | TSEC_MACCFG2_IFMODE_BYT);
  }
  else {
    sc->reg_ptr->maccfg2 =
      ((sc->reg_ptr->maccfg2 & ~TSEC_MACCFG2_IFMODE_MSK)
       | TSEC_MACCFG2_IFMODE_NIB);
  }
  /*
   * if we are 10MBit, then switch rate to 10M
   */
  if (IFM_10_T == IFM_SUBTYPE(media)) {
    sc->reg_ptr->ecntrl &= ~TSEC_ECNTRL_R100M;
  }
  else {
    sc->reg_ptr->ecntrl |= TSEC_ECNTRL_R100M;
  }
  /*
   * if we are half duplex then switch to half duplex
   */
  if (0 == (IFM_FDX & IFM_OPTIONS(media))) {
    sc->reg_ptr->maccfg2 &= ~TSEC_MACCFG2_FULLDUPLEX;
  }
  else {
    sc->reg_ptr->maccfg2 |=  TSEC_MACCFG2_FULLDUPLEX;
  }
  /*
   * store current media state for future compares
   */
  sc->media_state = media;

  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void tsec_watchdog
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   periodically poll the PHY. if mode has changed,                         |
|  then adjust the TSEC settings                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct ifnet *ifp
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    1, if success                                                       |
\*=========================================================================*/
{
  tsec_mode_adapt(ifp);
  ifp->if_timer    = TSEC_WATCHDOG_TIMEOUT;
}

static int tsec_driver_attach(struct rtems_bsdnet_ifconfig *config)
{
  tsec_config *tsec_cfg = config->drv_ctrl;
  int unitNumber = tsec_cfg->unit_number;
  char *unitName = tsec_cfg->unit_name;
  struct tsec_struct *sc;
  struct ifnet *ifp;

 /*
  * Is driver free?
  */
  if ((unitNumber <= 0) || (unitNumber > TSEC_COUNT)) {

    printk ("Bad TSEC unit number.\n");
    return 0;

    }

  sc = &tsec_driver[unitNumber - 1];
  ifp = &sc->arpcom.ac_if;

  if(ifp->if_softc != NULL) {
    printk ("Driver already in use.\n");
    return 0;
  }

  /*
   * Process options
   */
  if(config->hardware_address) {
    memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
      rtems_panic("TSEC: No Ethernet address specified!\n");
  }

  sc->rxBdCount       = (config->rbuf_count > 0) ? config->rbuf_count :  RX_BUF_COUNT;
  sc->txBdCount       = (config->xbuf_count > 0) ? config->xbuf_count :  TX_BUF_COUNT;
  sc->acceptBroadcast = !config->ignore_broadcast;

  /* get pointer to TSEC register block */
  sc->reg_ptr         = tsec_cfg->reg_ptr;
  sc->mdio_ptr        = tsec_cfg->mdio_ptr;

  /* IRQ numbers */
  sc->irq_num_tx      = tsec_cfg->irq_num_tx;
  sc->irq_num_rx      = tsec_cfg->irq_num_rx;
  sc->irq_num_err     = tsec_cfg->irq_num_err;

  /*
   * setup info about mdio interface
   */
  sc->mdio_info.mdio_r   = tsec_mdio_read;
  sc->mdio_info.mdio_w   = tsec_mdio_write;
  sc->mdio_info.has_gmii = 1; /* we support gigabit IF */

  /* PHY address */
  sc->phy_default = tsec_cfg->phy_default;

 /*
  * Set up network interface values
  */
  ifp->if_softc   = sc;
  ifp->if_unit    = unitNumber;
  ifp->if_name    = unitName;
  ifp->if_mtu     = (config->mtu > 0) ? config->mtu : ETHERMTU;
  ifp->if_init    = tsec_init;
  ifp->if_ioctl   = tsec_ioctl;
  ifp->if_start   = tsec_tx_start;
  ifp->if_output  = ether_output;
  ifp->if_watchdog =  tsec_watchdog; /* XXX: timer is set in "init" */

  ifp->if_flags   = (config->ignore_broadcast) ? 0 : IFF_BROADCAST;
  /*ifp->if_flags   = IFF_BROADCAST | IFF_SIMPLEX;*/

  if(ifp->if_snd.ifq_maxlen == 0) {
    ifp->if_snd.ifq_maxlen = ifqmaxlen;
  }

  /*
   * Attach the interface
   */
  if_attach(ifp);

  ether_ifattach(ifp);

  return 1;
}

int tsec_driver_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
)
{
  if (attaching) {
    return tsec_driver_attach(config);
  } else {
    return 0;
  }
}
