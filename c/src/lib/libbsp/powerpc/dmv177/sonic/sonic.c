void break_when_you_get_here();
/*
 *******************************************************************
 *******************************************************************
 **                                                               **
 **       RTEMS NETWORK DRIVER FOR NATIONAL DP83932 `SONIC'       **
 **         SYSTEMS-ORIENTED NETWORK INTERFACE CONTROLLER         **
 **                                                               **
 *******************************************************************
 *******************************************************************
 */

/*
 * $Revision$   $Date$   $Author$
 * $State$
 * $Id$
 */

/*
 * References:
 * 1) DP83932C-20/25/33 MHz SONIC(TM) Systems-Oriented Network Interface
 *    Controller data sheet.  TL/F/10492, RRD-B30M105, National Semiconductor,
 *    1995.
 *
 * 2) Software Driver Programmer's Guide for the DP83932 SONIC(TM),
 *    Application Note 746, Wesley Lee and Mike Lui, TL/F/11140, 
 *    RRD-B30M75, National Semiconductor, March, 1991.
 *
 * 3) SVME/DMV-171 Single Board Computer Documentation Package, #805905,
 *    DY 4 Systems Inc., Kanata, Ontario, September, 1996.
 */

#include <rtems/rtems_bsdnet.h>
#include "sonic.h"

#include <stdio.h>

#include <errno.h>
#include <rtems/error.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

/*
 * Debug levels
 *
 */

#define SONIC_DEBUG_NONE             0x0000
#define SONIC_DEBUG_ALL              0xFFFF
#define SONIC_DEBUG_PRINT_REGISTERS  0x0001
#define SONIC_DEBUG_MEMORY           0x0002
#define SONIC_DEBUG_MEMORY_ALLOCATE  0x0004
#define SONIC_DEBUG_FRAGMENTS        0x0008
#define SONIC_DEBUG_CAM              0x0008
#define SONIC_DEBUG_DESCRIPTORS      0x0010
#define SONIC_DEBUG_ERRORS           0x0020
#define SONIC_DEBUG_DUMP_TX_MBUFS    0x0040
#define SONIC_DEBUG_DUMP_RX_MBUFS    0x0080

#define SONIC_DEBUG_DUMP_MBUFS \
  (SONIC_DEBUG_DUMP_TX_MBUFS|SONIC_DEBUG_DUMP_RX_MBUFS)

#define SONIC_DEBUG_MEDIUM \
  ((SONIC_DEBUG_ALL) & ~(SONIC_DEBUG_PRINT_REGISTERS|SONIC_DEBUG_DUMP_MBUFS))
  /*
  ((SONIC_DEBUG_ALL) & ~(SONIC_DEBUG_DUMP_MBUFS))
*/

#define SONIC_DEBUG  SONIC_DEBUG_ALL

/* ((SONIC_DEBUG_ALL) & ~SONIC_DEBUG_PRINT_REGISTERS)  */
  /* (SONIC_DEBUG_ALL) */

/* (SONIC_DEBUG_ALL) */
/* (SONIC_DEBUG_ERRORS) */

/* (SONIC_DEBUG_MEMORY|SONIC_DEBUG_DESCRIPTORS) */

#if (SONIC_DEBUG & SONIC_DEBUG_DUMP_MBUFS)
#include <rtems/dumpbuf.h>
#endif

/*
 * XXX
 */

#include <dmv170.h>

/*
 *  Use the top line if you want more symbols.
 */

#define SONIC_STATIC 
/* #define SONIC_STATIC static */

/*
 * Number of devices supported by this driver
 */
#ifndef NSONIC
# define NSONIC 1
#endif

/*
 * Default location of device registers
 */
#ifndef SONIC_BASE_ADDRESS
# define SONIC_BASE_ADDRESS 0xF3000000
# warning "Using default SONIC_BASE_ADDRESS."
#endif

/*
 * Default interrupt vector
 */
#ifndef SONIC_VECTOR
# define SONIC_VECTOR 1
# warning "Using default SONIC_VECTOR."
#endif

/*
 * Default device configuration register values
 * Conservative, generic values.
 * DCR:
 *      No extended bus mode
 *      Unlatched bus retry
 *      Programmable outputs unused
 *      Asynchronous bus mode
 *      User definable pins unused
 *      No wait states (access time controlled by DTACK*)
 *      32-bit DMA
 *      Empty/Fill DMA mode
 *      Maximum Transmit/Receive FIFO
 * DC2:
 *      Extended programmable outputs unused
 *      Normal HOLD request
 *      Packet compress output unused
 *      No reject on CAM match
 */
#define SONIC_DCR \
   (DCR_DW32 | DCR_WAIT0 | DCR_PO0 | DCR_PO1  | DCR_RFT4 | DCR_TFT8)
#ifndef SONIC_DCR
# define SONIC_DCR (DCR_DW32 | DCR_TFT28)
#endif
#ifndef SONIC_DC2
# define SONIC_DC2 (0)
#endif

/*
 * Default sizes of transmit and receive descriptor areas
 */
#define RDA_COUNT     20
#define TDA_COUNT     10

/*
 * 
 * As suggested by National Application Note 746, make the
 * receive resource area bigger than the receive descriptor area.
 */
#define RRA_EXTRA_COUNT  0

/*
 * RTEMS event used by interrupt handler to signal daemons.
 */
#define INTERRUPT_EVENT  RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2

/*
 * Largest Ethernet frame.
 */
#define MAXIMUM_FRAME_SIZE  1518

/*
 * Receive buffer size.
 * Allow for a pointer, plus a full ethernet frame (including Frame
 * Check Sequence) rounded up to a 4-byte boundary.
 */
#define RBUF_SIZE  ((sizeof (void *) + (MAXIMUM_FRAME_SIZE) + 3) & ~3)
#define RBUF_WC    ((((MAXIMUM_FRAME_SIZE) + 3) & ~3) / 2)

/*
 * Macros for manipulating 32-bit pointers as 16-bit fragments
 */
#define LSW(p)   ((rtems_unsigned16)((rtems_unsigned32)(p)))
#define MSW(p)   ((rtems_unsigned16)((rtems_unsigned32)(p) >> 16))
#define PTR(m,l) ((void*)(((rtems_unsigned16)(m)<<16)|(rtems_unsigned16)(l)))

/*
 * Hardware-specific storage
 */
struct sonic_softc {
  /*
   * Connection to networking code
   * This entry *must* be the first in the sonic_softc structure.
   */
  struct arpcom                    arpcom;

  /*
   * Default location of device registers
   * ===CACHE===
   * This area must be non-cacheable, guarded.
   */
  void                             *sonic;

  /*
   *  Tables to map the mbufs from chip to stack
   */

  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;

  /*
   * Interrupt vector
   */
  rtems_vector_number             vector;

  /*
   *  Indicates configuration
   */
  int                             acceptBroadcast;

  /*
   * Task waiting for interrupts
   */
  rtems_id                        rxDaemonTid;
  rtems_id                        txDaemonTid;

  /*
   * Receive resource area
   */
  int                             rdaCount;
  ReceiveResourcePointer_t        rsa;
  ReceiveResourcePointer_t        rea;
  CamDescriptorPointer_t          cdp;
  ReceiveDescriptorPointer_t      rda;
  ReceiveDescriptorPointer_t      rdp_last;

  /*
   * Transmit descriptors
   */
  int                             tdaCount;
  TransmitDescriptorPointer_t     tdaHead;  /* Last filled */
  TransmitDescriptorPointer_t     tdaTail;  /* Next to retire */
  int                             tdaActiveCount;

  /*
   * Statistics
   */
  unsigned long                   Interrupts;
  unsigned long                   rxInterrupts;
  unsigned long                   rxMissed;
  unsigned long                   rxGiant;
  unsigned long                   rxNonOctet;
  unsigned long                   rxBadCRC;
  unsigned long                   rxCollision;

  unsigned long                   txInterrupts;
  unsigned long                   txSingleCollision;
  unsigned long                   txMultipleCollision;
  unsigned long                   txCollision;
  unsigned long                   txDeferred;
  unsigned long                   txUnderrun;
  unsigned long                   txLateCollision;
  unsigned long                   txExcessiveCollision;
  unsigned long                   txExcessiveDeferral;
  unsigned long                   txLostCarrier;
  unsigned long                   txRawWait;
};
SONIC_STATIC struct sonic_softc sonic_softc[NSONIC];

/*
 ******************************************************************
 *                                                                *
 *                        Support Routines                        *
 *                                                                *
 ******************************************************************
 */

void sonic_write_register(
  void       *base,
  unsigned32  regno,
  unsigned32  value
);

unsigned32 sonic_read_register(
  void       *base,
  unsigned32  regno
);

void sonic_enable_interrupts(
  void       *rp,
  unsigned32  mask
)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
      sonic_write_register(
         rp,
         SONIC_REG_IMR,
         sonic_read_register(rp, SONIC_REG_IMR) | mask
      );
  rtems_interrupt_enable( level );
}

/*
 * Allocate non-cacheable memory on a single 64k page.
 * Very simple minded -- just keeps trying till the memory is on a single page.
 */
SONIC_STATIC void * sonic_allocate(unsigned int nbytes)
{
  void *p;
  unsigned long a1, a2;

  for (;;) {
    /*
     * ===CACHE===
     * Change malloc to malloc_noncacheable_guarded.
     */
    p = malloc( nbytes, M_MBUF, M_NOWAIT );
    if (p == NULL)
      rtems_panic ("No memory!");
    memset (p, '\0', nbytes);
    a1 = (unsigned long)p;
    a2 = a1 + nbytes - 1;
    if ((a1 >> 16) == (a2 >> 16))
      break;
  }
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY_ALLOCATE)
  printf( "sonic_allocate %d bytes at %p\n", nbytes, p );
#endif
  return p;
}

/*
 * Shut down the interface.
 */

SONIC_STATIC void sonic_stop (struct sonic_softc *sc)
{
  void *rp = sc->sonic;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Stop the transmitter and receiver.
   */
  sonic_write_register( rp, SONIC_REG_CR, CR_HTX | CR_RXDIS );
}

/*
 * Show interface statistics
 */
SONIC_STATIC void sonic_stats (struct sonic_softc *sc)
{
  printf (" Total Interrupts:%-8lu", sc->Interrupts);
  printf ("    Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("            Giant:%-8lu", sc->rxGiant);
  printf ("        Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("          Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("        Collision:%-8lu", sc->rxCollision);
  printf ("           Missed:%-8lu\n", sc->rxMissed);

  printf (    "    Tx Interrupts:%-8lu", sc->txInterrupts);
  printf (  "           Deferred:%-8lu", sc->txDeferred);
  printf ("        Lost Carrier:%-8lu\n", sc->txLostCarrier);
  printf (   "Single Collisions:%-8lu", sc->txSingleCollision);
  printf ( "Multiple Collisions:%-8lu", sc->txMultipleCollision);
  printf ("Excessive Collisions:%-8lu\n", sc->txExcessiveCollision);
  printf (   " Total Collisions:%-8lu", sc->txCollision);
  printf ( "     Late Collision:%-8lu", sc->txLateCollision);
  printf ("            Underrun:%-8lu\n", sc->txUnderrun);
  printf (   "  Raw output wait:%-8lu\n", sc->txRawWait);
}

/*
 ******************************************************************
 *                                                                *
 *                        Interrupt Handler                       *
 *                                                                *
 ******************************************************************
 */

SONIC_STATIC rtems_isr sonic_interrupt_handler (rtems_vector_number v)
{
  struct sonic_softc *sc = sonic_softc;
  unsigned32 isr, imr;
  void *rp;

#if (NSONIC > 1)
  /*
   * Find the device which requires service
   */
  for (;;) {
    if (sc->vector == v)
      break;
    if (++sc == &sonic[NSONIC])
      return;  /* Spurious interrupt? */
  }
#endif /* NSONIC > 1 */

  /*
   * Get pointer to SONIC registers
   */
  rp = sc->sonic;

  sc->Interrupts++;

  isr = sonic_read_register( rp, SONIC_REG_ISR );
  imr = sonic_read_register( rp, SONIC_REG_IMR );

  /*
   * Packet received or receive buffer area exceeded?
   */
  if ((imr & (IMR_PRXEN | IMR_RBAEEN)) &&
      (isr & (ISR_PKTRX | ISR_RBAE))) {
    imr &= ~(IMR_PRXEN | IMR_RBAEEN);
    sc->rxInterrupts++;
    rtems_event_send (sc->rxDaemonTid, INTERRUPT_EVENT);
  }

  /*
   * Packet started, transmitter done or transmitter error?
   */
  if ((imr & (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN)) &&
      (isr & (ISR_PINT | ISR_TXDN | ISR_TXER))) {
    imr &= ~(IMR_PINTEN | IMR_PTXEN | IMR_TXEREN);
    sc->txInterrupts++;
    rtems_event_send (sc->txDaemonTid, INTERRUPT_EVENT);
  }

  sonic_write_register( rp, SONIC_REG_IMR, imr );
}

/*
 ******************************************************************
 *                                                                *
 *                      Transmitter Routines                      *
 *                                                                *
 ******************************************************************
 */

/*
 * Soak up transmit descriptors that have been sent.
 */

SONIC_STATIC void sonic_retire_tda (struct sonic_softc *sc)
{
  rtems_unsigned16 status;
  unsigned int collisions;
  struct mbuf *m, *n;

  /*
   * Repeat for all completed transmit descriptors.
   */
  while ((sc->tdaActiveCount != 0)
      && ((status = sc->tdaTail->status) != 0)) {

#if (SONIC_DEBUG & SONIC_DEBUG_DESCRIPTORS)
    printf( "retire TDA %p (0x%04x)\n", sc->tdaTail, status );
#endif

#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
    if ( status != 0x0001 )
      printf( "ERROR: retire TDA %p (0x%04x)\n", sc->tdaTail, status );
#endif

    /*
     * Check for errors which stop the transmitter.
     */
    if (status & (TDA_STATUS_EXD |
        TDA_STATUS_EXC |
        TDA_STATUS_FU |
        TDA_STATUS_BCM)) {
      /*
       * Restart the transmitter if there are
       * packets waiting to go.
       */
      rtems_unsigned16 link;
      link = *(sc->tdaTail->linkp);

      if ((link & TDA_LINK_EOL) == 0) {
        void *rp = sc->sonic;

        sonic_write_register( rp, SONIC_REG_CTDA, link );
        sonic_write_register( rp, SONIC_REG_CR, CR_TXP );
      }
    }

    /*
     * Update network statistics
     */
    collisions = (status & TDA_STATUS_COLLISION_MASK) >> TDA_STATUS_COLLISION_SHIFT;
    if (collisions) {
      if (collisions == 1)
        sc->txSingleCollision++;
      else
        sc->txMultipleCollision++;
      sc->txCollision += collisions;
    }
    if (status & TDA_STATUS_EXC)
      sc->txExcessiveCollision++;
    if (status & TDA_STATUS_OWC)
      sc->txLateCollision++;
    if (status & TDA_STATUS_EXD)
      sc->txExcessiveDeferral++;
    if (status & TDA_STATUS_DEF)
      sc->txDeferred++;
    if (status & TDA_STATUS_FU)
      sc->txUnderrun++;
    if (status & TDA_STATUS_CRSL)
      sc->txLostCarrier++;

    /*
     *  Free the packet and reset a couple of fields
     */
    sc->tdaActiveCount--;
    m = (struct mbuf *)&sc->tdaTail->mbufp;
    MFREE(m, n);

    sc->tdaTail->frag[0].frag_link = LSW(sc->tdaTail->link_pad);
    sc->tdaTail->frag_count        = 0;

    /*
     * Move to the next transmit descriptor
     */
    sc->tdaTail = sc->tdaTail->next;
#if (SONIC_DEBUG & SONIC_DEBUG_DESCRIPTORS)
    printf( "next TDA %p\n", sc->tdaTail );
#endif
  }
}

/*
 * Send packet
 */
SONIC_STATIC void sonic_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct sonic_softc *sc = ifp->if_softc;
  void *rp = sc->sonic;
  struct mbuf *l = NULL;
  TransmitDescriptorPointer_t tdp;
  volatile struct TransmitDescriptorFragLink *fp;
  unsigned int packetSize;
  int i;
  static char padBuf[64];

  /*
   * Free up transmit descriptors.
   */
  sonic_retire_tda (sc);

  /*
   * Wait for transmit descriptor to become available.
   */
  if (sc->tdaActiveCount == sc->tdaCount) {
#if (SONIC_DEBUG & SONIC_DEBUG_FRAGMENTS)
    puts( "Wait for more TDAs" );
#endif
    /*
     * Clear old events.
     */
    sonic_write_register( rp, SONIC_REG_ISR, ISR_PINT | ISR_TXDN | ISR_TXER );

    /*
     * Wait for transmit descriptor to become available.
     * Note that the transmit descriptors are checked
     * *before* * entering the wait loop -- this catches
     * the possibility that a transmit descriptor became
     * available between the `if' the started this block,
     * and the clearing of the interrupt status register.
     */
    sonic_retire_tda (sc);
    while (sc->tdaActiveCount == sc->tdaCount) {
      rtems_event_set events;
      /*
       * Enable transmitter interrupts.
       */
      sonic_enable_interrupts( rp, (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN) );

      /*
       * Wait for interrupt
       */
      rtems_bsdnet_event_receive (INTERRUPT_EVENT,
            RTEMS_WAIT|RTEMS_EVENT_ANY,
            RTEMS_NO_TIMEOUT,
            &events);
      sonic_write_register( rp, SONIC_REG_ISR, ISR_PINT | ISR_TXDN | ISR_TXER );
      sonic_retire_tda (sc);
    }
  }

  /*
   * Fill in the transmit descriptor fragment descriptors.
   * ===CACHE===
   * If data cache is operating in write-back mode, flush cached
   * data to memory.
   */
  tdp = sc->tdaHead->next;
  tdp->mbufp = m;
  packetSize = 0;
  fp = tdp->frag;
  for (i = 0 ; i < MAXIMUM_FRAGS_PER_DESCRIPTOR ; i++, fp++) {
    /*
     * Throw away empty mbufs
     */
    if (m->m_len) {
      void *p = mtod (m, void *);
      fp->frag_lsw = LSW(p);
      fp->frag_msw = MSW(p);
      fp->frag_size = m->m_len;
      packetSize += m->m_len;
#if (SONIC_DEBUG & SONIC_DEBUG_FRAGMENTS)
      printf( "fp %p 0x%04x%04x %d=%d .. %d\n",
        fp, fp->frag_msw, fp->frag_lsw, fp->frag_size, m->m_len, packetSize );
#endif
#if (SONIC_DEBUG & SONIC_DEBUG_DUMP_TX_MBUFS)
      Dump_Buffer(
        p,
        (fp->frag_size > MAXIMUM_FRAME_SIZE) ? MAXIMUM_FRAME_SIZE : fp->frag_size
      );
#endif
      l = m;
      m = m->m_next;
    }
    else {
      struct mbuf *n;
      MFREE (m, n);
      m = n;
      if (l != NULL)
        l->m_next = m;
    }
    /*
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if (m == NULL)
      break;
  }

  /*
   * Pad short packets.
   */
  if  ((packetSize < 64) && (i < MAXIMUM_FRAGS_PER_DESCRIPTOR)) {
    int padSize = 64 - packetSize;
    fp++;
    fp->frag_lsw = LSW(padBuf);
    fp->frag_msw = MSW(padBuf);
    fp->frag_size = padSize;
#if (SONIC_DEBUG & SONIC_DEBUG_FRAGMENTS)
    printf( "PAD fp %p 0x%04x%04x %d\n",
         fp, fp->frag_msw, fp->frag_lsw, fp->frag_size );
#endif
    packetSize += padSize;
    i++;
  }

  /*
   * Fill Transmit Descriptor
   */
  tdp->pkt_size = packetSize;
  tdp->frag_count = i + 1;
  tdp->status = 0;

  /*
   * Chain onto list and start transmission.
   */

  tdp->linkp = &(fp+1)->frag_link;
  *tdp->linkp = LSW(tdp->next) | TDA_LINK_EOL;
  if ( sc->tdaHead->frag_count )
    *sc->tdaHead->linkp &= ~TDA_LINK_EOL;
  sc->tdaActiveCount++;
  sc->tdaHead = tdp;

/* XXX */
/*   sonic_enable_interrupts( rp, (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN) ); */
  sonic_write_register( rp, SONIC_REG_CR, CR_TXP );
}

/*
 * Driver transmit daemon
 */
SONIC_STATIC void sonic_txDaemon (void *arg)
{
  struct sonic_softc *sc = (struct sonic_softc *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
    /*
     * Wait for packet
     */
    rtems_bsdnet_event_receive (
       START_TRANSMIT_EVENT,
       RTEMS_EVENT_ANY | RTEMS_WAIT,
       RTEMS_NO_TIMEOUT,
       &events
    );

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
      sonic_sendpacket (ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}

/*
 ******************************************************************
 *                                                                *
 *                        Receiver Routines                       *
 *                                                                *
 ******************************************************************
 */

/*
 * Wait for SONIC to hand over a Receive Descriptor.
 */

SONIC_STATIC void sonic_rda_wait(
  struct sonic_softc *sc,
  ReceiveDescriptorPointer_t rdp
)
{
  int i;
  void *rp = sc->sonic;
  rtems_event_set events;

  /*
   * Wait for Receive Descriptor.
   * The order of the tests is very important.
   *    The RDA is checked after RBAE is detected.  This ensures that
   *    the driver processes all RDA entries before reusing the RRA
   *    entry holding the giant packet.
   *    The event wait is done after the RDA and RBAE checks.  This
   *    catches the possibility that a Receive Descriptor became ready
   *    between the call to this function and the clearing of the
   *    interrupt status register bit.
   */
  for (;;) {
    /*
     * Has a giant packet arrived?
     * The National DP83932C data sheet is very vague on what
     * happens under this condition.  The description of the
     * Interrupt Status Register (Section 4.3.6) states,
     * ``Reception is aborted and the SONIC fetches the next
     * available resource descriptors in the RRA.  The buffer
     * space is not re-used and an RDA is not setup for the
     * truncated packet.''
     * I take ``Reception is aborted''  to mean that the RXEN
     * bit in the Command Register is cleared and must be set
     * by the driver to begin reception again.
     * Unfortunately, an alternative interpretation could be
     * that only reception of the current packet is aborted.
     * This would be more difficult to recover from....
     */
    if (sonic_read_register( rp, SONIC_REG_ISR ) & ISR_RBAE) {

#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
      printf( "ERROR: looks like a giant packet -- RBAE\n" );
#endif

      /*
       * One more check to soak up any Receive Descriptors
       * that may already have been handed back to the driver.
       */
      if (rdp->in_use == RDA_IN_USE) {
#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
      printf( "ERROR: nope just an RBAE\n" );
#endif
        break;
      }

      /*
       * Check my interpretation of the SONIC manual.
       */
      if (sonic_read_register( rp, SONIC_REG_CR ) & CR_RXEN)
        rtems_panic ("SONIC RBAE/RXEN");

      /*
       * Update statistics
       */
      sc->rxGiant++;

      /*
       * Reuse receive buffer.
       * Again, the manual is subject to interpretation.  The
       * RRP register is described as, `the lower address of
       * the next descriptor the SONIC will read.''
       * Since, acording to the ISR/RBAE notes, the SONIC has
       * ``fetched the next available resource descriptor in
       * the RRA'', I interpret this to mean that that the
       * driver has to move the RRP back *two* entries to
       * reuse the receive buffer holding the giant packet.
       */
      for (i = 0 ; i < 2 ; i++) {
        if (sonic_read_register( rp, SONIC_REG_RRP ) ==
            sonic_read_register( rp, SONIC_REG_RSA ))
          sonic_write_register(
            rp,
            SONIC_REG_RRP,
            sonic_read_register( rp, SONIC_REG_REA )
          );
          sonic_write_register(
             rp,
             SONIC_REG_RRP,
             sonic_read_register(rp, SONIC_REG_RRP) - sizeof(ReceiveResource_t)
          );
      }

      /*
       * Restart reception
       */
      sonic_write_register( rp, SONIC_REG_ISR, ISR_RBAE );
      sonic_write_register( rp, SONIC_REG_CR, CR_RXEN );
    }

    /*
     * Clear old packet-received events.
     */
    sonic_write_register( rp, SONIC_REG_ISR, ISR_PKTRX );

    /*
     * Has Receive Descriptor become available?
     */
    if (rdp->in_use == RDA_IN_USE)
      break;

    /*
     * Enable interrupts.
     */
    sonic_enable_interrupts( rp, (IMR_PRXEN | IMR_RBAEEN) );

    /*
     * Wait for interrupt.
     */
    rtems_bsdnet_event_receive(
      INTERRUPT_EVENT,
      RTEMS_WAIT|RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &events
    );
  }
#if (SONIC_DEBUG & SONIC_DEBUG_DESCRIPTORS)
  printf( "RDA %p\n", rdp );
#endif

#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
    if (rdp->status & 0x000E)
      printf( "ERROR: RDA %p (0x%04x)\n", rdp, rdp->status );
#endif

}

/*
 * SONIC reader task
 */
SONIC_STATIC void sonic_rxDaemon (void *arg)
{
  struct sonic_softc *sc = (struct sonic_softc *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  void *rp = sc->sonic;
  struct mbuf *m;
  rtems_unsigned16 status;
  ReceiveDescriptorPointer_t rdp;
  ReceiveResourcePointer_t rwp, rea;
  rtems_unsigned16 newMissedTally, oldMissedTally;
  unsigned32 rxMbufIndex;

  rwp = sc->rsa;
  rea = sc->rea;
  rdp = sc->rda;

  /*
   * Start the receiver
   */
  oldMissedTally = sonic_read_register( rp, SONIC_REG_MPT );

  /*
   * Input packet handling loop
   */
  rxMbufIndex = 0;
  for (;;) {
    /*
     * Wait till SONIC supplies a Receive Descriptor.
     */
    if (rdp->in_use == RDA_FREE) {
      sonic_rda_wait (sc, rdp);
    }

#if (SONIC_DEBUG & SONIC_DEBUG_DESCRIPTORS)
    printf( "Incoming packet %p status=0x%04x\n", rdp, rdp->status );
#endif

    /*
     * Check that packet is valid
     */
    status = rdp->status;
    if (status & RDA_STATUS_PRX) {
      struct ether_header *eh;
      void *p;

      /*
       * Pass the packet up the chain.
       * The mbuf count is reduced to remove
       * the frame check sequence at the end
       * of the packet.
       * ===CACHE===
       * Invalidate cache entries for this memory.
       */
      m = rdp->mbufp;
      m->m_len = m->m_pkthdr.len = rdp->byte_count -
                          sizeof(rtems_unsigned32) -
                          sizeof(struct ether_header);
      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
  
#if (SONIC_DEBUG & SONIC_DEBUG_DUMP_RX_MBUFS)
      Dump_Buffer( (void *) eh, sizeof(struct ether_header) );
      Dump_Buffer( (void *) m, 96 /* m->m_len*/ );
#endif

      ether_input (ifp, eh, m);

      /*
       * Sanity check that Receive Resource Area is
       * still in sync with Receive Descriptor Area
       * The buffer reported in the Receive Descriptor
       * should be the same as the buffer in the Receive
       * Resource we are about to reuse.
       */
/* XXX figure out whether this is valid or not */
#if 0
      if ((LSW(p) != rwp->buff_ptr_lsw)
       || (MSW(p) != rwp->buff_ptr_msw))
        rtems_panic ("SONIC RDA/RRA");
#endif

      /*
       * Allocate a new mbuf.
       */

      m= (void *)0xA0000000; /* hope for a fault :) */
      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;
      rdp->mbufp = m;
      p = mtod (m, void *);

      /*
       * Reuse Receive Resource.
       */

      rwp->buff_ptr_lsw = LSW(p);
      rwp->buff_ptr_msw = MSW(p);
      rwp->buff_wc_lsw = RBUF_WC;
      rwp->buff_wc_msw = 0;
      rwp++;

      if (rwp == rea) {
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
        printf( "Wrapping RWP from %p to %p\n", rwp, sc->rsa );
#endif
        rwp = sc->rsa;
      }
      sonic_write_register( rp, SONIC_REG_RWP , LSW(rwp) );

      /*
       * Tell the SONIC to reread the RRA.
       */
      if (sonic_read_register( rp, SONIC_REG_ISR ) & ISR_RBE)
        sonic_write_register( rp, SONIC_REG_ISR, ISR_RBE );
    }
    else {
      if (status & RDA_STATUS_COL)
        sc->rxCollision++;
      if (status & RDA_STATUS_FAER)
        sc->rxNonOctet++;
      else if (status & RDA_STATUS_CRCR)
        sc->rxBadCRC++;
    }

    /*
     * Count missed packets
     */
    newMissedTally = sonic_read_register( rp, SONIC_REG_MPT );
    if (newMissedTally != oldMissedTally) {
      sc->rxMissed += (newMissedTally - oldMissedTally) & 0xFFFF;
      newMissedTally = oldMissedTally;
    }

    /*
     * Move to next receive descriptor
     */

    rdp->in_use = RDA_FREE;
    rdp = rdp->next;
    rdp->link &= ~RDA_LINK_EOL;

  }
}

/*
 ******************************************************************
 *                                                                *
 *                     Initialization Routines                    *
 *                                                                *
 ******************************************************************
 */

/*
 * Initialize the SONIC hardware
 */
SONIC_STATIC void sonic_initialize_hardware(struct sonic_softc *sc)
{
  void *rp = sc->sonic;
  int i;
  unsigned char *hwaddr;
  rtems_isr_entry old_handler;
  TransmitDescriptorPointer_t tdp;
  ReceiveDescriptorPointer_t ordp, rdp;
  ReceiveResourcePointer_t rwp;
  struct mbuf *m;
  void *p;
  CamDescriptorPointer_t cdp;

  /*
   *  The Revision B SONIC has a horrible bug known as the "Zero
   *  Length Packet bug".  The initial board used to develop this
   *  driver had a newer revision of the SONIC so there was no reason
   *  to check for this.  If you have the Revision B SONIC chip, then
   *  you need to add some code to the RX path to handle this weirdness.
   */

  if ( sonic_read_register( rp, SONIC_REG_SR ) < SONIC_REVISION_C ) {
    rtems_fatal_error_occurred( 0x0BADF00D );  /* don't eat this part :) */
  }
  
  /*
   *  Allocate memory so we can figure out from the descriptor which
   *  mbuf to send to the stack.
   */

  sc->txMbuf = malloc (sc->tdaCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);
  if (!sc->txMbuf)
     rtems_panic ("No memory for TX mbuf pointers");

  sc->rxMbuf = malloc (sc->rdaCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
  if (!sc->rxMbuf)
     rtems_panic ("No memory for RX mbuf pointers");

  /*
   *  Set up circular linked list in Transmit Descriptor Area.
   *  Use the PINT bit in the transmit configuration field to
   *  request an interrupt on every other transmitted packet.
   *
   *  NOTE: sonic_allocate() zeroes all of the memory allocated.
   */

  sc->tdaActiveCount = 0;
  sc->tdaTail = sonic_allocate(sc->tdaCount * sizeof *tdp);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "tdaTail = %p\n", sc->tdaTail );
#endif
  tdp = sc->tdaTail;
  for (i = 0 ; i < sc->tdaCount ; i++) {
    /*
     *  Start off with the table of outstanding mbuf's 
     */
    sc->txMbuf[i] = NULL;

    /*
     *  status, pkt_config, pkt_size, and all fragment fields 
     *  are set to zero by sonic_allocate.
     */

/* XXX not used by the BSD drivers
    if (i & 1)
      tdp->pkt_config = TDA_CONFIG_PINT;
*/

    tdp->frag_count        = 0;
    tdp->frag[0].frag_link = LSW(tdp + 1);
    tdp->link_pad          = LSW(tdp + 1) | TDA_LINK_EOL;
    tdp->linkp             = &((tdp + 1)->frag[0].frag_link);
    tdp->next              = (TransmitDescriptor_t *)(tdp + 1);
    tdp++;
  }
  tdp--;
  sc->tdaHead = tdp;
  tdp->link_pad = LSW(sc->tdaTail) | TDA_LINK_EOL;
  tdp->next = (TransmitDescriptor_t *)sc->tdaTail;
  tdp->linkp = &sc->tdaTail->frag[0].frag_link;

  /*
   *  Set up circular linked list in Receive Descriptor Area.
   *  Leaves sc->rda pointing at the `beginning' of the list.
   *
   *  NOTE: The RDA and CDP must have the same MSW for their addresses.
   */

  sc->rda = sonic_allocate(
              (sc->rdaCount * sizeof(ReceiveDescriptor_t)) + 
                sizeof(CamDescriptor_t) );
  sc->cdp = (CamDescriptorPointer_t) ((unsigned char *)sc->rda +
        (sc->rdaCount * sizeof(ReceiveDescriptor_t)));
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rda area = %p\n", sc->rda );
  printf( "cdp area = %p\n", sc->cdp );
#endif

  ordp = rdp = sc->rda;
  for (i = 0 ; i < sc->rdaCount ; i++) {
    /*
     *  status, byte_count, pkt_ptr0, pkt_ptr1, and seq_no are set
     *  to zero by sonic_allocate.
     */
    rdp->link   = LSW(rdp + 1);
    rdp->in_use = RDA_FREE;
    rdp->next   = (ReceiveDescriptor_t *)(rdp + 1);
    ordp = rdp;
    rdp++;
  }
  /*
   *  Link the last desriptor to the 1st one and mark it as the end
   *  of the list.
   */
  ordp->next   = sc->rda;
  ordp->link   = LSW(sc->rda) | RDA_LINK_EOL;
  sc->rdp_last = rdp;
 
  /*
   * Allocate the receive resource area.
   * In accordance with National Application Note 746, make the
   * receive resource area bigger than the receive descriptor area.
   * This has the useful side effect of making the receive resource
   * area big enough to hold the CAM descriptor area.
   */

  sc->rsa = sonic_allocate((sc->rdaCount + RRA_EXTRA_COUNT) * sizeof *sc->rsa);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rsa area = %p\n", sc->rsa );
#endif

  /*
   *  Set up list in Receive Resource Area.
   *  Allocate space for incoming packets.
   */

  rwp = sc->rsa;
  for (i = 0 ; i < (sc->rdaCount + RRA_EXTRA_COUNT) ; i++, rwp++) {

    /*
     * Allocate memory for buffer.
     * Place a pointer to the mbuf at the beginning of the buffer
     * so we can find the mbuf when the SONIC returns the buffer
     * to the driver.
     */
    
    MGETHDR (m, M_WAIT, MT_DATA);
    MCLGET (m, M_WAIT);
    m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
    sc->rxMbuf[i] = m;
    sc->rda[i].mbufp = m;

    p = mtod (m, void *);

    /*
     * Set up RRA entry
     */
    rwp->buff_ptr_lsw = LSW(p);
    rwp->buff_ptr_msw = MSW(p);
    rwp->buff_wc_lsw = RBUF_WC;
    rwp->buff_wc_msw = 0;
  }
  sc->rea = rwp;
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rea area = %p\n", sc->rea );
#endif


  /*
   * Issue a software reset.
   */
  sonic_write_register( rp, SONIC_REG_CR, CR_RST | CR_STP | CR_RXDIS | CR_HTX );

  /*
   * Set up data configuration registers.
   */
  sonic_write_register( rp, SONIC_REG_DCR, SONIC_DCR );
  sonic_write_register( rp, SONIC_REG_DCR2, SONIC_DC2 );

  sonic_write_register( rp, SONIC_REG_CR, CR_STP | CR_RXDIS | CR_HTX );

  /*
   * Mask all interrupts
   */
  sonic_write_register( rp, SONIC_REG_IMR, 0x0 ); /* XXX was backwards */

  /*
   * Clear outstanding interrupts.
   */
  sonic_write_register( rp, SONIC_REG_ISR, 0x7FFF );

  /*
   *  Clear the tally counters
   */

  sonic_write_register( rp, SONIC_REG_CRCT, 0xFFFF );
  sonic_write_register( rp, SONIC_REG_FAET, 0xFFFF );
  sonic_write_register( rp, SONIC_REG_MPT, 0xFFFF );
  sonic_write_register( rp, SONIC_REG_RSC, 0 );

  /*
   *  Set the Receiver mode
   *
   *  Enable/disable reception of broadcast packets
   */

  if (sc->acceptBroadcast)
    sonic_write_register( rp, SONIC_REG_RCR, RCR_BRD );
  else
    sonic_write_register( rp, SONIC_REG_RCR, 0 );

  /*
   * Set up Resource Area pointers
   */

  sonic_write_register( rp, SONIC_REG_URRA, MSW(sc->rsa) );
  sonic_write_register( rp, SONIC_REG_RSA, LSW(sc->rsa) );

  sonic_write_register( rp, SONIC_REG_REA, LSW(sc->rea) );

  sonic_write_register( rp, SONIC_REG_RRP, LSW(sc->rsa) );
  sonic_write_register( rp, SONIC_REG_RWP, LSW(sc->rsa) ); /* XXX was rea */

  sonic_write_register( rp, SONIC_REG_URDA, MSW(sc->rda) );
  sonic_write_register( rp, SONIC_REG_CRDA, LSW(sc->rda) );

  sonic_write_register( rp, SONIC_REG_UTDA, MSW(sc->tdaTail) );
  sonic_write_register( rp, SONIC_REG_CTDA, LSW(sc->tdaTail) );

  /*
   * Set End Of Buffer Count register to the value recommended
   * in Note 1 of Section 3.4.4.4 of the SONIC data sheet.
   */

  sonic_write_register( rp, SONIC_REG_EOBC, RBUF_WC - 2 );

  /*
   *  Issue the load RRA command
   */

  sonic_write_register( rp, SONIC_REG_CR, CR_RRRA );
  while (sonic_read_register( rp, SONIC_REG_CR ) & CR_RRRA)
    continue;

  /*
   * Remove device reset
   */

  sonic_write_register( rp, SONIC_REG_CR, 0 );

  /*
   *  Set up the SONIC CAM with our hardware address.
   */

  hwaddr = sc->arpcom.ac_enaddr;
  cdp = sc->cdp;

#if (SONIC_DEBUG & SONIC_DEBUG_CAM)
  printf( "hwaddr: %2x:%2x:%2x:%2x:%2x:%2x\n",
     hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5] );
#endif

  cdp->cep  = 0;                     /* Fill first and only entry in CAM */
  cdp->cap0 = hwaddr[1] << 8 | hwaddr[0];
  cdp->cap1 = hwaddr[3] << 8 | hwaddr[2];
  cdp->cap2 = hwaddr[5] << 8 | hwaddr[4];
  cdp->ce   = 0x0001;                /* Enable first entry in CAM */

  sonic_write_register( rp, SONIC_REG_CDC, 1 );      /* 1 entry in CDA */
  sonic_write_register( rp, SONIC_REG_CDP, LSW(cdp) );
  sonic_write_register( rp, SONIC_REG_CR,  CR_LCAM );  /* Load the CAM */

  while (sonic_read_register( rp, SONIC_REG_CR ) & CR_LCAM)
    continue;

  /*
   * Verify that CAM was properly loaded.
   */

  sonic_write_register( rp, SONIC_REG_CR, CR_RST | CR_STP | CR_RXDIS | CR_HTX );

#if (SONIC_DEBUG & SONIC_DEBUG_CAM)
  sonic_write_register( rp, SONIC_REG_CEP, 0 );  /* Select first entry in CAM */
    printf ("Loaded Ethernet address into SONIC CAM.\n"
      "  Wrote %04x%04x%04x - %#x\n"
      "   Read %04x%04x%04x - %#x\n", 
        cdp->cap2, cdp->cap1, cdp->cap0, cdp->ce,
        sonic_read_register( rp, SONIC_REG_CAP2 ),
        sonic_read_register( rp, SONIC_REG_CAP1 ),
        sonic_read_register( rp, SONIC_REG_CAP0 ),
        sonic_read_register( rp, SONIC_REG_CE ));
#endif

  sonic_write_register( rp, SONIC_REG_CEP, 0 );  /* Select first entry in CAM */
  if ((sonic_read_register( rp, SONIC_REG_CAP2 ) != cdp->cap2)
   || (sonic_read_register( rp, SONIC_REG_CAP1 ) != cdp->cap1)
   || (sonic_read_register( rp, SONIC_REG_CAP0 ) != cdp->cap0)
   || (sonic_read_register( rp, SONIC_REG_CE ) != cdp->ce)) {
    printf ("Failed to load Ethernet address into SONIC CAM.\n"
      "  Wrote %04x%04x%04x - %#x\n"
      "   Read %04x%04x%04x - %#x\n", 
        cdp->cap2, cdp->cap1, cdp->cap0, cdp->ce,
        sonic_read_register( rp, SONIC_REG_CAP2 ),
        sonic_read_register( rp, SONIC_REG_CAP1 ),
        sonic_read_register( rp, SONIC_REG_CAP0 ),
        sonic_read_register( rp, SONIC_REG_CE ));
    rtems_panic ("SONIC LCAM");
  }

  sonic_write_register(rp, SONIC_REG_CR, /* CR_TXP | */CR_RXEN | CR_STP);

  /*
   * Attach SONIC interrupt handler
   */
/* XXX
  sonic_write_register( rp, SONIC_REG_IMR, 0 );
*/
  old_handler = set_vector(sonic_interrupt_handler, sc->vector, 0);

  /*
   * Remainder of hardware initialization is
   * done by the receive and transmit daemons.
   */
}

/*
 * Send packet (caller provides header).
 */

SONIC_STATIC void sonic_start(struct ifnet *ifp)
{
  struct sonic_softc *sc = ifp->if_softc;

  rtems_event_send(sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */

SONIC_STATIC void sonic_init (void *arg)
{
  struct sonic_softc *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  void *rp = sc->sonic;
  int rcr;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up SONIC hardware
     */
    sonic_initialize_hardware (sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SNtx", 4096, sonic_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SNrx", 4096, sonic_rxDaemon, sc);
  }

  /*
   * Set flags appropriately
   */
  rcr = sonic_read_register( rp, SONIC_REG_RCR );
  if (ifp->if_flags & IFF_PROMISC)
    rcr |= RCR_PRO;
  else
    rcr &= ~RCR_PRO;
  sonic_write_register( rp, SONIC_REG_RCR, rcr);

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  /* sonic_write_register( rp, SONIC_REG_IMR, 0 ); */
  sonic_enable_interrupts( rp, (IMR_PRXEN | IMR_RBAEEN) );

  sonic_write_register(rp, SONIC_REG_CR, /* CR_TXP | */ CR_RXEN);
}

/*
 * Driver ioctl handler
 */
static int
sonic_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
  struct sonic_softc *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl (ifp, command, data);
      break;

    case SIOCSIFFLAGS:
      switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
          sonic_stop (sc);
          break;

        case IFF_UP:
          sonic_init (sc);
          break;

        case IFF_UP | IFF_RUNNING:
          sonic_stop (sc);
          sonic_init (sc);
          break;

        default:
          break;
        }
      break;

    case SIO_RTEMS_SHOW_STATS:
      sonic_stats (sc);
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
 * Attach an SONIC driver to the system
 * This is the only `extern' function in the driver.
 */
int
rtems_sonic_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct sonic_softc *sc;
  struct ifnet *ifp;
  int mtu;
  int i;

  /*
   * Find an unused entry
   */
  i = 0;
  sc = sonic_softc;
  for (;;) {
    if (sc == &sonic_softc[NSONIC]) {
      printf ("No more SONIC devices.\n");
      return 0;
    }
    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc == NULL)
      break;
    sc++;
    i++;
  }

  /*
   *  zero out the control structure
   */

  memset( sc, 0, sizeof(*sc) );


  /*
   * Process options
   */
  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
    memset (sc->arpcom.ac_enaddr, 0x08, ETHER_ADDR_LEN);
  }
  if (config->mtu)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;
  if (config->rbuf_count)
    sc->rdaCount = config->rbuf_count;
  else
    sc->rdaCount = RDA_COUNT;
  if (config->xbuf_count)
    sc->tdaCount = config->xbuf_count;
  else
    sc->tdaCount = TDA_COUNT;
  sc->acceptBroadcast = !config->ignore_broadcast;

  sc->sonic = (void *) SONIC_BASE_ADDRESS;
  sc->vector = SONIC_VECTOR;

  /*
   * Set up network interface values
   */
  ifp->if_softc = sc;
  ifp->if_unit = i + 1;
  ifp->if_name = "sonic";
  ifp->if_mtu = mtu;
  ifp->if_init = sonic_init;
  ifp->if_ioctl = sonic_ioctl;
  ifp->if_start = sonic_start;
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
}

#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
#include <stdio.h>

char SONIC_Reg_name[64][6]= {
    "CR",         /* 0x00 */
    "DCR",        /* 0x01 */
    "RCR",        /* 0x02 */
    "TCR",        /* 0x03 */
    "IMR",        /* 0x04 */
    "ISR",        /* 0x05 */
    "UTDA",       /* 0x06 */
    "CTDA",       /* 0x07 */
    "0x08",       /* 0x08 */
    "0x09",       /* 0x09 */
    "0x0A",       /* 0x0A */
    "0x0B",       /* 0x0B */
    "0x0C",       /* 0x0C */
    "URDA",       /* 0x0D */
    "CRDA",       /* 0x0E */
    "0x0F",       /* 0x0F */
    "0x10",       /* 0x10 */
    "0x11",       /* 0x11 */
    "0x12",       /* 0x12 */
    "EOBC",       /* 0x13 */
    "URRA",       /* 0x14 */
    "RSA",        /* 0x15 */
    "REA",        /* 0x16 */
    "RRP",        /* 0x17 */
    "RWP",        /* 0x18 */
    "0x19",       /* 0x19 */
    "0x1A",       /* 0x1A */
    "0x1B",       /* 0x1B */
    "0x1C",       /* 0x1C */
    "0x0D",       /* 0x1D */
    "0x1E",       /* 0x1E */
    "0x1F",       /* 0x1F */
    "0x20",       /* 0x20 */
    "CEP",        /* 0x21 */
    "CAP2",       /* 0x22 */
    "CAP1",       /* 0x23 */
    "CAP0",       /* 0x24 */
    "CE",         /* 0x25 */
    "CDP",        /* 0x26 */
    "CDC",        /* 0x27 */
    "SR",         /* 0x28 */
    "WT0",        /* 0x29 */
    "WT1",        /* 0x2A */
    "RSC",        /* 0x2B */
    "CRCT",       /* 0x2C */
    "FAET",       /* 0x2D */
    "MPT",        /* 0x2E */
    "MDT",        /* 0x2F */
    "0x30",       /* 0x30 */
    "0x31",       /* 0x31 */
    "0x32",       /* 0x32 */
    "0x33",       /* 0x33 */
    "0x34",       /* 0x34 */
    "0x35",       /* 0x35 */
    "0x36",       /* 0x36 */
    "0x37",       /* 0x37 */
    "0x38",       /* 0x38 */
    "0x39",       /* 0x39 */
    "0x3A",       /* 0x3A */
    "0x3B",       /* 0x3B */
    "0x3C",       /* 0x3C */
    "0x3D",       /* 0x3D */
    "0x3E",       /* 0x3E */
    "DCR2"        /* 0x3F */
};
#endif
void sonic_write_register(
  void       *base,
  unsigned32  regno,
  unsigned32  value
)
{
  volatile unsigned32 *p = base;
{
  volatile unsigned32 *C = (void *)0x34CDF0;
  if ( *C ) printf( "W. *C = 0x%x\n", *C );
}

#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Write 0x%04x to %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  p[regno] = value;
}

unsigned32 sonic_read_register(
  void       *base,
  unsigned32  regno
)
{
  volatile unsigned32 *p = base;
  unsigned32           value;

{
  volatile unsigned32 *C = (void *)0x34CDF0;
  if ( *C ) printf( "R. *C = 0x%x\n", *C );
}

  value = p[regno];
#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Read 0x%04x from %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  return value;
}
