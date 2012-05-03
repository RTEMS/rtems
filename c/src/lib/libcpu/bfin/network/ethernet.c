/*
 *  RTEMS network driver for Blackfin ethernet controller
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems/cache.h>

#include <stdio.h>
#include <inttypes.h>
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

#include <libcpu/dmaRegs.h>
#include <libcpu/ethernetRegs.h>
#include "ethernet.h"

#if (BFIN_ETHERNET_DEBUG & BFIN_ETHERNET_DEBUG_DUMP_MBUFS)
#include <rtems/dumpbuf.h>
#endif

/*
 * Number of devices supported by this driver
 */
#ifndef N_BFIN_ETHERNET
# define N_BFIN_ETHERNET 1
#endif


/* #define BFIN_IPCHECKSUMS */


/*
 * RTEMS event used by interrupt handler to signal daemons.
 */
#define INTERRUPT_EVENT  RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2


/* largest Ethernet frame MAC will handle */
#define BFIN_ETHERNET_MAX_FRAME_LENGTH      1556

#if MCLBYTES < (BFIN_ETHERNET_MAX_FRAME_LENGTH + 2)
#error MCLBYTES too small
#endif

#define BFIN_REG16(base, offset) \
        (*((uint16_t volatile *) ((char *)(base) + (offset))))
#define BFIN_REG32(base, offset) \
        (*((uint32_t volatile *) ((char *)(base) + (offset))))


#define DMA_MODE_RX               (DMA_CONFIG_FLOW_DESC_LARGE | \
                                   (5 << DMA_CONFIG_NDSIZE_SHIFT) | \
                                   DMA_CONFIG_WDSIZE_32 | \
                                   DMA_CONFIG_WNR | \
                                   DMA_CONFIG_DMAEN)

#define DMA_MODE_TX               (DMA_CONFIG_FLOW_DESC_LARGE | \
                                   (5 << DMA_CONFIG_NDSIZE_SHIFT) | \
                                   DMA_CONFIG_WDSIZE_32 | \
                                   DMA_CONFIG_DMAEN)

#define DMA_MODE_STATUS           (DMA_CONFIG_FLOW_DESC_LARGE | \
                                   (5 << DMA_CONFIG_NDSIZE_SHIFT) | \
                                   DMA_CONFIG_DI_EN | \
                                   DMA_CONFIG_WDSIZE_32 | \
                                   DMA_CONFIG_WNR | \
                                   DMA_CONFIG_DMAEN)

#define DMA_MODE_STATUS_NO_INT    (DMA_CONFIG_FLOW_DESC_LARGE | \
                                   (5 << DMA_CONFIG_NDSIZE_SHIFT) | \
                                   DMA_CONFIG_WDSIZE_32 | \
                                   DMA_CONFIG_WNR | \
                                   DMA_CONFIG_DMAEN)

#define DMA_MODE_STATUS_LAST      (DMA_CONFIG_FLOW_STOP | \
                                   (0 << DMA_CONFIG_NDSIZE_SHIFT) | \
                                   DMA_CONFIG_DI_EN | \
                                   DMA_CONFIG_WDSIZE_32 | \
                                   DMA_CONFIG_WNR | \
                                   DMA_CONFIG_DMAEN)

/* five 16 bit words */
typedef struct dmaDescS {
  struct dmaDescS *next;
  void *addr;
  uint16_t dmaConfig;
} dmaDescT;

typedef struct {
  uint32_t status;
} txStatusT;

#ifdef BFIN_IPCHECKSUMS
typedef struct {
  uint16_t ipHeaderChecksum;
  uint16_t ipPayloadChecksum;
  uint32_t status;
} rxStatusT;
#else
typedef struct {
  uint32_t status;
} rxStatusT;
#endif

typedef struct {
  dmaDescT data;
  dmaDescT status;
  struct mbuf *m;
} rxPacketDescT;

typedef struct {
  dmaDescT data;
  dmaDescT status;
  bool     inUse;
  union {
    uint32_t dummy; /* try to force 32 bit alignment */
    struct {
      uint16_t length;
      char data[BFIN_ETHERNET_MAX_FRAME_LENGTH];
    } packet;
  } buffer;
} txPacketDescT;


/* hardware-specific storage */
struct bfin_ethernetSoftc {
  struct arpcom arpcom; /* this entry must be first */

  uint32_t sclk;

  void *ethBase;
  void *rxdmaBase;
  void *txdmaBase;

  int acceptBroadcast;

  rtems_id rxDaemonTid;
  rtems_id txDaemonTid;

  void *status;
  int rxDescCount;
  rxPacketDescT *rx;
  int txDescCount;
  txPacketDescT *tx;

  bool rmii;
  int phyAddr;

  /* statistics */
#ifdef BISON
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
#endif
};

static struct bfin_ethernetSoftc ethernetSoftc[N_BFIN_ETHERNET];


/* Shut down the interface.  */
static void ethernetStop(struct bfin_ethernetSoftc *sc) {
  struct ifnet *ifp;
  void *ethBase;

  ifp = &sc->arpcom.ac_if;
  ethBase = sc->ethBase;

  ifp->if_flags &= ~IFF_RUNNING;

  /* stop the transmitter and receiver.  */
  BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) &= ~(EMAC_OPMODE_TE |
                                               EMAC_OPMODE_RE);
}

/* Show interface statistics */
static void bfin_ethernetStats(struct bfin_ethernetSoftc *sc) {
#ifdef BISON
  printf(" Total Interrupts:%-8lu", sc->Interrupts);
  printf("    Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf("            Giant:%-8lu", sc->rxGiant);
  printf("        Non-octet:%-8lu\n", sc->rxNonOctet);
  printf("          Bad CRC:%-8lu", sc->rxBadCRC);
  printf("        Collision:%-8lu", sc->rxCollision);
  printf("           Missed:%-8lu\n", sc->rxMissed);

  printf(    "    Tx Interrupts:%-8lu", sc->txInterrupts);
  printf(  "           Deferred:%-8lu", sc->txDeferred);
  printf("        Lost Carrier:%-8lu\n", sc->txLostCarrier);
  printf(   "Single Collisions:%-8lu", sc->txSingleCollision);
  printf( "Multiple Collisions:%-8lu", sc->txMultipleCollision);
  printf("Excessive Collisions:%-8lu\n", sc->txExcessiveCollision);
  printf(   " Total Collisions:%-8lu", sc->txCollision);
  printf( "     Late Collision:%-8lu", sc->txLateCollision);
  printf("            Underrun:%-8lu\n", sc->txUnderrun);
  printf(   "  Raw output wait:%-8lu\n", sc->txRawWait);
#endif /*BISON*/
}

void bfin_ethernet_rxdma_isr(int vector) {
  struct bfin_ethernetSoftc *sc;
  void *rxdmaBase;
  uint16_t status;
  int i;

  for (i = 0; i < N_BFIN_ETHERNET; i++) {
    sc = &ethernetSoftc[i];
    rxdmaBase = sc->rxdmaBase;
    status = BFIN_REG16(rxdmaBase, DMA_IRQ_STATUS_OFFSET);
    if (status & DMA_IRQ_STATUS_DMA_DONE)
        rtems_event_send (sc->rxDaemonTid, INTERRUPT_EVENT);
    BFIN_REG16(rxdmaBase, DMA_IRQ_STATUS_OFFSET) = status;
  }
}

void bfin_ethernet_txdma_isr(int vector) {
  struct bfin_ethernetSoftc *sc;
  void *txdmaBase;
  uint16_t status;
  int i;

  for (i = 0; i < N_BFIN_ETHERNET; i++) {
    sc = &ethernetSoftc[i];
    txdmaBase = sc->txdmaBase;
    status = BFIN_REG16(txdmaBase, DMA_IRQ_STATUS_OFFSET);
    if (status & DMA_IRQ_STATUS_DMA_DONE)
        rtems_event_send (sc->txDaemonTid, INTERRUPT_EVENT);
    BFIN_REG16(txdmaBase, DMA_IRQ_STATUS_OFFSET) = status;
  }
}

void bfin_ethernet_mac_isr(int vector) {
  struct bfin_ethernetSoftc *sc;
  void *ethBase;
  int i;

  for (i = 0; i < N_BFIN_ETHERNET; i++) {
    sc = &ethernetSoftc[i];
    ethBase = sc->ethBase;
    BFIN_REG32(ethBase, EMAC_SYSTAT_OFFSET) = ~(uint32_t) 0;
  }
}

static bool txFree(struct bfin_ethernetSoftc *sc, int index) {
  bool       freed;
  txStatusT *status;

  freed = false;
  if (sc->tx[index].inUse) {
    status = (txStatusT *) sc->tx[index].status.addr;
    rtems_cache_invalidate_multiple_data_lines(status, sizeof(*status));
    if (status->status != 0) {
      /* update statistics */

      sc->tx[index].inUse = false;
      freed = true;
    }
  }

  return freed;
}

static void txDaemon(void *arg) {
  struct bfin_ethernetSoftc *sc;
  struct ifnet *ifp;
  struct mbuf *m, *first;
  rtems_event_set events;
  void *ethBase;
  void *txdmaBase;
  txStatusT *status;
  int head;
  int prevHead;
  int tail;
  int length;
  char *ptr;

  sc = (struct bfin_ethernetSoftc *) arg;
  ifp = &sc->arpcom.ac_if;

  ethBase = sc->ethBase;
  txdmaBase = sc->txdmaBase;
  head = 0;
  prevHead = sc->txDescCount - 1;
  tail = 0;

  while (1) {
    /* wait for packet or isr */
    rtems_bsdnet_event_receive(START_TRANSMIT_EVENT | INTERRUPT_EVENT,
                               RTEMS_EVENT_ANY | RTEMS_WAIT,
                               RTEMS_NO_TIMEOUT, &events);

    /* if no descriptors are available, try to free one.  To reduce
       transmit latency only do one here. */
    if (sc->tx[head].inUse && txFree(sc, tail)) {
      if (++tail == sc->txDescCount)
        tail = 0;
    }
    /* send packets until the queue is empty or we run out of tx
       descriptors */
    while (!sc->tx[head].inUse && (ifp->if_flags & IFF_OACTIVE)) {
      /* get the next mbuf chain to transmit */
      IF_DEQUEUE(&ifp->if_snd, m);
      if (m != NULL) {
        /* copy packet into our buffer */
        ptr = sc->tx[head].buffer.packet.data;
        length = 0;
        first = m;
        while (m && length <= BFIN_ETHERNET_MAX_FRAME_LENGTH) {
          length += m->m_len;
          if (length <= BFIN_ETHERNET_MAX_FRAME_LENGTH)
            memcpy(ptr, m->m_data, m->m_len);
          ptr += m->m_len;
          m = m->m_next;
        }
        m_freem(first); /* all done with mbuf */
        if (length <= BFIN_ETHERNET_MAX_FRAME_LENGTH) {
          sc->tx[head].buffer.packet.length = length;

          /* setup tx dma */
          status = (txStatusT *) sc->tx[head].status.addr;
          status->status = 0;
          sc->tx[head].inUse = true;
          rtems_cache_flush_multiple_data_lines(status, sizeof(*status));

          /* configure dma to stop after sending this packet */
          sc->tx[head].status.dmaConfig = DMA_MODE_STATUS_LAST;
          rtems_cache_flush_multiple_data_lines(
              &sc->tx[head].status.dmaConfig,
              sizeof(sc->tx[head].status.dmaConfig));
          rtems_cache_flush_multiple_data_lines(
              &sc->tx[head].buffer.packet,
              length + sizeof(uint16_t));

          /* modify previous descriptor to let it continue
             automatically */
          sc->tx[prevHead].status.dmaConfig = DMA_MODE_STATUS;
          rtems_cache_flush_multiple_data_lines(
              &sc->tx[prevHead].status.dmaConfig,
              sizeof(sc->tx[prevHead].status.dmaConfig));

          /* restart dma if it stopped before the packet we just
             added.  this is purely to reduce transmit latency,
             as it would be restarted anyway after this loop (and
             needs to be, as there's a very small chance that the
             dma controller had started the last status transfer
             before the new dmaConfig word was written above and
             is still doing that status transfer when we check the
             status below.  this will be caught by the check
             outside the loop as that is guaranteed to run at least
             once after the last dma complete interrupt. */
          if ((BFIN_REG16(txdmaBase, DMA_IRQ_STATUS_OFFSET) &
               DMA_IRQ_STATUS_DMA_RUN) == 0 &&
               BFIN_REG32(txdmaBase, DMA_NEXT_DESC_PTR_OFFSET) !=
               (uint32_t) sc->tx[head].data.next) {
            BFIN_REG16(txdmaBase, DMA_CONFIG_OFFSET) = DMA_MODE_TX;
            BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) |= EMAC_OPMODE_TE;
          }

          if (++head == sc->txDescCount)
            head = 0;
          if (++prevHead == sc->txDescCount)
            prevHead = 0;

          /* if no descriptors are available, try to free one */
          if (sc->tx[head].inUse && txFree(sc, tail)) {
            if (++tail == sc->txDescCount)
              tail = 0;
          }
        } else {
          /* dropping packet: too large */

        }
      } else {
        /* no packets queued */
        ifp->if_flags &= ~IFF_OACTIVE;
      }
    }

    /* if dma stopped and there's more to do, restart it */
    if ((BFIN_REG16(txdmaBase, DMA_IRQ_STATUS_OFFSET) &
         DMA_IRQ_STATUS_DMA_RUN) == 0 &&
        BFIN_REG32(txdmaBase, DMA_NEXT_DESC_PTR_OFFSET) !=
        (uint32_t) &sc->tx[head].data) {
      BFIN_REG16(txdmaBase, DMA_CONFIG_OFFSET) = DMA_MODE_TX;
      BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) |= EMAC_OPMODE_TE;
    }

    /* free up any additional tx descriptors */
    while (txFree(sc, tail)) {
      if (++tail == sc->txDescCount)
        tail = 0;
    }
  }
}


static void rxDaemon(void *arg) {
  struct bfin_ethernetSoftc *sc;
  struct ifnet *ifp;
  struct mbuf *m;
  struct mbuf *rxPacket;
  void *dataPtr;
  rtems_event_set events;
  struct ether_header *eh;
  rxStatusT *status;
  uint32_t rxStatus;
  int head;
  int prevHead;
  int length;
  void *ethBase;
  void *rxdmaBase;

  sc = (struct bfin_ethernetSoftc *) arg;
  rxdmaBase = sc->rxdmaBase;
  ethBase = sc->ethBase;
  ifp = &sc->arpcom.ac_if;
  prevHead = sc->rxDescCount - 1;
  head = 0;

  BFIN_REG16(rxdmaBase, DMA_CONFIG_OFFSET) = DMA_MODE_RX;
  BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) |= EMAC_OPMODE_RE;

  while (1) {
    status = sc->rx[head].status.addr;
    rtems_cache_invalidate_multiple_data_lines(status, sizeof(*status));
    while (status->status != 0) {
      if (status->status & EMAC_RX_STAT_RX_OK) {
        /* get new cluster to replace this one */
        MGETHDR(m, M_WAIT, MT_DATA);
        MCLGET(m, M_WAIT);
        m->m_pkthdr.rcvif = ifp;
      } else
        m = NULL;

      rxStatus = status->status;
      /* update statistics */


      if (m) {
        /* save received packet to send up a little later */
        rxPacket = sc->rx[head].m;
        dataPtr = sc->rx[head].data.addr;

        /* setup dma for new cluster */
        sc->rx[head].m = m;
        sc->rx[head].data.addr = (void *) (((intptr_t) m->m_data + 3) & ~3);
        /* invalidate cache for new data buffer, in case any lines
           are dirty from previous owner */
        rtems_cache_invalidate_multiple_data_lines(
            sc->rx[head].data.addr,
            BFIN_ETHERNET_MAX_FRAME_LENGTH + 2);
      } else
        rxPacket = NULL;

      sc->rx[head].status.dmaConfig = DMA_MODE_STATUS_LAST;
      rtems_cache_flush_multiple_data_lines(&sc->rx[head],
                                            sizeof(sc->rx[head]));

      /* mark descriptor as empty */
      status->status = 0;
      rtems_cache_flush_multiple_data_lines(&status->status,
                                            sizeof(status->status));

      /* allow dma to continue from previous descriptor into this
         one */
      sc->rx[prevHead].status.dmaConfig = DMA_MODE_STATUS;
      rtems_cache_flush_multiple_data_lines(
          &sc->rx[prevHead].status.dmaConfig,
          sizeof(sc->rx[prevHead].status.dmaConfig));

      if (rxPacket) {
        /* send it up */
        eh = (struct ether_header *) ((intptr_t) dataPtr + 2);
        rxPacket->m_data = (caddr_t) ((intptr_t) dataPtr + 2 + 14);
        length = (rxStatus & EMAC_RX_STAT_RX_FRLEN_MASK) >>
                  EMAC_RX_STAT_RX_FRLEN_SHIFT;
        rxPacket->m_len = length - 14;
        rxPacket->m_pkthdr.len = rxPacket->m_len;
        /* invalidate packet buffer cache again (even though it
           was invalidated prior to giving it to dma engine),
           because speculative reads might cause cache lines to
           be filled at any time */
        rtems_cache_invalidate_multiple_data_lines(eh, length);
        ether_input(ifp, eh, rxPacket);
      }

      if (++prevHead == sc->rxDescCount)
        prevHead = 0;
      if (++head == sc->rxDescCount)
        head = 0;
      status = sc->rx[head].status.addr;
      rtems_cache_invalidate_multiple_data_lines(status, sizeof(*status));
    }

    /* if dma stopped before the next descriptor, restart it */
    if ((BFIN_REG16(rxdmaBase, DMA_IRQ_STATUS_OFFSET) &
         DMA_IRQ_STATUS_DMA_RUN) == 0 &&
        BFIN_REG32(rxdmaBase, DMA_NEXT_DESC_PTR_OFFSET) !=
        (uint32_t) &sc->rx[head].data) {
      BFIN_REG16(rxdmaBase, DMA_CONFIG_OFFSET) = DMA_MODE_RX;
    }

    rtems_bsdnet_event_receive(INTERRUPT_EVENT, RTEMS_WAIT | RTEMS_EVENT_ANY,
                               RTEMS_NO_TIMEOUT, &events);
  }

}

/*
 ******************************************************************
 *                                                                *
 *                     Initialization Routines                    *
 *                                                                *
 ******************************************************************
 */

static void resetHardware(struct bfin_ethernetSoftc *sc) {
  void *ethBase;
  void *rxdmaBase;
  void *txdmaBase;

  ethBase = sc->ethBase;
  rxdmaBase = sc->rxdmaBase;
  txdmaBase = sc->txdmaBase;
  BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) = 0;
  BFIN_REG16(rxdmaBase, DMA_CONFIG_OFFSET) = 0;
  BFIN_REG16(txdmaBase, DMA_CONFIG_OFFSET) = 0;
}

static void initializeHardware(struct bfin_ethernetSoftc *sc) {
  struct ifnet *ifp;
  struct mbuf *m;
  unsigned char *hwaddr;
  int cacheAlignment;
  int rxStatusSize;
  int txStatusSize;
  char *ptr;
  int i;
  void *ethBase;
  void *rxdmaBase;
  void *txdmaBase;
  uint32_t divisor;

  ifp = &sc->arpcom.ac_if;
  ethBase = sc->ethBase;
  rxdmaBase = sc->rxdmaBase;
  txdmaBase = sc->txdmaBase;

  BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) = 0;
  BFIN_REG32(ethBase, EMAC_FLC_OFFSET) = 0;
  divisor = (sc->sclk / 25000000) / 2 - 1;
  BFIN_REG32(ethBase, EMAC_SYSCTL_OFFSET) = (divisor <<
                                             EMAC_SYSCTL_MDCDIV_SHIFT) |
                                            EMAC_SYSCTL_RXDWA;
#ifdef BFIN_IPCHECKSUMS
  BFIN_REG32(ethBase, EMAC_SYSCTL_OFFSET) |= EMAC_SYSCTL_RXCKS;
#endif
  BFIN_REG32(ethBase, EMAC_SYSTAT_OFFSET) = ~(uint32_t) 0;
  BFIN_REG32(ethBase, EMAC_RX_IRQE_OFFSET) = 0;
  BFIN_REG32(ethBase, EMAC_RX_STKY_OFFSET) = ~(uint32_t) 0;
  BFIN_REG32(ethBase, EMAC_TX_IRQE_OFFSET) = 0;
  BFIN_REG32(ethBase, EMAC_TX_STKY_OFFSET) = ~(uint32_t) 0;
  BFIN_REG32(ethBase, EMAC_MMC_RIRQE_OFFSET) = 0;
  BFIN_REG32(ethBase, EMAC_MMC_RIRQS_OFFSET) = ~(uint32_t) 0;
  BFIN_REG32(ethBase, EMAC_MMC_TIRQE_OFFSET) = 0;
  BFIN_REG32(ethBase, EMAC_MMC_TIRQS_OFFSET) = ~(uint32_t) 0;
  BFIN_REG32(ethBase, EMAC_MMC_CTL_OFFSET) = EMAC_MMC_CTL_MMCE |
                                             EMAC_MMC_CTL_CCOR |
                                             EMAC_MMC_CTL_RSTC;
  BFIN_REG32(ethBase, EMAC_MMC_CTL_OFFSET) = EMAC_MMC_CTL_MMCE |
                                             EMAC_MMC_CTL_CCOR;

  BFIN_REG16(rxdmaBase, DMA_CONFIG_OFFSET) = 0;
  BFIN_REG16(txdmaBase, DMA_CONFIG_OFFSET) = 0;
  BFIN_REG16(rxdmaBase, DMA_X_COUNT_OFFSET) = 0;
  BFIN_REG16(txdmaBase, DMA_X_COUNT_OFFSET) = 0;
  BFIN_REG16(rxdmaBase, DMA_X_MODIFY_OFFSET) = 4;
  BFIN_REG16(txdmaBase, DMA_X_MODIFY_OFFSET) = 4;
  BFIN_REG16(rxdmaBase, DMA_Y_COUNT_OFFSET) = 0;
  BFIN_REG16(txdmaBase, DMA_Y_COUNT_OFFSET) = 0;
  BFIN_REG16(rxdmaBase, DMA_Y_MODIFY_OFFSET) = 0;
  BFIN_REG16(txdmaBase, DMA_Y_MODIFY_OFFSET) = 0;
  BFIN_REG16(rxdmaBase, DMA_IRQ_STATUS_OFFSET) = DMA_IRQ_STATUS_DMA_ERR |
                                                 DMA_IRQ_STATUS_DMA_DONE;

  /* The status structures cannot share cache lines with anything else,
     including other status structures, so we can safely manage both the
     processor and DMA writing to them.  So this rounds up the structure
     sizes to a multiple of the cache line size. */
  cacheAlignment = rtems_cache_get_data_line_size();
  if (cacheAlignment == 0)
     cacheAlignment = 1;
  rxStatusSize = cacheAlignment * ((sizeof(rxStatusT) + cacheAlignment - 1) /
                                   cacheAlignment);
  txStatusSize = cacheAlignment * ((sizeof(txStatusT) + cacheAlignment - 1) /
                                   cacheAlignment);
  /* Allocate enough extra to allow structures to start at cache aligned
     boundary. */
  sc->status = malloc(sc->rxDescCount * rxStatusSize +
                      sc->txDescCount * txStatusSize +
                      cacheAlignment - 1, M_DEVBUF, M_NOWAIT);
  sc->rx = malloc(sc->rxDescCount * sizeof(*sc->rx), M_DEVBUF, M_NOWAIT);
  sc->tx = malloc(sc->txDescCount * sizeof(*sc->tx), M_DEVBUF, M_NOWAIT);
  if (sc->status == NULL || sc->rx == NULL || sc->tx == NULL)
    rtems_panic("No memory!\n");

  /* Start status structures at cache aligned boundary. */
  ptr = (char *) (((intptr_t) sc->status + cacheAlignment - 1) &
                  ~(cacheAlignment - 1));
  memset(ptr, 0, sc->rxDescCount * rxStatusSize +
                 sc->txDescCount * txStatusSize);
  memset(sc->rx, 0, sc->rxDescCount * sizeof(*sc->rx));
  memset(sc->tx, 0, sc->txDescCount * sizeof(*sc->tx));
  rtems_cache_flush_multiple_data_lines(ptr, sc->rxDescCount * rxStatusSize +
                                             sc->txDescCount * txStatusSize);
  for (i = 0; i < sc->rxDescCount; i++) {
    MGETHDR(m, M_WAIT, MT_DATA);
    MCLGET(m, M_WAIT);
    m->m_pkthdr.rcvif = ifp;
    sc->rx[i].m = m;
    /* start dma at 32 bit boundary */
    sc->rx[i].data.addr = (void *) (((intptr_t) m->m_data + 3) & ~3);
    rtems_cache_invalidate_multiple_data_lines(
        sc->rx[i].data.addr,
        BFIN_ETHERNET_MAX_FRAME_LENGTH + 2);
    sc->rx[i].data.dmaConfig = DMA_MODE_RX;
    sc->rx[i].data.next = &(sc->rx[i].status);
    sc->rx[i].status.addr = ptr;
    if (i < sc->rxDescCount - 1) {
      sc->rx[i].status.dmaConfig = DMA_MODE_STATUS;
      sc->rx[i].status.next = &(sc->rx[i + 1].data);
    } else {
      sc->rx[i].status.dmaConfig = DMA_MODE_STATUS_LAST;
      sc->rx[i].status.next = &(sc->rx[0].data);
    }
    ptr += rxStatusSize;
  }
  rtems_cache_flush_multiple_data_lines(sc->rx, sc->rxDescCount *
                                                sizeof(*sc->rx));
  for (i = 0; i < sc->txDescCount; i++) {
    sc->tx[i].data.addr = &sc->tx[i].buffer.packet;
    sc->tx[i].data.dmaConfig = DMA_MODE_TX;
    sc->tx[i].data.next = &(sc->tx[i].status);
    sc->tx[i].status.addr = ptr;
    sc->tx[i].status.dmaConfig = DMA_MODE_STATUS_LAST;
    if (i < sc->txDescCount - 1)
      sc->tx[i].status.next = &(sc->tx[i + 1].data);
    else
      sc->tx[i].status.next = &(sc->tx[0].data);
    sc->tx[i].inUse = false;
    ptr += txStatusSize;
  }
  rtems_cache_flush_multiple_data_lines(sc->tx, sc->txDescCount *
                                                sizeof(*sc->tx));

  BFIN_REG32(rxdmaBase, DMA_NEXT_DESC_PTR_OFFSET) = (uint32_t) &sc->rx[0].data;
  BFIN_REG32(txdmaBase, DMA_NEXT_DESC_PTR_OFFSET) = (uint32_t) &sc->tx[0].data;

  hwaddr = sc->arpcom.ac_enaddr;
  BFIN_REG16(ethBase, EMAC_ADDRHI_OFFSET) = ((uint16_t) hwaddr[5] << 8) |
                                            hwaddr[4];
  BFIN_REG32(ethBase, EMAC_ADDRLO_OFFSET) = ((uint32_t) hwaddr[3] << 24) |
                                            ((uint32_t) hwaddr[2] << 16) |
                                            ((uint32_t) hwaddr[1] << 8) |
                                            hwaddr[0];

  if (sc->acceptBroadcast)
    BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) &= ~EMAC_OPMODE_DBF;
  else
    BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) |= EMAC_OPMODE_DBF;

}

/* send packet (caller provides header) */
static void ethernetStart(struct ifnet *ifp) {
  struct bfin_ethernetSoftc *sc;

  sc = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;
  rtems_event_send(sc->txDaemonTid, START_TRANSMIT_EVENT);
}

/* initialize and start the device */
static void ethernetInit(void *arg) {
  struct bfin_ethernetSoftc *sc;
  struct ifnet *ifp;
  void *ethBase;
  void *rxdmaBase;
  void *txdmaBase;

  sc = arg;
  ifp = &sc->arpcom.ac_if;
  ethBase = sc->ethBase;
  rxdmaBase = sc->rxdmaBase;
  txdmaBase = sc->txdmaBase;

  if (sc->txDaemonTid == 0) {
    initializeHardware(sc);

    /* start driver tasks */
    sc->rxDaemonTid = rtems_bsdnet_newproc("BFrx", 4096, rxDaemon, sc);
    sc->txDaemonTid = rtems_bsdnet_newproc("BFtx", 4096, txDaemon, sc);

  }

  if (ifp->if_flags & IFF_PROMISC)
    BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) |= EMAC_OPMODE_PR;
  else
    BFIN_REG32(ethBase, EMAC_OPMODE_OFFSET) &= ~EMAC_OPMODE_PR;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

}

/* driver ioctl handler */
static int ethernetIoctl(struct ifnet *ifp, ioctl_command_t command,
                         caddr_t data) {
  int result;
  struct bfin_ethernetSoftc *sc = ifp->if_softc;

  result = 0;
  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    ether_ioctl(ifp, command, data);
    break;
  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      ethernetStop(sc);
      break;
    case IFF_UP:
      ethernetInit(sc);
      break;
    case IFF_UP | IFF_RUNNING:
      ethernetStop(sc);
      ethernetInit(sc);
      break;
    default:
      break;
    }
    break;
  case SIO_RTEMS_SHOW_STATS:
    bfin_ethernetStats(sc);
    break;
  case SIOCADDMULTI:
  case SIOCDELMULTI:
  default:
    result = EINVAL;
    break;
  }

  return result;
}

/* attach a BFIN ETHERNET driver to the system */
int bfin_ethernet_driver_attach(struct rtems_bsdnet_ifconfig *config,
                                int attaching,
                                bfin_ethernet_configuration_t *chip) {
  struct bfin_ethernetSoftc *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;

  if ((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0)
    return 0;

  if ((unitNumber <= 0) || (unitNumber > N_BFIN_ETHERNET)) {
    printf("Bad bfin ethernet unit number %d.\n", unitNumber);
    return 0;
  }
  sc = &ethernetSoftc[unitNumber - 1];
  ifp = &sc->arpcom.ac_if;
  if (ifp->if_softc != NULL) {
    printf("Driver already in use.\n");
    return 0;
  }

  memset(sc, 0, sizeof(*sc));

  /* process options */
  if (config->hardware_address)
    memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  else
    memset(sc->arpcom.ac_enaddr, 0x08, ETHER_ADDR_LEN);
  if (config->mtu)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;
  if (config->rbuf_count)
    sc->rxDescCount = config->rbuf_count;
  else
    sc->rxDescCount = chip->rxDescCount;
  if (config->xbuf_count)
    sc->txDescCount = config->xbuf_count;
  else
    sc->txDescCount = chip->txDescCount;
  /* minimum two of each type descriptor */
  if (sc->rxDescCount <= 1)
    sc->rxDescCount = 2;
  if (sc->txDescCount <= 1)
    sc->txDescCount = 2;

  sc->acceptBroadcast = !config->ignore_broadcast;

  sc->sclk = chip->sclk;
  sc->ethBase = chip->ethBaseAddress;
  sc->rxdmaBase = chip->rxdmaBaseAddress;
  sc->txdmaBase = chip->txdmaBaseAddress;

  /* make sure we should not have any interrupts asserted */
  resetHardware(sc);

  sc->rmii = (chip->phyType == rmii);
  sc->phyAddr = chip->phyAddr;

  /* set up network interface values */
  ifp->if_softc = sc;
  ifp->if_unit = unitNumber;
  ifp->if_name = unitName;
  ifp->if_mtu = mtu;
  ifp->if_init = ethernetInit;
  ifp->if_ioctl = ethernetIoctl;
  ifp->if_start = ethernetStart;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  if_attach(ifp);
  ether_ifattach(ifp);

  return 1;
}

