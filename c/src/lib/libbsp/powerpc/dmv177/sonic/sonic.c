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
#ifndef SONIC_DCR
# define SONIC_DCR (DCR_DW | DCR_TFT1 | DCR_TFT0)
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
  volatile struct SonicRegisters  *sonic;

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
static struct sonic sonic[NSONIC];

/*
 ******************************************************************
 *                                                                *
 *                        Support Routines                        *
 *                                                                *
 ******************************************************************
 */

/*
 * Allocate non-cacheable memory on a single 64k page.
 * Very simple minded -- just keeps trying till the memory is on a single page.
 */
static void *
sonic_allocate (unsigned int nbytes)
{
  void *p;
  unsigned long a1, a2;

  for (;;) {
    /*
     * ===CACHE===
     * Change malloc to malloc_noncacheable_guarded.
     */
    p = malloc (nbytes);
    if (p == NULL)
      rtems_panic ("No memory!");
    a1 = (unsigned long)p;
    a2 = a1 + nbytes - 1;
    if ((a1 >> 16) == (a2 >> 16))
      break;
  }
  return p;
}

/*
 * Shut down the interface.
 * This is a pretty simple-minded routine.  It doesn't worry
 * about cleaning up mbufs, shutting down daemons, etc.
 */
static int
sonic_stop (struct iface *iface)
{
  int i;
  struct sonic *dp = &sonic[iface->dev];
  volatile struct SonicRegisters *rp = dp->sonic;

  /*
   * Stop the transmitter and receiver.
   */
  rp->cr = CR_HTX | CR_RXDIS;

  /*
   * Wait for things to stop.
   * For safety's sake, there is an alternate exit.
   */
  i = 0;
  while (rp->cr & (CR_RXEN | CR_TXP)) {
    if (++i == 10000)
      break;
  }

  /*
   * Reset the device
   */
  rp->cr = CR_RST;
  rp->imr = 0;
  return 0;
}

/*
 * Show interface statistics
 */
static void
sonic_show (struct iface *iface)
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
static rtems_isr
sonic_interrupt_handler (rtems_vector_number v)
{
  struct sonic *dp = sonic;
  volatile struct SonicRegisters *rp;

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
  if ((rp->imr & (IMR_PRXEN | IMR_RBAEEN))
   && (rp->isr & (ISR_PKTRX | ISR_RBAE))) {
    rp->imr &= ~(IMR_PRXEN | IMR_RBAEEN);
    dp->rxInterrupts++;
    rtems_event_send (dp->iface->rxproc, INTERRUPT_EVENT);
  }

  /*
   * Packet started, transmitter done or transmitter error?
   */
  if ((rp->imr & (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN))
   && (rp->isr & (ISR_PINT | ISR_TXDN | ISR_TXER))) {
    rp->imr &= ~(IMR_PINTEN | IMR_PTXEN | IMR_TXEREN);
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
static void
sonic_retire_tda (struct sonic *dp)
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
        volatile struct SonicRegisters *rp = dp->sonic;

        rp->ctda = link;
        rp->cr = CR_TXP;
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
static int
sonic_raw (struct iface *iface, struct mbuf **bpp)
{
  struct sonic *dp = &sonic[iface->dev];
  volatile struct SonicRegisters *rp = dp->sonic;
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
    rp->isr = ISR_PINT | ISR_TXDN | ISR_TXER;

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
      rp->imr |= (IMR_PINTEN | IMR_PTXEN | IMR_TXEREN);

      /*
       * Wait for interrupt
       */
      rtems_ka9q_event_receive (INTERRUPT_EVENT,
            RTEMS_WAIT|RTEMS_EVENT_ANY,
            RTEMS_NO_TIMEOUT);
      rp->isr = ISR_PINT | ISR_TXDN | ISR_TXER;
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
  rp->cr = CR_TXP;
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
static void
sonic_rda_wait (struct sonic *dp, ReceiveDescriptorPointer_t rdp)
{
  int i;
  volatile struct SonicRegisters *rp = dp->sonic;

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
    if (rp->isr & ISR_RBAE) {
      /*
       * One more check to soak up any Receive Descriptors
       * that may already have been handed back to the driver.
       */
      if (rdp->in_use == 0)
        break;

      /*
       * Check my interpretation of the SONIC manual.
       */
      if (rp->cr & CR_RXEN)
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
        if (rp->rrp == rp->rsa)
          rp->rrp = rp->rea;
        rp->rrp -= sizeof (ReceiveResource_t);
      }

      /*
       * Restart reception
       */
      rp->isr = ISR_RBAE;
      rp->cr = CR_RXEN;
    }

    /*
     * Clear old packet-received events.
     */
    rp->isr = ISR_PKTRX;

    /*
     * Has Receive Descriptor become available?
     */
    if (rdp->in_use == 0)
      break;

    /*
     * Enable interrupts.
     */
    rp->imr |= IMR_PRXEN | IMR_RBAEEN;

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
static void
sonic_rx (int dev, void *p1, void *p2)
{
  struct iface *iface = (struct iface *)p1;
  struct sonic *dp = (struct sonic *)p2;
  volatile struct SonicRegisters *rp = dp->sonic;
  struct mbuf *bp;
  rtems_unsigned16 status;
  ReceiveDescriptor_t *rda;
  ReceiveDescriptorPointer_t ordp, rdp;
  ReceiveResourcePointer_t rwp, rea;
  rtems_unsigned16 newMissedTally, oldMissedTally;
  int i;
  int continuousCount;

  /*
   * Set up list in Receive Resource Area.
   * Allocate space for incoming packets.
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
   * Set up remaining Receive Resource Area pointers
   */
  rp->rsa = LSW(dp->rsa);
  rp->rrp = LSW(dp->rsa);
  rp->rea = LSW(rea);
  rp->rwp = LSW(rea);

  /*
   * Set End Of Buffer Count register to the value recommended
   * in Note 1 of Section 3.4.4.4 of the SONIC data sheet.
   */
  rp->eobc = RBUF_WC - 2;

  /*
   * Set up circular linked list in Receive Descriptor Area.
   * Leaves ordp pointing at the `end' of the list and
   * rdp pointing at the `beginning' of the list.
   */
  rda = sonic_allocate (dp->rdaCount * sizeof *rda);
  ordp = rdp = rda;
  for (i = 0 ; i < dp->rdaCount ; i++) {
    /*
     * Set up RDA entry
     */
    if (i == (dp->rdaCount - 1))
      rdp->next = rda;
    else
      rdp->next = (ReceiveDescriptor_t *)(rdp + 1);
    rdp->in_use = 1;
    ordp = rdp;
    rdp = rdp->next;
    ordp->link = LSW(rdp);
  }
  ordp->link |= RDA_LINK_EOL;
  rp->urda = MSW(rdp);
  rp->crda = LSW(rdp);

  /*
   * Start the receiver
   */
  oldMissedTally = rp->mpt;
  rp->cr = CR_RRRA;
  rp->cr = CR_RXEN;

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
      if ((LSW(p) != rwp->buff_ptr_lsw)
       || (MSW(p) != rwp->buff_ptr_msw))
        rtems_panic ("SONIC RDA/RRA");

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
      rp->rwp = LSW(rwp);

      /*
       * Tell the SONIC to reread the RRA.
       */
      if (rp->isr & ISR_RBE)
        rp->isr = ISR_RBE;
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
    newMissedTally = rp->mpt;
    if (newMissedTally != oldMissedTally) {
      dp->rxMissed += (newMissedTally - oldMissedTally) & 0xFFFF;
      newMissedTally = oldMissedTally;
    }

    /*
     * Move to next receive descriptor
     */
    rdp->link |= RDA_LINK_EOL;
    rdp->in_use = 1;
    ordp->link &= ~RDA_LINK_EOL;
    ordp = rdp;
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
static void
sonic_initialize_hardware (struct sonic *dp, int broadcastFlag)
{
  volatile struct SonicRegisters *rp = dp->sonic;
  int i;
  unsigned char *hwaddr;
  rtems_status_code sc;
  rtems_isr_entry old_handler;
  TransmitDescriptorPointer_t otdp, tdp;
  struct CamDescriptor{
    rtems_unsigned32  cep;
    rtems_unsigned32  cap0;
    rtems_unsigned32  cap1;
    rtems_unsigned32  cap2;
    rtems_unsigned32  ce;
  };
  volatile struct CamDescriptor *cdp;

  /*
   * Issue a software reset if necessary.
   */
  if ((rp->cr & CR_RST) == 0)
    rp->cr = CR_RST;

  /*
   * Set up data configuration registers.
   */
  rp->dcr = SONIC_DCR;
  rp->dcr2 = SONIC_DC2;

  /*
   * Remove device reset
   */
  rp->cr = 0;
  
  /*
   * Clear outstanding interrupts.
   */
  rp->isr = 0x7FFF;

  /*
   * Allocate the receive resource area.
   * In accordance with National Application Note 746, make the
   * receive resource area bigger than the receive descriptor area.
   * This has the useful side effect of making the receive resource
   * area big enough to hold the CAM descriptor area.
   */
  dp->rsa = sonic_allocate ((dp->rdaCount + RRA_EXTRA_COUNT) * sizeof *dp->rsa);
  rp->urra = MSW(dp->rsa);

  /*
   * Set up the SONIC CAM with our hardware address.
   * Use the Receive Resource Area to hold the CAM Descriptor Area.
   */
  hwaddr = dp->iface->hwaddr;
  cdp = (struct CamDescriptor *)dp->rsa;
  cdp->cep = 0;      /* Fill first entry in CAM */
  cdp->cap2 = hwaddr[0] << 8 | hwaddr[1];
  cdp->cap1 = hwaddr[2] << 8 | hwaddr[3];
  cdp->cap0 = hwaddr[4] << 8 | hwaddr[5];
  cdp->ce = 0x0001;    /* Enable first entry in CAM */
  rp->cdc = 1;      /* One entry in CDA */
  rp->cdp = LSW(cdp);
  rp->cr = CR_LCAM;  /* Load the CAM */
  while (rp->cr & CR_LCAM)
    continue;

  /*
   * Verify that CAM was properly loaded.
   */
  rp->cep = 0;      /* Select first entry in CAM */
  if ((rp->cap2 != cdp->cap2)
   || (rp->cap1 != cdp->cap1)
   || (rp->cap0 != cdp->cap0)
   || (rp->ce != cdp->ce)) {
    printf ("Failed to load Ethernet address into SONIC CAM.\n"
      "  Wrote %04x%04x%04x - %#x\n"
      "   Read %04x%04x%04x - %#x\n", 
        cdp->cap2, cdp->cap1, cdp->cap0, cdp->ce,
        rp->cap2, rp->cap1, rp->cap0, rp->ce);
    rtems_panic ("SONIC LCAM");
  }

  /*
   * Set up circular linked list in Transmit Descriptor Area.
   * Use the PINT bit in the transmit configuration field to
   * request an interrupt on every other transmitted packet.
   */
  dp->tdaActiveCount = 0;
  dp->tdaTail = sonic_allocate (dp->tdaCount * sizeof *tdp);
  otdp = tdp = dp->tdaTail;
  for (i = 0 ; i < dp->tdaCount ; i++) {
    if (i & 1)
      tdp->pkt_config = TDA_CONFIG_PINT;
    else
      tdp->pkt_config = 0;
    if (i == (dp->tdaCount - 1))
      tdp->next = (TransmitDescriptor_t *)dp->tdaTail;
    else
      tdp->next = (TransmitDescriptor_t *)(tdp + 1);
    otdp = tdp;
    tdp = tdp->next;
  }
  dp->tdaHead = otdp;
  dp->tdaHead->linkp = &dp->tdaHead->frag[0].frag_link;
  rp->utda = MSW(dp->tdaTail);
  rp->ctda = LSW(dp->tdaTail);

  /*
   * Enable/disable reception of broadcast packets
   */
  if (broadcastFlag)
    rp->rcr = RCR_BRD;
  else
    rp->rcr = 0;

  /*
   * Attach SONIC interrupt handler
   */
  rp->imr = 0;
  sc = rtems_interrupt_catch (sonic_interrupt_handler, dp->vector, &old_handler);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't attach SONIC interrupt handler: %s\n",
              rtems_status_text (sc));

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
