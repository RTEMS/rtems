/*
 *       RTEMS NETWORK DRIVER FOR NATIONAL DP83932 `SONIC'
 *         SYSTEMS-ORIENTED NETWORK INTERFACE CONTROLLER
 *
 *                     REUSABLE CHIP DRIVER
 *
 * References:
 *
 *  1) DP83932C-20/25/33 MHz SONIC(TM) Systems-Oriented Network Interface
 *     Controller data sheet.  TL/F/10492, RRD-B30M105, National Semiconductor,
 *     1995.
 *
 *  2) Software Driver Programmer's Guide for the DP83932 SONIC(TM),
 *     Application Note 746, Wesley Lee and Mike Lui, TL/F/11140,
 *     RRD-B30M75, National Semiconductor, March, 1991.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This driver was originally written and tested on a DY-4 DMV177,
 *  which had a 100 Mhz PPC603e.
 *
 *  This driver also works with DP83934CVUL-20/25 MHz, tested on
 *  Tharsys ERC32 VME board.
 *
 *  Rehaul to fix lost interrupts and buffers, and to use to use
 *  interrupt-free transmission by Jiri, 22/03/1999.
 */

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <libchip/sonic.h>

#include <stdio.h>
#include <string.h>

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

extern rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

#if (SONIC_DEBUG & SONIC_DEBUG_DUMP_MBUFS)
#include <rtems/dumpbuf.h>
#endif

/*
 *  Use the top line if you want more symbols.
 */

#define SONIC_STATIC static

/*
 * Number of devices supported by this driver
 */
#ifndef NSONIC
# define NSONIC 1
#endif

/*
 *
 * As suggested by National Application Note 746, make the
 * receive resource area bigger than the receive descriptor area.
 *
 * NOTE:  Changing this may break this driver since it currently
 *        assumes a 1<->1 mapping.
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
/* #define RBUF_WC    ((((MAXIMUM_FRAME_SIZE) + 3) & ~3) / 2) */
#define RBUF_WC    (RBUF_SIZE / 2)

/*
 * Macros for manipulating 32-bit pointers as 16-bit fragments
 */
#define LSW(p)   ((uint16_t)((uintptr_t)(p)))
#define MSW(p)   ((uint16_t)((uintptr_t)(p) >> 16))
#define PTR(m,l) ((void*)(((uint16_t)(m)<<16)|(uint16_t)(l)))

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
   *  Register access routines
   */
  sonic_write_register_t           write_register;
  sonic_read_register_t            read_register;

  /*
   * Interrupt vector
   */
  rtems_vector_number             vector;

  /*
   * Data Configuration Register values
   */
  uint32_t                  dcr_value;
  uint32_t                  dc2_value;

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
 *                         Debug Routines                         *
 *                                                                *
 ******************************************************************
 */

#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY_DESCRIPTORS)
void sonic_print_tx_descriptor(
  TransmitDescriptorPointer_t tdp
)
{
  printf( "TXD ==> %p", tdp );
  printf( "  pkt_config = 0x%04x", tdp->pkt_config & 0xffff);
  printf( "  pkt_size = 0x%04x\n", tdp->pkt_size & 0xffff );
  printf( "  frag_count = %d", tdp->frag_count & 0xffff );
  /* could print all the fragments */
  printf( "  next = %p", tdp->next );
  printf( "  linkp = %p\n", tdp->linkp );
  printf( "  mbufp = %p", tdp->mbufp );
  if ( tdp->mbufp )
    printf( "  mbufp->data = %p", mtod ( tdp->mbufp, void *) );
  puts("");
}

void sonic_print_rx_descriptor(
  ReceiveDescriptorPointer_t rdp
)
{
  printf( "RXD ==> %p\n", rdp );
  printf( "  status = 0x%04x", rdp->status & 0xffff );
  printf( "  byte_count = 0x%04x\n", rdp->byte_count & 0xffff );
  printf( "  pkt = 0x%04x%04x", rdp->pkt_msw, rdp->pkt_lsw );
  printf( "  seq_no = %d", rdp->seq_no );
  printf( "  link = %d\n", rdp->link );
  printf( "  in_use = %d", rdp->in_use );
  printf( "  next = %p", rdp->next );
  printf( "  mbufp = %p", rdp->mbufp );
  if ( rdp->mbufp )
    printf( "  mbufp->data = %p", mtod ( rdp->mbufp, void *) );
  puts("");
}
#endif

/*
 ******************************************************************
 *                                                                *
 *                        Support Routines                        *
 *                                                                *
 ******************************************************************
 */

static void sonic_enable_interrupts(
  struct sonic_softc *sc,
  uint32_t    mask
)
{
  void *rp = sc->sonic;
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
      (*sc->write_register)(
         rp,
         SONIC_REG_IMR,
         (*sc->read_register)(rp, SONIC_REG_IMR) | mask
      );
  rtems_interrupt_enable( level );
}

static void sonic_disable_interrupts(
  struct sonic_softc *sc,
  uint32_t    mask
)
{
  void *rp = sc->sonic;
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
  (*sc->write_register)(
         rp,
         SONIC_REG_IMR,
         (*sc->read_register)(rp, SONIC_REG_IMR) & ~mask
      );
  rtems_interrupt_enable( level );
}

static void sonic_clear_interrupts(
  struct sonic_softc *sc,
  uint32_t    mask
)
{
  void *rp = sc->sonic;
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
  (*sc->write_register)( rp, SONIC_REG_ISR, mask);
  rtems_interrupt_enable( level );
}

static void sonic_command(
  struct sonic_softc *sc,
  uint32_t    mask
)
{
  void *rp = sc->sonic;
  rtems_interrupt_level level;

  rtems_interrupt_disable( level );
  (*sc->write_register)( rp, SONIC_REG_CR, mask);
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
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Stop the transmitter and receiver.
   */
  sonic_command(sc, CR_HTX | CR_RXDIS );
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
  uint32_t   isr, imr;
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

  isr = (*sc->read_register)( rp, SONIC_REG_ISR );
  imr = (*sc->read_register)( rp, SONIC_REG_IMR );

  /*
   * Packet received or receive buffer area exceeded?
   */
  if (imr & isr & (IMR_PRXEN | IMR_RBAEEN)) {
    imr &= ~(IMR_PRXEN | IMR_RBAEEN);
    sc->rxInterrupts++;
    rtems_event_send (sc->rxDaemonTid, INTERRUPT_EVENT);
    (*sc->write_register)( rp, SONIC_REG_IMR, imr );
    (*sc->write_register)( rp, SONIC_REG_ISR, isr & ISR_PKTRX );
  }

  /*
   * Packet started, transmitter done or transmitter error?
   * TX interrupts only occur after an error or when all TDA's are
   * exhausted and we are waiting for buffer to come free.
   */
  if (imr & isr & (IMR_PINTEN | IMR_TXEREN)) {
    sc->txInterrupts++;
    rtems_event_send (sc->txDaemonTid, INTERRUPT_EVENT);
    (*sc->write_register)( rp, SONIC_REG_ISR, ISR_PINT | ISR_TXDN | ISR_TXER );
  }

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
  uint16_t   status;
  unsigned int collisions;
  struct mbuf *m, *n;

  /*
   * Repeat for all completed transmit descriptors.
   */
  while ((status = sc->tdaTail->status) != 0) {

#if (SONIC_DEBUG & SONIC_DEBUG_DESCRIPTORS)
    printf( "retire TDA %p (0x%04x)\n", sc->tdaTail, status );
#endif

#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
    /*
     *  If there is an error that was not a collision,
     *  then someone may want to see it.
     */

    if ( (status & ~(TDA_STATUS_COLLISION_MASK|TDA_STATUS_DEF)) != 0x0001 )
      printf( "ERROR: retire TDA %p (0x%08x)\n",
                sc->tdaTail, sc->tdaTail->status );
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
      uint16_t   link;
#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
      printf("restarting sonic after error\n");
#endif

      link = *(sc->tdaTail->linkp);

      if ((link & TDA_LINK_EOL) == 0) {
        void *rp = sc->sonic;

        (*sc->write_register)( rp, SONIC_REG_CTDA, link );
        sonic_command(sc, CR_TXP );
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
    m = sc->tdaTail->mbufp;
    while ( m ) {
      MFREE(m, n);
      m = n;
    }

    /*
    sc->tdaTail->frag[0].frag_link = LSW(sc->tdaTail->link_pad);
    sc->tdaTail->frag_count        = 0;
    */
    sc->tdaTail->status        = 0;

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
  struct mbuf *l = NULL;
  TransmitDescriptorPointer_t tdp;
  volatile struct TransmitDescriptorFragLink *fp;
  unsigned int packetSize;
  int i;
  rtems_event_set events;
  static char padBuf[64];

  /* printf( "sonic_sendpacket %p\n", m ); */


  /*
   * Wait for transmit descriptor to become available. Only retire TDA's
   * if there are no more free buffers to minimize TX latency. Retire TDA'a
   * on the way out.
   */

  while (sc->tdaHead->next->status != 0) {

  /*
   * Free up transmit descriptors
   */
    sonic_retire_tda (sc);

    if (sc->tdaHead->next->status == 0)
      break;

#if (SONIC_DEBUG & SONIC_DEBUG_ERRORS)
    printf("blocking until TDAs are available\n");
#endif
      /*
       * Enable PINT interrupts.
    sonic_clear_interrupts( sc, ISR_PINT );
    sonic_enable_interrupts( sc, IMR_PINTEN );
       */

      /*
       * Wait for PINT TX interrupt. Every fourth TX buffer will raise PINT.
       */
    rtems_bsdnet_event_receive (INTERRUPT_EVENT,
            RTEMS_WAIT|RTEMS_EVENT_ANY,
            RTEMS_NO_TIMEOUT,
            &events);
    sonic_disable_interrupts( sc, IMR_PINTEN );
    sonic_retire_tda (sc);
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
      rtems_print_buffer(
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
  sc->tdaHead = tdp;

  /* Start transmission */

  sonic_command(sc, CR_TXP );

  /*
   * Free up transmit descriptors on the way out.
   */
  sonic_retire_tda (sc);
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
    if ((*sc->read_register)( rp, SONIC_REG_ISR ) & ISR_RBAE) {

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
      if ((*sc->read_register)( rp, SONIC_REG_CR ) & CR_RXEN)
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
        if ((*sc->read_register)( rp, SONIC_REG_RRP ) ==
            (*sc->read_register)( rp, SONIC_REG_RSA ))
          (*sc->write_register)(
            rp,
            SONIC_REG_RRP,
            (*sc->read_register)( rp, SONIC_REG_REA )
          );
          (*sc->write_register)(
             rp,
             SONIC_REG_RRP,
             (*sc->read_register)(rp, SONIC_REG_RRP) - sizeof(ReceiveResource_t)
          );
      }

      /*
       * Restart reception
       */
      sonic_clear_interrupts( sc, ISR_RBAE );
      sonic_command( sc, CR_RXEN );
    }

    /*
     * Has Receive Descriptor become available?
     */
    if (rdp->in_use == RDA_IN_USE)
      break;

    /*
     * Enable interrupts.
     */
    sonic_enable_interrupts( sc, (IMR_PRXEN | IMR_RBAEEN) );

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

#ifdef CPU_U32_FIX

/*
 * Routine to align the received packet so that the ip header
 * is on a 32-bit boundary. Necessary for cpu's that do not
 * allow unaligned loads and stores and when the 32-bit DMA
 * mode is used.
 *
 * Transfers are done on word basis to avoid possibly slow byte
 * and half-word writes.
 */

void ipalign(struct mbuf *m)
{
  unsigned int *first, *last, data;
  unsigned int tmp = 0;

  if ((((int) m->m_data) & 2) && (m->m_len)) {
    last = (unsigned int *) ((((int) m->m_data) + m->m_len + 8) & ~3);
    first = (unsigned int *) (((int) m->m_data) & ~3);
    tmp = *first << 16;
    first++;
    do {
      data = *first;
      *first = tmp | (data >> 16);
      tmp = data << 16;
      first++;
    } while (first <= last);

    m->m_data = (caddr_t)(((int) m->m_data) + 2);
  }
}
#endif

/*
 * SONIC reader task
 */
SONIC_STATIC void sonic_rxDaemon (void *arg)
{
  struct sonic_softc *sc = (struct sonic_softc *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  void *rp = sc->sonic;
  struct mbuf *m;
  uint16_t   status;
  ReceiveDescriptorPointer_t rdp;
  ReceiveResourcePointer_t rwp, rea;
  uint16_t   newMissedTally, oldMissedTally;

  rwp = sc->rsa;
  rea = sc->rea;
  rdp = sc->rda;

  /*
   * Start the receiver
   */
  oldMissedTally = (*sc->read_register)( rp, SONIC_REG_MPT );

  /*
   * Input packet handling loop
   */
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
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY_DESCRIPTORS)
      sonic_print_rx_descriptor( rdp );
      if ((LSW(rdp->mbufp->m_data) != rdp->pkt_lsw)
       || (MSW(rdp->mbufp->m_data) != rdp->pkt_msw))
        printf ("SONIC RDA/RRA %p, %08x\n",rdp->mbufp->m_data,(rdp->pkt_msw << 16) |
	(rdp->pkt_lsw & 0x0ffff));
#endif
      rdp->byte_count &= 0x0ffff;    /* ERC32 pollutes msb of byte_count */
      m = rdp->mbufp;
      m->m_len = m->m_pkthdr.len = rdp->byte_count -
                          sizeof(uint32_t) -
                          sizeof(struct ether_header);
      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);

#ifdef CPU_U32_FIX
      ipalign(m);	/* Align packet on 32-bit boundary */
#endif

#if (SONIC_DEBUG & SONIC_DEBUG_DUMP_RX_MBUFS)
      rtems_print_buffer( (void *) eh, sizeof(struct ether_header) );
      rtems_print_buffer( (void *) m, 96 /* m->m_len*/ );
#endif

      /* printf( "ether_input %p\n", m ); */
      /*
      printf( "pkt %p, seq %04x, mbuf %p, m_data %p\n", rdp, rdp->seq_no, m, m->m_data );
      printf( "%u, %u\n", ((int*)m->m_data)[6], ((int*)m->m_data)[7]);
      */
      ether_input (ifp, eh, m);
      /*
      */

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
      (*sc->write_register)( rp, SONIC_REG_RWP , LSW(rwp) );

      /*
       * Tell the SONIC to reread the RRA.
       */
      if ((*sc->read_register)( rp, SONIC_REG_ISR ) & ISR_RBE)
      sonic_clear_interrupts( sc, ISR_RBE );
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
    newMissedTally = (*sc->read_register)( rp, SONIC_REG_MPT );
    if (newMissedTally != oldMissedTally) {
      sc->rxMissed += (newMissedTally - oldMissedTally) & 0xFFFF;
      newMissedTally = oldMissedTally;
    }

    /*
     * Move to next receive descriptor and update EOL
     */

    rdp->link |= RDA_LINK_EOL;
    rdp->in_use = RDA_FREE;
    sc->rdp_last->link &= ~RDA_LINK_EOL;
    sc->rdp_last = rdp;
    rdp = rdp->next;

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

  if ( (*sc->read_register)( rp, SONIC_REG_SR ) <= SONIC_REVISION_B ) {
    rtems_fatal_error_occurred( 0x0BADF00D );  /* don't eat this part :) */
  }

  /*
   *  Set up circular linked list in Transmit Descriptor Area.
   *  Use the PINT bit in the transmit configuration field to
   *  request an interrupt on every other transmitted packet.
   *
   *  NOTE: sonic_allocate() zeroes all of the memory allocated.
   */

  sc->tdaTail = sonic_allocate(sc->tdaCount * sizeof *tdp);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "tdaTail = %p\n", sc->tdaTail );
#endif
  tdp = sc->tdaTail;
  for (i = 0 ; i < sc->tdaCount ; i++) {
    /*
     *  Start off with the table of outstanding mbuf's
     */

    /*
     *  status, pkt_config, pkt_size, and all fragment fields
     *  are set to zero by sonic_allocate.
     */

/* XXX not used by the BSD drivers
    tdp->frag[0].frag_link = LSW(tdp + 1);
*/
    if (i & 3)
      tdp->pkt_config = TDA_CONFIG_PINT;

    tdp->status 	= 0;
    tdp->frag_count     = 0;
    tdp->link_pad       = LSW(tdp + 1) | TDA_LINK_EOL;
    tdp->linkp          = &((tdp + 1)->frag[0].frag_link);
    tdp->next           = (TransmitDescriptor_t *)(tdp + 1);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY_DESCRIPTORS)
    sonic_print_tx_descriptor( tdp );
#endif
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
  sc->rdp_last = ordp;

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
    sc->rda[i].mbufp = m;

    p = mtod (m, void *);

    /*
     * Set up RRA entry
     */
    rwp->buff_ptr_lsw = LSW(p);
    rwp->buff_ptr_msw = MSW(p);
    rwp->buff_wc_lsw = RBUF_WC;
    rwp->buff_wc_msw = 0;
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY_DESCRIPTORS)
    sonic_print_rx_descriptor( &sc->rda[i] );
#endif
  }
  sc->rea = rwp;
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rea area = %p\n", sc->rea );
#endif


  /*
   * Issue a software reset.
   */
  (*sc->write_register)( rp, SONIC_REG_CR, CR_RST | CR_STP | CR_RXDIS | CR_HTX );

  /*
   * Set up data configuration registers.
   */
  (*sc->write_register)( rp, SONIC_REG_DCR, sc->dcr_value );
  (*sc->write_register)( rp, SONIC_REG_DCR2, sc->dc2_value );

  (*sc->write_register)( rp, SONIC_REG_CR, CR_STP | CR_RXDIS | CR_HTX );

  /*
   * Mask all interrupts
   */
  (*sc->write_register)( rp, SONIC_REG_IMR, 0x0 ); /* XXX was backwards */

  /*
   * Clear outstanding interrupts.
   */
  (*sc->write_register)( rp, SONIC_REG_ISR, 0x7FFF );

  /*
   *  Clear the tally counters
   */

  (*sc->write_register)( rp, SONIC_REG_CRCT, 0xFFFF );
  (*sc->write_register)( rp, SONIC_REG_FAET, 0xFFFF );
  (*sc->write_register)( rp, SONIC_REG_MPT, 0xFFFF );
  (*sc->write_register)( rp, SONIC_REG_RSC, 0 );

  /*
   *  Set the Receiver mode
   *
   *  Enable/disable reception of broadcast packets
   */

  if (sc->acceptBroadcast)
    (*sc->write_register)( rp, SONIC_REG_RCR, RCR_BRD );
  else
    (*sc->write_register)( rp, SONIC_REG_RCR, 0 );

  /*
   * Set up Resource Area pointers
   */

  (*sc->write_register)( rp, SONIC_REG_URRA, MSW(sc->rsa) );
  (*sc->write_register)( rp, SONIC_REG_RSA, LSW(sc->rsa) );

  (*sc->write_register)( rp, SONIC_REG_REA, LSW(sc->rea) );

  (*sc->write_register)( rp, SONIC_REG_RRP, LSW(sc->rsa) );
  (*sc->write_register)( rp, SONIC_REG_RWP, LSW(sc->rsa) ); /* XXX was rea */

  (*sc->write_register)( rp, SONIC_REG_URDA, MSW(sc->rda) );
  (*sc->write_register)( rp, SONIC_REG_CRDA, LSW(sc->rda) );

  (*sc->write_register)( rp, SONIC_REG_UTDA, MSW(sc->tdaTail) );
  (*sc->write_register)( rp, SONIC_REG_CTDA, LSW(sc->tdaTail) );

  /*
   * Set End Of Buffer Count register to the value recommended
   * in Note 1 of Section 3.4.4.4 of the SONIC data sheet.
   */

  (*sc->write_register)( rp, SONIC_REG_EOBC, RBUF_WC - 2 );

  /*
   *  Issue the load RRA command
   */

  (*sc->write_register)( rp, SONIC_REG_CR, CR_RRRA );
  while ((*sc->read_register)( rp, SONIC_REG_CR ) & CR_RRRA)
    continue;

  /*
   * Remove device reset
   */

  (*sc->write_register)( rp, SONIC_REG_CR, 0 );

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

  (*sc->write_register)( rp, SONIC_REG_CDC, 1 );      /* 1 entry in CDA */
  (*sc->write_register)( rp, SONIC_REG_CDP, LSW(cdp) );
  (*sc->write_register)( rp, SONIC_REG_CR,  CR_LCAM );  /* Load the CAM */

  while ((*sc->read_register)( rp, SONIC_REG_CR ) & CR_LCAM)
    continue;

  /*
   * Verify that CAM was properly loaded.
   */

  (*sc->write_register)( rp, SONIC_REG_CR, CR_RST | CR_STP | CR_RXDIS | CR_HTX );

#if (SONIC_DEBUG & SONIC_DEBUG_CAM)
  (*sc->write_register)( rp, SONIC_REG_CEP, 0 );  /* Select first entry in CAM */
    printf ("Loaded Ethernet address into SONIC CAM.\n"
      "  Wrote %04x%04x%04x - %#x\n"
      "   Read %04x%04x%04x - %#x\n",
        cdp->cap2, cdp->cap1, cdp->cap0, cdp->ce,
        (*sc->read_register)( rp, SONIC_REG_CAP2 ),
        (*sc->read_register)( rp, SONIC_REG_CAP1 ),
        (*sc->read_register)( rp, SONIC_REG_CAP0 ),
        (*sc->read_register)( rp, SONIC_REG_CE ));

  (*sc->write_register)( rp, SONIC_REG_CEP, 0 );  /* Select first entry in CAM */
  if (((*sc->read_register)( rp, SONIC_REG_CAP2 ) != cdp->cap2)
   || ((*sc->read_register)( rp, SONIC_REG_CAP1 ) != cdp->cap1)
   || ((*sc->read_register)( rp, SONIC_REG_CAP0 ) != cdp->cap0)
   || ((*sc->read_register)( rp, SONIC_REG_CE ) != cdp->ce)) {
    printf ("Failed to load Ethernet address into SONIC CAM.\n"
      "  Wrote %04x%04x%04x - %#x\n"
      "   Read %04x%04x%04x - %#x\n",
        cdp->cap2, cdp->cap1, cdp->cap0, cdp->ce,
        (*sc->read_register)( rp, SONIC_REG_CAP2 ),
        (*sc->read_register)( rp, SONIC_REG_CAP1 ),
        (*sc->read_register)( rp, SONIC_REG_CAP0 ),
        (*sc->read_register)( rp, SONIC_REG_CE ));
    rtems_panic ("SONIC LCAM");
  }
#endif

  (*sc->write_register)(rp, SONIC_REG_CR, /* CR_TXP | */CR_RXEN | CR_STP);

  /*
   * Attach SONIC interrupt handler
   */
/* XXX
  (*sc->write_register)( rp, SONIC_REG_IMR, 0 );
*/

  /* Ignore returned old handler */
  (void) set_vector(sonic_interrupt_handler, sc->vector, 1);

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
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SNrx", 4096, sonic_rxDaemon, sc);
    sc->txDaemonTid = rtems_bsdnet_newproc ("SNtx", 4096, sonic_txDaemon, sc);
  }

  /*
   * Set flags appropriately
   */
  rcr = (*sc->read_register)( rp, SONIC_REG_RCR );
  if (ifp->if_flags & IFF_PROMISC)
    rcr |= RCR_PRO;
  else
    rcr &= ~RCR_PRO;
  (*sc->write_register)( rp, SONIC_REG_RCR, rcr);

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  sonic_enable_interrupts( sc, IMR_TXEREN | (IMR_PRXEN | IMR_RBAEEN) );
  sonic_command( sc, CR_RXEN );
}

/*
 * Driver ioctl handler
 */
static int
sonic_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
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
rtems_sonic_driver_attach (
  struct rtems_bsdnet_ifconfig *config,
  sonic_configuration_t *chip
)
{
  struct sonic_softc *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;

  /*
   * Parse driver name
   */
  if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
    return 0;

  /*
   * Is driver free?
   */
  if ((unitNumber <= 0) || (unitNumber > NSONIC)) {
    printf ("Bad SONIC unit number.\n");
     return 0;
  }
  sc = &sonic_softc[unitNumber - 1];
  ifp = &sc->arpcom.ac_if;
  if (ifp->if_softc != NULL) {
    printf ("Driver already in use.\n");
    return 0;
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
    sc->rdaCount = chip->rda_count;
  if (config->xbuf_count)
    sc->tdaCount = config->xbuf_count;
  else
    sc->tdaCount = chip->tda_count;
  sc->acceptBroadcast = !config->ignore_broadcast;

  sc->sonic = chip->base_address;
  sc->vector = chip->vector;
  sc->dcr_value = chip->dcr_value;
  sc->dc2_value  = chip->dc2_value;
  sc->write_register = chip->write_register;
  sc->read_register  = chip->read_register;

  /*
   * Set up network interface values
   */
  ifp->if_softc = sc;
  ifp->if_unit = unitNumber;
  ifp->if_name = unitName;
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
