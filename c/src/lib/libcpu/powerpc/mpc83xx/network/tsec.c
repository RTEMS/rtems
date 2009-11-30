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
/*
 * this driver has the following HW assumptions:
 * - PHY for TSEC1 uses address 0
 * - PHY for TSEC2 uses address 1
 * - PHY uses GMII for 1000Base-T and MII for the rest of the modes
 */
#include <stdlib.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <mpc83xx/mpc83xx.h>
#include <mpc83xx/tsec.h>
#include <libcpu/spr.h>
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

/* System Version Register */
#define SVR 286
SPR_RO( SVR)

/* Processor Version Register */
SPR_RO( PVR)

#define CLREVENT_IN_IRQ

#define TSEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */

/*
 * Device data
 */
struct mpc83xx_tsec_struct {
  struct arpcom           arpcom;
  int                     acceptBroadcast;

  /*
   * HW links: (filled from rtems_bsdnet_ifconfig
   */
  m83xxTSEC_Registers_t  *reg_ptr;    /* pointer to TSEC register block */
  m83xxTSEC_Registers_t  *mdio_ptr;   /* pointer to TSEC register block which is responsible for MDIO communication */
  int                    irq_num_tx;  /* tx irq number                  */
  int                    irq_num_rx;  /* rx irq number                  */
  int                    irq_num_err; /* error irq number               */

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

static struct mpc83xx_tsec_struct tsec_driver[M83xx_TSEC_NIFACES];

/*
 * default numbers for buffers
 */
#define RX_BUF_COUNT 64
#define TX_BUF_COUNT 64

/*
 * mask for all Tx interrupts
 */
#define M83xx_IEVENT_TXALL (M83xx_TSEC_IEVENT_GTSC	\
			    | M83xx_TSEC_IEVENT_TXC 	\
			    /*| M83xx_TSEC_IEVENT_TXB*/	\
			    | M83xx_TSEC_IEVENT_TXF )

/*
 * mask for all Rx interrupts
 */
#define M83xx_IEVENT_RXALL (M83xx_TSEC_IEVENT_RXC  	\
			    /* | M83xx_TSEC_IEVENT_RXB */	\
			    | M83xx_TSEC_IEVENT_GRSC 	\
			    | M83xx_TSEC_IEVENT_RXF  )

/*
 * mask for all Error interrupts
 */
#define M83xx_IEVENT_ERRALL (M83xx_TSEC_IEVENT_BABR   		\
			    | M83xx_TSEC_IEVENT_BSY    		\
			    | M83xx_TSEC_IEVENT_EBERR  		\
			    | M83xx_TSEC_IEVENT_MSRO   		\
			    | M83xx_TSEC_IEVENT_BABT   		\
			    | M83xx_TSEC_IEVENT_TXE    		\
			    | M83xx_TSEC_IEVENT_LC     		\
			    | M83xx_TSEC_IEVENT_CRL_XDA		\
			    | M83xx_TSEC_IEVENT_XFUN   )

#define M83xx_TSEC_IMASK_SET(reg,mask,val) {	\
  rtems_interrupt_level level;			\
  						\
  rtems_interrupt_disable(level);		\
  (reg) = (((reg) & ~(mask)) |			\
	   ((val) &  (mask)));			\
  rtems_interrupt_enable(level);		\
}

#define M83xx_TSEC_ALIGN_BUFFER(buf,align)		\
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

static int mpc83xx_tsec_ioctl
(
 struct ifnet *ifp,                    /* interface information            */
 ioctl_command_t command,              /* ioctl command code               */
 caddr_t data                          /* optional data                    */
 );

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_tsec_hwinit
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize hardware register                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  m83xxTSEC_Registers_t  *reg_ptr = sc->reg_ptr; /* pointer to TSEC registers*/
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
  reg_ptr->ecntrl = ((reg_ptr->ecntrl & ~M83xx_TSEC_ECNTRL_AUTOZ)
		     | M83xx_TSEC_ECNTRL_CLRCNT
		     | M83xx_TSEC_ECNTRL_STEN
		     | M83xx_TSEC_ECNTRL_R100M);

  /*
   * init DMA control register:
   * - enable snooping
   * - write BD status before interrupt request
   * - do not poll TxBD, but wait for TSTAT[THLT] to be written
   */
  reg_ptr->dmactrl = (M83xx_TSEC_DMACTL_TDSEN
		      | M83xx_TSEC_DMACTL_TBDSEN
		      | M83xx_TSEC_DMACTL_WWR
		      | M83xx_TSEC_DMACTL_WOP);

  /*
   * init Attribute register:
   * - enable read snooping for data and BD
   */
  reg_ptr->attr = (M83xx_TSEC_ATTR_RDSEN
		   | M83xx_TSEC_ATTR_RBDSEN);


  reg_ptr->mrblr  = MCLBYTES-64; /* take care of buffer size lost
				  * due to alignment              */

  /*
   * init EDIS register: disable all error reportings
   */
  reg_ptr->edis = (M83xx_TSEC_EDIS_BSYDIS    |
		   M83xx_TSEC_EDIS_EBERRDIS  |
		   M83xx_TSEC_EDIS_TXEDIS    |
		   M83xx_TSEC_EDIS_LCDIS     |
		   M83xx_TSEC_EDIS_CRLXDADIS |
		   M83xx_TSEC_EDIS_FUNDIS);
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
  reg_ptr->txic = (M83xx_TSEC_TXIC_ICEN
		   | M83xx_TSEC_TXIC_ICFCT(2)
		   | M83xx_TSEC_TXIC_ICTT(32));
  /*
   * init receive interrupt coalescing register
   */
#if 0
  reg_ptr->rxic = (M83xx_TSEC_RXIC_ICEN
		   | M83xx_TSEC_RXIC_ICFCT(2)
		   | M83xx_TSEC_RXIC_ICTT(32));
#else
  reg_ptr->rxic = 0;
#endif
  /*
   * init MACCFG1 register
   */
  reg_ptr->maccfg1 = (M83xx_TSEC_MACCFG1_RX_FLOW
		      | M83xx_TSEC_MACCFG1_TX_FLOW);

  /*
   * init MACCFG2 register
   */
  reg_ptr->maccfg2 = ((reg_ptr->maccfg2 & M83xx_TSEC_MACCFG2_IFMODE_MSK)
		      | M83xx_TSEC_MACCFG2_PRELEN( 7)
		      | M83xx_TSEC_MACCFG2_FULLDUPLEX);

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
static void mpc83xx_tsec_mdio_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the MIIM interface                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc        /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{

  /* Set TSEC registers for MDIO communication */

  /*
   * FIXME: Not clear if this works for all boards.
   * Tested only on MPC8313ERDB.
   */
  sc->mdio_ptr = &mpc83xx.tsec [0];

  /*
   * set clock divider
   */
  sc->mdio_ptr->miimcfg = 3;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mpc83xx_tsec_mdio_read
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
  struct mpc83xx_tsec_struct *sc = uarg;/* control structure                */

  /* pointer to TSEC registers */
  m83xxTSEC_Registers_t *reg_ptr = sc->mdio_ptr;

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
  reg_ptr->miimadd = (M83xx_TSEC_MIIMADD_PHY(phy)
		      | M83xx_TSEC_MIIMADD_REGADDR(reg));
  /*
   * start read cycle
   */
  reg_ptr->miimcom = 0;
  reg_ptr->miimcom = M83xx_TSEC_MIIMCOM_READ;

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(2);
  }  while (0 != (reg_ptr->miimind & M83xx_TSEC_MIIMIND_BUSY));
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
int mpc83xx_tsec_mdio_write
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
  struct mpc83xx_tsec_struct *sc = uarg;/* control structure                */

  /* pointer to TSEC registers */
  m83xxTSEC_Registers_t *reg_ptr = sc->mdio_ptr;

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
  reg_ptr->miimadd = (M83xx_TSEC_MIIMADD_PHY(phy)
		      | M83xx_TSEC_MIIMADD_REGADDR(reg));
  /*
   * start write cycle
   */
  reg_ptr->miimcon = val;

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(2);
  }  while (0 != (reg_ptr->miimind & M83xx_TSEC_MIIMIND_BUSY));
  reg_ptr->miimcom = 0;
  return 0;
}


/***************************************************************************\
|  RX receive functions                                                     |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_event_set mpc83xx_tsec_rx_wait_for_events
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all rx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc,       /* control structure                */
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
 M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,M83xx_IEVENT_RXALL,~0);

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
 struct mpc83xx_tsec_struct *sc        /* control structure                */
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
static void mpc83xx_tsec_receive_packets
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   process any received packets                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc        /* control structure                */
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

  while ((0 == ((status = BD_ptr->status) & M83xx_BD_EMPTY)) &&
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
    if ((status & (M83xx_BD_LAST |
		   M83xx_BD_FIRST_IN_FRAME |
		   M83xx_BD_LONG |
		   M83xx_BD_NONALIGNED |
		   M83xx_BD_CRC_ERROR |
		   M83xx_BD_OVERRUN ))
	== (M83xx_BD_LAST |
	    M83xx_BD_FIRST_IN_FRAME ) ) {
      /*
       * send mbuf of this buffer to ether_input()
       */
      m->m_len   = m->m_pkthdr.len = (BD_ptr->length
				    - sizeof(uint32_t)
				    - sizeof(struct ether_header));
      eh         = mtod(m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
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
static void mpc83xx_tsec_refill_rxbds
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   link new buffers to rx BDs                                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc        /* control structure                */
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
      m->m_data        = M83xx_TSEC_ALIGN_BUFFER(m->m_ext.ext_buf,64);
      BD_ptr->buffer   = m->m_data;
      BD_ptr->length   = 0;
      BD_ptr->status   = (M83xx_BD_EMPTY
			  | M83xx_BD_INTERRUPT
			  | ((BD_ptr == sc->Rx_Last_BD)
			     ? M83xx_BD_WRAP
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
static void mpc83xx_tsec_rxDaemon
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)arg;
  bool finished = false;
  rtems_event_set events;
#if !defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif
  /*
   * enable Rx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 |= M83xx_TSEC_MACCFG1_RXEN;
  while (!finished) {
    /*
     * fetch MBufs, associate them to RxBDs
     */
    mpc83xx_tsec_refill_rxbds(sc);
    /*
     * wait for events to come in
     */
    events = mpc83xx_tsec_rx_wait_for_events(sc,INTERRUPT_EVENT);
#if !defined(CLREVENT_IN_IRQ)
    /*
     * clear any pending RX events
     */
    irq_events = sc->reg_ptr->ievent & M83xx_IEVENT_RXALL;
    sc->reg_ptr->ievent = irq_events;
#endif
    /*
     * fetch any completed buffers/packets received
     * and stuff them into the TCP/IP Stack
     */
    mpc83xx_tsec_receive_packets(sc);
  }
  /*
   * disable Rx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 &= ~M83xx_TSEC_MACCFG1_RXEN;
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
 struct mpc83xx_tsec_struct *sc        /* control structure                */
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
void mpc83xx_tsec_tx_start
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
  struct mpc83xx_tsec_struct *sc = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_event_set mpc83xx_tsec_tx_wait_for_events
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc,       /* control structure                */
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
 M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,M83xx_IEVENT_TXALL,~0);

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
static void mpc83xx_tsec_tx_retire
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc        /* control structure                */
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
	 && (0 == (RetBD->status & M83xx_BD_READY ))) {/* BD no longer ready*/

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
static void mpc83xx_tsec_sendpacket
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   handle all tx events                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc,       /* control structure                */
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
	mpc83xx_tsec_tx_retire(sc);
	if (CurrBD->buffer != NULL) {
	  /*
	   * Wait for anything to happen...
	   */
	  mpc83xx_tsec_tx_wait_for_events(sc,INTERRUPT_EVENT);
	}
      }
      status = ((M83xx_BD_PAD_CRC | M83xx_BD_TX_CRC)
		| ((m->m_next == NULL)
		   ? M83xx_BD_LAST | M83xx_BD_INTERRUPT
		   : 0)
		| ((CurrBD == sc->Tx_Last_BD) ? M83xx_BD_WRAP : 0));

      /*
       * link buffer to BD
       */
      CurrBD->buffer = mtod(m, void *);
      CurrBD->length = (uint32_t)m->m_len;
      l = m;       /* remember: we use this mbuf          */

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
	status |= M83xx_BD_READY;
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
    FrstBD->status |= M83xx_BD_READY;
  }
  sc->Tx_NxtFill_BD = CurrBD;
  /*
   * wake up transmitter (clear TSTAT[THLT])
   */
  sc->reg_ptr->tstat = M83xx_TSEC_TSTAT_THLT;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_tsec_txDaemon
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)arg;
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
  sc->reg_ptr->maccfg1 |= M83xx_TSEC_MACCFG1_TXEN;
  while (!finished) {
    /*
     * wait for events to come in
     */
    events = mpc83xx_tsec_tx_wait_for_events(sc,
					     START_TRANSMIT_EVENT
					     | INTERRUPT_EVENT);
#if !defined(CLREVENT_IN_IRQ)
    /*
     * clear any pending TX events
     */
    irq_events = sc->reg_ptr->ievent & M83xx_IEVENT_TXALL;
    sc->reg_ptr->ievent = irq_events;
#endif
    /*
     * retire any sent tx BDs
     */
    mpc83xx_tsec_tx_retire(sc);
    /*
     * Send packets till queue is empty
     */
    do {
      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);

      if (m) {
	mpc83xx_tsec_sendpacket(sc,m);
      }
    } while (m != NULL);

    ifp->if_flags &= ~IFF_OACTIVE;
  }
  /*
   * disable Tx in MACCFG1 register
   */
  sc->reg_ptr->maccfg1 &= ~M83xx_TSEC_MACCFG1_TXEN;
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
static void mpc83xx_tsec_tx_irq_handler
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)handle;
#if defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif

  sc->txInterrupts++;
  /*
   * disable tx interrupts
   */
  M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,M83xx_IEVENT_TXALL,0);

#if defined(CLREVENT_IN_IRQ)
  /*
   * clear any pending TX events
   */
  irq_events = sc->reg_ptr->ievent & M83xx_IEVENT_TXALL;
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
static void mpc83xx_tsec_rx_irq_handler
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)handle;
#if defined(CLREVENT_IN_IRQ)
  uint32_t irq_events;
#endif

  sc->rxInterrupts++;
  /*
   * disable rx interrupts
   */
  M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,M83xx_IEVENT_RXALL,0);
#if defined(CLREVENT_IN_IRQ)
  /*
   * clear any pending RX events
   */
  irq_events = sc->reg_ptr->ievent & M83xx_IEVENT_RXALL;
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
static void mpc83xx_tsec_err_irq_handler
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)handle;
  /*
   * clear error events in IEVENT
   */
  sc->reg_ptr->ievent = M83xx_IEVENT_ERRALL;
  /*
   * has Rx been stopped? then restart it
   */
  if (0 != (sc->reg_ptr->rstat & M83xx_TSEC_RSTAT_QHLT)) {
    sc->rxErrors++;
    sc->reg_ptr->rstat = M83xx_TSEC_RSTAT_QHLT;
  }
  /*
   * has Tx been stopped? then restart it
   */
  if (0 != (sc->reg_ptr->tstat & M83xx_TSEC_TSTAT_THLT)) {
    sc->txErrors++;
    sc->reg_ptr->tstat = M83xx_TSEC_TSTAT_THLT;
  }
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static uint32_t mpc83xx_tsec_irq_mask
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   determine irq mask for given interrupt number                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int irqnum,
 struct mpc83xx_tsec_struct *sc
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    interrupt mask (for ievent/imask register)                             |
\*=========================================================================*/
{
  return ((irqnum == sc->irq_num_tx)
	  ? M83xx_IEVENT_TXALL
	  : ((irqnum == sc->irq_num_rx)
	     ? M83xx_IEVENT_RXALL
	     : ((irqnum == sc->irq_num_err)
		? M83xx_IEVENT_ERRALL
		: 0)));
}
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_tsec_irq_on
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)(irq_conn_data->handle);

  M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,
		       mpc83xx_tsec_irq_mask(irq_conn_data->name,sc),
		       ~0);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_tsec_irq_off
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)irq_conn_data->handle;

  M83xx_TSEC_IMASK_SET(sc->reg_ptr->imask,
		       mpc83xx_tsec_irq_mask(irq_conn_data->name,sc),
		       0);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_tsec_irq_isOn
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
  struct mpc83xx_tsec_struct *sc =
    (struct mpc83xx_tsec_struct *)irq_conn_data->handle;

  return (0 != (sc->reg_ptr->imask
		& mpc83xx_tsec_irq_mask(irq_conn_data->name,sc)));
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void mpc83xx_tsec_install_irq_handlers
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   (un-)install the interrupt handlers                                     |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc,        /* ptr to control structure        */
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
      mpc83xx_tsec_tx_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,     /* (rtems_irq_hdl_param)   */
      mpc83xx_tsec_irq_on,         /* (rtems_irq_enable)      */
      mpc83xx_tsec_irq_off,        /* (rtems_irq_disable)     */
      mpc83xx_tsec_irq_isOn        /* (rtems_irq_is_enabled)  */
    },{
      sc->irq_num_rx,
      mpc83xx_tsec_rx_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,     /* (rtems_irq_hdl_param)   */
      mpc83xx_tsec_irq_on,         /* (rtems_irq_enable)      */
      mpc83xx_tsec_irq_off,        /* (rtems_irq_disable)     */
      mpc83xx_tsec_irq_isOn        /* (rtems_irq_is_enabled)  */
    },{
      sc->irq_num_err,
      mpc83xx_tsec_err_irq_handler, /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)sc,      /* (rtems_irq_hdl_param)   */
      mpc83xx_tsec_irq_on,          /* (rtems_irq_enable)      */
      mpc83xx_tsec_irq_off,         /* (rtems_irq_disable)     */
      mpc83xx_tsec_irq_isOn         /* (rtems_irq_is_enabled)  */
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
static void mpc83xx_tsec_init
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
  struct mpc83xx_tsec_struct *sc = (struct mpc83xx_tsec_struct *)arg;
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
    mpc83xx_tsec_hwinit(sc);
    /*
     * init access to phys
     */
    mpc83xx_tsec_mdio_init(sc);
    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc("TStx",
					   4096,
					   mpc83xx_tsec_txDaemon,
					   sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc("TSrx", 4096,
					   mpc83xx_tsec_rxDaemon,
					   sc);
    /*
     * install interrupt handlers
     */
    mpc83xx_tsec_install_irq_handlers(sc,true);
  }
  /*
   * Set flags appropriately
   */
  if(ifp->if_flags & IFF_PROMISC) {
    sc->reg_ptr->rctrl |=  M83xx_TSEC_RCTRL_PROM;
  }
  else {
    sc->reg_ptr->rctrl &= ~M83xx_TSEC_RCTRL_PROM;
  }

#if defined(HSC_CM01)
  /*
   * for HSC CM01: we need to configure the PHY to use maximum skew adjust
   */

  mpc83xx_tsec_mdio_write(-1,sc,23,0x0100);
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
static void mpc83xx_tsec_off
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   deinitialize the driver and the hardware                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc         /* ptr to control structure        */
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
static void mpc83xx_tsec_stats
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   print statistics                                                        |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct mpc83xx_tsec_struct *sc         /* ptr to control structure        */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  int media;
  int result;
  /*
   * fetch/print media info
   */
  media = IFM_MAKEWORD(0,0,0,sc->phy_default); /* fetch from default phy */

  result = mpc83xx_tsec_ioctl(&(sc->arpcom.ac_if),
			      SIOCGIFMEDIA,
			      (caddr_t)&media);
  if (result == 0) {
    rtems_ifmedia2str(media,NULL,0);
    printf ("\n");
  }
#if 0 /* print all PHY registers */
  {
    int reg;
    uint32_t reg_val;
    printf("****** PHY register values****\n");
    for (reg = 0;reg <= 31;reg++) {
      mpc83xx_tsec_mdio_read(-1,sc,reg,&reg_val);
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
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rpkt]);
  printf ("   Rx broadcasts:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rbca]);
  printf ("   Rx multicasts:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rmca]);
  printf ("           Giant:%-8lu\n",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rovr]);
  printf ("       Non-octet:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_raln]);
  printf ("         Bad CRC:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rfcs]);
  printf ("         Overrun:%-8lu\n",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_rdrp]);

  printf ("   Tx Interrupts:%-8lu",   sc->txInterrupts);
  printf ("       Tx Errors:%-8lu",   sc->txErrors);
  printf ("      Tx packets:%-8lu\n",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_tpkt]);
  printf ("        Deferred:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_tdfr]);
  printf ("  Late Collision:%-8lu",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_tlcl]);
  printf ("Retransmit Limit:%-8lu\n",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_tedf]);
  printf ("        Underrun:%-8lu\n",
	  sc->reg_ptr->rmon_mib[m83xx_tsec_rmon_tund]);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_tsec_ioctl
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
  struct mpc83xx_tsec_struct *sc = ifp->if_softc;
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
      mpc83xx_tsec_off(sc);
    }
    if (ifp->if_flags & IFF_UP) {
      mpc83xx_tsec_init(sc);
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    /*
     * show interface statistics
     */
    mpc83xx_tsec_stats(sc);
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

/* #define DEBUG */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int rtems_mpc83xx_tsec_mode_adapt
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
  struct mpc83xx_tsec_struct *sc = ifp->if_softc;
  int media = IFM_MAKEWORD( 0, 0, 0, sc->phy_default);

#ifdef DEBUG
  printf("c");
#endif
  /*
   * fetch media status
   */
  result = mpc83xx_tsec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
  if (result != 0) {
    return result;
  }
#ifdef DEBUG
  printf("C");
#endif
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
    result = mpc83xx_tsec_ioctl(ifp,SIOCSIFMEDIA,(caddr_t)&media);
    if (result != 0) {
      return result;
    }
    /*
     * wait for auto-negotiation to terminate
     */
    do {
      media = IFM_MAKEWORD(0,0,0,sc->phy_default);
      result = mpc83xx_tsec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
      if (result != 0) {
	return result;
      }
    } while (IFM_NONE == IFM_SUBTYPE(media));
  }

  /*
   * now set HW according to media results:
   */
  /*
   * if we are 1000MBit, then switch IF to byte mode
   */
  if (IFM_1000_T == IFM_SUBTYPE(media)) {
    sc->reg_ptr->maccfg2 =
      ((sc->reg_ptr->maccfg2 & ~M83xx_TSEC_MACCFG2_IFMODE_MSK)
       | M83xx_TSEC_MACCFG2_IFMODE_BYT);
  }
  else {
    sc->reg_ptr->maccfg2 =
      ((sc->reg_ptr->maccfg2 & ~M83xx_TSEC_MACCFG2_IFMODE_MSK)
       | M83xx_TSEC_MACCFG2_IFMODE_NIB);
  }
  /*
   * if we are 10MBit, then switch rate to 10M
   */
  if (IFM_10_T == IFM_SUBTYPE(media)) {
    sc->reg_ptr->ecntrl &= ~M83xx_TSEC_ECNTRL_R100M;
  }
  else {
    sc->reg_ptr->ecntrl |= M83xx_TSEC_ECNTRL_R100M;
  }
  /*
   * if we are half duplex then switch to half duplex
   */
  if (0 == (IFM_FDX & IFM_OPTIONS(media))) {
    sc->reg_ptr->maccfg2 &= ~M83xx_TSEC_MACCFG2_FULLDUPLEX;
  }
  else {
    sc->reg_ptr->maccfg2 |=  M83xx_TSEC_MACCFG2_FULLDUPLEX;
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
static void mpc83xx_tsec_watchdog
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
  rtems_mpc83xx_tsec_mode_adapt(ifp);
  ifp->if_timer    = TSEC_WATCHDOG_TIMEOUT;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static int mpc83xx_tsec_driver_attach
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   attach the driver                                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct rtems_bsdnet_ifconfig *config  /* interface configuration          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    1, if success                                                          |
\*=========================================================================*/
{
  struct mpc83xx_tsec_struct *sc;
  struct ifnet *ifp;
  int    unitNumber;
  char   *unitName;
  uint32_t svr = _read_SVR();
  uint32_t pvr = _read_PVR();

 /*
  * Parse driver name
  */
  if((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0) {
    return 0;
  }

 /*
  * Is driver free?
  */
  if ((unitNumber <= 0) || (unitNumber > M83xx_TSEC_NIFACES)) {

    printk ("Bad TSEC unit number.\n");
    return 0;

    }

  sc = &tsec_driver[unitNumber - 1];
  ifp = &sc->arpcom.ac_if;
  /*
   * add sc to config
   */
  config->drv_ctrl = sc;

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
  sc->reg_ptr         = &mpc83xx.tsec[unitNumber-1];

  if (svr == 0x80b00010 && pvr == 0x80850010) {
    /*
     * This is a special case for MPC8313ERDB with silicon revision 1.  Look in
     * "MPC8313ECE Rev. 3, 3/2008" errata for "IPIC 1".
     */
    if (unitNumber == 1) {
      sc->irq_num_tx      = 37;
      sc->irq_num_rx      = 36;
      sc->irq_num_err     = 35;
    } else if (unitNumber == 2) {
      sc->irq_num_tx      = 34;
      sc->irq_num_rx      = 33;
      sc->irq_num_err     = 32;
    } else {
      return 0;
    }
  } else {
    /* get base interrupt number (for Tx irq, Rx=base+1,Err=base+2) */
    sc->irq_num_tx      = config->irno + 0;  /* tx  irq number from BSP */
    sc->irq_num_rx      = config->irno + 1;  /* rx  irq number from BSP */
    sc->irq_num_err     = config->irno + 2;  /* err irq number from BSP */
  }

  if (config->irno  == 0) {
    rtems_panic("TSEC: interupt base number irno not defined");
  }
  /*
   * setup info about mdio interface
   */
  sc->mdio_info.mdio_r   = mpc83xx_tsec_mdio_read;
  sc->mdio_info.mdio_w   = mpc83xx_tsec_mdio_write;
  sc->mdio_info.has_gmii = 1; /* we support gigabit IF */

  /*
   * XXX: Although most hardware builders will assign the PHY addresses
   * like this, this should be more configurable
   */
#ifdef MPC8313ERDB
  if (unitNumber == 2) {
	  sc->phy_default = 4;
  } else {
	  /* TODO */
	  return 0;
  }
#else /* MPC8313ERDB */
  sc->phy_default = unitNumber-1;
#endif /* MPC8313ERDB */

 /*
  * Set up network interface values
  */
  ifp->if_softc   = sc;
  ifp->if_unit    = unitNumber;
  ifp->if_name    = unitName;
  ifp->if_mtu     = (config->mtu > 0) ? config->mtu : ETHERMTU;
  ifp->if_init    = mpc83xx_tsec_init;
  ifp->if_ioctl   = mpc83xx_tsec_ioctl;
  ifp->if_start   = mpc83xx_tsec_tx_start;
  ifp->if_output  = ether_output;
  ifp->if_watchdog =  mpc83xx_tsec_watchdog; /* XXX: timer is set in "init" */

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

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int rtems_mpc83xx_tsec_driver_attach_detach
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   attach or detach the driver                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct rtems_bsdnet_ifconfig *config, /* interface configuration          */
 int attaching                         /* 0 = detach, else attach          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    1, if success                                                       |
\*=========================================================================*/
{
  if (attaching) {
    return mpc83xx_tsec_driver_attach(config);
  }
  else {
    return 0;
  }
}

