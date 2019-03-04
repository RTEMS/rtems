/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBCPU_POWERPC_MPC55XX_EDMA_H
#define LIBCPU_POWERPC_MPC55XX_EDMA_H

#include <mpc55xx/regs.h>

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if MPC55XX_CHIP_FAMILY == 551
  #define EDMA_CHANNEL_COUNT 16U
#elif MPC55XX_CHIP_FAMILY == 564
  #define EDMA_CHANNEL_COUNT 16U
#elif MPC55XX_CHIP_FAMILY == 567
  #define EDMA_CHANNEL_COUNT 96U
#else
  #define EDMA_CHANNEL_COUNT 64U
#endif

#define EDMA_MODULE_COUNT ((EDMA_CHANNEL_COUNT + 63U) / 64U)

#define EDMA_CHANNELS_PER_MODULE 64U

#if EDMA_MODULE_COUNT == 1
  #define EDMA_TCD_BY_CHANNEL_INDEX(channel_index) \
    (&EDMA.TCD[(channel_index)])
#elif EDMA_MODULE_COUNT == 2
  #define EDMA_TCD_BY_CHANNEL_INDEX(channel_index) \
    ((channel_index) < EDMA_CHANNELS_PER_MODULE ? \
      &EDMA_A.TCD[(channel_index)] \
        : &EDMA_B.TCD[(channel_index) - EDMA_CHANNELS_PER_MODULE])
#else
  #error "unsupported module count"
#endif

/* FIXME: These values are only valid for the MPC5566 and MPC5674F */
typedef enum {
  EDMA_EQADC_A_FISR0_CFFF0 = 0,
  EDMA_EQADC_A_FISR0_RFDF0 = 1,
  EDMA_EQADC_A_FISR1_CFFF1 = 2,
  EDMA_EQADC_A_FISR1_RFDF1 = 3,
  EDMA_EQADC_A_FISR2_CFFF2 = 4,
  EDMA_EQADC_A_FISR2_RFDF2 = 5,
  EDMA_EQADC_A_FISR3_CFFF3 = 6,
  EDMA_EQADC_A_FISR3_RFDF3 = 7,
  EDMA_EQADC_A_FISR4_CFFF4 = 8,
  EDMA_EQADC_A_FISR4_RFDF4 = 9,
  EDMA_EQADC_A_FISR5_CFFF5 = 10,
  EDMA_EQADC_A_FISR5_RFDF5 = 11,
  EDMA_DSPI_B_SR_TFFF = 12,
  EDMA_DSPI_B_SR_RFDF = 13,
  EDMA_DSPI_C_SR_TFFF = 14,
  EDMA_DSPI_C_SR_RFDF = 15,
  EDMA_DSPI_D_SR_TFFF = 16,
  EDMA_DSPI_D_SR_RFDF = 17,
  EDMA_ESCI_A_COMBTX = 18,
  EDMA_ESCI_A_COMBRX = 19,
  EDMA_EMIOS_GFR_F0 = 20,
  EDMA_EMIOS_GFR_F1 = 21,
  EDMA_EMIOS_GFR_F2 = 22,
  EDMA_EMIOS_GFR_F3 = 23,
  EDMA_EMIOS_GFR_F4 = 24,
  EDMA_EMIOS_GFR_F8 = 25,
  EDMA_EMIOS_GFR_F9 = 26,
  EDMA_ETPU_CDTRSR_A_DTRS0 = 27,
  EDMA_ETPU_CDTRSR_A_DTRS1 = 28,
  EDMA_ETPU_CDTRSR_A_DTRS2 = 29,
  EDMA_ETPU_CDTRSR_A_DTRS14 = 30,
  EDMA_ETPU_CDTRSR_A_DTRS15 = 31,
  EDMA_DSPI_A_SR_TFFF = 32,
  EDMA_DSPI_A_SR_RFDF = 33,
  EDMA_ESCI_B_COMBTX = 34,
  EDMA_ESCI_B_COMBRX = 35,
  EDMA_EMIOS_GFR_F6 = 36,
  EDMA_EMIOS_GFR_F7 = 37,
  EDMA_EMIOS_GFR_F10 = 38,
  EDMA_EMIOS_GFR_F11 = 39,
  EDMA_EMIOS_GFR_F16 = 40,
  EDMA_EMIOS_GFR_F17 = 41,
  EDMA_EMIOS_GFR_F18 = 42,
  EDMA_EMIOS_GFR_F19 = 43,
  EDMA_ETPU_CDTRSR_A_DTRS12 = 44,
  EDMA_ETPU_CDTRSR_A_DTRS13 = 45,
  EDMA_ETPU_CDTRSR_A_DTRS28 = 46,
  EDMA_ETPU_CDTRSR_A_DTRS29 = 47,
  EDMA_SIU_EISR_EIF0 = 48,
  EDMA_SIU_EISR_EIF1 = 49,
  EDMA_SIU_EISR_EIF2 = 50,
  EDMA_SIU_EISR_EIF3 = 51,
  EDMA_ETPU_CDTRSR_B_DTRS0 = 52,
  EDMA_ETPU_CDTRSR_B_DTRS1 = 53,
  EDMA_ETPU_CDTRSR_B_DTRS2 = 54,
  EDMA_ETPU_CDTRSR_B_DTRS3 = 55,
  EDMA_ETPU_CDTRSR_B_DTRS12 = 56,
  EDMA_ETPU_CDTRSR_B_DTRS13 = 57,
  EDMA_ETPU_CDTRSR_B_DTRS14 = 58,
  EDMA_ETPU_CDTRSR_B_DTRS15 = 59,
  EDMA_ETPU_CDTRSR_B_DTRS28 = 60,
  EDMA_ETPU_CDTRSR_B_DTRS29 = 61,
  EDMA_ETPU_CDTRSR_B_DTRS30 = 62,
  EDMA_ETPU_CDTRSR_B_DTRS31 = 63
  #if MPC55XX_CHIP_FAMILY == 567
    ,
    EDMA_EQADC_B_FISR0_CFFF0 = 64 + 0,
    EDMA_EQADC_B_FISR0_RFDF0 = 64 + 1,
    EDMA_EQADC_B_FISR1_CFFF1 = 64 + 2,
    EDMA_EQADC_B_FISR1_RFDF1 = 64 + 3,
    EDMA_EQADC_B_FISR2_CFFF2 = 64 + 4,
    EDMA_EQADC_B_FISR2_RFDF2 = 64 + 5,
    EDMA_EQADC_B_FISR3_CFFF3 = 64 + 6,
    EDMA_EQADC_B_FISR3_RFDF3 = 64 + 7,
    EDMA_EQADC_B_FISR4_CFFF4 = 64 + 8,
    EDMA_EQADC_B_FISR4_RFDF4 = 64 + 9,
    EDMA_EQADC_B_FISR5_CFFF5 = 64 + 10,
    EDMA_EQADC_B_FISR5_RFDF5 = 64 + 11,
    EDMA_DECFILTER_A_IB = 64 + 12,
    EDMA_DECFILTER_A_OB = 64 + 13,
    EDMA_DECFILTER_B_IB = 64 + 14,
    EDMA_DECFILTER_B_OB = 64 + 15,
    EDMA_DECFILTER_C_IB = 64 + 16,
    EDMA_DECFILTER_C_OB = 64 + 17,
    EDMA_DECFILTER_D_IB = 64 + 18,
    EDMA_DECFILTER_D_OB = 64 + 19,
    EDMA_DECFILTER_E_IB = 64 + 20,
    EDMA_DECFILTER_E_OB = 64 + 21,
    EDMA_DECFILTER_F_IB = 64 + 22,
    EDMA_DECFILTER_F_OB = 64 + 23,
    EDMA_DECFILTER_G_IB = 64 + 24,
    EDMA_DECFILTER_G_OB = 64 + 25,
    EDMA_DECFILTER_H_IB = 64 + 26,
    EDMA_DECFILTER_H_OB = 64 + 27
  #endif
} edma_channel;

typedef struct edma_channel_context {
  rtems_chain_node node;
  volatile struct tcd_t *edma_tcd;
  void (*done)(struct edma_channel_context *, uint32_t);
} edma_channel_context;

void mpc55xx_edma_init(void);

/**
 * @brief Obtains an eDMA channel.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_RESOURCE_IN_USE The channel is already in use.
 */
rtems_status_code mpc55xx_edma_obtain_channel_by_tcd(
  volatile struct tcd_t *edma_tcd
);

void mpc55xx_edma_release_channel_by_tcd(volatile struct tcd_t *edma_tcd);

/**
 * @brief Obtains an eDMA channel and registers the channel context.
 *
 * The done handler of the channel context will be called
 * - during minor or major loop completions if interrupts are enabled in the
 *   corresponding TCD, or
 * - in case a channel error occurs.
 *
 * An error status value not equal to zero indicates an error.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_RESOURCE_IN_USE The channel is already in use.
 * @retval RTEMS_IO_ERROR Unable to install interrupt handler for this channel.
 */
rtems_status_code mpc55xx_edma_obtain_channel(
  edma_channel_context *ctx,
  unsigned irq_priority
);

void mpc55xx_edma_release_channel(edma_channel_context *ctx);

/**
 * @brief Copies a source TCD to an eDMA TCD.
 *
 * The DONE flag of the eDMA TCD is cleared before the actual copy operation.
 * This enables the setting of channel link or scatter/gather options.
 *
 * This function can be used to start the channel if the START flags is
 * set in the source TCD.
 */
void mpc55xx_edma_copy(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
);

/**
 * @brief Copies a source TCD to an eDMA TCD and enables hardware requests.
 *
 * The DONE flag of the eDMA TCD is cleared before the actual copy operation.
 * This enables the setting of channel link or scatter/gather options.
 */
void mpc55xx_edma_copy_and_enable_hardware_requests(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
);

void mpc55xx_edma_sg_link(
  volatile struct tcd_t *edma_tcd,
  const struct tcd_t *source_tcd
);

static inline volatile struct EDMA_tag *mpc55xx_edma_by_tcd(
  volatile struct tcd_t *edma_tcd
)
{
  return (volatile struct EDMA_tag *)
    ((uintptr_t) edma_tcd & ~(uintptr_t) 0x1fff);
}

static inline unsigned mpc55xx_edma_channel_by_tcd(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);

  return edma_tcd - &edma->TCD[0];
}

static inline void mpc55xx_edma_enable_hardware_requests(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->SERQR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_disable_hardware_requests(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->CERQR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_enable_error_interrupts(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->SEEIR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_disable_error_interrupts(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->CEEIR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_set_start(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->SSBR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_clear_done(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->CDSBR.R = (uint8_t) channel;
}

static inline void mpc55xx_edma_clear_interrupts(
  volatile struct tcd_t *edma_tcd
)
{
  volatile struct EDMA_tag *edma = mpc55xx_edma_by_tcd(edma_tcd);
  unsigned channel = edma_tcd - &edma->TCD[0];

  edma->CIRQR.R = (uint8_t) channel;
}

static inline bool mpc55xx_edma_is_done(
  volatile struct tcd_t *edma_tcd
)
{
  return edma_tcd->BMF.B.DONE;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_EDMA_H */
