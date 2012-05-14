/**
 * @file rtems/zilog/z8036.h
 *
 * This include file defines information related to a Zilog Z8036
 * Counter/Timer/IO Chip.  It is a memory mapped part.
 *
 * @note This file shares as much as possible with the include file
 * for the Z8536 via z8x36.h.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_ZILOG_Z8036_H
#define _RTEMS_ZILOG_Z8036_H

#ifdef __cplusplus
extern "C" {
#endif

/* macros */

#define Z8036( ptr ) ((volatile struct z8036_map *)(ptr))

#define Z8x36_STATE0 ( z8036 ) \
  { /*char *garbage = *(Z8036(z8036))->???; */ }


#define Z8x36_WRITE( z8036, reg, data ) \
   (Z8036(z8036))->reg = (data)


#define Z8x36_READ( z8036, reg, data ) \
   (Z8036(z8036))->reg = (data)

/* structures */

struct z8036_map {
/* MAIN CONTROL REGISTERS (0x00-0x07) */
  uint8_t   MASTER_INTR;           /* Master Interrupt Ctl Reg */
  uint8_t   MASTER_CFG;            /* Master Configuration Ctl Reg */
  uint8_t   PORTA_VECTOR;          /* Port A - Interrupt Vector */
  uint8_t   PORTB_VECTOR;          /* Port B - Interrupt Vector */
  uint8_t   CNT_TMR_VECTOR;        /* Counter/Timer Interrupt Vector */
  uint8_t   PORTC_DATA_POLARITY;   /* Port C - Data Path Polarity */
  uint8_t   PORTC_DIRECTION;       /* Port C - Data Direction */
  uint8_t   PORTC_SPECIAL_IO_CTL;  /* Port C - Special IO Control */
/* MOST OFTEN ACCESSED REGISTERS (0x08 - 0x0f) */
  uint8_t   PORTA_CMD_STATUS;      /* Port A - Command Status Reg */
  uint8_t   PORTB_CMD_STATUS;      /* Port B - Command Status Reg */
  uint8_t   CT1_CMD_STATUS;        /* Ctr/Timer 1 - Command Status Reg */
  uint8_t   CT2_CMD_STATUS;        /* Ctr/Timer 2 - Command Status Reg */
  uint8_t   CT3_CMD_STATUS;        /* Ctr/Timer 3 - Command Status Reg */
  uint8_t   PORTA_DATA;            /* Port A - Data */
  uint8_t   PORTB_DATA;            /* Port B - Data */
  uint8_t   PORTC_DATA;            /* Port C - Data */
/* COUNTER/TIMER RELATED REGISTERS (0x10-0x1f) */
  uint8_t   CT1_CUR_CNT_MSB;       /* Ctr/Timer 1 - Current Count (MSB) */
  uint8_t   CT1_CUR_CNT_LSB;       /* Ctr/Timer 1 - Current Count (LSB) */
  uint8_t   CT2_CUR_CNT_MSB;       /* Ctr/Timer 2 - Current Count (MSB) */
  uint8_t   CT2_CUR_CNT_LSB;       /* Ctr/Timer 2 - Current Count (LSB) */
  uint8_t   CT3_CUR_CNT_MSB;       /* Ctr/Timer 3 - Current Count (MSB) */
  uint8_t   CT3_CUR_CNT_LSB;       /* Ctr/Timer 3 - Current Count (LSB) */
  uint8_t   CT1_TIME_CONST_MSB;    /* Ctr/Timer 1 - Time Constant (MSB) */
  uint8_t   CT1_TIME_CONST_LSB;    /* Ctr/Timer 1 - Time Constant (LSB) */
  uint8_t   CT2_TIME_CONST_MSB;    /* Ctr/Timer 2 - Time Constant (MSB) */
  uint8_t   CT2_TIME_CONST_LSB;    /* Ctr/Timer 2 - Time Constant (LSB) */
  uint8_t   CT3_TIME_CONST_MSB;    /* Ctr/Timer 3 - Time Constant (MSB) */
  uint8_t   CT3_TIME_CONST_LSB;    /* Ctr/Timer 3 - Time Constant (LSB) */
  uint8_t   CT1_MODE_SPEC;         /* Ctr/Timer 1 - Mode Specification  */
  uint8_t   CT2_MODE_SPEC;         /* Ctr/Timer 2 - Mode Specification  */
  uint8_t   CT3_MODE_SPEC;         /* Ctr/Timer 3 - Mode Specification  */
  uint8_t   CURRENT_VECTOR;        /* Current Vector */
/* PORT A SPECIFICATION REGISTERS (0x20 -0x27) */
  uint8_t   PORTA_MODE;            /* Port A - Mode Specification  */
  uint8_t   PORTA_HANDSHAKE;       /* Port A - Handshake Specification  */
  uint8_t   PORTA_DATA_POLARITY;   /* Port A - Data Path Polarity */
  uint8_t   PORTA_DIRECTION;       /* Port A - Data Direction */
  uint8_t   PORTA_SPECIAL_IO_CTL;  /* Port A - Special IO Control */
  uint8_t   PORTA_PATT_POLARITY;   /* Port A - Pattern Polarity */
  uint8_t   PORTA_PATT_TRANS;      /* Port A - Pattern Transition */
  uint8_t   PORTA_PATT_MASK;       /* Port A - Pattern Mask */
/* PORT B SPECIFICATION REGISTERS (0x28-0x2f) */
  uint8_t   PORTB_MODE;            /* Port B - Mode Specification  */
  uint8_t   PORTB_HANDSHAKE;       /* Port B - Handshake Specification  */
  uint8_t   PORTB_DATA_POLARITY;   /* Port B - Data Path Polarity */
  uint8_t   PORTB_DIRECTION;       /* Port B - Data Direction */
  uint8_t   PORTB_SPECIAL_IO_CTL;  /* Port B - Special IO Control */
  uint8_t   PORTB_PATT_POLARITY;   /* Port B - Pattern Polarity */
  uint8_t   PORTB_PATT_TRANS;      /* Port B - Pattern Transition */
  uint8_t   PORTB_PATT_MASK;       /* Port B - Pattern Mask */
};

#ifdef __cplusplus
}
#endif

#endif
