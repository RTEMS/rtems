/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Enhanced Direct Memory Access (eDMA).
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <mpc55xx/edma.h>
#include <mpc55xx/mpc55xx.h>

#include <assert.h>

#include <bsp.h>
#include <bsp/irq.h>

#define EDMA_CHANNELS_PER_GROUP 32U

#define EDMA_GROUP_COUNT ((EDMA_CHANNEL_COUNT + 31U) / 32U)

#define EDMA_GROUP_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_GROUP)

#define EDMA_GROUP_BIT(channel) (1U << ((channel) % EDMA_CHANNELS_PER_GROUP))

#define EDMA_MODULE_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_MODULE)

static uint32_t edma_channel_occupation [EDMA_GROUP_COUNT];

static RTEMS_CHAIN_DEFINE_EMPTY(edma_channel_chain);

static unsigned edma_channel_index_of_tcd(volatile struct tcd_t *edma_tcd)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

#if EDMA_MODULE_COUNT == 1
  return channel;
#elif EDMA_MODULE_COUNT == 2
  return channel + (&EDMA_A == edma ? 0 : EDMA_CHANNELS_PER_MODULE);
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
  edma_channel_context *ctx = arg;

  mpc55xx_edma_clear_interrupts(ctx->edma_tcd);

  (*ctx->done)(ctx, 0);
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
    edma_channel_context *ctx = (edma_channel_context *) node;
    unsigned channel_index = edma_channel_index_of_tcd(ctx->edma_tcd);
    unsigned group_index = EDMA_GROUP_INDEX(channel_index);
    unsigned group_bit = EDMA_GROUP_BIT(channel_index);

    if ((error_channels [group_index] & group_bit) != 0) {
      unsigned module_index = EDMA_MODULE_INDEX(channel_index);

      (*ctx->done)(ctx, error_status [module_index]);
    }

    node = rtems_chain_next(node);
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
      mpc55xx_fatal(MPC55XX_FATAL_EDMA_IRQ_INSTALL);
    }
  }
}

rtems_status_code mpc55xx_edma_obtain_channel_by_tcd(
  volatile struct tcd_t *edma_tcd
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_index = edma_channel_index_of_tcd(edma_tcd);
  rtems_interrupt_level level;
  uint32_t channel_occupation;

  rtems_interrupt_disable(level);
  channel_occupation = edma_bit_array_set(
    channel_index,
    &edma_channel_occupation [0]
  );
  rtems_interrupt_enable(level);

  if ((channel_occupation & EDMA_GROUP_BIT(channel_index)) != 0) {
    sc = RTEMS_RESOURCE_IN_USE;
  }

  return sc;
}

void mpc55xx_edma_release_channel_by_tcd(volatile struct tcd_t *edma_tcd)
{
  unsigned channel_index = edma_channel_index_of_tcd(edma_tcd);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  edma_bit_array_clear(channel_index, &edma_channel_occupation [0]);
  rtems_interrupt_enable(level);

  mpc55xx_edma_disable_hardware_requests(edma_tcd);
  mpc55xx_edma_disable_error_interrupts(edma_tcd);
}

rtems_status_code mpc55xx_edma_obtain_channel(
  edma_channel_context *ctx,
  unsigned irq_priority
)
{
  rtems_status_code sc = mpc55xx_edma_obtain_channel_by_tcd(ctx->edma_tcd);
  if (sc == RTEMS_SUCCESSFUL) {
    unsigned channel_index = edma_channel_index_of_tcd(ctx->edma_tcd);

    sc = mpc55xx_interrupt_handler_install(
      MPC55XX_IRQ_EDMA(channel_index),
      "eDMA Channel",
      RTEMS_INTERRUPT_SHARED,
      irq_priority,
      edma_interrupt_handler,
      ctx
    );
    if (sc == RTEMS_SUCCESSFUL) {
      rtems_chain_prepend(&edma_channel_chain, &ctx->node);
      mpc55xx_edma_enable_error_interrupts(ctx->edma_tcd);
    } else {
      mpc55xx_edma_release_channel_by_tcd(ctx->edma_tcd);
      sc = RTEMS_IO_ERROR;
    }
  }

  return sc;
}

void mpc55xx_edma_release_channel(edma_channel_context *ctx)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_index = edma_channel_index_of_tcd(ctx->edma_tcd);

  mpc55xx_edma_release_channel_by_tcd(ctx->edma_tcd);
  rtems_chain_explicit_extract(&edma_channel_chain, &ctx->node);

  sc = rtems_interrupt_handler_remove(
    MPC55XX_IRQ_EDMA(channel_index),
    edma_interrupt_handler,
    ctx
  );
  if (sc != RTEMS_SUCCESSFUL) {
    mpc55xx_fatal(MPC55XX_FATAL_EDMA_IRQ_REMOVE);
  }
}

void mpc55xx_edma_copy(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
)
{
  /* Clear DONE flag */
  edma_tcd->BMF.R = 0;

  edma_tcd->SADDR = source_tcd->SADDR;
  edma_tcd->SDF.R = source_tcd->SDF.R;
  edma_tcd->NBYTES = source_tcd->NBYTES;
  edma_tcd->SLAST = source_tcd->SLAST;
  edma_tcd->DADDR = source_tcd->DADDR;
  edma_tcd->CDF.R = source_tcd->CDF.R;
  edma_tcd->DLAST_SGA = source_tcd->DLAST_SGA;
  edma_tcd->BMF.R = source_tcd->BMF.R;
}

void mpc55xx_edma_copy_and_enable_hardware_requests(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
)
{
  mpc55xx_edma_copy(edma_tcd, source_tcd);
  mpc55xx_edma_enable_hardware_requests(edma_tcd);
}

void mpc55xx_edma_sg_link(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
)
{
  edma_tcd->DLAST_SGA = (int32_t) source_tcd;
  edma_tcd->BMF.B.E_SG = 1;

  if (!edma_tcd->BMF.B.E_SG) {
    mpc55xx_edma_copy(edma_tcd, source_tcd);
  }
}
