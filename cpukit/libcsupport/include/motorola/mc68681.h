/**
 * @file rtems/motorola/mc68681.h
 *
 *
 * mc68681-duart.h -- Low level support code for the Motorola mc68681
 *                   DUART.
 */

/*
 *
 *  Originally written by rob@cygnus.com (Rob Savoye) for the libgloss
 *    IDP support.
 */

#ifndef _RTEMS_MOTOROLA_MC68681_H
#define _RTEMS_MOTOROLA_MC68681_H

/*
 *  In the dark ages when this controller was designed, it was actually
 *  possible to access data on unaligned byte boundaries with no penalty.
 *  Now we find this chip in configurations in which the registers are
 *  at 16-bit, 32-bit, and 64-bit boundaries at the whim of the board
 *  designer.  If the registers are not at byte addresses, then
 *  set this multiplier before including this file to correct the offsets.
 */

#ifndef MC68681_OFFSET_MULTIPLIER
#define MC68681_OFFSET_MULTIPLIER 1
#endif

#define __MC68681_REG(_R) ((_R) * MC68681_OFFSET_MULTIPLIER)

/*
 * mc68681 register offsets Read/Write Addresses
 */
#define MC68681_MODE_REG_1A  __MC68681_REG(0)  /* MR1A-MR Prior to Read */
#define MC68681_MODE_REG_2A  __MC68681_REG(0)  /* MR2A-MR After Read    */

#define MC68681_COUNT_MODE_CURRENT_MSB       __MC68681_REG(6)  /* CTU   */
#define MC68681_COUNTER_TIMER_UPPER_REG      __MC68681_REG(6)  /* CTU   */
#define MC68681_COUNT_MODE_CURRENT_LSB       __MC68681_REG(7)  /* CTL   */
#define MC68681_COUNTER_TIMER_LOWER_REG      __MC68681_REG(7)  /* CTL   */
#define MC68681_INTERRUPT_VECTOR_REG         __MC68681_REG(12) /* IVR   */

#define MC68681_MODE_REG_1B  __MC68681_REG(8)  /* MR1B-MR Prior to Read */
#define MC68681_MODE_REG_2B  __MC68681_REG(8)  /* MR2BA-MR After Read   */

/*
 * mc68681 register offsets Read Only  Addresses
 */
#define MC68681_STATUS_REG_A                __MC68681_REG(1)   /* SRA   */
#define MC68681_MASK_ISR_REG                __MC68681_REG(2)   /* MISR  */
#define MC68681_RECEIVE_BUFFER_A            __MC68681_REG(3)   /* RHRA  */
#define MC68681_INPUT_PORT_CHANGE_REG       __MC68681_REG(4)   /* IPCR  */
#define MC68681_INTERRUPT_STATUS_REG        __MC68681_REG(5)   /* ISR   */
#define MC68681_STATUS_REG_B                __MC68681_REG(9)   /* SRB   */
#define MC68681_RECEIVE_BUFFER_B            __MC68681_REG(11)  /* RHRB  */
#define MC68681_INPUT_PORT                  __MC68681_REG(13)  /* IP    */
#define MC68681_START_COUNT_CMD             __MC68681_REG(14)  /* SCC   */
#define MC68681_STOP_COUNT_CMD              __MC68681_REG(15)  /* STC   */

/*
 * mc68681 register offsets Write Only  Addresses
 */
#define MC68681_CLOCK_SELECT_REG_A          __MC68681_REG(1)   /* CSRA  */
#define MC68681_COMMAND_REG_A               __MC68681_REG(2)   /* CRA   */
#define MC68681_TRANSMIT_BUFFER_A           __MC68681_REG(3)   /* THRA  */
#define MC68681_AUX_CTRL_REG                __MC68681_REG(4)   /* ACR   */
#define MC68681_INTERRUPT_MASK_REG          __MC68681_REG(5)   /* IMR   */
#define MC68681_CLOCK_SELECT_REG_B          __MC68681_REG(9)   /* CSRB  */
#define MC68681_COMMAND_REG_B               __MC68681_REG(10)  /* CRB   */
#define MC68681_TRANSMIT_BUFFER_B           __MC68681_REG(11)  /* THRB  */
#define MC68681_OUTPUT_PORT_CONFIG_REG      __MC68681_REG(13)  /* OPCR  */
#define MC68681_OUTPUT_PORT_SET_REG         __MC68681_REG(14)  /* SOPBC */
#define MC68681_OUTPUT_PORT_RESET_BITS      __MC68681_REG(15)  /* COPBC */


#ifndef MC6681_VOL
#define MC6681_VOL( ptr )   ((volatile unsigned char *)(ptr))
#endif

#define MC68681_WRITE( _base, _reg, _data ) \
   *((volatile unsigned char *)_base+_reg) = (_data)

#define MC68681_READ( _base, _reg ) \
   *(((volatile unsigned char *)_base+_reg))



#define  MC68681_CLEAR                                     0x00

#define MC68681_PORT_A                                     0
#define MC68681_PORT_B                                     1

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
 * The following Baud rates assume the X1 clock pin is driven with a
 * 3.6864 MHz signal.  If a different frequency is used the DUART channel
 * is running at the follwoing baud rate:
 *       ((Table Baud Rate)*frequency)/3.6864 MHz
 */

/*
 * Definitions for the Clock Select Register:
 *
 * MC68681_CLOCK_SELECT_REG_A,MC68681_CLOCK_SELECT_REG_A
 *
 * Note:  ACR[7] is the MSB of the Auxiliary Control register
 *        X is the extend bit.
 *        CRA - 0x08  Set Rx BRG Select Extend Bit   (X=1)
 *        CRA - 0x09  Clear Rx BRG Select Extend Bit (X=0)
 *        CRB - 0x0a  Set Tx BRG Select Extend Bit   (X=1)
 *        CRB - 0x0b  Clear Tx BRG Select Extend Bit (x=1)
 */
#define MC68681_BAUD_RATE_MASK_50           0x00   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_75           0x00   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_110          0x01
#define MC68681_BAUD_RATE_MASK_134_5        0x02
#define MC68681_BAUD_RATE_MASK_150          0x03   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_200          0x03   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_300          0x04   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_600          0x05   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_1050         0x07   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_1200         0x06   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_1800         0x0a   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_2400         0x08   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_3600         0x04   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_4800         0x09
#define MC68681_BAUD_RATE_MASK_7200         0x0a   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_9600         0xbb

#define MC68681_BAUD_RATE_MASK_14_4K        0x05   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_19_2K        0xcc   /* ACR[7]=1,X=0 */
                                                   /* ARC[7]=0,X=1 */
#define MC68681_BAUD_RATE_MASK_28_8K        0x06   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_38_4K        0xcc   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_57_6K        0x07   /* ACR[7]=0,X=0 */
                                                   /* ARC[7]=1,X=1 */
#define MC68681_BAUD_RATE_MASK_115_5K       0x08
#define MC68681_BAUD_RATE_MASK_TIMER        0xdd
#define MC68681_BAUD_RATE_MASK_TIMER_16X    0xee
#define MC68681_BAUD_RATE_MASK_TIMER_1X     0xff

#endif
