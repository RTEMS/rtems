/*  consolebsp.h
 *
 *  This include file contains all console driver definations
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __CONSOLEBSP_H
#define __CONSOLEBSP_H

#include <rtems.h>
#include <rtems/ringbuf.h>
#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 * Note:  The Ports are numbered 0..NUM_Z85C30_CHIPS with port 0 and 1
 *        being on the first chip, and ports 2 and 3 being on the
 *        second chip...
 */

/*
 * Z85c30 configuration informaiton.
 */

#if (HAS_PMC_PSC8)
#define NUM_Z85C30_CHIPS_ON_MEZZANINE  4
#else
#define NUM_Z85C30_CHIPS_ON_MEZZANINE  0
#endif

#define NUM_Z85C30_CHIPS       (2 + NUM_Z85C30_CHIPS_ON_MEZZANINE)
#define NUM_Z85C30_PORTS       (NUM_Z85C30_CHIPS * 2)

typedef enum {
  CONSOLE_x1_CLOCK,
  CONSOLE_x16_CLOCK,
  CONSOLE_x32_CLOCK,
  CONSOLE_x64_CLOCK,
} CONSOLE_Clock_speed;

typedef enum {
  CONSOLE_STOP_BITS_1,
  CONSOLE_STOP_BITS_1_AND_A_HALF,
  CONSOLE_STOP_BITS_2,
} CONSOLE_Stop_bits;

typedef enum {
  CONSOLE_PARITY_NONE,
  CONSOLE_PARITY_ODD,
  CONSOLE_PARITY_EVEN,
} CONSOLE_Parity;

typedef enum {
  CONSOLE_CHARACTER_BITS_8,
  CONSOLE_CHARACTER_BITS_7,
  CONSOLE_CHARACTER_BITS_6,
  CONSOLE_CHARACTER_BITS_5,
} CONSOLE_Character_bits;

typedef struct {
  uint32_t                baud_rate;         /* baud rate value */
  CONSOLE_Stop_bits        stop_bits;
  CONSOLE_Parity           parity;
  CONSOLE_Character_bits   read_char_bits;
  CONSOLE_Character_bits   write_char_bits;

#if CONSOLE_USE_INTERRUPTS
  volatile Ring_buffer_t  TX_Buffer;         /* Transmit Buffer    */
  volatile bool           Is_TX_active;      /* Transmitting       */
  void          *console_termios_data;
#endif

} Console_Protocol;

/*
 * Structure used for chip level information.
 */
typedef struct {
  uint32_t               vector;
  uint32_t               clock_frequency;
  uint16_t               clock_x;
  CONSOLE_Clock_speed     clock_speed;
} Chip_85C30_info;

/*
 * Structure used for port level informaiton.
 */
typedef struct {

  volatile unsigned char    *ctrl;           /* Port Ctrl byte     */
  volatile unsigned char    *data;           /* Port data byte     */

  unsigned char              port;          /* Port-id / minor #  */

  Console_Protocol          *Protocol;
  Chip_85C30_info           *Chip;           /* Chip specific info */

} Port_85C30_info;

/*
 * Console port chip configuration tables.
 */
extern Chip_85C30_info Chips_85C30 [ NUM_Z85C30_CHIPS ];
extern const Port_85C30_info Ports_85C30 [ NUM_Z85C30_PORTS ];

/*
 *  85c30.c prototypes.
 */
void initialize_85c30_port(
  const Port_85C30_info *Port
);

void outbyte_polled_85c30(
  volatile unsigned char  *csr,                     /* IN  */
  char ch
);

int inbyte_nonblocking_85c30(
  const Port_85C30_info      *Port
);

void Reset_85c30_chip(
  volatile unsigned char *ctrl_0,
  volatile unsigned char *ctrl_1
);

#if CONSOLE_USE_INTERRUPTS
rtems_isr ISR_85c30_Async(
   const Port_85C30_info   *Port
);
#endif
#ifdef __cplusplus
}
#endif

#endif
