/**
 * @file
 *
 * @ingroup m
 *
 * @brief MSCAN support functions code.
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

#include <bsp.h>
#include <bsp/mscan-base.h>

#define MIN_NO_OF_TQ         7
#define TSEG_1               1
#define TSEG_2               2
#define NO_OF_TABLE_ENTRIES  4
#define SJW                  3

#define CAN_MAX_NO_OF_TQ         25
#define CAN_MAX_NO_OF_TQ_TSEG1   15
#define CAN_MAX_NO_OF_TQ_TSEG2   7
#define CAN_MAX_NO_OF_TQ_SJW     2

/**
 * Time segmant table.
 *
 * <table>
 *   <tr>
 *     <td>Number of time quantas</th>
 *     <td>Time Segment 1</th>
 *     <td>Time Segment 2</th>
 *     <td>Sync: Jump width</th>
 *   </tr>
 * </table>
 */
static uint8_t can_time_segment_table
  [CAN_MAX_NO_OF_TQ - MIN_NO_OF_TQ + 1] [NO_OF_TABLE_ENTRIES] = {
  {7, 4, 2, 1},
  {8, 5, 2, 1},
  {9, 6, 2, 2},
  {10, 6, 3, 2},
  {11, 7, 3, 2},
  {12, 8, 3, 2},
  {13, 8, 4, 2},
  {14, 9, 4, 2},
  {15, 10, 4, 2},
  {16, 10, 5, 2},
  {17, 11, 5, 2},
  {18, 12, 5, 2},
  {19, 12, 6, 2},
  {20, 13, 6, 2},
  {21, 14, 6, 2},
  {22, 14, 7, 2},
  {23, 15, 7, 2},
  {24, 15, 8, 2},
  {25, 16, 8, 2}
};

/**
 * @brief Calculates the MSCAN clock prescaler value.
 */
static uint8_t prescaler_calculation(
  unsigned can_bit_rate,
  unsigned can_clock_frq,
  uint8_t *tq_no
)
{

/* local variables */
  uint8_t presc_val,
    tq_no_dev_min = 0;
  uint32_t bit_rate,
    bit_rate_dev,
    frq_tq,
    bit_rate_dev_min = 0xFFFFFFFF;

/* loop through all values of time quantas */
  for (*tq_no = CAN_MAX_NO_OF_TQ; *tq_no >= MIN_NO_OF_TQ; (*tq_no)--) {

    /* calculate time quanta freq. */
    frq_tq = *tq_no * can_bit_rate;

    /* calculate the optimized prescal. val. */
    presc_val = (can_clock_frq + frq_tq / 2) / frq_tq;

    /* calculate the bitrate */
    bit_rate = can_clock_frq / (*tq_no * presc_val);

    /* calculate the bitrate deviation */
    if (can_bit_rate >= bit_rate) {
      /* calculate the bitrate deviation */
      bit_rate_dev = can_bit_rate - bit_rate;
    } else {
      /* calculate the bitrate deviation */
      bit_rate_dev = bit_rate - can_bit_rate;
    }

    /* check the deviation freq. */
    if (bit_rate_dev == 0) {

      /* return if best match (zero deviation) */
      return (uint8_t) (presc_val);
    } else {

      /* check for minimum of bit rate deviation */
      if (bit_rate_dev < bit_rate_dev_min) {

        /* recognize the minimum freq. deviation */
        bit_rate_dev_min = bit_rate_dev;

        /* recognize the no. of time quantas */
        tq_no_dev_min = *tq_no;
      }
    }
  }

  /* get the no of tq's */
  *tq_no = tq_no_dev_min;

  /* calculate time quanta freq. */
  frq_tq = *tq_no * can_bit_rate;

  /* return the optimized prescaler value */
  return (uint8_t) ((can_clock_frq + frq_tq / 2) / frq_tq);
}

/**
 * @brief Sets the bit rate for the MSCAN module @a m to @a can_bit_rate
 * in [bits/s].
 */
bool mscan_set_bit_rate( volatile mscan *m, unsigned can_bit_rate)
{
  mscan_context context;
  unsigned prescale_val = 0;
  uint8_t tq_no,
    tseg_1,
    tseg_2,
    sseg;

  if (can_bit_rate < MSCAN_BIT_RATE_MIN || can_bit_rate > MSCAN_BIT_RATE_MAX) {
    return false;
  }

  /* Enter initialization mode */
  mscan_initialization_mode_enter( m, &context);

  /* get optimized prescaler value */
  prescale_val = prescaler_calculation(can_bit_rate, IPB_CLOCK, &tq_no);

  /* Check prescaler value */
  if (prescale_val > 64) {
    /* Leave initialization mode */
    mscan_initialization_mode_leave( m, &context);

    return false;
  }

  /* get time segment length from time segment table */
  tseg_1 = can_time_segment_table[tq_no - MIN_NO_OF_TQ][TSEG_1];
  tseg_2 = can_time_segment_table[tq_no - MIN_NO_OF_TQ][TSEG_2];
  sseg = can_time_segment_table[tq_no - MIN_NO_OF_TQ][SJW];

  /* Bus Timing Register 0 MSCAN_A/_B ------------------------------ */
  /*    [07]:SJW1        1 : Synchronization jump width, Bit1       */
  /*    [06]:SJW0        0 : Synchronization jump width, Bit0       */
  /*                         SJW = 2 -> 3 Tq clock cycles           */
  /*    [05]:BRP5        0 : Baud Rate Prescaler, Bit 5             */
  /*    [04]:BRP4        0 : Baud Rate Prescaler, Bit 4             */
  /*    [03]:BRP3        0 : Baud Rate Prescaler, Bit 3             */
  /*    [02]:BRP2        1 : Baud Rate Prescaler, Bit 2             */
  /*    [01]:BRP1        0 : Baud Rate Prescaler, Bit 1             */
  /*    [00]:BRP0        1 : Baud Rate Prescaler, Bit 0             */
  m->btr0 = (BTR0_SJW(sseg - 1) | BTR0_BRP(prescale_val - 1));

  /* Bus Timing Register 1 MSCAN_A/_B ------------------------------ */
  /*    [07]:SAMP        0 : One Sample per bit                     */
  /*    [06]:TSEG22      0 : Time Segment 2, Bit 2                  */
  /*    [05]:TSEG21      1 : Time Segment 2, Bit 1                  */
  /*    [04]:TSEG20      0 : Time Segment 2, Bit 0                  */
  /*                         -> PHASE_SEG2 = 3 Tq                   */
  /*    [03]:TSEG13      0 : Time Segment 1, Bit 3                  */
  /*    [02]:TSEG12      1 : Time Segment 1, Bit 2                  */
  /*    [01]:TSEG11      1 : Time Segment 1, Bit 1                  */
  /*    [00]:TSEG10      0 : Time Segment 1, Bit 0                  */
  m->btr1 = (BTR1_TSEG2(tseg_2 - 1) | BTR1_TSEG1(tseg_1 - 1));

  /* Leave initialization mode */
  mscan_initialization_mode_leave( m, &context);

  return true;
}

/**
 * @brief Disables all interrupts for the MSCAN module @a m.
 */
void mscan_interrupts_disable( volatile mscan *m)
{
  m->rier = 0;
  m->tier = 0;
}

/**
 * @brief Enter initialization mode for the MSCAN module @a m.
 *
 * Saves the current MSCAN context in @a context.
 */
void mscan_initialization_mode_enter( volatile mscan *m, mscan_context *context)
{
  /* Save context */
  context->ctl0 = m->ctl0 & CTL0_TIME;
  context->rier = m->rier;
  context->tier = m->tier;

  /* Request initialization mode */
  m->ctl0 |= CTL0_INITRQ;

  /* Wait for initialization mode acknowledge */
  while ((m->ctl1 & CTL1_INITAK) == 0) {
    /* Wait */
  }
}

/**
 * @brief Leave initialization mode for the MSCAN module @a m.
 *
 * Saves the previous MSCAN context saved in @a context.
 */
void mscan_initialization_mode_leave( volatile mscan *m, const mscan_context *context)
{
  /* Clear initialization mode request */
  m->ctl0 &= ~CTL0_INITRQ;

  /* Wait for clearing of initialization mode acknowledge */
  while ((m->ctl1 & CTL1_INITAK) != 0) {
    /* Wait */
  }

  /* Leave sleep mode */
  mscan_sleep_mode_leave( m);

  /* Restore context */
  m->ctl0 |= context->ctl0;
  m->rier |= context->rier;
  m->tier |= context->tier;
}

/**
 * @brief Enter sleep mode for the MSCAN module @a m.
 */
void mscan_sleep_mode_enter( volatile mscan *m)
{
  /* Request sleep mode */
  m->ctl0 |= CTL0_SLPRQ;
}

/**
 * @brief Leave sleep mode for the MSCAN module @a m.
 */
void mscan_sleep_mode_leave( volatile mscan *m)
{
  /* Clear sleep mode request */
  m->ctl0 &= ~CTL0_SLPRQ;

  /* Wait for clearing of sleep mode acknowledge */
  while ((m->ctl1 & CTL1_SLPAK) != 0) {
    /* Wait */
  }
}

/**
 * @brief Enables and initializes the MSCAN module @a m.
 *
 * The module is set to listen only mode.
 */
bool mscan_enable( volatile mscan *m, unsigned bit_rate)
{
  bool s = true;

  /* Disable the module */
  mscan_disable( m);

  /* Enable module in listen only */
  m->ctl1 = CTL1_CANE | CTL1_LISTEN;

  /* Close acceptance filters */
  m->idac = IDAC_IDAM1 | IDAC_IDAM0;

  /* Clear filter */
  mscan_filter_clear( m);

  /* Set bit rate and leave initialization mode */
  s = mscan_set_bit_rate( m, bit_rate);

  /* Clear all flags */
  m->ctl0 = CTL0_RXFRM;

  /* Disable interrupts */
  mscan_interrupts_disable( m);

  return s;
}

/**
 * @brief Disables the MSCAN module @a m.
 *
 * The module is set to sleep mode and disabled afterwards.
 */
void mscan_disable( volatile mscan *m)
{
  mscan_context context;

  /* Disable interrupts */
  mscan_interrupts_disable( m);

  /* Enter initialization mode */
  mscan_initialization_mode_enter( m, &context);

  /* Disable module */
  m->ctl1 &= ~CTL1_CANE;
}

/**
 * @brief Sets the filter ID and mask registers of the MSCAN module @a m to
 * default values.
 */
void mscan_filter_clear( volatile mscan *m)
{
  mscan_context context;

  mscan_initialization_mode_enter( m, &context);

  /* Setup ID acceptance registers */
  m->idar0 = MSCAN_FILTER_ID_DEFAULT;
  m->idar1 = MSCAN_FILTER_ID_DEFAULT;
  m->idar2 = MSCAN_FILTER_ID_DEFAULT;
  m->idar3 = MSCAN_FILTER_ID_DEFAULT;
  m->idar4 = MSCAN_FILTER_ID_DEFAULT;
  m->idar5 = MSCAN_FILTER_ID_DEFAULT;
  m->idar6 = MSCAN_FILTER_ID_DEFAULT;
  m->idar7 = MSCAN_FILTER_ID_DEFAULT;

  /* Setup ID mask registers */
  m->idmr0 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr1 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr2 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr3 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr4 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr5 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr6 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;
  m->idmr7 = (uint8_t) MSCAN_FILTER_MASK_DEFAULT;

  mscan_initialization_mode_leave( m, &context);
}

/**
 * @brief Returns the number of active filters of the MSCAN module @a m.
 *
 * @see MSCAN_FILTER_NUMBER_MIN, MSCAN_FILTER_NUMBER_2, MSCAN_FILTER_NUMBER_4
 * and MSCAN_FILTER_NUMBER_MAX.
 */
unsigned mscan_filter_number( volatile mscan *m)
{
  uint8_t idam = m->idac & IDAC_IDAM;

  switch (idam) {
    case 0:
      return MSCAN_FILTER_NUMBER_2;
    case IDAC_IDAM0:
      return MSCAN_FILTER_NUMBER_4;
    case IDAC_IDAM1:
      return MSCAN_FILTER_NUMBER_MAX;
    default:
      return MSCAN_FILTER_NUMBER_MIN;
  }
}

/**
 * @brief Sets the number of active filters of the MSCAN module @a m to @a
 * number and returns true if @a number is valid.
 *
 * @see MSCAN_FILTER_NUMBER_MIN, MSCAN_FILTER_NUMBER_2, MSCAN_FILTER_NUMBER_4
 * and MSCAN_FILTER_NUMBER_MAX.
 */
bool mscan_set_filter_number( volatile mscan *m, unsigned number)
{
  mscan_context context;
  uint8_t idac = IDAC_IDAM1 | IDAC_IDAM0;

  switch (number) {
    case MSCAN_FILTER_NUMBER_MIN:
      break;
    case MSCAN_FILTER_NUMBER_2:
      idac = 0;
      break;
    case MSCAN_FILTER_NUMBER_4:
      idac = IDAC_IDAM0;
      break;
    case MSCAN_FILTER_NUMBER_MAX:
      idac = IDAC_IDAM1;
      break;
    default:
      return false;
  }

  mscan_initialization_mode_enter( m, &context);

  m->idac = idac;

  mscan_initialization_mode_leave( m, &context);

  /* Clear filter */
  mscan_filter_clear( m);

  return true;
}

/**
 * @brief Returns the  address of the CANIDAR register with index @a i of the
 * MSCAN module @a m.
 *
 * @warning The index @a i is not checked if it is in range.
 */
volatile uint8_t *mscan_id_acceptance_register( volatile mscan *m, unsigned i)
{
  volatile uint8_t *const idar [8] = {
    &m->idar0,
    &m->idar1,
    &m->idar2,
    &m->idar3,
    &m->idar4,
    &m->idar5,
    &m->idar6,
    &m->idar7
  };

  return idar [i];
}

/**
 * @brief Returns the  address of the CANIDMR register with index @a i of the
 * MSCAN module @a m.
 *
 * @warning The index @a i is not checked if it is in range.
 */
volatile uint8_t *mscan_id_mask_register( volatile mscan *m, unsigned i)
{
  volatile uint8_t *const idmr [8] = {
    &m->idmr0,
    &m->idmr1,
    &m->idmr2,
    &m->idmr3,
    &m->idmr4,
    &m->idmr5,
    &m->idmr6,
    &m->idmr7
  };

  return idmr [i];
}

/**
 * @brief Sets or gets the filter ID and mask in @a id and @a mask depending on
 * @a set of MSCAN module @a m.  The filter is selected by the value of @a
 * index.
 *
 * Returns true if the operation was successful.
 */
bool mscan_filter_operation(
  volatile mscan *m,
  bool set,
  unsigned index,
  uint32_t *id,
  uint32_t *mask
)
{
  unsigned number = mscan_filter_number( m);
  unsigned offset = MSCAN_FILTER_NUMBER_MAX / number;
  unsigned shift = 24;

  volatile uint8_t *idar = NULL;
  volatile uint8_t *idmr = NULL;

  if (!set) {
    *id = MSCAN_FILTER_ID_DEFAULT;
    *mask = MSCAN_FILTER_MASK_DEFAULT;
  }

  if (index < number) {
    mscan_context context;

    mscan_initialization_mode_enter( m, &context);

    index *= offset;
    offset += index;
    while (index < offset) {
      idar = mscan_id_acceptance_register( m, index);
      idmr = mscan_id_mask_register( m, index);

      if (set) {
        *idar = (uint8_t) (*id >> shift);
        *idmr = (uint8_t) (*mask >> shift);
      } else {
        *id = (*id & ~(0xffU << shift)) | (*idar << shift);
        *mask = (*mask & ~(0xffU << shift)) | (*idmr << shift);
      }

      shift -= 8;

      ++index;
    }

    mscan_initialization_mode_leave( m, &context);
  } else {
    return false;
  }

  return true;
}

/**
 * @brief Returns the receiver and transmitter error counter values in @a rec
 * and @a tec of MSCAN module @a m.
 */
void mscan_get_error_counters( volatile mscan *m, unsigned *rec, unsigned *tec)
{
  mscan_context context;

  mscan_initialization_mode_enter( m, &context);

  *rec = m->rxerr;
  *tec = m->txerr;

  mscan_initialization_mode_leave( m, &context);
}
