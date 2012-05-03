/*
 * RTEMS for Nintendo DS WiFi driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *                       Benjamin Ratier <agho.pwn@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <bsp.h>

#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <rtems/irq.h>

#include <nds.h>
#include <dswifi9.h>

/*
 * RTEMS event number used by reception task to receive packet
 */

#define RECEIVE_EVENT RTEMS_EVENT_1

/*
 * RTEMS event number used to start the transmit daemon
 */

#define TRANSMIT_EVENT RTEMS_EVENT_2

/*
 * from compat.c
 */

void compat_wifi_output (struct mbuf *m);

/*
 * wifi driver structure
 */

static struct
{
  /* The bsdnet information structure */
  struct arpcom arpcom;

  /* indicate if the hwaddr is already set */
  uint8_t is_hwaddr_set;

  /* the thread ID of the transmit task */
  rtems_id tx_id;
  /* the thread ID of the receive task */
  rtems_id rx_id;

  /* reception queue (filled in compat.c:ReceiveHardwarePacket) */
  struct ifqueue recvq;
} softc;

static void
print_byte (unsigned char b)
{
  printk ("%x%x", b >> 4, b & 0x0f);
}

/*
 * wifi timer function, must be called every 50 ms
 */

static void
Timer_50ms (void)
{
  Wifi_Timer (50);
}

/*
 * notification function to send fifo message to arm7
 */

static void
arm9_synctoarm7 (void)
{
  /* send fifo message */
  REG_IPC_FIFO_TX = 0x87654321;
}

/*
 * interrupt handler to receive fifo messages from arm7
 */

static void
arm9_fifo (void)
{
  u32 value = REG_IPC_FIFO_RX;

  /* check incoming fifo messages */
  if (value == 0x87654321)
    Wifi_Sync ();
}

/*
 * signal an icoming packet
 */

void
wifi_signal (struct mbuf *m)
{
  rtems_interrupt_level level;

  /* enqueue the incoming packet */
  rtems_interrupt_disable (level);
  IF_ENQUEUE (&softc.recvq, m);
  rtems_interrupt_enable (level);

  /* signal the rx daemon */
  rtems_event_send (softc.rx_id, RECEIVE_EVENT);
}


/*
 * packet reception daemon
 */

static void
wifi_rxd (void *arg)
{
  rtems_interrupt_level level;
  struct ifnet *ifp = &softc.arpcom.ac_if;

  while (1) {
    rtems_event_set events;

    rtems_bsdnet_event_receive (RECEIVE_EVENT,
                                RTEMS_WAIT | RTEMS_EVENT_ANY,
                                RTEMS_NO_TIMEOUT, &events);

    while (1) {
      struct ether_header *eh;
      struct mbuf *m;

      rtems_interrupt_disable (level);
      if (softc.recvq.ifq_head == NULL) {
        rtems_interrupt_enable (level);
        break;
      }

      /* get next packet */
      IF_DEQUEUE (&softc.recvq, m);
      rtems_interrupt_enable (level);
      if (m == NULL) {
        panic ("wifi_rxd");
      }

      m->m_pkthdr.rcvif = ifp;

      /* extract ethernet header */
      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof (struct ether_header);
      m->m_len = m->m_pkthdr.len = m->m_len - sizeof (struct ether_header);

      /* push the packet into the stack */
      ether_input (ifp, eh, m);
    }
  }
}

/*
 * packet sending daemon
 */

static void
wifi_txd (void *arg)
{
  rtems_interrupt_level level;
  struct ifnet *ifp = &softc.arpcom.ac_if;

  while (1) {
    rtems_event_set events;

    rtems_bsdnet_event_receive (TRANSMIT_EVENT,
                                RTEMS_WAIT | RTEMS_EVENT_ANY,
                                RTEMS_NO_TIMEOUT, &events);
    while (1) {
      struct mbuf *m;

      rtems_interrupt_disable (level);
      if (ifp->if_snd.ifq_head == NULL) {
        ifp->if_flags &= ~IFF_OACTIVE;
        rtems_interrupt_enable (level);
        break;
      }

      /* grab next packet */
      IF_DEQUEUE (&ifp->if_snd, m);
      rtems_interrupt_enable (level);

      if (m == NULL) {
        panic ("wifi_txd");
      }

      /* call compatibility glue to dswifi */
      compat_wifi_output (m);
    }
  }
}

/*
 * wifi device initialization procedure
 */

static void
wifi_init (void *arg)
{
  struct ifnet *ifp = &softc.arpcom.ac_if;
  int i;

  /* create the receive & send daemons */
  if (softc.tx_id == 0) {
    Wifi_AutoConnect ();

    while (1) {
      i = Wifi_AssocStatus ();
      if (i == ASSOCSTATUS_ASSOCIATED) {
        printk ("Connected successfully!\n");
        break;
      }
      if (i == ASSOCSTATUS_CANNOTCONNECT) {
        printk ("Could not connect!\n");
        break;
      }
    }

    printk ("Starting daemon\n");
    softc.tx_id = rtems_bsdnet_newproc ("SCtx", 4096, wifi_txd, &softc);
    softc.rx_id = rtems_bsdnet_newproc ("SCrx", 4096, wifi_rxd, &softc);
  }

  ifp->if_flags |= IFF_RUNNING;
}

/*
 * wifi deactivation method.
 */

static void
wifi_shutdown (void *arg)
{
  /* XXX */
}

/*
 * ioctl hook function.
 */

static int
wifi_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  void *sc = ifp->if_softc;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    return ether_ioctl (ifp, command, data);

  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      wifi_shutdown (sc);
      break;
    case IFF_UP:
      wifi_init (sc);
      break;
    case IFF_UP | IFF_RUNNING:
      wifi_shutdown (sc);
      wifi_init (sc);
      break;
    default:
      break;
    }
    return 0;

  default:
    return EINVAL;
  }
}

/*
 * function for sending a packet
 */

static void
wifi_start (struct ifnet *ifp)
{
  /* wake up the send daemon */
  ifp->if_flags |= IFF_OACTIVE;
  rtems_event_send (softc.tx_id, TRANSMIT_EVENT);
}

/*
 * RTEMS device attach method for wiwi driver
 */

int
rtems_wifi_driver_attach (struct rtems_bsdnet_ifconfig *config, int attach)
{
  struct ifnet *ifp = &softc.arpcom.ac_if;

  printk ("wifi attach...\n");

  /* initialize ifnet structure */
  memset (&softc, 0, sizeof (softc));
  ifp->if_softc = &softc;
  ifp->if_unit = 0;
  ifp->if_name = "dswifi";
  ifp->if_mtu = ETHERMTU;
  ifp->if_init = wifi_init;
  ifp->if_ioctl = wifi_ioctl;
  ifp->if_watchdog = NULL;
  ifp->if_start = wifi_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0) {
    ifp->if_snd.ifq_maxlen = ifqmaxlen;
  }

  if (config->mtu != 0) {
    ifp->if_mtu = config->mtu;
  }

  if (config->hardware_address != NULL) {
    softc.is_hwaddr_set = 1;
    memcpy (softc.arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  } else {
    softc.is_hwaddr_set = 0;
  }

  u32 Wifi_pass;
  int i;

  /* wifi hardware initialization took from dswifi example code */

  /* send fifo message to initialize the arm7 wifi */
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

  Wifi_pass =
    Wifi_Init (WIFIINIT_OPTION_USELED | WIFIINIT_OPTION_USECUSTOMALLOC);
  REG_IPC_FIFO_TX = 0x12345678;
  REG_IPC_FIFO_TX = Wifi_pass;

  /* setup fifo IRQ */
  irqSet (IRQ_FIFO_NOT_EMPTY, arm9_fifo);
  irqEnable (IRQ_FIFO_NOT_EMPTY);

  /* enable FIFO IRQ */
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

  /* tell wifi lib to use our handler to notify arm7 */
  Wifi_SetSyncHandler (arm9_synctoarm7);

  /* set timer3 */
  TIMER3_DATA = -6553;          // 6553.1 * 256 cycles = ~50ms;
  TIMER3_CR = 0x00C2;           // enable, irq, 1/256 clock

  /* setup timer IRQ */
  irqSet (IRQ_TIMER3, Timer_50ms);
  irqEnable (IRQ_TIMER3);

  while (Wifi_CheckInit () == 0) {
    /* wait for arm7 to be initted successfully */
    while (REG_VCOUNT > 192);
    while (REG_VCOUNT < 192);
  }

  /* wifi init complete - wifi lib can now be used! */

  printk ("ok.\n");

  /* retrieve MAC address if unspecified by the user */
  if (!softc.is_hwaddr_set) {
    Wifi_GetData (WIFIGETDATA_MACADDRESS, ETHER_ADDR_LEN,
                  softc.arpcom.ac_enaddr);
  }

  print_byte (softc.arpcom.ac_enaddr[0]);
  for (i = 1; i < ETHER_ADDR_LEN; i++) {
    printk (":");
    print_byte (softc.arpcom.ac_enaddr[i]);
  }
  printk ("\n");

  /* attach the interface */
  if_attach (ifp);
  ether_ifattach (ifp);

  printk ("network device '%s' initialized\n", config->name);

  return 0;
}
