/*
 *  RTEMS driver for TULIP based Ethernet Controller
 *
 *  Copyright (C) 1999 Emmanuel Raguet. raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * ------------------------------------------------------------------------
 * [22.05.2000,StWi/CWA] added support for the DEC/Intel 21143 chip
 *
 * Thanks go to Andrew Klossner who provided the vital information about the
 * Intel 21143 chip.  FWIW: The 21143 additions to this driver were initially
 * tested with a PC386 BSP using a Kingston KNE100TX with 21143PD chip.
 *
 * The driver will automatically detect whether there is a 21140 or 21143
 * network card in the system and activate support accordingly. It will
 * look for the 21140 first. If the 21140 is not found the driver will
 * look for the 21143.
 *
 * 2004-11-10, Joel/Richard - 21143 support works on MVME2100.
 * ------------------------------------------------------------------------
 *
 * 2003-03-13, Greg Menke, gregory.menke@gsfc.nasa.gov
 *
 * Added support for up to 8 units (which is an arbitrary limit now),
 * consolidating their support into a single pair of rx/tx daemons and a
 * single ISR for all vectors servicing the DEC units.  The driver now
 * simply uses whatever INTERRUPT_LINE the card supplies, requiring it
 * be configured either by the boot monitor or bspstart() hackery.
 * Tested on a MCP750 PPC based system with 2 DEC21140 boards.
 *
 * Also fixed a few bugs related to board configuration, start and stop.
 *
 */

#include <rtems.h>

/*
 *  This driver only supports architectures with the new style
 *  exception processing.  The following checks try to keep this
 *  from being compiled on systems which can't support this driver.
 */

#if defined(__i386__)
  #define DEC21140_SUPPORTED
  #define PCI_DRAM_OFFSET 0
#endif
#if defined(__PPC__)
  #define DEC21140_SUPPORTED
#endif

#include <bsp.h>

#if !defined(PCI_DRAM_OFFSET)
  #undef DEC21140_SUPPORTED
#endif

#if defined(DEC21140_SUPPORTED)
#include <rtems/pci.h>

#if defined(__PPC__)
#include <libcpu/byteorder.h>
#include <libcpu/io.h>
#endif

#if defined(__i386__)
#include <libcpu/byteorder.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <bsp/irq.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

#define DEC_DEBUG

/* note: the 21143 isn't really a DEC, it's an Intel chip */
#define PCI_VENDOR_ID_DEC               0x1011
#define PCI_DEVICE_ID_DEC_21140         0x0009
#define PCI_DEVICE_ID_DEC_21143         0x0019

#define DRIVER_PREFIX   "dc"

#define IO_MASK   0x3
#define MEM_MASK  0xF

/* command and status registers, 32-bit access, only if IO-ACCESS */
#define ioCSR0  0x00    /* bus mode register */
#define ioCSR1  0x08    /* transmit poll demand */
#define ioCSR2  0x10    /* receive poll demand */
#define ioCSR3  0x18    /* receive list base address */
#define ioCSR4  0x20    /* transmit list base address */
#define ioCSR5  0x28    /* status register */
#define ioCSR6  0x30    /* operation mode register */
#define ioCSR7  0x38    /* interrupt mask register */
#define ioCSR8  0x40    /* missed frame counter */
#define ioCSR9  0x48    /* Ethernet ROM register */
#define ioCSR10 0x50    /* reserved */
#define ioCSR11 0x58    /* full-duplex register */
#define ioCSR12 0x60    /* SIA status register */
#define ioCSR13 0x68
#define ioCSR14 0x70
#define ioCSR15 0x78    /* SIA general register */

/* command and status registers, 32-bit access, only if MEMORY-ACCESS */
#define memCSR0  0x00    /* bus mode register */
#define memCSR1  0x02    /* transmit poll demand */
#define memCSR2  0x04    /* receive poll demand */
#define memCSR3  0x06    /* receive list base address */
#define memCSR4  0x08    /* transmit list base address */
#define memCSR5  0x0A    /* status register */
#define memCSR6  0x0C    /* operation mode register */
#define memCSR7  0x0E    /* interrupt mask register */
#define memCSR8  0x10    /* missed frame counter */
#define memCSR9  0x12    /* Ethernet ROM register */
#define memCSR10 0x14    /* reserved */
#define memCSR11 0x16    /* full-duplex register */
#define memCSR12 0x18    /* SIA status register */
#define memCSR13 0x1A
#define memCSR14 0x1C
#define memCSR15 0x1E    /* SIA general register */

#define DEC_REGISTER_SIZE    0x100   /* to reserve virtual memory */




#define RESET_CHIP   0x00000001
#if defined(__PPC__)
#define CSR0_MODE    0x0030e002   /* 01b08000 */
#else
#define CSR0_MODE    0x0020e002   /* 01b08000 */
#endif
#define ROM_ADDRESS  0x00004800
#define CSR6_INIT    0x022cc000   /* 022c0000 020c0000 */
#define CSR6_TX      0x00002000
#define CSR6_TXRX    0x00002002
#define IT_SETUP     0x000100c0   /* 000100e0 */
#define CLEAR_IT     0xFFFFFFFF
#define NO_IT        0x00000000

/* message descriptor entry */
struct MD {
    /* used by hardware */
    volatile uint32_t   status;
    volatile uint32_t   counts;
    volatile uint32_t   buf1, buf2;
    /* used by software */
    volatile struct mbuf *m;
    volatile struct MD *next;
} __attribute__ ((packed));

/*
** These buffers allocated for each unit, so ensure
**
**   rtems_bsdnet_config.mbuf_bytecount
**   rtems_bsdnet_config.mbuf_cluster_bytecount
**
** are adequately sized to provide enough clusters and mbufs for all the
** units.  The default bsdnet configuration is sufficient for one dec
** unit, but will be nearing exhaustion with 2 or more.  Although a
** little expensive in memory, the following configuration should
** eliminate all mbuf/cluster issues;
**
**   rtems_bsdnet_config.mbuf_bytecount           = 128*1024;
**   rtems_bsdnet_config.mbuf_cluster_bytecount   = 256*1024;
*/

#define NRXBUFS 16    /* number of receive buffers */
#define NTXBUFS 16    /* number of transmit buffers */

/*
 * Number of DEC boards supported by this driver
 */
#define NDECDRIVER    8

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE    1536

#define ET_MINLEN       60    /* minimum message length */

/*
** Events, one per unit.  The event is sent to the rx task from the isr
** or from the stack to the tx task whenever a unit needs service.  The
** rx/tx tasks identify the requesting unit(s) by their particular
** events so only requesting units are serviced.
*/

static rtems_event_set unit_signals[NDECDRIVER]= { RTEMS_EVENT_1,
                                                   RTEMS_EVENT_2,
                                                   RTEMS_EVENT_3,
                                                   RTEMS_EVENT_4,
                                                   RTEMS_EVENT_5,
                                                   RTEMS_EVENT_6,
                                                   RTEMS_EVENT_7,
                                                   RTEMS_EVENT_8 };

#if defined(__PPC__)
#define phys_to_bus(address) ((unsigned int)((address)) + PCI_DRAM_OFFSET)
#define bus_to_phys(address) ((unsigned int)((address)) - PCI_DRAM_OFFSET)
#define CPU_CACHE_ALIGNMENT_FOR_BUFFER PPC_CACHE_ALIGNMENT
#else
extern void Wait_X_ms( unsigned int timeToWait );
#define phys_to_bus(address) ((unsigned int) ((address)))
#define bus_to_phys(address) ((unsigned int) ((address)))
#define rtems_bsp_delay_in_bus_cycles(cycle) Wait_X_ms( cycle/100 )
#define CPU_CACHE_ALIGNMENT_FOR_BUFFER PG_SIZE
#endif

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct dec21140_softc {

      struct arpcom             arpcom;

      rtems_irq_connect_data    irqInfo;
      rtems_event_set           ioevent;

      int                       numRxbuffers, numTxbuffers;

      volatile struct MD        *MDbase;
      volatile struct MD        *nextRxMD;
      volatile unsigned char   *bufferBase;
      int                       acceptBroadcast;

      volatile struct MD       *TxMD;
      volatile struct MD       *SentTxMD;
      int                       PendingTxCount;
      int                       TxSuspended;

      unsigned int              port;
      volatile uint32_t        *base;

      /*
       * Statistics
       */
      unsigned long     rxInterrupts;
      unsigned long     rxNotFirst;
      unsigned long     rxNotLast;
      unsigned long     rxGiant;
      unsigned long     rxNonOctet;
      unsigned long     rxRunt;
      unsigned long     rxBadCRC;
      unsigned long     rxOverrun;
      unsigned long     rxCollision;

      unsigned long     txInterrupts;
      unsigned long     txDeferred;
      unsigned long     txHeartbeat;
      unsigned long     txLateCollision;
      unsigned long     txRetryLimit;
      unsigned long     txUnderrun;
      unsigned long     txLostCarrier;
      unsigned long     txRawWait;
};

static struct dec21140_softc dec21140_softc[NDECDRIVER];
static rtems_id rxDaemonTid;
static rtems_id txDaemonTid;

/*
 * This routine reads a word (16 bits) from the serial EEPROM.
 */
/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK    0x02    /* EEPROM shift clock. */
#define EE_CS           0x01    /* EEPROM chip select. */
#define EE_DATA_WRITE   0x04    /* EEPROM chip data in. */
#define EE_WRITE_0      0x01
#define EE_WRITE_1      0x05
#define EE_DATA_READ    0x08    /* EEPROM chip data out. */
#define EE_ENB          (0x4800 | EE_CS)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD    (5 << 6)
#define EE_READ_CMD     (6 << 6)
#define EE_ERASE_CMD    (7 << 6)

static int eeget16(volatile uint32_t *ioaddr, int location)
{
   int i;
   unsigned short retval = 0;
   int read_cmd = location | EE_READ_CMD;

   st_le32(ioaddr, EE_ENB & ~EE_CS);
   st_le32(ioaddr, EE_ENB);

   /* Shift the read command bits out. */
   for (i = 10; i >= 0; i--) {
      short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
      st_le32(ioaddr, EE_ENB | dataval);
      rtems_bsp_delay_in_bus_cycles(200);
      st_le32(ioaddr, EE_ENB | dataval | EE_SHIFT_CLK);
      rtems_bsp_delay_in_bus_cycles(200);
      st_le32(ioaddr, EE_ENB | dataval); /* Finish EEPROM a clock tick. */
      rtems_bsp_delay_in_bus_cycles(200);
   }
   st_le32(ioaddr, EE_ENB);

   for (i = 16; i > 0; i--) {
      st_le32(ioaddr, EE_ENB | EE_SHIFT_CLK);
      rtems_bsp_delay_in_bus_cycles(200);
      retval = (retval << 1) | ((ld_le32(ioaddr) & EE_DATA_READ) ? 1 : 0);
      st_le32(ioaddr, EE_ENB);
      rtems_bsp_delay_in_bus_cycles(200);
   }

   /* Terminate the EEPROM access. */
   st_le32(ioaddr, EE_ENB & ~EE_CS);
   return ( ((retval<<8)&0xff00) | ((retval>>8)&0xff) );
}

static void no_op(const rtems_irq_connect_data* irq)
{
   return;
}

static int dec21140IsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}

/*
 * DEC21140 interrupt handler
 */
static rtems_isr
dec21140Enet_interrupt_handler ( struct dec21140_softc *sc )
{
   volatile uint32_t      *tbase;
   uint32_t               status;

   tbase = (uint32_t*)(sc->base);

   /*
    * Read status
    */
   status = ld_le32(tbase+memCSR5);
   st_le32((tbase+memCSR5), status);

   /*
    * Frame received?
    */
   if( status & 0x000000c0 )
   {
      sc->rxInterrupts++;
      rtems_event_send(rxDaemonTid, sc->ioevent);
   }
}

static rtems_isr
dec21140Enet_interrupt_handler_entry(void)
{
   int i;

   /*
   ** Check all the initialized dec units for interrupt service
   */

   for(i=0; i< NDECDRIVER; i++ )
   {
      if( dec21140_softc[i].base )
         dec21140Enet_interrupt_handler( &dec21140_softc[i] );
   }
}

/*
 * Initialize the ethernet hardware
 */
static void
dec21140Enet_initialize_hardware (struct dec21140_softc *sc)
{
   int i,st;
   volatile uint32_t      *tbase;
   volatile unsigned char *cp, *setup_frm, *eaddrs;
   volatile unsigned char *buffer;
   volatile struct MD     *rmd;


#ifdef DEC_DEBUG
   printk("dec2114x : %02x:%02x:%02x:%02x:%02x:%02x   name '%s%d', io %x, mem %x, int %d\n",
          sc->arpcom.ac_enaddr[0], sc->arpcom.ac_enaddr[1],
          sc->arpcom.ac_enaddr[2], sc->arpcom.ac_enaddr[3],
          sc->arpcom.ac_enaddr[4], sc->arpcom.ac_enaddr[5],
          sc->arpcom.ac_if.if_name, sc->arpcom.ac_if.if_unit,
          sc->port, (unsigned) sc->base, sc->irqInfo.name );
#endif

   tbase = sc->base;

   /*
    * WARNING : First write in CSR6
    *           Then Reset the chip ( 1 in CSR0)
    */
   st_le32( (tbase+memCSR6), CSR6_INIT);
   st_le32( (tbase+memCSR0), RESET_CHIP);
   rtems_bsp_delay_in_bus_cycles(200);

   st_le32( (tbase+memCSR7), NO_IT);

   /*
    * Init CSR0
    */
   st_le32( (tbase+memCSR0), CSR0_MODE);

   /*
    * Init RX ring
    */
   cp = (volatile unsigned char *)malloc(((sc->numRxbuffers+sc->numTxbuffers)*sizeof(struct MD))
                                         + (sc->numTxbuffers*RBUF_SIZE)
                                         + CPU_CACHE_ALIGNMENT_FOR_BUFFER);
   sc->bufferBase = cp;
   cp += (CPU_CACHE_ALIGNMENT_FOR_BUFFER - (int)cp) & (CPU_CACHE_ALIGNMENT_FOR_BUFFER - 1);
#if defined(__i386__)
#ifdef PCI_BRIDGE_DOES_NOT_ENSURE_CACHE_COHERENCY_FOR_DMA
   if (_CPU_is_paging_enabled())
      _CPU_change_memory_mapping_attribute
         (NULL, cp,
          ((sc->numRxbuffers+sc->numTxbuffers)*sizeof(struct MD))
          + (sc->numTxbuffers*RBUF_SIZE),
          PTE_CACHE_DISABLE | PTE_WRITABLE);
#endif
#endif
   rmd = (volatile struct MD*)cp;
   sc->MDbase = rmd;
   sc->nextRxMD = sc->MDbase;

   buffer = cp + ((sc->numRxbuffers+sc->numTxbuffers)*sizeof(struct MD));
   st_le32( (tbase+memCSR3), (long)(phys_to_bus((long)(sc->MDbase))));

   for (i=0 ; i<sc->numRxbuffers; i++)
   {
      struct mbuf *m;

      /* allocate an mbuf for each receive descriptor */
      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
      rmd->m = m;

      rmd->buf2   = phys_to_bus(rmd+1);
      rmd->buf1   = phys_to_bus(mtod(m, void *));
      rmd->status = 0x80000000;
      rmd->counts = 0xfdc00000 | (RBUF_SIZE);
      rmd->next   = rmd+1;
      rmd++;
   }
   /*
    * mark last RX buffer.
    */
   sc->MDbase [sc->numRxbuffers-1].buf2   = 0;
   sc->MDbase [sc->numRxbuffers-1].counts = 0xfec00000 | (RBUF_SIZE);
   sc->MDbase [sc->numRxbuffers-1].next   = sc->MDbase;



   /*
    * Init TX ring
    */
   st_le32( (tbase+memCSR4), (long)(phys_to_bus((long)(rmd))) );
   for (i=0 ; i<sc->numTxbuffers; i++){
      (rmd+i)->buf2   = phys_to_bus(rmd+i+1);
      (rmd+i)->buf1   = phys_to_bus(buffer + (i*RBUF_SIZE));
      (rmd+i)->counts = 0x01000000;
      (rmd+i)->status = 0x0;
      (rmd+i)->next   = rmd+i+1;
      (rmd+i)->m      = 0;
   }

   /*
    * mark last TX buffer.
    */
   (rmd+sc->numTxbuffers-1)->buf2   = phys_to_bus(rmd);
   (rmd+sc->numTxbuffers-1)->next   = rmd;


   /*
    * Build setup frame
    */
   setup_frm = (volatile unsigned char *)(bus_to_phys(rmd->buf1));
   eaddrs = (unsigned char *)(sc->arpcom.ac_enaddr);
   /* Fill the buffer with our physical address. */
   for (i = 1; i < 16; i++) {
      *setup_frm++ = eaddrs[0];
      *setup_frm++ = eaddrs[1];
      *setup_frm++ = eaddrs[0];
      *setup_frm++ = eaddrs[1];
      *setup_frm++ = eaddrs[2];
      *setup_frm++ = eaddrs[3];
      *setup_frm++ = eaddrs[2];
      *setup_frm++ = eaddrs[3];
      *setup_frm++ = eaddrs[4];
      *setup_frm++ = eaddrs[5];
      *setup_frm++ = eaddrs[4];
      *setup_frm++ = eaddrs[5];
   }

   /* Add the broadcast address when doing perfect filtering */
   memset((void*) setup_frm, 0xff, 12);
   rmd->counts = 0x09000000 | 192 ;
   rmd->status = 0x80000000;
   st_le32( (tbase+memCSR6), CSR6_INIT | CSR6_TX);
   st_le32( (tbase+memCSR1), 1);

   while (rmd->status != 0x7fffffff);
   rmd->counts = 0x01000000;

   sc->TxMD = rmd+1;

   sc->irqInfo.hdl  = (rtems_irq_hdl)dec21140Enet_interrupt_handler_entry;
   sc->irqInfo.on   = no_op;
   sc->irqInfo.off  = no_op;
   sc->irqInfo.isOn = dec21140IsOn;

#ifdef DEC_DEBUG
   printk( "dec2114x: Installing IRQ %d\n", sc->irqInfo.name );
#endif
#ifdef BSP_SHARED_HANDLER_SUPPORT
   st = BSP_install_rtems_shared_irq_handler( &sc->irqInfo );
#else
   st = BSP_install_rtems_irq_handler( &sc->irqInfo );
#endif

   if (!st)
      rtems_panic ("dec2114x : Interrupt name %d already in use\n", sc->irqInfo.name );
}

static void
dec21140_rxDaemon (void *arg)
{
   volatile struct MD    *rmd;
   struct dec21140_softc *sc;
   struct ifnet          *ifp;
   struct ether_header   *eh;
   struct mbuf           *m;
   unsigned int          i,len;
   rtems_event_set       events;

   for (;;)
   {

      rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                  RTEMS_WAIT|RTEMS_EVENT_ANY,
                                  RTEMS_NO_TIMEOUT,
                                  &events);

      for(i=0; i< NDECDRIVER; i++ )
      {
         sc = &dec21140_softc[i];
         if( sc->base )
         {
            if( events & sc->ioevent )
            {
               ifp   = &sc->arpcom.ac_if;
               rmd   = sc->nextRxMD;

               /*
               ** Read off all the packets we've received on this unit
               */
               while((rmd->status & 0x80000000) == 0)
               {
                  /* printk("unit %i rx\n", ifp->if_unit ); */

                  /* pass on the packet in the mbuf */
                  len = (rmd->status >> 16) & 0x7ff;
                  m = (struct mbuf *)(rmd->m);
                  m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);
                  eh = mtod (m, struct ether_header *);
                  m->m_data += sizeof(struct ether_header);
                  ether_input (ifp, eh, m);

                  /* get a new mbuf for the 21140 */
                  MGETHDR (m, M_WAIT, MT_DATA);
                  MCLGET (m, M_WAIT);
                  m->m_pkthdr.rcvif = ifp;
                  rmd->m = m;
                  rmd->buf1 = phys_to_bus(mtod(m, void *));

                  /* mark the descriptor as ready to receive */
                  rmd->status = 0x80000000;

                  rmd=rmd->next;
               }

               sc->nextRxMD = rmd;
            }
         }
      }

   }
}

static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
   struct dec21140_softc   *dp = ifp->if_softc;
   volatile struct MD      *tmd;
   volatile unsigned char  *temp;
   struct mbuf             *n;
   unsigned int            len;
   volatile uint32_t      *tbase;

   tbase = dp->base;
   /*
    * Waiting for Transmitter ready
    */

   tmd = dp->TxMD;
   n = m;

   while ((tmd->status & 0x80000000) != 0)
   {
      tmd=tmd->next;
   }

   len = 0;
   temp = (volatile unsigned char *)(bus_to_phys(tmd->buf1));

   for (;;)
   {
      len += m->m_len;
      memcpy((void*) temp, (char *)m->m_data, m->m_len);
      temp += m->m_len ;
      if ((m = m->m_next) == NULL)
         break;
   }

   if (len < ET_MINLEN) len = ET_MINLEN;
   tmd->counts =  0xe1000000 | (len & 0x7ff);
   tmd->status = 0x80000000;

   st_le32( (tbase+memCSR1), 0x1);

   m_freem(n);

   dp->TxMD = tmd->next;
}

/*
 * Driver transmit daemon
 */
void
dec21140_txDaemon (void *arg)
{
   struct dec21140_softc *sc;
   struct ifnet          *ifp;
   struct mbuf           *m;
   int i;
   rtems_event_set       events;

   for (;;)
   {
      /*
       * Wait for packets bound for any of the dec units
       */
      rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
                                  RTEMS_EVENT_ANY | RTEMS_WAIT,
                                  RTEMS_NO_TIMEOUT, &events);

      for(i=0; i< NDECDRIVER; i++ )
      {
         sc  = &dec21140_softc[i];
         if( sc->base )
         {
            if( events & sc->ioevent )
            {
               ifp = &sc->arpcom.ac_if;

               /*
                * Send packets till queue is empty
                */
               for(;;)
               {
                  IF_DEQUEUE(&ifp->if_snd, m);
                  if( !m ) break;
                  /* printk("unit %i tx\n", ifp->if_unit ); */
                  sendpacket (ifp, m);
               }

               ifp->if_flags &= ~IFF_OACTIVE;
            }
         }
      }

   }
}

static void
dec21140_start (struct ifnet *ifp)
{
   struct dec21140_softc *sc = ifp->if_softc;
   rtems_event_send( txDaemonTid, sc->ioevent );
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
   volatile uint32_t *tbase;

   /*
    * Set up DEC21140 hardware if its not already been done
    */
   if( !sc->irqInfo.hdl )
   {
      dec21140Enet_initialize_hardware (sc);
   }

   /*
    * Enable RX and TX
    */
   tbase = sc->base;
   st_le32( (tbase+memCSR5), IT_SETUP);
   st_le32( (tbase+memCSR7), IT_SETUP);
   st_le32( (tbase+memCSR6), CSR6_INIT | CSR6_TXRX);

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
  volatile uint32_t *tbase;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Stop the transmitter
   */
  tbase = sc->base;
  st_le32( (tbase+memCSR7), NO_IT);
  st_le32( (tbase+memCSR6), CSR6_INIT);

  /*  free((void*)sc->bufferBase); */
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
dec21140_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
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
int iftap(struct ifnet *ifp, struct ether_header *eh, struct mbuf *m )
{
   int i;

   if(  ifp->if_unit == 1 ) return 0;

   printf("unit %i, src ", ifp->if_unit );
   for(i=0; i< ETHER_ADDR_LEN; i++) printf("%02x", (char) eh->ether_shost[i] );
   printf(" dest ");
   for(i=0; i< ETHER_ADDR_LEN; i++) printf("%02x", (char) eh->ether_dhost[i] );
   printf("\n");

   return -1;
}
*/

/*
 * Attach an DEC21140 driver to the system
 */
int
rtems_dec21140_driver_attach (struct rtems_bsdnet_ifconfig *config, int attach)
{
   struct dec21140_softc *sc;
   struct ifnet *ifp;
   char         *unitName;
   int          unitNumber;
   int          mtu;
   unsigned char cvalue;
#if defined(__i386__)
   uint32_t     value;
   uint8_t      interrupt;
#endif
   int          pbus, pdev, pfun;
#if defined(__PPC__)
   int          tmp;
   uint32_t     lvalue;
#endif

   /*
    * Get the instance number for the board we're going to configure
    * from the user.
    */
   if( (unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) == -1 )
   {
      return 0;
   }
   if( strcmp(unitName, DRIVER_PREFIX) )
   {
      printk("dec2114x : unit name '%s' not %s\n", unitName, DRIVER_PREFIX );
      return 0;
   }

   /*
    * Find the board
    */
   if ( pci_find_device(PCI_VENDOR_ID_DEC, PCI_DEVICE_ID_DEC_21140,
                          unitNumber-1, &pbus, &pdev, &pfun) == -1 ) {
      if ( pci_find_device(PCI_VENDOR_ID_DEC, PCI_DEVICE_ID_DEC_21143,
                             unitNumber-1, &pbus, &pdev, &pfun) != -1 ) {

        /* the 21143 chip must be enabled before it can be accessed */
#if defined(__i386__)
        pci_write_config_dword(pbus, pdev, pfun, 0x40, 0 );
#else
        pci_write_config_dword(pbus, pdev, pfun, 0x40, PCI_DEVICE_ID_DEC_21143);
#endif

      } else {
         printk("dec2114x : device '%s' not found on PCI bus\n", config->name );
         return 0;
      }
   }

#ifdef DEC_DEBUG
   else {
      printk("dec21140 : found device '%s', bus 0x%02x, dev 0x%02x, func 0x%02x\n",
             config->name, pbus, pdev, pfun);
   }
#endif

   if ((unitNumber < 1) || (unitNumber > NDECDRIVER))
   {
      printk("dec2114x : unit %i is invalid, must be (1 <= n <= %d)\n", unitNumber);
      return 0;
   }

   sc = &dec21140_softc[unitNumber - 1];
   ifp = &sc->arpcom.ac_if;
   if (ifp->if_softc != NULL)
   {
      printk("dec2114x : unit %i already in use.\n", unitNumber );
      return 0;
   }


   /*
   ** Get this unit's rx/tx event
   */
   sc->ioevent = unit_signals[unitNumber-1];

   /*
   ** Save the buffer counts
   */
   sc->numRxbuffers = (config->rbuf_count) ? config->rbuf_count : NRXBUFS;
   sc->numTxbuffers = (config->xbuf_count) ? config->xbuf_count : NTXBUFS;


   /*
    * Get card address spaces & retrieve its isr vector
    */
#if defined(__i386__)

   pci_read_config_dword(pbus, pdev, pfun, 16, &value);
   sc->port = value & ~IO_MASK;

   pci_read_config_dword(pbus, pdev, pfun, 20, &value);
   if (_CPU_is_paging_enabled())
      _CPU_map_phys_address((void **) &(sc->base),
                            (void *)(value & ~MEM_MASK),
                            DEC_REGISTER_SIZE ,
                            PTE_CACHE_DISABLE | PTE_WRITABLE);
   else
      sc->base = (uint32_t *)(value & ~MEM_MASK);

   pci_read_config_byte(pbus, pdev, pfun, 60, &interrupt);
   cvalue = interrupt;
#endif
#if defined(__PPC__)
   (void)pci_read_config_dword(pbus,
                               pdev,
                               pfun,
                               PCI_BASE_ADDRESS_0,
                               &lvalue);

   sc->port = lvalue & (unsigned int)(~IO_MASK);

   (void)pci_read_config_dword(pbus,
                               pdev,
                               pfun,
                               PCI_BASE_ADDRESS_1,
                               &lvalue);

   tmp = (unsigned int)(lvalue & (unsigned int)(~MEM_MASK))
      + (unsigned int)PCI_MEM_BASE;

   sc->base = (uint32_t*)(tmp);

   pci_read_config_byte(pbus,
                        pdev,
                        pfun,
                        PCI_INTERRUPT_LINE,
                        &cvalue);

#endif

   /*
   ** Prep the board
   */

   pci_write_config_word(pbus, pdev, pfun,
      PCI_COMMAND,
      (uint16_t) ( PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER ) );

   /*
   ** Store the interrupt name, we'll use it later when we initialize
   ** the board.
   */
   memset(&sc->irqInfo,0,sizeof(rtems_irq_connect_data));
   sc->irqInfo.name = cvalue;


#ifdef DEC_DEBUG
   printk("dec2114x : unit %d base address %08x.\n", unitNumber, sc->base );
#endif


   /*
   ** Setup ethernet address
   */
   if (config->hardware_address) {
      memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
              ETHER_ADDR_LEN);
   }
   else {
      union {char c[64]; unsigned short s[32];} rombuf;
      int i;

      for (i=0; i<32; i++){
         rombuf.s[i] = eeget16( sc->base + memCSR9, i);
      }
#if defined(__i386__)
      for (i=0 ; i<(ETHER_ADDR_LEN/2); i++){
         sc->arpcom.ac_enaddr[2*i]   = rombuf.c[20+2*i+1];
         sc->arpcom.ac_enaddr[2*i+1] = rombuf.c[20+2*i];
      }
#endif
#if defined(__PPC__)
      memcpy (sc->arpcom.ac_enaddr, rombuf.c+20, ETHER_ADDR_LEN);
#endif
   }

   if (config->mtu)
      mtu = config->mtu;
   else
      mtu = ETHERMTU;

   sc->acceptBroadcast = !config->ignore_broadcast;

   /*
    * Set up network interface values
    */

/*   ifp->if_tap = iftap; */

   ifp->if_softc = sc;
   ifp->if_unit = unitNumber;
   ifp->if_name = unitName;
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

#ifdef DEC_DEBUG
   printk( "dec2114x : driver attached\n" );
#endif

   /*
    * Start driver tasks if this is the first dec unit initialized
    */
   if (txDaemonTid == 0)
   {
      rxDaemonTid = rtems_bsdnet_newproc( "DCrx", 4096,
                                          dec21140_rxDaemon, NULL);

      txDaemonTid = rtems_bsdnet_newproc( "DCtx", 4096,
                                          dec21140_txDaemon, NULL);
#ifdef DEC_DEBUG
      printk( "dec2114x : driver tasks created\n" );
#endif
   }

   return 1;
};

#endif /* DEC21140_SUPPORTED */
