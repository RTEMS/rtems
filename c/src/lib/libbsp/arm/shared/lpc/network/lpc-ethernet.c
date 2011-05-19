/**
 * @file
 *
 * @ingroup lpc_eth
 *
 * @brief Ethernet driver.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc-ethernet-config.h>
#include <bsp/utility.h>

#include <rtems/status-checks.h>

#if MCLBYTES > (2 * 1024)
  #error "MCLBYTES to large"
#endif

#ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
  #define LPC_ETH_CONFIG_TX_BUF_SIZE sizeof(struct mbuf *)
#else
  #define LPC_ETH_CONFIG_TX_BUF_SIZE 1518U
#endif

#define DEFAULT_PHY 0
#define WATCHDOG_TIMEOUT 5

typedef struct {
  uint32_t start;
  uint32_t control;
} lpc_eth_transfer_descriptor;

typedef struct {
  uint32_t info;
  uint32_t hash_crc;
} lpc_eth_receive_status;

typedef struct {
  uint32_t mac1;
  uint32_t mac2;
  uint32_t ipgt;
  uint32_t ipgr;
  uint32_t clrt;
  uint32_t maxf;
  uint32_t supp;
  uint32_t test;
  uint32_t mcfg;
  uint32_t mcmd;
  uint32_t madr;
  uint32_t mwtd;
  uint32_t mrdd;
  uint32_t mind;
  uint32_t reserved_0 [2];
  uint32_t sa0;
  uint32_t sa1;
  uint32_t sa2;
  uint32_t reserved_1 [45];
  uint32_t command;
  uint32_t status;
  uint32_t rxdescriptor;
  uint32_t rxstatus;
  uint32_t rxdescriptornum;
  uint32_t rxproduceindex;
  uint32_t rxconsumeindex;
  uint32_t txdescriptor;
  uint32_t txstatus;
  uint32_t txdescriptornum;
  uint32_t txproduceindex;
  uint32_t txconsumeindex;
  uint32_t reserved_2 [10];
  uint32_t tsv0;
  uint32_t tsv1;
  uint32_t rsv;
  uint32_t reserved_3 [3];
  uint32_t flowcontrolcnt;
  uint32_t flowcontrolsts;
  uint32_t reserved_4 [34];
  uint32_t rxfilterctrl;
  uint32_t rxfilterwolsts;
  uint32_t rxfilterwolclr;
  uint32_t reserved_5 [1];
  uint32_t hashfilterl;
  uint32_t hashfilterh;
  uint32_t reserved_6 [882];
  uint32_t intstatus;
  uint32_t intenable;
  uint32_t intclear;
  uint32_t intset;
  uint32_t reserved_7 [1];
  uint32_t powerdown;
} lpc_eth_controller;

static volatile lpc_eth_controller *const lpc_eth = 
  (volatile lpc_eth_controller *) LPC_ETH_CONFIG_REG_BASE;

/* ETH_RX_CTRL */

#define ETH_RX_CTRL_SIZE_MASK 0x000007ffU
#define ETH_RX_CTRL_INTERRUPT 0x80000000U

/* ETH_RX_STAT */

#define ETH_RX_STAT_RXSIZE_MASK 0x000007ffU
#define ETH_RX_STAT_BYTES 0x00000100U
#define ETH_RX_STAT_CONTROL_FRAME 0x00040000U
#define ETH_RX_STAT_VLAN 0x00080000U
#define ETH_RX_STAT_FAIL_FILTER 0x00100000U
#define ETH_RX_STAT_MULTICAST 0x00200000U
#define ETH_RX_STAT_BROADCAST 0x00400000U
#define ETH_RX_STAT_CRC_ERROR 0x00800000U
#define ETH_RX_STAT_SYMBOL_ERROR 0x01000000U
#define ETH_RX_STAT_LENGTH_ERROR 0x02000000U
#define ETH_RX_STAT_RANGE_ERROR 0x04000000U
#define ETH_RX_STAT_ALIGNMENT_ERROR 0x08000000U
#define ETH_RX_STAT_OVERRUN 0x10000000U
#define ETH_RX_STAT_NO_DESCRIPTOR 0x20000000U
#define ETH_RX_STAT_LAST_FLAG 0x40000000U
#define ETH_RX_STAT_ERROR 0x80000000U

/* ETH_TX_CTRL */

#define ETH_TX_CTRL_SIZE_MASK 0x7ffU
#define ETH_TX_CTRL_SIZE_SHIFT 0
#define ETH_TX_CTRL_OVERRIDE 0x04000000U
#define ETH_TX_CTRL_HUGE 0x08000000U
#define ETH_TX_CTRL_PAD 0x10000000U
#define ETH_TX_CTRL_CRC 0x20000000U
#define ETH_TX_CTRL_LAST 0x40000000U
#define ETH_TX_CTRL_INTERRUPT 0x80000000U

/* ETH_TX_STAT */

#define ETH_TX_STAT_COLLISION_COUNT_MASK 0x01e00000U
#define ETH_TX_STAT_DEFER 0x02000000U
#define ETH_TX_STAT_EXCESSIVE_DEFER 0x04000000U
#define ETH_TX_STAT_EXCESSIVE_COLLISION 0x08000000U
#define ETH_TX_STAT_LATE_COLLISION 0x10000000U
#define ETH_TX_STAT_UNDERRUN 0x20000000U
#define ETH_TX_STAT_NO_DESCRIPTOR 0x40000000U
#define ETH_TX_STAT_ERROR 0x80000000U

/* ETH_INT */

#define ETH_INT_RX_OVERRUN 0x00000001U
#define ETH_INT_RX_ERROR 0x00000002U
#define ETH_INT_RX_FINISHED 0x00000004U
#define ETH_INT_RX_DONE 0x00000008U
#define ETH_INT_TX_UNDERRUN 0x00000010U
#define ETH_INT_TX_ERROR 0x00000020U
#define ETH_INT_TX_FINISHED 0x00000040U
#define ETH_INT_TX_DONE 0x00000080U
#define ETH_INT_SOFT 0x00001000U
#define ETH_INT_WAKEUP 0x00002000U

/* ETH_RX_FIL_CTRL */

#define ETH_RX_FIL_CTRL_ACCEPT_UNICAST 0x00000001U
#define ETH_RX_FIL_CTRL_ACCEPT_BROADCAST 0x00000002U
#define ETH_RX_FIL_CTRL_ACCEPT_MULTICAST 0x00000004U
#define ETH_RX_FIL_CTRL_ACCEPT_UNICAST_HASH 0x00000008U
#define ETH_RX_FIL_CTRL_ACCEPT_MULTICAST_HASH 0x00000010U
#define ETH_RX_FIL_CTRL_ACCEPT_PERFECT 0x00000020U
#define ETH_RX_FIL_CTRL_MAGIC_PACKET_WOL 0x00001000U
#define ETH_RX_FIL_CTRL_RX_FILTER_WOL 0x00002000U

/* ETH_CMD */

#define ETH_CMD_RX_ENABLE 0x00000001U
#define ETH_CMD_TX_ENABLE 0x00000002U
#define ETH_CMD_REG_RESET 0x00000008U
#define ETH_CMD_TX_RESET 0x00000010U
#define ETH_CMD_RX_RESET 0x00000020U
#define ETH_CMD_PASS_RUNT_FRAME 0x00000040U
#define ETH_CMD_PASS_RX_FILTER 0X00000080U
#define ETH_CMD_TX_FLOW_CONTROL 0x00000100U
#define ETH_CMD_RMII 0x00000200U
#define ETH_CMD_FULL_DUPLEX 0x00000400U

/* ETH_STAT */

#define ETH_STAT_RX_ACTIVE 0x00000001U
#define ETH_STAT_TX_ACTIVE 0x00000002U

/* ETH_MAC2 */

#define ETH_MAC2_FULL_DUPLEX BSP_BIT32(8)

/* ETH_SUPP */

#define ETH_SUPP_SPEED BSP_BIT32(8)

/* ETH_MCFG */

#define ETH_MCFG_CLOCK_SELECT(val) BSP_FLD32(val, 2, 4)

/* ETH_MCMD */

#define ETH_MCMD_READ BSP_BIT32(0)
#define ETH_MCMD_SCAN BSP_BIT32(1)

/* ETH_MADR */

#define ETH_MADR_REG(val) BSP_FLD32(val, 0, 4)
#define ETH_MADR_PHY(val) BSP_FLD32(val, 8, 12)

/* ETH_MIND */

#define ETH_MIND_BUSY BSP_BIT32(0)
#define ETH_MIND_SCANNING BSP_BIT32(1)
#define ETH_MIND_NOT_VALID BSP_BIT32(2)
#define ETH_MIND_MII_LINK_FAIL BSP_BIT32(3)

/* Events */

#define LPC_ETH_EVENT_INITIALIZE RTEMS_EVENT_1

#define LPC_ETH_EVENT_START RTEMS_EVENT_2

#define LPC_ETH_EVENT_INTERRUPT RTEMS_EVENT_3

/* Status */

#define LPC_ETH_INTERRUPT_RECEIVE \
  (ETH_INT_RX_ERROR | ETH_INT_RX_FINISHED | ETH_INT_RX_DONE)

#define LPC_ETH_INTERRUPT_TRANSMIT \
  (ETH_INT_TX_DONE | ETH_INT_TX_FINISHED | ETH_INT_TX_ERROR)

#define LPC_ETH_RX_STAT_ERRORS \
  (ETH_RX_STAT_CRC_ERROR \
    | ETH_RX_STAT_SYMBOL_ERROR \
    | ETH_RX_STAT_LENGTH_ERROR \
    | ETH_RX_STAT_ALIGNMENT_ERROR \
    | ETH_RX_STAT_OVERRUN \
    | ETH_RX_STAT_NO_DESCRIPTOR)

#define LPC_ETH_LAST_FRAGMENT_FLAGS \
  (ETH_TX_CTRL_OVERRIDE \
    | ETH_TX_CTRL_PAD \
    | ETH_TX_CTRL_CRC \
    | ETH_TX_CTRL_INTERRUPT \
    | ETH_TX_CTRL_LAST)

/* Debug */

#ifdef DEBUG
  #define LPC_ETH_PRINTF(...) printf(__VA_ARGS__)
  #define LPC_ETH_PRINTK(...) printk(__VA_ARGS__)
#else
  #define LPC_ETH_PRINTF(...)
  #define LPC_ETH_PRINTK(...)
#endif

typedef enum {
  LPC_ETH_NOT_INITIALIZED,
  LPC_ETH_INITIALIZED,
  LPC_ETH_STARTED,
  LPC_ETH_RUNNING
} lpc_eth_state;

typedef struct {
  struct arpcom arpcom;
  lpc_eth_state state;
  struct rtems_mdio_info mdio;
  uint32_t anlpar;
  rtems_id receive_task;
  rtems_id transmit_task;
  unsigned rx_unit_count;
  unsigned tx_unit_count;
  volatile lpc_eth_transfer_descriptor *rx_desc_table;
  volatile lpc_eth_receive_status *rx_status_table;
  struct mbuf **rx_mbuf_table;
  volatile lpc_eth_transfer_descriptor *tx_desc_table;
  volatile uint32_t *tx_status_table;
  void *tx_buf_table;
  unsigned received_frames;
  unsigned receive_interrupts;
  unsigned transmitted_frames;
  unsigned transmit_interrupts;
  unsigned receive_overrun_errors;
  unsigned receive_fragment_errors;
  unsigned receive_crc_errors;
  unsigned receive_symbol_errors;
  unsigned receive_length_errors;
  unsigned receive_alignment_errors;
  unsigned receive_no_descriptor_errors;
  unsigned receive_fatal_errors;
  unsigned transmit_underrun_errors;
  unsigned transmit_late_collision_errors;
  unsigned transmit_excessive_collision_errors;
  unsigned transmit_excessive_defer_errors;
  unsigned transmit_no_descriptor_errors;
  unsigned transmit_overflow_errors;
  unsigned transmit_fatal_errors;
} lpc_eth_driver_entry;

static lpc_eth_driver_entry lpc_eth_driver_data = {
  .state = LPC_ETH_NOT_INITIALIZED,
  .receive_task = RTEMS_ID_NONE,
  .transmit_task = RTEMS_ID_NONE
};

static inline uint32_t lpc_eth_increment(
  uint32_t value,
  uint32_t cycle
)
{
  if (value < cycle) {
    return ++value;
  } else {
    return 0;
  }
}

static void lpc_eth_enable_promiscous_mode(bool enable)
{
  if (enable) {
    lpc_eth->rxfilterctrl = ETH_RX_FIL_CTRL_ACCEPT_UNICAST
      | ETH_RX_FIL_CTRL_ACCEPT_MULTICAST
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  } else {
    lpc_eth->rxfilterctrl = ETH_RX_FIL_CTRL_ACCEPT_PERFECT
      | ETH_RX_FIL_CTRL_ACCEPT_MULTICAST_HASH
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  }
}

static void lpc_eth_interrupt_handler(void *arg)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;
  rtems_event_set re = 0;
  rtems_event_set te = 0;
  uint32_t ie = 0;

  /* Get interrupt status */
  uint32_t im = lpc_eth->intenable;
  uint32_t is = lpc_eth->intstatus & im;

  /* Check receive interrupts */
  if ((is & ETH_INT_RX_OVERRUN) != 0) {
    re = LPC_ETH_EVENT_INITIALIZE;
    ++e->receive_fatal_errors;
  } else if ((is & LPC_ETH_INTERRUPT_RECEIVE) != 0) {
    re = LPC_ETH_EVENT_INTERRUPT;
    ie |= LPC_ETH_INTERRUPT_RECEIVE;
  }

  /* Send events to receive task */
  if (re != 0) {
    ++e->receive_interrupts;
    (void) rtems_event_send(e->receive_task, re);
  }

  /* Check transmit interrupts */
  if ((is & ETH_INT_TX_UNDERRUN) != 0) {
    te = LPC_ETH_EVENT_INITIALIZE;
    ++e->transmit_fatal_errors;
  } else if ((is & LPC_ETH_INTERRUPT_TRANSMIT) != 0) {
    te = LPC_ETH_EVENT_INTERRUPT;
    ie |= LPC_ETH_INTERRUPT_TRANSMIT;
  }

  /* Send events to transmit task */
  if (te != 0) {
    ++e->transmit_interrupts;
    (void) rtems_event_send(e->transmit_task, te);
  }

  LPC_ETH_PRINTK("interrupt: rx = 0x%08x, tx = 0x%08x\n", re, te);

  /* Update interrupt mask */
  lpc_eth->intenable = im & ~ie;

  /* Clear interrupts */
  lpc_eth->intclear = is;
}

static void lpc_eth_enable_receive_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable |= LPC_ETH_INTERRUPT_RECEIVE;
  rtems_interrupt_enable(level);
}

static void lpc_eth_disable_receive_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable &= ~LPC_ETH_INTERRUPT_RECEIVE;
  rtems_interrupt_enable(level);
}

static void lpc_eth_enable_transmit_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable |= LPC_ETH_INTERRUPT_TRANSMIT;
  rtems_interrupt_enable(level);
}

static void lpc_eth_disable_transmit_interrupts(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc_eth->intenable &= ~LPC_ETH_INTERRUPT_TRANSMIT;
  rtems_interrupt_enable(level);
}

#define LPC_ETH_RX_DATA_OFFSET 2

static struct mbuf *lpc_eth_new_mbuf(struct ifnet *ifp, bool wait)
{
  struct mbuf *m = NULL;
  int mw = wait ? M_WAIT : M_DONTWAIT;

  MGETHDR(m, mw, MT_DATA);
  if (m != NULL) {
    MCLGET(m, mw);
    if ((m->m_flags & M_EXT) != 0) {
      /* Set receive interface */
      m->m_pkthdr.rcvif = ifp;

      /* Adjust by two bytes for proper IP header alignment */
      m->m_data = mtod(m, char *) + LPC_ETH_RX_DATA_OFFSET;

      return m;
    } else {
      m_free(m);
    }
  }

  return NULL;
}

static bool lpc_eth_add_new_mbuf(
  struct ifnet *ifp,
  volatile lpc_eth_transfer_descriptor *desc,
  struct mbuf **mbufs,
  uint32_t i,
  bool wait
)
{
  /* New mbuf */
  struct mbuf *m = lpc_eth_new_mbuf(ifp, wait);

  /* Check mbuf */
  if (m != NULL) {
    /* Cache invalidate */
    rtems_cache_invalidate_multiple_data_lines(
      mtod(m, void *),
      MCLBYTES - LPC_ETH_RX_DATA_OFFSET
    );

    /* Add mbuf to queue */
    desc [i].start = mtod(m, uint32_t);
    desc [i].control = (MCLBYTES - LPC_ETH_RX_DATA_OFFSET - 1)
      | ETH_RX_CTRL_INTERRUPT;

    /* Cache flush of descriptor  */
    rtems_cache_flush_multiple_data_lines(
      (void *) &desc [i],
      sizeof(desc [0])
    );

    /* Add mbuf to table */
    mbufs [i] = m;

    return true;
  } else {
    return false;
  }
}

static void lpc_eth_receive_task(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  lpc_eth_driver_entry *const e = (lpc_eth_driver_entry *) arg;
  struct ifnet *const ifp = &e->arpcom.ac_if;
  volatile lpc_eth_transfer_descriptor *const desc = e->rx_desc_table;
  volatile lpc_eth_receive_status *const status = e->rx_status_table;
  struct mbuf **const mbufs = e->rx_mbuf_table;
  uint32_t const index_max = e->rx_unit_count - 1;
  uint32_t produce_index = 0;
  uint32_t consume_index = 0;
  uint32_t receive_index = 0;

  LPC_ETH_PRINTF("%s\n", __func__);

  /* Main event loop */
  while (true) {
    bool wait_for_mbuf = false;

    /* Wait for events */
    sc = rtems_bsdnet_event_receive(
      LPC_ETH_EVENT_INITIALIZE | LPC_ETH_EVENT_INTERRUPT,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    RTEMS_CLEANUP_SC(sc, cleanup, "wait for events");

    LPC_ETH_PRINTF("rx: wake up: 0x%08" PRIx32 "\n", events);

    /* Initialize receiver? */
    if ((events & LPC_ETH_EVENT_INITIALIZE) != 0) {
      /* Disable receive interrupts */
      lpc_eth_disable_receive_interrupts();

      /* Disable receiver */
      lpc_eth->command &= ~ETH_CMD_RX_ENABLE;

      /* Wait for inactive status */
      while ((lpc_eth->status & ETH_STAT_RX_ACTIVE) != 0) {
        /* Wait */
      }

      /* Reset */
      lpc_eth->command |= ETH_CMD_RX_RESET;

      /* Clear receive interrupts */
      lpc_eth->intclear = LPC_ETH_INTERRUPT_RECEIVE;

      /* Move existing mbufs to the front */
      consume_index = 0;
      for (produce_index = 0; produce_index <= index_max; ++produce_index) {
        if (mbufs [produce_index] != NULL) {
          mbufs [consume_index] = mbufs [produce_index];
          ++consume_index;
        }
      }

      /* Fill receive queue */
      for (
        produce_index = consume_index;
        produce_index <= index_max;
        ++produce_index
      ) {
        lpc_eth_add_new_mbuf(ifp, desc, mbufs, produce_index, true);
      }

      /* Receive descriptor table */
      lpc_eth->rxdescriptornum = index_max;
      lpc_eth->rxdescriptor = (uint32_t) desc;
      lpc_eth->rxstatus = (uint32_t) status;

      /* Initialize indices */
      produce_index = lpc_eth->rxproduceindex;
      consume_index = lpc_eth->rxconsumeindex;
      receive_index = consume_index;

      /* Enable receiver */
      lpc_eth->command |= ETH_CMD_RX_ENABLE;

      /* Enable receive interrupts */
      lpc_eth_enable_receive_interrupts();

      /* Wait for events */
      continue;
    }

    while (true) {
      /* Clear receive interrupt status */
      lpc_eth->intclear = LPC_ETH_INTERRUPT_RECEIVE;

      /* Get current produce index */
      produce_index = lpc_eth->rxproduceindex;

      if (receive_index != produce_index) {
        uint32_t stat = 0;

        /* Fragment mbuf */
        struct mbuf *m = mbufs [receive_index];

        /* Fragment status */
        rtems_cache_invalidate_multiple_data_lines(
          (void *) &status [receive_index],
          sizeof(status [0])
        );
        stat = status [receive_index].info;

        /* Remove mbuf from table */
        mbufs [receive_index] = NULL;

        if (
          (stat & ETH_RX_STAT_LAST_FLAG) != 0
            && (stat & LPC_ETH_RX_STAT_ERRORS) == 0
        ) {
          /* Ethernet header */
          struct ether_header *eh = mtod(m, struct ether_header *);

          /* Discard Ethernet header and CRC */
          int sz = (int) (stat & ETH_RX_STAT_RXSIZE_MASK) + 1
            - ETHER_HDR_LEN - ETHER_CRC_LEN;

          /* Update mbuf */
          m->m_len = sz;
          m->m_pkthdr.len = sz;
          m->m_data = mtod(m, char *) + ETHER_HDR_LEN;

          LPC_ETH_PRINTF("rx: %02" PRIu32 ": %u\n", receive_index, sz);

          /* Hand over */
          ether_input(ifp, eh, m);

          /* Increment received frames counter */
          ++e->received_frames;
        } else {
          /* Release mbuf */
          m_free(m);

          /* Update error counters */
          if ((stat & ETH_RX_STAT_OVERRUN) != 0) {
            ++e->receive_overrun_errors;
          }
          if ((stat & ETH_RX_STAT_LAST_FLAG) == 0) {
            ++e->receive_fragment_errors;
          }
          if ((stat & ETH_RX_STAT_CRC_ERROR) != 0) {
            ++e->receive_crc_errors;
          }
          if ((stat & ETH_RX_STAT_SYMBOL_ERROR) != 0) {
            ++e->receive_symbol_errors;
          }
          if ((stat & ETH_RX_STAT_LENGTH_ERROR) != 0) {
            ++e->receive_length_errors;
          }
          if ((stat & ETH_RX_STAT_ALIGNMENT_ERROR) != 0) {
            ++e->receive_alignment_errors;
          }
          if ((stat & ETH_RX_STAT_NO_DESCRIPTOR) != 0) {
            ++e->receive_no_descriptor_errors;
          }
        }

        /* Increment receive index */
        receive_index = lpc_eth_increment(receive_index, index_max);
      } else {
        /* Nothing to do, enable receive interrupts */
        lpc_eth_enable_receive_interrupts();
        break;
      }
    }

    /* Wait for mbuf? */
    wait_for_mbuf =
      lpc_eth_increment(produce_index, index_max) == consume_index;

    /* Fill queue with new mbufs */
    while (consume_index != produce_index) {
      /* Add new mbuf to queue */
      if (
        !lpc_eth_add_new_mbuf(ifp, desc, mbufs, consume_index, wait_for_mbuf)
      ) {
        break;
      }

      /* We wait for at most one mbuf */
      wait_for_mbuf = false;

      /* Increment consume index */
      consume_index = lpc_eth_increment(consume_index, index_max);

      /* Update consume indices */
      lpc_eth->rxconsumeindex = consume_index;
    }
  }

cleanup:

  /* Clear task ID */
  e->receive_task = RTEMS_ID_NONE;

  /* Release network semaphore */
  rtems_bsdnet_semaphore_release();

  /* Terminate self */
  (void) rtems_task_delete(RTEMS_SELF);
}

static struct mbuf *lpc_eth_next_fragment(
  struct ifnet *ifp,
  struct mbuf *m,
  uint32_t *ctrl
)
{
  struct mbuf *n = NULL;
  int size = 0;

  while (true) {
    if (m == NULL) {
      /* Dequeue first fragment of the next frame */
      IF_DEQUEUE(&ifp->if_snd, m);

      /* Empty queue? */
      if (m == NULL) {
        return m;
      }
    }

    /* Get fragment size */
    size = m->m_len;

    if (size > 0) {
      /* Now we have a not empty fragment */
      break;
    } else {
      /* Discard empty fragments */
      m = m_free(m);
    }
  }

  /* Set fragment size */
  *ctrl = (uint32_t) (size - 1);

  /* Discard empty successive fragments */
  n = m->m_next;
  while (n != NULL && n->m_len <= 0) {
    n = m_free(n);
  }
  m->m_next = n;

  /* Is our fragment the last in the frame? */
  if (n == NULL) {
    *ctrl |= LPC_ETH_LAST_FRAGMENT_FLAGS;
  }

  return m;
}

static void lpc_eth_transmit_task(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;
  volatile lpc_eth_transfer_descriptor *const desc = e->tx_desc_table;
  volatile uint32_t *const status = e->tx_status_table;
  #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    struct mbuf **const mbufs = e->tx_buf_table;
  #else
    char *const buf = e->tx_buf_table;
  #endif
  struct mbuf *m = NULL;
  uint32_t const index_max = e->tx_unit_count - 1;
  uint32_t produce_index = 0;
  uint32_t consume_index = 0;
  uint32_t ctrl = 0;
  #ifndef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    uint32_t frame_length = 0;
    char *frame_buffer = NULL;
  #endif

  LPC_ETH_PRINTF("%s\n", __func__);

  #ifndef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
    /* Initialize descriptor table */
    for (produce_index = 0; produce_index <= index_max; ++produce_index) {
      desc [produce_index].start =
        (uint32_t) (buf + produce_index * LPC_ETH_CONFIG_TX_BUF_SIZE);
    }
  #endif

  /* Main event loop */
  while (true) {
    /* Wait for events */
    sc = rtems_bsdnet_event_receive(
      LPC_ETH_EVENT_INITIALIZE
        | LPC_ETH_EVENT_START
        | LPC_ETH_EVENT_INTERRUPT,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    RTEMS_CLEANUP_SC(sc, cleanup, "wait for events");

    LPC_ETH_PRINTF("tx: wake up: 0x%08" PRIx32 "\n", events);

    /* Initialize transmitter? */
    if ((events & LPC_ETH_EVENT_INITIALIZE) != 0) {
      /* Disable transmit interrupts */
      lpc_eth_disable_transmit_interrupts();

      /* Disable transmitter */
      lpc_eth->command &= ~ETH_CMD_TX_ENABLE;

      /* Wait for inactive status */
      while ((lpc_eth->status & ETH_STAT_TX_ACTIVE) != 0) {
        /* Wait */
      }

      /* Reset */
      lpc_eth->command |= ETH_CMD_TX_RESET;

      /* Clear transmit interrupts */
      lpc_eth->intclear = LPC_ETH_INTERRUPT_TRANSMIT;

      /* Transmit descriptors */
      lpc_eth->txdescriptornum = index_max;
      lpc_eth->txdescriptor = (uint32_t) desc;
      lpc_eth->txstatus = (uint32_t) status;

      #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
        /* Discard outstanding fragments (= data loss) */
        for (produce_index = 0; produce_index <= index_max; ++produce_index) {
          struct mbuf *victim = mbufs [produce_index];

          if (victim != NULL) {
            m_free(victim);
            mbufs [produce_index] = NULL;
          }
        }
      #endif

      /* Initialize indices */
      produce_index = lpc_eth->txproduceindex;
      consume_index = lpc_eth->txconsumeindex;

      #ifndef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
        /* Fresh frame length and buffer start */
        frame_length = 0;
        frame_buffer = (char *) desc [produce_index].start;
      #endif

      /* Enable transmitter */
      lpc_eth->command |= ETH_CMD_TX_ENABLE;
    }

    /* Free consumed fragments */
    while (true) {
      /* Save last known consume index */
      uint32_t c = consume_index;

      /* Clear transmit interrupt status */
      lpc_eth->intclear = LPC_ETH_INTERRUPT_TRANSMIT;

      /* Get new consume index */
      consume_index = lpc_eth->txconsumeindex;

      /* Nothing consumed in the meantime? */
      if (c == consume_index) {
        break;
      }

      while (c != consume_index) {
        uint32_t s = status [c];

        /* Update error counters */
        if ((s & (ETH_TX_STAT_ERROR | ETH_TX_STAT_NO_DESCRIPTOR)) != 0) {
          if ((s & ETH_TX_STAT_UNDERRUN) != 0) {
            ++e->transmit_underrun_errors;
          }
          if ((s & ETH_TX_STAT_LATE_COLLISION) != 0) {
            ++e->transmit_late_collision_errors;
          }
          if ((s & ETH_TX_STAT_EXCESSIVE_COLLISION) != 0) {
            ++e->transmit_excessive_collision_errors;
          }
          if ((s & ETH_TX_STAT_EXCESSIVE_DEFER) != 0) {
            ++e->transmit_excessive_defer_errors;
          }
          if ((s & ETH_TX_STAT_NO_DESCRIPTOR) != 0) {
            ++e->transmit_no_descriptor_errors;
          }
        }

        #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
          /* Release mbuf */
          m_free(mbufs [c]);
          mbufs [c] = NULL;
        #endif

        /* Next consume index */
        c = lpc_eth_increment(c, index_max);
      }
    }

    /* Transmit new fragments */
    while (true) {
      /* Compute next produce index */
      uint32_t p = lpc_eth_increment(produce_index, index_max);

      /* Get next fragment and control value */
      m = lpc_eth_next_fragment(ifp, m, &ctrl);

      /* Queue full? */
      if (p == consume_index) {
        LPC_ETH_PRINTF("tx: full queue: 0x%08x\n", m);

        /* The queue is full, wait for transmit interrupt */
        break;
      }

      /* New fragment? */
      if (m != NULL) {
        #ifdef LPC_ETH_CONFIG_USE_TRANSMIT_DMA
          /* Set the transfer data */
          rtems_cache_flush_multiple_data_lines(
            mtod(m, const void *),
            (size_t) m->m_len
          );
          desc [produce_index].start = mtod(m, uint32_t);
          desc [produce_index].control = ctrl;
          rtems_cache_flush_multiple_data_lines(
            (void *) &desc [produce_index],
            sizeof(desc [0])
           );
          mbufs [produce_index] = m;

          LPC_ETH_PRINTF(
            "tx: %02" PRIu32 ": %u %s\n",
            produce_index, m->m_len,
            (ctrl & ETH_TX_CTRL_LAST) != 0 ? "L" : ""
          );

          /* Next produce index */
          produce_index = p;

          /* Last fragment of a frame? */
          if ((ctrl & ETH_TX_CTRL_LAST) != 0) {
            /* Update the produce index */
            lpc_eth->txproduceindex = produce_index;

            /* Increment transmitted frames counter */
            ++e->transmitted_frames;
          }

          /* Next fragment of the frame */
          m = m->m_next;
        #else
          size_t fragment_length = (size_t) m->m_len;
          void *fragment_start = mtod(m, void *);
          uint32_t new_frame_length = frame_length + fragment_length;

          /* Check buffer size */
          if (new_frame_length > LPC_ETH_CONFIG_TX_BUF_SIZE) {
            LPC_ETH_PRINTF("tx: overflow\n");

            /* Discard overflow data */
            new_frame_length = LPC_ETH_CONFIG_TX_BUF_SIZE;
            fragment_length = new_frame_length - frame_length;

            /* Finalize frame */
            ctrl |= LPC_ETH_LAST_FRAGMENT_FLAGS;

            /* Update error counter */
            ++e->transmit_overflow_errors;
          }

          LPC_ETH_PRINTF(
            "tx: copy: %" PRIu32 "%s%s\n",
            fragment_length,
            (m->m_flags & M_EXT) != 0 ? ", E" : "",
            (m->m_flags & M_PKTHDR) != 0 ? ", H" : ""
          );

          /* Copy fragment to buffer in Ethernet RAM */
          memcpy(frame_buffer, fragment_start, fragment_length);

          if ((ctrl & ETH_TX_CTRL_LAST) != 0) {
            /* Finalize descriptor */
            desc [produce_index].control = (ctrl & ~ETH_TX_CTRL_SIZE_MASK)
              | (new_frame_length - 1);

            LPC_ETH_PRINTF(
              "tx: %02" PRIu32 ": %" PRIu32 "\n",
              produce_index,
              new_frame_length
            );

            /* Cache flush of data */
            rtems_cache_flush_multiple_data_lines(
              (const void *) desc [produce_index].start,
              new_frame_length
            );

            /* Cache flush of descriptor  */
            rtems_cache_flush_multiple_data_lines(
              (void *) &desc [produce_index],
              sizeof(desc [0])
            );

            /* Next produce index */
            produce_index = p;

            /* Update the produce index */
            lpc_eth->txproduceindex = produce_index;

            /* Fresh frame length and buffer start */
            frame_length = 0;
            frame_buffer = (char *) desc [produce_index].start;

            /* Increment transmitted frames counter */
            ++e->transmitted_frames;
          } else {
            /* New frame length */
            frame_length = new_frame_length;

            /* Update current frame buffer start */
            frame_buffer += fragment_length;
          }

          /* Free mbuf and get next */
          m = m_free(m);
        #endif
      } else {
        /* Nothing to transmit */
        break;
      }
    }

    /* No more fragments? */
    if (m == NULL) {
      /* Interface is now inactive */
      ifp->if_flags &= ~IFF_OACTIVE;
    } else {
      LPC_ETH_PRINTF("tx: enable interrupts\n");

      /* Enable transmit interrupts */
      lpc_eth_enable_transmit_interrupts();
    }
  }

cleanup:

  /* Clear task ID */
  e->transmit_task = RTEMS_ID_NONE;

  /* Release network semaphore */
  rtems_bsdnet_semaphore_release();

  /* Terminate self */
  (void) rtems_task_delete(RTEMS_SELF);
}

static void lpc_eth_mdio_wait_for_not_busy(void)
{
  while ((lpc_eth->mind & ETH_MIND_BUSY) != 0) {
    rtems_task_wake_after(2);
  }
}

static uint32_t lpc_eth_mdio_read_anlpar(void)
{
  uint32_t madr = ETH_MADR_REG(MII_ANLPAR) | ETH_MADR_PHY(DEFAULT_PHY);
  uint32_t anlpar = 0;

  if (lpc_eth->madr != madr) {
    lpc_eth->madr = madr;
  }

  if (lpc_eth->mcmd != ETH_MCMD_READ) {
    lpc_eth->mcmd = 0;
    lpc_eth->mcmd = ETH_MCMD_READ;
  }

  lpc_eth_mdio_wait_for_not_busy();

  anlpar = lpc_eth->mrdd;

  /* Start next read */
  lpc_eth->mcmd = 0;
  lpc_eth->mcmd = ETH_MCMD_READ;

  return anlpar;
}

static int lpc_eth_mdio_read(
  int phy __attribute__((unused)),
  void *arg __attribute__((unused)),
  unsigned reg,
  uint32_t *val
)
{
  int eno = 0;

  if (phy == -1 || phy == 0) {
    lpc_eth->madr = ETH_MADR_REG(reg) | ETH_MADR_PHY(DEFAULT_PHY);
    lpc_eth->mcmd = 0;
    lpc_eth->mcmd = ETH_MCMD_READ;
    lpc_eth_mdio_wait_for_not_busy();
    *val = lpc_eth->mrdd;
  } else {
    eno = EINVAL;
  }

  return eno;
}

static int lpc_eth_mdio_write(
  int phy __attribute__((unused)),
  void *arg __attribute__((unused)),
  unsigned reg,
  uint32_t val
)
{
  int eno = 0;

  if (phy == -1 || phy == 0) {
    lpc_eth->madr = ETH_MADR_REG(reg) | ETH_MADR_PHY(DEFAULT_PHY);
    lpc_eth->mwtd = val;
    lpc_eth_mdio_wait_for_not_busy();
  } else {
    eno = EINVAL;
  }

  return eno;
}

static void lpc_eth_interface_init(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;

  LPC_ETH_PRINTF("%s\n", __func__);

  if (e->state == LPC_ETH_INITIALIZED) {
    lpc_eth_config_module_enable();

    /* Soft reset */

    /* Do soft reset */
    lpc_eth->command = 0x38;
    lpc_eth->mac1 = 0xcf00;
    lpc_eth->mac1 = 0x0;

    /* Initialize PHY */
    lpc_eth->mcfg = ETH_MCFG_CLOCK_SELECT(0x7);
    /* TODO */

    /* Reinitialize registers */
    lpc_eth->mac2 = 0x31;
    lpc_eth->ipgt = 0x15;
    lpc_eth->ipgr = 0x12;
    lpc_eth->clrt = 0x370f;
    lpc_eth->maxf = 0x0600;
    lpc_eth->supp = ETH_SUPP_SPEED;
    lpc_eth->test = 0;
    #ifdef LPC_ETH_CONFIG_RMII
      lpc_eth->command = 0x0600;
    #else
      lpc_eth->command = 0x0400;
    #endif
    lpc_eth->intenable = 0;
    lpc_eth->intclear = 0x30ff;
    lpc_eth->powerdown = 0;

    /* MAC address */
    lpc_eth->sa0 = ((uint32_t) e->arpcom.ac_enaddr [5] << 8)
      | (uint32_t) e->arpcom.ac_enaddr [4];
    lpc_eth->sa1 = ((uint32_t) e->arpcom.ac_enaddr [3] << 8)
      | (uint32_t) e->arpcom.ac_enaddr [2];
    lpc_eth->sa2 = ((uint32_t) e->arpcom.ac_enaddr [1] << 8)
      | (uint32_t) e->arpcom.ac_enaddr [0];

    /* Enable receiver */
    lpc_eth->mac1 = 0x03;

    /* Start receive task */
    if (e->receive_task == RTEMS_ID_NONE) {
      e->receive_task = rtems_bsdnet_newproc(
        "ntrx",
        4096,
        lpc_eth_receive_task,
        e
      );
      sc = rtems_event_send(e->receive_task, LPC_ETH_EVENT_INITIALIZE);
      RTEMS_SYSLOG_ERROR_SC(sc, "send receive initialize event");
    }

    /* Start transmit task */
    if (e->transmit_task == RTEMS_ID_NONE) {
      e->transmit_task = rtems_bsdnet_newproc(
        "nttx",
        4096,
        lpc_eth_transmit_task,
        e
      );
      sc = rtems_event_send(e->transmit_task, LPC_ETH_EVENT_INITIALIZE);
      RTEMS_SYSLOG_ERROR_SC(sc, "send transmit initialize event");
    }

    /* Change state */
    if (
      e->receive_task != RTEMS_ID_NONE && e->transmit_task != RTEMS_ID_NONE
    ) {
      e->state = LPC_ETH_STARTED;
    }
  }

  if (e->state == LPC_ETH_STARTED) {
    /* Enable fatal interrupts */
    lpc_eth->intenable = ETH_INT_RX_OVERRUN | ETH_INT_TX_UNDERRUN;

    /* Enable promiscous mode */
    lpc_eth_enable_promiscous_mode((ifp->if_flags & IFF_PROMISC) != 0);

    /* Start watchdog timer */
    ifp->if_timer = 1;

    /* Set interface to running state */
    ifp->if_flags |= IFF_RUNNING;

    /* Change state */
    e->state = LPC_ETH_RUNNING;
  }
}

static void lpc_eth_interface_stats(lpc_eth_driver_entry *e)
{
  int media = IFM_MAKEWORD(0, 0, 0, 0);
  int eno = rtems_mii_ioctl(&e->mdio, e, SIOCGIFMEDIA, &media);

  rtems_bsdnet_semaphore_release();

  if (eno == 0) {
    rtems_ifmedia2str(media, NULL, 0);
    printf("\n");
  }

  printf("received frames:                     %u\n", e->received_frames);
  printf("receive interrupts:                  %u\n", e->receive_interrupts);
  printf("transmitted frames:                  %u\n", e->transmitted_frames);
  printf("transmit interrupts:                 %u\n", e->transmit_interrupts);
  printf("receive overrun errors:              %u\n", e->receive_overrun_errors);
  printf("receive fragment errors:             %u\n", e->receive_fragment_errors);
  printf("receive CRC errors:                  %u\n", e->receive_crc_errors);
  printf("receive symbol errors:               %u\n", e->receive_symbol_errors);
  printf("receive length errors:               %u\n", e->receive_length_errors);
  printf("receive alignment errors:            %u\n", e->receive_alignment_errors);
  printf("receive no descriptor errors:        %u\n", e->receive_no_descriptor_errors);
  printf("receive fatal errors:                %u\n", e->receive_fatal_errors);
  printf("transmit underrun errors:            %u\n", e->transmit_underrun_errors);
  printf("transmit late collision errors:      %u\n", e->transmit_late_collision_errors);
  printf("transmit excessive collision errors: %u\n", e->transmit_excessive_collision_errors);
  printf("transmit excessive defer errors:     %u\n", e->transmit_excessive_defer_errors);
  printf("transmit no descriptor errors:       %u\n", e->transmit_no_descriptor_errors);
  printf("transmit overflow errors:            %u\n", e->transmit_overflow_errors);
  printf("transmit fatal errors:               %u\n", e->transmit_fatal_errors);

  rtems_bsdnet_semaphore_obtain();
}

static int lpc_eth_multicast_control(
  bool add,
  struct ifreq *ifr,
  struct arpcom *ac
)
{
  int eno = 0;

  if (add) {
    eno = ether_addmulti(ifr, ac);
  } else {
    eno = ether_delmulti(ifr, ac);
  }

  if (eno == ENETRESET) {
    struct ether_multistep step;
    struct ether_multi *enm;

    eno = 0;

    lpc_eth->hashfilterl = 0;
    lpc_eth->hashfilterh = 0;

    ETHER_FIRST_MULTI(step, ac, enm);
    while (enm != NULL) {
      uint64_t addrlo = 0;
      uint64_t addrhi = 0;

      memcpy(&addrlo, enm->enm_addrlo, ETHER_ADDR_LEN);
      memcpy(&addrhi, enm->enm_addrhi, ETHER_ADDR_LEN);
      while (addrlo <= addrhi) {
        /* XXX: ether_crc32_le() does not work, why? */
        uint32_t crc = ether_crc32_be((uint8_t *) &addrlo, ETHER_ADDR_LEN);
        uint32_t index = (crc >> 23) & 0x3f;

        if (index < 32) {
          lpc_eth->hashfilterl |= 1U << index;
        } else {
          lpc_eth->hashfilterh |= 1U << (index - 32);
        }
        ++addrlo;
      }
      ETHER_NEXT_MULTI(step, enm);
    }
  }

  return eno;
}

static int lpc_eth_interface_ioctl(
  struct ifnet *ifp,
  ioctl_command_t cmd,
  caddr_t data
)
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) ifp->if_softc;
  struct ifreq *ifr = (struct ifreq *) data;
  int eno = 0;

  LPC_ETH_PRINTF("%s\n", __func__);

  switch (cmd)  {
    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      rtems_mii_ioctl(&e->mdio, e, cmd, (int *) data);
      break;
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, cmd, data);
      break;
    case SIOCSIFFLAGS:
      if (ifp->if_flags & IFF_RUNNING) {
        /* TODO: off */
      }
      if (ifp->if_flags & IFF_UP) {
        ifp->if_flags |= IFF_RUNNING;
        /* TODO: init */
      }
      break;
    case SIOCADDMULTI:
    case SIOCDELMULTI:
      eno = lpc_eth_multicast_control(cmd == SIOCADDMULTI, ifr, &e->arpcom);
      break;
    case SIO_RTEMS_SHOW_STATS:
      lpc_eth_interface_stats(e);
      break;
    default:
      eno = EINVAL;
      break;
  }

  return eno;
}

static void lpc_eth_interface_start(struct ifnet *ifp)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  sc = rtems_event_send(e->transmit_task, LPC_ETH_EVENT_START);
  RTEMS_SYSLOG_ERROR_SC(sc, "send transmit start event");
}

static void lpc_eth_interface_watchdog(struct ifnet *ifp __attribute__((unused)))
{
  lpc_eth_driver_entry *e = (lpc_eth_driver_entry *) ifp->if_softc;
  uint32_t anlpar = lpc_eth_mdio_read_anlpar();

  if (e->anlpar != anlpar) {
    bool full_duplex = false;
    bool speed = false;

    e->anlpar = anlpar;

    if ((anlpar & ANLPAR_TX_FD) != 0) {
      full_duplex = true;
      speed = true;
    } else if ((anlpar & ANLPAR_T4) != 0) {
      speed = true;
    } else if ((anlpar & ANLPAR_TX) != 0) {
      speed = true;
    } else if ((anlpar & ANLPAR_10_FD) != 0) {
      full_duplex = true;
    }

    if (full_duplex) {
      lpc_eth->mac2 |= ETH_MAC2_FULL_DUPLEX;
    } else {
      lpc_eth->mac2 &= ~ETH_MAC2_FULL_DUPLEX;
    }

    if (speed) {
      lpc_eth->supp |= ETH_SUPP_SPEED;
    } else {
      lpc_eth->supp &= ~ETH_SUPP_SPEED;
    }
  }

  ifp->if_timer = WATCHDOG_TIMEOUT;
}

static unsigned lpc_eth_fixup_unit_count(int count, int default_value, int max)
{
  if (count <= 0) {
    count = default_value;
  } else if (count > max) {
    count = max;
  }

  return LPC_ETH_CONFIG_UNIT_MULTIPLE
    + (((unsigned) count - 1U) & ~(LPC_ETH_CONFIG_UNIT_MULTIPLE - 1U));
}

static int lpc_eth_attach(struct rtems_bsdnet_ifconfig *config)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc_eth_driver_entry *e = &lpc_eth_driver_data;
  struct ifnet *ifp = &e->arpcom.ac_if;
  char *unit_name = NULL;
  int unit_index = rtems_bsdnet_parse_driver_name(config, &unit_name);
  size_t table_area_size = 0;
  char *table_area = NULL;
  char *table_location = NULL;

  /* Check parameter */
  if (unit_index < 0) {
    RTEMS_SYSLOG_ERROR("parse error for interface name\n");
    return 0;
  }
  if (unit_index != 0) {
    RTEMS_DO_CLEANUP(cleanup, "unexpected unit number");
  }
  if (config->hardware_address == NULL) {
    RTEMS_DO_CLEANUP(cleanup, "MAC address missing");
  }
  if (e->state != LPC_ETH_NOT_INITIALIZED) {
    RTEMS_DO_CLEANUP(cleanup, "already attached");
  }

  /* MDIO */
  e->mdio.mdio_r = lpc_eth_mdio_read;
  e->mdio.mdio_w = lpc_eth_mdio_write;
  e->mdio.has_gmii = 0;
  e->anlpar = 0;

  /* Interrupt number */
  config->irno = LPC_ETH_CONFIG_INTERRUPT;

  /* Device control */
  config->drv_ctrl = e;

  /* Receive unit count */
  e->rx_unit_count = lpc_eth_fixup_unit_count(
    config->rbuf_count,
    LPC_ETH_CONFIG_RX_UNIT_COUNT_DEFAULT,
    LPC_ETH_CONFIG_RX_UNIT_COUNT_MAX
  );
  config->rbuf_count = (int) e->rx_unit_count;

  /* Transmit unit count */
  e->tx_unit_count = lpc_eth_fixup_unit_count(
    config->xbuf_count,
    LPC_ETH_CONFIG_TX_UNIT_COUNT_DEFAULT,
    LPC_ETH_CONFIG_TX_UNIT_COUNT_MAX
  );
  config->xbuf_count = (int) e->tx_unit_count;

  /* Disable interrupts */
  lpc_eth->intenable = 0;

  /* Install interrupt handler */
  sc = rtems_interrupt_handler_install(
    config->irno,
    "Ethernet",
    RTEMS_INTERRUPT_UNIQUE,
    lpc_eth_interrupt_handler,
    e
  );
  RTEMS_CLEANUP_SC(sc, cleanup, "install interrupt handler");

  /* Copy MAC address */
  memcpy(e->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

  /* Allocate and clear table area */
  table_area_size =
    e->rx_unit_count
      * (sizeof(lpc_eth_transfer_descriptor)
        + sizeof(lpc_eth_receive_status)
        + sizeof(struct mbuf *))
    + e->tx_unit_count
      * (sizeof(lpc_eth_transfer_descriptor)
        + sizeof(uint32_t)
        + LPC_ETH_CONFIG_TX_BUF_SIZE);
  table_area = lpc_eth_config_alloc_table_area(table_area_size);
  if (table_area == NULL) {
    RTEMS_DO_CLEANUP(cleanup, "no memory for table area");
  }
  memset(table_area, 0, table_area_size);

  table_location = table_area;

  /*
   * The receive status table must be the first one since it has the strictest
   * alignment requirements.
   */
  e->rx_status_table = (volatile lpc_eth_receive_status *) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_status_table [0]);

  e->rx_desc_table = (volatile lpc_eth_transfer_descriptor *) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_desc_table [0]);

  e->rx_mbuf_table = (struct mbuf **) table_location;
  table_location += e->rx_unit_count * sizeof(e->rx_mbuf_table [0]);

  e->tx_desc_table = (volatile lpc_eth_transfer_descriptor *) table_location;
  table_location += e->tx_unit_count * sizeof(e->tx_desc_table [0]);

  e->tx_status_table = (volatile uint32_t *) table_location;
  table_location += e->tx_unit_count * sizeof(e->tx_status_table [0]);

  e->tx_buf_table = table_location;

  /* Set interface data */
  ifp->if_softc = e;
  ifp->if_unit = (short) unit_index;
  ifp->if_name = unit_name;
  ifp->if_mtu = (config->mtu > 0) ? (u_long) config->mtu : ETHERMTU;
  ifp->if_init = lpc_eth_interface_init;
  ifp->if_ioctl = lpc_eth_interface_ioctl;
  ifp->if_start = lpc_eth_interface_start;
  ifp->if_output = ether_output;
  ifp->if_watchdog = lpc_eth_interface_watchdog;
  ifp->if_flags = IFF_MULTICAST | IFF_BROADCAST | IFF_SIMPLEX;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;
  ifp->if_timer = 0;

  /* Change status */
  e->state = LPC_ETH_INITIALIZED;

  /* Attach the interface */
  if_attach(ifp);
  ether_ifattach(ifp);

  return 1;

cleanup:

  lpc_eth_config_free_table_area(table_area);

  /* FIXME: Type */
  free(unit_name, (int) 0xdeadbeef);

  return 0;
}

static int lpc_eth_detach(
  struct rtems_bsdnet_ifconfig *config __attribute__((unused))
)
{
  /* FIXME: Detach the interface from the upper layers? */

  /* Module soft reset */
  lpc_eth->command = 0x38;
  lpc_eth->mac1 = 0xcf00;

  /* FIXME: More cleanup */

  return 0;
}

int lpc_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
)
{
  /* FIXME: Return value */

  if (attaching) {
    return lpc_eth_attach(config);
  } else {
    return lpc_eth_detach(config);
  }
}
