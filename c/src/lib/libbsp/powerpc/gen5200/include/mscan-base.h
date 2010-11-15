/**
 * @file
 *
 * @ingroup m
 *
 * @brief MSCAN register definitions and support functions.
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

#ifndef LIBBSP_MSCAN_BASE_H
#define LIBBSP_MSCAN_BASE_H

#include <stdbool.h>

#include <bsp/mpc5200.h>

/**
 * @defgroup m MSCAN
 *
 * @{
 */

#define MSCAN_BIT_RATE_MIN 10000

#define MSCAN_BIT_RATE_MAX 1000000

#define MSCAN_BIT_RATE_DEFAULT 125000

#define MSCAN_FILTER_NUMBER_MIN 0

#define MSCAN_FILTER_NUMBER_2 2

#define MSCAN_FILTER_NUMBER_4 4

#define MSCAN_FILTER_NUMBER_MAX 8

#define MSCAN_FILTER_ID_DEFAULT 0U

#define MSCAN_FILTER_MASK_DEFAULT 0xffffffffU

#define MSCAN_TRANSMIT_BUFFER_NUMBER 3

/**
 * @name MSCAN Control Register 0 (CANCTL0)
 *
 * @{
 */

#define CTL0_RXFRM               (1 << 7)
#define CTL0_RXACT               (1 << 6)
#define CTL0_CSWAI               (1 << 5)
#define CTL0_SYNCH               (1 << 4)
#define CTL0_TIME                (1 << 3)
#define CTL0_WUPE                (1 << 2)
#define CTL0_SLPRQ               (1 << 1)
#define CTL0_INITRQ              (1 << 0)

/** @} */

/**
 * @name MSCAN Control Register 1 (CANCTL1)
 *
 * @{
 */

#define CTL1_CANE                (1 << 7)
#define CTL1_CLKSRC              (1 << 6)
#define CTL1_LOOPB               (1 << 5)
#define CTL1_LISTEN              (1 << 4)
#define CTL1_WUPM                (1 << 2)
#define CTL1_SLPAK               (1 << 1)
#define CTL1_INITAK              (1 << 0)

/** @} */

/**
 * @name MSCAN Bus Timing Register 0 (CANBTR0)
 *
 * @{
 */

#define BTR0_SJW_MASK            0xc0
#define BTR0_BRP_MASK            0x3f

#define BTR0_SJW( btr0)          ((btr0) << 6)
#define BTR0_BRP( btr0)          ((btr0) << 0)

#define BTR0_GET_SJW( btr0)      (((btr0) & BTR0_SJW_MASK) >> 6)
#define BTR0_GET_BRP( btr0)      (((btr0) & BTR0_BRP_MASK) >> 0)

/** @} */

/**
 * @name MSCAN Bus Timing Register 1 (CANBTR1)
 *
 * @{
 */

#define BTR1_SAMP_MASK           0x80
#define BTR1_TSEG1_MASK          0x0f
#define BTR1_TSEG2_MASK          0x70

#define BTR1_SAMP                (1 << 7)
#define BTR1_TSEG1( btr1)        ((btr1) << 0)
#define BTR1_TSEG2( btr1)        ((btr1) << 4)

#define BTR1_GET_TSEG1( btr0)    (((btr0) & BTR1_TSEG1_MASK) >> 0)
#define BTR1_GET_TSEG2( btr0)    (((btr0) & BTR1_TSEG2_MASK) >> 4)

/** @} */

/**
 * @name MSCAN Receiver Flag Register (CANRFLG)
 *
 * @{
 */

#define RFLG_WUPIF               (1 << 7)
#define RFLG_CSCIF               (1 << 6)
#define RFLG_RSTAT_MASK          (3 << 4)
#define RFLG_RSTAT_OK            (0 << 4)
#define RFLG_RSTAT_WRN           (1 << 4)
#define RFLG_RSTAT_ERR           (2 << 4)
#define RFLG_RSTAT_OFF           (3 << 4)
#define RFLG_TSTAT_MASK          (3 << 2)
#define RFLG_TSTAT_OK            (0 << 2)
#define RFLG_TSTAT_WRN           (1 << 2)
#define RFLG_TSTAT_ERR           (2 << 2)
#define RFLG_TSTAT_OFF           (3 << 2)
#define RFLG_OVRIF               (1 << 1)
#define RFLG_RXF                 (1 << 0)
#define RFLG_GET_RX_STATE(rflg)  (((rflg) >> 4) & 0x03)
#define RFLG_GET_TX_STATE(rflg)  (((rflg) >> 2) & 0x03)

/** @} */

/**
 * @name MSCAN Receiver Interrupt Enable Register (CANRIER)
 *
 * @{
 */

#define RIER_WUPIE               (1 << 7)
#define RIER_CSCIE               (1 << 6)
#define RIER_RSTAT(rier)         ((rier) << 4)
#define RIER_TSTAT(rier)         ((rier) << 2)
#define RIER_OVRIE               (1 << 1)
#define RIER_RXFIE               (1 << 0)

/** @} */

/**
 * @name MSCAN Transmitter Flag Register (CANTFLG)
 *
 * @{
 */

#define TFLG_TXE2                (1 << 2)
#define TFLG_TXE1                (1 << 1)
#define TFLG_TXE0                (1 << 0)

/** @} */

/**
 * @name MSCAN Transmitter Interrupt Enable Register (CANTIER)
 *
 * @{
 */

#define TIER_TXEI2               (1 << 2)
#define TIER_TXEI1               (1 << 1)
#define TIER_TXEI0               (1 << 0)

/** @} */

/**
 * @name MSCAN Transmitter Message Abort Request (CANTARQ)
 *
 * @{
 */

#define TARQ_ABTRQ2              (1 << 2)
#define TARQ_ABTRQ1              (1 << 1)
#define TARQ_ABTRQ0              (1 << 0)

/** @} */

/**
 * @name MSCAN Transmitter Message Abort Acknoledge (CANTAAK)
 *
 * @{
 */

#define TAAK_ABTRQ2              (1 << 2)
#define TAAK_ABTRQ1              (1 << 1)
#define TAAK_ABTRQ0              (1 << 0)

/** @} */

/**
 * @name MSCAN Transmit Buffer Selection (CANBSEL)
 *
 * @{
 */

#define BSEL_TX2                 (1 << 2)
#define BSEL_TX1                 (1 << 1)
#define BSEL_TX0                 (1 << 0)

/** @} */

/**
 * @name MSCAN ID Acceptance Control Register (CANIDAC)
 *
 * @{
 */

#define IDAC_IDAM1               (1 << 5)
#define IDAC_IDAM0               (1 << 4)
#define IDAC_IDAM                (IDAC_IDAM1 | IDAC_IDAM0)
#define IDAC_IDHIT( idac)        ((idac) & 0x7)

/** @} */

/**
 * @brief MSCAN registers.
 */
typedef struct mpc5200_mscan mscan;

/**
 * @brief MSCAN context that has to be saved throughout the initialization
 * mode.
 */
typedef struct {
  uint8_t ctl0;
  uint8_t rier;
  uint8_t tier;
} mscan_context;

bool mscan_enable( volatile mscan *m, unsigned bit_rate);

void mscan_disable( volatile mscan *m);

void mscan_interrupts_disable( volatile mscan *m);

bool mscan_set_bit_rate( volatile mscan *m, unsigned bit_rate);

void mscan_initialization_mode_enter( volatile mscan *m, mscan_context *context);

void mscan_initialization_mode_leave( volatile mscan *m, const mscan_context *context);

void mscan_sleep_mode_enter( volatile mscan *m);

void mscan_sleep_mode_leave( volatile mscan *m);

volatile uint8_t *mscan_id_acceptance_register( volatile mscan *m, unsigned i);

volatile uint8_t *mscan_id_mask_register( volatile mscan *m, unsigned i);

unsigned mscan_filter_number( volatile mscan *m);

bool mscan_set_filter_number( volatile mscan *m, unsigned number);

bool mscan_filter_operation( volatile mscan *m, bool set, unsigned index, uint32_t *id, uint32_t *mask);

void mscan_filter_clear( volatile mscan *m);

void mscan_get_error_counters( volatile mscan *m, unsigned *rec, unsigned *tec);

/** @} */

#endif /* LIBBSP_MSCAN_BASE_H */
