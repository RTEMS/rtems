/*
 *******************************************************************
 *******************************************************************
 **                                                               **
 **         RTEMS/KA9Q DRIVER FOR NATIONAL DP83932 `SONIC'        **
 **         SYSTEMS-ORIENTED NETWORK INTERFACE CONTROLLER         **
 **                                                               **
 *******************************************************************
 *******************************************************************
 */

/*
 * $Revision$   $Date$   $Author$
 * $State$
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

#include "sonic.h"

#include <rtems/error.h>
#include <ka9q/rtems_ka9q.h>
#include <ka9q/global.h>
#include <ka9q/domain.h>
#include <ka9q/enet.h>
#include <ka9q/iface.h>
#include <ka9q/netuser.h>
#include <ka9q/trace.h>
#include <ka9q/commands.h>

/*
 * Debug levels
 *
 */

#define SONIC_DEBUG_NONE             0x0000
#define SONIC_DEBUG_ALL              0xFFFF
#define SONIC_DEBUG_PRINT_REGISTERS  0x0001
#define SONIC_DEBUG_MEMORY           0x0002

#define SONIC_DEBUG      SONIC_DEBUG_ALL

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
   (DCR_DW32 | DCR_WAIT0 | DCR_PO0 | DCR_PO1  | DCR_RFT24 | DCR_TFT28)
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
#define RRA_EXTRA_COUNT  3

/*
 * RTEMS event used by interrupt handler to signal daemons.
 */
#define INTERRUPT_EVENT  RTEMS_EVENT_1

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
struct sonic {
  /*
   * Connection to KA9Q
   */
  struct iface                     *iface;

  /*
   * Default location of device registers
   * ===CACHE===
   * This area must be non-cacheable, guarded.
   */
  void                             *sonic;

  /*
   * Interrupt vector
   */
  rtems_vector_number             vector;

  /*
   * Task waiting for transmit resources
   */
  rtems_id                        txWaitTid;

  /*
   * Receive resource area
   */
  int                             rdaCount;
  ReceiveResourcePointer_t        rsa;
  ReceiveDescriptorPointer_t      rdp_last;

  /*
   * Transmit descriptors
   */
  int        tdaCount;
  TransmitDescriptorPointer_t     tdaHead;  /* Last filled */
  TransmitDescriptorPointer_t     tdaTail;  /* Next to retire */
  int                             tdaActiveCount;

  /*
   * Statistics
   */
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
SONIC_STATIC struct sonic sonic[NSONIC];

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

/*
 * Allocate non-cacheable memory on a single 64k page.
 * Very simple minded -- just keeps trying till the memory is on a single page.
 */
SONIC_STATIC void * sonic_allocate (unsigned int nbytes)
{
  void *p;
  unsigned long a1, a2;

  for (;;) {
    /*
     * ===CACHE===
     * Change malloc to malloc_noncacheable_guarded.
     */
    p = calloc(1, nbytes);
    if (p == NULL)
      rtems_panic ("No memory!");
    a1 = (unsigned long)p;
    a2 = a1 + nbytes - 1;
    if ((a1 >> 16) == (a2 >> 16))
      break;
  }
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "sonic_allocate %d bytes at %p\n", nbytes, p );
#endif
  return p;
}

/*
 * Shut down the interface.
 * This is a pretty simple-minded routine.  It doesn't worry
 * about cleaning up mbufs, shutting down daemons, etc.
 */

SONIC_STATIC int sonic_stop (struct iface *iface)
{
  int i;
  struct sonic *dp = &sonic[iface->dev];
  void *rp = dp->sonic;

  /*
   * Stop the transmitter and receiver.
   */
  sonic_write_register( rp, SONIC_REG_CR, CR_HTX | CR_RXDIS );

  /*
   * Wait for things to stop.
   * For safety's sake, there is an alternate exit.
   */
  i = 0;
  while (sonic_read_register( rp, SONIC_REG_CR ) & (CR_RXEN | CR_TXP)) {
    if (++i == 10000)
      break;
  }

  /*
   * Reset the device
   */
  sonic_write_register( rp, SONIC_REG_CR, CR_RST );
  sonic_write_register( rp, SONIC_REG_IMR, 0 );
  return 0;
}

/*
 * Show interface statistics
 */

SONIC_STATIC void sonic_show (struct iface *iface)
{
  struct sonic *dp = &sonic[iface->dev];

  printf ("    Rx Interrupts:%-8lu", dp->rxInterrupts);
  printf ("            Giant:%-8lu", dp->rxGiant);
  printf ("        Non-octet:%-8lu\n", dp->rxNonOctet);
  printf ("          Bad CRC:%-8lu", dp->rxBadCRC);
  printf ("        Collision:%-8lu", dp->rxCollision);
  printf ("           Missed:%-8lu\n", dp->rxMissed);

  printf (    "    Tx Interrupts:%-8lu", dp->txInterrupts);
  printf (  "           Deferred:%-8lu", dp->txDeferred);
  printf ("        Lost Carrier:%-8lu\n", dp->txLostCarrier);
  printf (   "Single Collisions:%-8lu", dp->txSingleCollision);
  printf ( "Multiple Collisions:%-8lu", dp->txMultipleCollision);
  printf ("Excessive Collisions:%-8lu\n", dp->txExcessiveCollision);
  printf (   " Total Collisions:%-8lu", dp->txCollision);
  printf ( "     Late Collision:%-8lu", dp->txLateCollision);
  printf ("            Underrun:%-8lu\n", dp->txUnderrun);
  printf (   "  Raw output wait:%-8lu\n", dp->txRawWait);
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
  struct sonic *dp = sonic;
  void *rp;

#if (NSONIC > 1)
  /*
   * Find the device which requires service
   */
  for (;;) {
    if (dp->vector == v)
      break;
    if (++dp == &sonic[NSONIC])
      return;  /* Spurious interrupt? */
  }
#endif /* NSONIC > 1 */

  /*
   * Get pointer to SONIC registers
   */
  rp = dp->sonic;

  /*
   * Packet received or receive buffer area exceeded?
   */
  if ((sonic_read_register( rp, SONIC_REG_IMR ) & (IMR_PRXEN | IMR_RBAEEN)) &&
      (sonic_read_register( rp, SONIC_REG_ISR ) & (ISR_PKTRX | ISR_RBAE))) {
    sonic_write_register(
       rp,
       SONIC_REG_IMR,
       sonic_read_register( rp, SONIC_REG_IMR) & ~(IMR_PRXEN | IMR_RBAEEN)
    );
    dp->rxInterrupts++;
    rtems_event_send (dp->iface->rxproc, INTERRUPT_EVENT);
  }

  /*
   * Packet started, transmitter done or transmitter error?
   */
  if ((sonic_read_register( rp, SONIC_REG_IMR ) & (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN))
   && (sonic_read_register( rp, SONIC_REG_ISR ) & (ISR_PINT | ISR_TXDN | ISR_TXER))) {
    sonic_write_register(
       rp,
       SONIC_REG_IMR,
       sonic_read_register( rp, SONIC_REG_IMR) &
                  ~(IMR_PINTEN | IMR_PTXEN | IMR_TXEREN)
    );
    dp->rxInterrupts++;
    dp->txInterrupts++;
    rtems_event_send (dp->txWaitTid, INTERRUPT_EVENT);
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

SONIC_STATIC void sonic_retire_tda (struct sonic *dp)
{
  rtems_unsigned16 status;
  unsigned int collisions;

  /*
   * Repeat for all completed transmit descriptors.
   */
  while ((dp->tdaActiveCount != 0)
      && ((status = dp->tdaTail->status) != 0)) {
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
      link = *(dp->tdaTail->linkp);

      if ((link & TDA_LINK_EOL) == 0) {
        void *rp = dp->sonic;

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
        dp->txSingleCollision++;
      else
        dp->txMultipleCollision++;
      dp->txCollision += collisions;
    }
    if (status & TDA_STATUS_EXC)
      dp->txExcessiveCollision++;
    if (status & TDA_STATUS_OWC)
      dp->txLateCollision++;
    if (status & TDA_STATUS_EXD)
      dp->txExcessiveDeferral++;
    if (status & TDA_STATUS_DEF)
      dp->txDeferred++;
    if (status & TDA_STATUS_FU)
      dp->txUnderrun++;
    if (status & TDA_STATUS_CRSL)
      dp->txLostCarrier++;

    /*
     * Free the packet
     */
    dp->tdaActiveCount--;
    free_p ((struct mbuf **)&dp->tdaTail->mbufp);

    /*
     * Move to the next transmit descriptor
     */
    dp->tdaTail = dp->tdaTail->next;
  }
}

/*
 * Send raw packet (caller provides header).
 * This code runs in the context of the interface transmit
 * task (most packets)  or in the context of the network
 * task (for ARP requests).
 */

SONIC_STATIC int sonic_raw (struct iface *iface, struct mbuf **bpp)
{
  struct sonic *dp = &sonic[iface->dev];
  void *rp = dp->sonic;
  struct mbuf *bp;
  TransmitDescriptorPointer_t tdp;
  volatile struct TransmitDescriptorFragLink *fp;
  unsigned int packetSize;
  int i;
  static char padBuf[64];

  /*
   * Update the log.
   */
  iface->rawsndcnt++;
  iface->lastsent = secclock ();
  dump (iface, IF_TRACE_OUT, *bpp);

  /*
   * It would not do to have two tasks active in the transmit
   * loop at the same time.
   * The blocking is simple-minded since the odds of two tasks
   * simultaneously attempting to use this code are low.  The only
   * way that two tasks can try to run here is:
   *  1) Task A enters this code and ends up having to
   *     wait for a transmit buffer descriptor.
   *  2) Task B gains control and tries to transmit a packet.
   * The RTEMS/KA9Q scheduling semaphore ensures that there
   * are no race conditions associated with manipulating the
   * txWaitTid variable.
   */
  if (dp->txWaitTid) {
    dp->txRawWait++;
    while (dp->txWaitTid)
      rtems_ka9q_ppause (10);
  }

  /*
   * Free up transmit descriptors.
   */
  sonic_retire_tda (dp);

  /*
   * Wait for transmit descriptor to become available.
   */
  if (dp->tdaActiveCount == dp->tdaCount) {
    /*
     * Find out who we are
     */
    if (dp->txWaitTid == 0)
      rtems_task_ident (RTEMS_SELF, 0, &dp->txWaitTid);

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
    sonic_retire_tda (dp);
    while (dp->tdaActiveCount == dp->tdaCount) {
      /*
       * Enable transmitter interrupts.
       */
      sonic_write_register(
         rp,
         SONIC_REG_IMR,
         sonic_read_register( rp, SONIC_REG_IMR) |
                (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN)
      );

      /*
       * Wait for interrupt
       */
      rtems_ka9q_event_receive (INTERRUPT_EVENT,
            RTEMS_WAIT|RTEMS_EVENT_ANY,
            RTEMS_NO_TIMEOUT);
      sonic_write_register( rp, SONIC_REG_ISR, ISR_PINT | ISR_TXDN | ISR_TXER );
      sonic_retire_tda (dp);
    }
  }

  /*
   * Get the head of the packet mbuf chain.
   */
  bp = *bpp;

  /*
   * Fill in the transmit descriptor fragment descriptors.
   * ===CACHE===
   * If data cache is operating in write-back mode, flush cached
   * data to memory.
   */
  tdp = dp->tdaHead->next;
  tdp->mbufp = bp;
  packetSize = 0;
  fp = tdp->frag;
  for (i = 0 ; i < MAXIMUM_FRAGS_PER_DESCRIPTOR ; i++, fp++) {
    fp->frag_lsw = LSW(bp->data);
    fp->frag_msw = MSW(bp->data);
    fp->frag_size = bp->cnt;
    packetSize += bp->cnt;

    /*
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if ((bp = bp->next) == NULL)
      break;
  }

  /*
   * Pad short packets.
   */
  if  ((packetSize < 64) && (i < MAXIMUM_FRAGS_PER_DESCRIPTOR)) {
    int padSize = 64 - packetSize;
    fp->frag_lsw = LSW(padBuf);
    fp->frag_msw = MSW(padBuf);
    fp->frag_size = padSize;
    packetSize += padSize;
    i++;
    fp++;
  }

  /*
   * Fill Transmit Descriptor
   */
  tdp->pkt_size = packetSize;
  tdp->frag_count = i;
  tdp->status = 0;

  /*
   * Chain onto list and start transmission.
   */
  tdp->linkp = &fp->frag_link;
  *tdp->linkp = LSW(tdp->next) | TDA_LINK_EOL;
  *dp->tdaHead->linkp &= ~TDA_LINK_EOL;
  sonic_write_register( rp, SONIC_REG_CR, CR_TXP );
  dp->tdaActiveCount++;
  dp->tdaHead = tdp;

  /*
   * Let KA9Q know the packet is on the way.
   */
  dp->txWaitTid = 0;
  *bpp = NULL;
  return 0;
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
  struct sonic *dp,
  ReceiveDescriptorPointer_t rdp
)
{
  int i;
  void *rp = dp->sonic;

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
      /*
       * One more check to soak up any Receive Descriptors
       * that may already have been handed back to the driver.
       */
      if (rdp->in_use == 0)
        break;

      /*
       * Check my interpretation of the SONIC manual.
       */
      if (sonic_read_register( rp, SONIC_REG_CR ) & CR_RXEN)
        rtems_panic ("SONIC RBAE/RXEN");

      /*
       * Update statistics
       */
      dp->rxGiant++;

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
    if (rdp->in_use == 0)
      break;

    /*
     * Enable interrupts.
     */
    sonic_write_register(
       rp,
       SONIC_REG_IMR,
       sonic_read_register( rp, SONIC_REG_IMR) | (IMR_PRXEN | IMR_RBAEEN)
    );

    /*
     * Wait for interrupt.
     */
    rtems_ka9q_event_receive (INTERRUPT_EVENT,
            RTEMS_WAIT|RTEMS_EVENT_ANY,
            RTEMS_NO_TIMEOUT);
  }
}

/*
 * SCC reader task
 */

SONIC_STATIC void sonic_rx (int dev, void *p1, void *p2)
{
  struct iface *iface = (struct iface *)p1;
  struct sonic *dp = (struct sonic *)p2;
  void *rp = dp->sonic;
  struct mbuf *bp;
  rtems_unsigned16 status;
  ReceiveDescriptorPointer_t rdp;
  ReceiveResourcePointer_t rwp, rea;
  rtems_unsigned16 newMissedTally, oldMissedTally;
  int continuousCount;

  rwp = dp->rsa;
  rea = rwp;
  rdp = dp->rdp_last;

  /*
   * Start the receiver
   */
  oldMissedTally = sonic_read_register( rp, SONIC_REG_MPT );
  sonic_write_register( rp, SONIC_REG_CR, CR_RRRA );
  sonic_write_register( rp, SONIC_REG_CR, CR_RXEN );

  /*
   * Input packet handling loop
   */
  continuousCount = 0;
  for (;;) {
    /*
     * Wait till SONIC supplies a Receive Descriptor.
     */
    if (rdp->in_use) {
      continuousCount = 0;
      sonic_rda_wait (dp, rdp);
    }

    /*
     * Check that packet is valid
     */
    status = rdp->status;
    if (status & RDA_STATUS_PRX) {
      struct mbuf **mbp;
      void *p;

      /*
       * Get the mbuf pointer
       */
      p = PTR(rdp->pkt_msw, rdp->pkt_lsw);
      mbp = (struct mbuf **)p - 1;
      bp = *mbp;

      /*
       * Pass the packet up the chain.
       * The mbuf count is reduced to remove
       * the frame check sequence at the end
       * of the packet.
       * ===CACHE===
       * Invalidate cache entries for this memory.
       */
      bp->cnt = rdp->byte_count - sizeof (uint32);
      net_route (iface, &bp);

      /*
       * Give the network code a chance to digest the
       * packet.  This guards against a flurry of 
       * incoming packets (usually an ARP storm) from
       * using up all the available memory.
       */
      if (++continuousCount >= dp->rdaCount)
        kwait_null ();

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
      bp = ambufw (RBUF_SIZE);
      mbp = (struct mbuf **)bp->data;
      bp->data += sizeof *mbp;
      *mbp = bp;

      /*
       * Reuse Receive Resource.
       */
      rwp->buff_ptr_lsw = LSW(bp->data);
      rwp->buff_ptr_msw = MSW(bp->data);
      rwp++;
      if (rwp == rea)
        rwp = dp->rsa;
      sonic_write_register( rp, SONIC_REG_RWP , LSW(rwp) );

      /*
       * Tell the SONIC to reread the RRA.
       */
      if (sonic_read_register( rp, SONIC_REG_ISR ) & ISR_RBE)
        sonic_write_register( rp, SONIC_REG_ISR, ISR_RBE );
    }
    else {
      if (status & RDA_STATUS_COL)
        dp->rxCollision++;
      if (status & RDA_STATUS_FAER)
        dp->rxNonOctet++;
      else if (status & RDA_STATUS_CRCR)
        dp->rxBadCRC++;
    }

    /*
     * Count missed packets
     */
    newMissedTally = sonic_read_register( rp, SONIC_REG_MPT );
    if (newMissedTally != oldMissedTally) {
      dp->rxMissed += (newMissedTally - oldMissedTally) & 0xFFFF;
      newMissedTally = oldMissedTally;
    }

    /*
     * Move to next receive descriptor
     */
    rdp->link |= RDA_LINK_EOL;
    rdp->in_use = 1;
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
SONIC_STATIC void sonic_initialize_hardware(
  struct sonic *dp,
  int broadcastFlag
)
{
  void *rp = dp->sonic;
  int i;
  ReceiveDescriptor_t *rda;
  unsigned char *hwaddr;
  rtems_isr_entry old_handler;
  TransmitDescriptorPointer_t otdp, tdp;
  ReceiveDescriptorPointer_t ordp, rdp;
  ReceiveResourcePointer_t rwp, rea;
  struct mbuf *bp;
  struct CamDescriptor{
    struct {
      rtems_unsigned32  cep;  /* CAM Entry Pointer */
      rtems_unsigned32  cap0; /* CAM Address Port 0 xx-xx-xx-xx-YY-YY */
      rtems_unsigned32  cap1; /* CAM Address Port 1 xx-xx-YY-YY-xxxx */
      rtems_unsigned32  cap2; /* CAM Address Port 2 YY-YY-xx-xx-xx-xx */
    } desc[16];
    rtems_unsigned32  ce;
  };
  volatile struct CamDescriptor *cdp;

  /*
   *  Set up circular linked list in Transmit Descriptor Area.
   *  Use the PINT bit in the transmit configuration field to
   *  request an interrupt on every other transmitted packet.
   *
   *  NOTE: sonic_allocate() zeroes all of the memory allocated.
   */

  dp->tdaActiveCount = 0;
  dp->tdaTail = sonic_allocate (dp->tdaCount * sizeof *tdp);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "tdaTail = %p\n", dp->tdaTail );
#endif
  otdp = tdp = dp->tdaTail;
  for (i = 0 ; i < dp->tdaCount ; i++) {
    if (i & 1)
      tdp->pkt_config = TDA_CONFIG_PINT;
    else
      tdp->pkt_config = 0;
    tdp->frag_count = 1;
    if (i == (dp->tdaCount - 1))
      tdp->next = (TransmitDescriptor_t *)dp->tdaTail;
    else
      tdp->next = (TransmitDescriptor_t *)(tdp + 1);
    otdp = tdp;
    tdp = tdp->next;
  }
  dp->tdaHead = otdp;
  dp->tdaHead->linkp = &dp->tdaHead->frag[0].frag_link;

  /*
   *  Set up circular linked list in Receive Descriptor Area.
   *  Leaves ordp pointing at the `end' of the list and
   *  rdp pointing at the `beginning' of the list.
   */

  rda = sonic_allocate (dp->rdaCount * sizeof *rda);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rda area = %p\n", rda );
#endif
  ordp = rdp = rda;
  for (i = 0 ; i < dp->rdaCount ; i++) {
    /*
     * Set up RDA entry
     */
    if (i == (dp->rdaCount - 1))
      rdp->next = rda;
    else
      rdp->next = (ReceiveDescriptor_t *)(rdp + 1);
    rdp->in_use = 0;           /* XXX was 1 */
    ordp = rdp;
    rdp = rdp->next;
    ordp->link = LSW(rdp);
  }
  ordp->link |= RDA_LINK_EOL;
  dp->rdp_last = rdp;

  /*
   * Allocate the receive resource area.
   * In accordance with National Application Note 746, make the
   * receive resource area bigger than the receive descriptor area.
   * This has the useful side effect of making the receive resource
   * area big enough to hold the CAM descriptor area.
   */

  dp->rsa = sonic_allocate ((dp->rdaCount + RRA_EXTRA_COUNT) * sizeof *dp->rsa);
#if (SONIC_DEBUG & SONIC_DEBUG_MEMORY)
  printf( "rsa area = %p\n", dp->rsa );
#endif

  /*
   *  Set up list in Receive Resource Area.
   *  Allocate space for incoming packets.
   */

  rwp = dp->rsa;
  for (i = 0 ; i < (dp->rdaCount + RRA_EXTRA_COUNT) ; i++, rwp++) {
    struct mbuf **mbp;

    /*
     * Allocate memory for buffer.
     * Place a pointer to the mbuf at the beginning of the buffer
     * so we can find the mbuf when the SONIC returns the buffer
     * to the driver.
     */
    bp = ambufw (RBUF_SIZE);
    mbp = (struct mbuf **)bp->data;
    bp->data += sizeof *mbp;
    *mbp = bp;

    /*
     * Set up RRA entry
     */
    rwp->buff_ptr_lsw = LSW(bp->data);
    rwp->buff_ptr_msw = MSW(bp->data);
    rwp->buff_wc_lsw = RBUF_WC;
    rwp->buff_wc_msw = 0;
  }
  rea = rwp;

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
  sonic_write_register( rp, SONIC_REG_IMR, 0x3fff );

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

  /*
   *  Set the Receiver mode
   *
   *  Enable/disable reception of broadcast packets
   */

  if (broadcastFlag)
    sonic_write_register( rp, SONIC_REG_RCR, RCR_BRD );
  else
    sonic_write_register( rp, SONIC_REG_RCR, 0 );

  /*
   * Set up Resource Area pointers
   */
  sonic_write_register( rp, SONIC_REG_URRA, MSW(dp->rsa) );
  sonic_write_register( rp, SONIC_REG_RSA, LSW(dp->rsa) );

  sonic_write_register( rp, SONIC_REG_REA, LSW(rea) );

  sonic_write_register( rp, SONIC_REG_RRP, LSW(dp->rsa) );
  sonic_write_register( rp, SONIC_REG_RWP, LSW(dp->rsa) ); /* XXX was rea */
  sonic_write_register( rp, SONIC_REG_RSC, 0 );

  sonic_write_register( rp, SONIC_REG_URDA, MSW(rdp) );
  sonic_write_register( rp, SONIC_REG_CRDA, LSW(rdp) );

  sonic_write_register( rp, SONIC_REG_UTDA, MSW(dp->tdaTail) );
  sonic_write_register( rp, SONIC_REG_CTDA, LSW(dp->tdaTail) );

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

#if 0
  /*
   * Remove device reset
   */

  sonic_write_register( rp, SONIC_REG_CR, 0 );
#endif

  /*
   * Set up the SONIC CAM with our hardware address.
   * Use the Receive Resource Area to hold the CAM Descriptor Area.
   */
  hwaddr = dp->iface->hwaddr;
  cdp = (struct CamDescriptor *)dp->rsa;
  for (i=0 ; i<16 ; i++ ) {
    cdp->desc[i].cep = i;
  }

  cdp->desc[0].cep = 0;      /* Fill first entry in CAM */
  cdp->desc[0].cap2 = hwaddr[0] << 8 | hwaddr[1];
  cdp->desc[0].cap1 = hwaddr[2] << 8 | hwaddr[3];
  cdp->desc[0].cap0 = hwaddr[4] << 8 | hwaddr[5];
  cdp->ce = 0x0001;    /* Enable first entry in CAM */

  sonic_write_register( rp, SONIC_REG_CDC, 16 );      /* 16 entries in CDA */
  sonic_write_register( rp, SONIC_REG_CDP, LSW(cdp) );
  sonic_write_register( rp, SONIC_REG_CR, CR_LCAM );  /* Load the CAM */
  while (sonic_read_register( rp, SONIC_REG_CR ) & CR_LCAM)
    continue;

  /*
   * Verify that CAM was properly loaded.
   */
#if 0
  sonic_write_register( rp, SONIC_REG_CEP, 0 );      /* Select first entry in CAM */
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
#endif

  /*
   * Attach SONIC interrupt handler
   */
  sonic_write_register( rp, SONIC_REG_IMR, 0 );
  old_handler = set_vector(sonic_interrupt_handler, dp->vector, 0);

  /*
   * Remainder of hardware initialization is
   * done by the receive and transmit daemons.
   */
}

/*
 * Attach an SONIC driver to the system
 * This is the only `extern' function in the driver.
 *
 * argv[0]: interface label, e.g. "rtems"
 * The remainder of the arguments are optional key/value pairs:
 * mtu ##                  --  maximum transmission unit, default 1500
 * broadcast y/n           -- accept or ignore broadcast packets, default yes
 * rbuf ##                 -- Set number of receive descriptor entries
 * tbuf ##                 -- Set number of transmit descriptor entries
 * ip ###.###.###.###      -- IP address
 * ether ##:##:##:##:##:## -- Ethernet address
 * reg ######              -- Address of SONIC device registers
 * vector ###              -- SONIC interrupt vector
 */
int
rtems_ka9q_driver_attach (int argc, char *argv[], void *p)
{
  struct sonic *dp;
  struct iface *iface;
  char *cp;
  int argIndex;
  int broadcastFlag;
  char cbuf[30];

  /*
   * Find an unused entry
   */
  dp = sonic;
  for (;;) {
    if (dp == &sonic[NSONIC]) {
      printf ("No more SONIC devices.\n");
      return -1;
    }
    if (dp->iface == NULL)
      break;
    dp++;
  }
  if (if_lookup (argv[0]) != NULL) {
    printf ("Interface %s already exists\n", argv[0]);
    return -1;
  }

  /*
   * Create an inteface descriptor
   */
  iface = callocw (1, sizeof *iface);
  iface->name = strdup (argv[0]);
  iface->dev = dp - sonic;;

  /*
   * Set default values
   */
  broadcastFlag = 1;
  dp->txWaitTid = 0;
  dp->rdaCount = RDA_COUNT;
  dp->tdaCount = TDA_COUNT;
  iface->mtu = 1500;
  iface->addr = Ip_addr;
  iface->hwaddr = mallocw (EADDR_LEN);
  memset (iface->hwaddr, 0x08, EADDR_LEN);
  dp->sonic = (struct SonicRegisters *)SONIC_BASE_ADDRESS;
  dp->vector = SONIC_VECTOR;

  /*
   * Parse remaining arguments
   */
  for (argIndex = 1 ; argIndex < (argc - 1) ; argIndex++) {
    if (strcmp ("mtu", argv[argIndex]) == 0) {
      iface->mtu = strtoul (argv[++argIndex], NULL, 0);
    }
    else if (strcmp ("broadcast", argv[argIndex]) == 0) {
      if (*argv[++argIndex] == 'n')
        broadcastFlag = 0;
    }
    else if (strcmp ("rbuf", argv[argIndex]) == 0) {
      /*
       * The minimum RDA count is 2.  A single-entry RDA
       * would be difficult to use since the SONIC does
       * not release (in_use = 0) the RDA that has the
       * EOL bit set.
       */
      dp->rdaCount = strtoul (argv[++argIndex], NULL, 0);
      if ((dp->rdaCount <= 1) || (dp->rdaCount > 200)) {
        printf ("RDA option (%d) is invalid.\n", dp->rdaCount);
        return -1;
      }
    }
    else if (strcmp ("tbuf", argv[argIndex]) == 0) {
      dp->tdaCount = strtoul (argv[++argIndex], NULL, 0);
      if ((dp->tdaCount <= 1) || (dp->tdaCount > 200)) {
        printf ("TDA option (%d) is invalid.\n", dp->tdaCount);
        return -1;
      }
    }
    else if (strcmp ("ip", argv[argIndex]) == 0) {
      iface->addr = resolve (argv[++argIndex]);
    }
    else if (strcmp ("ether", argv[argIndex]) == 0) {
      gether (iface->hwaddr, argv[++argIndex]);
    }
    else if (strcmp ("reg", argv[argIndex]) == 0) {
      dp->sonic = (struct SonicRegisters *)strtoul (argv[++argIndex], NULL, 0);
    }
    else if (strcmp ("vector", argv[argIndex]) == 0) {
      dp->vector = strtoul (argv[++argIndex], NULL, 0);
    }
    else {
      printf ("Argument %d (%s) is invalid.\n", argIndex, argv[argIndex]);
      return -1;
    }
  }
  printf ("Ethernet address: %s\n", pether (cbuf, iface->hwaddr));
  iface->raw = sonic_raw;
  iface->stop = sonic_stop;
  iface->show = sonic_show;
  dp->iface = iface;
  setencap (iface, "Ethernet");

  /*
   * Set up SONIC hardware
   */
  sonic_initialize_hardware (dp, broadcastFlag);

  /*
   * Chain onto list of interfaces
   */
  iface->next = Ifaces;
  Ifaces = iface;

  /*
   * Start I/O daemons
   */
  cp = if_name (iface, " tx");
  iface->txproc = newproc (cp, 2048, if_tx, iface->dev, iface, NULL, 0);
  free (cp);
  cp = if_name (iface, " rx");
  iface->rxproc = newproc (cp, 2048, sonic_rx, iface->dev, iface, dp, 0);
  free (cp);
  return 0;
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

  value = p[regno];
#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Read 0x%04x from %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  return value;
}

