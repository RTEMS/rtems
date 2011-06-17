/* GT64260eth.c : GT64260 10/100 Mb ethernet MAC driver
 *
 * Copyright (c) 2003,2004 Brookhaven National  Laboratory
 *               S. Kate Feng <feng1@bnl.gov>
 * All rights reserved
 *
 * Acknowledgements:
 * netBSD : Copyright (c) 2002 Allegro Networks, Inc., Wasabi Systems, Inc.
 * Marvell : NDA document for the discovery system controller
 *
 * Some notes from the author, S. Kate Feng :
 *
 * 1) Mvme5500 uses Eth0 (controller 0) of the GT64260 to implement
 *    the 10/100 BaseT Ethernet with PCI Master Data Byte Swap\
 *    control.
 * 2) Implemented hardware snoop instead of software snoop
 *    to ensure SDRAM cache coherency. (Copyright : NDA item)
 * 3) Added S/W support for multi mbuf.  (TODO : Let the H/W do it)
 *
 */
#define BYTE_ORDER BIG_ENDIAN

#define INET

#include <rtems.h>
#include <rtems/bspIo.h>	    /* printk */
#include <stdio.h>		    /* printf for statistics */
#include <string.h>

#include <libcpu/io.h>		    /* inp & friends */
#include <libcpu/spr.h>             /* registers.h is included here */
#include <bsp.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>
#include <rtems/error.h>
#include <errno.h>

#include <rtems/rtems/types.h>

/* #include <sys/queue.h> */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sockio.h>             /* SIOCADDMULTI, SIOC...     */
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#ifdef INET
#include <netinet/in_var.h>
#endif

#include <bsp/irq.h>
#include <bsp/GT64260ethreg.h>
#include <bsp/GT64260eth.h>
#include <bsp/VPD.h>

extern unsigned char ReadConfVPD_buff(int offset); /* in startup/bspstart.c */

#define GT_ETH_TASK_NAME  "Geth"
#define PKT_BUF_SZ 1536
#define SOFTC_ALIGN  31
#define HASH_ALIGN   15

#define TXQ_HiLmt_OFF 2

/* <skf>
 * 1. printk debug is for diagnosis only, which may cause
 * unexpected result, especially if txq is under heavy load
 * because CPU is fast with a decent cache.
 */
#define GTeth_debug 0
#define GTeth_rx_debug 0

#if 0
#define GE_FORGOT
#define GE_NORX
#define GT_DEBUG
#endif

/* RTEMS event to kill the daemon */
#define KILL_EVENT		RTEMS_EVENT_1
/* RTEMS event to (re)start the transmitter */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2
/* RTEMS events used by the ISR */
#define RX_EVENT		RTEMS_EVENT_3
#define TX_EVENT		RTEMS_EVENT_4
#define ERR_EVENT		RTEMS_EVENT_5

#define ALL_EVENTS (KILL_EVENT|START_TRANSMIT_EVENT|RX_EVENT|TX_EVENT|ERR_EVENT)

enum GTeth_whack_op {
	GE_WHACK_START,		GE_WHACK_RESTART,
	GE_WHACK_CHANGE,	GE_WHACK_STOP
};

enum GTeth_hash_op {
	GE_HASH_ADD,		GE_HASH_REMOVE,
};

#define	ET_MINLEN 64		/* minimum message length */

static int GTeth_ifioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data);
static void GTeth_ifstart (struct ifnet *);
static void GTeth_ifchange(struct GTeth_softc *sc);
static void GTeth_init_rx_ring(struct GTeth_softc *sc);
static void GT64260eth_daemon(void *arg);
static int GT64260eth_sendpacket(struct GTeth_softc *sc,struct mbuf *m);
static unsigned GTeth_txq_done(struct GTeth_softc *sc);
static void GTeth_tx_cleanup(struct GTeth_softc *sc);
static void GTeth_tx_start(struct GTeth_softc *sc);
static void GTeth_tx_stop(struct GTeth_softc *sc);
static void GTeth_rx_cleanup(struct GTeth_softc *sc);
static int GT64260eth_rx(struct GTeth_softc *sc);
static void GTeth_rx_setup(struct GTeth_softc *sc);
static void GTeth_rxprio_setup(struct GTeth_softc *sc);
static void GTeth_rx_stop(struct GTeth_softc *dc);
static void GT64260eth_isr(void);
static int GTeth_hash_compute(struct GTeth_softc *sc,unsigned char eaddr[ETHER_ADDR_LEN]);
static int GTeth_hash_entry_op(struct GTeth_softc *sc, enum GTeth_hash_op op,
	enum GTeth_rxprio prio,unsigned char eaddr[ETHER_ADDR_LEN]);

static int GTeth_hash_fill(struct GTeth_softc *sc);
static void GTeth_hash_init(struct GTeth_softc *sc);

static struct GTeth_softc *root_GT64260eth_dev = NULL;

static void GT64260eth_irq_on(const rtems_irq_connect_data *irq)
{
  struct GTeth_softc *sc;

  for (sc= root_GT64260eth_dev; sc; sc= sc-> next_module) {
    outl(0x30883444,ETH0_EIMR); /* MOTLoad default interrupt mask */
    return;
  }
}

static void GT64260eth_irq_off(const rtems_irq_connect_data *irq)
{
  struct GTeth_softc *sc;

  for (sc= root_GT64260eth_dev; sc; sc= sc-> next_module)
      outl(0, ETH0_EIMR);
}

static int GT64260eth_irq_is_on(const rtems_irq_connect_data *irq)
{
  return(inl(ETH0_EICR) & ETH_IR_EtherIntSum);
}

static void GT64260eth_isr(void)
{
  struct GTeth_softc *sc = root_GT64260eth_dev;
  rtems_event_set  events=0;
  uint32_t cause;


  cause = inl(ETH0_EICR);
  outl( ~cause,ETH0_EICR);  /* clear the ICR */

  if ( (!cause) || (cause & 0x803d00)) {
       sc->intr_errsts[sc->intr_err_ptr2++]=cause;
       sc->intr_err_ptr2 %=INTR_ERR_SIZE;   /* Till Straumann */
       events |= ERR_EVENT;
  }

  /* ETH_IR_RxBuffer_3|ETH_IR_RxError_3 */
  if (cause & 0x880000) {
     sc->stats.rxInterrupts++;
     events |= RX_EVENT;
  }
  /* If there is an error, we want to continue to next descriptor */
  /* ETH_IR_TxBufferHigh|ETH_IR_TxEndHigh|ETH_IR_TxErrorHigh */
  if (cause & 0x444) {
       sc->stats.txInterrupts++;
       events |= TX_EVENT;
       /* It seems to be unnecessary. However, it's there
        * to be on the safe side due to the datasheet.
        * So far, it does not seem to affect the network performance
        * based on the EPICS catime.
        */
       /* ETH_ESDCMR_TXDH | ETH_ESDCMR_ERD = 0x800080 */
       if ((sc->txq_nactive > 1)&& ((inl(ETH0_ESDCMR)&ETH_ESDCMR_TXDH)==0))
          outl(0x800080,ETH0_ESDCMR);


  }
  rtems_event_send(sc->daemonTid, events);
}

static rtems_irq_connect_data GT64260ethIrqData={
	BSP_MAIN_ETH0_IRQ,
	(rtems_irq_hdl) GT64260eth_isr,
        NULL,
	(rtems_irq_enable) GT64260eth_irq_on,
	(rtems_irq_disable) GT64260eth_irq_off,
	(rtems_irq_is_enabled) GT64260eth_irq_is_on,
};

static void GT64260eth_init_hw(struct GTeth_softc *sc)
{

#ifdef GT_DEBUG
  printk("GT64260eth_init_hw(");
#endif
  /* Kate Feng : Turn the hardware snoop on as MOTLoad did not have
   * it on by default.
   */
  outl(RxBSnoopEn|TxBSnoopEn|RxDSnoopEn|TxDSnoopEn, GT_CUU_Eth0_AddrCtrlLow);
  outl(HashSnoopEn, GT_CUU_Eth0_AddrCtrlHigh);

  sc->rxq_intrbits=0;
  sc->sc_flags=0;

#ifndef GE_NORX
  GTeth_rx_setup(sc);
#endif

#ifndef GE_NOTX
  GTeth_tx_start(sc);
#endif

  sc->sc_pcr |= ETH_EPCR_HS_512;
  outl(sc->sc_pcr, ETH0_EPCR);
  outl(sc->sc_pcxr, ETH0_EPCXR); /* port config. extended reg. */
  outl(0, ETH0_EICR); /* interrupt cause register */
  outl(sc->sc_intrmask, ETH0_EIMR);
#ifndef GE_NOHASH
  /* Port Hash Table Pointer Reg*/
  outl(((unsigned) sc->sc_hashtable),ETH0_EHTPR);
#endif
#ifndef GE_NORX
  outl(ETH_ESDCMR_ERD,ETH0_ESDCMR); /* enable Rx DMA in SDMA Command Register */
  sc->sc_flags |= GE_RXACTIVE;
#endif
#ifdef GT_DEBUG
  printk("SDCMR 0x%x ", inl(ETH0_ESDCMR));
#endif

  /* connect the interrupt handler which should
   * take care of enabling interrupts
   */
  if (!BSP_install_rtems_irq_handler(&GT64260ethIrqData))
     printk("GT64260eth: unable to install ISR");

  /* The ethernet port is ready to transmit/receive */
  outl(sc->sc_pcr | ETH_EPCR_EN, ETH0_EPCR);

#ifdef GT_DEBUG
  printk(")\n");
#endif
}

static void GT64260eth_stop_hw(struct GTeth_softc *sc)
{

  printk("GT64260eth_stop_hw(");

  /* remove our interrupt handler which will also
  * disable interrupts at the MPIC and the device
  * itself
  */
  if (!BSP_remove_rtems_irq_handler(&GT64260ethIrqData))
     printk("GT64260eth: unable to remove IRQ handler!");

  outl(sc->sc_pcr, ETH0_EPCR);
  outl(0, ETH0_EIMR);

  sc->arpcom.ac_if.if_flags &= ~IFF_RUNNING;
#ifndef GE_NOTX
  GTeth_tx_stop(sc);
#endif
#ifndef GE_NORX
  GTeth_rx_stop(sc);
#endif
  sc->sc_hashtable = NULL;
  if (GTeth_debug>0) printk(")");
}

static void GT64260eth_stop(struct GTeth_softc *sc)
{
  if (GTeth_debug>0) printk("GT64260eth_stop(");

  /* The hardware is shutdown in the daemon */
  /* kill the daemon. We also must release the networking
   * semaphore or there'll be a deadlock...
   */
  rtems_event_send(sc->daemonTid, KILL_EVENT);
  rtems_bsdnet_semaphore_release();

  sc->daemonTid=0;
  /* now wait for it to die */
  rtems_semaphore_obtain(sc->daemonSync,RTEMS_WAIT,RTEMS_NO_TIMEOUT);

  /* reacquire the bsdnet semaphore */
  rtems_bsdnet_semaphore_obtain();
  if (GTeth_debug>0) printk(")");
}

static void GT64260eth_ifinit(void *arg)
{
  struct GTeth_softc *sc = (struct GTeth_softc*)arg;
  int i;

#ifdef GT_DEBUG
  printk("GT64260eth_ifinit(): daemon ID: 0x%08x)\n", sc->daemonTid);
#endif
  if (sc->daemonTid) {
#ifdef GT_DEBUG
     printk("GT64260eth: daemon already up, doing nothing\n");
#endif
     return;
  }

#ifndef GE_NOHASH
  /* Mvme5500, the user must initialize the hash table before enabling the
   * Ethernet controller
   */
  GTeth_hash_init(sc);
  GTeth_hash_fill(sc);
#endif

  sc->intr_err_ptr1=0;
  sc->intr_err_ptr2=0;
  for (i=0; i< INTR_ERR_SIZE; i++) sc->intr_errsts[i]=0;

  /* initialize the hardware (we are holding the network semaphore at this point) */
  (void)GT64260eth_init_hw(sc);

  /* launch network daemon */

  /* NOTE:
   * in ss-20011025 (and later) any task created by 'bsdnet_newproc' is
   * wrapped by code which acquires the network semaphore...
   */
   sc->daemonTid = rtems_bsdnet_newproc(GT_ETH_TASK_NAME,4096,GT64260eth_daemon,arg);

  /* Tell the world that we're running */
  sc->arpcom.ac_if.if_flags |= IFF_RUNNING;

}

/* attach parameter tells us whether to attach or to detach the driver */
/* Attach this instance, and then all the sub-devices */
int rtems_GT64260eth_driver_attach(struct rtems_bsdnet_ifconfig *config, int attach)
{
  struct GTeth_softc *sc;
  struct ifnet *ifp;
  unsigned sdcr, data;
  unsigned char hwaddr[6];
  int i, unit, phyaddr;
  void	   *softc_mem;
  char     *name;

  unit = rtems_bsdnet_parse_driver_name(config, &name);
  if (unit < 0) return 0;

  printk("\nEthernet driver name %s unit %d \n",name, unit);
  printk("RTEMS-mvme5500 BSP Copyright (c) 2004, Brookhaven National Lab., Shuchen Kate Feng \n");
  /* Make certain elements e.g. descriptor lists are aligned. */
  softc_mem = rtems_bsdnet_malloc(sizeof(*sc) + SOFTC_ALIGN, M_FREE, M_NOWAIT);

  /* Check for the very unlikely case of no memory. */
  if (softc_mem == NULL)
     printk("GT64260eth: OUT OF MEMORY");

  sc = (void *)(((long)softc_mem + SOFTC_ALIGN) & ~SOFTC_ALIGN);
  memset(sc, 0, sizeof(*sc));

  if (GTeth_debug>0) printk("txq_desc[0] addr:%x, rxq_desc[0] addr:%x, sizeof sc %d\n",&sc->txq_desc[0], &sc->rxq_desc[0], sizeof(*sc));

  sc->sc_macno = unit-1;

  data = inl(ETH_EPAR);
  phyaddr = ETH_EPAR_PhyAD_GET(data, sc->sc_macno);

  /* try to read HW address from the device if not overridden
   * by config
   */
  if (config->hardware_address) {
     memcpy(hwaddr, config->hardware_address, ETHER_ADDR_LEN);
  } else {
    printk("Read EEPROM ");
     for (i = 0; i < 6; i++)
       hwaddr[i] = ReadConfVPD_buff(VPD_ENET0_OFFSET+i);
  }

#ifdef GT_DEBUG
  printk("using MAC addr from device:");
  for (i = 0; i < ETHER_ADDR_LEN; i++) printk("%x:", hwaddr[i]);
  printk("\n");
#endif

  memcpy(sc->arpcom.ac_enaddr, hwaddr, ETHER_ADDR_LEN);

  ifp = &sc->arpcom.ac_if;

  sc->sc_pcr = inl(ETH0_EPCR);
  sc->sc_pcxr = inl(ETH0_EPCXR);
  sc->sc_intrmask = inl(ETH0_EIMR) | ETH_IR_MIIPhySTC;

  printk("address %s\n", ether_sprintf(hwaddr));

#ifdef GT_DEBUG
  printk(", pcr %x, pcxr %x ", sc->sc_pcr, sc->sc_pcxr);
#endif


  sc->sc_pcxr |= ETH_EPCXR_PRIOrx_Override;
  sc->sc_pcxr |= (3<<6); /* highest priority only */
  sc->sc_pcxr &= ~ETH_EPCXR_RMIIEn;  /* MII mode */

  /* Max. Frame Length (packet) allowed for reception is 1536 bytes,
   * instead of 2048 (MOTLoad default) or 64K.
   */
  sc->sc_pcxr &= ~(3 << 14);
  sc->sc_pcxr |= (ETH_EPCXR_MFL_1536 << 14);
  sc->sc_max_frame_length = PKT_BUF_SZ;


  if (sc->sc_pcr & ETH_EPCR_EN) {
      int tries = 1000;
      /* Abort transmitter and receiver and wait for them to quiese*/
      outl(ETH_ESDCMR_AR|ETH_ESDCMR_AT,ETH0_ESDCMR);
      do {
	rtems_bsp_delay(100);
      } while (tries-- > 0 && (inl(ETH0_ESDCMR) & (ETH_ESDCMR_AR|ETH_ESDCMR_AT)));
  }
#ifdef GT_DEBUG
  printk(", phy %d (mii)\n", phyaddr);
  printk("ETH0_ESDCMR %x ", inl(ETH0_ESDCMR));
#endif

  sc->sc_pcr &= ~(ETH_EPCR_EN | ETH_EPCR_RBM | ETH_EPCR_PM | ETH_EPCR_PBF);

#ifdef GT_DEBUG
	printk("Now sc_pcr %x,sc_pcxr %x", sc->sc_pcr, sc->sc_pcxr);
#endif

  /*
   * Now turn off the GT.  If it didn't quiese, too ***ing bad.
   */
  outl(sc->sc_pcr, ETH0_EPCR);
  outl(sc->sc_intrmask, ETH0_EIMR);
  sdcr = inl(ETH0_ESDCR);
  /* Burst size is limited to 4 64bit words */
  ETH_ESDCR_BSZ_SET(sdcr, ETH_ESDCR_BSZ_4);
  sdcr |= ETH_ESDCR_RIFB;/*limit interrupt on frame boundaries, instead of buffer*/
#if 0
  sdcr &= ~(ETH_ESDCR_BLMT|ETH_ESDCR_BLMR); /* MOTLoad defualt Big-endian */
#endif
  outl(sdcr, ETH0_ESDCR);

#ifdef GT_DEBUG
  printk("sdcr %x \n", sdcr);
#endif

  if (phyaddr== -1) printk("MII auto negotiation ?");

  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;

  ifp->if_softc = sc;

  /* set this interface's name and unit */
  ifp->if_unit = unit;
  ifp->if_name = name;

  ifp->if_mtu = config->mtu ? config->mtu : ETHERMTU;

  ifp->if_init = GT64260eth_ifinit;
  ifp->if_ioctl = GTeth_ifioctl;
  ifp->if_start = GTeth_ifstart;
  ifp->if_output = ether_output;

  /*  ifp->if_watchdog = GTeth_ifwatchdog;*/

  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /* create the synchronization semaphore */
  if (RTEMS_SUCCESSFUL != rtems_semaphore_create(
     rtems_build_name('G','e','t','h'),0,0,0,&sc->daemonSync))
     printk("GT64260eth: semaphore creation failed");

  sc->next_module = root_GT64260eth_dev;
  root_GT64260eth_dev = sc;

  /* Actually attach the interface */
  if_attach(ifp);
  ether_ifattach(ifp);

#ifdef GT_DEBUG
  printk("GT64260eth: Ethernet driver has been attached (handle 0x%08x,ifp 0x%08x)\n",sc, ifp);
#endif

  return(1);
}

static void GT64260eth_stats(struct GTeth_softc *sc)
{
#if 0
  struct ifnet *ifp = &sc->arpcom.ac_if;

  printf("       Rx Interrupts:%-8lu\n", sc->stats.rxInterrupts);
  printf("     Receive Packets:%-8lu\n", ifp->if_ipackets);
  printf("     Receive  errors:%-8lu\n", ifp->if_ierrors);
  printf("      Framing Errors:%-8lu\n", sc->stats.frame_errors);
  printf("          Crc Errors:%-8lu\n", sc->stats.crc_errors);
  printf("    Oversized Frames:%-8lu\n", sc->stats.length_errors);
  printf("         Active Rxqs:%-8u\n",  sc->rxq_active);
  printf("       Tx Interrupts:%-8lu\n", sc->stats.txInterrupts);
#endif
  printf("Multi-BuffTx Packets:%-8lu\n", sc->stats.txMultiBuffPacket);
  printf("Multi-BuffTx max len:%-8lu\n", sc->stats.txMultiMaxLen);
  printf("SingleBuffTx max len:%-8lu\n", sc->stats.txSinglMaxLen);
  printf("Multi-BuffTx maxloop:%-8lu\n", sc->stats.txMultiMaxLoop);
  printf("Tx buffer max len   :%-8lu\n", sc->stats.txBuffMaxLen);
#if 0
  printf("   Transmitt Packets:%-8lu\n", ifp->if_opackets);
  printf("   Transmitt  errors:%-8lu\n", ifp->if_oerrors);
  printf("    Tx/Rx collisions:%-8lu\n", ifp->if_collisions);
  printf("         Active Txqs:%-8u\n", sc->txq_nactive);
#endif
}

void GT64260eth_printStats(void)
{
  GT64260eth_stats(root_GT64260eth_dev);
}

static int GTeth_ifioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
  struct GTeth_softc *sc = ifp->if_softc;
  struct ifreq *ifr = (struct ifreq *) data;

  int error = 0;

#ifdef GT_DEBUG
  printk("GTeth_ifioctl(");
#endif

  switch (cmd) {
    default:
      if (GTeth_debug >0) {
         printk("etherioctl(");
         if (cmd== SIOCSIFADDR) printk("SIOCSIFADDR ");
      }
      return ether_ioctl(ifp, cmd, data);

    case SIOCSIFFLAGS:
       switch (ifp->if_flags & (IFF_UP|IFF_RUNNING)) {
            case IFF_RUNNING:	/* not up, so we stop */
	         GT64260eth_stop(sc);
	         break;
            case IFF_UP:  /* not running, so we start */
	         GT64260eth_ifinit(sc);
	         break;
	    case IFF_UP|IFF_RUNNING:/* active->active, update */
	         GT64260eth_stop(sc);
                 GT64260eth_ifinit(sc);
	         break;
            default:			/* idle->idle: do nothing */
	         break;
       }
       break;
    case SIO_RTEMS_SHOW_STATS:
       GT64260eth_stats (sc);
       break;
    case SIOCADDMULTI:
    case SIOCDELMULTI:
       error = (cmd == SIOCADDMULTI)
		  ? ether_addmulti(ifr, &sc->arpcom)
		  : ether_delmulti(ifr, &sc->arpcom);
       if (error == ENETRESET) {
	   if (ifp->if_flags & IFF_RUNNING)
	      GTeth_ifchange(sc);
	   else
	      error = 0;
       }
       break;
    case SIOCSIFMTU:
       if (ifr->ifr_mtu > ETHERMTU || ifr->ifr_mtu < ETHERMIN) {
	  error = EINVAL;
	  break;
       }
       ifp->if_mtu = ifr->ifr_mtu;
       break;
  }

#ifdef GT_DEBUG
  printk("exit ioctl\n");
#endif
  return error;
}

static void GTeth_ifstart(struct ifnet *ifp)
{
  struct GTeth_softc *sc = ifp->if_softc;

#ifdef GT_DEBUG
  printk("GTeth_ifstart(");
#endif

  if ((ifp->if_flags & IFF_RUNNING) == 0) {
#ifdef GT_DEBUG
     printk("IFF_RUNNING==0\n");
#endif
     return;
  }

  ifp->if_flags |= IFF_OACTIVE;
  rtems_event_send (sc->daemonTid, START_TRANSMIT_EVENT);
#ifdef GT_DEBUG
  printk(")\n");
#endif
}

/* Initialize the Rx rings */
static void GTeth_init_rx_ring(struct GTeth_softc *sc)
{
  int i;
  volatile struct GTeth_desc *rxd;
  unsigned nxtaddr;

  sc->rxq_fi=0;
  sc->rxq_head_desc = &sc->rxq_desc[0];
  rxd = sc->rxq_head_desc;

  sc->rxq_desc_busaddr = (unsigned long) sc->rxq_head_desc;
#ifdef GT_DEBUG
  printk("rxq_desc_busaddr %x ,&sc->rxq_desc[0] %x\n",
        sc->rxq_desc_busaddr, sc->rxq_head_desc);
#endif

  nxtaddr = sc->rxq_desc_busaddr + sizeof(*rxd);
  sc->rx_buf_sz = (sc->arpcom.ac_if.if_mtu <= 1500 ? PKT_BUF_SZ : sc->arpcom.ac_if.if_mtu + 32);
  sc->rxq_active = RX_RING_SIZE;

  for (i = 0; i < RX_RING_SIZE; i++, rxd++, nxtaddr += sizeof(*rxd)) {
    struct mbuf *m;

    rxd->ed_lencnt= sc->rx_buf_sz <<16;
    rxd->ed_cmdsts = RX_CMD_F|RX_CMD_L|RX_CMD_O|RX_CMD_EI;

    MGETHDR(m, M_WAIT, MT_DATA);
    MCLGET(m, M_WAIT);
    m->m_pkthdr.rcvif =  &sc->arpcom.ac_if;
    sc->rxq_mbuf[i] = m;

    /* convert mbuf pointer to data pointer of correct type */
    rxd->ed_bufptr = (unsigned) mtod(m, void *);

    /*
     * update the nxtptr to point to the next txd.
     */
    if (i == RX_RING_SIZE - 1)
	nxtaddr = sc->rxq_desc_busaddr;
    rxd->ed_nxtptr = nxtaddr;

#ifdef GT_DEBUG
  printk("ed_lencnt %x, rx_buf_sz %x ",rxd->ed_lencnt, sc->rx_buf_sz);
  printk("ed_cmdsts %x \n",rxd->ed_cmdsts);
  printk("mbuf @ 0x%x, next desc. @ 0x%x\n",rxd->ed_bufptr,rxd->ed_nxtptr);
#endif
  }
}

void GTeth_rxprio_setup(struct GTeth_softc *sc)
{

  GTeth_init_rx_ring(sc);

  sc->rxq_intrbits = ETH_IR_RxBuffer|ETH_IR_RxError|ETH_IR_RxBuffer_3|ETH_IR_RxError_3;
}

static int GT64260eth_rx(struct GTeth_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  int nloops=0;

#ifdef GT_DEBUG
  if (GTeth_rx_debug>0) printk("GT64260eth_rx(");
#endif

  while (sc->rxq_active > 0) {
    volatile struct GTeth_desc *rxd = &sc->rxq_desc[sc->rxq_fi];
    struct ether_header *eh;
    unsigned int cmdsts;
    unsigned int byteCount;

    cmdsts = rxd->ed_cmdsts;

    /*
     * Sometimes the GE "forgets" to reset the ownership bit.
     * But if the length has been rewritten, the packet is ours
     * so pretend the O bit is set.
     *
     */
    byteCount = rxd->ed_lencnt & 0xffff;

    if (cmdsts & RX_CMD_O) {
      if (byteCount == 0)
         return(0);

     /* <Kate Feng> Setting command/status to be zero seems to eliminate
      * the spurious interrupt associated with the GE_FORGOT issue.
      */
      rxd->ed_cmdsts=0;

#ifdef GE_FORGOT
      /*
       * For dignosis purpose only. Not a good practice to turn it on
       */
      printk("Rxq %d %d %d\n", sc->rxq_fi, byteCount,nloops);
#endif
    }

    /* GT gave the ownership back to the CPU or the length has
     * been rewritten , which means there
     * is new packet in the descriptor/buffer
     */

    nloops++;
    /*
     * If this is not a single buffer packet with no errors
     * or for some reason it's bigger than our frame size,
     * ignore it and go to the next packet.
     */
    if ((cmdsts & (RX_CMD_F|RX_CMD_L|RX_STS_ES)) !=
                            (RX_CMD_F|RX_CMD_L) ||
                    byteCount > sc->sc_max_frame_length) {
        --sc->rxq_active;
        ifp->if_ipackets++;
        ifp->if_ierrors++;
        if (cmdsts & RX_STS_OR) sc->stats.or_errors++;
        if (cmdsts & RX_STS_CE) sc->stats.crc_errors++;
        if (cmdsts & RX_STS_MFL) sc->stats.length_errors++;
        if (cmdsts & RX_STS_SF) sc->stats.frame_errors++;
        if ((cmdsts & RX_STS_LC) || (cmdsts & RX_STS_COL))
           ifp->if_collisions++;
        /* recycle the buffer */
        m->m_len=sc->rx_buf_sz;        
    }
    else {
        m = sc->rxq_mbuf[sc->rxq_fi];
        m->m_len = m->m_pkthdr.len = byteCount - sizeof(struct ether_header);
        eh = mtod (m, struct ether_header *);
        m->m_data += sizeof(struct ether_header);
        ether_input (ifp, eh, m);

        ifp->if_ipackets++;
        ifp->if_ibytes+=byteCount;
        --sc->rxq_active;
        MGETHDR (m, M_WAIT, MT_DATA);
        MCLGET (m, M_WAIT);
     }
     m->m_pkthdr.rcvif = ifp;
     sc->rxq_mbuf[sc->rxq_fi]= m;
     /* convert mbuf pointer to data pointer of correct type */
     rxd->ed_bufptr = (unsigned) mtod(m, void*);
     rxd->ed_lencnt = (unsigned long) sc->rx_buf_sz <<16;
     rxd->ed_cmdsts = RX_CMD_F|RX_CMD_L|RX_CMD_O|RX_CMD_EI;

     if (++sc->rxq_fi == RX_RING_SIZE) sc->rxq_fi = 0;

     sc->rxq_active++;
  } /* while (sc->rxq_active > 0) */
#ifdef GT_DEBUG
  if (GTeth_rx_debug>0) printk(")");
#endif
  return nloops;
}

static void GTeth_rx_setup(struct GTeth_softc *sc)
{

  if (GTeth_rx_debug>0) printk("GTeth_rx_setup(");

  GTeth_rxprio_setup(sc);

  if ((sc->sc_flags & GE_RXACTIVE) == 0) {
     /* First Rx Descriptor Pointer 3 */
     outl( sc->rxq_desc_busaddr, ETH0_EFRDP3);
     /* Current Rx Descriptor Pointer 3 */
     outl( sc->rxq_desc_busaddr,ETH0_ECRDP3);
#ifdef GT_DEBUG
     printk("ETH0_EFRDP3 0x%x, ETH0_ECRDP3 0x%x \n", inl(ETH0_EFRDP3),
	    inl(ETH0_ECRDP3));
#endif
  }
  sc->sc_intrmask |= sc->rxq_intrbits;

  if (GTeth_rx_debug>0) printk(")\n");
}

static void GTeth_rx_cleanup(struct GTeth_softc *sc)
{
  int i;

  if (GTeth_rx_debug>0) printk( "GTeth_rx_cleanup(");

  for (i=0; i< RX_RING_SIZE; i++) {
    if (sc->rxq_mbuf[i]) {
      m_freem(sc->rxq_mbuf[i]);
      sc->rxq_mbuf[i]=0;
    }
  }
  if (GTeth_rx_debug>0) printk(")");
}

static void GTeth_rx_stop(struct GTeth_softc *sc)
{
  if (GTeth_rx_debug>0) printk( "GTeth_rx_stop(");
  sc->sc_flags &= ~GE_RXACTIVE;
  sc->sc_idlemask &= ~(ETH_IR_RxBits|ETH_IR_RxBuffer_3|ETH_IR_RxError_3);
  sc->sc_intrmask &= ~(ETH_IR_RxBits|ETH_IR_RxBuffer_3|ETH_IR_RxError_3);
  outl(sc->sc_intrmask, ETH0_EIMR);
  outl(ETH_ESDCMR_AR, ETH0_ESDCMR); /* abort receive */
  do {
     rtems_bsp_delay(10);
  } while (inl(ETH0_ESDCMR) & ETH_ESDCMR_AR);
  GTeth_rx_cleanup(sc);
  if (GTeth_rx_debug>0) printk(")");
}

static void GTeth_txq_free(struct GTeth_softc *sc, unsigned cmdsts)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  volatile struct GTeth_desc *txd = &sc->txq_desc[sc->txq_fi];

  /* ownership is sent back to CPU */
  if (GTeth_debug>0) printk("txq%d,active %d\n", sc->txq_fi, sc->txq_nactive);

  txd->ed_cmdsts &= ~TX_CMD_O; /* <skf> in case GT forgot */

  /* statistics */
  ifp->if_opackets++;
  ifp->if_obytes += sc->txq_mbuf[sc->txq_fi]->m_len;
  if (cmdsts & TX_STS_ES) {
       ifp->if_oerrors++;
       if ((cmdsts & TX_STS_LC) || (cmdsts & TX_STS_COL))
	   ifp->if_collisions++;
  }
  /* Free the original mbuf chain */
  m_freem(sc->txq_mbuf[sc->txq_fi]);
  sc->txq_mbuf[sc->txq_fi] = 0;
  ifp->if_timer = 5;

  sc->txq_free++;
  if (++sc->txq_fi == TX_RING_SIZE) sc->txq_fi = 0;
  --sc->txq_nactive;
}

#if UNUSED
static int txq_high_limit(struct GTeth_softc *sc)
{
  /*
   * Have we [over]consumed our limit of descriptors?
   * Do we have enough free descriptors?
   */
  if ( TX_RING_SIZE == sc->txq_nactive + TXQ_HiLmt_OFF) {
     volatile struct GTeth_desc *txd2 = &sc->txq_desc[sc->txq_fi];
     unsigned cmdsts;

     cmdsts = txd2->ed_cmdsts;
     if (cmdsts & TX_CMD_O) {  /* Ownership (1=GT 0=CPU) */
	 int nextin;

	 /*
	  * Sometime the Discovery forgets to update the
	  * last descriptor.  See if CPU owned the descriptor
	  * after it (since we know we've turned that to
	  * the discovery and if CPU owned it, the Discovery
	  * gave it back).  If CPU does, we know the Discovery
	  * gave back this one but forgot to mark it back to CPU.
	  */
	 nextin = (sc->txq_fi + 1) % TX_RING_SIZE;
	 if (sc->txq_desc[nextin].ed_cmdsts & TX_CMD_O) {
#if 0
	    printk("Overconsuming Tx descriptors!\n");
#endif
	    return 1;
         }
         printk("Txq %d forgot\n", sc->txq_fi);
     }
    /* Txq ring is almost full, let's free the current buffer here */
#if 0
    printk("Txq ring near full, free desc%d\n",sc->txq_fi);
#endif
    GTeth_txq_free(sc, cmdsts);
  } /* end if ( TX_RING_SIZE == sc->txq_nactive + TXQ_HiLmt_OFF) */
  return 0;
}
#endif

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

static int GT64260eth_sendpacket(struct GTeth_softc *sc,struct mbuf *m)
{
  volatile struct GTeth_desc *txd = &sc->txq_desc[sc->txq_lo];
  unsigned intrmask = sc->sc_intrmask;
  unsigned loop=0, index= sc->txq_lo;

  /*
   * The end-of-list descriptor we put on last time is the starting point
   * for this packet.  The GT is supposed to terminate list processing on
   * a NULL nxtptr but that currently is broken so a CPU-owned descriptor
   * must terminate the list.
   */
  intrmask = sc->sc_intrmask;

  if ( !(m->m_next)) {/* single buffer packet */
    sc->txq_mbuf[index]= m;
    sc->stats.txSinglMaxLen= MAX(m->m_len, sc->stats.txSinglMaxLen);
  }
  else /* multiple mbufs in this packet */
  {
    struct mbuf *mtp, *mdest;
    volatile unsigned char *pt;
    int len, y;

#ifdef GT_DEBUG
    printk("multi mbufs ");
#endif
    MGETHDR(mdest, M_WAIT, MT_DATA);
    MCLGET(mdest, M_WAIT);
    pt = (volatile unsigned char *)mdest->m_data;
    for (mtp=m,len=0;mtp;mtp=mtp->m_next) {
      loop++;
      if ( (y=(len+mtp->m_len)) > sizeof(union mcluster)) {
	/* GT64260 allows us to chain the remaining to the next
         * free descriptors.
         */
        printk("packet size %x > mcluster %x\n", y,sizeof(union mcluster));
	printk("GT64260eth : packet too large ");
      }
      memcpy((void *)pt,(char *)mtp->m_data, mtp->m_len);
      pt += mtp->m_len;
#if 0
    printk("%d ",mtp->m_len);
#endif
      len += mtp->m_len;
      sc->stats.txBuffMaxLen=MAX(mtp->m_len,sc->stats.txBuffMaxLen);
    }
    sc->stats.txMultiMaxLoop=MAX(loop, sc->stats.txMultiMaxLoop);
#if 0
    printk("\n");
#endif
    mdest->m_len=len;
    /* free old mbuf chain */
    m_freem(m);
    sc->txq_mbuf[index] = m = mdest;
    sc->stats.txMultiBuffPacket++;
    sc->stats.txMultiMaxLen= MAX(m->m_len, sc->stats.txMultiMaxLen);
  }
  if (m->m_len < ET_MINLEN) m->m_len = ET_MINLEN;

  txd->ed_bufptr = (unsigned) mtod(m, void*);
  txd->ed_lencnt = m->m_len << 16;
  /*txd->ed_cmdsts = TX_CMD_L|TX_CMD_GC|TX_CMD_P|TX_CMD_O|TX_CMD_F|TX_CMD_EI;*/
  txd->ed_cmdsts = 0x80c70000;
  while (txd->ed_cmdsts != 0x80c70000);
  memBar();

#ifdef GT_DEBUG
  printk("len = %d, cmdsts 0x%x ", m->m_len,txd->ed_cmdsts);
#endif

  /*
   * Tell the SDMA engine to "Fetch!"
   * Start Tx high.
   */
  sc->txq_nactive++;
  outl(0x800080, ETH0_ESDCMR); /* ETH_ESDCMR_TXDH| ETH_ESDCMR_ERD */
  if ( ++sc->txq_lo == TX_RING_SIZE) sc->txq_lo = 0;
  sc->txq_free--;

#if 0
  /*
   * Since we have put an item into the packet queue, we now want
   * an interrupt when the transmit queue finishes processing the
   * list.  But only update the mask if needs changing.
   */
  intrmask |= sc->txq_intrbits & ( ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh);
  if (sc->sc_intrmask != intrmask) {
      sc->sc_intrmask = intrmask;
      outl(sc->sc_intrmask, ETH0_EIMR);
  }
#endif

#if 0
  printk("EICR= %x, EIMR= %x ", inl(ETH0_EICR), inl(ETH0_EIMR));
  printk("%s:transmit frame #%d queued in slot %d.\n",
	      sc->arpcom.ac_if.if_name, sc->txq_lo, index);
  printk("pcr %x, pcxr %x DMA dcr %x cmr %x\n", inl(ETH0_EPCR), inl(ETH0_EPCXR), inl(ETH0_ESDCR), inl(ETH0_ESDCMR));
#endif

  return 1;
}

static unsigned GTeth_txq_done(struct GTeth_softc *sc)
{
  if (GTeth_debug>0) printk("Txdone(" );

  while (sc->txq_nactive > 0) {
    /* next to be returned to the CPU */
    volatile struct GTeth_desc *txd = &sc->txq_desc[sc->txq_fi];
    unsigned cmdsts;

    /* if GT64260 still owns it ....... */
    if ((cmdsts = txd->ed_cmdsts) & TX_CMD_O) {
       int nextin;

       /* Someone quoted :
	* "Sometimes the Discovery forgets to update the
	* ownership bit in the descriptor."
        * <skf> More correctly, the last descriptor of each
        * transmitted frame is returned to CPU ownership and
        * status is updated only after the actual transmission
        * of the packet is completed.  Also, if there is an error
        * during transmission, we want to continue the
        * transmission of the next descriptor, in additions to
        * reporting the error.
        */
       /* The last descriptor */
       if (sc->txq_nactive == 1) return(0);

       /*
	* Sometimes the Discovery forgets to update the
	* ownership bit in the descriptor.  See if CPU owned
	* the descriptor after it (since we know we've turned
	* that to the Discovery and if CPU owned it now then the
	* Discovery gave it back).  If we do, we know the
        * Discovery gave back this one but forgot to mark it
	* back to CPU.
	*/
       nextin = (sc->txq_fi + 1) % TX_RING_SIZE;

       if (sc->txq_desc[nextin].ed_cmdsts & TX_CMD_O) return(0);
       printk("Txq%d forgot\n",sc->txq_fi);
    } /* end checking GT64260eth owner */
    GTeth_txq_free(sc, cmdsts);
  }  /* end while */
  if (GTeth_debug>0) printk(")\n");
  return(1);
}

static void GTeth_tx_start(struct GTeth_softc *sc)
{
  int i;
  volatile struct GTeth_desc *txd;
  unsigned nxtaddr;

#ifdef GT_DEBUG
  printk("GTeth_tx_start(");
#endif
  sc->sc_intrmask &= ~(ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh|
			     ETH_IR_TxEndLow |ETH_IR_TxBufferLow);

  txd = &sc->txq_desc[0];
  sc->txq_desc_busaddr = (unsigned long) &sc->txq_desc[0];
#ifdef GT_DEBUG
  printk("txq_desc_busaddr %x, &sc->txq_desc[0] %x \n",
         sc->txq_desc_busaddr,&sc->txq_desc[0]);
#endif

  nxtaddr = sc->txq_desc_busaddr + sizeof(*txd);

  sc->txq_pendq.ifq_maxlen = 10;
  sc->txq_pendq.ifq_head= NULL;
  sc->txq_pendq.ifq_tail= NULL;
  sc->txq_nactive = 0;
  sc->txq_fi = 0;
  sc->txq_lo = 0;
  sc->txq_inptr = PKT_BUF_SZ;
  sc->txq_outptr = 0;
  sc->txq_free = TX_RING_SIZE;

  for (i = 0; i < TX_RING_SIZE;
       i++, txd++,  nxtaddr += sizeof(*txd)) {
      sc->txq_mbuf[i]=0;
      txd->ed_bufptr = 0;

      /*
       * update the nxtptr to point to the next txd.
       */
      txd->ed_cmdsts = 0;
      if ( i== TX_RING_SIZE-1) nxtaddr = sc->txq_desc_busaddr;
      txd->ed_nxtptr =  nxtaddr;
#ifdef GT_DEBUG
      printk("next desc. @ 0x%x\n",txd->ed_nxtptr);
#endif
  }

  sc->txq_intrbits = ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh;
  sc->txq_esdcmrbits = ETH_ESDCMR_TXDH; /* Start Tx high */
  sc->txq_epsrbits = ETH_EPSR_TxHigh;
  /* offset to current tx desc ptr reg */
  sc->txq_ectdp = (caddr_t)ETH0_ECTDP1;
  /* Current Tx Desc Pointer 1 */
  outl(sc->txq_desc_busaddr,ETH0_ECTDP1);

#ifdef GT_DEBUG
  printk(")\n");
#endif
}

static void GTeth_tx_cleanup(struct GTeth_softc *sc)
{
  int i;

  for (i=0; i< TX_RING_SIZE; i++) {
    if (sc->txq_mbuf[i]) {
      m_freem(sc->txq_mbuf[i]);
      sc->txq_mbuf[i]=0;
    }
  }
}

static void GTeth_tx_stop(struct GTeth_softc *sc)
{
  /* SDMA command register : stop Tx high and low */
  outl(ETH_ESDCMR_STDH|ETH_ESDCMR_STDL, ETH0_ESDCMR);

  GTeth_txq_done(sc);
  sc->sc_intrmask &= ~(ETH_IR_TxEndHigh|ETH_IR_TxBufferHigh|
                             ETH_IR_TxEndLow |ETH_IR_TxBufferLow);
  GTeth_tx_cleanup(sc);

  sc->arpcom.ac_if.if_timer = 0;
}

static void GTeth_ifchange(struct GTeth_softc *sc)
{
  if (GTeth_debug>0) printk("GTeth_ifchange(");
  if (GTeth_debug>5) printk("(pcr=%#x,imr=%#x)",inl(ETH0_EPCR),inl(ETH0_EIMR));
  /*  printk("SIOCADDMULTI (SIOCDELMULTI): is it about rx or tx ?\n");*/
  outl(sc->sc_pcr | ETH_EPCR_EN, ETH0_EPCR);
  outl(sc->sc_intrmask, ETH0_EIMR);
  GTeth_ifstart(&sc->arpcom.ac_if);
  /* Current Tx Desc Pointer 0 and 1 */
  if (GTeth_debug>5) printk("(ectdp0=%#x, ectdp1=%#x)",
	    inl(ETH0_ECTDP0), inl(ETH0_ECTDP1));
  if (GTeth_debug>0) printk(")");
}

static int GTeth_hash_compute(struct GTeth_softc *sc,unsigned char eaddr[ETHER_ADDR_LEN])
{
  unsigned w0, add0, add1;
  unsigned result;

  if (GTeth_debug>0) printk("GTeth_hash_compute(");
  add0 = ((unsigned) eaddr[5] <<  0) |
	 ((unsigned) eaddr[4] <<  8) |
	 ((unsigned) eaddr[3] << 16);

  add0 = ((add0 & 0x00f0f0f0) >> 4) | ((add0 & 0x000f0f0f) << 4);
  add0 = ((add0 & 0x00cccccc) >> 2) | ((add0 & 0x00333333) << 2);
  add0 = ((add0 & 0x00aaaaaa) >> 1) | ((add0 & 0x00555555) << 1);

  add1 = ((unsigned) eaddr[2] <<  0) |
	 ((unsigned) eaddr[1] <<  8) |
	 ((unsigned) eaddr[0] << 16);

  add1 = ((add1 & 0x00f0f0f0) >> 4) | ((add1 & 0x000f0f0f) << 4);
  add1 = ((add1 & 0x00cccccc) >> 2) | ((add1 & 0x00333333) << 2);
  add1 = ((add1 & 0x00aaaaaa) >> 1) | ((add1 & 0x00555555) << 1);

#ifdef GT_DEBUG
  printk("eaddr= %s add1:%x add0:%x\n", ether_sprintf1(eaddr), add1, add0);
#endif

  /*
   * hashResult is the 15 bits Hash entry address.
   * ethernetADD is a 48 bit number, which is derived from the Ethernet
   * MAC address, by nibble swapping in every byte (i.e MAC address
   * of 0x123456789abc translates to ethernetADD of 0x21436587a9cb).
   */
  if ((sc->sc_pcr & ETH_EPCR_HM) == 0) {
     /*
      * hashResult[14:0] = hashFunc0(ethernetADD[47:0])
      *
      * hashFunc0 calculates the hashResult in the following manner:
      * hashResult[ 8:0] = ethernetADD[14:8,1,0]
      *	XOR ethernetADD[23:15] XOR ethernetADD[32:24]
      */
     result = (add0 & 3) | ((add0 >> 6) & ~3);
     result ^= (add0 >> 15) ^ (add1 >>  0);
     result &= 0x1ff;
     /*
      *   hashResult[14:9] = ethernetADD[7:2]
      */
     result |= (add0 & ~3) << 7;	/* excess bits will be masked */
#ifdef GT_DEBUG
     printk("hash result %x  ", result & 0x7fff);
#endif
  } else {
#define	TRIBITFLIP	073516240	/* yes its in octal */
     /*
      * hashResult[14:0] = hashFunc1(ethernetADD[47:0])
      *
      * hashFunc1 calculates the hashResult in the following manner:
      * hashResult[08:00] = ethernetADD[06:14]
      *	XOR ethernetADD[15:23] XOR ethernetADD[24:32]
      */
     w0 = ((add0 >> 6) ^ (add0 >> 15) ^ (add1)) & 0x1ff;
     /*
      * Now bitswap those 9 bits
      */
     result = 0;
     result |= ((TRIBITFLIP >> (((w0 >> 0) & 7) * 3)) & 7) << 6;
     result |= ((TRIBITFLIP >> (((w0 >> 3) & 7) * 3)) & 7) << 3;
     result |= ((TRIBITFLIP >> (((w0 >> 6) & 7) * 3)) & 7) << 0;

     /*
      *   hashResult[14:09] = ethernetADD[00:05]
      */
     result |= ((TRIBITFLIP >> (((add0 >> 0) & 7) * 3)) & 7) << 12;
     result |= ((TRIBITFLIP >> (((add0 >> 3) & 7) * 3)) & 7) << 9;
#ifdef GT_DEBUG
     printk("1(%#x)", result);
#endif
  }
#ifdef GT_DEBUG
  printk(")");
#endif

  /* 1/2K address filtering (MOTLoad default )? ->16KB memory required
   * or 8k address filtering ? -> 256KB memory required
   */
  return result & ((sc->sc_pcr & ETH_EPCR_HS_512) ? 0x7ff : 0x7fff);
}

static int GTeth_hash_entry_op(struct GTeth_softc *sc, enum GTeth_hash_op op,
	enum GTeth_rxprio prio,unsigned char eaddr[ETHER_ADDR_LEN])
{
  unsigned long long he;
  unsigned long long *maybe_he_p = NULL;
  int limit;
  int hash;
  int maybe_hash = 0;

#ifdef GT_DEBUG
  printk("GTeth_hash_entry_op(prio %d ", prio);
#endif

  hash = GTeth_hash_compute(sc, eaddr);

  if (sc->sc_hashtable == NULL) {
	printk("hashtable == NULL!");
  }
#ifdef GT_DEBUG
  printk("Hash computed %x eaddr %s\n", hash,ether_sprintf1(eaddr));
#endif

  /*
   * Assume we are going to insert so create the hash entry we
   * are going to insert.  We also use it to match entries we
   * will be removing.  The datasheet is wrong for this.
   */
  he = (((unsigned long long) eaddr[5]) << 43) |
       (((unsigned long long) eaddr[4]) << 35) |
       (((unsigned long long) eaddr[3]) << 27) |
       (((unsigned long long) eaddr[2]) << 19) |
       (((unsigned long long) eaddr[1]) << 11) |
       (((unsigned long long) eaddr[0]) <<  3) |
       ((unsigned long long) HSH_PRIO_INS(prio) | HSH_V | HSH_R);
  /*   he = 0x1b1acd87d08005;*/
  /*
   * The GT will search upto 12 entries for a hit, so we must mimic that.
   */
  hash &= (sc->sc_hashmask / sizeof(he));

#ifdef GT_DEBUG
  if (GTeth_debug>0) {
    unsigned val1, val2;

    val1= he & 0xffffffff;
    val2= (he >>32) & 0xffffffff;
    printk("Hash addr value %x%x, entry %x\n",val2,val1, hash);
  }
#endif

  for (limit = HSH_LIMIT; limit > 0 ; --limit) {
      /*
       * Does the GT wrap at the end, stop at the, or overrun the
       * end?  Assume it wraps for now.  Stash a copy of the
       * current hash entry.
       */
      unsigned long long *he_p = &sc->sc_hashtable[hash];
      unsigned long long thishe = *he_p;

      /*
       * If the hash entry isn't valid, that break the chain.  And
       * this entry a good candidate for reuse.
       */
      if ((thishe & HSH_V) == 0) {
	 maybe_he_p = he_p;
	 break;
      }

      /*
       * If the hash entry has the same address we are looking for
       * then ...  if we are removing and the skip bit is set, its
       * already been removed.  if are adding and the skip bit is
       * clear, then its already added.  In either return EBUSY
       * indicating the op has already been done.  Otherwise flip
       * the skip bit and return 0.
       */
      if (((he ^ thishe) & HSH_ADDR_MASK) == 0) {
	 if (((op == GE_HASH_REMOVE) && (thishe & HSH_S)) ||
	    ((op == GE_HASH_ADD) && (thishe & HSH_S) == 0))
	    return EBUSY;
	  *he_p = thishe ^ HSH_S;

	  if (GTeth_debug>0) {
             unsigned val1, val2;

             val1= *he_p & 0xffffffff;
             val2= (*he_p >>32) & 0xffffffff;
             printk("flip skip bit result %x%x entry %x ",val2,val1, hash);
          }
	  return 0;
       }

       /*
	* If we haven't found a slot for the entry and this entry
        * is currently being skipped, return this entry.
	*/
       if (maybe_he_p == NULL && (thishe & HSH_S)) {
	  maybe_he_p = he_p;
	  maybe_hash = hash;
       }
       hash = (hash + 1) & (sc->sc_hashmask / sizeof(he));
  }

  /*
   * If we got here, then there was no entry to remove.
   */
  if (op == GE_HASH_REMOVE) {
     printk("GT64260eth : No entry to remove\n");
     return ENOENT;
  }

  /*
   * If we couldn't find a slot, return an error.
   */
  if (maybe_he_p == NULL) {
     printk("GT64260eth : No slot found");
     return ENOSPC;
  }

  /* Update the entry.*/
  *maybe_he_p = he;
  if (GTeth_debug>0) {
    unsigned val1, val2;
#if 0
    unsigned long *pt= sc->sc_hashtable;
    int i, loop;

  for (loop= 0; loop< 256; loop++) {
    printk("%d)", loop);
    for (i=0; i< 16; i++, pt++) printk("%x ",*pt);
    printk("\n");
  }
#endif
    val1= he & 0xffffffff;
    val2= (he >>32) & 0xffffffff;
    printk("Update Hash result %x%x, table addr %x entry %x )\n",val2, val1, maybe_he_p, hash);
  }
  return 0;
}

static int GTeth_hash_fill(struct GTeth_softc *sc)
{
  struct ether_multistep step;
  struct ether_multi *enm;
  int error;

#ifdef GT_DEBUG
  printk( "GTeth_hash_fill(");
#endif
  error = GTeth_hash_entry_op(sc,GE_HASH_ADD,GE_RXPRIO_HI,sc->arpcom.ac_enaddr);

  if (error) {
     if (GTeth_debug>0) printk("!");
     return error;
  }

  sc->sc_flags &= ~GE_ALLMULTI;
  if ((sc->arpcom.ac_if.if_flags & IFF_PROMISC) == 0)
     sc->sc_pcr &= ~ETH_EPCR_PM;
  /* see lib/include/netinet/if_ether.h */
  ETHER_FIRST_MULTI(step, &sc->arpcom, enm);
  while (enm != NULL) {
    if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
      /* Frames are received regardless of their destinatin address */
       sc->sc_flags |= GE_ALLMULTI;
       sc->sc_pcr |= ETH_EPCR_PM;
    } else {
      /* Frames are only received if the destinatin address is found
       * in the hash table
       */
       error = GTeth_hash_entry_op(sc, GE_HASH_ADD,
	     GE_RXPRIO_MEDLO, enm->enm_addrlo);
       if (error == ENOSPC) break;
    }
    ETHER_NEXT_MULTI(step, enm);
  }
#ifdef GT_DEBUG
  printk(")\n");
#endif
  return error;
}

static void GTeth_hash_init(struct GTeth_softc *sc)
{
  void *hash_mem;

  if (GTeth_debug>0) printk("GTeth_hash_init(");
  /* MOTLoad defualt : 512 bytes of address filtering, which
   * requires 16KB of memory
   */
#if 1
  hash_mem = rtems_bsdnet_malloc(HASH_DRAM_SIZE + HASH_ALIGN, M_FREE, M_NOWAIT);
  sc->sc_hashtable  =(void *)(((long)hash_mem+ HASH_ALIGN) & ~HASH_ALIGN);
#else
  /* only for test */
  hash_mem = 0x68F000;
  sc->sc_hashtable  =(unsigned long long *)hash_mem;
#endif
  sc->sc_hashmask = HASH_DRAM_SIZE - 1;

  memset((void *)sc->sc_hashtable, 0,HASH_DRAM_SIZE);
#ifdef GT_DEBUG
  printk("hashtable addr:%x, mask %x)\n", sc->sc_hashtable,sc->sc_hashmask);
#endif
}

#ifdef GT64260eth_DEBUG
static void GT64260eth_error(struct GTeth_softc *sc)
{
  struct ifnet	        *ifp = &sc->arpcom.ac_if;
  unsigned int		intr_status= sc->intr_errsts[sc->intr_err_ptr1];

  /* read and reset the status; because this is written
   * by the ISR, we must disable interrupts here
   */
  if (intr_status) {
    printk("%s%d: ICR = 0x%x ",
	   ifp->if_name, ifp->if_unit, intr_status);
#if 1
    if (intr_status & INTR_RX_ERROR) {
       printk("Rxq error, if_ierrors %d\n",
	      ifp->if_ierrors);
    }
#endif
    /* Rx error is handled in GT64260eth_rx() */
    if (intr_status & INTR_TX_ERROR) {
       ifp->if_oerrors++;
       printk("Txq error,  if_oerrors %d\n",ifp->if_oerrors);
    }
  }
  else
    printk("%s%d: Ghost interrupt ?\n",ifp->if_name,
	   ifp->if_unit);
  sc->intr_errsts[sc->intr_err_ptr1++]=0;
  sc->intr_err_ptr1 %= INTR_ERR_SIZE;   /* Till Straumann */
}
#endif

/* The daemon does all of the work; RX, TX and cleaning up buffers/descriptors */
static void GT64260eth_daemon(void *arg)
{
  struct GTeth_softc *sc = (struct GTeth_softc*)arg;
  rtems_event_set	events;
  struct mbuf	*m=0;
  struct ifnet	*ifp=&sc->arpcom.ac_if;

#if 0
  /* see comments in GT64260eth_init(); in newer versions of
   * rtems, we hold the network semaphore at this point
   */
  rtems_semaphore_release(sc->daemonSync);
#endif

  /* NOTE: our creator possibly holds the bsdnet_semaphore.
   *       since that has PRIORITY_INVERSION enabled, our
   *       subsequent call to bsdnet_event_receive() will
   *       _not_ release it. It's still in posession of our
   *       owner.
   *       This is different from how killing this task
   *       is handled.
   */

  for (;;) {
     /* sleep until there's work to be done */
     /* Note: bsdnet_event_receive() acquires
      *       the global bsdnet semaphore for
      *       mutual exclusion.
      */
     rtems_bsdnet_event_receive(ALL_EVENTS,
				RTEMS_WAIT | RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT,
				&events);

     if (KILL_EVENT & events) break;

#ifndef GE_NORX
     if (events & RX_EVENT) GT64260eth_rx(sc);
#endif
#if 0
     printk("%x ", inb(ETH0_EPSR));
     if ( ((i++) % 15)==0) printk("\n");
#endif

     /* clean up and try sending packets */
     do {
	 if (sc->txq_nactive) GTeth_txq_done(sc);

         while (sc->txq_free>0) {
           if (sc->txq_free>TXQ_HiLmt_OFF) {
	      m=0;
	      IF_DEQUEUE(&ifp->if_snd,m);
              if (m==0) break;
              GT64260eth_sendpacket(sc, m);
           }
           else {
              GTeth_txq_done(sc);
              break;
           }
         }
         /* we leave this loop
	  *  - either because there's no free buffer
          *    (m=0 initializer && !sc->txq_free)
	  *  - or there's nothing to send (IF_DEQUEUE
	  *    returned 0
	  */
       } while (m);

       ifp->if_flags &= ~IFF_OACTIVE;

       /* Log errors and other uncommon events. */
#ifdef GT64260eth_DEBUG
       if (events & ERR_EVENT) GT64260eth_error(sc);
#endif
  } /* end for(;;) { rtems_bsdnet_event_receive() .....*/

  ifp->if_flags &= ~(IFF_RUNNING|IFF_OACTIVE);

  /* shut down the hardware */
  GT64260eth_stop_hw(sc);
  /* flush the output queue */
  for (;;) {
      IF_DEQUEUE(&ifp->if_snd,m);
      if (!m) break;
      m_freem(m);
  }
  /* as of 'rtems_bsdnet_event_receive()' we own the
   * networking semaphore
   */
  rtems_bsdnet_semaphore_release();
  rtems_semaphore_release(sc->daemonSync);

  /* Note that I dont use sc->daemonTid here -
   * theoretically, that variable could already
   * hold a newly created TID
   */
  rtems_task_delete(RTEMS_SELF);
}

