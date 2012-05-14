/*
 *  This include file contains all private driver definitions for the
 *  Zilog z85c30.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __Z85C30_P_H
#define __Z85C30_P_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define Z85C30_STATIC to nothing while debugging so the entry points
 *  will show up in the symbol table.
 */

#define Z85C30_STATIC

/* #define Z85C30_STATIC static */

/* bit values for write register 0 */
/* command register */

#define SCC_WR0_SEL_WR0   0x00
#define SCC_WR0_SEL_WR1   0x01
#define SCC_WR0_SEL_WR2   0x02
#define SCC_WR0_SEL_WR3   0x03
#define SCC_WR0_SEL_WR4   0x04
#define SCC_WR0_SEL_WR5   0x05
#define SCC_WR0_SEL_WR6   0x06
#define SCC_WR0_SEL_WR7   0x07
#define SCC_WR0_SEL_WR8   0x08
#define SCC_WR0_SEL_WR9   0x09
#define SCC_WR0_SEL_WR10  0x0a
#define SCC_WR0_SEL_WR11  0x0b
#define SCC_WR0_SEL_WR12  0x0c
#define SCC_WR0_SEL_WR13  0x0d
#define SCC_WR0_SEL_WR14  0x0e
#define SCC_WR0_SEL_WR15  0x0f
#define SCC_WR0_SEL_RD0   0x00
#define SCC_WR0_SEL_RD1   0x01
#define SCC_WR0_SEL_RD2   0x02
#define SCC_WR0_SEL_RD3   0x03
#define SCC_WR0_SEL_RD4   0x04
#define SCC_WR0_SEL_RD5   0x05
#define SCC_WR0_SEL_RD6   0x06
#define SCC_WR0_SEL_RD7   0x07
#define SCC_WR0_SEL_RD8   0x08
#define SCC_WR0_SEL_RD9   0x09
#define SCC_WR0_SEL_RD10  0x0a
#define SCC_WR0_SEL_RD11  0x0b
#define SCC_WR0_SEL_RD12  0x0c
#define SCC_WR0_SEL_RD13  0x0d
#define SCC_WR0_SEL_RD14  0x0e
#define SCC_WR0_SEL_RD15  0x0f
#define SCC_WR0_NULL_CODE 0x00
#define SCC_WR0_RST_INT   0x10
#define SCC_WR0_SEND_ABORT  0x18
#define SCC_WR0_EN_INT_RX 0x20
#define SCC_WR0_RST_TX_INT  0x28
#define SCC_WR0_ERR_RST   0x30
#define SCC_WR0_RST_HI_IUS  0x38
#define SCC_WR0_RST_RX_CRC  0x40
#define SCC_WR0_RST_TX_CRC  0x80
#define SCC_WR0_RST_TX_UND  0xc0

/* write register 2 */
/* interrupt vector */

/* bit values for write register 1 */
/* tx/rx interrupt and data transfer mode definition */

#define SCC_WR1_EXT_INT_EN  0x01
#define SCC_WR1_TX_INT_EN   0x02
#define SCC_WR1_PARITY      0x04
#define SCC_WR1_RX_INT_DIS  0x00
#define SCC_WR1_RX_INT_FIR  0x08
#define SCC_WR1_INT_ALL_RX  0x10
#define SCC_WR1_RX_INT_SPE  0x18
#define SCC_WR1_RDMA_RECTR  0x20
#define SCC_WR1_RDMA_FUNC   0x40
#define SCC_WR1_RDMA_EN     0x80

#define SCC_ENABLE_ALL_INTR \
    (SCC_WR1_EXT_INT_EN | SCC_WR1_TX_INT_EN | SCC_WR1_INT_ALL_RX)

#define SCC_DISABLE_ALL_INTR 0x00

#define SCC_ENABLE_ALL_INTR_EXCEPT_TX \
    (SCC_WR1_EXT_INT_EN | SCC_WR1_INT_ALL_RX)

/* bit values for write register 3 */
/* receive parameters and control */

#define SCC_WR3_RX_EN   0x01
#define SCC_WR3_SYNC_CHAR 0x02
#define SCC_WR3_ADR_SEARCH  0x04
#define SCC_WR3_RX_CRC_EN 0x08
#define SCC_WR3_ENTER_HUNT  0x10
#define SCC_WR3_AUTO_EN   0x20
#define SCC_WR3_RX_5_BITS 0x00
#define SCC_WR3_RX_7_BITS 0x40
#define SCC_WR3_RX_6_BITS 0x80
#define SCC_WR3_RX_8_BITS 0xc0

/* bit values for write register 4 */
/* tx/rx misc parameters and modes */

#define SCC_WR4_PAR_EN    0x01
#define SCC_WR4_PAR_EVEN  0x02
#define SCC_WR4_SYNC_EN   0x00
#define SCC_WR4_1_STOP    0x04
#define SCC_WR4_2_STOP    0x0c
#define SCC_WR4_8_SYNC    0x00
#define SCC_WR4_16_SYNC   0x10
#define SCC_WR4_SDLC    0x20
#define SCC_WR4_EXT_SYNC  0x30
#define SCC_WR4_1_CLOCK   0x00
#define SCC_WR4_16_CLOCK  0x40
#define SCC_WR4_32_CLOCK  0x80
#define SCC_WR4_64_CLOCK  0xc0

/* bit values for write register 5 */
/* transmit parameter and controls */

#define SCC_WR5_TX_CRC_EN 0x01
#define SCC_WR5_RTS   0x02
#define SCC_WR5_SDLC    0x04
#define SCC_WR5_TX_EN   0x08
#define SCC_WR5_SEND_BRK  0x10

#define SCC_WR5_TX_5_BITS 0x00
#define SCC_WR5_TX_7_BITS 0x20
#define SCC_WR5_TX_6_BITS 0x40
#define SCC_WR5_TX_8_BITS 0x60
#define SCC_WR5_DTR   0x80

/* write register 6 */
/* sync chars or sdlc address field */

/* write register 7 */
/* sync char or sdlc flag */

/* write register 8 */
/* transmit buffer */

/* bit values for write register 9 */
/* master interrupt control */

#define SCC_WR9_VIS   0x01
#define SCC_WR9_NV    0x02
#define SCC_WR9_DLC   0x04
#define SCC_WR9_MIE   0x08
#define SCC_WR9_STATUS_HI 0x10
#define SCC_WR9_NO_RST    0x00
#define SCC_WR9_CH_B_RST  0x40
#define SCC_WR9_CH_A_RST  0x80
#define SCC_WR9_HDWR_RST  0xc0

/* bit values for write register 10 */
/* misc tx/rx control bits */

#define SCC_WR10_6_BIT_SYNC 0x01
#define SCC_WR10_LOOP_MODE  0x02
#define SCC_WR10_ABORT_UND  0x04
#define SCC_WR10_MARK_IDLE  0x08
#define SCC_WR10_ACT_POLL 0x10
#define SCC_WR10_NRZ    0x00
#define SCC_WR10_NRZI   0x20
#define SCC_WR10_FM1    0x40
#define SCC_WR10_FM0    0x60
#define SCC_WR10_CRC_PRESET 0x80

/* bit values for write register 11 */
/* clock mode control */

#define SCC_WR11_OUT_XTAL 0x00
#define SCC_WR11_OUT_TX_CLK 0x01
#define SCC_WR11_OUT_BR_GEN 0x02
#define SCC_WR11_OUT_DPLL 0x03
#define SCC_WR11_TRXC_OI  0x04
#define SCC_WR11_TX_RTXC  0x00
#define SCC_WR11_TX_TRXC  0x08
#define SCC_WR11_TX_BR_GEN  0x10
#define SCC_WR11_TX_DPLL  0x18
#define SCC_WR11_RX_RTXC  0x00
#define SCC_WR11_RX_TRXC  0x20
#define SCC_WR11_RX_BR_GEN  0x40
#define SCC_WR11_RX_DPLL  0x60
#define SCC_WR11_RTXC_XTAL  0x80

/* write register 12 */
/* lower byte of baud rate generator time constant */

/* write register 13 */
/* upper byte of baud rate generator time constant */

/* bit values for write register 14 */
/* misc control bits */

#define SCC_WR14_BR_EN    0x01
#define SCC_WR14_BR_SRC   0x02
#define SCC_WR14_DTR_FUNC 0x04
#define SCC_WR14_AUTO_ECHO  0x08
#define SCC_WR14_LCL_LOOP 0x10
#define SCC_WR14_NULL   0x00
#define SCC_WR14_SEARCH   0x20
#define SCC_WR14_RST_CLK  0x40
#define SCC_WR14_DIS_DPLL 0x60
#define SCC_WR14_SRC_BR   0x80
#define SCC_WR14_SRC_RTXC 0xa0
#define SCC_WR14_FM_MODE  0xc0
#define SCC_WR14_NRZI   0xe0

/* bit values for write register 15 */
/* external/status interrupt control */

#define SCC_WR15_ZERO_CNT 0x02
#define SCC_WR15_CD_IE    0x08
#define SCC_WR15_SYNC_IE  0x10
#define SCC_WR15_CTS_IE   0x20
#define SCC_WR15_TX_UND_IE  0x40
#define SCC_WR15_BREAK_IE 0x80

/* bit values for read register 0 */
/* tx/rx buffer status and external status  */

#define SCC_RR0_RX_AVAIL  0x01
#define SCC_RR0_ZERO_CNT  0x02
#define SCC_RR0_TX_EMPTY  0x04
#define SCC_RR0_CD    0x08
#define SCC_RR0_SYNC    0x10
#define SCC_RR0_CTS   0x20
#define SCC_RR0_TX_UND    0x40
#define SCC_RR0_BREAK   0x80

/* bit values for read register 1 */

#define SCC_RR1_ALL_SENT  0x01
#define SCC_RR1_RES_CD_2  0x02
#define SCC_RR1_RES_CD_1  0x01
#define SCC_RR1_RES_CD_0  0x08
#define SCC_RR1_PAR_ERR   0x10
#define SCC_RR1_RX_OV_ERR 0x20
#define SCC_RR1_CRC_ERR   0x40
#define SCC_RR1_END_FRAME 0x80

/* read register 2 */
/* interrupt vector */

/* bit values for read register 3 */
/* interrupt pending register */

#define SCC_RR3_B_EXT_IP  0x01
#define SCC_RR3_B_TX_IP   0x02
#define SCC_RR3_B_RX_IP   0x04
#define SCC_RR3_A_EXT_IP  0x08
#define SCC_RR3_A_TX_IP   0x10
#define SCC_RR3_A_RX_IP   0x20

/* read register 8 */
/* receive data register */

/* bit values for read register 10 */
/* misc status bits */

#define SCC_RR10_ON_LOOP  0x02
#define SCC_RR10_LOOP_SEND  0x10
#define SCC_RR10_2_CLK_MIS  0x40
#define SCC_RR10_1_CLK_MIS  0x80

/* read register 12 */
/* lower byte of time constant */

/* read register 13 */
/* upper byte of time constant */

/* bit values for read register 15 */
/* external/status ie bits */

#define SCC_RR15_ZERO_CNT 0x02
#define SCC_RR15_CD_IE    0x08
#define SCC_RR15_SYNC_IE  0x10
#define SCC_RR15_CTS_IE   0x20
#define SCC_RR15_TX_UND_IE  0x40
#define SCC_RR15_BREAK_IE 0x80

typedef struct _z85c30_context
{
  uint8_t   ucModemCtrl;
} z85c30_context;

/*
 * The following macro calculates the Baud constant. For the Z85C30 chip.
 *
 * Note: baud constant = ((clock frequency / Clock_X) / (2 * Baud Rate)) - 2
 *       eg ((10,000,000 / 16) / (2 * Baud Rate)) - 2
 */

#define Z85C30_Baud( _clock, _baud_rate  )   \
  ( ((_clock) /(  16 * 2 * _baud_rate))  - 2)

#define Z85C30_Status_Is_RX_character_available(_status) \
  ((_status) & SCC_RR0_RX_AVAIL)

#define Z85C30_Status_Is_TX_buffer_empty(_status) \
  ((_status) & SCC_RR0_TX_EMPTY)

#define Z85C30_Status_Is_CTS_asserted(_status) \
  ((_status) & SCC_RR0_CTS)

#define Z85C30_Status_Is_break_abort(_status) \
  ((_status) & SCC_RR0_BREAK)

/*
 * Private routines
 */

Z85C30_STATIC void z85c30_init(int minor);

Z85C30_STATIC int z85c30_set_attributes(
  int                   minor,
  const struct termios *t
);

Z85C30_STATIC int z85c30_open(
  int major,
  int minor,
  void  * arg
);

Z85C30_STATIC int z85c30_close(
  int major,
  int minor,
  void  * arg
);

Z85C30_STATIC void z85c30_write_polled(
  int   minor,
  char  cChar
);

Z85C30_STATIC int z85c30_assert_RTS(
  int minor
);

Z85C30_STATIC int z85c30_negate_RTS(
  int minor
);

Z85C30_STATIC int z85c30_assert_DTR(
  int minor
);

Z85C30_STATIC int z85c30_negate_DTR(
  int minor
);

Z85C30_STATIC void z85c30_initialize_interrupts(int minor);

Z85C30_STATIC ssize_t z85c30_write_support_int(
  int   minor,
  const char *buf,
  size_t len
);

Z85C30_STATIC ssize_t z85c30_write_support_polled(
  int   minor,
  const char *buf,
  size_t len
);

Z85C30_STATIC int z85c30_inbyte_nonblocking_polled(
  int minor
);

Z85C30_STATIC void z85c30_enable_interrupts(
  int minor,
  int interrupt_mask
);

#ifdef __cplusplus
}
#endif

#endif
