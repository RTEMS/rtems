/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Enhanced Direct Memory Access (eDMA).
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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

#include <mpc55xx/regs.h>
#include <mpc55xx/edma.h>
#include <mpc55xx/mpc55xx.h>

#include <assert.h>

#include <bsp/irq.h>

#if MPC55XX_CHIP_TYPE / 10 == 551
  #define EDMA_CHANNEL_COUNT 16U
#elif MPC55XX_CHIP_TYPE / 10 == 564
  #define EDMA_CHANNEL_COUNT 16U
#elif MPC55XX_CHIP_TYPE / 10 == 567
  #define EDMA_CHANNEL_COUNT 96U
#else
  #define EDMA_CHANNEL_COUNT 64U
#endif

#define EDMA_CHANNELS_PER_GROUP 32U

#define EDMA_CHANNELS_PER_MODULE 64U

#define EDMA_GROUP_COUNT ((EDMA_CHANNEL_COUNT + 31U) / 32U)

#define EDMA_MODULE_COUNT ((EDMA_CHANNEL_COUNT + 63U) / 64U)

#define EDMA_INVALID_CHANNEL EDMA_CHANNEL_COUNT

#define EDMA_IS_CHANNEL_INVALID(i) ((unsigned) (i) >= EDMA_CHANNEL_COUNT)

#define EDMA_IS_CHANNEL_VALID(i) ((unsigned) (i) < EDMA_CHANNEL_COUNT)

#define EDMA_GROUP_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_GROUP)

#define EDMA_GROUP_BIT(channel) (1U << ((channel) % EDMA_CHANNELS_PER_GROUP))

#define EDMA_MODULE_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_MODULE)

#define EDMA_MODULE_BIT(channel) (1U << ((channel) % EDMA_CHANNELS_PER_MODULE))

static uint32_t edma_channel_occupation [EDMA_GROUP_COUNT];

static RTEMS_CHAIN_DEFINE_EMPTY(edma_channel_chain);

static volatile struct EDMA_tag *edma_get_regs_by_channel(unsigned channel)
{
#if EDMA_MODULE_COUNT == 1
  return &EDMA;
#elif EDMA_MODULE_COUNT == 2
  return channel < EDMA_CHANNELS_PER_MODULE ? &EDMA_A : &EDMA_B;
#else
  #error "unsupported module count"
#endif
}

static volatile struct EDMA_tag *edma_get_regs_by_module(unsigned module)
{
#if EDMA_MODULE_COUNT == 1
  return &EDMA;
#elif EDMA_MODULE_COUNT == 2
  return module == 0 ? &EDMA_A : &EDMA_B;
#else
  #error "unsupported module count"
#endif
}

static uint32_t edma_bit_array_set(unsigned channel, uint32_t *bit_array)
{
  unsigned array = channel / 32;
  uint32_t bit = 1U << (channel % 32);
  uint32_t previous = bit_array [array];

  bit_array [array] = previous | bit;

  return previous;
}

static uint32_t edma_bit_array_clear(unsigned channel, uint32_t *bit_array)
{
  unsigned array = channel / 32;
  uint32_t bit = 1U << (channel % 32);
  uint32_t previous = bit_array [array];

  bit_array [array] = previous & ~bit;

  return previous;
}

static void edma_interrupt_handler(void *arg)
{
  mpc55xx_edma_channel_entry *e = arg;
  unsigned channel = e->channel;
  volatile struct EDMA_tag *edma = edma_get_regs_by_channel(channel);

  edma->CIRQR.R = (uint8_t) channel;

  e->done(e, 0);
}

static void edma_interrupt_error_handler(void *arg)
{
  rtems_chain_control *chain = &edma_channel_chain;
  rtems_chain_node *node = rtems_chain_first(chain);
  uint32_t error_channels [] = {
#if EDMA_GROUP_COUNT >= 1
    EDMA.ERL.R
#endif
#if EDMA_GROUP_COUNT >= 2
    , EDMA.ERH.R
#endif
#if EDMA_GROUP_COUNT >= 3
    , EDMA_B.ERL.R
#endif
  };
  uint32_t error_status [] = {
#if EDMA_GROUP_COUNT >= 1
    EDMA.ESR.R
#endif
#if EDMA_GROUP_COUNT >= 3
    , EDMA_B.ESR.R
#endif
  };

#if EDMA_GROUP_COUNT >= 1
  EDMA.ERL.R = error_channels [0];
#endif
#if EDMA_GROUP_COUNT >= 2
  EDMA.ERH.R = error_channels [1];
#endif
#if EDMA_GROUP_COUNT >= 3
  EDMA_B.ERL.R = error_channels [2];
#endif

  while (!rtems_chain_is_tail(chain, node)) {
    mpc55xx_edma_channel_entry *e = (mpc55xx_edma_channel_entry *) node;
    unsigned channel = e->channel;
    unsigned group_index = EDMA_GROUP_INDEX(channel);
    unsigned group_bit = EDMA_GROUP_BIT(channel);

    if ((error_channels [group_index] & group_bit) != 0) {
      unsigned module_index = EDMA_MODULE_INDEX(channel);

      e->done(e, error_status [module_index]);
    }

    node = rtems_chain_next(node);
  }
}

void mpc55xx_edma_enable_hardware_requests(unsigned channel, bool enable)
{
  volatile struct EDMA_tag *edma = edma_get_regs_by_channel(channel);

  assert(EDMA_IS_CHANNEL_VALID(channel));

  if (enable) {
    edma->SERQR.R = (uint8_t) channel;
  } else {
    edma->CERQR.R = (uint8_t) channel;
  }
}

void mpc55xx_edma_enable_error_interrupts(unsigned channel, bool enable)
{
  volatile struct EDMA_tag *edma = edma_get_regs_by_channel(channel);

  assert(EDMA_IS_CHANNEL_VALID(channel));

  if (enable) {
    edma->SEEIR.R = (uint8_t) channel;
  } else {
    edma->CEEIR.R = (uint8_t) channel;
  }
}

void mpc55xx_edma_init(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_remaining = EDMA_CHANNEL_COUNT;
  unsigned module = 0;
  unsigned group = 0;

  for (module = 0; module < EDMA_MODULE_COUNT; ++module) {
    volatile struct EDMA_tag *edma = edma_get_regs_by_module(module);
    unsigned channel_count = channel_remaining < EDMA_CHANNELS_PER_MODULE ?
      channel_remaining : EDMA_CHANNELS_PER_MODULE;
    unsigned channel = 0;

    channel_remaining -= channel_count;

    /* Disable requests */
    edma->CERQR.B.CERQ = 0x40;

    /* Arbitration mode: group round robin, channel fixed */
    edma->CR.B.ERGA = 1;
    edma->CR.B.ERCA = 0;
    for (channel = 0; channel < channel_count; ++channel) {
      volatile struct tcd_t *tcd = &edma->TCD [channel];
      edma->CPR [channel].R = 0x80U | (channel & 0xfU);

      /* Initialize TCD, stop channel first */
      tcd->BMF.R = 0;
      tcd->SADDR = 0;
      tcd->SDF.R = 0;
      tcd->NBYTES = 0;
      tcd->SLAST = 0;
      tcd->DADDR = 0;
      tcd->CDF.R = 0;
      tcd->DLAST_SGA = 0;
    }

    /* Clear interrupt requests */
    edma->CIRQR.B.CINT = 0x40;
    edma->CER.B.CERR = 0x40;
  }

  for (group = 0; group < EDMA_GROUP_COUNT; ++group) {
    sc = mpc55xx_interrupt_handler_install(
      MPC55XX_IRQ_EDMA_ERROR(group),
      "eDMA Error",
      RTEMS_INTERRUPT_UNIQUE,
      MPC55XX_INTC_DEFAULT_PRIORITY,
      edma_interrupt_error_handler,
      NULL
    );
    if (sc != RTEMS_SUCCESSFUL) {
      /* FIXME */
      rtems_fatal_error_occurred(0xdeadbeef);
    }
  }
}

rtems_status_code mpc55xx_edma_obtain_channel(
  mpc55xx_edma_channel_entry *e,
  unsigned irq_priority
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  unsigned channel = e->channel;
  uint32_t channel_occupation = 0;

  if (EDMA_IS_CHANNEL_INVALID(channel)) {
    return RTEMS_INVALID_ID;
  }

  rtems_interrupt_disable(level);
  channel_occupation = edma_bit_array_set(
    channel,
    &edma_channel_occupation [0]
  );
  rtems_interrupt_enable(level);

  if ((channel_occupation & EDMA_GROUP_BIT(channel))) {
    return RTEMS_RESOURCE_IN_USE;
  }

  sc = mpc55xx_interrupt_handler_install(
    MPC55XX_IRQ_EDMA(channel),
    "eDMA Channel",
    RTEMS_INTERRUPT_SHARED,
    irq_priority,
    edma_interrupt_handler,
    e
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_interrupt_disable(level);
    edma_bit_array_clear(channel, &edma_channel_occupation [0]);
    rtems_interrupt_enable(level);

    return RTEMS_IO_ERROR;
  }

  rtems_chain_prepend(&edma_channel_chain, &e->node);
  mpc55xx_edma_enable_error_interrupts(channel, true);

  return RTEMS_SUCCESSFUL;
}

void mpc55xx_edma_release_channel(mpc55xx_edma_channel_entry *e)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  unsigned channel = e->channel;

  rtems_interrupt_disable(level);
  edma_bit_array_clear(channel, &edma_channel_occupation [0]);
  rtems_interrupt_enable(level);

  mpc55xx_edma_enable_hardware_requests(channel, false);
  mpc55xx_edma_enable_error_interrupts(channel, false);
  rtems_chain_extract(&e->node);

  sc = rtems_interrupt_handler_remove(
    MPC55XX_IRQ_EDMA(e->channel),
    edma_interrupt_handler,
    e
  );
  if (sc != RTEMS_SUCCESSFUL) {
    /* FIXME */
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  e->done(e, 0);
}
