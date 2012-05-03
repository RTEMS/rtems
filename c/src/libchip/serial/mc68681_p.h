/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _MC68681_P_H_
#define _MC68681_P_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define MC68681_STATIC to nothing while debugging so the entry points
 *  will show up in the symbol table.
 */

#define MC68681_STATIC

/* #define MC68681_STATIC static */

/*
 * mc68681 register offsets Read/Write Addresses
 */

#define MC68681_MODE_REG_1A  0  /* MR1A-MR Prior to Read */
#define MC68681_MODE_REG_2A  0  /* MR2A-MR After Read    */

#define MC68681_COUNT_MODE_CURRENT_MSB       6  /* CTU   */
#define MC68681_COUNTER_TIMER_UPPER_REG      6  /* CTU   */
#define MC68681_COUNT_MODE_CURRENT_LSB       7  /* CTL   */
#define MC68681_COUNTER_TIMER_LOWER_REG      7  /* CTL   */
#define MC68681_INTERRUPT_VECTOR_REG        12  /* IVR   */

#define MC68681_MODE_REG_1B  8  /* MR1B-MR Prior to Read */
#define MC68681_MODE_REG_2B  8  /* MR2BA-MR After Read   */

/*
 * mc68681 register offsets Read Only  Addresses
 */

#define MC68681_STATUS_REG_A                 1  /* SRA   */
#define MC68681_MASK_ISR_REG                 2  /* MISR  */
#define MC68681_RECEIVE_BUFFER_A             3  /* RHRA  */
#define MC68681_INPUT_PORT_CHANGE_REG        4  /* IPCR  */
#define MC68681_INTERRUPT_STATUS_REG         5  /* ISR   */
#define MC68681_STATUS_REG_B                 9  /* SRB   */
#define MC68681_RECEIVE_BUFFER_B            11  /* RHRB  */
#define MC68681_INPUT_PORT                  13  /* IP    */
#define MC68681_START_COUNT_CMD             14  /* SCC   */
#define MC68681_STOP_COUNT_CMD              15  /* STC   */

/*
 * mc68681 register offsets Write Only  Addresses
 */

#define MC68681_CLOCK_SELECT_REG_A           1  /* CSRA  */
#define MC68681_COMMAND_REG_A                2  /* CRA   */
#define MC68681_TRANSMIT_BUFFER_A            3  /* THRA  */
#define MC68681_AUX_CTRL_REG                 4  /* ACR   */
#define MC68681_INTERRUPT_MASK_REG           5  /* IMR   */
#define MC68681_CLOCK_SELECT_REG_B           9  /* CSRB  */
#define MC68681_COMMAND_REG_B               10  /* CRB   */
#define MC68681_TRANSMIT_BUFFER_B           11  /* THRB  */
#define MC68681_OUTPUT_PORT_CONFIG_REG      13  /* OPCR  */
#define MC68681_OUTPUT_PORT_SET_REG         14  /* SOPBC */
#define MC68681_OUTPUT_PORT_RESET_BITS      15  /* COPBC */

/*
 *  DUART Command Register Definitions:
 *
 *  MC68681_COMMAND_REG_A,MC68681_COMMAND_REG_B
 */

#define MC68681_MODE_REG_ENABLE_RX                          0x01
#define MC68681_MODE_REG_DISABLE_RX                         0x02
#define MC68681_MODE_REG_ENABLE_TX                          0x04
#define MC68681_MODE_REG_DISABLE_TX                         0x08
#define MC68681_MODE_REG_RESET_MR_PTR                       0x10
#define MC68681_MODE_REG_RESET_RX                           0x20
#define MC68681_MODE_REG_RESET_TX                           0x30
#define MC68681_MODE_REG_RESET_ERROR                        0x40
#define MC68681_MODE_REG_RESET_BREAK                        0x50
#define MC68681_MODE_REG_START_BREAK                        0x60
#define MC68681_MODE_REG_STOP_BREAK                         0x70
#define MC68681_MODE_REG_SET_RX_BRG                         0x80
#define MC68681_MODE_REG_CLEAR_RX_BRG                       0x90
#define MC68681_MODE_REG_SET_TX_BRG                         0xa0
#define MC68681_MODE_REG_CLEAR_TX_BRG                       0xb0
#define MC68681_MODE_REG_SET_STANDBY                        0xc0
#define MC68681_MODE_REG_SET_ACTIVE                         0xd0

/*
 * Mode Register Definitions
 *
 *  MC68681_MODE_REG_1A
 *  MC68681_MODE_REG_1B
 */

#define MC68681_5BIT_CHARS                                  0x00
#define MC68681_6BIT_CHARS                                  0x01
#define MC68681_7BIT_CHARS                                  0x02
#define MC68681_8BIT_CHARS                                  0x03

#define MC68681_ODD_PARITY                                  0x00
#define MC68681_EVEN_PARITY                                 0x04

#define MC68681_WITH_PARITY                                 0x00
#define MC68681_FORCE_PARITY                                0x08
#define MC68681_NO_PARITY                                   0x10
#define MC68681_MULTI_DROP                                  0x18

#define MC68681_ERR_MODE_CHAR                               0x00
#define MC68681_ERR_MODE_BLOCK                              0x20

#define MC68681_RX_INTR_RX_READY                            0x00
#define MC68681_RX_INTR_FFULL                               0x40

#define MC68681_NO_RX_RTS_CTL                               0x00
#define MC68681_RX_RTS_CTRL                                 0x80

/*
 * Mode Register Definitions
 *
 *  MC68681_MODE_REG_2A
 *  MC68681_MODE_REG_2B
 */

#define MC68681_STOP_BIT_LENGTH__563                        0x00
#define MC68681_STOP_BIT_LENGTH__625                        0x01
#define MC68681_STOP_BIT_LENGTH__688                        0x02
#define MC68681_STOP_BIT_LENGTH__75                         0x03
#define MC68681_STOP_BIT_LENGTH__813                        0x04
#define MC68681_STOP_BIT_LENGTH__875                        0x05
#define MC68681_STOP_BIT_LENGTH__938                        0x06
#define MC68681_STOP_BIT_LENGTH_1                           0x07
#define MC68681_STOP_BIT_LENGTH_1_563                       0x08
#define MC68681_STOP_BIT_LENGTH_1_625                       0x09
#define MC68681_STOP_BIT_LENGTH_1_688                       0x0a
#define MC68681_STOP_BIT_LENGTH_1_75                        0x0b
#define MC68681_STOP_BIT_LENGTH_1_813                       0x0c
#define MC68681_STOP_BIT_LENGTH_1_875                       0x0d
#define MC68681_STOP_BIT_LENGTH_1_938                       0x0e
#define MC68681_STOP_BIT_LENGTH_2                           0x0f

#define MC68681_CTS_ENABLE_TX                               0x10
#define MC68681_TX_RTS_CTRL                                 0x20

#define MC68681_CHANNEL_MODE_NORMAL                         0x00
#define MC68681_CHANNEL_MODE_ECHO                           0x40
#define MC68681_CHANNEL_MODE_LOCAL_LOOP                     0x80
#define MC68681_CHANNEL_MODE_REMOTE_LOOP                    0xc0

/*
 * Status Register Definitions
 *
 *    MC68681_STATUS_REG_A,  MC68681_STATUS_REG_B
 */

#define MC68681_RX_READY                                    0x01
#define MC68681_FFULL                                       0x02
#define MC68681_TX_READY                                    0x04
#define MC68681_TX_EMPTY                                    0x08
#define MC68681_OVERRUN_ERROR                               0x10
#define MC68681_PARITY_ERROR                                0x20
#define MC68681_FRAMING_ERROR                               0x40
#define MC68681_RECEIVED_BREAK                              0x80

#define MC68681_RX_ERRORS \
  (MC68681_OVERRUN_ERROR|MC68681_PARITY_ERROR| \
   MC68681_FRAMING_ERROR|MC68681_RECEIVED_BREAK)

/*
 * Interupt Status Register Definitions.
 *
 * MC68681_INTERRUPT_STATUS_REG
 */

/*
 *  Interupt Mask Register Definitions
 *
 *  MC68681_INTERRUPT_MASK_REG
 */

/* These are passed to mc68681_build_imr */
#define MC68681_IR_TX_READY                                 0x01
#define MC68681_IR_RX_READY                                 0x02
#define MC68681_IR_BREAK                                    0x04
#define MC68681_IMR_ENABLE_ALL                              0x07
#define MC68681_IMR_DISABLE_ALL                             0x00
#define MC68681_IMR_ENABLE_ALL_EXCEPT_TX                    0x06

#define MC68681_IR_TX_READY_A                               0x01
#define MC68681_IR_RX_READY_A                               0x02
#define MC68681_IR_BREAK_A                                  0x04
#define MC68681_IR_COUNTER_READY                            0x08
#define MC68681_IR_TX_READY_B                               0x10
#define MC68681_IR_RX_READY_B                               0x20
#define MC68681_IR_BREAK_B                                  0x40
#define MC68681_IR_INPUT_PORT_CHANGE                        0x80

/*
 * Status Register Definitions.
 *
 * MC68681_STATUS_REG_A,MC68681_STATUS_REG_B
 */

#define MC68681_STATUS_RXRDY                                0x01
#define MC68681_STATUS_FFULL                                0x02
#define MC68681_STATUS_TXRDY                                0x04
#define MC68681_STATUS_TXEMT                                0x08
#define MC68681_STATUS_OVERRUN_ERROR                        0x10
#define MC68681_STATUS_PARITY_ERROR                         0x20
#define MC68681_STATUS_FRAMING_ERROR                        0x40
#define MC68681_STATUS_RECEIVED_BREAK                       0x80

/*
 * Definitions for the Interrupt Vector Register:
 *
 * MC68681_INTERRUPT_VECTOR_REG
 */

#define  MC68681_INTERRUPT_VECTOR_INIT                      0x0f

/*
 * Definitions for the Auxiliary Control Register
 *
 * MC68681_AUX_CTRL_REG
 */

#define MC68681_AUX_BRG_SET1                                0x00
#define MC68681_AUX_BRG_SET2                                0x80

/*
 *  Per chip context control
 */

typedef struct _mc68681_context
{
  int            mate;
  unsigned char  imr;
} mc68681_context;

/*
 * Driver functions
 */
MC68681_STATIC bool mc68681_probe(int minor);

MC68681_STATIC int mc68681_set_attributes(
  int minor,
  const struct termios *t
);

MC68681_STATIC void mc68681_init(int minor);

MC68681_STATIC int mc68681_open(
  int major,
  int minor,
  void  * arg
);

MC68681_STATIC int mc68681_close(
  int major,
  int minor,
  void  * arg
);

MC68681_STATIC void mc68681_write_polled(
  int   minor,
  char  cChar
);

MC68681_STATIC void mc68681_initialize_interrupts(int minor);

MC68681_STATIC ssize_t mc68681_write_support_int(
  int   minor,
  const char *buf,
  size_t len
);

MC68681_STATIC ssize_t mc68681_write_support_polled(
  int   minor,
  const char *buf,
  size_t   len
  );

MC68681_STATIC int mc68681_inbyte_nonblocking_polled(
  int minor
);

MC68681_STATIC unsigned int mc68681_build_imr(
  int  minor,
  int  enable_flag
);

MC68681_STATIC void mc68681_process(
  int  minor
);

MC68681_STATIC void mc68681_enable_interrupts(
  int minor,
  int imr_mask
);

#ifdef __cplusplus
}
#endif

#endif /* _MC68681_P_H_ */
