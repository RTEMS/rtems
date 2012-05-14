/**
 * @file rtems/zilog/z8536.h
 *
 * This include file defines information related to a Zilog Z8536
 * Counter/Timer/IO Chip.  It is a IO mapped part.
 *
 * @note This file shares as much as possible with the include
 *       file for the Z8036 via z8x36.h.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_ZILOG_Z8536_H
#define _RTEMS_ZILOG_Z8536_H

#ifdef __cplusplus
extern "C" {
#endif

/* macros */

#define VOL8( ptr )   ((volatile uint8_t   *)(ptr))

#define Z8x36_STATE0 ( z8536 ) \
  { char *garbage; \
    (garbage) = *(VOL8(z8536+0xC)) \
  }

#define Z8x36_WRITE( z8536, reg, data ) \
   *(VOL8(z8536+0xC)) = (reg); \
   *(VOL8(z8536+0xC)) = (data)

#define Z8x36_READ( z8536, reg, data ) \
   *(VOL8(z8536+0xC)) = (reg); \
   (data) = *(VOL8(z8536+0xC))

/* structures */

/* MAIN CONTROL REGISTERS (0x00-0x07) */
#define MASTER_INTR           0x00   /* Master Interrupt Ctl Reg */
#define MASTER_CFG            0x01   /* Master Configuration Ctl Reg */
#define PORTA_VECTOR          0x02   /* Port A - Interrupt Vector */
#define PORTB_VECTOR          0x03   /* Port B - Interrupt Vector */
#define CNT_TMR_VECTOR        0x04   /* Counter/Timer Interrupt Vector */
#define PORTC_DATA_POLARITY   0x05   /* Port C - Data Path Polarity */
#define PORTC_DIRECTION       0x06   /* Port C - Data Direction */
#define PORTC_SPECIAL_IO_CTL  0x07   /* Port C - Special IO Control */

/* MOST OFTEN ACCESSED REGISTERS (0x08 - 0x0f) */
#define PORTA_CMD_STATUS      0x08   /* Port A - Command Status Reg */
#define PORTB_CMD_STATUS      0x09   /* Port B - Command Status Reg */
#define CT1_CMD_STATUS        0x0a   /* Ctr/Timer 1 - Command Status Reg */
#define CT2_CMD_STATUS        0x0b   /* Ctr/Timer 2 - Command Status Reg */
#define CT3_CMD_STATUS        0x0c   /* Ctr/Timer 3 - Command Status Reg */
#define PORTA_DATA            0x0d   /* Port A - Data */
#define PORTB_DATA            0x0e   /* Port B - Data */
#define PORTC_DATA            0x0f   /* Port C - Data */

/* COUNTER/TIMER RELATED REGISTERS (0x10-0x1f) */
#define CT1_CUR_CNT_MSB       0x10   /* Ctr/Timer 1 - Current Count (MSB) */
#define CT1_CUR_CNT_LSB       0x11   /* Ctr/Timer 1 - Current Count (LSB) */
#define CT2_CUR_CNT_MSB       0x12   /* Ctr/Timer 2 - Current Count (MSB) */
#define CT2_CUR_CNT_LSB       0x13   /* Ctr/Timer 2 - Current Count (LSB) */
#define CT3_CUR_CNT_MSB       0x14   /* Ctr/Timer 3 - Current Count (MSB) */
#define CT3_CUR_CNT_LSB       0x15   /* Ctr/Timer 3 - Current Count (LSB) */
#define CT1_TIME_CONST_MSB    0x16   /* Ctr/Timer 1 - Time Constant (MSB) */
#define CT1_TIME_CONST_LSB    0x17   /* Ctr/Timer 1 - Time Constant (LSB) */
#define CT2_TIME_CONST_MSB    0x18   /* Ctr/Timer 2 - Time Constant (MSB) */
#define CT2_TIME_CONST_LSB    0x19   /* Ctr/Timer 2 - Time Constant (LSB) */
#define CT3_TIME_CONST_MSB    0x1a   /* Ctr/Timer 3 - Time Constant (MSB) */
#define CT3_TIME_CONST_LSB    0x1b   /* Ctr/Timer 3 - Time Constant (LSB) */
#define CT1_MODE_SPEC         0x1c   /* Ctr/Timer 1 - Mode Specification  */
#define CT2_MODE_SPEC         0x1d   /* Ctr/Timer 2 - Mode Specification  */
#define CT3_MODE_SPEC         0x1e   /* Ctr/Timer 3 - Mode Specification  */
#define CURRENT_VECTOR        0x1f   /* Current Vector */

/* PORT A SPECIFICATION REGISTERS (0x20 -0x27) */
#define PORTA_MODE            0x20   /* Port A - Mode Specification  */
#define PORTA_HANDSHAKE       0x21   /* Port A - Handshake Specification  */
#define PORTA_DATA_POLARITY   0x22   /* Port A - Data Path Polarity */
#define PORTA_DIRECTION       0x23   /* Port A - Data Direction */
#define PORTA_SPECIAL_IO_CTL  0x24   /* Port A - Special IO Control */
#define PORTA_PATT_POLARITY   0x25   /* Port A - Pattern Polarity */
#define PORTA_PATT_TRANS      0x26   /* Port A - Pattern Transition */
#define PORTA_PATT_MASK       0x27   /* Port A - Pattern Mask */

/* PORT B SPECIFICATION REGISTERS (0x28-0x2f) */
#define PORTB_MODE            0x28   /* Port B - Mode Specification  */
#define PORTB_HANDSHAKE       0x29   /* Port B - Handshake Specification  */
#define PORTB_DATA_POLARITY   0x2a   /* Port B - Data Path Polarity */
#define PORTB_DIRECTION       0x2b   /* Port B - Data Direction */
#define PORTB_SPECIAL_IO_CTL  0x2c   /* Port B - Special IO Control */
#define PORTB_PATT_POLARITY   0x2d   /* Port B - Pattern Polarity */
#define PORTB_PATT_TRANS      0x2e   /* Port B - Pattern Transition */
#define PORTB_PATT_MASK       0x2f   /* Port B - Pattern Mask */

#ifdef __cplusplus
}
#endif

#endif
