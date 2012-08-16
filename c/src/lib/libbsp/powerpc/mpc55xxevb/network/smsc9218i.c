/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief SMSC - LAN9218i
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

#include <mpc55xx/regs.h>

#if defined(RTEMS_NETWORKING) && defined(MPC55XX_HAS_SIU)

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

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

#include <mpc55xx/edma.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/utility.h>
#include <libcpu/powerpc-utility.h>
#include <bsp/smsc9218i.h>

#if MCLBYTES != 2048
  #warning "unexpected MCLBYTES value"
#endif

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define SMSC9218I_EVENT_TX RTEMS_EVENT_1

#define SMSC9218I_EVENT_TX_START RTEMS_EVENT_2

#define SMSC9218I_EVENT_TX_ERROR RTEMS_EVENT_3

#define SMSC9218I_EVENT_RX RTEMS_EVENT_4

#define SMSC9218I_EVENT_RX_ERROR RTEMS_EVENT_5

#define SMSC9218I_EVENT_PHY RTEMS_EVENT_6

#define SMSC9218I_EVENT_DMA RTEMS_EVENT_7

#define SMSC9218I_EVENT_DMA_ERROR RTEMS_EVENT_8

/* Adjust by two bytes for proper IP header alignment */
#define SMSC9218I_RX_DATA_OFFSET 2

#define SMSC9218I_RX_JOBS 32

#define SMSC9218I_TX_JOBS 64

/* Maximum number of fragments per frame, see manual section 3.11.3.2 */
#define SMSC9218I_TX_FRAGMENT_MAX 86

#if SMSC9218I_TX_JOBS > SMSC9218I_TX_FRAGMENT_MAX
  #error "too many TX jobs"
#endif

#define SMSC9218I_IRQ_CFG_GLOBAL_ENABLE \
  (SMSC9218I_IRQ_CFG_IRQ_EN | SMSC9218I_IRQ_CFG_IRQ_TYPE)

#define SMSC9218I_IRQ_CFG_GLOBAL_DISABLE SMSC9218I_IRQ_CFG_IRQ_TYPE

#define SMSC9218I_EDMA_RX_TCD_CDF 0x10004

#define SMSC9218I_EDMA_RX_TCD_BMF 0x10003

#define SMSC9218I_TCD_BMF_LINK 0x10011

#define SMSC9218I_TCD_BMF_LAST 0x10003

#define SMSC9218I_TCD_BMF_CLEAR 0x10000

#define SMSC9218I_ERROR_INTERRUPTS \
  (SMSC9218I_INT_TXSO \
     | SMSC9218I_INT_RWT \
     | SMSC9218I_INT_RXE \
     | SMSC9218I_INT_TXE)

#define SMSC9218I_UNLIKELY(x) __builtin_expect((x), 0)

#ifdef DEBUG
  #define SMSC9218I_PRINTF(...) printf(__VA_ARGS__)
  #define SMSC9218I_PRINTK(...) printk(__VA_ARGS__)
#else
  #define SMSC9218I_PRINTF(...)
  #define SMSC9218I_PRINTK(...)
#endif

typedef enum {
  SMSC9218I_NOT_INITIALIZED,
  SMSC9218I_CONFIGURED,
  SMSC9218I_STARTED,
  SMSC9218I_RUNNING
} smsc9218i_state;

static const char *const state_to_string [] = {
  "NOT INITIALIZED",
  "CONFIGURED",
  "STARTED",
  "RUNNING"
};

typedef struct {
  struct arpcom arpcom;
  struct rtems_mdio_info mdio;
  smsc9218i_state state;
  rtems_id receive_task;
  rtems_id transmit_task;
  mpc55xx_edma_channel_entry edma_receive;
  mpc55xx_edma_channel_entry edma_transmit;
  unsigned phy_interrupts;
  unsigned received_frames;
  unsigned receiver_errors;
  unsigned receive_interrupts;
  unsigned receive_dma_interrupts;
  unsigned receive_too_long_errors;
  unsigned receive_collision_errors;
  unsigned receive_crc_errors;
  unsigned receive_dma_errors;
  unsigned receive_drop;
  unsigned receive_watchdog_timeouts;
  unsigned transmitted_frames;
  unsigned transmitter_errors;
  unsigned transmit_interrupts;
  unsigned transmit_dma_interrupts;
  unsigned transmit_status_overflows;
  unsigned transmit_frame_errors;
  unsigned transmit_dma_errors;
} smsc9218i_driver_entry;

typedef struct {
  uint32_t a;
  uint32_t b;
} smsc9218i_transmit_command;

typedef struct {
  struct tcd_t command_tcd_table [SMSC9218I_TX_JOBS];
  struct tcd_t data_tcd_table [SMSC9218I_TX_JOBS];
  smsc9218i_transmit_command command_table [SMSC9218I_TX_JOBS];
  struct mbuf *fragment_table [SMSC9218I_TX_JOBS];
  struct mbuf *frame;
  struct mbuf *next_fragment;
  int empty_index;
  int transfer_index;
  int transfer_last_index;
  int todo_index;
  int empty;
  int transfer;
  int todo;
  uint32_t frame_length;
  uint32_t command_b;
  uint16_t tag;
  bool done;
  unsigned frame_compact_count;
} smsc9218i_transmit_job_control;

typedef struct {
  struct tcd_t tcd_table [SMSC9218I_RX_JOBS];
  struct mbuf *mbuf_table [SMSC9218I_RX_JOBS];
  int consume;
  int done;
  int produce;
} smsc9218i_receive_job_control;

static smsc9218i_receive_job_control smsc_rx_jc __attribute__((aligned (32)));

static void smsc9218i_transmit_dma_done(
  mpc55xx_edma_channel_entry *channel_entry,
  uint32_t error_status
);

static void smsc9218i_receive_dma_done(
  mpc55xx_edma_channel_entry *e,
  uint32_t error_status
);

static smsc9218i_driver_entry smsc9218i_driver_data = {
  .state = SMSC9218I_NOT_INITIALIZED,
  .receive_task = RTEMS_ID_NONE,
  .transmit_task = RTEMS_ID_NONE,
  .edma_receive = {
    .channel = SMSC9218I_EDMA_RX_CHANNEL,
    .done = smsc9218i_receive_dma_done,
    .id = RTEMS_ID_NONE
  },
  .edma_transmit = {
    .channel = SMSC9218I_EDMA_TX_CHANNEL,
    .done = smsc9218i_transmit_dma_done,
    .id = RTEMS_ID_NONE
  }
};

static rtems_interval smsc9218i_timeout_init(void)
{
  return rtems_clock_get_ticks_since_boot();
}

static bool smsc9218i_timeout_not_expired(rtems_interval start)
{
  rtems_interval elapsed = rtems_clock_get_ticks_since_boot() - start;

  return elapsed < rtems_clock_get_ticks_per_second();
}

static bool smsc9218i_mac_wait(volatile smsc9218i_registers *regs)
{
  rtems_interval start = smsc9218i_timeout_init();
  bool busy;

  while (
    (busy = (regs->mac_csr_cmd & SMSC9218I_MAC_CSR_CMD_BUSY) != 0)
      && smsc9218i_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return !busy;
}

static bool smsc9218i_mac_write(
  volatile smsc9218i_registers *regs,
  uint32_t address,
  uint32_t data
)
{
  bool ok = smsc9218i_mac_wait(regs);

  if (ok) {
    regs->mac_csr_data = SMSC9218I_SWAP(data);
    regs->mac_csr_cmd = SMSC9218I_MAC_CSR_CMD_BUSY
      | SMSC9218I_MAC_CSR_CMD_ADDR(address);
    ok = smsc9218i_mac_wait(regs);
  }

  return ok;
}

static uint32_t smsc9218i_mac_read(
  volatile smsc9218i_registers *regs,
  uint32_t address,
  bool *ok_ptr
)
{
  uint32_t mac_csr_data = 0;
  bool ok = smsc9218i_mac_wait(regs);

  if (ok) {
    regs->mac_csr_cmd = SMSC9218I_MAC_CSR_CMD_BUSY
      | SMSC9218I_MAC_CSR_CMD_READ
      | SMSC9218I_MAC_CSR_CMD_ADDR(address);
    ok = smsc9218i_mac_wait(regs);

    if (ok) {
      mac_csr_data = regs->mac_csr_data;
    }
  }

  if (ok_ptr != NULL) {
    *ok_ptr = ok;
  }

  return SMSC9218I_SWAP(mac_csr_data);
}

static bool smsc9218i_phy_wait(volatile smsc9218i_registers *regs)
{
  rtems_interval start = smsc9218i_timeout_init();
  uint32_t mac_mii_acc;
  bool busy;

  do {
    mac_mii_acc = smsc9218i_mac_read(regs, SMSC9218I_MAC_MII_ACC, NULL);
  } while (
    (busy = (mac_mii_acc & SMSC9218I_MAC_MII_ACC_BUSY) != 0)
      && smsc9218i_timeout_not_expired(start)
  );

  return !busy;
}

static bool smsc9218i_phy_write(
  volatile smsc9218i_registers *regs,
  uint32_t address,
  uint32_t data
)
{
  bool ok = smsc9218i_phy_wait(regs);

  if (ok) {
    ok = smsc9218i_mac_write(regs, SMSC9218I_MAC_MII_DATA, data);

    if (ok) {
      ok = smsc9218i_mac_write(
        regs,
        SMSC9218I_MAC_MII_ACC,
        SMSC9218I_MAC_MII_ACC_PHY_DEFAULT
          | SMSC9218I_MAC_MII_ACC_BUSY
          | SMSC9218I_MAC_MII_ACC_WRITE
          | SMSC9218I_MAC_MII_ACC_ADDR(address)
      );

      if (ok) {
        ok = smsc9218i_phy_wait(regs);
      }
    }
  }

  return ok;
}

static uint32_t smsc9218i_phy_read(
  volatile smsc9218i_registers *regs,
  uint32_t address
)
{
  smsc9218i_phy_wait(regs);
  smsc9218i_mac_write(
    regs,
    SMSC9218I_MAC_MII_ACC,
    SMSC9218I_MAC_MII_ACC_PHY_DEFAULT
      | SMSC9218I_MAC_MII_ACC_BUSY
      | SMSC9218I_MAC_MII_ACC_ADDR(address)
  );
  smsc9218i_phy_wait(regs);
  return smsc9218i_mac_read(regs, SMSC9218I_MAC_MII_DATA, NULL);
}

static bool smsc9218i_enable_promiscous_mode(
  volatile smsc9218i_registers *regs,
  bool enable
)
{
  bool ok;
  uint32_t mac_cr = smsc9218i_mac_read(regs, SMSC9218I_MAC_CR, &ok);

  if (ok) {
    uint32_t flags = SMSC9218I_MAC_CR_RXALL | SMSC9218I_MAC_CR_PRMS;

    if (enable) {
      mac_cr |= flags;
    } else {
      mac_cr &= ~flags;
    }

    ok = smsc9218i_mac_write(regs, SMSC9218I_MAC_CR, mac_cr);
  }

  return ok;
}

#if defined(DEBUG)
static void smsc9218i_register_dump(volatile smsc9218i_registers *regs)
{
  uint32_t reg = 0;

  reg = regs->id_rev;
  printf(
    "id_rev: 0x%08" PRIx32 " (ID = 0x%04" PRIx32
      ", revision = 0x%04" PRIx32 ")\n",
    SMSC9218I_SWAP(reg),
    SMSC9218I_ID_REV_GET_ID(reg),
    SMSC9218I_ID_REV_GET_REV(reg)
  );

  reg = regs->irq_cfg;
  printf("irq_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->int_sts;
  printf("int_sts: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->int_en;
  printf("int_en: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->byte_test;
  printf("byte_test: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->fifo_int;
  printf("fifo_int: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->rx_cfg;
  printf("rx_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->tx_cfg;
  printf("tx_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->hw_cfg;
  printf("hw_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->rx_dp_ctl;
  printf("rx_dp_ctl: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->rx_fifo_inf;
  printf(
    "rx_fifo_inf: 0x%08" PRIx32 " (status used = %" PRIu32
      ", data used = %" PRIu32 ")\n",
    SMSC9218I_SWAP(reg),
    SMSC9218I_RX_FIFO_INF_GET_SUSED(reg),
    SMSC9218I_RX_FIFO_INF_GET_DUSED(reg)
  );

  reg = regs->tx_fifo_inf;
  printf(
    "tx_fifo_inf: 0x%08" PRIx32 " (status unused = %" PRIu32
      ", free = %" PRIu32 ")\n",
    SMSC9218I_SWAP(reg),
    SMSC9218I_TX_FIFO_INF_GET_SUSED(reg),
    SMSC9218I_TX_FIFO_INF_GET_FREE(reg)
  );

  reg = regs->pmt_ctrl;
  printf("pmt_ctrl: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->gpio_cfg;
  printf("gpio_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->gpt_cfg;
  printf("gpt_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->gpt_cnt;
  printf("gpt_cnt: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->word_swap;
  printf("word_swap: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->free_run;
  printf("free_run: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->rx_drop;
  printf("rx_drop: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  reg = regs->afc_cfg;
  printf("afc_cfg: 0x%08" PRIx32 "\n", SMSC9218I_SWAP(reg));

  printf("mac: cr: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_CR));
  printf("mac: addrh: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_ADDRH));
  printf("mac: addrl: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_ADDRL));
  printf("mac: hashh: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_HASHH));
  printf("mac: hashl: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_HASHL));
  printf("mac: mii_acc: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_MII_ACC));
  printf("mac: mii_data: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_MII_DATA));
  printf("mac: flow: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_FLOW));
  printf("mac: vlan1: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_VLAN1));
  printf("mac: vlan2: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_VLAN2));
  printf("mac: wuff: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_WUFF));
  printf("mac: wucsr: 0x%08" PRIx32 "\n", smsc9218i_mac_read(regs, SMSC9218I_MAC_WUCSR));

  printf("phy: bcr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_BMCR));
  printf("phy: bsr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_BMSR));
  printf("phy: id1: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_PHYIDR1));
  printf("phy: id2: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_PHYIDR2));
  printf("phy: anar: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_ANAR));
  printf("phy: anlpar: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_ANLPAR));
  printf("phy: anexpr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, MII_ANER));
  printf("phy: mcsr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_MCSR));
  printf("phy: spmodes: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_SPMODES));
  printf("phy: cisr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_CSIR));
  printf("phy: isr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_ISR));
  printf("phy: imr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_IMR));
  printf("phy: physcsr: 0x%08" PRIx32 "\n", smsc9218i_phy_read(regs, SMSC9218I_PHY_PHYSCSR));
}
#endif

static void smsc9218i_flush_tcd(struct tcd_t *tcd)
{
  ppc_data_cache_block_store(tcd);
}

static uint32_t smsc9218i_align_up(uint32_t val)
{
  return 4U + ((val - 1U) & ~0x3U);
}

static void smsc9218i_discard_frame(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs,
  uint32_t rx_fifo_status,
  uint32_t frame_length,
  uint32_t data_length
)
{
  /* Update error counters */
  if ((rx_fifo_status & SMSC9218I_RX_STS_ERROR_TOO_LONG) != 0) {
    ++e->receive_too_long_errors;
  }
  if ((rx_fifo_status & SMSC9218I_RX_STS_ERROR_COLLISION) != 0) {
    ++e->receive_collision_errors;
  }
  if ((rx_fifo_status & SMSC9218I_RX_STS_ERROR_CRC) != 0) {
    ++e->receive_crc_errors;
  }

  /* Discard frame */
  if (frame_length > 16) {
    /* Fast forward */
    regs->rx_dp_ctl = SMSC9218I_RX_DP_CTRL_FFWD;

    while ((regs->rx_dp_ctl & SMSC9218I_RX_DP_CTRL_FFWD) != 0) {
      /* Wait */
    }
  } else {
    uint32_t len = data_length / 4;
    uint32_t i = 0;

    /* Discard data */
    for (i = 0; i < len; ++i) {
      regs->rx_fifo_data;
    }
  }
}

static void smsc9218i_setup_receive_dma(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs,
  smsc9218i_receive_job_control *jc
)
{
  int c = jc->consume;
  int p = jc->produce;
  int np = (p + 1) % SMSC9218I_RX_JOBS;
  struct tcd_t *first = &jc->tcd_table [p];
  struct tcd_t *last = NULL;

  while (np != c) {
    uint32_t rx_fifo_inf = 0;
    uint32_t status_used = 0;

    /* Clear FIFO level status */
    regs->int_sts = SMSC9218I_INT_RSFL;

    /* Next FIFO status */
    rx_fifo_inf = regs->rx_fifo_inf;
    status_used = SMSC9218I_RX_FIFO_INF_GET_SUSED(rx_fifo_inf);

    if (status_used > 0) {
      uint32_t status = regs->rx_fifo_status;
      uint32_t frame_length = SMSC9218I_RX_STS_GET_LENGTH(status);
      uint32_t data_length = smsc9218i_align_up(
        SMSC9218I_RX_DATA_OFFSET + frame_length
      );
      bool frame_ok = (status & SMSC9218I_RX_STS_ERROR) == 0;

      if (frame_ok) {
        struct mbuf *m = jc->mbuf_table [p];
        int mbuf_length = (int) frame_length - ETHER_HDR_LEN - ETHER_CRC_LEN;
        struct tcd_t *current = &jc->tcd_table [p];

        m->m_len = mbuf_length;
        m->m_pkthdr.len = mbuf_length;

        current->NBYTES = data_length;
        smsc9218i_flush_tcd(current);

        last = current;
        p = np;
        np = (p + 1) % SMSC9218I_RX_JOBS;
      } else {
        smsc9218i_discard_frame(e, regs, status, frame_length, data_length);
      }
    } else {
      break;
    }
  }

  jc->produce = p;

  if (last != NULL) {
    volatile struct tcd_t *channel = &EDMA.TCD [e->edma_receive.channel];

    /* Setup last TCD */
    last->BMF.R = SMSC9218I_TCD_BMF_LAST;
    smsc9218i_flush_tcd(last);
    ppc_synchronize_data();

    /* Start eDMA transfer */
    channel->SADDR = first->SADDR;
    channel->SDF.R = first->SDF.R;
    channel->NBYTES = first->NBYTES;
    channel->SLAST = first->SLAST;
    channel->DADDR = first->DADDR;
    channel->CDF.R = first->CDF.R;
    channel->DLAST_SGA = first->DLAST_SGA;
    channel->BMF.R = SMSC9218I_TCD_BMF_CLEAR;
    channel->BMF.R = first->BMF.R;
  }
}

static void smsc9218i_receive_dma_done(
  mpc55xx_edma_channel_entry *channel_entry,
  uint32_t error_status
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  smsc9218i_driver_entry *e = &smsc9218i_driver_data;
  smsc9218i_receive_job_control *jc = &smsc_rx_jc;

  SMSC9218I_PRINTK(
    "edma: id = 0x%08x, error status = 0x%08x\n",
    channel_entry->id,
    error_status
  );

  ++e->receive_dma_interrupts;
  if (SMSC9218I_UNLIKELY(error_status != 0)) {
    ++e->receive_dma_errors;
  }

  sc = rtems_event_send(channel_entry->id, SMSC9218I_EVENT_DMA);
  ASSERT_SC(sc);

  jc->done = jc->produce;
}

static void smsc9218i_transmit_dma_done(
  mpc55xx_edma_channel_entry *channel_entry,
  uint32_t error_status
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  smsc9218i_driver_entry *e = &smsc9218i_driver_data;
  rtems_event_set event = error_status == 0 ?
    SMSC9218I_EVENT_DMA : SMSC9218I_EVENT_DMA_ERROR;

  SMSC9218I_PRINTK(
    "edma: id = 0x%08x, error status = 0x%08x\n",
    channel_entry->id,
    error_status
  );

  ++e->transmit_dma_interrupts;

  sc = rtems_event_send(channel_entry->id, event);
  ASSERT_SC(sc);
}

static void smsc9218i_interrupt_handler(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) arg;
  volatile smsc9218i_registers *const regs = smsc9218i;
  uint32_t int_en = regs->int_en;
  uint32_t int_sts = regs->int_sts & int_en;
  #ifdef DEBUG
    uint32_t irq_cfg = regs->irq_cfg;
  #endif

  /* Get interrupt status */
  SMSC9218I_PRINTK(
    "interrupt: irq_cfg = 0x%08x, int_sts = 0x%08x, int_en = 0x%08x\n",
    SMSC9218I_SWAP(irq_cfg),
    SMSC9218I_SWAP(int_sts),
    SMSC9218I_SWAP(int_en)
  );

  /* Disable module interrupts */
  regs->irq_cfg = SMSC9218I_IRQ_CFG_GLOBAL_DISABLE;

  /* Clear external interrupt status */
  SIU.EISR.R = 1;

  /* Clear interrupts */
  regs->int_sts = int_sts;

  /* Error interrupts */
  if (SMSC9218I_UNLIKELY((int_sts & SMSC9218I_ERROR_INTERRUPTS) != 0)) {
    if ((int_sts & SMSC9218I_INT_TXSO) != 0) {
      ++e->transmit_status_overflows;
    }
    if ((int_sts & SMSC9218I_INT_RWT) != 0) {
      ++e->receive_watchdog_timeouts;
    }
    if ((int_sts & SMSC9218I_INT_RXE) != 0) {
      ++e->receiver_errors;
    }
    if ((int_sts & SMSC9218I_INT_TXE) != 0) {
      ++e->transmitter_errors;
    }
  }

  /* Check receive interrupts */
  if ((int_sts & SMSC9218I_INT_RSFL) != 0) {
    int_en &= ~SMSC9218I_INT_RSFL;
    ++e->receive_interrupts;

    sc = rtems_event_send(e->receive_task, SMSC9218I_EVENT_RX);
    ASSERT_SC(sc);
  }

  /* Check PHY interrupts */
  if (SMSC9218I_UNLIKELY((int_sts & SMSC9218I_INT_PHY) != 0)) {
    SMSC9218I_PRINTK("interrupt: phy\n");
    int_en &= ~SMSC9218I_INT_PHY;
    ++e->phy_interrupts;
    sc = rtems_event_send(e->receive_task, SMSC9218I_EVENT_PHY);
    ASSERT_SC(sc);
  }

  /* Check transmit interrupts */
  if ((int_sts & SMSC9218I_INT_TDFA) != 0) {
    SMSC9218I_PRINTK("interrupt: transmit\n");
    int_en &= ~SMSC9218I_INT_TDFA;
    ++e->transmit_interrupts;
    sc = rtems_event_send(e->transmit_task, SMSC9218I_EVENT_TX);
    ASSERT_SC(sc);
  }

  /* Update interrupt enable */
  regs->int_en = int_en;

  /* Enable module interrupts */
  regs->irq_cfg = SMSC9218I_IRQ_CFG_GLOBAL_ENABLE;
}

static void smsc9218i_enable_transmit_interrupts(
  volatile smsc9218i_registers *regs
)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  regs->int_en |= SMSC9218I_INT_TDFA;
  rtems_interrupt_enable(level);
}

static void smsc9218i_enable_phy_interrupts(
  volatile smsc9218i_registers *regs
)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  regs->int_en |= SMSC9218I_INT_PHY;
  rtems_interrupt_enable(level);
}

static void smsc9218i_phy_clear_interrupts(
  volatile smsc9218i_registers *regs
)
{
  smsc9218i_phy_read(regs, SMSC9218I_PHY_ISR);
}

static bool smsc9218i_media_status(smsc9218i_driver_entry *e, int *media)
{
  struct ifnet *ifp = &e->arpcom.ac_if;

  *media = IFM_MAKEWORD(0, 0, 0, SMSC9218I_MAC_MII_ACC_PHY_DEFAULT);

  return (*ifp->if_ioctl)(ifp, SIOCGIFMEDIA, (caddr_t) media) == 0;
}

static void smsc9218i_media_status_change(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs
)
{
  int media = 0;
  bool media_ok = false;
  uint32_t mac_cr = 0;

  smsc9218i_phy_clear_interrupts(regs);
  smsc9218i_enable_phy_interrupts(regs);

  media_ok = smsc9218i_media_status(e, &media);
  mac_cr = smsc9218i_mac_read(regs, SMSC9218I_MAC_CR, NULL);
  if (media_ok && (IFM_OPTIONS(media) & IFM_FDX) == 0) {
    mac_cr &= ~SMSC9218I_MAC_CR_FDPX;
  } else {
    mac_cr |= SMSC9218I_MAC_CR_FDPX;
  }
  smsc9218i_mac_write(regs, SMSC9218I_MAC_CR, mac_cr);
}

static void smsc9218i_new_mbuf(
  struct ifnet *ifp,
  smsc9218i_receive_job_control *jc,
  int i
)
{
  struct mbuf *m = m_gethdr(M_WAIT, MT_DATA);
  struct tcd_t *tcd = &jc->tcd_table [i];
  char *data = NULL;

  m->m_pkthdr.rcvif = ifp;
  MCLGET(m, M_WAIT);

  data = mtod(m, char *);
  m->m_data = data + SMSC9218I_RX_DATA_OFFSET + ETHER_HDR_LEN;

  jc->mbuf_table [i] = m;

  tcd->DADDR = (uint32_t) data;
  tcd->BMF.R = SMSC9218I_TCD_BMF_LINK;

  /* FIXME: This is maybe a problem in case of a lot of small frames */
  rtems_cache_invalidate_multiple_data_lines(
    data,
    SMSC9218I_RX_DATA_OFFSET + ETHER_HDR_LEN + ETHERMTU + ETHER_CRC_LEN
  );
}

static void smsc9218i_init_receive_jobs(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs,
  volatile smsc9218i_registers *regs_dma,
  smsc9218i_receive_job_control *jc
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  struct ifnet *ifp = &e->arpcom.ac_if;
  int i = 0;

  /* Obtain receive eDMA channel */
  e->edma_receive.id = e->receive_task;
  sc = mpc55xx_edma_obtain_channel(
    &e->edma_receive,
    MPC55XX_INTC_DEFAULT_PRIORITY
  );
  ASSERT_SC(sc);

  for (i = 0; i < SMSC9218I_RX_JOBS; ++i) {
    struct tcd_t *tcd = &jc->tcd_table [i];
    struct tcd_t *next_tcd = &jc->tcd_table [(i + 1) % SMSC9218I_RX_JOBS];

    tcd->SADDR = (uint32_t) &regs_dma->rx_fifo_data;
    tcd->SDF.B.SSIZE = 0x2;
    tcd->SDF.B.DSIZE = 0x2;
    tcd->CDF.B.CITER = 1;
    tcd->CDF.B.DOFF = 4;
    tcd->DLAST_SGA = (int32_t) next_tcd;

    smsc9218i_new_mbuf(ifp, jc, i);
  }
}

static void smsc9218i_ether_input(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs,
  smsc9218i_receive_job_control *jc
)
{
  rtems_interrupt_level level;
  struct ifnet *ifp = &e->arpcom.ac_if;
  int c = jc->consume;
  int d = jc->done;

  while (c != d) {
    struct mbuf *m = jc->mbuf_table [c];
    struct ether_header *eh = (struct ether_header *)
      (mtod(m, char *) - ETHER_HDR_LEN);

    ++e->received_frames;
    ether_input(ifp, eh, m);
    smsc9218i_new_mbuf(ifp, jc, c);

    c = (c + 1) % SMSC9218I_RX_JOBS;
  }

  jc->consume = c;

  rtems_interrupt_disable(level);
  /* Enabling the receive interrupts while the DMA is active leads to chaos */
  if (c == jc->produce) {
    regs->int_en |= SMSC9218I_INT_RSFL;
  }
  rtems_interrupt_enable(level);
}

static void smsc9218i_receive_task(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  smsc9218i_receive_job_control *jc = &smsc_rx_jc;
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) arg;
  volatile smsc9218i_registers *const regs = smsc9218i;
  volatile smsc9218i_registers *const regs_dma = smsc9218i_dma;
  uint32_t mac_cr = 0;

  smsc9218i_init_receive_jobs(e, regs, regs_dma, jc);

  /* Configure receiver */
  regs->rx_cfg = SMSC9218I_RX_CFG_END_ALIGN_4
    | SMSC9218I_RX_CFG_DOFF(SMSC9218I_RX_DATA_OFFSET);

  /* Enable MAC receiver */
  mac_cr = smsc9218i_mac_read(regs, SMSC9218I_MAC_CR, NULL);
  mac_cr |= SMSC9218I_MAC_CR_RXEN;
  smsc9218i_mac_write(regs, SMSC9218I_MAC_CR, mac_cr);

  /* Enable receive interrupts */
  rtems_interrupt_disable(level);
  regs->int_en |= SMSC9218I_INT_RSFL;
  rtems_interrupt_enable(level);

  /* Enable PHY interrupts */
  smsc9218i_phy_write(
    regs,
    SMSC9218I_PHY_IMR,
    SMSC9218I_PHY_IMR_INT4 | SMSC9218I_PHY_IMR_INT6
  );
  smsc9218i_enable_phy_interrupts(regs);

  while (true) {
    rtems_event_set events;

    sc = rtems_bsdnet_event_receive(
      SMSC9218I_EVENT_DMA | SMSC9218I_EVENT_PHY | SMSC9218I_EVENT_RX,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    ASSERT_SC(sc);

    if ((events & (SMSC9218I_EVENT_RX | SMSC9218I_EVENT_DMA)) != 0) {
      smsc9218i_setup_receive_dma(e, regs, jc);
    }

    if ((events & SMSC9218I_EVENT_DMA) != 0) {
      smsc9218i_ether_input(e, regs, jc);
    }

    if ((events & SMSC9218I_EVENT_PHY) != 0) {
      smsc9218i_media_status_change(e, regs);
    }
  }
}

#if defined(DEBUG)
static void smsc9218i_transmit_job_dump(
  smsc9218i_transmit_job_control *jc,
  const char *msg
)
{
  char out [SMSC9218I_TX_JOBS + 1];
  int c = 0;
  int s = 0;

  out [SMSC9218I_TX_JOBS] = '\0';

  memset(out, '?', SMSC9218I_TX_JOBS);

  c = jc->todo_index;
  s = jc->empty;
  while (s > 0) {
    out [c] = 'E';
    --s;
    c = (c + 1) % SMSC9218I_TX_JOBS;
  }

  c = jc->transfer_index;
  s = jc->todo;
  while (s > 0) {
    out [c] = 'T';
    --s;
    c = (c + 1) % SMSC9218I_TX_JOBS;
  }

  c = jc->empty_index;
  s = jc->transfer;
  while (s > 0) {
    out [c] = 'D';
    --s;
    c = (c + 1) % SMSC9218I_TX_JOBS;
  }

  printf(
    "tx: %s: %02u:%02u:%02u %s\n",
    msg,
    jc->empty,
    jc->todo,
    jc->transfer,
    out
  );
}
#endif /* defined(DEBUG) */

static struct mbuf *smsc9218i_compact_frame(
  smsc9218i_transmit_job_control *jc,
  uint32_t frame_length
)
{
  struct mbuf *old_m = jc->frame;
  struct mbuf *new_m = m_gethdr(M_WAIT, MT_DATA);
  char *data = NULL;

  ++jc->frame_compact_count;

  MCLGET(new_m, M_WAIT);
  data = mtod(new_m, char *);

  new_m->m_len = (int) frame_length;
  new_m->m_pkthdr.len = (int) frame_length;

  while (old_m != NULL) {
    size_t len = (size_t) old_m->m_len;
    memcpy(data, mtod(old_m, void *), len);
    data += len;
    old_m = m_free(old_m);
  }

  jc->frame = new_m;

  return new_m;
}

static struct mbuf *smsc9218i_next_transmit_fragment(
  struct ifnet *ifp,
  smsc9218i_transmit_job_control *jc
)
{
  struct mbuf *m = jc->next_fragment;

  if (m != NULL) {
    /* Next fragment is from the current frame */
    jc->next_fragment = m->m_next;
  } else {
    /* Dequeue first fragment of the next frame */
    IF_DEQUEUE(&ifp->if_snd, m);

    /* Update frame head */
    jc->frame = m;

    if (m != NULL) {
      struct mbuf *n = m;
      struct mbuf *p = NULL;
      uint32_t frame_length = 0;
      unsigned fragments = 0;
      bool tiny = false;

      /* Calculate frame length and fragment number */
      do {
        int len = n->m_len;

        if (len > 0) {
          ++fragments;
          frame_length += (uint32_t) len;
          tiny = tiny || len < 4;

          /* Next fragment */
          p = n;
          n = n->m_next;
        } else {
          /* Discard empty fragment and get next */
          n = m_free(n);

          /* Remove fragment from list */
          if (p != NULL) {
            p->m_next = n;
          } else {
            jc->frame = n;
          }
        }
      } while (n != NULL);

      if (SMSC9218I_UNLIKELY(tiny || fragments > SMSC9218I_TX_JOBS)) {
        fragments = 1;
        m = smsc9218i_compact_frame(jc, frame_length);
      }

      /* Set frame length */
      jc->frame_length = frame_length;

      /* Update next fragment */
      jc->next_fragment = jc->frame->m_next;

      /* Update tag */
      ++jc->tag;

      /* Command B */
      jc->command_b = ((uint32_t) SMSC9218I_TX_B_TAG(jc->tag))
        | SMSC9218I_TX_B_FRAME_LENGTH(jc->frame_length);

      SMSC9218I_PRINTF(
        "tx: next frame: tag = %i, frame length = %" PRIu32
          ", fragments = %u\n",
        jc->tag,
        frame_length,
        fragments
      );
    } else {
      /* The transmit queue is empty, we have no next fragment */
      jc->next_fragment = NULL;

      /* Interface is now inactive */
      ifp->if_flags &= ~IFF_OACTIVE;

      /* Transmit task may wait for events */
      jc->done = true;

      SMSC9218I_PRINTF("tx: inactive\n");
    }
  }

  return m;
}

static void smsc9218i_transmit_create_jobs(
  smsc9218i_transmit_job_control *jc,
  volatile smsc9218i_registers *const regs,
  struct ifnet *ifp
)
{
  int n = jc->empty;

  if (n > 0) {
    int c = jc->todo_index;
    int i = 0;

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "create");
    #endif

    for (i = 0; i < n; ++i) {
      struct mbuf *m = smsc9218i_next_transmit_fragment(ifp, jc);

      if (m != NULL) {
        uint32_t first = m == jc->frame ? SMSC9218I_TX_A_FIRST : 0;
        uint32_t last = m->m_next == NULL ? SMSC9218I_TX_A_LAST : 0;
        uint32_t fragment_length = (uint32_t) m->m_len;
        uint32_t fragment_misalign = (uint32_t) (mtod(m, uintptr_t) % 4);
        uint32_t data_length = fragment_misalign + fragment_length;
        uint32_t data_misalign = data_length % 4;
        uint32_t data = (uint32_t) (mtod(m, char *) - fragment_misalign);

        /* Align data length on four byte boundary */
        if (data_misalign > 0) {
          data_length += 4 - data_misalign;
        }

        /* Cache flush for fragment data */
        rtems_cache_flush_multiple_data_lines(
          (const void *) data,
          data_length
        );

        /* Remember fragement */
        jc->fragment_table [c] = m;

        /* Command A and B */
        jc->command_table [c].a = SMSC9218I_TX_A_END_ALIGN_4
          | SMSC9218I_TX_A_DOFF(fragment_misalign)
          | SMSC9218I_TX_A_FRAGMENT_LENGTH(fragment_length)
          | first
          | last;
        jc->command_table [c].b = jc->command_b;

        /* Data TCD */
        jc->data_tcd_table [c].SADDR = data;
        jc->data_tcd_table [c].NBYTES = data_length;

        SMSC9218I_PRINTF("tx: create: index = %u\n", c);
      } else {
        /* Nothing to do */
        break;
      }

      c = (c + 1) % SMSC9218I_TX_JOBS;
    }

    /* Increment jobs to do */
    jc->todo += i;

    /* Decrement empty count */
    jc->empty -= i;

    /* Update todo index */
    jc->todo_index = c;

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "create done");
    #endif
  } else {
    /* Transmit task may wait for events */
    jc->done = true;
  }
}

static void smsc9218i_transmit_do_jobs(
  smsc9218i_transmit_job_control *jc,
  volatile smsc9218i_registers *const regs,
  smsc9218i_driver_entry *e
)
{
  if (jc->transfer == 0) {
    uint32_t tx_fifo_inf = regs->tx_fifo_inf;
    uint32_t data_free = SMSC9218I_TX_FIFO_INF_GET_FREE(tx_fifo_inf);
    int c = jc->transfer_index;
    int last_index = c;
    int i = 0;
    int n = jc->todo;

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "transfer");
    #endif

    for (i = 0; i < n; ++i) {
      struct tcd_t *tcd = &jc->data_tcd_table [c];
      uint32_t data_length = tcd->NBYTES + 14;

      if (data_length <= data_free) {
        /* Reduce free FIFO space */
        data_free -= data_length;

        /* Index of last TCD */
        last_index = c;

        /* Cache flush for data TCD */
        smsc9218i_flush_tcd(tcd);
      } else {
        break;
      }

      c = (c + 1) % SMSC9218I_TX_JOBS;
    }

    if (i > 0) {
      volatile struct tcd_t *channel = &EDMA.TCD [e->edma_transmit.channel];
      struct tcd_t *start = &jc->command_tcd_table [jc->transfer_index];
      struct tcd_t *last = &jc->data_tcd_table [last_index];

      /* New transfer index */
      jc->transfer_index = c;

      /* New last transfer index */
      jc->transfer_last_index = last_index;

      /* Set jobs in transfer */
      jc->transfer = i;

      /* Decrement jobs to do */
      jc->todo -= i;

      /* Cache flush for command table */
      rtems_cache_flush_multiple_data_lines(
        jc->command_table,
        sizeof(jc->command_table)
      );

      /* Enable interrupt for last data TCD */
      last->BMF.R = SMSC9218I_TCD_BMF_LAST;
      smsc9218i_flush_tcd(last);
      ppc_synchronize_data();

      /* Start eDMA transfer */
      channel->SADDR = start->SADDR;
      channel->SDF.R = start->SDF.R;
      channel->NBYTES = start->NBYTES;
      channel->SLAST = start->SLAST;
      channel->DADDR = start->DADDR;
      channel->CDF.R = start->CDF.R;
      channel->DLAST_SGA = start->DLAST_SGA;
      channel->BMF.R = SMSC9218I_TCD_BMF_CLEAR;
      channel->BMF.R = start->BMF.R;

      /* Transmit task may wait for events */
      jc->done = true;
    } else if (n > 0) {
      /* We need to wait until the FIFO has more free space */
      smsc9218i_enable_transmit_interrupts(regs);

      /* Transmit task may wait for events */
      jc->done = true;
    }

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "transfer done");
    #endif
  }
}

static void smsc9218i_transmit_finish_jobs(
  smsc9218i_transmit_job_control *jc,
  volatile smsc9218i_registers *const regs,
  smsc9218i_driver_entry *e,
  rtems_event_set events
)
{
  uint32_t tx_fifo_inf = regs->tx_fifo_inf;
  uint32_t status_used = SMSC9218I_TX_FIFO_INF_GET_SUSED(tx_fifo_inf);
  uint32_t s = 0;
  int n = jc->transfer;

  for (s = 0; s < status_used; ++s) {
    uint32_t tx_fifo_status = regs->tx_fifo_status;

    if ((tx_fifo_status & SMSC9218I_TX_STS_ERROR) == 0) {
      ++e->transmitted_frames;
    } else {
      ++e->transmit_frame_errors;
    }

    SMSC9218I_PRINTF(
      "tx: transmit status: tag = %" PRIu32 ", status = 0x%08" PRIx32 "\n",
      SMSC9218I_TX_STS_GET_TAG(tx_fifo_status),
      SMSC9218I_SWAP(tx_fifo_status)
    );
  }

  if (
    (events & (SMSC9218I_EVENT_DMA | SMSC9218I_EVENT_DMA_ERROR)) != 0
      && n > 0
  ) {
    int c = jc->empty_index;
    int i = 0;

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "finish");
    #endif

    if ((events & SMSC9218I_EVENT_DMA_ERROR) != 0) {
      ++e->transmit_dma_errors;
    }

    /* Restore last data TCD */
    jc->data_tcd_table [jc->transfer_last_index].BMF.R =
      SMSC9218I_TCD_BMF_LINK;

    for (i = 0; i < n; ++i) {
      /* Free fragment buffer */
      m_free(jc->fragment_table [c]);

      c = (c + 1) % SMSC9218I_TX_JOBS;
    }

    /* Increment empty count */
    jc->empty += n;

    /* New empty index */
    jc->empty_index = jc->transfer_index;

    /* Clear jobs in transfer */
    jc->transfer = 0;

    /* Update empty index */
    jc->empty_index = c;

    #ifdef DEBUG
      smsc9218i_transmit_job_dump(jc, "finish done");
    #endif
  }
}

/* FIXME */
static smsc9218i_transmit_job_control smsc_tx_jc __attribute__((aligned (32))) = {
  .frame = NULL,
  .next_fragment = NULL,
  .frame_length = 0,
  .tag = 0,
  .command_b = 0,
  .done = false,
  .empty_index = 0,
  .transfer_index = 0,
  .todo_index = 0,
  .empty = SMSC9218I_TX_JOBS,
  .transfer = 0,
  .todo = 0
};

static void smsc9218i_transmit_task(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set events = 0;
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;
  volatile smsc9218i_registers *const regs = smsc9218i;
  volatile smsc9218i_registers *const regs_dma = smsc9218i_dma;
  uint32_t mac_cr = 0;
  smsc9218i_transmit_job_control *jc = &smsc_tx_jc;
  unsigned i = 0;

  SMSC9218I_PRINTF("%s\n", __func__);

  /* Obtain transmit eDMA channel */
  e->edma_transmit.id = e->transmit_task;
  sc = mpc55xx_edma_obtain_channel(
    &e->edma_transmit,
    MPC55XX_INTC_DEFAULT_PRIORITY
  );
  ASSERT_SC(sc);

  /* Setup transmit eDMA descriptors */
  for (i = 0; i < SMSC9218I_TX_JOBS; ++i) {
    unsigned ii = (i + 1) % SMSC9218I_TX_JOBS;
    struct tcd_t tcd = EDMA_TCD_DEFAULT;
    struct tcd_t *command_tcd = &jc->command_tcd_table [i];
    struct tcd_t *data_tcd = &jc->data_tcd_table [i];
    struct tcd_t *next_command_tcd = &jc->command_tcd_table [ii];

    tcd.SDF.B.SSIZE = 2;
    tcd.SDF.B.SOFF = 4;
    tcd.SDF.B.DSIZE = 2;
    tcd.CDF.B.CITER = 1;
    tcd.BMF.R = SMSC9218I_TCD_BMF_LINK;

    tcd.DADDR = (uint32_t) &regs_dma->tx_fifo_data;
    tcd.DLAST_SGA = (int32_t) next_command_tcd;
    *data_tcd = tcd;

    tcd.DADDR = (uint32_t) &regs->tx_fifo_data;
    tcd.SADDR = (uint32_t) &jc->command_table [i].a;
    tcd.NBYTES = 8;
    tcd.DLAST_SGA = (int32_t) data_tcd;
    *command_tcd = tcd;
  }

  /*
   * Cache flush for command TCD.  The content of the command TCD remains
   * invariant.
   */
  rtems_cache_flush_multiple_data_lines(
    jc->command_tcd_table,
    sizeof(jc->command_tcd_table)
  );

  /* Configure transmitter */
  regs->tx_cfg = SMSC9218I_TX_CFG_SAO | SMSC9218I_TX_CFG_ON;

  /* Enable MAC transmitter */
  mac_cr = smsc9218i_mac_read(regs, SMSC9218I_MAC_CR, NULL) | SMSC9218I_MAC_CR_TXEN;
  smsc9218i_mac_write(regs, SMSC9218I_MAC_CR, mac_cr);

  /* Main event loop */
  while (true) {
    /* Wait for events */
    sc = rtems_bsdnet_event_receive(
      SMSC9218I_EVENT_TX
        | SMSC9218I_EVENT_TX_START
        | SMSC9218I_EVENT_DMA
        | SMSC9218I_EVENT_DMA_ERROR,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    ASSERT_SC(sc);

    SMSC9218I_PRINTF("tx: wake up: events = 0x%08" PRIx32 "\n", events);

    do {
      jc->done = false;
      smsc9218i_transmit_finish_jobs(jc, regs, e, events);
      smsc9218i_transmit_create_jobs(jc, regs, ifp);
      smsc9218i_transmit_do_jobs(jc, regs, e);
    } while (!jc->done);

    SMSC9218I_PRINTF("tx: done\n");
  }

  /* Release network semaphore */
  rtems_bsdnet_semaphore_release();

  /* Terminate self */
  (void) rtems_task_delete(RTEMS_SELF);
}

#if defined(DEBUG)
static void smsc9218i_test_macros(void)
{
  unsigned i = 0;
  unsigned byte_test = 0x87654321U;
  unsigned val8 = 0xa5;
  unsigned val16 = 0xa55a;
  int r = 0;

  r = SMSC9218I_SWAP(SMSC9218I_BYTE_TEST) == byte_test;
  printf("[%i] SMSC9218I_SWAP\n", r);

  for (i = 0; i < 32; ++i) {
    r = SMSC9218I_SWAP(SMSC9218I_FLAG(i)) == (1U << i);
    printf("[%i] flag: %u\n", r, i);
  }

  for (i = 0; i < 32; i += 8) {
    r = SMSC9218I_SWAP(SMSC9218I_FIELD_8(val8, i)) == (val8 << i);
    printf("[%i] field 8: %u\n", r, i);
  }

  for (i = 0; i < 32; i += 16) {
    r = SMSC9218I_SWAP(SMSC9218I_FIELD_16(val16, i)) == (val16 << i);
    printf("[%i] field 16: %u\n", r, i);
  }

  for (i = 0; i < 32; i += 8) {
    r = SMSC9218I_GET_FIELD_8(SMSC9218I_BYTE_TEST, i)
      == ((byte_test >> i) & 0xffU);
    printf("[%i] get field 8: %u\n", r, i);
  }

  for (i = 0; i < 32; i += 16) {
    r = SMSC9218I_GET_FIELD_16(SMSC9218I_BYTE_TEST, i)
      == ((byte_test >> i) & 0xffffU);
    printf("[%i] get field 16: %u\n", r, i);
  }
}
#endif

static bool smsc9218i_wait_for_eeprom_access(
  volatile smsc9218i_registers *regs
)
{
  rtems_interval start = smsc9218i_timeout_init();
  bool busy;

  while (
    (busy = (regs->e2p_cmd & SMSC9218I_E2P_CMD_EPC_BUSY) != 0)
      && smsc9218i_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return !busy;
}

static bool smsc9218i_set_mac_address(
  volatile smsc9218i_registers *regs,
  unsigned char address [6]
)
{
  bool ok = smsc9218i_mac_write(
    regs,
    SMSC9218I_MAC_ADDRL,
    ((uint32_t) address [3] << 24) | ((uint32_t) address [2] << 16)
      | ((uint32_t) address [1] << 8) | (uint32_t) address [0]
  );

  if (ok) {
    ok = smsc9218i_mac_write(
      regs,
      SMSC9218I_MAC_ADDRH,
      ((uint32_t) address [5] << 8) | (uint32_t) address [4]
    );
  }

  return ok;
}

#if defined(DEBUG)
static void smsc9218i_mac_address_dump(volatile smsc9218i_registers *regs)
{
  uint32_t low = smsc9218i_mac_read(regs, SMSC9218I_MAC_ADDRL, NULL);
  uint32_t high = smsc9218i_mac_read(regs, SMSC9218I_MAC_ADDRH, NULL);

  printf(
    "MAC address: %02" PRIx32 ":%02" PRIx32 ":%02" PRIx32
      ":%02" PRIx32 ":%02" PRIx32 ":%02" PRIx32 "\n",
    low & 0xff,
    (low >> 8) & 0xff,
    (low >> 16) & 0xff,
    (low >> 24) & 0xff,
    high & 0xff,
    (high >> 8) & 0xff
  );
}
#endif

static void smsc9218i_interrupt_init(
  smsc9218i_driver_entry *e,
  volatile smsc9218i_registers *regs
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  union SIU_PCR_tag pcr = MPC55XX_ZERO_FLAGS;
  union SIU_DIRER_tag direr = MPC55XX_ZERO_FLAGS;
  union SIU_DIRSR_tag dirsr = MPC55XX_ZERO_FLAGS;
  union SIU_ORER_tag orer = MPC55XX_ZERO_FLAGS;
  union SIU_IREER_tag ireer = MPC55XX_ZERO_FLAGS;
  union SIU_IFEER_tag ifeer = MPC55XX_ZERO_FLAGS;
  union SIU_IDFR_tag idfr = MPC55XX_ZERO_FLAGS;
  rtems_interrupt_level level;

  /* Configure IRQ input pin */
  pcr.B.PA = 2;
  pcr.B.OBE = 0;
  pcr.B.IBE = 1;
#if MPC55XX_CHIP_TYPE / 10 != 551
  pcr.B.DSC = 0;
#endif
  pcr.B.ODE = 0;
  pcr.B.HYS = 0;
  pcr.B.SRC = 0;
  pcr.B.WPE = 0;
  pcr.B.WPS = 1;
  SIU.PCR [SMSC9218I_IRQ_PIN].R = pcr.R;

  /* DMA/Interrupt Request Select */
  rtems_interrupt_disable(level);
  dirsr.R = SIU.DIRSR.R;
#if MPC55XX_CHIP_TYPE / 10 != 551
  dirsr.B.DIRS0 = 0;
#endif
  SIU.DIRSR.R = dirsr.R;
  rtems_interrupt_enable(level);

  /* Overrun Request Enable */
  rtems_interrupt_disable(level);
  orer.R = SIU.ORER.R;
  orer.B.ORE0 = 0;
  SIU.ORER.R = orer.R;
  rtems_interrupt_enable(level);

  /* IRQ Rising-Edge Enable */
  rtems_interrupt_disable(level);
  ireer.R = SIU.IREER.R;
  ireer.B.IREE0 = 0;
  SIU.IREER.R = ireer.R;
  rtems_interrupt_enable(level);

  /* IRQ Falling-Edge Enable */
  rtems_interrupt_disable(level);
  ifeer.R = SIU.IFEER.R;
  ifeer.B.IFEE0 = 1;
  SIU.IFEER.R = ifeer.R;
  rtems_interrupt_enable(level);

  /* IRQ Digital Filter */
  rtems_interrupt_disable(level);
  idfr.R = SIU.IDFR.R;
  idfr.B.DFL = 0;
  SIU.IDFR.R = idfr.R;
  rtems_interrupt_enable(level);

  /* Clear external interrupt status */
  SIU.EISR.R = 1;

  /* DMA/Interrupt Request Enable */
  rtems_interrupt_disable(level);
  direr.R = SIU.DIRER.R;
  direr.B.EIRE0 = 1;
  SIU.DIRER.R = direr.R;
  rtems_interrupt_enable(level);

  /* Install interrupt handler */
  sc = rtems_interrupt_handler_install(
    MPC55XX_IRQ_SIU_EXTERNAL_0,
    "SMSC9218i",
    RTEMS_INTERRUPT_UNIQUE,
    smsc9218i_interrupt_handler,
    e
  );
  ASSERT_SC(sc);

  /* Enable interrupts and use push-pull driver (active low) */
  regs->irq_cfg = SMSC9218I_IRQ_CFG_GLOBAL_ENABLE;

  /* Enable error interrupts */
  regs->int_en = SMSC9218I_ERROR_INTERRUPTS;
}

static void smsc9218i_reset_signal(bool signal)
{
  SIU.GPDO [SMSC9218I_RESET_PIN].R = signal ? 1 : 0;
}

static void smsc9218i_reset_signal_init(void)
{
  union SIU_PCR_tag pcr = MPC55XX_ZERO_FLAGS;

  smsc9218i_reset_signal(true);

  pcr.B.PA = 0;
  pcr.B.OBE = 1;
  pcr.B.IBE = 0;
#if MPC55XX_CHIP_TYPE / 10 != 551
  pcr.B.DSC = 0;
#endif
  pcr.B.ODE = 0;
  pcr.B.HYS = 0;
  pcr.B.SRC = 0;
  pcr.B.WPE = 1;
  pcr.B.WPS = 1;

  SIU.PCR [SMSC9218I_RESET_PIN].R = pcr.R;
}

static bool smsc9218i_hardware_reset(volatile smsc9218i_registers *regs)
{
  rtems_interval start = smsc9218i_timeout_init();
  bool not_ready;

  smsc9218i_reset_signal_init();
  smsc9218i_reset_signal(false);
  rtems_bsp_delay(200);
  smsc9218i_reset_signal(true);

  while (
    (not_ready = (regs->pmt_ctrl & SMSC9218I_PMT_CTRL_READY) == 0)
      && smsc9218i_timeout_not_expired(start)
  ) {
    /* Wait */
  }

  return !not_ready;
}

static void smsc9218i_interface_init(void *arg)
{
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) arg;
  struct ifnet *ifp = &e->arpcom.ac_if;
  volatile smsc9218i_registers *const regs = smsc9218i;
  bool ok = true;

  SMSC9218I_PRINTF("%s\n", __func__);

  if (e->state == SMSC9218I_CONFIGURED) {
    ok = smsc9218i_hardware_reset(regs);
    if (ok) {
      e->state = SMSC9218I_NOT_INITIALIZED;
    }
  }

  if (e->state == SMSC9218I_NOT_INITIALIZED) {
#if defined(DEBUG)
    smsc9218i_register_dump(regs);
#endif

    /* Set hardware configuration */
    regs->hw_cfg = SMSC9218I_HW_CFG_MBO | SMSC9218I_HW_CFG_TX_FIF_SZ(5);

    ok = smsc9218i_wait_for_eeprom_access(regs);

    if (ok) {
      ok = smsc9218i_set_mac_address(regs, e->arpcom.ac_enaddr);

      if (ok) {
#if defined(DEBUG)
        smsc9218i_mac_address_dump(regs);
#endif

        /* Auto-negotiation advertisment */
        ok = smsc9218i_phy_write(
          regs,
          MII_ANAR,
          ANAR_TX_FD | ANAR_TX | ANAR_10_FD | ANAR_10 | ANAR_CSMA
        );

        if (ok) {
#ifdef SMSC9218I_ENABLE_LED_OUTPUTS
          regs->gpio_cfg = SMSC9218I_HW_CFG_LED_1
            | SMSC9218I_HW_CFG_LED_2
            | SMSC9218I_HW_CFG_LED_3;
#endif

          smsc9218i_interrupt_init(e, regs);

          /* Set MAC control */
          ok = smsc9218i_mac_write(regs, SMSC9218I_MAC_CR, SMSC9218I_MAC_CR_FDPX);
          if (ok) {
            /* Set FIFO interrupts */
            regs->fifo_int = SMSC9218I_FIFO_INT_TDAL(32);

            /* Clear receive drop counter */
            regs->rx_drop;

            /* Start receive task */
            if (e->receive_task == RTEMS_ID_NONE) {
              e->receive_task = rtems_bsdnet_newproc(
                "ntrx",
                4096,
                smsc9218i_receive_task,
                e
              );
            }

            /* Start transmit task */
            if (e->transmit_task == RTEMS_ID_NONE) {
              e->transmit_task = rtems_bsdnet_newproc(
                "nttx",
                4096,
                smsc9218i_transmit_task,
                e
              );
            }

            /* Change state */
            if (e->receive_task != RTEMS_ID_NONE
              && e->transmit_task != RTEMS_ID_NONE) {
              e->state = SMSC9218I_STARTED;
            }
          }
        }
      }
    }
  }

  if (e->state == SMSC9218I_STARTED) {
    /* Enable promiscous mode */
    ok = smsc9218i_enable_promiscous_mode(
      regs,
      (ifp->if_flags & IFF_PROMISC) != 0
    );

    if (ok) {
      /* Set interface to running state */
      ifp->if_flags |= IFF_RUNNING;

      /* Change state */
      e->state = SMSC9218I_RUNNING;
    }
  }
}

static int smsc9218i_mdio_read(
  int phy,
  void *arg,
  unsigned phy_reg,
  uint32_t *val
)
{
  volatile smsc9218i_registers *const regs = smsc9218i;

  *val = smsc9218i_phy_read(regs, phy_reg);

  return 0;
}

static int smsc9218i_mdio_write(
  int phy,
  void *arg,
  unsigned phy_reg,
  uint32_t data
)
{
  volatile smsc9218i_registers *const regs = smsc9218i;

  smsc9218i_phy_write(regs, phy_reg, data);

  return 0;
}

static void smsc9218i_interface_stats(smsc9218i_driver_entry *e)
{
  volatile smsc9218i_registers *const regs = smsc9218i;
  smsc9218i_transmit_job_control *jc = &smsc_tx_jc;
  int media = 0;
  bool media_ok = smsc9218i_media_status(e, &media);

  if (media_ok) {
    rtems_ifmedia2str(media, NULL, 0);
    printf ("\n");
  } else {
    printf ("PHY communication error\n");
  }

  e->receive_drop += SMSC9218I_SWAP(regs->rx_drop);

  printf("PHY interrupts:            %u\n", e->phy_interrupts);
  printf("received frames:           %u\n", e->received_frames);
  printf("receiver errors:           %u\n", e->receiver_errors);
  printf("receive interrupts:        %u\n", e->receive_interrupts);
  printf("receive DMA interrupts:    %u\n", e->receive_dma_interrupts);
  printf("receive to long errors:    %u\n", e->receive_too_long_errors);
  printf("receive collision errors:  %u\n", e->receive_collision_errors);
  printf("receive CRC errors:        %u\n", e->receive_crc_errors);
  printf("receive DMA errors:        %u\n", e->receive_dma_errors);
  printf("receive drops:             %u\n", e->receive_drop);
  printf("receive watchdog timeouts: %u\n", e->receive_watchdog_timeouts);
  printf("transmitted frames:        %u\n", e->transmitted_frames);
  printf("transmitter errors:        %u\n", e->transmitter_errors);
  printf("transmit interrupts:       %u\n", e->transmit_interrupts);
  printf("transmit DMA interrupts:   %u\n", e->transmit_dma_interrupts);
  printf("transmit status overflows: %u\n", e->transmit_status_overflows);
  printf("transmit frame errors:     %u\n", e->transmit_frame_errors);
  printf("transmit DMA errors:       %u\n", e->transmit_dma_errors);
  printf("frame compact count:       %u\n", jc->frame_compact_count);
  printf("interface state:           %s\n", state_to_string [e->state]);
}

static void smsc9218i_interface_off(struct ifnet *ifp)
{
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) ifp->if_softc;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    MPC55XX_IRQ_SIU_EXTERNAL_0,
    smsc9218i_interrupt_handler,
    e
  );
  ASSERT_SC(sc);

  mpc55xx_edma_release_channel(
    &e->edma_receive
  );

  mpc55xx_edma_release_channel(
    &e->edma_transmit
  );

  /* set in reset state */
  smsc9218i_reset_signal(false);
}

static int smsc9218i_interface_ioctl(
  struct ifnet *ifp,
  ioctl_command_t command,
  caddr_t data
) {
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) ifp->if_softc;
  int rv = 0;

  SMSC9218I_PRINTF("%s\n", __func__);

  switch (command)  {
    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      rtems_mii_ioctl(&e->mdio, e, command, (int *) data);
      break;
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, command, data);
      break;
    case SIOCSIFFLAGS:
      if (ifp->if_flags & IFF_UP) {
        /* TODO: init */
      } else {
        smsc9218i_interface_off(ifp);
      }
      break;
    case SIO_RTEMS_SHOW_STATS:
      smsc9218i_interface_stats(e);
      break;
    default:
      rv = EINVAL;
      break;
  }

  return rv;
}

static void smsc9218i_interface_start(struct ifnet *ifp)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  smsc9218i_driver_entry *e = (smsc9218i_driver_entry *) ifp->if_softc;

  /* Interface is now active */
  ifp->if_flags |= IFF_OACTIVE;

  sc = rtems_event_send(e->transmit_task, SMSC9218I_EVENT_TX_START);
  ASSERT_SC(sc);
}

static void smsc9218i_interface_watchdog(struct ifnet *ifp)
{
  SMSC9218I_PRINTF("%s\n", __func__);
}

static void smsc9218i_attach(struct rtems_bsdnet_ifconfig *config)
{
  smsc9218i_driver_entry *e = &smsc9218i_driver_data;
  struct ifnet *ifp = &e->arpcom.ac_if;
  char *unit_name = NULL;
  int unit_number = rtems_bsdnet_parse_driver_name(config, &unit_name);

  /* Check parameter */
  assert(unit_number == 0);
  assert(config->hardware_address != NULL);
  assert(e->state == SMSC9218I_NOT_INITIALIZED);

  /* Interrupt number */
  config->irno = MPC55XX_IRQ_SIU_EXTERNAL_0;

  /* Device control */
  config->drv_ctrl = e;

  /* Receive unit number */
  config->rbuf_count = 0;

  /* Transmit unit number */
  config->xbuf_count = 0;

  /* Copy MAC address */
  memcpy(e->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

  /* Set interface data */
  ifp->if_softc = e;
  ifp->if_unit = (short) unit_number;
  ifp->if_name = unit_name;
  ifp->if_mtu = config->mtu > 0 ? (u_long) config->mtu : ETHERMTU;
  ifp->if_init = smsc9218i_interface_init;
  ifp->if_ioctl = smsc9218i_interface_ioctl;
  ifp->if_start = smsc9218i_interface_start;
  ifp->if_output = ether_output;
  ifp->if_watchdog = smsc9218i_interface_watchdog;
  ifp->if_flags = config->ignore_broadcast ? 0 : IFF_BROADCAST;
  ifp->if_snd.ifq_maxlen = ifqmaxlen;
  ifp->if_timer = 0;

  /* MDIO */
  e->mdio.mdio_r = smsc9218i_mdio_read;
  e->mdio.mdio_w = smsc9218i_mdio_write;

  /* Change status */
  e->state = SMSC9218I_CONFIGURED;

  /* Attach the interface */
  if_attach(ifp);
  ether_ifattach(ifp);
}

int smsc9218i_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
) {
  if (attaching) {
    smsc9218i_attach(config);
  } else {
    /* TODO */
  }

  /* FIXME: Return value */
  return 0;
}

#endif /* defined(RTEMS_NETWORKING) && defined(MPC55XX_HAS_SIU) */
