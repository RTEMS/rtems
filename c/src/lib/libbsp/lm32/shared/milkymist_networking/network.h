/*  network.h
 *
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


#ifndef _MM_NETWORKING_H_
#define _MM_NETWORKING_H_

#include "../include/system_conf.h"

#define IRQ_ETHRX 11
#define IRQ_ETHTX 12

#define INTERRUPT_EVENT RTEMS_EVENT_1
#define START_TRANSMIT_EVENT  RTEMS_EVENT_2

#define MINIMAC_STATE_EMPTY (0x0)
#define MINIMAC_STATE_LOADED  (0x1)
#define MINIMAC_STATE_PENDING (0x2)

#define MINIMAC_SETUP_RXRST (0x1)
#define MINIMAC_SETUP_TXRST (0x2)

#define NB_RX_SLOTS 4

#define MM_ETHTX_IRQMASK  (1 << IRQ_ETHTX)
#define MM_ETHRX_IRQMASK  (1 << IRQ_ETHRX)
#define ETHERNET_FRAME_LENGTH 1532

struct mm_packet {
  unsigned char preamble[8];
  char raw_data[MLEN];
} __attribute__((aligned(4), packed));

struct minimac_softc {

  struct arpcom arpcom;
  uint8_t registered;

  /*
   *  Statistics
   */

  rtems_id  rxDaemonTid;
  rtems_id  txDaemonTid;

  unsigned long int rxInterrupts;
  unsigned long int txInterrupts;
  unsigned long int rxedPackets;
  unsigned long int txFifoFull;
  unsigned long int txErrors;
};

int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *, int);

static void minimac_start(struct ifnet *);
static void minimac_init(void *);
static int minimac_ioctl(struct ifnet *, ioctl_command_t, caddr_t);
static void minimac_stop(struct minimac_softc *);

static void minimac_txDaemon(void *);
static void minimac_rxDaemon(void *);
static void minimac_sendpacket(struct ifnet *, struct mbuf *);

static rtems_isr minimac_rx_interrupt_handler (rtems_vector_number);
static rtems_isr minimac_tx_interrupt_handler (rtems_vector_number);

static void minimac_stats(struct minimac_softc *);

#endif
