 /*
 *  This file contains the table for the z85c30 port
 *  used by the console driver.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "consolebsp.h"
#include <bsp.h>
#include <bsp/irq.h>

#define CONSOLE_DEFAULT_BAUD_RATE            9600
#define CONSOLE_DEFAULT_BAUD_CONSTANT        Score603e_Z8530_Chip0_Baud(9600)

#define CONSOLE_DEFAULT_STOP_BITS            CONSOLE_STOP_BITS_1
#define CONSOLE_DEFAULT_PARITY               CONSOLE_PARITY_NONE
#define CONSOLE_DEFAULT_READ_CHARACTER_BITS  CONSOLE_CHARACTER_BITS_8
#define CONSOLE_DEFAULT_WRITE_CHARACTER_BITS CONSOLE_CHARACTER_BITS_8
#define CONSOLE_DEFAULT_CONSOLE_CLOCK        CONSOLE_x16_CLOCK

#define DEFAULT_PROTOCOL  { CONSOLE_DEFAULT_BAUD_RATE,              \
                            CONSOLE_DEFAULT_STOP_BITS,              \
                            CONSOLE_DEFAULT_PARITY,                 \
                            CONSOLE_DEFAULT_READ_CHARACTER_BITS,    \
                            CONSOLE_DEFAULT_WRITE_CHARACTER_BITS }

/*
 * Tables of information necessary to use the console 85c30 routines.
 */
Console_Protocol Protocols_85c30 [ NUM_Z85C30_PORTS ] =
{
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,

#if (HAS_PMC_PSC8)
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
  DEFAULT_PROTOCOL,
#endif
};

/*
 * Table of chip unique information for each chip.
 * See consolebsp.h for the Chip_85C30_info structure defination.
 */
Chip_85C30_info Chips_85C30 [ NUM_Z85C30_CHIPS ] =
{
  {
    SCORE603E_85C30_0_IRQ,
    SCORE603E_85C30_0_CLOCK,
    SCORE603E_85C30_0_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },
  {
    SCORE603E_85C30_1_IRQ,
    SCORE603E_85C30_1_CLOCK,
    SCORE603E_85C30_1_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },

#if (HAS_PMC_PSC8)
  {
    SCORE603E_85C30_2_IRQ,
    SCORE603E_85C30_2_CLOCK,
    SCORE603E_85C30_2_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },
  {
    SCORE603E_85C30_3_IRQ,
    SCORE603E_85C30_3_CLOCK,
    SCORE603E_85C30_3_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },
  {
    SCORE603E_85C30_4_IRQ,
    SCORE603E_85C30_4_CLOCK,
    SCORE603E_85C30_4_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },
  {
    SCORE603E_85C30_5_IRQ,
    SCORE603E_85C30_5_CLOCK,
    SCORE603E_85C30_5_CLOCK_X,
    CONSOLE_DEFAULT_CONSOLE_CLOCK
  },
#endif

};

/*
 * Table of port unique information for each port.
 * See consolebsp.h for the Port_85C30_info structure defination.
 */
const Port_85C30_info Ports_85C30 [ NUM_Z85C30_PORTS ] = {
  {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_0,
    (volatile unsigned char *) SCORE603E_85C30_DATA_0,
    0x00,
    &Protocols_85c30[0],
    &Chips_85C30[0],
  },
  {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_1,
    (volatile unsigned char *) SCORE603E_85C30_DATA_1,
    0x01,
    &Protocols_85c30[1],
    &Chips_85C30[0],
  },
  {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_2,
    (volatile unsigned char *) SCORE603E_85C30_DATA_2,
    0x02,
    &Protocols_85c30[2],
    &Chips_85C30[1],
  },
  {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_3,
    (volatile unsigned char *) SCORE603E_85C30_DATA_3,
    0x03,
    &Protocols_85c30[3],
    &Chips_85C30[1],
 },

#if (HAS_PMC_PSC8)
  {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_4,
    (volatile unsigned char *) SCORE603E_85C30_DATA_4,
    0x04,
    &Protocols_85c30[4],
    &Chips_85C30[2],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_5,
    (volatile unsigned char *) SCORE603E_85C30_DATA_5,
    0x05,
    &Protocols_85c30[5],
    &Chips_85C30[2],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_6,
    (volatile unsigned char *) SCORE603E_85C30_DATA_6,
    0x06,
    &Protocols_85c30[6],
    &Chips_85C30[3],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_7,
    (volatile unsigned char *) SCORE603E_85C30_DATA_7,
    0x07,
    &Protocols_85c30[7],
    &Chips_85C30[3],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_8,
    (volatile unsigned char *) SCORE603E_85C30_DATA_8,
    0x08,
    &Protocols_85c30[8],
    &Chips_85C30[4],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_9,
    (volatile unsigned char *) SCORE603E_85C30_DATA_9,
    0x09,
    &Protocols_85c30[9],
    &Chips_85C30[4],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_10,
    (volatile unsigned char *) SCORE603E_85C30_DATA_10,
    0x0a,
    &Protocols_85c30[10],
    &Chips_85C30[5],
 },
 {
    (volatile unsigned char *) SCORE603E_85C30_CTRL_11,
    (volatile unsigned char *) SCORE603E_85C30_DATA_11,
    0x0b,
    &Protocols_85c30[11],
    &Chips_85C30[5],
 },
#endif
};
