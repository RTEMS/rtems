/*
 * Copyright (c) 2004,2005 RTEMS/Mvme5500 port by S. Kate Feng <feng1@bnl.gov>
 *      Brookhaven National Laboratory, All rights reserved
 *
 * Acknowledgements:
 * netBSD : Copyright (c) 2001, 2002, 2003, 2004 Wasabi Systems, Inc.
 *          Jason R. Thorpe for Wasabi Systems, Inc.
 * Intel : NDA document 
 *
 * Some notes from the author, S. Kate Feng :
 * 
 * 1) The error reporting routine i82544EI_error() employs two pointers
 *    for the error report buffer. One for the ISR and another one for
 *    the error report.
 * 2) Enable the hardware Auto-Negotiation state machine.
 * 3) Set Big Endian mode in the WMREG_CTRL so that we do not need htole32
 *    because PPC is big endian mode.
 *    However, the data packet structure defined in if_wmreg.h
 *    should be redefined for the big endian mode.
 * 4) To ensure the cache coherence, the MOTLoad had the PCI
 *    snoop control registers (0x1f00) set to "snoop to WB region" for
 *    the entire 512MB of memory.
 * 5) MOTLoad default :
 *    little endian mode, cache line size is 32 bytes, no checksum control,
 *    hardware auto-neg. state machine disabled. PCI control "snoop
 *    to WB region", MII mode (PHY) instead of TBI mode.
 * 6) We currently only use 32-bit (instead of 64-bit) DMA addressing.
 * 7) Support for checksum offloading and TCP segmentation offload will
 *    be available for releasing in 2008, upon request, if I still believe.
 *    
 */

#define BYTE_ORDER BIG_ENDIAN

#define INET

#include <rtems.h>
#include <rtems/bspIo.h>      /* printk */
#include <stdio.h>	      /* printf for statistics */
#include <string.h>

#include <libcpu/io.h>	      /* inp & friends */
#include <libcpu/spr.h>       /* registers.h is included here */
#include <bsp.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>
#include <rtems/error.h>           
#include <errno.h>

#include <rtems/rtems/types.h>
#include <rtems/score/cpu.h>

#include <sys/queue.h>

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
#include <bsp/pci.h>
#include <bsp/pcireg.h>
#include <bsp/if_wmreg.h>
#define	WMREG_RADV	0x282c	/* Receive Interrupt Absolute Delay Timer */

/*#define CKSUM_OFFLOAD*/

#define	ETHERTYPE_FLOWCONTROL	0x8808	/* 802.3x flow control packet */

#define i82544EI_TASK_NAME "IGHZ"
#define SOFTC_ALIGN        4095

#define INTR_ERR_SIZE        16

/*#define WM_DEBUG*/
#ifdef WM_DEBUG
#define	WM_DEBUG_LINK		0x01
#define	WM_DEBUG_TX		0x02
#define	WM_DEBUG_RX		0x04
#define	WM_DEBUG_GMII		0x08
int	wm_debug = WM_DEBUG_TX|WM_DEBUG_RX|WM_DEBUG_LINK;

#define	DPRINTF(x, y)	if (wm_debug & (x)) printk y
#else
#define	DPRINTF(x, y)	/* nothing */
#endif /* WM_DEBUG */

/* RTEMS event to kill the daemon */
#define KILL_EVENT		RTEMS_EVENT_1
/* RTEMS event to (re)start the transmitter */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2
/* RTEMS events used by the ISR */
#define RX_EVENT		RTEMS_EVENT_3
#define TX_EVENT		RTEMS_EVENT_4
#define ERR_EVENT		RTEMS_EVENT_5
#define INIT_EVENT              RTEMS_EVENT_6
 
#define ALL_EVENTS (KILL_EVENT|START_TRANSMIT_EVENT|RX_EVENT|TX_EVENT|ERR_EVENT|INIT_EVENT)


#define	NTXDESC		        128
#define	NTXDESC_MASK		(NTXDESC - 1)
#define	WM_NEXTTX(x)		(((x) + 1) & NTXDESC_MASK)

#define	NRXDESC		        64
#define	NRXDESC_MASK		(NRXDESC - 1)
#define	WM_NEXTRX(x)		(((x) + 1) & NRXDESC_MASK)
#define	WM_PREVRX(x)		(((x) - 1) & NRXDESC_MASK)

#define	WM_CDOFF(x)	offsetof(struct wm_control_data, x)
#define	WM_CDTXOFF(x)	WM_CDOFF(sc_txdescs[(x)])
#define	WM_CDRXOFF(x)	WM_CDOFF(sc_rxdescs[(x)])

#define TXQ_HiLmt_OFF 64

static uint32_t TxDescCmd;

/*
 * Software state per device.
 */
struct wm_softc {
	wiseman_txdesc_t sc_txdescs[NTXDESC]; /* transmit descriptor memory */
  	wiseman_rxdesc_t sc_rxdescs[NRXDESC]; /* receive descriptor memory */
	struct mbuf *txs_mbuf[NTXDESC];	       /* transmit buffer memory */
	struct mbuf *rxs_mbuf[NRXDESC];	       /* receive buffer memory */
        struct wm_softc *next_module;
        volatile unsigned int intr_errsts[INTR_ERR_SIZE]; /* intr_status */
        unsigned int intr_err_ptr1;     /* ptr used in i82544EI_error() */
        unsigned int intr_err_ptr2;     /* ptr used in ISR */
	int txs_firstdesc;		/* first descriptor in packet */
	int txs_lastdesc;		/* last descriptor in packet */
	int txs_ndesc;			/* # of descriptors used */
        unsigned sc_membase;	        /* Memory space base address */
        unsigned sc_memsize;	        /* Memory space size */

	char	dv_xname[16];		/* external name (name + unit) */
	void *sc_sdhook;		/* shutdown hook */
        struct arpcom arpcom;	        /* rtems if structure, contains ifnet */
	int sc_flags;			/* flags; see below */
	int sc_bus_speed;		/* PCI/PCIX bus speed */
	int sc_flowflags;		/* 802.3x flow control flags */

	void *sc_ih;			/* interrupt cookie */

	int sc_ee_addrbits;		/* EEPROM address bits */
        rtems_id	daemonTid;
        rtems_id	daemonSync;     /* synchronization with the daemon */

	int	 txq_next;		/* next Tx descriptor ready for transmitting */
        uint32_t txq_nactive;	        /* number of active TX descriptors */
	uint32_t txq_fi;		/* next free Tx descriptor */
	uint32_t txq_free;		/* number of free Tx jobs */
	uint32_t sc_txctx_ipcs;		/* cached Tx IP cksum ctx */
	uint32_t sc_txctx_tucs;		/* cached Tx TCP/UDP cksum ctx */

	int	sc_rxptr;		/* next ready Rx descriptor/queue ent */
	int	sc_rxdiscard;
	int	sc_rxlen;
	uint32_t sc_ctrl;		/* prototype CTRL register */
#if 0
	uint32_t sc_ctrl_ext;		/* prototype CTRL_EXT register */
#endif
	uint32_t sc_icr;		/* prototype interrupt bits */
	uint32_t sc_tctl;		/* prototype TCTL register */
	uint32_t sc_rctl;		/* prototype RCTL register */
	uint32_t sc_tipg;		/* prototype TIPG register */
	uint32_t sc_fcrtl;		/* prototype FCRTL register */

	int sc_mchash_type;		/* multicast filter offset */

        /* statistics */
        struct {
          volatile unsigned long     rxInterrupts;
          volatile unsigned long     txInterrupts;
          unsigned long	    txMultiBuffPacket;
          unsigned long     txMultiMaxLen;
          unsigned long     txSinglMaxLen;
          unsigned long     txMultiMaxLoop;
          unsigned long     txBuffMaxLen;
	  unsigned long     linkInterrupts;
          unsigned long     length_errors;
          unsigned long	    frame_errors;
          unsigned long	    crc_errors;
          unsigned long     rxOvrRunInterrupts; /* Rx overrun interrupt */
          unsigned long     rxSeqErr;
          unsigned long     rxC_ordered;
          unsigned long     ghostInterrupts;
          unsigned long     linkStatusChng;
        } stats;
};

/* <skf> our memory address seen from the PCI bus should be 1:1 */
#define htole32(x)  le32toh(x)
#define le32toh(x)  CPU_swap_u32((unsigned int) x)
#define le16toh(x)  CPU_swap_u16(x)

/* sc_flags */
#define	WM_F_HAS_MII		0x01	/* has MII */
/* 82544 EI does not perform EEPROM handshake, EEPROM interface is not SPI */
#define	WM_F_EEPROM_HANDSHAKE	0x02	/* requires EEPROM handshake */
#define	WM_F_EEPROM_SPI		0x04	/* EEPROM is SPI */
#define	WM_F_IOH_VALID		0x10	/* I/O handle is valid */
#define	WM_F_BUS64		0x20	/* bus is 64-bit */
#define	WM_F_PCIX		0x40	/* bus is PCI-X */

#define	CSR_READ(sc,reg) in_le32((volatile unsigned *)(sc->sc_membase+reg))
#define	CSR_WRITE(sc,reg,val) out_le32((volatile unsigned *)(sc->sc_membase+reg), val)

#define	WM_CDTXADDR(sc)	( (uint32_t) &sc->sc_txdescs[0] )
#define	WM_CDRXADDR(sc)	( (uint32_t) &sc->sc_rxdescs[0] )

static struct wm_softc *root_i82544EI_dev = NULL;

static void i82544EI_ifstart(struct ifnet *ifp);
static int  wm_ioctl(struct ifnet *ifp, u_long cmd,uint32_t data);
static void i82544EI_ifinit(void *arg);
static void wm_stop(struct ifnet *ifp, int disable);

static void wm_rxdrain(struct wm_softc *sc);
static int  wm_add_rxbuf(struct wm_softc *sc, int idx);
static int  wm_read_eeprom(struct wm_softc *sc,int word,int wordcnt, uint16_t *data);
static void i82544EI_daemon(void *arg);
static void wm_set_filter(struct wm_softc *sc);

static void i82544EI_isr(void);
static void i82544EI_sendpacket(struct wm_softc *sc, struct mbuf *m);
extern int pci_mem_find(int b, int d, int f, int reg, unsigned *basep,unsigned *sizep);
extern int pci_io_find(int b, int d, int f, int reg,unsigned *basep,unsigned *sizep);
extern int pci_get_capability(int b, int d, int f, int capid,int *offset,uint32_t *value);
extern char * ether_sprintf1(void);

static void i82544EI_irq_on(const rtems_irq_connect_data *irq)
{
  struct wm_softc *sc;
  unsigned int irqMask=  ICR_TXDW | ICR_LSC | ICR_RXSEQ | ICR_RXDMT0 | ICR_RXO | ICR_RXT0 | ICR_RXCFG;
  
  for (sc= root_i82544EI_dev; sc; sc= sc-> next_module) {
    CSR_WRITE(sc,WMREG_IMS,(CSR_READ(sc,WMREG_IMS)| irqMask) );
    return;
  }
}

static void i82544EI_irq_off(const rtems_irq_connect_data *irq)
{
  struct wm_softc *sc;
  unsigned int irqMask=  ICR_TXDW | ICR_LSC | ICR_RXSEQ | ICR_RXDMT0 | ICR_RXO | ICR_RXT0 |ICR_RXCFG ;

  for (sc= root_i82544EI_dev; sc; sc= sc-> next_module) {
    CSR_WRITE(sc,WMREG_IMS, (CSR_READ(sc,WMREG_IMS) & ~irqMask) );
    return;
  }
}

static int i82544EI_irq_is_on(const rtems_irq_connect_data *irq)
{
  return(CSR_READ(root_i82544EI_dev,WMREG_ICR) & root_i82544EI_dev->sc_icr);
}

static rtems_irq_connect_data i82544IrqData={
	BSP_GPP_82544_IRQ,
	(rtems_irq_hdl) i82544EI_isr,
	(rtems_irq_enable) i82544EI_irq_on,
	(rtems_irq_disable) i82544EI_irq_off,
	(rtems_irq_is_enabled) i82544EI_irq_is_on, 
};

int rtems_i82544EI_driver_attach(struct rtems_bsdnet_ifconfig *config, int attach)
{
  struct wm_softc *sc;
  struct ifnet *ifp;
  uint8_t enaddr[ETHER_ADDR_LEN];
  uint16_t myea[ETHER_ADDR_LEN / 2], cfg1, cfg2, swdpin;
  unsigned reg;
  int b,d,f; /* PCI bus/device/function */
  int unit;
  void	   *softc_mem;
  char     *name;

  unit = rtems_bsdnet_parse_driver_name(config, &name);
  if (unit < 0) return 0;
 
  printk("\nEthernet driver name %s unit %d \n",name, unit);
  printk("Copyright (c) 2004,2005 S. Kate Feng <feng1@bnl.gov> (RTEMS/mvme5500 port)\n");

  /* Make sure certain elements e.g. descriptor lists are aligned.*/ 
  softc_mem = rtems_bsdnet_malloc(sizeof(*sc) + SOFTC_ALIGN, M_FREE, M_NOWAIT);

  /* Check for the very unlikely case of no memory. */
  if (softc_mem == NULL)
     rtems_panic("i82544EI: OUT OF MEMORY");

  sc = (void *)(((long)softc_mem + SOFTC_ALIGN) & ~SOFTC_ALIGN);
  memset(sc, 0, sizeof(*sc));

  sprintf(sc->dv_xname, "%s%d", name, unit);

  if (pci_find_device(PCI_VENDOR_ID_INTEL,PCI_DEVICE_INTEL_82544EI_COPPER,
			unit-1,&b, &d, &f))
    rtems_panic("i82544EI device ID not found\n");

#if WM_DEBUG
  printk("82544EI:b%d, d%d, f%d\n", b, d,f);
#endif

  /* Memory-mapped acccess is required for normal operation.*/
  if ( pci_mem_find(b,d,f,PCI_MAPREG_START, &sc->sc_membase, &sc->sc_memsize))
     rtems_panic("i82544EI: unable to map memory space\n");

#ifdef WM_DEBUG
  printk("Memory base addr 0x%x\n", sc->sc_membase);
  printk("txdesc[0] addr:0x%x, rxdesc[0] addr:0x%x, sizeof sc %d\n",&sc->sc_txdescs[0], &sc->sc_rxdescs[0], sizeof(*sc));      
#endif


  sc->sc_ctrl |=CSR_READ(sc,WMREG_CTRL);  
  /*
   * Determine a few things about the bus we're connected to.
   */
  reg = CSR_READ(sc,WMREG_STATUS); 
  if (reg & STATUS_BUS64) sc->sc_flags |= WM_F_BUS64;
  sc->sc_bus_speed = (reg & STATUS_PCI66) ? 66 : 33;
#ifdef WM_DEBUG
  printk("%s%d: %d-bit %dMHz PCI bus\n",name, unit,
	 (sc->sc_flags & WM_F_BUS64) ? 64 : 32, sc->sc_bus_speed);
#endif

  /*
   * Setup some information about the EEPROM.
   */

  sc->sc_ee_addrbits = 6;

#ifdef WM_DEBUG
  printk("%s%d: %u word (%d address bits) MicroWire EEPROM\n",
	    name, unit, 1U << sc->sc_ee_addrbits,
	    sc->sc_ee_addrbits);
#endif

  /*
   * Read the Ethernet address from the EEPROM.
   */
  if (wm_read_eeprom(sc, EEPROM_OFF_MACADDR,
	    sizeof(myea) / sizeof(myea[0]), myea)) 
     rtems_panic("i82544ei 1GHZ ethernet: unable to read Ethernet address");

  enaddr[0] = myea[0] & 0xff;
  enaddr[1] = myea[0] >> 8;
  enaddr[2] = myea[1] & 0xff;
  enaddr[3] = myea[1] >> 8;
  enaddr[4] = myea[2] & 0xff;
  enaddr[5] = myea[2] >> 8;


  memcpy(sc->arpcom.ac_enaddr, enaddr, ETHER_ADDR_LEN);
#ifdef WM_DEBUG
  printk("%s: Ethernet address %s\n", sc->dv_xname,
	    ether_sprintf1(enaddr));
#endif

  /*
   * Read the config info from the EEPROM, and set up various
   * bits in the control registers based on their contents.
   */
  if (wm_read_eeprom(sc, EEPROM_OFF_CFG1, 1, &cfg1)) {
     printk("%s: unable to read CFG1 from EEPROM\n",sc->dv_xname);
     return(0);
  }
  if (wm_read_eeprom(sc, EEPROM_OFF_CFG2, 1, &cfg2)) {
     printk("%s: unable to read CFG2 from EEPROM\n",sc->dv_xname);
     return(0);
  }
  if (wm_read_eeprom(sc, EEPROM_OFF_SWDPIN, 1, &swdpin)) {
     printk("%s: unable to read SWDPIN from EEPROM\n",sc->dv_xname);
     return(0);
  }

  if (cfg1 & EEPROM_CFG1_ILOS) sc->sc_ctrl |= CTRL_ILOS;
  sc->sc_ctrl|=((swdpin >> EEPROM_SWDPIN_SWDPIO_SHIFT) & 0xf) <<
                CTRL_SWDPIO_SHIFT;
  sc->sc_ctrl |= ((swdpin >> EEPROM_SWDPIN_SWDPIN_SHIFT) & 0xf) <<
		CTRL_SWDPINS_SHIFT;

  CSR_WRITE(sc,WMREG_CTRL, sc->sc_ctrl);
#if 0
  CSR_WRITE(sc,WMREG_CTRL_EXT, sc->sc_ctrl_ext);
#endif

  ifp = &sc->arpcom.ac_if;
  /* set this interface's name and unit */
  ifp->if_unit = unit;
  ifp->if_name = name;
  ifp->if_softc = sc;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
  ifp->if_mtu = config->mtu ? config->mtu : ETHERMTU;
  ifp->if_ioctl = wm_ioctl;
  ifp->if_start = i82544EI_ifstart;
  /*  ifp->if_watchdog = wm_watchdog;*/
  ifp->if_init = i82544EI_ifinit;
  if (ifp->if_snd.ifq_maxlen == 0)
      ifp->if_snd.ifq_maxlen = ifqmaxlen;

  ifp->if_output = ether_output;

  /* create the synchronization semaphore */
  if (RTEMS_SUCCESSFUL != rtems_semaphore_create(
     rtems_build_name('I','G','H','Z'),0,0,0,&sc->daemonSync))
     rtems_panic("i82544EI: semaphore creation failed");

  sc->next_module = root_i82544EI_dev;
  root_i82544EI_dev = sc;

  /* Attach the interface. */
  if_attach(ifp);
  ether_ifattach(ifp);
#ifdef WM_DEBUG
  printk("82544EI: Ethernet driver has been attached (handle 0x%08x,ifp 0x%08x)\n",sc, ifp);
#endif

  return(1);
}

/*
 * i82544EI_ifstart:		[ifnet interface function]
 *
 *	Start packet transmission on the interface.
 */
static void
i82544EI_ifstart(struct ifnet *ifp)
{
  struct wm_softc *sc = ifp->if_softc;

#ifdef WM_DEBUG
  printk("i82544EI_ifstart(");
#endif

  if ((ifp->if_flags & IFF_RUNNING) == 0) {
#ifdef WM_DEBUG
     printk("IFF_RUNNING==0\n");
#endif		
     return;
  }

  ifp->if_flags |= IFF_OACTIVE;
  rtems_event_send (sc->daemonTid, START_TRANSMIT_EVENT);
#ifdef WM_DEBUG
  printk(")\n");
#endif
}

static void i82544EI_stats(struct wm_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  printf("       Rx Interrupts:%-8u\n", sc->stats.rxInterrupts);
  printf("     Receive Packets:%-8u\n", CSR_READ(sc,WMREG_GPRC));
  printf("     Receive Overrun:%-8u\n", sc->stats.rxOvrRunInterrupts);
  printf("     Receive  errors:%-8u\n", CSR_READ(sc,WMREG_RXERRC));
  printf("   Rx sequence error:%-8u\n", sc->stats.rxSeqErr);
  printf("      Rx /C/ ordered:%-8u\n", sc->stats.rxC_ordered);
  printf("    Rx Length Errors:%-8u\n", CSR_READ(sc,WMREG_RLEC));
  printf("       Tx Interrupts:%-8u\n", sc->stats.txInterrupts);
#if 0
  printf("Multi-BuffTx Packets:%-8u\n", sc->stats.txMultiBuffPacket);
  printf("Multi-BuffTx max len:%-8u\n", sc->stats.txMultiMaxLen);
  printf("SingleBuffTx max len:%-8u\n", sc->stats.txSinglMaxLen);
  printf("Multi-BuffTx maxloop:%-8u\n", sc->stats.txMultiMaxLoop);
  printf("Tx buffer max len   :%-8u\n", sc->stats.txBuffMaxLen);
#endif
  printf("   Transmitt Packets:%-8u\n", CSR_READ(sc,WMREG_GPTC));
  printf("   Transmitt  errors:%-8u\n", ifp->if_oerrors);
  printf("         Active Txqs:%-8u\n", sc->txq_nactive); 
  printf("          collisions:%-8u\n", CSR_READ(sc,WMREG_COLC));
  printf("          Crc Errors:%-8u\n", CSR_READ(sc,WMREG_CRCERRS));
  printf("  Link Status Change:%-8u\n", sc->stats.linkStatusChng);
}

/*
 * wm_ioctl:		[ifnet interface function]
 *
 *	Handle control requests from the operator.
 */
static int wm_ioctl(struct ifnet *ifp, u_long cmd,uint32_t data)
{
  struct wm_softc *sc = ifp->if_softc;
  int error=0;

  switch (cmd) {
    default:
      error = ether_ioctl(ifp, cmd, data);
      if (error == ENETRESET) {
	  /*
	   * Multicast list has changed; set the hardware filter
	   * accordingly.
	   */
	  wm_set_filter(sc);
	  error = 0;
      }
      break;
    case SIO_RTEMS_SHOW_STATS:
      i82544EI_stats(sc);
      break;
  }

  /* Try to get more packets going.*/
  i82544EI_ifstart(ifp);
  return (error);
}

/*
 * wm_isr:
 *
 *	Interrupt service routine.
 */
static void i82544EI_isr()
{
  volatile struct wm_softc *sc = root_i82544EI_dev;
  uint32_t icr;
  rtems_event_set  events=0;

  /* Reading the WMREG_ICR clears the interrupt bits */
  icr = CSR_READ(sc,WMREG_ICR);

  if ( icr & (ICR_RXDMT0|ICR_RXT0)) {
     sc->stats.rxInterrupts++;
     events |= RX_EVENT;
  }

  if (icr & ICR_TXDW) {
     sc->stats.txInterrupts++;
     events |= TX_EVENT;
  }
  /* <SKF> Rx overrun : no available receive buffer
   * or PCI receive bandwidth inadequate.
   */
  if (icr & ICR_RXO) {
     sc->stats.rxOvrRunInterrupts++;
     events |= INIT_EVENT;
  }
  if (icr & ICR_RXSEQ) /* framing error */ {
     sc->intr_errsts[sc->intr_err_ptr2++]=icr;
     sc->intr_err_ptr2 %=INTR_ERR_SIZE; /* Till Straumann */
     events |= ERR_EVENT;
     sc->stats.rxSeqErr++;
  }
  if ( !icr) sc->stats.ghostInterrupts++;

  if (icr & ICR_LSC) sc->stats.linkStatusChng++;
  if (icr & ICR_RXCFG) sc->stats.rxC_ordered++;

  rtems_event_send(sc->daemonTid, events);
}

/*
 * i82544EI_sendpacket:
 *
 *	Helper; handle transmit interrupts.
 */
static void i82544EI_sendpacket(struct wm_softc *sc, struct mbuf *m)
{

#ifdef WM_DEBUG_TX
  printk("sendpacket(");
#endif

  if ( !(m->m_next)) { /* single buffer packet */
    sc->txs_mbuf[sc->txq_next]= m;
    /* Note: we currently only use 32-bit DMA addresses. */
    sc->sc_txdescs[sc->txq_next].wtx_addr.wa_low = htole32(mtod(m, void*));
    sc->sc_txdescs[sc->txq_next].wtx_cmdlen =htole32(TxDescCmd | m->m_len);
    sc->txs_lastdesc= sc->txq_next;
    sc->txq_next = WM_NEXTTX(sc->txq_next);
    sc->txq_nactive++;
    sc->txq_free--;
  }
  else /* multiple mbufs in this packet */
  { 
    struct mbuf *mtp, *mdest;
    volatile unsigned char *pt;
    int len, y, loop=0;

#ifdef WM_DEBUG_TX
    printk("multi mbufs ");
#endif 
    mtp = m;
    while ( mtp) { 
      MGETHDR(mdest, M_WAIT, MT_DATA);
      MCLGET(mdest, M_WAIT);
      pt = (volatile unsigned char *)mdest->m_data;
      for ( len=0;mtp;mtp=mtp->m_next) {
	loop++;
	/* Each descriptor gets a 2k (MCLBYTES) buffer, although
         * the length of each descriptor can be up to 16288 bytes.
         * For packets which fill more than one buffer ( >2k), we
         * chain them together.
         * <Kate Feng> : This effective for packets > 2K 
         * The other way is effective for packets < 2K
         */
        if ( ((y=(len+mtp->m_len)) > sizeof(union mcluster))) {
          printk(">2048, use next descriptor\n");
	  break; 
        }
        memcpy((void *)pt,(char *)mtp->m_data, mtp->m_len);
        pt += mtp->m_len;
        len += mtp->m_len;
#if 0
        sc->stats.txSinglMaxLen= MAX(mtp->m_len, sc->stats.txSinglMaxLen);
#endif
      } /* end for loop */
      mdest->m_len=len;
      sc->txs_mbuf[sc->txq_next] = mdest;
      /* Note: we currently only use 32-bit DMA addresses. */
      sc->sc_txdescs[sc->txq_next].wtx_addr.wa_low = htole32(mtod(mdest, void*));
      sc->sc_txdescs[sc->txq_next].wtx_cmdlen = htole32(TxDescCmd|mdest->m_len);
      sc->txs_lastdesc = sc->txq_next;
      sc->txq_next = WM_NEXTTX(sc->txq_next);
      sc->txq_nactive ++;
      if (sc->txq_free) 
         sc->txq_free--;
      else
         rtems_panic("i8254EI : no more free descriptors");
#if 0
      sc->stats.txMultiMaxLen= MAX(mdest->m_len, sc->stats.txMultiMaxLen);
      sc->stats.txMultiBuffPacket++;
#endif
    } /* end for while */
    /* free old mbuf chain */
#if 0
    sc->stats.txMultiMaxLoop=MAX(loop, sc->stats.txMultiMaxLoop);
#endif
    m_freem(m); 
    m=0;
  }  /* end  multiple mbufs */
   
  DPRINTF(WM_DEBUG_TX,("%s: TX: desc %d: cmdlen 0x%08x\n", sc->dv_xname,
	 sc->txs_lastdesc, le32toh(sc->sc_txdescs[sc->txs_lastdesc].wtx_cmdlen)));
  DPRINTF(WM_DEBUG_TX,("status 0x%08x\n",sc->sc_txdescs[sc->txq_fi].wtx_fields.wtxu_status));

  memBar();

  /* This is the location where software writes the first NEW descriptor */
  CSR_WRITE(sc,WMREG_TDT, sc->txq_next);

  DPRINTF(WM_DEBUG_TX,("%s: addr 0x%08x, TX: TDH %d, TDT %d\n",sc->dv_xname,
  le32toh(sc->sc_txdescs[sc->txs_lastdesc].wtx_addr.wa_low), CSR_READ(sc,WMREG_TDH),
	  CSR_READ(sc,WMREG_TDT)));

  DPRINTF(WM_DEBUG_TX,("%s: TX: finished transmitting packet, job %d\n",
		    sc->dv_xname, sc->txq_next));

}

static void i82544EI_txq_free(struct wm_softc *sc, uint8_t status)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  /* We might use the statistics registers instead of variables 
   * to keep tack of the network statistics
   */ 

  /* statistics */
  ifp->if_opackets++;

  if (status & (WTX_ST_EC|WTX_ST_LC)) {
       ifp->if_oerrors++;

     if (status & WTX_ST_LC)  
        printf("%s: late collision\n", sc->dv_xname);
     else if (status & WTX_ST_EC) {
        ifp->if_collisions += 16;
         printf("%s: excessive collisions\n", sc->dv_xname);
     }
  }
  /* Free the original mbuf chain */
  m_freem(sc->txs_mbuf[sc->txq_fi]);
  sc->txs_mbuf[sc->txq_fi] = 0;
  sc->sc_txdescs[sc->txq_fi].wtx_fields.wtxu_status=0;

  sc->txq_free ++;
  sc->txq_fi = WM_NEXTTX(sc->txq_fi);
  --sc->txq_nactive;
}

static void i82544EI_txq_done(struct wm_softc *sc)
{
  uint8_t status;

  /*
   * Go through the Tx list and free mbufs for those
   * frames which have been transmitted.
   */
  while ( sc->txq_nactive > 0) {
    status = sc->sc_txdescs[sc->txq_fi].wtx_fields.wtxu_status;
    if ((status & WTX_ST_DD) == 0) break;
    i82544EI_txq_free(sc, status);
    DPRINTF(WM_DEBUG_TX,("%s: TX: job %d done\n",
		    sc->dv_xname, sc->txq_fi));
  }
}

static void wm_init_rxdesc(struct wm_softc *sc, int x)	
{			
  wiseman_rxdesc_t *__rxd = &(sc)->sc_rxdescs[(x)];		
  struct mbuf *m;

  m = sc->rxs_mbuf[x];
		
  __rxd->wrx_addr.wa_low=htole32(mtod(m, void*));  
  __rxd->wrx_addr.wa_high = 0;				        
  __rxd->wrx_len = 0;                                             
  __rxd->wrx_cksum = 0;                                           
  __rxd->wrx_status = 0;	 					
  __rxd->wrx_errors = 0;                                          
  __rxd->wrx_special = 0;                                         
  /* Receive Descriptor Tail: add Rx desc. to H/W free list */    
  CSR_WRITE(sc,WMREG_RDT, (x)); 			        	
} 

static void i82544EI_rx(struct wm_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  int i, len;
  uint8_t status, errors;
  struct ether_header *eh;

#ifdef WM_DEBUG
  printk("i82544EI_rx()\n");
#endif

  for (i = sc->sc_rxptr;; i = WM_NEXTRX(i)) {
    DPRINTF(WM_DEBUG_RX, ("%s: RX: checking descriptor %d\n",
		    sc->dv_xname, i));

    status = sc->sc_rxdescs[i].wrx_status;
    errors = sc->sc_rxdescs[i].wrx_errors;
    len = le16toh(sc->sc_rxdescs[i].wrx_len);
    m = sc->rxs_mbuf[i];

    if ((status & WRX_ST_DD) == 0)  break; /* descriptor not done */

    if (sc->sc_rxdiscard) {
       printk("RX: discarding contents of descriptor %d\n", i);
       wm_init_rxdesc(sc, i);
       if (status & WRX_ST_EOP) {
	  /* Reset our state. */
	  printk("RX: resetting rxdiscard -> 0\n");
	  sc->sc_rxdiscard = 0;
       }
       continue;
    }

    /*
     * If an error occurred, update stats and drop the packet.
     */
    if (errors &(WRX_ER_CE|WRX_ER_SE|WRX_ER_SEQ|WRX_ER_CXE|WRX_ER_RXE)) {
       ifp->if_ierrors++;
       if (errors & WRX_ER_SE)
	  printk("%s: symbol error\n",sc->dv_xname);
       else if (errors & WRX_ER_SEQ)
	  printk("%s: receive sequence error\n",sc->dv_xname);
	    else if (errors & WRX_ER_CE)
		 printk("%s: CRC error\n",sc->dv_xname);
       m_freem(m);
       goto give_it_back;
    }

    /*
     * No errors.  Receive the packet.
     *
     * Note, we have configured the chip to include the
     * CRC with every packet.
     */
    m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);

    DPRINTF(WM_DEBUG_RX,("%s: RX: buffer at %p len %d\n",
		    sc->dv_xname, m->m_data, len));


    eh = mtod (m, struct ether_header *);
    m->m_data += sizeof(struct ether_header);
    ether_input (ifp, eh, m);
    /* Pass it on. */
    ifp->if_ipackets++;
    
    give_it_back:
    /* Add a new receive buffer to the ring.*/
    if (wm_add_rxbuf(sc, i) != 0) {
       /*
        * Failed, throw away what we've done so
        * far, and discard the rest of the packet.
	*/
       printk("Failed in wm_add_rxbuf(), drop packet\n");
       ifp->if_ierrors++;
       wm_init_rxdesc(sc, i);
       if ((status & WRX_ST_EOP) == 0)
          sc->sc_rxdiscard = 1;
       m_freem(m);
    }
  } /* end for */

  /* Update the receive pointer. */
  sc->sc_rxptr = i;
  DPRINTF(WM_DEBUG_RX, ("%s: RX: rxptr -> %d\n", sc->dv_xname, i));
}

static int i82544EI_init_hw(struct wm_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  int i,error;
  uint8_t cksumfields;

  /* Cancel any pending I/O. */
  wm_stop(ifp, 0);

  /* Initialize the error buffer ring */
  sc->intr_err_ptr1=0;
  sc->intr_err_ptr2=0;
  for (i=0; i< INTR_ERR_SIZE; i++) sc->intr_errsts[i]=0;

  /* Initialize the transmit descriptor ring. */
  memset(sc->sc_txdescs, 0, sizeof(sc->sc_txdescs));
  sc->txq_free = NTXDESC;
  sc->txq_next = 0;
  sc->txs_lastdesc = 0;
  sc->txq_next = 0;
  sc->txq_free = NTXDESC;
  sc->txq_nactive = 0;

  sc->sc_txctx_ipcs = 0xffffffff;
  sc->sc_txctx_tucs = 0xffffffff;

  CSR_WRITE(sc,WMREG_TBDAH, 0);
  CSR_WRITE(sc,WMREG_TBDAL, WM_CDTXADDR(sc));
#ifdef WM_DEBUG
  printk("TBDAL 0x%x, TDLEN %d\n", WM_CDTXADDR(sc), sizeof(sc->sc_txdescs));
#endif
  CSR_WRITE(sc,WMREG_TDLEN, sizeof(sc->sc_txdescs));
  CSR_WRITE(sc,WMREG_TDH, 0);
  CSR_WRITE(sc,WMREG_TDT, 0);
  CSR_WRITE(sc,WMREG_TIDV, 64 ); 
  CSR_WRITE(sc,WMREG_TADV, 128);

  CSR_WRITE(sc,WMREG_TXDCTL, TXDCTL_PTHRESH(0) |
		    TXDCTL_HTHRESH(0) | TXDCTL_WTHRESH(0));
  CSR_WRITE(sc,WMREG_RXDCTL, RXDCTL_PTHRESH(0) |
		    RXDCTL_HTHRESH(0) | RXDCTL_WTHRESH(1) | RXDCTL_GRAN );

  CSR_WRITE(sc,WMREG_TQSA_LO, 0);
  CSR_WRITE(sc,WMREG_TQSA_HI, 0);

  /*
   * Set up checksum offload parameters for
   * this packet.
   */
#ifdef CKSUM_OFFLOAD
  if (m0->m_pkthdr.csum_flags &
     (M_CSUM_IPv4|M_CSUM_TCPv4|M_CSUM_UDPv4)) {
     if (wm_tx_cksum(sc, txs, &TxDescCmd,&cksumfields) != 0) {
	 /* Error message already displayed. */
	 continue;
     }
  } else {
#endif
     TxDescCmd = 0;
     cksumfields = 0;
#ifdef CKSUM_OFFLOAD
  }
#endif

  TxDescCmd |= WTX_CMD_EOP|WTX_CMD_IFCS|WTX_CMD_RS; 

  /* Initialize the transmit job descriptors. */
  for (i = 0; i < NTXDESC; i++) {
      sc->txs_mbuf[i] = 0;
      sc->sc_txdescs[i].wtx_fields.wtxu_options=cksumfields;
      sc->sc_txdescs[i].wtx_addr.wa_high = 0;
      sc->sc_txdescs[i].wtx_addr.wa_low = 0;
      sc->sc_txdescs[i].wtx_cmdlen = htole32(TxDescCmd); 
  }

  /*
   * Initialize the receive descriptor and receive job
   * descriptor rings.
   */
  memset(sc->sc_rxdescs, 0, sizeof(sc->sc_rxdescs));
  CSR_WRITE(sc,WMREG_RDBAH, 0);
  CSR_WRITE(sc,WMREG_RDBAL, WM_CDRXADDR(sc));
  CSR_WRITE(sc,WMREG_RDLEN, sizeof(sc->sc_rxdescs));
  CSR_WRITE(sc,WMREG_RDH, 0);
  CSR_WRITE(sc,WMREG_RDT, 0);
  CSR_WRITE(sc,WMREG_RDTR, 0 |RDTR_FPD); 
  CSR_WRITE(sc, WMREG_RADV, 256);

  for (i = 0; i < NRXDESC; i++) {
      if (sc->rxs_mbuf[i] == NULL) {
         if ((error = wm_add_rxbuf(sc, i)) != 0) {
	    printk("%s%d: unable to allocate or map rx buffer"
	    "%d, error = %d\n",ifp->if_name,ifp->if_unit, i, error);
            /*
             * XXX Should attempt to run with fewer receive
             * XXX buffers instead of just failing.
             */
            wm_rxdrain(sc);
            return(error);
          }
      } else {
	printk("sc->rxs_mbuf[%d] not NULL.\n", i);
	wm_init_rxdesc(sc, i);
      }
  }
  sc->sc_rxptr = 0;
  sc->sc_rxdiscard = 0;

  /*
   * Clear out the VLAN table -- we don't use it (yet).
   */
  CSR_WRITE(sc,WMREG_VET, 0);
  for (i = 0; i < WM_VLAN_TABSIZE; i++)
       CSR_WRITE(sc,WMREG_VFTA + (i << 2), 0);

  /*
   * Set up flow-control parameters.
   *
   * XXX Values could probably stand some tuning.
   */
  CSR_WRITE(sc,WMREG_FCAL, FCAL_CONST);/*safe,even though MOTLOAD 0x00c28001 */
  CSR_WRITE(sc,WMREG_FCAH, FCAH_CONST);/*safe,even though MOTLOAD 0x00000100 */
  CSR_WRITE(sc,WMREG_FCT, ETHERTYPE_FLOWCONTROL);/*safe,even though MOTLoad 0x8808 */


  /* safe,even though MOTLoad default all 0 */
  sc->sc_fcrtl = FCRTL_DFLT;

  CSR_WRITE(sc,WMREG_FCRTH, FCRTH_DFLT);
  CSR_WRITE(sc,WMREG_FCRTL, sc->sc_fcrtl);
  CSR_WRITE(sc,WMREG_FCTTV, FCTTV_DFLT);

  sc->sc_ctrl &= ~CTRL_VME;
  /*sc->sc_ctrl |= CTRL_TFCE | CTRL_RFCE;*/
  /* enable Big Endian Mode for the powerPC 
  sc->sc_ctrl |= CTRL_BEM;*/

  /* Write the control registers. */
  CSR_WRITE(sc,WMREG_CTRL, sc->sc_ctrl);
#if 0
  CSR_WRITE(sc,WMREG_CTRL_EXT, sc->sc_ctrl_ext);
#endif

  /* MOTLoad : WMREG_RXCSUM (0x5000)= 0, no Rx checksum offloading */  

  /*
   * Set up the interrupt registers.
   */
  CSR_WRITE(sc,WMREG_IMC, 0xffffffffU);
  /* Reading the WMREG_ICR clears the interrupt bits */
  CSR_READ(sc,WMREG_ICR);

  /*  printf("WMREG_IMS 0x%x\n", CSR_READ(sc,WMREG_IMS));*/

  sc->sc_icr = ICR_TXDW | ICR_LSC | ICR_RXSEQ | ICR_RXCFG | ICR_RXDMT0 | ICR_RXO | ICR_RXT0;

  CSR_WRITE(sc,WMREG_IMS, sc->sc_icr);

  /* Set up the inter-packet gap. */
  CSR_WRITE(sc,WMREG_TIPG, sc->sc_tipg);

#if 0 /* XXXJRT */
  /* Set the VLAN ethernetype. */
  CSR_WRITE(sc,WMREG_VET, ETHERTYPE_VLAN);
#endif

  /*
   * Set up the transmit control register; we start out with
   * a collision distance suitable for FDX, but update it when
   * we resolve the media type.
   */
  sc->sc_tctl = TCTL_EN | TCTL_PSP | TCTL_CT(TX_COLLISION_THRESHOLD) |
	    TCTL_COLD(TX_COLLISION_DISTANCE_FDX) | TCTL_RTLC; /*transmitter enable*/

  /*
   * Set up the receive control register; we actually program
   * the register when we set the receive filter.  Use multicast
   * address offset type 0.
   *
   * Only the i82544 has the ability to strip the incoming
   * CRC, so we don't enable that feature. (TODO)
   */
  sc->sc_mchash_type = 0;
  sc->sc_rctl = RCTL_EN | RCTL_LBM_NONE | RCTL_RDMTS_1_2 | RCTL_LPE |
	    RCTL_DPF | RCTL_MO(sc->sc_mchash_type);

  /* (MCLBYTES == 2048) */
  sc->sc_rctl |= RCTL_2k;

#ifdef WM_DEBUG
  printk("RDBAL 0x%x,RDLEN %d, RDT %d\n",CSR_READ(sc,WMREG_RDBAL),CSR_READ(sc,WMREG_RDLEN), CSR_READ(sc,WMREG_RDT));
#endif

  /* Set the receive filter. */
  wm_set_filter(sc);

  CSR_WRITE(sc,WMREG_TCTL, sc->sc_tctl);

  /* Map and establish our interrupt. */
  if (!BSP_install_rtems_irq_handler(&i82544IrqData))
     rtems_panic("1GHZ ethernet: unable to install ISR");

  return(0);
}

/*
 * i82544EI_ifinit:		[ifnet interface function]
 *
 *	Initialize the interface.
 */
static void i82544EI_ifinit(void *arg)
{
  struct wm_softc *sc = (struct wm_softc*)arg;

#ifdef WM_DEBUG
  printk("i82544EI_ifinit(): daemon ID: 0x%08x)\n", sc->daemonTid);
#endif
  if (sc->daemonTid) {
#ifdef WM_DEBUG
     printk("i82544EI: daemon already up, doing nothing\n");
#endif
     return;
  }
  i82544EI_init_hw(sc);

  sc->daemonTid = rtems_bsdnet_newproc(i82544EI_TASK_NAME,4096,i82544EI_daemon,arg);

  /* ...all done! */
  sc->arpcom.ac_if.if_flags |= IFF_RUNNING; 

#ifdef WM_DEBUG
  printk(")");
#endif
}

/*
 * wm_txdrain:
 *
 *	Drain the transmit queue.
 */
static void wm_txdrain(struct wm_softc *sc)
{
  int i;

  /* Release any queued transmit buffers. */
  for (i = 0; i <  NTXDESC; i++) {
      if (sc->txs_mbuf[i] != NULL) {
	  m_freem(sc->txs_mbuf[i]);
	  sc->txs_mbuf[i] = NULL;
      }
  }
}

/*
 * wm_rxdrain:
 *
 *	Drain the receive queue.
 */
static void wm_rxdrain(struct wm_softc *sc)
{
  int i;

  for (i = 0; i < NRXDESC; i++) {
      if (sc->rxs_mbuf[i] != NULL) {
	  m_freem(sc->rxs_mbuf[i]);
	  sc->rxs_mbuf[i] = NULL;
      }
  }
}

static void i82544EI_tx_stop(struct wm_softc *sc)
{
  wm_txdrain(sc);
}

static void i82544EI_rx_stop(struct wm_softc *sc)
{
  wm_rxdrain(sc);
}

static void i82544EI_stop_hw(struct wm_softc *sc)
{
#ifdef WM_DEBUG
  printk("i82544EI_stop_hw(");
#endif

  /* remove our interrupt handler which will also
  * disable interrupts at the MPIC and the device
  * itself
  */
  if (!BSP_remove_rtems_irq_handler(&i82544IrqData))
     rtems_panic("i82544EI: unable to remove IRQ handler!");

  CSR_WRITE(sc,WMREG_IMS, 0);

  sc->arpcom.ac_if.if_flags &= ~IFF_RUNNING;
  i82544EI_tx_stop(sc);
  i82544EI_rx_stop(sc);
#ifdef WM_DEBUG
  printk(")");
#endif
}

/*
 * wm_stop:		[ifnet interface function]
 *
 *	Stop transmission on the interface.
 */
static void wm_stop(struct ifnet *ifp, int disable)
{
  struct wm_softc *sc = ifp->if_softc;

#ifdef WM_DEBUG
  printk("wm_stop(");
#endif  
  /* Stop the transmit and receive processes. */
  CSR_WRITE(sc,WMREG_TCTL, 0);
  CSR_WRITE(sc,WMREG_RCTL, 0);

  wm_txdrain(sc);
  wm_rxdrain(sc);

  /* Mark the interface as down  */
  ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
#ifdef WM_DEBUG
  printk(")\n");
#endif  
}

/*
 * wm_eeprom_sendbits:
 *
 *	Send a series of bits to the EEPROM.
 */
static void wm_eeprom_sendbits(struct wm_softc *sc, uint32_t bits, int nbits)
{
  uint32_t reg;
  int x;

  reg = CSR_READ(sc,WMREG_EECD);

  for (x = nbits; x > 0; x--) {
      if (bits & (1U << (x - 1)))
	 reg |= EECD_DI;
      else
	 reg &= ~EECD_DI;
      CSR_WRITE(sc,WMREG_EECD, reg);
      rtems_bsp_delay(2);
      CSR_WRITE(sc,WMREG_EECD, reg | EECD_SK);
      rtems_bsp_delay(2);
      CSR_WRITE(sc,WMREG_EECD, reg);
      rtems_bsp_delay(2);
  }
}

/*
 * wm_eeprom_recvbits:
 *
 *	Receive a series of bits from the EEPROM.
 */
static void wm_eeprom_recvbits(struct wm_softc *sc, uint32_t *valp, int nbits)
{
  uint32_t reg, val;
  int x;

  reg = CSR_READ(sc,WMREG_EECD) & ~EECD_DI;

  val = 0;
  for (x = nbits; x > 0; x--) {
      CSR_WRITE(sc,WMREG_EECD, reg | EECD_SK);
      rtems_bsp_delay(2);
      if (CSR_READ(sc,WMREG_EECD) & EECD_DO)
	 val |= (1U << (x - 1));
      CSR_WRITE(sc,WMREG_EECD, reg);
      rtems_bsp_delay(2);
  }
  *valp = val;
}

/*
 * wm_read_eeprom_uwire:
 *
 * Read a word from the EEPROM using the MicroWire protocol.
 *
 * (The 82544EI Gigabit Ethernet Controller is compatible with 
 * most MicroWire interface, serial EEPROM devices.)
 */
static int wm_read_eeprom_uwire(struct wm_softc *sc, int word, int wordcnt, uint16_t *data)
{
  uint32_t reg, val;
  int i;

  for (i = 0; i < wordcnt; i++) {
      /* Clear SK and DI. */
      reg = CSR_READ(sc,WMREG_EECD) & ~(EECD_SK | EECD_DI);
      CSR_WRITE(sc,WMREG_EECD, reg);

      /* Set CHIP SELECT. */
      reg |= EECD_CS;
      CSR_WRITE(sc,WMREG_EECD, reg);
      rtems_bsp_delay(2);

      /* Shift in the READ command. */
      wm_eeprom_sendbits(sc, UWIRE_OPC_READ, 3);

      /* Shift in address. */
      wm_eeprom_sendbits(sc, word + i, sc->sc_ee_addrbits);

      /* Shift out the data. */
      wm_eeprom_recvbits(sc, &val, 16);
      data[i] = val & 0xffff;

      /* Clear CHIP SELECT. */
      reg = CSR_READ(sc,WMREG_EECD) & ~EECD_CS;
      CSR_WRITE(sc,WMREG_EECD, reg);
      rtems_bsp_delay(2);
  }
  return (0);
}

/*
 * wm_acquire_eeprom:
 *
 *      Perform the EEPROM handshake required on some chips.
 */
static int wm_acquire_eeprom(struct wm_softc *sc)
{
  uint32_t reg;
  int x;

  reg = CSR_READ(sc,WMREG_EECD);

  /* Request EEPROM access. */
  reg |= EECD_EE_REQ;
  CSR_WRITE(sc,WMREG_EECD, reg);

  /* ..and wait for it to be granted. */
  for (x = 0; x < 100; x++) {
      reg = CSR_READ(sc,WMREG_EECD);
      if (reg & EECD_EE_GNT) break;
      rtems_bsp_delay(500);
  }
  if ((reg & EECD_EE_GNT) == 0) {
      printk("Could not acquire EEPROM GNT x= %d\n", x);
      reg &= ~EECD_EE_REQ;
      CSR_WRITE(sc,WMREG_EECD, reg);
      return (1);
  }

  return (0);
}

/*
 * wm_read_eeprom:
 *
 * Read data from the serial EEPROM.
 * 82544EI does not Perform the EEPROM handshake
 */
static int wm_read_eeprom(struct wm_softc *sc, int word, int wordcnt, uint16_t *data)
{
#if 0
  /* base on the datasheet, this does not seem to be applicable */
  if (wm_acquire_eeprom(sc))
     return(1);
#endif
  return(wm_read_eeprom_uwire(sc, word, wordcnt, data));
}

/*
 * wm_add_rxbuf:
 *
 *	Add a receive buffer to the indiciated descriptor.
 */
static int wm_add_rxbuf(struct wm_softc *sc, int idx)
{
  struct mbuf *m;

  MGETHDR(m, M_WAIT, MT_DATA);
  if (m == NULL) return (ENOBUFS);
  MCLGET(m, M_WAIT);
  if ((m->m_flags & M_EXT) == 0) {
     m_freem(m);
     return (ENOBUFS);
  }
  m->m_pkthdr.rcvif =  &sc->arpcom.ac_if;
  sc->rxs_mbuf[idx] = m;
  /*  m->m_len = m->m_pkthdr.len = m->m_ext.ext_size;*/
  wm_init_rxdesc(sc, idx);
#if 0
  printk("sc->rxs_mbuf[%d]= 0x%x, mbuf @ 0x%x\n",
    idx, sc->rxs_mbuf[idx], le32toh(sc->sc_rxdescs[idx].wrx_addr.wa_low));
#endif
  return(0);
}

/*
 * wm_set_ral:
 *
 *	Set an entery in the receive address list.
 */
static void
wm_set_ral(struct wm_softc *sc, const uint8_t *enaddr, int idx)
{
  uint32_t ral_lo, ral_hi;

  if (enaddr != NULL) {
     ral_lo = enaddr[0]|(enaddr[1] << 8)|(enaddr[2] << 16)|(enaddr[3] << 24);
     ral_hi = enaddr[4] | (enaddr[5] << 8);
     ral_hi |= RAL_AV;
  } else {
     ral_lo = 0;
     ral_hi = 0;
  }

  CSR_WRITE(sc,WMREG_RAL_LO(WMREG_CORDOVA_RAL_BASE, idx),ral_lo);
  CSR_WRITE(sc,WMREG_RAL_HI(WMREG_CORDOVA_RAL_BASE, idx),ral_hi);
}

/*
 * wm_mchash:
 *
 *	Compute the hash of the multicast address for the 4096-bit
 *	multicast filter.
 */
static uint32_t
wm_mchash(struct wm_softc *sc, const uint8_t *enaddr)
{
  static const int lo_shift[4] = { 4, 3, 2, 0 };
  static const int hi_shift[4] = { 4, 5, 6, 8 };
  uint32_t hash;

  hash = (enaddr[4] >> lo_shift[sc->sc_mchash_type]) |
	    (((uint16_t) enaddr[5]) << hi_shift[sc->sc_mchash_type]);

  return (hash & 0xfff);
}

/*
 * wm_set_filter: Set up the receive filter.
 */
static void wm_set_filter(struct wm_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct ether_multi *enm;
  struct ether_multistep step;
  uint32_t mta_reg;
  uint32_t hash, reg, bit;
  int i;

#ifdef WM_DEBUG
  printk("wm_set_filter(");
#endif  
  mta_reg = WMREG_CORDOVA_MTA;
  sc->sc_rctl &= ~(RCTL_BAM | RCTL_UPE | RCTL_MPE);

  /*  if (ifp->if_flags & IFF_BROADCAST)*/
     sc->sc_rctl |= RCTL_BAM;
  if (ifp->if_flags & IFF_PROMISC) {
     sc->sc_rctl |= RCTL_UPE;
     goto allmulti;
  }

  /*
   * Set the station address in the first RAL slot, and
   * clear the remaining slots.
   */
  wm_set_ral(sc, sc->arpcom.ac_enaddr, 0);
  for (i = 1; i < WM_RAL_TABSIZE; i++)
      wm_set_ral(sc, NULL, i);

  /* Clear out the multicast table. */
  for (i = 0; i < WM_MC_TABSIZE; i++)
      CSR_WRITE(sc,mta_reg + (i << 2), 0);

  ETHER_FIRST_MULTI(step, &sc->arpcom, enm);
  while (enm != NULL) {
	if (memcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
	    /*
	     * We must listen to a range of multicast addresses.
	     * For now, just accept all multicasts, rather than
	     * trying to set only those filter bits needed to match
	     * the range.  (At this time, the only use of address
	     * ranges is for IP multicast routing, for which the
	     * range is big enough to require all bits set.)
	     */
	    goto allmulti;
         }

         hash = wm_mchash(sc, enm->enm_addrlo);

         reg = (hash >> 5) & 0x7f;
         bit = hash & 0x1f;

	 hash = CSR_READ(sc,mta_reg + (reg << 2));
	 hash |= 1U << bit;

	 /* XXX Hardware bug?? */
	 if ((reg & 0xe) == 1) {
	    bit = CSR_READ(sc,mta_reg + ((reg - 1) << 2));
	    CSR_WRITE(sc,mta_reg + (reg << 2), hash);
	    CSR_WRITE(sc,mta_reg + ((reg - 1) << 2), bit);
	 } else
	    CSR_WRITE(sc,mta_reg + (reg << 2), hash);

         ETHER_NEXT_MULTI(step, enm);
  }

  ifp->if_flags &= ~IFF_ALLMULTI;
  goto setit;

 allmulti:
  ifp->if_flags |= IFF_ALLMULTI;
  sc->sc_rctl |= RCTL_MPE;

 setit:
  CSR_WRITE(sc,WMREG_RCTL, sc->sc_rctl);

#ifdef WM_DEBUG
  printk("RCTL 0x%x)\n", CSR_READ(sc,WMREG_RCTL));
#endif  
}

static void i82544EI_error(struct wm_softc *sc)
{
  struct ifnet	        *ifp = &sc->arpcom.ac_if;
  unsigned long		intr_status= sc->intr_errsts[sc->intr_err_ptr1++];

  /* read and reset the status; because this is written
   * by the ISR, we must disable interrupts here
   */
  sc->intr_err_ptr1 %=INTR_ERR_SIZE; /* Till Straumann */
  if (intr_status) {
    printk("Error %s%d:", ifp->if_name, ifp->if_unit);
    if (intr_status & ICR_RXSEQ) {
       printk("Rxq framing error (ICR= %x), if_ierrors %d\n",
	      intr_status, ifp->if_ierrors);
    }
  }
  else 
    printk("%s%d: Ghost interrupt ?\n",ifp->if_name,ifp->if_unit);
}

void i82544EI_printStats()
{
  i82544EI_stats(root_i82544EI_dev);
}

/* The daemon does all of the work; RX, TX and cleaning up buffers/descriptors */
static void i82544EI_daemon(void *arg)
{
  struct wm_softc *sc = (struct wm_softc*)arg;
  rtems_event_set	events;
  struct mbuf	*m=0;
  struct ifnet	*ifp=&sc->arpcom.ac_if;

#ifdef WM_DEBUG
  printk("i82544EI_daemon()\n");
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
     if (KILL_EVENT & events)  break;

     if (events & RX_EVENT)  i82544EI_rx(sc);

     /* clean up and try sending packets */
     do { 
	i82544EI_txq_done(sc);

        while (sc->txq_free>0) {
           if (sc->txq_free>TXQ_HiLmt_OFF) {
	      IF_DEQUEUE(&ifp->if_snd,m);
              if (m==0) break;
              i82544EI_sendpacket(sc, m); 
           }
           else {
	      i82544EI_txq_done(sc);
	      break;
           }
           if (events & RX_EVENT)  i82544EI_rx(sc);
        }
         /* we leave this loop
	  *  - either because there's no free buffer
          *    (m=0 initializer && !sc->txq_free)
	  *  - or there's nothing to send (IF_DEQUEUE
	  *    returned 0
	  */
     } while (m && sc->txq_free);

     ifp->if_flags &= ~IFF_OACTIVE;

     /* Log errors and other uncommon events. */
     if (events & ERR_EVENT) i82544EI_error(sc); 
     /* Rx overrun */
     if ( events & INIT_EVENT) {
        printk("Warnning, Rx overrun.  Make sure the old mbuf was free\n");
        i82544EI_ifinit(arg);
     }

  } /* end for(;;) { rtems_bsdnet_event_receive() .....*/

  printf("out of daemon\n");
  ifp->if_flags &= ~(IFF_RUNNING|IFF_OACTIVE);

  /* shut down the hardware */
  i82544EI_stop_hw(sc);
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
