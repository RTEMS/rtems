/*  z8036.h
 *
 *  This include file defines information related to a Zilog Z8036
 *  Counter/Timer/IO Chip.  It is a memory mapped part.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This file shares as much as possible with the include
 *        file for the Z8536 via z8x36.h.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __Z8036_h
#define __Z8036_h

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
  rtems_unsigned8 MASTER_INTR;           /* Master Interrupt Ctl Reg */
  rtems_unsigned8 MASTER_CFG;            /* Master Configuration Ctl Reg */
  rtems_unsigned8 PORTA_VECTOR;          /* Port A - Interrupt Vector */
  rtems_unsigned8 PORTB_VECTOR;          /* Port B - Interrupt Vector */
  rtems_unsigned8 CNT_TMR_VECTOR;        /* Counter/Timer Interrupt Vector */
  rtems_unsigned8 PORTC_DATA_POLARITY;   /* Port C - Data Path Polarity */
  rtems_unsigned8 PORTC_DIRECTION;       /* Port C - Data Direction */
  rtems_unsigned8 PORTC_SPECIAL_IO_CTL;  /* Port C - Special IO Control */
/* MOST OFTEN ACCESSED REGISTERS (0x08 - 0x0f) */
  rtems_unsigned8 PORTA_CMD_STATUS;      /* Port A - Command Status Reg */
  rtems_unsigned8 PORTB_CMD_STATUS;      /* Port B - Command Status Reg */
  rtems_unsigned8 CT1_CMD_STATUS;        /* Ctr/Timer 1 - Command Status Reg */
  rtems_unsigned8 CT2_CMD_STATUS;        /* Ctr/Timer 2 - Command Status Reg */
  rtems_unsigned8 CT3_CMD_STATUS;        /* Ctr/Timer 3 - Command Status Reg */
  rtems_unsigned8 PORTA_DATA;            /* Port A - Data */
  rtems_unsigned8 PORTB_DATA;            /* Port B - Data */
  rtems_unsigned8 PORTC_DATA;            /* Port C - Data */
/* COUNTER/TIMER RELATED REGISTERS (0x10-0x1f) */
  rtems_unsigned8 CT1_CUR_CNT_MSB;       /* Ctr/Timer 1 - Current Count (MSB) */
  rtems_unsigned8 CT1_CUR_CNT_LSB;       /* Ctr/Timer 1 - Current Count (LSB) */
  rtems_unsigned8 CT2_CUR_CNT_MSB;       /* Ctr/Timer 2 - Current Count (MSB) */
  rtems_unsigned8 CT2_CUR_CNT_LSB;       /* Ctr/Timer 2 - Current Count (LSB) */
  rtems_unsigned8 CT3_CUR_CNT_MSB;       /* Ctr/Timer 3 - Current Count (MSB) */
  rtems_unsigned8 CT3_CUR_CNT_LSB;       /* Ctr/Timer 3 - Current Count (LSB) */
  rtems_unsigned8 CT1_TIME_CONST_MSB;    /* Ctr/Timer 1 - Time Constant (MSB) */
  rtems_unsigned8 CT1_TIME_CONST_LSB;    /* Ctr/Timer 1 - Time Constant (LSB) */
  rtems_unsigned8 CT2_TIME_CONST_MSB;    /* Ctr/Timer 2 - Time Constant (MSB) */
  rtems_unsigned8 CT2_TIME_CONST_LSB;    /* Ctr/Timer 2 - Time Constant (LSB) */
  rtems_unsigned8 CT3_TIME_CONST_MSB;    /* Ctr/Timer 3 - Time Constant (MSB) */
  rtems_unsigned8 CT3_TIME_CONST_LSB;    /* Ctr/Timer 3 - Time Constant (LSB) */
  rtems_unsigned8 CT1_MODE_SPEC;         /* Ctr/Timer 1 - Mode Specification  */
  rtems_unsigned8 CT2_MODE_SPEC;         /* Ctr/Timer 2 - Mode Specification  */
  rtems_unsigned8 CT3_MODE_SPEC;         /* Ctr/Timer 3 - Mode Specification  */
  rtems_unsigned8 CURRENT_VECTOR;        /* Current Vector */
/* PORT A SPECIFICATION REGISTERS (0x20 -0x27) */
  rtems_unsigned8 PORTA_MODE;            /* Port A - Mode Specification  */
  rtems_unsigned8 PORTA_HANDSHAKE;       /* Port A - Handshake Specification  */
  rtems_unsigned8 PORTA_DATA_POLARITY;   /* Port A - Data Path Polarity */
  rtems_unsigned8 PORTA_DIRECTION;       /* Port A - Data Direction */
  rtems_unsigned8 PORTA_SPECIAL_IO_CTL;  /* Port A - Special IO Control */
  rtems_unsigned8 PORTA_PATT_POLARITY;   /* Port A - Pattern Polarity */
  rtems_unsigned8 PORTA_PATT_TRANS;      /* Port A - Pattern Transition */
  rtems_unsigned8 PORTA_PATT_MASK;       /* Port A - Pattern Mask */
/* PORT B SPECIFICATION REGISTERS (0x28-0x2f) */
  rtems_unsigned8 PORTB_MODE;            /* Port B - Mode Specification  */
  rtems_unsigned8 PORTB_HANDSHAKE;       /* Port B - Handshake Specification  */
  rtems_unsigned8 PORTB_DATA_POLARITY;   /* Port B - Data Path Polarity */
  rtems_unsigned8 PORTB_DIRECTION;       /* Port B - Data Direction */
  rtems_unsigned8 PORTB_SPECIAL_IO_CTL;  /* Port B - Special IO Control */
  rtems_unsigned8 PORTB_PATT_POLARITY;   /* Port B - Pattern Polarity */
  rtems_unsigned8 PORTB_PATT_TRANS;      /* Port B - Pattern Transition */
  rtems_unsigned8 PORTB_PATT_MASK;       /* Port B - Pattern Mask */
};

#ifdef __cplusplus
}
#endif

#endif

