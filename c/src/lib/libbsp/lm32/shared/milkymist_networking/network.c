/*
 *  RTEMS driver for Minimac2 ethernet IP-core of Milkymist SoC
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis, France
 *  Copyright (C) 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <stdio.h>
#include <string.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/status-checks.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <rtems.h>
#include "bspopts.h"
#include "../include/system_conf.h"
#include "network.h"

#define CTS_EVENT             RTEMS_EVENT_1
#define RX_EVENT              RTEMS_EVENT_1
#define START_TRANSMIT_EVENT  RTEMS_EVENT_2

static struct arpcom arpcom;
static rtems_id rx_daemon_id;
static rtems_id tx_daemon_id;

static void minimac_init(void *arg);
static int minimac_ioctl(struct ifnet *ifp, ioctl_command_t command,
  caddr_t data);
static void minimac_start(struct ifnet *ifp);

static void rx_daemon(void *arg);
static void tx_daemon(void *arg);
static rtems_isr rx_interrupt_handler(rtems_vector_number vector);
static rtems_isr tx_interrupt_handler(rtems_vector_number vector);

static bool validate_mac(const char *m)
{
  int i;
  
  for(i=0;i<6;i++)
    if((m[i] != 0x00) && (m[i] != 0xff))
      return true;
  return false;
}

static const char *get_mac_address(void)
{
  const char *flash_mac = (const char *)FLASH_OFFSET_MAC_ADDRESS;
  static const char fallback_mac[6] = { 0x10, 0xe2, 0xd5, 0x00, 0x00, 0x00 };
  
  if(validate_mac(flash_mac))
    return flash_mac;
  else {
    printk("Warning: using fallback MAC address\n");
    return fallback_mac;
  }
}

int rtems_minimac_driver_attach(struct rtems_bsdnet_ifconfig *config,
  int attaching)
{
  struct ifnet *ifp;
  rtems_isr_entry dummy;
  int i;
  static int registered;
  uint8_t *tx_buffer = (uint8_t *)MINIMAC_TX_BASE;

  if(!attaching) {
    printk("Minimac driver cannot be detached.\n");
    return 0;
  }

  ifp = &(arpcom.ac_if);

  if(registered) {
    printk("Minimac driver already in use.\n");
    return 0;
  }
  registered = 1;

  memcpy(arpcom.ac_enaddr, get_mac_address(), 6);
  ifp->if_mtu = ETHERMTU;
  ifp->if_unit = 0;
  ifp->if_name = "minimac";
  ifp->if_init = minimac_init;
  ifp->if_ioctl = minimac_ioctl;
  ifp->if_start = minimac_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;

  if_attach(ifp);
  ether_ifattach(ifp);

  rx_daemon_id = rtems_bsdnet_newproc("mrxd", 4096, rx_daemon, NULL);
  tx_daemon_id = rtems_bsdnet_newproc("mtxd", 4096, tx_daemon, NULL);
  rtems_interrupt_catch(rx_interrupt_handler, MM_IRQ_ETHRX, &dummy);
  rtems_interrupt_catch(tx_interrupt_handler, MM_IRQ_ETHTX, &dummy);
  
  MM_WRITE(MM_MINIMAC_STATE0, MINIMAC_STATE_LOADED);
  MM_WRITE(MM_MINIMAC_STATE1, MINIMAC_STATE_LOADED);

  for(i=0;i<7; i++)
    tx_buffer[i] = 0x55;
  tx_buffer[7] = 0xd5;
  MM_WRITE(MM_MINIMAC_SETUP, 0);
  rtems_event_send(tx_daemon_id, CTS_EVENT);
  
  bsp_interrupt_vector_enable(MM_IRQ_ETHRX);
  bsp_interrupt_vector_enable(MM_IRQ_ETHTX);

  return 1;
}

static void minimac_start(struct ifnet *ifp)
{
  rtems_event_send(tx_daemon_id, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

static void minimac_init(void *arg)
{
  struct ifnet *ifp = &arpcom.ac_if;
  ifp->if_flags |= IFF_RUNNING;
}

static void minimac_stop(void)
{
  struct ifnet *ifp = &arpcom.ac_if;
  ifp->if_flags &= ~IFF_RUNNING;
}

static int minimac_ioctl(struct ifnet *ifp, ioctl_command_t command,
  caddr_t data)
{
  int error;
  
  error = 0;
  switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, command, data);
      break;

    case SIOCSIFFLAGS:
      switch(ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
          minimac_stop();
          break;
        case IFF_UP:
          minimac_init(NULL);
          break;
        case IFF_UP | IFF_RUNNING:
          minimac_stop();
          minimac_init(NULL);
          break;
        default:
          break;
      }
      break;

    default:
      error = EINVAL;
      break;
  }
  return error;
}


static rtems_isr rx_interrupt_handler(rtems_vector_number vector)
{
  /* Deassert IRQ line.
   * The RX daemon will then read all the slots we marked as empty.
   */
  if(MM_READ(MM_MINIMAC_STATE0) == MINIMAC_STATE_PENDING)
    MM_WRITE(MM_MINIMAC_STATE0, MINIMAC_STATE_EMPTY);
  if(MM_READ(MM_MINIMAC_STATE1) == MINIMAC_STATE_PENDING)
    MM_WRITE(MM_MINIMAC_STATE1, MINIMAC_STATE_EMPTY);

  rtems_event_send(rx_daemon_id, RX_EVENT);

  lm32_interrupt_ack(1 << MM_IRQ_ETHRX);
}

static void receive_packet(uint8_t *buffer, int length)
{
  struct ifnet *ifp = &arpcom.ac_if;
  struct mbuf *m;
  struct ether_header *eh;
  uint32_t computed_crc, net_crc;
  
  if(length < 64) {
    printk("Warning: Ethernet packet too short\n");
    return;
  }

  length -= 4; /* strip CRC */
  net_crc = ((uint32_t)buffer[length])
    | ((uint32_t)buffer[length+1] << 8)
    | ((uint32_t)buffer[length+2] << 16)
    | ((uint32_t)buffer[length+3] << 24);
  length -= 8; /* strip preamble */
  computed_crc = ether_crc32_le(&buffer[8], length) ^ 0xffffffff;
  if(computed_crc == net_crc) {
    MGETHDR(m, M_WAIT, MT_DATA);
    MCLGET(m, M_WAIT);
    length -= sizeof(struct ether_header); /* strip Ethernet header */
    memcpy(m->m_data, &buffer[8+sizeof(struct ether_header)], length);
    m->m_len = m->m_pkthdr.len = length;
    m->m_pkthdr.rcvif = ifp;
    eh = (struct ether_header *)&buffer[8];
    ether_input(ifp, eh, m);
  } else
    printk("Ethernet CRC error: got %08x expected %08x (len=%d)\n",
      net_crc, computed_crc, length);
}

static void rx_daemon(void *arg)
{
  rtems_event_set events;
  
  while(1) {
    rtems_bsdnet_event_receive(RX_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT, &events);

    if(MM_READ(MM_MINIMAC_STATE0) == MINIMAC_STATE_EMPTY) {
      receive_packet((uint8_t *)MINIMAC_RX0_BASE, MM_READ(MM_MINIMAC_COUNT0));
      MM_WRITE(MM_MINIMAC_STATE0, MINIMAC_STATE_LOADED);
    }
    if(MM_READ(MM_MINIMAC_STATE1) == MINIMAC_STATE_EMPTY) {
      receive_packet((uint8_t *)MINIMAC_RX1_BASE, MM_READ(MM_MINIMAC_COUNT1));
      MM_WRITE(MM_MINIMAC_STATE1, MINIMAC_STATE_LOADED);
    }
  }
}

/* RTEMS apparently doesn't support m_length() ... */
static int copy_mbuf_chain(struct mbuf *m, uint8_t *target)
{
  int len;

  len = 0;
  while(m != NULL) {
    if(m->m_len > 0) {
      m_copydata(m, 0, m->m_len, (caddr_t)(target + len));
      len += m->m_len;
    }
    m = m->m_next;
  }
  return len;
}

static void send_packet(struct ifnet *ifp, struct mbuf *m)
{
  unsigned int len;
  unsigned int crc;
  uint8_t *tx_buffer = (uint8_t *)(MINIMAC_TX_BASE+8);
  
  len = copy_mbuf_chain(m, tx_buffer);
  for(;len<60;len++)
    tx_buffer[len] = 0x00; // Padding

  crc = ether_crc32_le(tx_buffer, len) ^ 0xffffffff;

  tx_buffer[len] = crc & 0xff;
  tx_buffer[len+1] = (crc & 0xff00) >> 8;
  tx_buffer[len+2] = (crc & 0xff0000) >> 16;
  tx_buffer[len+3] = crc >> 24;

  len += 4; // We add 4 bytes of CRC32

  MM_WRITE(MM_MINIMAC_TXCOUNT, len + 8);
}

static rtems_isr tx_interrupt_handler(rtems_vector_number vector)
{
  lm32_interrupt_ack(1 << MM_IRQ_ETHTX);
  rtems_event_send(tx_daemon_id, CTS_EVENT);
}

static void tx_daemon(void *arg)
{
  struct ifnet *ifp = &arpcom.ac_if;
  rtems_event_set events;
  struct mbuf *m;
  
  while(1) {
    rtems_bsdnet_event_receive(START_TRANSMIT_EVENT,
      RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);
    while(1) {
      IF_DEQUEUE(&ifp->if_snd, m);
      if(m == NULL)
        break;
      rtems_bsdnet_event_receive(CTS_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT,
        RTEMS_NO_TIMEOUT, &events);
      send_packet(ifp, m);
      m_freem(m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}
