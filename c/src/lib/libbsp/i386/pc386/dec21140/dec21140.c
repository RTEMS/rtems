/*
 * RTEMS driver for TULIP based Ethernet Controller
 *
 *  $Header$
 */

#include <bsp.h>
#include <pcibios.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <libcpu/cpu.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
 
#include <irq.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free 
#endif

#define PCI_VENDOR_ID_DEC 0x1011
#define PCI_DEVICE_ID_DEC_TULIP_FAST 0x0009

#define IO_MASK  0x3
#define MEM_MASK  0xF

/* command and status registers, 32-bit access, only if IO-ACCESS */
#define ioCSR0  0x00	/* bus mode register */
#define ioCSR1  0x08	/* transmit poll demand */
#define ioCSR2  0x10	/* receive poll demand */
#define ioCSR3  0x18	/* receive list base address */
#define ioCSR4  0x20	/* transmit list base address */
#define ioCSR5  0x28	/* status register */
#define ioCSR6  0x30	/* operation mode register */
#define ioCSR7  0x38	/* interrupt mask register */
#define ioCSR8  0x40	/* missed frame counter */
#define ioCSR9  0x48	/* Ethernet ROM register */
#define ioCSR10 0x50	/* reserved */
#define ioCSR11 0x58	/* full-duplex register */
#define ioCSR12 0x60	/* SIA status register */
#define ioCSR13 0x68
#define ioCSR14 0x70
#define ioCSR15 0x78	/* SIA general register */

/* command and status registers, 32-bit access, only if MEMORY-ACCESS */
#define memCSR0  0x00	/* bus mode register */
#define memCSR1  0x02	/* transmit poll demand */
#define memCSR2  0x04	/* receive poll demand */
#define memCSR3  0x06	/* receive list base address */
#define memCSR4  0x08	/* transmit list base address */
#define memCSR5  0x0A	/* status register */
#define memCSR6  0x0C	/* operation mode register */
#define memCSR7  0x0E	/* interrupt mask register */
#define memCSR8  0x10	/* missed frame counter */
#define memCSR9  0x12	/* Ethernet ROM register */
#define memCSR10 0x14	/* reserved */
#define memCSR11 0x16	/* full-duplex register */
#define memCSR12 0x18	/* SIA status register */
#define memCSR13 0x1A
#define memCSR14 0x1C
#define memCSR15 0x1E	/* SIA general register */

#define DEC_REGISTER_SIZE    0x100   /* to reserve virtual memory */

#define RESET_CHIP   0x00000001
#define CSR0_MODE    0x01a08000   /* 01a08000 */
#define ROM_ADDRESS  0x00004800
#define CSR6_INIT    0x020c0000   /* 020c0000 */  
#define CSR6_TX      0x00002000   
#define CSR6_TXRX    0x00002002   
#define IT_SETUP     0x00010040   /* 0001ebef */
#define CLEAR_IT     0xFFFFFFFF   
#define NO_IT        0x00000000   

#define NRXBUFS 7	/* number of receive buffers */
#define NTXBUFS 1	/* number of transmit buffers */

/* message descriptor entry */
struct MD {
    volatile unsigned long status;
    volatile unsigned long counts;
    unsigned long buf1, buf2;	
};

/*
 * Number of WDs supported by this driver
 */
#define NDECDRIVER	1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE	1520

#define	ET_MINLEN 60		/* minimum message length */

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
 struct dec21140_softc {
  struct arpcom			arpcom;
  rtems_irq_connect_data	irqInfo;
  struct MD			*MDbase;
  char				*bufferBase;
  int				acceptBroadcast;
  int				rxBdCount;
  int				txBdCount;
  rtems_id			rxDaemonTid;
  rtems_id			txDaemonTid;

  unsigned int 			port;
  unsigned int			*base;
  unsigned long			bpar;
   
  /*
   * Statistics
   */
  unsigned long	rxInterrupts;
  unsigned long	rxNotFirst;
  unsigned long	rxNotLast;
  unsigned long	rxGiant;
  unsigned long	rxNonOctet;
  unsigned long	rxRunt;
  unsigned long	rxBadCRC;
  unsigned long	rxOverrun;
  unsigned long	rxCollision;
  
  unsigned long	txInterrupts;
  unsigned long	txDeferred;
  unsigned long	txHeartbeat;
  unsigned long	txLateCollision;
  unsigned long	txRetryLimit;
  unsigned long	txUnderrun;
  unsigned long	txLostCarrier;
  unsigned long	txRawWait;
};

static struct dec21140_softc dec21140_softc[NDECDRIVER];

/*
 * DEC21140 interrupt handler
 */
static rtems_isr
dec21140Enet_interrupt_handler (rtems_vector_number v)
{
  unsigned int *tbase;
  unsigned long status;

  unsigned int sc;
  
  tbase = dec21140_softc[0].base ;

  /*
   * Read status
   */
  *(tbase+memCSR7) = NO_IT;
  status = *(tbase+memCSR5);
  *(tbase+memCSR5) = CLEAR_IT;

  /*
   * Frame received?
   */
  if (status & 0x00000040){
    dec21140_softc[0].rxInterrupts++;
    sc = rtems_event_send (dec21140_softc[0].rxDaemonTid, INTERRUPT_EVENT);
  }
}

static void nopOn(const rtems_irq_connect_data* notUsed)
{
  /*
   * code should be moved from dec21140Enet_initialize_hardware
   * to this location
   */
}

static int dec21140IsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}

/*
 * Read and write the MII registers using software-generated serial
 * MDIO protocol.
 */
#define MDIO_SHIFT_CLK		0x10000
#define MDIO_DATA_WRITE0 	0x00000
#define MDIO_DATA_WRITE1 	0x20000
#define MDIO_ENB		0x00000	 
#define MDIO_ENB_IN		0x40000
#define MDIO_DATA_READ		0x80000

static int mdio_read(unsigned int *ioaddr, int phy_id, int location)
{
	int i, i3;
	int read_cmd = (0xf6 << 10) | (phy_id << 5) | location;
	unsigned short retval = 0;

	/* Establish sync by sending at least 32 logic ones. */ 
	for (i = 32; i >= 0; i--) {
		*ioaddr = MDIO_ENB | MDIO_DATA_WRITE1;
		for(i3=0; i3<1000; i3++);
		*ioaddr =  MDIO_ENB | MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
	}
	/* Shift the read command bits out. */
	for (i = 17; i >= 0; i--) {
		int dataval = (read_cmd & (1 << i)) ? MDIO_DATA_WRITE1 : 0;
		*ioaddr =  dataval;
		for(i3=0; i3<1000; i3++);
		*ioaddr =  dataval | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
		*ioaddr =  dataval;
		for(i3=0; i3<1000; i3++);
	}
	*ioaddr =  MDIO_ENB_IN | MDIO_SHIFT_CLK;
	for(i3=0; i3<1000; i3++);
	*ioaddr =  MDIO_ENB_IN;

	for (i = 16; i > 0; i--) {
		*ioaddr =  MDIO_ENB_IN | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
		retval = (retval << 1) | ((*ioaddr & MDIO_DATA_READ) ? 1 : 0);
		*ioaddr =  MDIO_ENB_IN;
		for(i3=0; i3<1000; i3++);
	}
	/* Clear out extra bits. */
	for (i = 16; i > 0; i--) {
		*ioaddr =  MDIO_ENB_IN | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
		*ioaddr =  MDIO_ENB_IN;
		for(i3=0; i3<1000; i3++);
	}
	return retval;
}

static int mdio_write(unsigned int *ioaddr, int phy_id, int location, int value)
{
	int i, i3;
	int cmd = (0x5002 << 16) | (phy_id << 23) | (location << 18) | value;

	/* Establish sync by sending at least 32 logic ones. */ 
	for (i = 32; i >= 0; i--) {
		*ioaddr =  MDIO_ENB | MDIO_DATA_WRITE1;
		for(i3=0; i3<1000; i3++);
		*ioaddr = MDIO_ENB | MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
	}
	/* Shift the read command bits out. */
	for (i = 31; i >= 0; i--) {
		int dataval = (cmd & (1 << i)) ? MDIO_DATA_WRITE1 : 0;
		*ioaddr =  dataval;
		for(i3=0; i3<1000; i3++);
		*ioaddr =  dataval | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
	}

	/* Clear out extra bits. */
	for (i = 2; i > 0; i--) {
		*ioaddr =  MDIO_ENB_IN;
		for(i3=0; i3<1000; i3++);
		*ioaddr = MDIO_ENB_IN | MDIO_SHIFT_CLK;
		for(i3=0; i3<1000; i3++);
	}
	return 0;


}

/*
 * This routine reads a word (16 bits) from the serial EEPROM.
 */
/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK		0x02	/* EEPROM shift clock. */
#define EE_CS			0x01	/* EEPROM chip select. */
#define EE_DATA_WRITE		0x04	/* EEPROM chip data in. */
#define EE_WRITE_0		0x01
#define EE_WRITE_1		0x05
#define EE_DATA_READ		0x08	/* EEPROM chip data out. */
#define EE_ENB			(0x4800 | EE_CS)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD	(5 << 6)
#define EE_READ_CMD	(6 << 6)
#define EE_ERASE_CMD	(7 << 6)

static int eeget16(unsigned int *ioaddr, int location)
{
	int i, i3;
	unsigned short retval = 0;
	int read_cmd = location | EE_READ_CMD;
	
	*ioaddr = EE_ENB & ~EE_CS;
	*ioaddr = EE_ENB;
	
	/* Shift the read command bits out. */
	for (i = 10; i >= 0; i--) {
		short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		*ioaddr = EE_ENB | dataval;
		for (i3=0; i3<1000; i3++) ;
		*ioaddr = EE_ENB | dataval | EE_SHIFT_CLK;
		for (i3=0; i3<1000; i3++) ;
		*ioaddr = EE_ENB | dataval; /* Finish EEPROM a clock tick. */
		for (i3=0; i3<1000; i3++) ;
	}
	*ioaddr = EE_ENB;
	
	for (i = 16; i > 0; i--) {
		*ioaddr = EE_ENB | EE_SHIFT_CLK;
		for (i3=0; i3<1000; i3++) ;
		retval = (retval << 1) | ((*ioaddr & EE_DATA_READ) ? 1 : 0);
		*ioaddr = EE_ENB;
		for (i3=0; i3<1000; i3++) ;
	}

	/* Terminate the EEPROM access. */
	*ioaddr = EE_ENB & ~EE_CS;
	return retval;
}

/*
 * Initialize the ethernet hardware
 */
static void
dec21140Enet_initialize_hardware (struct dec21140_softc *sc)
{
  rtems_status_code st;
  unsigned int *tbase;
  union {char c[64]; unsigned short s[32];} rombuf;
  int i, i2, i3;
  char *cp, direction, *setup_frm, *eaddrs; 
  unsigned long csr12_val, mii_reg0;
  unsigned char *buffer;
  struct MD *rmd;

  
  tbase = sc->base;

  /*
   * WARNING : First write in CSR6
   *           Then Reset the chip ( 1 in CSR0)
   */

  *(tbase+memCSR6) = CSR6_INIT;  
  *(tbase+memCSR0) = RESET_CHIP;  
  for(i3=0; i3<1000; i3++);

  /*
   * Init CSR0
   */
  *(tbase+memCSR0) = CSR0_MODE;  

  csr12_val = *(tbase+memCSR8);
  
  for (i=0; i<32; i++)
    rombuf.s[i] = eeget16(tbase+memCSR9, i);
  memcpy (sc->arpcom.ac_enaddr, rombuf.c+20, ETHER_ADDR_LEN);


  mii_reg0 = mdio_read(tbase+memCSR9, 0, 0); 
  mdio_write(tbase+memCSR9, 0, 0, mii_reg0 | 0x1000);
 
#ifdef DEC_DEBUG
  printk("DC21140 %x:%x:%x:%x:%x:%x IRQ %d IO %x M %x .........\n",
	 sc->arpcom.ac_enaddr[0], sc->arpcom.ac_enaddr[1],
	 sc->arpcom.ac_enaddr[2], sc->arpcom.ac_enaddr[3],
	 sc->arpcom.ac_enaddr[4], sc->arpcom.ac_enaddr[5],
	 sc->irqInfo.name, sc->port, sc->base);
#endif
  
  /*
   * Init RX ring
   */
  sc->rxBdCount = 0;
  
  cp = (char *)malloc((NRXBUFS+NTXBUFS)*(sizeof(struct MD)+ RBUF_SIZE) + PG_SIZE);
  sc->bufferBase = cp;
  cp += (PG_SIZE - (int)cp) & MASK_OFFSET;
#ifdef PCI_BRIDGE_DOES_NOT_ENSURE_CACHE_COHERENCY_FOR_DMA 
  if (_CPU_is_paging_enabled())
    _CPU_change_memory_mapping_attribute
                   (NULL, cp,
		    (NRXBUFS+NTXBUFS)*(sizeof(struct MD)+ RBUF_SIZE),
		    PTE_CACHE_DISABLE | PTE_WRITABLE);
#endif
  rmd = (struct MD*)cp;
  sc->MDbase = rmd;
  buffer = cp + ((NRXBUFS+NTXBUFS)*sizeof(struct MD));
  
  *(tbase+memCSR3) = (long)(sc->MDbase);
  for (i=0 ; i<NRXBUFS; i++){
    rmd->buf2 = 0;
    rmd->buf1 = (unsigned long)(buffer + (i*RBUF_SIZE));  
    if (i == NRXBUFS-1)
      rmd->counts = 0xfec00000 | (RBUF_SIZE);
    else
      rmd->counts = 0xfcc00000 | (RBUF_SIZE);
    rmd->status = 0x80000000;
    rmd++;
  }

  /*
   * Init TX ring
   */
  sc->txBdCount = 0;
  *(tbase+memCSR4) = (long)(rmd);
  rmd->buf2 = 0;
  rmd->buf1 = (unsigned long)(buffer + (NRXBUFS*RBUF_SIZE));
  rmd->counts = 0x62000000;
  rmd->status = 0x0;
  
  /*
   * Set up interrupts
   */
  *(tbase+memCSR5) = IT_SETUP;
  *(tbase+memCSR7) = IT_SETUP; 

  sc->irqInfo.hdl = (rtems_irq_hdl)dec21140Enet_interrupt_handler;
  sc->irqInfo.on  = nopOn;
  sc->irqInfo.off = nopOn;
  sc->irqInfo.isOn = dec21140IsOn;  
  st = BSP_install_rtems_irq_handler (&sc->irqInfo);
  if (!st)
    rtems_panic ("Can't attach DEC21140 interrupt handler for irq %d\n",
		  sc->irqInfo.name);

  /*
   * Start TX for setup frame
   */
  *(tbase+memCSR6) = CSR6_INIT | CSR6_TX;

  /*
   * Build setup frame
   */
  setup_frm = (char *)(rmd->buf1);
  eaddrs = (char *)(sc->arpcom.ac_enaddr);
  /* Fill the buffer with our physical address. */
  for (i = 1; i < 16; i++) {
	*setup_frm++ = eaddrs[0];
	*setup_frm++ = eaddrs[1];
	setup_frm += 2;
	*setup_frm++ = eaddrs[2];
	*setup_frm++ = eaddrs[3];
	setup_frm += 2;
	*setup_frm++ = eaddrs[4];
	*setup_frm++ = eaddrs[5];
	setup_frm += 2;
  }
  /* Add the broadcast address when doing perfect filtering */
  memset(setup_frm, 0xff, 12);
  rmd->counts = 0x0a000000 | 192 ;
  rmd->status = 0x80000000;
  *(tbase+memCSR1) = 1;
  while (rmd->status != 0x7fffffff);

  /*
   * Enable RX and TX
   */
  *(tbase+memCSR6) = CSR6_INIT | CSR6_TXRX;
  
  /*
   * Set up PHY
   */
  
  i = rombuf.c[27];
  i+=2;
  direction = rombuf.c[i];
  i +=4;
  *(tbase+memCSR12) = direction | 0x100;
  for (i2 = 0; i2 < rombuf.c[(i+2) + rombuf.c[i+1]]; i2++){
    *(tbase + memCSR12) = rombuf.c[(i+3) + rombuf.c[i+1] + i2];
  }
  for (i2 = 0; i2 < rombuf.c[i+1]; i2++){
    *(tbase + memCSR12) = rombuf.c[(i+2) + i2];
  }
}

static void
dec21140_rxDaemon (void *arg)
{
  unsigned int *tbase;
  struct ether_header *eh;
  struct dec21140_softc *dp = (struct dec21140_softc *)&dec21140_softc[0];
  struct ifnet *ifp = &dp->arpcom.ac_if;
  struct mbuf *m;
  struct MD *rmd;
  unsigned int len;
  char *temp;
  rtems_event_set events;
  int nbMD;
  
  tbase = dec21140_softc[0].base ;

  for (;;){

    rtems_bsdnet_event_receive (INTERRUPT_EVENT,
				RTEMS_WAIT|RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT,
				&events);
    rmd = dec21140_softc[0].MDbase;
    nbMD = 0;
    
    while (nbMD < NRXBUFS){
      if ( (rmd->status & 0x80000000) == 0){
	len = (rmd->status >> 16) & 0x7ff;
	MGETHDR (m, M_WAIT, MT_DATA);
	MCLGET (m, M_WAIT);
	m->m_pkthdr.rcvif = ifp;
	temp = m->m_data;
	m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);
	memcpy(temp, (char *)rmd->buf1, len);
	rmd->status = 0x80000000;
	eh = mtod (m, struct ether_header *);
	m->m_data += sizeof(struct ether_header);
	ether_input (ifp, eh, m);
      }
      rmd++;
      nbMD++;
    }
    *(tbase+memCSR7) = IT_SETUP; 
  }	
}

static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct dec21140_softc *dp = ifp->if_softc;
  volatile struct MD *tmd;
  unsigned char *temp;
  struct mbuf *n;
  unsigned int len;
  unsigned int *tbase;

  tbase = dp->base;

  /*
   * Waiting for Transmitter ready
   */	
  tmd = dec21140_softc[0].MDbase + NRXBUFS;
  while ( (tmd->status & 0x80000000) != 0 );
  len = 0;
  n = m;
  temp = (char *)(tmd->buf1);
  
  for (;;){
    len += m->m_len;
    memcpy(temp, (char *)m->m_data, m->m_len);
    temp += m->m_len ;
    if ((m = m->m_next) == NULL)
      break;
  }

  if (len < ET_MINLEN) len = ET_MINLEN;
  tmd->counts = 0xe2000000 | len;
  tmd->status = 0x80000000;

  *(tbase+memCSR1) = 0x1;

  m_freem(n);
}

/*
 * Driver transmit daemon
 */
void
dec21140_txDaemon (void *arg)
{
  struct dec21140_softc *sc = (struct dec21140_softc *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
    /*
     * Wait for packet
     */

    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);

    /*
     * Send packets till queue is empty
     */
    for (;;) {
      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);
      if (!m)
	break;
      sendpacket (ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}	


static void
dec21140_start (struct ifnet *ifp)
{
	struct dec21140_softc *sc = ifp->if_softc;

	rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
	ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
dec21140_init (void *arg)
{
  struct dec21140_softc *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {
    
    /*
     * Set up DEC21140 hardware
     */
    dec21140Enet_initialize_hardware (sc);
    
    /*
     * Start driver tasks
     */
    sc->rxDaemonTid = rtems_bsdnet_newproc ("DCrx", 4096,
					    dec21140_rxDaemon, sc);
    sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
					    dec21140_txDaemon, sc);
  }

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

}

/*
 * Stop the device
 */
static void
dec21140_stop (struct dec21140_softc *sc)
{
  unsigned int *tbase;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Stop the transmitter
   */
  tbase=dec21140_softc[0].base ;
  *(tbase+memCSR7) = NO_IT;
  *(tbase+memCSR6) = CSR6_INIT;
  free(sc->bufferBase);
}


/*
 * Show interface statistics
 */
static void
dec21140_stats (struct dec21140_softc *sc)
{
	printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
	printf ("       Not First:%-8lu", sc->rxNotFirst);
	printf ("        Not Last:%-8lu\n", sc->rxNotLast);
	printf ("              Giant:%-8lu", sc->rxGiant);
	printf ("            Runt:%-8lu", sc->rxRunt);
	printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
	printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
	printf ("         Overrun:%-8lu", sc->rxOverrun);
	printf ("       Collision:%-8lu\n", sc->rxCollision);

	printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
	printf ("        Deferred:%-8lu", sc->txDeferred);
	printf (" Missed Hearbeat:%-8lu\n", sc->txHeartbeat);
	printf ("         No Carrier:%-8lu", sc->txLostCarrier);
	printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
	printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
	printf ("           Underrun:%-8lu", sc->txUnderrun);
	printf (" Raw output wait:%-8lu\n", sc->txRawWait);
}

/*
 * Driver ioctl handler
 */
static int
dec21140_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
	struct dec21140_softc *sc = ifp->if_softc;
	int error = 0;

	switch (command) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		ether_ioctl (ifp, command, data);
		break;

	case SIOCSIFFLAGS:
		switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
		case IFF_RUNNING:
			dec21140_stop (sc);
			break;

		case IFF_UP:
			dec21140_init (sc);
			break;

		case IFF_UP | IFF_RUNNING:
			dec21140_stop (sc);
			dec21140_init (sc);
			break;

		default:
			break;
		}
		break;

	case SIO_RTEMS_SHOW_STATS:
		dec21140_stats (sc);
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

/*
 * Attach an DEC21140 driver to the system
 */
int
rtems_dec21140_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
	struct dec21140_softc *sc;
	struct ifnet *ifp;
	int mtu;
	int i;
	int signature;
	int value;
	char interrupt;
	int diag;
	
	/*
	 * Initialise PCI module
	 */
	if (pcib_init() == PCIB_ERR_NOTPRESENT)
	  rtems_panic("PCI BIOS not found !!");
	
	/*
	 * First, find a DEC board
	 */
	if ((diag = pcib_find_by_devid(PCI_VENDOR_ID_DEC,
				       PCI_DEVICE_ID_DEC_TULIP_FAST,
				       0,
				       &signature)) != PCIB_ERR_SUCCESS)
	  rtems_panic("DEC PCI board not found !! (%d)\n", diag);
	else {
	  printk("DEC PCI Device found\n");
	}
	
	/*
	 * Find a free driver
	 */
	for (i = 0 ; i < NDECDRIVER ; i++) {
		sc = &dec21140_softc[i];
		ifp = &sc->arpcom.ac_if;
		if (ifp->if_softc == NULL)
			break;
	}
	if (i >= NDECDRIVER) {
		printk ("Too many DEC drivers.\n");
		return 0;
	}

	/*
	 * Process options
	 */

	pcib_conf_read32(signature, 16, &value);
	sc->port = value & ~IO_MASK;
        
	pcib_conf_read32(signature, 20, &value);
	if (_CPU_is_paging_enabled())
	  _CPU_map_phys_address(&(sc->base),
				(void *)(value & ~MEM_MASK),
				DEC_REGISTER_SIZE ,
				PTE_CACHE_DISABLE | PTE_WRITABLE);
	else
	  sc->base = (unsigned int *)(value & ~MEM_MASK);
	
	pcib_conf_read8(signature, 60, &interrupt);
	  sc->irqInfo.name = (rtems_irq_symbolic_name)interrupt;
	
 	if (config->hardware_address) {
	  memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
		  ETHER_ADDR_LEN);
	}
	else {
	  memset (sc->arpcom.ac_enaddr, 0x08,ETHER_ADDR_LEN);
	}
	if (config->mtu)
		mtu = config->mtu;
	else
		mtu = ETHERMTU;

	sc->acceptBroadcast = !config->ignore_broadcast;

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = sc;
	ifp->if_unit = i + 1;
	ifp->if_name = "dc";
	ifp->if_mtu = mtu;
	ifp->if_init = dec21140_init;
	ifp->if_ioctl = dec21140_ioctl;
	ifp->if_start = dec21140_start;
	ifp->if_output = ether_output;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
	if (ifp->if_snd.ifq_maxlen == 0)
		ifp->if_snd.ifq_maxlen = ifqmaxlen;

 	/*
	 * Attach the interface
	 */
	if_attach (ifp);
	ether_ifattach (ifp);

	return 1;
};











