/*
 *  RTEMS driver for Minimac ethernet IP-core of Milkymist SoC
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis, France
 */


#include <bsp.h>
#include "../include/system_conf.h"
#include "bspopts.h"
#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <rtems.h>
#include "network.h"

unsigned int mm_ether_crc32(const unsigned char *buffer, unsigned int len);
static char rxbuff0[ETHERNET_FRAME_LENGTH] __attribute__((aligned (4)));
static char rxbuff1[ETHERNET_FRAME_LENGTH] __attribute__((aligned (4)));
static char rxbuff2[ETHERNET_FRAME_LENGTH] __attribute__((aligned (4)));
static char rxbuff3[ETHERNET_FRAME_LENGTH] __attribute__((aligned (4)));

static char *rxbuffs[4] = {rxbuff0, rxbuff1, rxbuff2, rxbuff3};

static struct minimac_softc minimac_softc;

static uint32_t rx_slot_state[4] = {MM_MINIMAC_STATE0, MM_MINIMAC_STATE1,
                            MM_MINIMAC_STATE2, MM_MINIMAC_STATE3};

static uint32_t rx_slot_addr[4] = {MM_MINIMAC_ADDR0, MM_MINIMAC_ADDR1,
                            MM_MINIMAC_ADDR2, MM_MINIMAC_ADDR3};

static uint32_t rx_slot_count[4] = {MM_MINIMAC_COUNT0, MM_MINIMAC_COUNT1,
                            MM_MINIMAC_COUNT2, MM_MINIMAC_COUNT3};
#ifdef CPU_U32_FIX

/*
 * Routine to align the received packet so that the ip header
 * is on a 32-bit boundary. Necessary for cpu's that do not
 * allow unaligned loads and stores and when the 32-bit DMA
 * mode is used.
 *
 * Transfers are done on word basis to avoid possibly slow byte
 * and half-word writes.
 *
 * Copied over from sonic.c driver
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

static inline int minimac_read(unsigned int reg)
{
  return *((int*)(reg));
}

static inline void minimac_write(unsigned int reg, int value)
{
  *((int*)reg) = value;
}

int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *config, 
  int attaching)
{
  struct minimac_softc *sc;
  struct ifnet *ifp;
  

  if (!attaching) {
    printk ("MINIMAC driver cannot be detached.\n");
    return 0;
  }

  sc = &minimac_softc;
  ifp = &(sc->arpcom.ac_if);

  if (sc->registered) {
    printk ("Driver already in use.\n");
    return 0;
  }

  sc->registered = 1;

  /*
   *  Mac address of Milkymist One board is 1 by default
   */

  sc->arpcom.ac_enaddr[0] = 0x00;
  sc->arpcom.ac_enaddr[1] = 0x23;
  sc->arpcom.ac_enaddr[2] = 0x8b;
  sc->arpcom.ac_enaddr[3] = 0x47;
  sc->arpcom.ac_enaddr[4] = 0x86;
  sc->arpcom.ac_enaddr[5] = 0x20;
  ifp->if_softc = sc;
  ifp->if_mtu = ETHERMTU;
  ifp->if_unit = 0;
  ifp->if_name = "minimac";
  ifp->if_init = minimac_init;
  ifp->if_ioctl = minimac_ioctl;
  ifp->if_start = minimac_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;

  if_attach (ifp);
  ether_ifattach(ifp);
  printk("[minimac] Ethernet driver attached\n");
  return 1;
}

static void minimac_start(struct ifnet *ifp)
{
  struct minimac_softc *sc = ifp->if_softc;
  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
//  printk("[minimac] start();\n");
}

/*
 *  Initialize and start the device
 */
static void minimac_init (void *arg)
{
  struct minimac_softc *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  unsigned char j;
  if (sc->txDaemonTid == 0) {
    sc->txDaemonTid = rtems_bsdnet_newproc ("MINIMACtx", 4096, minimac_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("MINIMACrx", 4096, minimac_rxDaemon, sc);
    set_vector(minimac_rx_interrupt_handler, IRQ_ETHRX, 1);
    set_vector(minimac_tx_interrupt_handler, IRQ_ETHTX, 1);
    lm32_interrupt_unmask(MM_ETHRX_IRQMASK);
    lm32_interrupt_unmask(MM_ETHTX_IRQMASK);
  }
 
  /*   
   *  Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   *  Start the receiver and transmitter
   */

  lm32_interrupt_ack( MM_ETHTX_IRQMASK | MM_ETHRX_IRQMASK );
  minimac_write(MM_MINIMAC_TXREMAINING, 0);

  for (j = 0 ; j < NB_RX_SLOTS ; j++) {
    minimac_write(rx_slot_addr[j], (unsigned int)rxbuffs[j]);
    minimac_write(rx_slot_state[j], MINIMAC_STATE_LOADED);
  }
  

  minimac_write(MM_MINIMAC_SETUP, 0);
  rtems_event_send(sc->rxDaemonTid, INTERRUPT_EVENT);
}

static void minimac_stop (struct minimac_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  unsigned char j;
  ifp->if_flags &= ~IFF_RUNNING;

  /*
   *  Shuts down receiver and transmitter
   */
  for (j = 0 ; j < NB_RX_SLOTS ; j++)
    minimac_write(rx_slot_state[j], MINIMAC_STATE_EMPTY);
  minimac_write(MM_MINIMAC_TXREMAINING, 0);
  minimac_write(MM_MINIMAC_SETUP, MINIMAC_SETUP_RXRST | MINIMAC_SETUP_TXRST);
}

static int minimac_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{

        struct minimac_softc *sc = ifp->if_softc;
        int error = 0; 
        switch (command) {
        case SIOCGIFADDR:
        case SIOCSIFADDR:
                ether_ioctl (ifp, command, data);
                break;

        case SIOCSIFFLAGS:
                switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
                case IFF_RUNNING:
                        minimac_stop (sc);
                        break;

                case IFF_UP:
                        minimac_init (sc);
                        break;

                case IFF_UP | IFF_RUNNING:
                        minimac_stop (sc);
                        minimac_init (sc);
                        break;

                default:
                        break;
                }    
                break;

        case SIO_RTEMS_SHOW_STATS:
                minimac_stats (sc);
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

rtems_isr minimac_rx_interrupt_handler(rtems_vector_number vector)
{
  unsigned int ip;
  struct minimac_softc *sc = &minimac_softc;
  lm32_read_interrupts(ip);
  if (ip & MM_ETHRX_IRQMASK) {
    lm32_interrupt_mask(MM_ETHRX_IRQMASK);
    rtems_event_send(sc->rxDaemonTid, INTERRUPT_EVENT);
    sc->rxInterrupts++; // update stats
  }
}

rtems_isr minimac_tx_interrupt_handler(rtems_vector_number vector)
{
  int ip;
  struct minimac_softc *sc = &minimac_softc;
  lm32_read_interrupts(ip);
  if (ip & MM_ETHTX_IRQMASK) {
    lm32_interrupt_ack(MM_ETHTX_IRQMASK);
    rtems_event_send(sc->txDaemonTid, INTERRUPT_EVENT);
    sc->txInterrupts++; // update stats
  }
}


static void minimac_rxDaemon(void *arg)
{
  struct ifnet *ifp = &minimac_softc.arpcom.ac_if;
  rtems_event_set events;
  struct minimac_softc *sc = &minimac_softc;
  for (;;) {
    uint32_t *buf;
    int rxlen;
    uint8_t j;
    struct mbuf *m;
    struct ether_header *eh;
    rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
    RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &events);
    if(minimac_read(MM_MINIMAC_SETUP) & MINIMAC_SETUP_RXRST ) {
      printk("Minimac RX FIFO overflow!\n");
      minimac_write(MM_MINIMAC_SETUP, 0);
      lm32_interrupt_ack(MM_ETHRX_IRQMASK);
      lm32_interrupt_unmask(MM_ETHRX_IRQMASK);
      sc->txFifoFull++; // update stats
    }

    for (j = 0 ; j < NB_RX_SLOTS ; j++) {
      if (minimac_read(rx_slot_state[j]) == MINIMAC_STATE_PENDING) {
        __asm__ volatile( /* Invalidate Level-1 data cache */
          "wcsr DCC, r0\n"
          "nop\n"
        );
        rxlen = minimac_read(rx_slot_count[j]);
        rxlen -= 8; // we drop the preamble
        MGETHDR(m, M_WAIT, MT_DATA);
        MCLGET(m, M_WAIT);
        m->m_pkthdr.rcvif = ifp;
        buf = (uint32_t *) mtod(m, uint32_t*);
        memcpy(buf, (uint8_t *)minimac_read(rx_slot_addr[j]) + 8, rxlen);
        m->m_len = m->m_pkthdr.len = rxlen - sizeof(uint32_t) - sizeof(struct ether_header);

        minimac_write(rx_slot_state[j], MINIMAC_STATE_EMPTY);
        minimac_write(rx_slot_state[j], MINIMAC_STATE_LOADED);
        eh = mtod(m, struct ether_header*);
        m->m_data += sizeof(struct ether_header);
#ifdef  CPU_U32_FIX
        ipalign(m);
#endif
        ether_input(ifp, eh, m);
      }
    }
    lm32_interrupt_ack(MM_ETHRX_IRQMASK); // we ack once for all the rx interruptions
    lm32_interrupt_unmask(MM_ETHRX_IRQMASK);
  }
}
static void minimac_txDaemon(void *arg)
{
  struct ifnet *ifp = &minimac_softc.arpcom.ac_if;
  rtems_event_set events;
  struct mbuf *m;
  int txq;

  for (;;) {
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT | INTERRUPT_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);
    for (;;) {
      txq = 2048;

      if (txq < ifp->if_mtu)
        break;

      IF_DEQUEUE(&ifp->if_snd, m);

      if (!m)
        break;
      minimac_sendpacket(ifp, m);
      m_freem(m);
    }
  ifp->if_flags &= ~IFF_OACTIVE;
  }
}

static void minimac_stats(struct minimac_softc *sc)
{

}


static void minimac_sendpacket(struct ifnet *ifp, struct mbuf *m)
{
  struct mbuf *nm = m;
  struct minimac_softc *sc = &minimac_softc;
  unsigned int len = 0;
  struct mm_packet p;
  unsigned int crc;
  uint8_t i;
  for (i = 0 ; i < 7 ; i++) // Preamble
    p.preamble[i] = 0x55;
  p.preamble[7] = 0xd5;
  
  do
    {
      unsigned int mlen;
      mlen = nm->m_len;
      if (nm->m_len > 0) {
        m_copydata(nm, 0, mlen, p.raw_data + len);
        len += nm->m_len;
      }

    } while ( (nm = nm->m_next) != 0 );
    for ( ; len < 60 ; len++)
      p.raw_data[len] = 0x00; // Padding

    crc = mm_ether_crc32((uint8_t *)p.raw_data, len); // CRC32

    p.raw_data[len] = crc & 0xff;
    p.raw_data[len+1] = (crc & 0xff00) >> 8;
    p.raw_data[len+2] = (crc & 0xff0000) >> 16;
    p.raw_data[len+3] = crc >> 24;
    
    len += 4; // We add 4 bytes of CRC32

    if (len + 8 < 64) {
      printk("[minimac] Packet is too small !\n");
      sc->txErrors++; // update stats
      return;
    }
    
    minimac_write(MM_MINIMAC_TXADR, (unsigned int)&p);
    minimac_write(MM_MINIMAC_TXREMAINING, (unsigned int)(len + 8));
}
