/*
  ------------------------------------------------------------------------

  Copyright Cybertec Pty Ltd, 2000
  All rights reserved Cybertec Pty Ltd, 2000

  Port to the DIMM PC copyright (c) 2004 Angelo Fraietta
   This project has been assisted by the Commonwealth Government
   through the Australia Council, its arts funding and advisory body.

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  CS8900 RTEMS driver.

  See the header file for details.

*/

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "cs8900.h"

/*
 * We expect to be able to read a complete packet into an mbuf.
 */

#if (MCLBYTES < 1520)
#error "CS8900 Driver must have MCLBYTES >= 1520"
#endif

/*
 * Task event usage.
 */

#define CS8900_RX_OK_EVENT    RTEMS_EVENT_1
#define CS8900_TX_START_EVENT RTEMS_EVENT_1
#define CS8900_TX_OK_EVENT    RTEMS_EVENT_2
#define CS8900_TX_WAIT_EVENT  RTEMS_EVENT_3

/*
 * IO Packet Page inteface.
 */

static inline unsigned short
io_pp_get_reg_16 (cs8900_device *cs, unsigned short reg)
{
  rtems_interrupt_level level;
  unsigned short        data;
  rtems_interrupt_disable (level);
  cs8900_io_set_reg (cs, CS8900_IO_PACKET_PAGE_PTR,
                     0x3000 | CS8900_PPP_AUTO_INCREMENT | reg);
  data = cs8900_io_get_reg (cs, CS8900_IO_PP_DATA_PORT0);
  rtems_interrupt_enable (level);
  return data;
}

static inline uint32_t
io_pp_get_reg_32 (cs8900_device *cs, uint16_t reg)
{
  rtems_interrupt_level level;
  uint32_t data;
  rtems_interrupt_disable (level);
  cs8900_io_set_reg (cs, CS8900_IO_PACKET_PAGE_PTR,
                     0x3000 | CS8900_PPP_AUTO_INCREMENT | reg);
  data = cs8900_io_get_reg (cs, CS8900_IO_PP_DATA_PORT0);
  data <<= 16;
  data |= cs8900_io_get_reg (cs, CS8900_IO_PP_DATA_PORT1);
  rtems_interrupt_enable (level);
  return data;
}

static inline void
io_pp_set_reg_16 (cs8900_device *cs, unsigned short reg, unsigned short data)
{
  rtems_interrupt_level level;
  rtems_interrupt_disable (level);
  cs8900_io_set_reg (cs, CS8900_IO_PACKET_PAGE_PTR,
                     0x3000 | CS8900_PPP_AUTO_INCREMENT | reg);
  cs8900_io_set_reg (cs, CS8900_IO_PP_DATA_PORT0, data);
  rtems_interrupt_enable (level);
}

static inline void
io_pp_set_reg_32 (cs8900_device *cs, unsigned short reg, unsigned long data)
{
  cs8900_io_set_reg (cs, CS8900_IO_PACKET_PAGE_PTR,
                     0x3000 | CS8900_PPP_AUTO_INCREMENT | reg);
  cs8900_io_set_reg (cs, CS8900_IO_PP_DATA_PORT0, data >> 16);
  cs8900_io_set_reg (cs, CS8900_IO_PP_DATA_PORT1, data);
}

static inline void
io_pp_bit_set_reg_16 (cs8900_device *cs, unsigned short reg, unsigned short mask)
{
  rtems_interrupt_level level;
  rtems_interrupt_disable (level);
  io_pp_set_reg_16 (cs, reg, io_pp_get_reg_16 (cs, reg) | mask);
  rtems_interrupt_enable (level);
}

static inline void
io_pp_bit_clear_reg_16 (cs8900_device *cs, unsigned short reg, unsigned short mask)
{
  rtems_interrupt_level level;
  rtems_interrupt_disable (level);
  io_pp_set_reg_16 (cs, reg, io_pp_get_reg_16 (cs, reg) & ~mask);
  rtems_interrupt_enable (level);
}

/*
 * Memory Mapped Packet Page interface.
 *
 * If the BSP does not configure mem_base use the I/O register accesses.
 */

static inline unsigned short
mem_pp_get_reg (cs8900_device *cs, unsigned short reg)
{
  if (!cs->mem_base)
    return io_pp_get_reg_16 (cs, reg);
  return cs8900_mem_get_reg (cs, reg);
}

static inline void
mem_pp_set_reg (cs8900_device *cs, unsigned short reg, unsigned short data)
{
  if (!cs->mem_base)
    io_pp_set_reg_16 (cs, reg, data);
  else
    cs8900_mem_set_reg (cs, reg, data);
}

static inline void
mem_pp_bit_set_reg (cs8900_device *cs, unsigned short reg, unsigned short mask)
{
  if (!cs->mem_base)
    io_pp_bit_set_reg_16 (cs, reg, mask);
  else
  {
    rtems_interrupt_level level;
    rtems_interrupt_disable (level);
    mem_pp_set_reg (cs, reg, mem_pp_get_reg (cs, reg) | mask);
    rtems_interrupt_enable (level);
  }
}

static inline void
mem_pp_bit_clear_reg (cs8900_device *cs, unsigned short reg, unsigned short mask)
{
  if (!cs->mem_base)
    io_pp_bit_clear_reg_16 (cs, reg, mask);
  else
  {
    rtems_interrupt_level level;
    rtems_interrupt_disable (level);
    mem_pp_set_reg (cs, reg, mem_pp_get_reg (cs, reg) & ~mask);
    rtems_interrupt_enable (level);
  }
}

/*
 * Trace defines and control structures.
 */

#define CS8900_T_INT         (0)
#define CS8900_T_RX_OK       (1)
#define CS8900_T_RX_DROPPED  (2)
#define CS8900_T_NO_MBUF     (3)
#define CS8900_T_NO_CLUSTERS (4)
#define CS8900_T_RX_BEGIN    (5)
#define CS8900_T_RX_END      (6)

#if CS8900_TRACE

static const char *cs8900_trace_labels[] =
{
  "int",
  "rx ok",
  "rx dropped",
  "no mbuf",
  "no clusters",
  "rx begin",
  "rx end"
};

/*
 * Assumes a micro-second timer such as the Coldfire.
 */

uint32_t   rtems_read_timer ();

static inline void
cs8900_trace (cs8900_device *cs, unsigned short key, unsigned long var)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);

  if (cs->trace_in < CS8900_TRACE_SIZE)
  {
    cs->trace_key[cs->trace_in]  = key;
    cs->trace_var[cs->trace_in]  = var;
    cs->trace_time[cs->trace_in] = rtems_read_timer ();
    cs->trace_in++;
  }

  rtems_interrupt_enable (level);
}
#else
#define cs8900_trace(c, k, v)
#endif

void cs8900_get_mac_addr (cs8900_device *cs, unsigned char *mac_address)
{
  unsigned short ma;

  /*
   * Only ever use IO calls for this function as it can be
   * called before memory mode has been enabled.
   */

  ma = io_pp_get_reg_16 (cs, CS8900_PP_IA);
  mac_address[0] = ma >> 8;
  mac_address[1] = ma;

  ma = io_pp_get_reg_16 (cs, CS8900_PP_IA + 2);
  mac_address[2] = ma >> 8;
  mac_address[3] = ma;

  ma = io_pp_get_reg_16 (cs, CS8900_PP_IA + 4);
  mac_address[4] = ma >> 8;
  mac_address[5] = ma;
}

/*
 * Bring the chip online.
 */

static void
cs8900_hardware_init (cs8900_device *cs)
{
  unsigned long  prod_id;
  unsigned short status;

  /*
   * Do nothing while the device is calibrating and checking the EEPROM.
   * We must wait 20msecs.
   */

  io_pp_bit_set_reg_16 (cs, CS8900_PP_SelfCTL, CS8900_SELF_CTRL_RESET);

  rtems_task_wake_after (TOD_MILLISECONDS_TO_TICKS (20));

  status = io_pp_get_reg_16 (cs, CS8900_PP_SelfST);
  if (status == 0) {
      printf("Reading status register again\n");
      status = io_pp_get_reg_16 (cs, CS8900_PP_SelfST);
  }

  if (((status & CS8900_SELF_STATUS_INITD) == 0) ||
      ((status & CS8900_SELF_STATUS_INITD) &&
       (status & CS8900_SELF_STATUS_EEPROM_PRESENT) &&
       (status & CS8900_SELF_STATUS_SIBUST)))
  {
    printf ("CS8900: %s. Initialisation aborted.\n",
            (status & CS8900_SELF_STATUS_INITD) ?
            "EEPROM read/write failed to complete" :
            "Failed to complete to reset");
    return;
  }

  /* Set the RX queue size if not set by the BSP. */

  if (cs->rx_queue_size == 0)
    cs->rx_queue_size = 10;

  /* Probe the device for its ID */

  prod_id = io_pp_get_reg_32 (cs, CS8900_PP_PROD_ID);

  if ((prod_id >> 16) != CS8900_ESIA_ID)
  {
    printf ("CS8900: Invalid EISA ID, read product code 0x%08lx\n", prod_id);
    return;
  }

  if ((prod_id & 0x000000ff) != 0)
  {
    printf ("CS8900: Unsupported product id, read product code 0x%08lx\n",
            prod_id);
    return;
  }

  printf ("CS8900 Rev %ld, %s, %s.\n",
          (prod_id >> 8) & 0x1f,
          status & CS8900_SELF_STATUS_3_3_V ? "3.3V" : "5.0V",
          status & CS8900_SELF_STATUS_EEPROM_PRESENT ?
          "EEPROM present" : "no EEPROM");

  /*
   * Switch to memory base accesses as they are faster. No indirect access.
   */

  if (cs->mem_base)
  {
    io_pp_set_reg_16 (cs, CS8900_PP_MEM_BASE, cs->mem_base);
    io_pp_set_reg_16 (cs, CS8900_PP_MEM_BASE + 2, (cs->mem_base >> 16) & 0xf);

    io_pp_set_reg_16 (cs,
                      CS8900_PP_BusCTL,
                      CS8900_BUS_CTRL_RESET_RX_DMA |
                      CS8900_BUS_CTRL_USE_SA |
                      CS8900_BUS_CTRL_MEMORY_ENABLE);
    io_pp_set_reg_16 (cs,
                      CS8900_PP_BusCTL,
                      CS8900_BUS_CTRL_USE_SA |
                      CS8900_BUS_CTRL_MEMORY_ENABLE);
  }

  /*
   * We are now in memory mapped mode.
   */

  /*
   * Program the Line Control register with the mode we want.
   *
   * No auto detect support at the moment. Only 10BaseT.
   */

  mem_pp_set_reg (cs, CS8900_PP_LineCFG, CS8900_LINE_CTRL_10BASET);

  /*
   * Ask the user for the MAC address, the program into the device.
   */

#define MACO(o) cs->arpcom.ac_enaddr[o]

  mem_pp_set_reg (cs, CS8900_PP_IA,
                  (((unsigned int) MACO (1)) << 8) |
                  ((unsigned int) MACO (0)));
  mem_pp_set_reg (cs, CS8900_PP_IA + 2,
                  (((unsigned int) MACO (3)) << 8) |
                  ((unsigned int) MACO (2)));
  mem_pp_set_reg (cs, CS8900_PP_IA + 4,
                  (((unsigned int) MACO (5)) << 8) |
                  ((unsigned int) MACO (4)));

  /*
   * Set the Buffer configuration.
   */

  mem_pp_set_reg (cs, CS8900_PP_BufCFG,
                  CS8900_BUFFER_CONFIG_RDY_FOR_TX |
                  CS8900_BUFFER_CONFIG_TX_UNDERRUN |
                  CS8900_BUFFER_CONFIG_TX_COL_OVF |
                  CS8900_BUFFER_CONFIG_RX_MISSED_OVF);

  /*
   * Set the Receiver configuration.
   */

  mem_pp_set_reg (cs, CS8900_PP_RxCFG,
                  CS8900_RX_CONFIG_RX_OK |
                  CS8900_RX_CONFIG_CRC_ERROR |
                  CS8900_RX_CONFIG_RUNT|
                  CS8900_RX_CONFIG_EXTRA_DATA);

  /*
   * Set the Receiver control.
   */

  mem_pp_set_reg (cs, CS8900_PP_RxCTL,
                  CS8900_RX_CTRL_RX_OK |
                  CS8900_RX_CTRL_MULTICAST |
                  CS8900_RX_CTRL_INDIVIDUAL |
                  CS8900_RX_CTRL_BROADCAST);

  /*
   * Set the Transmitter configuration.
   */

  mem_pp_set_reg (cs, CS8900_PP_TxCFG,
                  CS8900_TX_CONFIG_TX_OK |
                  CS8900_TX_CONFIG_OUT_OF_WINDOW |
                  CS8900_TX_CONFIG_JABBER |
                  CS8900_TX_CONFIG_16_COLLISION);

  /*
   * Attach the interrupt handler.
   */

  cs8900_attach_interrupt (cs);

  /*
   * Program the interrupt level we require then enable interrupts.
   *
   * Note, this will need to change to support other levels.
   */

  mem_pp_set_reg (cs, CS8900_PP_INT, cs->irq_level & 3);

  mem_pp_bit_set_reg (cs, CS8900_PP_BusCTL,
                      CS8900_BUS_CTRL_ENABLE_INT);
}

rtems_isr
cs8900_interrupt (rtems_vector_number v, void *csp)
{
  cs8900_device  *cs = csp;
  unsigned short isq = 0;
  struct mbuf    *m;
  unsigned char  *p;

  ++cs->eth_stats.interrupts;

  while (1)
  {
    isq = mem_pp_get_reg (cs, CS8900_PP_ISQ);

    cs8900_trace (cs, CS8900_T_INT, isq);

    /*
     * No more interrupts to service.
     */

    if (isq == 0)
      return;

    switch (isq & 0x1f)
    {
      case 0x04:

        /*
         * RxEvent.
         */

        ++cs->eth_stats.rx_interrupts;

        if (isq & CS8900_RX_EVENT_RX_OK)
        {
          m = cs->rx_ready_head;
          if (m)
          {
            cs->rx_ready_head = m->m_nextpkt;
            if (cs->rx_ready_head == 0)
              cs->rx_ready_tail = 0;
            m->m_nextpkt = 0;
            cs->rx_ready_len--;

            p = mtod (m, unsigned char *);

            m->m_pkthdr.len = cs8900_get_data_block (cs, p);

            if (cs->rx_loaded_tail == 0)
              cs->rx_loaded_head = m;
            else
              cs->rx_loaded_tail->m_nextpkt = m;
            cs->rx_loaded_tail = m;
            cs->rx_loaded_len++;

            if (cs->rx_loaded_len == 1)
            {
              cs8900_trace (cs, CS8900_T_RX_OK, cs->rx_loaded_len);
              rtems_event_send (cs->rx_task, CS8900_RX_OK_EVENT);
            }
          }
          else
          {
            ++cs->eth_stats.rx_dropped;

            cs8900_trace (cs, CS8900_T_RX_DROPPED, cs->rx_loaded_len);

            if (cs->rx_loaded_len == 0)
              rtems_event_send (cs->rx_task, CS8900_RX_OK_EVENT);
          }
        }
        else
        {
          if (isq & CS8900_RX_EVENT_CRC_ERROR)
            ++cs->eth_stats.rx_crc_errors;

          if (isq & CS8900_RX_EVENT_RUNT)
            ++cs->eth_stats.rx_runt_errors;

          if (isq & CS8900_RX_EVENT_EXTRA_DATA)
            ++cs->eth_stats.rx_oversize_errors;
        }
        break;

      case 0x08:

        /*
         * TxEvent.
         */

        ++cs->eth_stats.tx_interrupts;

        if (cs->tx_active)
        {
          if (isq & CS8900_TX_EVENT_TX_OK)
            ++cs->eth_stats.tx_ok;

          cs->tx_active = 0;

          rtems_event_send (cs->tx_task, CS8900_TX_OK_EVENT);
        }
        break;

      case 0x0c:

        /*
         * BufEvent.
         */

        if (isq & CS8900_BUFFER_EVENT_RDY_FOR_TX)
        {
          if (cs->tx_active)
          {
            ++cs->eth_stats.tx_rdy4tx;
            rtems_event_send (cs->tx_task, CS8900_TX_WAIT_EVENT);
          }
        }
        else if (isq & CS8900_BUFFER_EVENT_TX_UNDERRUN)
        {
          ++cs->eth_stats.tx_underrun_errors;
          if (cs->tx_active)
            rtems_event_send (cs->tx_task, CS8900_TX_OK_EVENT);
        }
        else if (isq & CS8900_BUFFER_EVENT_SW_INT)
        {
          ++cs->eth_stats.int_swint_res;
        }
        break;

      case 0x10:

        /*
         * RxMiss.
         */

        cs->eth_stats.rx_missed_errors +=
          mem_pp_get_reg (cs, CS8900_PP_RxMISS) >> 6;
        break;

      case 0x12:

        /*
         * TxCol.
         */

        cs->eth_stats.tx_collisions +=
          mem_pp_get_reg (cs, CS8900_PP_TxCol) >> 6;
        break;

      default:
        break;
    }
  }

}

int
cs8900_link_active (cs8900_device *cs)
{
  return ((mem_pp_get_reg (cs, CS8900_PP_LineST) & CS8900_LINE_STATUS_LINK_OK) ?
          1 : 0);
}

static inline void
cs8900_rx_refill_queue (cs8900_device *cs)
{
  struct ifnet          *ifp = &cs->arpcom.ac_if;
  struct mbuf           *m;
  rtems_interrupt_level level;

  /*
   * Hold a single queue of mbuf's at the interface. This
   * will lower the latency of the driver.
   */

  while (cs->rx_ready_len < cs->rx_queue_size)
  {
    MGETHDR (m, M_DONTWAIT, MT_DATA);

    if (!m)
    {
      ++cs->eth_stats.rx_no_mbufs;
      cs8900_trace (cs, CS8900_T_NO_MBUF, cs->eth_stats.rx_no_mbufs);
      return;
    }

    MCLGET (m, M_DONTWAIT);

    if (!m->m_ext.ext_buf)
    {
      ++cs->eth_stats.rx_no_clusters;
      cs8900_trace (cs, CS8900_T_NO_CLUSTERS, cs->eth_stats.rx_no_clusters);
      m_free (m);
      return;
    }
    m->m_pkthdr.rcvif = ifp;
    m->m_nextpkt = 0;

    rtems_interrupt_disable (level);

    if (cs->rx_ready_tail == 0)
      cs->rx_ready_head = m;
    else
      cs->rx_ready_tail->m_nextpkt = m;
    cs->rx_ready_tail = m;
    cs->rx_ready_len++;

    rtems_interrupt_enable (level);
  }
}

static void
cs8900_rx_task (void *arg)
{
  cs8900_device         *cs = arg;
  struct ifnet          *ifp = &cs->arpcom.ac_if;
  rtems_event_set       events;
  struct mbuf           *m;
  struct ether_header   *eh;
  rtems_status_code     sc;
  rtems_interrupt_level level;

  /*
   * Turn the receiver and transmitter on.
   */

  mem_pp_bit_set_reg (cs, CS8900_PP_LineCFG,
                      CS8900_LINE_CTRL_RX_ON |
                      CS8900_LINE_CTRL_TX_ON);

  /*
   * Start the software interrupt watchdog.
   */

  mem_pp_bit_set_reg (cs, CS8900_PP_BufCFG,
                      CS8900_BUFFER_CONFIG_SW_INT);
  ++cs->eth_stats.int_swint_req;

  /*
   * Loop reading packets.
   */

  while (1)
  {
    cs8900_rx_refill_queue (cs);

    sc = rtems_bsdnet_event_receive (CS8900_RX_OK_EVENT,
                                     RTEMS_WAIT | RTEMS_EVENT_ANY,
                                     TOD_MILLISECONDS_TO_TICKS (250),
                                     &events);

    cs8900_rx_refill_queue (cs);

    if (sc == RTEMS_TIMEOUT)
    {
      /*
       * We need to check the interrupt hardware in the cs8900a
       * has not locked up. It seems this occurs if the ISQ
       * queue fills up.
       * To test we generate a software interrupt and watch
       * a counter go up. If the counter does not go for 2
       * software interrupts requests we flush the ISQ queue.
       */

      if ((cs->eth_stats.int_swint_req - cs->eth_stats.int_swint_res) > 1)
      {
        printf ("cs8900: int lockup, isq flush\n");

        mem_pp_bit_clear_reg (cs, CS8900_PP_BusCTL,
                              CS8900_BUS_CTRL_ENABLE_INT);

        while (mem_pp_get_reg (cs, CS8900_PP_ISQ) != 0);

        cs->eth_stats.int_swint_req = cs->eth_stats.int_swint_res = 0;
        ++cs->eth_stats.int_lockup;

        mem_pp_bit_set_reg (cs, CS8900_PP_BusCTL,
                            CS8900_BUS_CTRL_ENABLE_INT);
      }

      mem_pp_bit_set_reg (cs, CS8900_PP_BufCFG,
                          CS8900_BUFFER_CONFIG_SW_INT);
      ++cs->eth_stats.int_swint_req;
    }

    cs8900_trace (cs, CS8900_T_RX_BEGIN, cs->rx_loaded_len);

    while (cs->rx_loaded_len)
    {
      rtems_interrupt_disable (level);

      m = cs->rx_loaded_head;
      if (m)
      {
        cs->rx_loaded_head = m->m_nextpkt;
        if (cs->rx_loaded_head == 0)
          cs->rx_loaded_tail = 0;
        m->m_nextpkt = 0;
        cs->rx_loaded_len--;

        rtems_interrupt_enable (level);

        m->m_pkthdr.rcvif = ifp;

        cs->eth_stats.rx_bytes += m->m_pkthdr.len;

        m->m_len = m->m_pkthdr.len = m->m_pkthdr.len - sizeof (struct ether_header);

        eh = mtod (m, struct ether_header *);
        m->m_data += sizeof (struct ether_header);

        ++cs->eth_stats.rx_packets;

        ether_input (ifp, eh, m);
      }
      else
      {
        rtems_interrupt_enable (level);
      }
    }
    cs8900_trace (cs, CS8900_T_RX_END, cs->rx_loaded_len);
  }
}

static void
cs8900_tx_task (void *arg)
{
  cs8900_device     *cs = arg;
  struct ifnet      *ifp = &cs->arpcom.ac_if;
  rtems_event_set   events;
  struct mbuf       *m;
  rtems_status_code sc;

  /*
   * Wait for the link to come up.
   */

  rtems_task_wake_after (TOD_MILLISECONDS_TO_TICKS (750));

  /*
   * Loop processing the tx queue.
   */

  while (1)
  {
    /*
     * Fetch the mbuf list from the interface's queue.
     */

    IF_DEQUEUE (&ifp->if_snd, m);

    /*
     * If something actually is present send it.
     */

    if (!m)
    {
      ifp->if_flags &= ~IFF_OACTIVE;

      rtems_bsdnet_event_receive (CS8900_TX_START_EVENT,
                                  RTEMS_WAIT | RTEMS_EVENT_ANY,
                                  RTEMS_NO_TIMEOUT,
                                  &events);
    }
    else
    {
      if (cs8900_link_active (cs))
      {
        int resending;

        do
        {
          unsigned short buf_status;

          resending = 0;

          cs->tx_active = 1;

          mem_pp_set_reg (cs, CS8900_PP_TxCMD,
                          CS8900_TX_CMD_STATUS_TX_START_ENTIRE |
                          CS8900_TX_CMD_STATUS_FORCE);
          mem_pp_set_reg (cs, CS8900_PP_TxLength, m->m_pkthdr.len);

          buf_status = mem_pp_get_reg (cs, CS8900_PP_BusST);

          /*
           * If the bid for memory in the device fails trash the
           * transmit and try again next time.
           */

          if (buf_status & CS8900_BUS_STATUS_TX_BID_ERROR)
            ++cs->eth_stats.tx_bid_errors;
          else
          {
            /*
             * If the buffer is not read enable the interrupt and then wait.
             */

            if ((buf_status & CS8900_BUS_STATUS_RDY_FOR_TX_NOW) == 0)
            {
              cs->eth_stats.tx_wait_for_rdy4tx++;
              sc = rtems_bsdnet_event_receive (CS8900_TX_WAIT_EVENT,
                                               RTEMS_WAIT | RTEMS_EVENT_ANY,
                                               TOD_MILLISECONDS_TO_TICKS (750),
                                               &events);
              if (sc == RTEMS_TIMEOUT)
              {
                /*
                 * For some reason the wait request has been dropped,
                 * so lets resend from the start.
                 */

                printf ("tx resend\n");
                ++cs->eth_stats.tx_resends;
                resending = 1;
              }
            }

            if (!resending)
            {
              cs8900_tx_load (cs, m);
              cs->eth_stats.tx_packets++;
              cs->eth_stats.tx_bytes += m->m_pkthdr.len;
            }
          }
        }
        while (resending);

        m_freem (m);

        do
        {
          rtems_bsdnet_event_receive (CS8900_TX_OK_EVENT,
                                      RTEMS_WAIT | RTEMS_EVENT_ANY,
                                      RTEMS_NO_TIMEOUT,
                                      &events);
        }
        while (cs->tx_active);
      }
      else
      {
        ++cs->eth_stats.tx_dropped;
        m_freem (m);
      }
    }
  }
}

static void
cs8900_start (struct ifnet *ifp)
{
  cs8900_device *cs = ifp->if_softc;

  /*
   * Tell the transmit daemon to wake up and send a packet.
   */

  ifp->if_flags |= IFF_OACTIVE;

  rtems_event_send (cs->tx_task, CS8900_TX_START_EVENT);
}

static void
cs8900_stop (cs8900_device *cs)
{
  mem_pp_bit_clear_reg (cs, CS8900_PP_LineCFG,
                        CS8900_LINE_CTRL_RX_ON |
                        CS8900_LINE_CTRL_TX_ON);

  mem_pp_bit_clear_reg (cs, CS8900_PP_BusCTL,
                        CS8900_BUS_CTRL_ENABLE_INT);
}

static const char *eth_statistics_labels[] =
{
  "rx packets",
  "tx packets",
  "rx bytes",
  "tx bytes",
  "rx interrupts",
  "tx interrupts",
  "rx dropped",
  "rx no mbuf",
  "rx no custers",
  "rx oversize errors",
  "rx crc errors",
  "rx runt errors",
  "rx missed errors",
  "tx ok",
  "tx collisions",
  "tx bid errors",
  "tx wait for rdy4tx",
  "tx rdy4tx",
  "tx underrun errors",
  "tx dropped",
  "tx resends",
  "int swint req",
  "int swint res",
  "int lockup",
  "interrupts"
};

static void
cs8900_stats (cs8900_device *cs)
{
  int           i;
  int           max_label = 0;
  int           len;
  unsigned long *value = (unsigned long*) &cs->eth_stats.rx_packets;

  cs->eth_stats.rx_missed_errors +=
    mem_pp_get_reg (cs, CS8900_PP_RxMISS) >> 6;

  cs->eth_stats.tx_collisions +=
    mem_pp_get_reg (cs, CS8900_PP_TxCol) >> 6;

  printf ("Network Driver Stats for CS8900 :\n");

  for (i = 0; i < (sizeof (eth_statistics_labels) / sizeof (const char *)); i++)
  {
    len = strlen (eth_statistics_labels[i]);
    if (len > max_label)
      max_label = len;
  }

  max_label += 2;

  printf ("%*s - %10u %*s - %10u\n",
          max_label, "rx ready len", cs->rx_ready_len,
          max_label, "rx loaded len", cs->rx_loaded_len);

  for (i = 0;
       i < (sizeof (eth_statistics_labels) / sizeof (const char *));
       i++)
  {
    printf ("%*s - %10lu",
            max_label, eth_statistics_labels[i], value[i]);

    i++;

    if (i < (sizeof (eth_statistics_labels) / sizeof (const char *)))
      printf (" %*s - %10lu",
              max_label, eth_statistics_labels[i], value[i]);
    printf ("\n");
  }

#if CS8900_TRACE

  for (i = 0; i < cs->trace_in; i++)
  {
    printf ("%8ld.%03ld ", cs->trace_time[i] / 1000, cs->trace_time[i] % 1000);

    if (cs->trace_key[i] < sizeof (cs8900_trace_labels) / sizeof (char*))
      printf ("%s : ", cs8900_trace_labels[cs->trace_key[i]]);
    else
      printf ("unknown trace key, %d : ", cs->trace_key[i]);

    if (cs->trace_key[i] == CS8900_T_INT)
    {
      printf ("0x%04lx ", cs->trace_var[i]);
      if (cs->trace_var[i] == 0)
        printf ("end");
      else
      {
        switch (cs->trace_var[i] & 0x1f)
        {
          case 0x04:
            printf ("rx event");
            break;

          case 0x08:
            printf ("tx event");
            break;

          case 0x0c:
            printf ("buffer event");
            break;

          case 0x10:
            printf ("rx missed");
            break;

          case 0x12:
            printf ("tx collisions");
            break;

          case 0x1f:
            printf ("tx request");
            break;

          case 0x1e:
            printf ("tx wait 4 tx");
            break;

          case 0x1d:
            printf ("tx already active");
            break;

          default:
            printf ("unknown event");
            break;
        }
      }
    }
    else
      printf ("0x%08lx", cs->trace_var[i]);

    printf ("\n");
  }

  cs->trace_in = 0;

#endif
}

static void
cs8900_init (void *arg)
{
  cs8900_device *cs  = arg;
  struct ifnet  *ifp = &cs->arpcom.ac_if;

  if (cs->rx_task == 0)
  {

    /*
     * Set up the hardware.
     */

    cs8900_hardware_init (cs);

    /*
     * Start driver task. We have only one task.
     */

    cs->rx_task = rtems_bsdnet_newproc ("CSr0", 4096, cs8900_rx_task, cs);
    cs->tx_task = rtems_bsdnet_newproc ("CSt0", 4096, cs8900_tx_task, cs);
  }

#ifdef todo
  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
  else
#endif

  /*
   * Tell the world that we're running.
   */

  ifp->if_flags |= IFF_RUNNING;

  /*
   * Set the Line Control to bring the receive and transmitter online.
   */

  mem_pp_bit_set_reg (cs, CS8900_PP_LineCFG,
                      CS8900_LINE_CTRL_RX_ON |
                      CS8900_LINE_CTRL_TX_ON);

  mem_pp_bit_set_reg (cs, CS8900_PP_BusCTL,
                      CS8900_BUS_CTRL_ENABLE_INT);
}

static int
cs8900_ioctl (struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
  cs8900_device *cs = ifp->if_softc;
  int           error = 0;

  switch (cmd)
  {
    case SIOCGIFADDR:
    case SIOCSIFADDR:

      error = ether_ioctl (ifp, cmd, data);
      break;

    case SIOCSIFFLAGS:

      switch (ifp->if_flags & (IFF_UP | IFF_RUNNING))
      {
        case IFF_RUNNING:

          cs8900_stop (cs);
          break;

        case IFF_UP:

          cs8900_init (cs);
          break;

        case IFF_UP | IFF_RUNNING:

          cs8900_stop (cs);
          cs8900_init (cs);
          break;

        default:
          break;
      }
      break;

    case SIO_RTEMS_SHOW_STATS:

      cs8900_stats (cs);
      break;

      /* FIXME: Multicast commands must be added here.  */

    default:
      error = EINVAL;
      break;
  }

  return error;
}

int
cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching)
{
  cs8900_device *cs;
  struct ifnet  *ifp;
  int           mtu;
  int           unit;
  char          *name;

  /*
    * Parse driver name
   */

  if ((unit = rtems_bsdnet_parse_driver_name (config, &name)) < 0)
    return 0;

  cs      = config->drv_ctrl;
  cs->dev = unit;
  ifp     = &cs->arpcom.ac_if;

  if (attaching)
  {
    if (ifp->if_softc)
    {
      printf ("Driver `%s' already in use.\n", config->name);
      return 0;
    }

    /*
     * Process options
     */

    if (config->hardware_address)
      memcpy (cs->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
    else
      cs8900_get_mac_addr (cs, cs->arpcom.ac_enaddr);

    if (config->mtu)
      mtu = config->mtu;
    else
      mtu = ETHERMTU;

    cs->accept_bcast = !config->ignore_broadcast;

    /*
     * Set up network interface values.
     */

    ifp->if_softc  = cs;
    ifp->if_unit   = unit;
    ifp->if_name   = name;
    ifp->if_mtu    = mtu;
    ifp->if_init   = cs8900_init;
    ifp->if_ioctl  = cs8900_ioctl;
    ifp->if_start  = cs8900_start;
    ifp->if_output = ether_output;
    ifp->if_flags  = IFF_BROADCAST | IFF_SIMPLEX;

    if (ifp->if_snd.ifq_maxlen == 0)
      ifp->if_snd.ifq_maxlen = ifqmaxlen;

    /*
     * Attach the interface to the stack.
     */

    if_attach (ifp);
    ether_ifattach (ifp);
  }
  else
  {
    if (!ifp->if_softc)
    {
      printf ("Driver `%s' not found.\n", config->name);
      return 0;
    }

    cs8900_stop (cs);
    cs8900_detach_interrupt (cs);
  }

  return 1;
}
