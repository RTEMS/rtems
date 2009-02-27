/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Network driver.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

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
#include <bsp/lpc24xx.h>
#include <bsp/irq.h>
#include <bsp/utility.h>

#include <rtems/status-checks.h>

#if MCLBYTES > (2 * 1024)
  #error "MCLBYTES to large"
#endif

#define LPC24XX_ETH_INTERFACE_NUMBER 1

/**
 * @brief Start address of the Ethernet RAM.
 */
#define LPC24XX_ETH_RAM_START 0x7fe00000U

/**
 * @brief Size of the Ethernet RAM.
 */
#define LPC24XX_ETH_RAM_SIZE (16 * 1024)

#define LPC24XX_ETH_RAM_END \
  (LPC24XX_ETH_RAM_START + LPC24XX_ETH_RAM_SIZE)

#define LPC24XX_ETH_TRANSMIT_BUFFER_SIZE 1518

#define LPC24XX_ETH_RECEIVE_UNIT_SIZE \
  (ETH_TRANSFER_DESCRIPTOR_SIZE \
    + ETH_RECEIVE_INFO_SIZE \
    + 4)

#define LPC24XX_ETH_TRANSMIT_UNIT_SIZE \
  (ETH_TRANSFER_DESCRIPTOR_SIZE \
    + ETH_TRANSMIT_STATUS_SIZE \
    + LPC24XX_ETH_TRANSMIT_BUFFER_SIZE)

#define LPC24XX_ETH_RECEIVE_UNIT_NUMBER 54

#define LPC24XX_ETH_TRANSMIT_UNIT_NUMBER 10

#define LPC24XX_ETH_RECEIVE_DATA_SIZE \
  (LPC24XX_ETH_RECEIVE_UNIT_NUMBER \
    * LPC24XX_ETH_RECEIVE_UNIT_SIZE)

#define LPC24XX_ETH_RECEIVE_DATA_START \
  LPC24XX_ETH_RAM_START

#define LPC24XX_ETH_RECEIVE_DATA_END \
  (LPC24XX_ETH_RECEIVE_DATA_START \
    + LPC24XX_ETH_RECEIVE_DATA_SIZE)

#define LPC24XX_ETH_TRANSMIT_DATA_SIZE \
  (LPC24XX_ETH_TRANSMIT_UNIT_NUMBER \
    * LPC24XX_ETH_TRANSMIT_UNIT_SIZE)

#if 1

#define LPC24XX_ETH_TRANSMIT_DATA_START \
  LPC24XX_ETH_RECEIVE_DATA_END

#else

static char lpc24xx_eth_transmit_buffer [LPC24XX_ETH_TRANSMIT_DATA_SIZE];

#define LPC24XX_ETH_TRANSMIT_DATA_START \
  lpc24xx_eth_transmit_buffer

#endif

#define LPC24XX_ETH_TRANSMIT_DATA_END \
  (LPC24XX_ETH_TRANSMIT_DATA_START \
    + LPC24XX_ETH_TRANSMIT_DATA_SIZE)

#if LPC24XX_ETH_TRANSMIT_DATA_END > LPC24XX_ETH_RAM_END && 0
  #error "Ethernet RAM overflow"
#endif

#define LPC24XX_ETH_RECEIVE_DESC_START \
  LPC24XX_ETH_RECEIVE_DATA_START

#define LPC24XX_ETH_RECEIVE_INFO_START \
  (LPC24XX_ETH_RECEIVE_DESC_START \
    + LPC24XX_ETH_RECEIVE_UNIT_NUMBER * ETH_TRANSFER_DESCRIPTOR_SIZE)

#define LPC24XX_ETH_RECEIVE_MBUF_START \
  (LPC24XX_ETH_RECEIVE_INFO_START \
    + LPC24XX_ETH_RECEIVE_UNIT_NUMBER * ETH_RECEIVE_INFO_SIZE)

#define LPC24XX_ETH_TRANSMIT_DESC_START \
  LPC24XX_ETH_TRANSMIT_DATA_START

#define LPC24XX_ETH_TRANSMIT_STATUS_START \
  (LPC24XX_ETH_TRANSMIT_DATA_START \
    + LPC24XX_ETH_TRANSMIT_UNIT_NUMBER * ETH_TRANSFER_DESCRIPTOR_SIZE)

#define LPC24XX_ETH_TRANSMIT_BUFFER_START \
  (LPC24XX_ETH_TRANSMIT_STATUS_START \
    + LPC24XX_ETH_TRANSMIT_UNIT_NUMBER * ETH_TRANSMIT_STATUS_SIZE)

#define LPC24XX_ETH_EVENT_TRANSMIT RTEMS_EVENT_1

#define LPC24XX_ETH_EVENT_TRANSMIT_START RTEMS_EVENT_2

#define LPC24XX_ETH_EVENT_TRANSMIT_ERROR RTEMS_EVENT_3

#define LPC24XX_ETH_EVENT_RECEIVE RTEMS_EVENT_4

#define LPC24XX_ETH_EVENT_RECEIVE_ERROR RTEMS_EVENT_5

#define LPC24XX_ETH_TIMEOUT 10

#define LPC24XX_ETH_INTERRUPT_RECEIVE \
  (ETH_INT_RX_ERROR | ETH_INT_RX_FINISHED | ETH_INT_RX_DONE)

#define LPC24XX_ETH_INTERRUPT_TRANSMIT (ETH_INT_TX_DONE | ETH_INT_TX_FINISHED | ETH_INT_TX_ERROR)

#define LPC24XX_ETH_RX_STAT_ERRORS \
  (ETH_RX_STAT_CRC_ERROR \
    | ETH_RX_STAT_SYMBOL_ERROR \
    | ETH_RX_STAT_LENGTH_ERROR \
    | ETH_RX_STAT_ALIGNMENT_ERROR \
    | ETH_RX_STAT_OVERRUN \
    | ETH_RX_STAT_NO_DESCRIPTOR)

#define LPC24XX_ETH_LAST_FRAGMENT_FLAGS \
  (ETH_TX_CTRL_OVERRIDE \
    | ETH_TX_CTRL_PAD \
    | ETH_TX_CTRL_CRC \
    | ETH_TX_CTRL_INTERRUPT \
    | ETH_TX_CTRL_LAST)

#ifdef DEBUG
  #define LPC24XX_ETH_PRINTF( ...) printf( __VA_ARGS__)
  #define LPC24XX_ETH_PRINTK( ...) printk( __VA_ARGS__)
#else
  #define LPC24XX_ETH_PRINTF( ...)
  #define LPC24XX_ETH_PRINTK( ...)
#endif

typedef enum {
  LPC24XX_ETH_NOT_INITIALIZED,
  LPC24XX_ETH_INITIALIZED,
  LPC24XX_ETH_STARTED,
  LPC24XX_ETH_RUNNING
} lpc24xx_eth_state;

typedef struct {
  struct arpcom arpcom;
  struct rtems_mdio_info mdio_info;
  lpc24xx_eth_state state;
  rtems_id receive_task;
  rtems_id transmit_task;
  unsigned receive_unit_number;
  unsigned transmit_unit_number;
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
} lpc24xx_eth_driver_entry;

static lpc24xx_eth_driver_entry lpc24xx_eth_driver_data = {
  .state = LPC24XX_ETH_NOT_INITIALIZED,
  .receive_task = RTEMS_ID_NONE,
  .transmit_task = RTEMS_ID_NONE
};

static inline uint32_t lpc24xx_eth_increment(
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

static void lpc24xx_eth_reset_filter( void)
{
  MAC_RXFILTERCTRL = 0;
  MAC_RXFILTERWOLCLR = 0xcf;
  MAC_HASHFILTERL = 0;
  MAC_HASHFILTERH = 0;
}

static void lpc24xx_eth_enable_promiscous_mode( bool enable)
{
  if (enable) {
    MAC_RXFILTERCTRL = ETH_RX_FIL_CTRL_ACCEPT_PERFECT
      | ETH_RX_FIL_CTRL_ACCEPT_UNICAST
      | ETH_RX_FIL_CTRL_ACCEPT_MULTICAST
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  } else {
    MAC_RXFILTERCTRL = ETH_RX_FIL_CTRL_ACCEPT_PERFECT
      | ETH_RX_FIL_CTRL_ACCEPT_BROADCAST;
  }
}

static void lpc24xx_eth_interrupt_handler(
  rtems_vector_number vector,
  void *arg
)
{
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) arg;
  rtems_event_set re = 0;
  rtems_event_set te = 0;
  uint32_t ie = 0;

  /* Get interrupt status */
  uint32_t im = MAC_INTENABLE;
  uint32_t is = MAC_INTSTATUS & im;

  /* Check receive interrupts */
  if (IS_FLAG_SET( is, ETH_INT_RX_OVERRUN)) {
    re = LPC24XX_ETH_EVENT_RECEIVE_ERROR;
    ++e->receive_fatal_errors;
    /* FIXME */
    printk( "%s: fatal receive error\n", __func__);
    while (1);
  } else if (IS_ANY_FLAG_SET( is, LPC24XX_ETH_INTERRUPT_RECEIVE)) {
    re = LPC24XX_ETH_EVENT_RECEIVE;
    ie = SET_FLAGS( ie, LPC24XX_ETH_INTERRUPT_RECEIVE);
  }

  /* Send events to receive task */
  if (re != 0) {
    ++e->receive_interrupts;
    (void) rtems_event_send( e->receive_task, re);
  }

  /* Check transmit interrupts */
  if (IS_FLAG_SET( is, ETH_INT_TX_UNDERRUN)) {
    te = LPC24XX_ETH_EVENT_TRANSMIT_ERROR;
    ++e->transmit_fatal_errors;
    /* FIXME */
    printk( "%s: fatal transmit error\n", __func__);
    while (1);
  } else if (IS_ANY_FLAG_SET( is, LPC24XX_ETH_INTERRUPT_TRANSMIT)) {
    te = LPC24XX_ETH_EVENT_TRANSMIT;
    ie = SET_FLAGS( ie, LPC24XX_ETH_INTERRUPT_TRANSMIT);
  }

  /* Send events to transmit task */
  if (te != 0) {
    ++e->transmit_interrupts;
    (void) rtems_event_send( e->transmit_task, te);
  }

  LPC24XX_ETH_PRINTK( "interrupt: rx = 0x%08x, tx = 0x%08x\n", re, te);

  /* Update interrupt mask */
  MAC_INTENABLE = CLEAR_FLAGS( im, ie);

  /* Clear interrupts */
  MAC_INTCLEAR = is;
}

static void lpc24xx_eth_enable_receive_interrupts( void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  MAC_INTENABLE = SET_FLAGS( MAC_INTENABLE, LPC24XX_ETH_INTERRUPT_RECEIVE);
  rtems_interrupt_enable( level);
}

static void lpc24xx_eth_disable_receive_interrupts( void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  MAC_INTENABLE = CLEAR_FLAGS( MAC_INTENABLE, LPC24XX_ETH_INTERRUPT_RECEIVE);
  rtems_interrupt_enable( level);
}

static void lpc24xx_eth_enable_transmit_interrupts( void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  MAC_INTENABLE = SET_FLAGS( MAC_INTENABLE, LPC24XX_ETH_INTERRUPT_TRANSMIT);
  rtems_interrupt_enable( level);
}

static void lpc24xx_eth_disable_transmit_interrupts( void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  MAC_INTENABLE = CLEAR_FLAGS( MAC_INTENABLE, LPC24XX_ETH_INTERRUPT_TRANSMIT);
  rtems_interrupt_enable( level);
}

static struct mbuf *lpc24xx_eth_new_mbuf( struct ifnet *ifp, bool wait)
{
  struct mbuf *m = NULL;
  int mw = wait ? M_WAIT : M_DONTWAIT;

  MGETHDR( m, mw, MT_DATA);
  if (m != NULL) {
    MCLGET( m, mw);
    if (IS_FLAG_SET( m->m_flags, M_EXT)) {
      /* Set receive interface */
      m->m_pkthdr.rcvif = ifp;

      /* Adjust by two bytes for proper IP header alignment */
      m->m_data = mtod( m, char *) + 2;

      return m;
    } else {
      m_free( m);
    }
  }

  return NULL;
}

static bool lpc24xx_eth_add_new_mbuf(
  struct ifnet *ifp,
  volatile lpc24xx_eth_transfer_descriptor *desc,
  struct mbuf **mbuf_table,
  unsigned i,
  bool wait
)
{
  /* New mbuf */
  struct mbuf *m = lpc24xx_eth_new_mbuf( ifp, wait);

  /* Check mbuf */
  if (m != NULL) {
    /* Add mbuf to queue */
    desc [i].start = mtod( m, uint32_t);
    desc [i].control = SET_ETH_RX_CTRL_SIZE( 0, MCLBYTES - 1)
      | ETH_RX_CTRL_INTERRUPT;

    /* Add mbuf to table */
    mbuf_table [i] = m;

    return true;
  } else {
    return false;
  }
}

static void lpc24xx_eth_receive_task( void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;
  volatile lpc24xx_eth_transfer_descriptor *const desc =
    (volatile lpc24xx_eth_transfer_descriptor *)
      LPC24XX_ETH_RECEIVE_DESC_START;
  volatile lpc24xx_eth_receive_info *const info = 
    (volatile lpc24xx_eth_receive_info *)
      LPC24XX_ETH_RECEIVE_INFO_START;
  struct mbuf **const mbuf_table =
    (struct mbuf **) LPC24XX_ETH_RECEIVE_MBUF_START;
  uint32_t index_max = e->receive_unit_number - 1;
  uint32_t produce_index = 0;
  uint32_t consume_index = 0;
  uint32_t receive_index = 0;

  LPC24XX_ETH_PRINTF( "%s\n", __func__);

  /* Disable receive interrupts */
  lpc24xx_eth_disable_receive_interrupts();

  /* Disable receiver */
  MAC_COMMAND = CLEAR_FLAG( MAC_COMMAND, ETH_CMD_RX_ENABLE);

  /* Clear receive interrupts */
  MAC_INTCLEAR = LPC24XX_ETH_INTERRUPT_RECEIVE;

  /* Fill receive queue */
  for (produce_index = 0; produce_index <= index_max; ++produce_index) {
    if (
      !lpc24xx_eth_add_new_mbuf( ifp, desc, mbuf_table, produce_index, false)
    ) {
      break;
    }
  }

  /* Check if the queue is full */
  if (produce_index == 0) {
    RTEMS_DO_CLEANUP(
      cleanup,
      "no buffers to fill receive queue: terminate receive task\n"
    );
  } else if (produce_index <= index_max) {
    /* Reduce the queue size */
    index_max = produce_index - 1;

    RTEMS_SYSLOG_ERROR( "not enough buffers to fill receive queue");
  }

  /* Receive descriptor table */
  MAC_RXDESCRIPTORNUM = index_max;
  MAC_RXDESCRIPTOR = (uint32_t) desc;
  MAC_RXSTATUS = (uint32_t) info;

  /* Initialize indices */
  produce_index = MAC_RXPRODUCEINDEX;
  consume_index = MAC_RXCONSUMEINDEX;
  receive_index = consume_index;

  /* Enable receiver */
  MAC_COMMAND = SET_FLAG( MAC_COMMAND, ETH_CMD_RX_ENABLE);

  /* Enable receive interrupts */
  lpc24xx_eth_enable_receive_interrupts();

  /* Main event loop */
  while (true) {
    bool wait_for_mbuf = false;

    /* Wait for events */
    sc = rtems_bsdnet_event_receive(
      LPC24XX_ETH_EVENT_RECEIVE, 
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    RTEMS_CLEANUP_SC( sc, cleanup, "wait for events");

    LPC24XX_ETH_PRINTF( "rx: wake up: 0x%08" PRIx32 "\n", events);

    while (true) {
      /* Clear receive interrupt status */
      MAC_INTCLEAR = LPC24XX_ETH_INTERRUPT_RECEIVE;

      /* Get current produce index */
      produce_index = MAC_RXPRODUCEINDEX;

      if (receive_index != produce_index) {
        /* Fragment mbuf and status */
        struct mbuf *m = mbuf_table [receive_index];
        uint32_t stat = info [receive_index].status;

        if (
          IS_FLAG_SET( stat, ETH_RX_STAT_LAST_FLAG)
            && ARE_FLAGS_CLEARED( stat, LPC24XX_ETH_RX_STAT_ERRORS)
        ) {
          /* Ethernet header */
          struct ether_header *eh = mtod( m, struct ether_header *);

          /* Discard Ethernet header and CRC */
          int sz = (int) GET_ETH_RX_STAT_RXSIZE( stat) + 1
            - ETHER_HDR_LEN - ETHER_CRC_LEN;

          /* Update mbuf */
          m->m_len = sz;
          m->m_pkthdr.len = sz;
          m->m_data = mtod( m, char *) + ETHER_HDR_LEN;

          LPC24XX_ETH_PRINTF( "rx: %02" PRIu32 ": %u\n", receive_index, sz);

          /* Hand over */
          ether_input( ifp, eh, m);

          /* Increment received frames counter */
          ++e->received_frames;
        } else {
          /* Release mbuf */
          m_free( m);

          /* Update error counters */
          if (IS_FLAG_SET( stat, ETH_RX_STAT_OVERRUN)) {
            ++e->receive_overrun_errors;
          }
          if (IS_FLAG_CLEARED( stat, ETH_RX_STAT_LAST_FLAG)) {
            ++e->receive_fragment_errors;
          }
          if (IS_FLAG_SET( stat, ETH_RX_STAT_CRC_ERROR)) {
            ++e->receive_crc_errors;
          }
          if (IS_FLAG_SET( stat, ETH_RX_STAT_SYMBOL_ERROR)) {
            ++e->receive_symbol_errors;
          }
          if (IS_FLAG_SET( stat, ETH_RX_STAT_LENGTH_ERROR)) {
            ++e->receive_length_errors;
          }
          if (IS_FLAG_SET( stat, ETH_RX_STAT_ALIGNMENT_ERROR)) {
            ++e->receive_alignment_errors;
          }
          if (IS_FLAG_SET( stat, ETH_RX_STAT_NO_DESCRIPTOR)) {
            ++e->receive_no_descriptor_errors;
          }
        }

        /* Increment receive index */
        receive_index = lpc24xx_eth_increment( receive_index, index_max);
      } else {
        /* Nothing to do, enable receive interrupts */
        lpc24xx_eth_enable_receive_interrupts();
        break;
      }
    }

    /* Wait for mbuf? */
    wait_for_mbuf =
      lpc24xx_eth_increment( produce_index, index_max) == consume_index;

    /* Fill queue with new mbufs */
    while (consume_index != produce_index) {
      /* Add new mbuf to queue */
      if (
        !lpc24xx_eth_add_new_mbuf(
          ifp, desc, mbuf_table, consume_index, wait_for_mbuf
        )
      ) {
        break;
      }

      /* We wait for at most one mbuf */
      wait_for_mbuf = false;

      /* Increment consume index */
      consume_index = lpc24xx_eth_increment( consume_index, index_max);

      /* Update consume indices */
      MAC_RXCONSUMEINDEX = consume_index;
    }
  }

cleanup:

  /* Release network semaphore */
  rtems_bsdnet_semaphore_release();

  /* Terminate self */
  (void) rtems_task_delete( RTEMS_SELF);
}

static struct mbuf *lpc24xx_eth_next_fragment(
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
      IF_DEQUEUE( &ifp->if_snd, m);

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
      m = m_free( m);
    }
  }

  /* Set fragment size */
  *ctrl = SET_ETH_TX_CTRL_SIZE( 0, size - 1);

  /* Discard empty successive fragments */
  n = m->m_next;
  while (n != NULL && n->m_len <= 0) {
    n = m_free( n);
  }
  m->m_next = n;

  /* Is our fragment the last in the frame? */
  if (n == NULL) {
    *ctrl = SET_FLAGS( *ctrl, LPC24XX_ETH_LAST_FRAGMENT_FLAGS);
  }

  return m;
}

static void lpc24xx_eth_transmit_task( void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;
  volatile lpc24xx_eth_transfer_descriptor *const desc =
    (volatile lpc24xx_eth_transfer_descriptor *)
      LPC24XX_ETH_TRANSMIT_DESC_START;
  volatile uint32_t *const status =
    (volatile uint32_t *) LPC24XX_ETH_TRANSMIT_STATUS_START;
  volatile char *const buf =
    (volatile char *) LPC24XX_ETH_TRANSMIT_BUFFER_START;
  struct mbuf *m = NULL;
  uint32_t index_max = e->transmit_unit_number - 1;
  uint32_t produce_index = 0;
  uint32_t consume_index = 0;
  uint32_t ctrl = 0;
  uint32_t frame_length = 0;
  char *frame_buffer = NULL;

  LPC24XX_ETH_PRINTF( "%s\n", __func__);

  /* Disable transmit interrupts */
  lpc24xx_eth_disable_transmit_interrupts();

  /* Disable transmitter */
  MAC_COMMAND = CLEAR_FLAG( MAC_COMMAND, ETH_CMD_TX_ENABLE);

  /* Clear transmit interrupts */
  MAC_INTCLEAR = LPC24XX_ETH_INTERRUPT_TRANSMIT;

  /* Initialize descriptor table */
  for (produce_index = 0; produce_index <= index_max; ++produce_index) {
    desc [produce_index].start =
      (uint32_t) (buf + produce_index * LPC24XX_ETH_TRANSMIT_BUFFER_SIZE);
    desc [produce_index].control = 0;
  }

  /* Transmit descriptors */
  MAC_TXDESCRIPTORNUM = index_max;
  MAC_TXDESCRIPTOR = (uint32_t) desc;
  MAC_TXSTATUS = (uint32_t) status;

  /* Initialize indices */
  produce_index = MAC_TXPRODUCEINDEX;
  consume_index = MAC_TXCONSUMEINDEX;

  /* Frame buffer start */
  frame_buffer = (char *) desc [produce_index].start;

  /* Enable transmitter */
  MAC_COMMAND = SET_FLAG( MAC_COMMAND, ETH_CMD_TX_ENABLE);

  /* Main event loop */
  while (true) {
    /* Wait for events */
    sc = rtems_bsdnet_event_receive(
      LPC24XX_ETH_EVENT_TRANSMIT | LPC24XX_ETH_EVENT_TRANSMIT_START, 
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    RTEMS_CLEANUP_SC( sc, cleanup, "wait for events");

    LPC24XX_ETH_PRINTF( "tx: wake up: 0x%08" PRIx32 "\n", events);

    /* Free consumed fragments */
    while (true) {
      /* Save last known consume index */
      uint32_t c = consume_index;

      /* Clear transmit interrupt status */
      MAC_INTCLEAR = LPC24XX_ETH_INTERRUPT_TRANSMIT;

      /* Get new consume index */
      consume_index = MAC_TXCONSUMEINDEX;

      /* Nothing consumed in the meantime? */
      if (c == consume_index) {
        break;
      }

      while (c != consume_index) {
        uint32_t s = status [c];

        /* Update error counters */
        if (
          IS_ANY_FLAG_SET( s, ETH_TX_STAT_ERROR | ETH_TX_STAT_NO_DESCRIPTOR)
        ) {
          if (IS_FLAG_SET( s, ETH_TX_STAT_UNDERRUN)) {
            ++e->transmit_underrun_errors;
          }
          if (IS_FLAG_SET( s, ETH_TX_STAT_LATE_COLLISION)) {
            ++e->transmit_late_collision_errors;
          }
          if (IS_FLAG_SET( s, ETH_TX_STAT_EXCESSIVE_COLLISION)) {
            ++e->transmit_excessive_collision_errors;
          }
          if (IS_FLAG_SET( s, ETH_TX_STAT_EXCESSIVE_DEFER)) {
            ++e->transmit_excessive_defer_errors;
          }
          if (IS_FLAG_SET( s, ETH_TX_STAT_NO_DESCRIPTOR)) {
            ++e->transmit_no_descriptor_errors;
          }
        }

        /* Reinitialize control field */
        desc [c].control = 0;

        /* Next consume index */
        c = lpc24xx_eth_increment( c, index_max);
      }
    }

    /* Transmit new fragments */
    while (true) {
      /* Compute next produce index */
      uint32_t p = lpc24xx_eth_increment( produce_index, index_max);

      /* Queue full? */
      if (p == consume_index) {
        /* The queue is full, wait for transmit interrupt */
        break;
      }

      /* Get next fragment and control value */
      m = lpc24xx_eth_next_fragment( ifp, m, &ctrl);

      /* New fragment? */
      if (m != NULL) {
        size_t fragment_length = (size_t) m->m_len;
        void *fragment_start = mtod( m, void *);
        uint32_t new_frame_length = frame_length + fragment_length;

        /* Check buffer size */
        if (new_frame_length > LPC24XX_ETH_TRANSMIT_BUFFER_SIZE) {
          LPC24XX_ETH_PRINTF( "tx: overflow\n");

          /* Discard overflow data */
          new_frame_length = LPC24XX_ETH_TRANSMIT_BUFFER_SIZE;
          fragment_length = new_frame_length - frame_length;

          /* Finalize frame */
          ctrl = SET_FLAGS( ctrl, LPC24XX_ETH_LAST_FRAGMENT_FLAGS);

          /* Update error counter */
          ++e->transmit_overflow_errors;
        }

        LPC24XX_ETH_PRINTF(
          "tx: copy: %" PRIu32 "%s%s\n",
          fragment_length,
          IS_FLAG_SET( m->m_flags, M_EXT) ? ", E" : "",
          IS_FLAG_SET( m->m_flags, M_PKTHDR) ? ", H" : ""
        );

        /* Copy fragment to buffer in Ethernet RAM */
        memcpy( frame_buffer, fragment_start, fragment_length);

        if (IS_FLAG_SET( ctrl, ETH_TX_CTRL_LAST)) {
          /* Finalize descriptor */
          desc [produce_index].control =
            SET_ETH_TX_CTRL_SIZE( ctrl, new_frame_length - 1);

          LPC24XX_ETH_PRINTF( "tx: %02" PRIu32 ": %" PRIu32 "\n", produce_index, new_frame_length);

          /* Next produce index */
          produce_index = p;

          /* Update the produce index */
          MAC_TXPRODUCEINDEX = produce_index;

          /* Reinitialize frame length and buffer start */
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
        m = m_free( m);
      } else {
        /* Nothing to transmit */
        break;
      }
    }

    /* No more fragments? */
    if (m == NULL) {
      /* Interface is now inactive */
      ifp->if_flags = CLEAR_FLAG( ifp->if_flags, IFF_OACTIVE);
    } else {
      /* Enable transmit interrupts */
      lpc24xx_eth_enable_transmit_interrupts();
    }
  }

cleanup:

  /* Release network semaphore */
  rtems_bsdnet_semaphore_release();

  /* Terminate self */
  (void) rtems_task_delete( RTEMS_SELF);
}

static void lpc24xx_eth_interface_init( void *arg)
{
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;

  LPC24XX_ETH_PRINTF( "%s\n", __func__);

  if (e->state == LPC24XX_ETH_INITIALIZED) {
    #ifndef LPC24XX_HAS_UBOOT
      rtems_interrupt_level level;

      rtems_interrupt_disable( level);

      /* Enable power */
      PCONP = SET_FLAGS( PCONP, 0x40000000);

      /* Set PIN selects */
      #ifdef LPC24XX_ETHERNET_RMII
        PINSEL2 = SET_FLAGS( PINSEL2, 0x55555555);
      #else
        PINSEL2 = SET_FLAGS( PINSEL2, 0x50150105);
      #endif
      PINSEL3 = SET_FLAGS( PINSEL3, 0x05);

      rtems_interrupt_enable( level);

      /* Soft reset */

      /* Do soft reset */
      MAC_MAC1 = 0xcf00;
      MAC_COMMAND = 0x38;

      /* Initialize PHY */
      /* TODO */

      /* Reinitialize registers */
      MAC_MAC2 = 0x31;
      MAC_IPGT = 0x15;
      MAC_IPGR = 0x12;
      MAC_CLRT = 0x370f;
      MAC_MAXF = 0x0600;
      MAC_SUPP = 0x0100;
      MAC_TEST = 0;
      #ifdef LPC24XX_ETHERNET_RMII
        MAC_COMMAND = 0x0400;
      #else
        MAC_COMMAND = 0x0600;
      #endif
      MAC_INTENABLE = 0;
      MAC_INTCLEAR = 0x30ff;
      MAC_POWERDOWN = 0;

      /* MAC address */
      MAC_SA0 = ((uint32_t) e->arpcom.ac_enaddr [5] << 8)
        | (uint32_t) e->arpcom.ac_enaddr [4];
      MAC_SA1 = ((uint32_t) e->arpcom.ac_enaddr [3] << 8)
        | (uint32_t) e->arpcom.ac_enaddr [2];
      MAC_SA2 = ((uint32_t) e->arpcom.ac_enaddr [1] << 8)
        | (uint32_t) e->arpcom.ac_enaddr [0];

      /* Enable receiver */
      MAC_MAC1 = 0x03;
    #else /* LPC24XX_HAS_UBOOT */
      uint32_t reg = 0;

      /* TODO */

      /* MAC configuration */
      MAC_MAC1 = 0x3;

      /* Disable and reset receiver and transmitter */
      reg = MAC_COMMAND;
      reg = CLEAR_FLAGS( reg, ETH_CMD_RX_ENABLE | ETH_CMD_TX_ENABLE);
      reg = SET_FLAGS( reg, ETH_CMD_RX_RESET | ETH_CMD_TX_RESET);
      MAC_COMMAND = reg;
    #endif /* LPC24XX_HAS_UBOOT */

    /* Start receive task */
    if (e->receive_task == RTEMS_ID_NONE) {
      e->receive_task = rtems_bsdnet_newproc(
        "ntrx", 
        4096, 
        lpc24xx_eth_receive_task,
        e
      );
    }

    /* Start transmit task */
    if (e->transmit_task == RTEMS_ID_NONE) {
      e->transmit_task = rtems_bsdnet_newproc(
        "nttx", 
        4096, 
        lpc24xx_eth_transmit_task,
        e
      );
    }

    /* Change state */
    if (
      e->receive_task != RTEMS_ID_NONE && e->transmit_task != RTEMS_ID_NONE
    ) {
      e->state = LPC24XX_ETH_STARTED;
    }
  }

  if (e->state == LPC24XX_ETH_STARTED) {
    /* Enable fatal interrupts */
    MAC_INTENABLE = ETH_INT_RX_OVERRUN | ETH_INT_TX_UNDERRUN;

    /* Enable promiscous mode */
    lpc24xx_eth_enable_promiscous_mode(
      IS_FLAG_SET( ifp->if_flags, IFF_PROMISC)
    );

    /* Start watchdog timer */
    ifp->if_timer = 1;

    /* Set interface to running state */
    ifp->if_flags = SET_FLAG( ifp->if_flags, IFF_RUNNING);

    /* Change state */
    e->state = LPC24XX_ETH_RUNNING;
  }
}

static void lpc24xx_eth_interface_stats( const lpc24xx_eth_driver_entry *e)
{
  printf( "received frames:                     %u\n", e->received_frames);
  printf( "receive interrupts:                  %u\n", e->receive_interrupts);
  printf( "transmitted frames:                  %u\n", e->transmitted_frames);
  printf( "transmit interrupts:                 %u\n", e->transmit_interrupts);
  printf( "receive overrun errors:              %u\n", e->receive_overrun_errors);
  printf( "receive fragment errors:             %u\n", e->receive_fragment_errors);
  printf( "receive CRC errors:                  %u\n", e->receive_crc_errors);
  printf( "receive symbol errors:               %u\n", e->receive_symbol_errors);
  printf( "receive length errors:               %u\n", e->receive_length_errors);
  printf( "receive alignment errors:            %u\n", e->receive_alignment_errors);
  printf( "receive no descriptor errors:        %u\n", e->receive_no_descriptor_errors);
  printf( "receive fatal errors:                %u\n", e->receive_fatal_errors);
  printf( "transmit underrun errors:            %u\n", e->transmit_underrun_errors);
  printf( "transmit late collision errors:      %u\n", e->transmit_late_collision_errors);
  printf( "transmit excessive collision errors: %u\n", e->transmit_excessive_collision_errors);
  printf( "transmit excessive defer errors:     %u\n", e->transmit_excessive_defer_errors);
  printf( "transmit no descriptor errors:       %u\n", e->transmit_no_descriptor_errors);
  printf( "transmit overflow errors:            %u\n", e->transmit_overflow_errors);
  printf( "transmit fatal errors:               %u\n", e->transmit_fatal_errors);
}

static int lpc24xx_eth_interface_ioctl(
  struct ifnet *ifp,
  ioctl_command_t command,
  caddr_t data
)
{
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) ifp->if_softc;
  int rv = 0;

  LPC24XX_ETH_PRINTF( "%s\n", __func__);

  switch (command)  {
    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      rtems_mii_ioctl( &e->mdio_info, e, (int) command, (int *) data);
      break;
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl( ifp, command, data);
      break;
    case SIOCSIFFLAGS:
      if (ifp->if_flags & IFF_RUNNING) {
        /* TODO: off */
      }
      if (ifp->if_flags & IFF_UP) {
        ifp->if_flags = SET_FLAG( ifp->if_flags, IFF_RUNNING);
        /* TODO: init */
      }
      break;
    case SIO_RTEMS_SHOW_STATS:
      lpc24xx_eth_interface_stats( e);
      break;
    default:
      rv = EINVAL;
      break;
  }

  return rv;
}

static void lpc24xx_eth_interface_start( struct ifnet *ifp)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) ifp->if_softc;

  ifp->if_flags = SET_FLAG( ifp->if_flags, IFF_OACTIVE);

  sc = rtems_event_send( e->transmit_task, LPC24XX_ETH_EVENT_TRANSMIT_START);
  RTEMS_SYSLOG_ERROR_SC( sc, "send transmit start event");
}

static void lpc24xx_eth_interface_watchdog( struct ifnet *ifp)
{
  lpc24xx_eth_driver_entry *e = (lpc24xx_eth_driver_entry *) ifp->if_softc;

  LPC24XX_ETH_PRINTF( "%s\n", __func__);
}

static int lpc24xx_eth_attach( struct rtems_bsdnet_ifconfig *config)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  lpc24xx_eth_driver_entry *e = &lpc24xx_eth_driver_data;
  struct ifnet *ifp = &e->arpcom.ac_if;
  char *unit_name = NULL;
  int unit_number = rtems_bsdnet_parse_driver_name( config, &unit_name);
  uint32_t reg = 0;

  /* Check parameter */
  if (unit_number < 0) {
    RTEMS_SYSLOG_ERROR( "parse error for interface name\n");
    return 0;
  }
  if (unit_number != 0) {
    RTEMS_DO_CLEANUP( cleanup, "unexpected unit number");
  }
  if (config->hardware_address == NULL) {
    RTEMS_DO_CLEANUP( cleanup, "MAC address missing");
  }
  if (e->state != LPC24XX_ETH_NOT_INITIALIZED) {
    RTEMS_DO_CLEANUP( cleanup, "already attached");
  }

  /* Interrupt number */
  config->irno = LPC24XX_IRQ_ETHERNET;

  /* Device control */
  config->drv_ctrl = e;

  /* Receive unit number */
  if (
    config->rbuf_count <= 0
      || config->rbuf_count > LPC24XX_ETH_RECEIVE_UNIT_NUMBER
  ) {
    config->rbuf_count = LPC24XX_ETH_RECEIVE_UNIT_NUMBER;
  }
  e->receive_unit_number = (unsigned) config->rbuf_count;

  /* Transmit unit number */
  if (
    config->xbuf_count <= 0
      || config->xbuf_count > LPC24XX_ETH_TRANSMIT_UNIT_NUMBER
  ) {
    config->xbuf_count = LPC24XX_ETH_TRANSMIT_UNIT_NUMBER;
  }
  e->transmit_unit_number = (unsigned) config->xbuf_count;

  /* Disable interrupts */
  MAC_INTENABLE = 0;

  /* Install interrupt handler */
  sc = rtems_interrupt_handler_install(
    config->irno,
    "Ethernet",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_eth_interrupt_handler,
    e
  );
  RTEMS_CLEANUP_SC( sc, cleanup, "install interrupt handler");

  /* Copy MAC address */
  memcpy( e->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

  /* Clear Ethernet RAM */
  memset( (void *) LPC24XX_ETH_RAM_START, 0, (size_t) LPC24XX_ETH_RAM_SIZE);

  /* Set interface data */
  ifp->if_softc = e;
  ifp->if_unit = (short) unit_number;
  ifp->if_name = unit_name;
  ifp->if_mtu = (config->mtu > 0) ? (u_long) config->mtu : ETHERMTU;
  ifp->if_init = lpc24xx_eth_interface_init;
  ifp->if_ioctl = lpc24xx_eth_interface_ioctl;
  ifp->if_start = lpc24xx_eth_interface_start;
  ifp->if_output = ether_output;
  ifp->if_watchdog = lpc24xx_eth_interface_watchdog;
  ifp->if_flags = config->ignore_broadcast ? 0 : IFF_BROADCAST;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;
  ifp->if_timer = 0;

  /* Change status */
  e->state = LPC24XX_ETH_INITIALIZED;

  /* Attach the interface */
  if_attach( ifp);
  ether_ifattach( ifp);

  return 1;

cleanup:

  /* FIXME: Type */
  free( unit_name, (int) 0xdeadbeef);

  return 0;
}

int lpc24xx_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
)
{
  /* FIXME: Return value */

  if (attaching) {
    return lpc24xx_eth_attach( config);
  } else {
    /* TODO */
    return 0;
  }
}
