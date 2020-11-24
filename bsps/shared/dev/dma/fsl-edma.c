/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedFslEDMA
 *
 * @brief Freescale / NXP Enhanced Direct Memory Access (eDMA).
 */

/*
 * Copyright (C) 2008-2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <fsl/edma.h>
#include <fsl/regs-edma.h>

#include <assert.h>

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#ifdef LIBBSP_ARM_IMXRT_BSP_H
#include <MIMXRT1052.h>
#endif

#define EDMA_CHANNELS_PER_GROUP 32U

#define EDMA_GROUP_COUNT ((EDMA_CHANNEL_COUNT + 31U) / 32U)

#define EDMA_GROUP_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_GROUP)

#define EDMA_GROUP_BIT(channel) (1U << ((channel) % EDMA_CHANNELS_PER_GROUP))

#define EDMA_MODULE_INDEX(channel) ((channel) / EDMA_CHANNELS_PER_MODULE)

static uint32_t edma_channel_occupation [EDMA_GROUP_COUNT];

static RTEMS_CHAIN_DEFINE_EMPTY(edma_channel_chain);

volatile struct fsl_edma *edma_inst[EDMA_MODULE_COUNT] = {
#ifdef LIBBSP_ARM_IMXRT_BSP_H
  (volatile struct fsl_edma *) DMA0,
#else /* ! LIBBSP_ARM_IMXRT_BSP_H */
  #if EDMA_MODULE_COUNT == 1
    (volatile struct fsl_edma *) &EDMA,
  #elif EDMA_MODULE_COUNT == 2
    (volatile struct fsl_edma *) &EDMA_A,
    (volatile struct fsl_edma *) &EDMA_B,
  #else
    #error "unsupported module count"
  #endif
#endif /* LIBBSP_ARM_IMXRT_BSP_H */
};

unsigned fsl_edma_channel_index_of_tcd(
  volatile struct fsl_edma_tcd *edma_tcd
)
{
  volatile struct fsl_edma *edma = fsl_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

#if EDMA_MODULE_COUNT == 2
  if (edma_inst[1] == edma) {
    channel += EDMA_CHANNELS_PER_MODULE;
  }
#elif EDMA_MODULE_COUNT > 2
  #error "unsupported module count"
#endif

  return channel;
}

volatile struct fsl_edma_tcd *fsl_edma_tcd_of_channel_index(unsigned index)
{
  unsigned module = index / EDMA_CHANNELS_PER_MODULE;
  unsigned channel = index % EDMA_CHANNELS_PER_MODULE;

  return &edma_inst[module]->TCD[channel];
}

static volatile struct fsl_edma *fsl_edma_get_regs_by_module(unsigned module)
{
  return edma_inst[module];
}

static uint32_t fsl_edma_bit_array_get_lowest_0(uint32_t *bit_array)
{
  unsigned array;

  for (array = 0; array < EDMA_MODULE_COUNT; ++array) {
    uint32_t first_0 = __builtin_ffs(~(bit_array[array]));
    if (first_0 > 0) {
      return (first_0 - 1) + array * 32;
    }
  }

  return UINT32_MAX;
}

static uint32_t fsl_edma_bit_array_set(unsigned channel, uint32_t *bit_array)
{
  unsigned array = channel / 32;
  uint32_t bit = 1U << (channel % 32);
  uint32_t previous = bit_array [array];

  bit_array [array] = previous | bit;

  return previous;
}

static uint32_t fsl_edma_bit_array_clear(unsigned channel, uint32_t *bit_array)
{
  unsigned array = channel / 32;
  uint32_t bit = 1U << (channel % 32);
  uint32_t previous = bit_array [array];

  bit_array [array] = previous & ~bit;

  return previous;
}

static void fsl_edma_interrupt_handler(void *arg)
{
  fsl_edma_channel_context *ctx = arg;

  fsl_edma_clear_interrupts(ctx->edma_tcd);

  (*ctx->done)(ctx, 0);
}

static void edma_interrupt_error_handler(void *arg)
{
  rtems_chain_control *chain = &edma_channel_chain;
  rtems_chain_node *node = rtems_chain_first(chain);

  uint32_t error_channels [] = {
#if EDMA_GROUP_COUNT >= 1
    edma_inst[0]->ERL,
#endif
#if EDMA_GROUP_COUNT >= 2
    edma_inst[0]->ERH,
#endif
#if EDMA_GROUP_COUNT >= 3
    edma_inst[1]->ERL,
#endif
  };
  uint32_t error_status [] = {
#if EDMA_GROUP_COUNT >= 1
    edma_inst[0]->ESR,
#endif
#if EDMA_GROUP_COUNT >= 3
    edma_inst[1]->ESR,
#endif
  };

#if EDMA_GROUP_COUNT >= 1
  edma_inst[0]->ERL = error_channels [0];
#endif
#if EDMA_GROUP_COUNT >= 2
  edma_inst[0]->ERH = error_channels [1];
#endif
#if EDMA_GROUP_COUNT >= 3
  edma_inst[1]->ERL = error_channels [2];
#endif

  while (!rtems_chain_is_tail(chain, node)) {
    fsl_edma_channel_context *ctx = (fsl_edma_channel_context *) node;
    unsigned channel_index = fsl_edma_channel_index_of_tcd(ctx->edma_tcd);
    unsigned group_index = EDMA_GROUP_INDEX(channel_index);
    unsigned group_bit = EDMA_GROUP_BIT(channel_index);

    if ((error_channels [group_index] & group_bit) != 0) {
      unsigned module_index = EDMA_MODULE_INDEX(channel_index);

      (*ctx->done)(ctx, error_status [module_index]);
    }

    node = rtems_chain_next(node);
  }
}

void fsl_edma_init(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_remaining = EDMA_CHANNEL_COUNT;
  unsigned module = 0;
  unsigned group = 0;

  for (module = 0; module < EDMA_MODULE_COUNT; ++module) {
    volatile struct fsl_edma *edma = fsl_edma_get_regs_by_module(module);
    unsigned channel_count = channel_remaining < EDMA_CHANNELS_PER_MODULE ?
      channel_remaining : EDMA_CHANNELS_PER_MODULE;
    unsigned channel = 0;

    channel_remaining -= channel_count;

    /* Disable requests */
    edma->CERQR = EDMA_CERQR_CAER;

    /* Arbitration mode: group round robin, channel fixed */
    edma->CR |= EDMA_CR_ERGA;
    edma->CR &= ~EDMA_CR_ERCA;
#if defined(BSP_FSL_EDMA_EMLM)
    edma->CR |= EDMA_CR_EMLM;
#endif
    for (channel = 0; channel < channel_count; ++channel) {
      volatile struct fsl_edma_tcd *tcd = &edma->TCD [channel];
      edma->CPR [channel] = EDMA_CPR_ECP | EDMA_CPR_CHPRI(channel);

      /* Initialize TCD, stop channel first */
      tcd->BMF = 0;
      tcd->SADDR = 0;
      tcd->SDF = 0;
      tcd->NBYTES = 0;
      tcd->SLAST = 0;
      tcd->DADDR = 0;
      tcd->CDF = 0;
      tcd->DLAST_SGA = 0;
    }

    /* Clear interrupt requests */
    edma->CIRQR = EDMA_CIRQR_CAIR;
    edma->CER = EDMA_CER_CAEI;
  }

  for (group = 0; group < EDMA_GROUP_COUNT; ++group) {
#if defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
    sc = mpc55xx_interrupt_handler_install(
      MPC55XX_IRQ_EDMA_ERROR(group),
      "eDMA Error",
      RTEMS_INTERRUPT_UNIQUE,
      MPC55XX_INTC_DEFAULT_PRIORITY,
      edma_interrupt_error_handler,
      NULL
    );
#elif defined(LIBBSP_ARM_IMXRT_BSP_H)
    sc = rtems_interrupt_handler_install(
      DMA_ERROR_IRQn,
      "eDMA Error",
      RTEMS_INTERRUPT_UNIQUE,
      edma_interrupt_error_handler,
      NULL
    );
#else
  #error "Unknown chip"
#endif
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_fatal(MPC55XX_FATAL_EDMA_IRQ_INSTALL);
    }
  }
}

static rtems_status_code fsl_edma_obtain_next_free_channel_and_get_tcd(
  volatile struct fsl_edma_tcd **edma_tcd
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_index;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  channel_index = fsl_edma_bit_array_get_lowest_0(&edma_channel_occupation [0]);
  if (channel_index > EDMA_CHANNEL_COUNT) {
    sc = RTEMS_RESOURCE_IN_USE;
  } else {
    fsl_edma_bit_array_set(
      channel_index,
      &edma_channel_occupation [0]
    );
  }
  rtems_interrupt_enable(level);

  if (sc == RTEMS_SUCCESSFUL) {
    *edma_tcd = fsl_edma_tcd_of_channel_index(channel_index);
  }

  return sc;
}

rtems_status_code fsl_edma_obtain_channel_by_tcd(
  volatile struct fsl_edma_tcd *edma_tcd
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_index = fsl_edma_channel_index_of_tcd(edma_tcd);
  rtems_interrupt_level level;
  uint32_t channel_occupation;

  rtems_interrupt_disable(level);
  channel_occupation = fsl_edma_bit_array_set(
    channel_index,
    &edma_channel_occupation [0]
  );
  rtems_interrupt_enable(level);

  if ((channel_occupation & EDMA_GROUP_BIT(channel_index)) != 0) {
    sc = RTEMS_RESOURCE_IN_USE;
  }

  return sc;
}

void fsl_edma_release_channel_by_tcd(volatile struct fsl_edma_tcd *edma_tcd)
{
  unsigned channel_index = fsl_edma_channel_index_of_tcd(edma_tcd);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  fsl_edma_bit_array_clear(channel_index, &edma_channel_occupation [0]);
  rtems_interrupt_enable(level);

  fsl_edma_disable_hardware_requests(edma_tcd);
  fsl_edma_disable_error_interrupts(edma_tcd);
}

static rtems_status_code fsl_edma_install_obtained_channel(
  fsl_edma_channel_context *ctx,
  unsigned irq_priority
)
{
  unsigned channel_index = fsl_edma_channel_index_of_tcd(ctx->edma_tcd);
  rtems_status_code sc;

#if defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
  sc = fsl_interrupt_handler_install(
    MPC55XX_IRQ_EDMA(channel_index),
    "eDMA Channel",
    RTEMS_INTERRUPT_SHARED,
    irq_priority,
    fsl_edma_interrupt_handler,
    ctx
  );
#elif defined(LIBBSP_ARM_IMXRT_BSP_H)
  sc = rtems_interrupt_handler_install(
    DMA0_DMA16_IRQn + (channel_index % 16),
    "eDMA Channel",
    RTEMS_INTERRUPT_SHARED,
    fsl_edma_interrupt_handler,
    ctx
  );
#else
  #error "Unknown chip"
#endif
  if (sc == RTEMS_SUCCESSFUL) {
    rtems_chain_prepend(&edma_channel_chain, &ctx->node);
    fsl_edma_enable_error_interrupts(ctx->edma_tcd);
  } else {
    fsl_edma_release_channel_by_tcd(ctx->edma_tcd);
    sc = RTEMS_IO_ERROR;
  }

  return sc;
}

rtems_status_code fsl_edma_obtain_channel(
  fsl_edma_channel_context *ctx,
  unsigned irq_priority
)
{
  rtems_status_code sc = fsl_edma_obtain_channel_by_tcd(ctx->edma_tcd);
  if (sc == RTEMS_SUCCESSFUL) {
    sc = fsl_edma_install_obtained_channel(ctx, irq_priority);
  }

  return sc;
}

rtems_status_code fsl_edma_obtain_next_free_channel(
  fsl_edma_channel_context *ctx
)
{
  rtems_status_code sc;

  sc = fsl_edma_obtain_next_free_channel_and_get_tcd(&ctx->edma_tcd);
  if (sc == RTEMS_SUCCESSFUL) {
    sc = fsl_edma_install_obtained_channel(ctx,
#ifdef LIBBSP_POWERPC_MPC55XXEVB_BSP_H
      MPC55XX_INTC_DEFAULT_PRIORITY
#else
      0
#endif
    );
  }

  return sc;
}

void fsl_edma_release_channel(fsl_edma_channel_context *ctx)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned channel_index = fsl_edma_channel_index_of_tcd(ctx->edma_tcd);

  fsl_edma_release_channel_by_tcd(ctx->edma_tcd);
  rtems_chain_extract(&ctx->node);

  sc = rtems_interrupt_handler_remove(
#if defined(LIBBSP_POWERPC_MPC55XXEVB_BSP_H)
    MPC55XX_IRQ_EDMA(channel_index),
#elif defined(LIBBSP_ARM_IMXRT_BSP_H)
    DMA0_DMA16_IRQn + (channel_index % 16),
#else
  #error "Unknown chip"
#endif
    fsl_edma_interrupt_handler,
    ctx
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(MPC55XX_FATAL_EDMA_IRQ_REMOVE);
  }
}

void fsl_edma_copy(
  volatile struct fsl_edma_tcd *edma_tcd,
  const struct fsl_edma_tcd *source_tcd
)
{
  /* Clear DONE flag */
  edma_tcd->BMF = 0;

  edma_tcd->SADDR = source_tcd->SADDR;
  edma_tcd->SDF = source_tcd->SDF;
  edma_tcd->NBYTES = source_tcd->NBYTES;
  edma_tcd->SLAST = source_tcd->SLAST;
  edma_tcd->DADDR = source_tcd->DADDR;
  edma_tcd->CDF = source_tcd->CDF;
  edma_tcd->DLAST_SGA = source_tcd->DLAST_SGA;
  edma_tcd->BMF = source_tcd->BMF;
}

void fsl_edma_copy_and_enable_hardware_requests(
  volatile struct fsl_edma_tcd *edma_tcd,
  const struct fsl_edma_tcd *source_tcd
)
{
  fsl_edma_copy(edma_tcd, source_tcd);
  fsl_edma_enable_hardware_requests(edma_tcd);
}

void fsl_edma_sg_link(
  volatile struct fsl_edma_tcd *edma_tcd,
  const struct fsl_edma_tcd *source_tcd
)
{
  edma_tcd->DLAST_SGA = (int32_t) source_tcd;
  edma_tcd->BMF |= EDMA_TCD_BMF_E_SG;

  if ((edma_tcd->BMF & EDMA_TCD_BMF_E_SG) == 0) {
    fsl_edma_copy(edma_tcd, source_tcd);
  }
}
