/*
 *  MIPS Mongoose-V specific information
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __MONGOOSEV_h
#define __MONGOOSEV_h

/*
 *  Macros to assist in accessing memory mapped Mongoose registers
 */

#define MONGOOSEV_READ( _base ) \
  *((volatile unsigned32 *)(_base))

#define MONGOOSEV_WRITE( _base, _value ) \
  *((volatile unsigned32 *)(_base)) = (_value)

#define MONGOOSEV_READ_REGISTER( _base, _register ) \
  *((volatile unsigned32 *)((_base) + (_register)))

#define MONGOOSEV_WRITE_REGISTER( _base, _register, _value ) \
  *((volatile unsigned32 *)((_base) + (_register))) = (_value)

#define MONGOOSEV_ATOMIC_MASK( _addr, _mask, _new ) \
  do { \
    rtems_interrupt_level  Irql; \
    rtems_unsigned32       tmp; \
    \
    rtems_interrupt_disable(Irql); \
      tmp = *((volatile unsigned32 *)(_addr)) & ~(_mask); \
      *((volatile unsigned32 *)(_addr)) = tmp | (_new); \
    rtems_interrupt_enable(Irql); \
  } while (0)

/*
 *  BIU and DRAM Registers
 */

#define MONGOOSEV_BIU_CACHE_CONFIGURATION_REGISTER       0xFFFE0130
#define MONGOOSEV_DRAM_CONFIGURATION_REGISTER            0xFFFE0120
#define MONGOOSEV_REFRESH_TIMER_INITIAL_COUNTER_REGISTER 0xFFFE0010
#define MONGOOSEV_WAIT_STATE_CONFIGURATION_REGISTER_BASE 0xFFFE0100

/*
 *  Peripheral Function Addresses
 *
 *  NOTE: Status and Interrupt Cause use the same bits
 */

#define MONGOOSEV_PERIPHERAL_COMMAND_REGISTER                   0xFFFE0180
#define MONGOOSEV_PERIPHERAL_STATUS_REGISTER                    0xFFFE0184
#define MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER  0xFFFE0188
#define MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_MASK_REGISTER   0xFFFE018C

#define MONGOOSEV_PFICR MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER
#define MONGOOSEV_PFIMR MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_MASK_REGISTER

#define mongoosev_set_in_pficr( _mask, _bits ) \
  MONGOOSEV_ATOMIC_MASK( MONGOOSEV_PFICR, _mask, _bits )
#define mongoosev_clear_in_pficr( _mask, _bits ) \
  MONGOOSEV_ATOMIC_MASK( MONGOOSEV_PFICR, _mask, ~(_bits) )

#define mongoosev_set_in_pfimr( _mask, _bits ) \
  MONGOOSEV_ATOMIC_MASK( MONGOOSEV_PFIMR, _mask, _bits )
#define mongoosev_clear_in_pfimr( _mask, _bits ) \
  MONGOOSEV_ATOMIC_MASK( MONGOOSEV_PFIMR, _mask, ~(_bits) )

/* UART Bits in Peripheral Command Register Bits (TX/RX tied together here) */
#define MONGOOSEV_UART_CMD_RESET_BOTH_PORTS   0x0001
#define MONGOOSEV_UART_CMD_LOOPBACK_CTSN      0x0002
#define MONGOOSEV_UART_CMD_LOOPBACK_RXTX      0x0004

#define MONGOOSEV_UART_CMD_TX_ENABLE        0x001
#define MONGOOSEV_UART_CMD_TX_DISABLE       0x000
#define MONGOOSEV_UART_CMD_RX_ENABLE        0x002
#define MONGOOSEV_UART_CMD_RX_DISABLE       0x000
#define MONGOOSEV_UART_CMD_TX_READY         0x004
#define MONGOOSEV_UART_CMD_PARITY_ENABLE    0x008
#define MONGOOSEV_UART_CMD_PARITY_DISABLE   0x000
#define MONGOOSEV_UART_CMD_PARITY_EVEN      0x800
#define MONGOOSEV_UART_CMD_PARITY_ODD       0x000

#define MONGOOSEV_UART0_CMD_SHIFT 5
#define MONGOOSEV_UART1_CMD_SHIFT 11

#define MONGOOSEV_UART_CMD_TX_ENABLE_0 \
        (MONGOOSEV_UART_CMD_TX_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_RX_ENABLE_0 \
        (MONGOOSEV_UART_CMD_RX_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_TX_READY_0 \
        (MONGOOSEV_UART_CMD_TX_READY << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ENABLE_0 \
        (MONGOOSEV_UART_CMD_PARITY_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_DISABLE_0 \
        (MONGOOSEV_UART_CMD_PARITY_DISABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_EVEN_0 \
        (MONGOOSEV_UART_CMD_PARITY_EVEN << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ODD_0 \
        (MONGOOSEV_UART_CMD_PARITY_ODD << MONGOOSEV_UART0_CMD_SHIFT)

#define MONGOOSEV_UART_CMD_TX_ENABLE_1 \
        (MONGOOSEV_UART_CMD_TX_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_RX_ENABLE_1 \
        (MONGOOSEV_UART_CMD_RX_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_TX_READY_1 \
        (MONGOOSEV_UART_CMD_TX_READY << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ENABLE_1 \
        (MONGOOSEV_UART_CMD_PARITY_ENABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_DISABLE_1 \
        (MONGOOSEV_UART_CMD_PARITY_DISABLE << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_EVEN_1 \
        (MONGOOSEV_UART_CMD_PARITY_EVEN << MONGOOSEV_UART0_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ODD_1 \
        (MONGOOSEV_UART_CMD_PARITY_ODD << MONGOOSEV_UART0_CMD_SHIFT)

/* UART Bits in Peripheral Status and Interrupt Cause Register */
#define MONGOOSEV_UART_RX_FRAME_ERROR              0x0001
#define MONGOOSEV_UART_RX_OVERRUN_ERROR            0x0002
#define MONGOOSEV_UART_TX_EMPTY                    0x0004
#define MONGOOSEV_UART_TX_READY                    0x0008
#define MONGOOSEV_UART_RX_READY                    0x0010

#define MONGOOSEV_UART_ALL_RX_STATUS_BITS          0x0003
#define MONGOOSEV_UART_ALL_STATUS_BITS             0x001F
#define MONGOOSEV_UART_ALL_IRQ_BITS                0x001F

/* 
 *  The Peripheral Interrupt Status, Cause, and Mask registers have the 
 *  same bit assignments although some revisions of the document have 
 *  the Cause and Status registers incorrect.  
 */

#define MONGOOSEV_UART0_IRQ_SHIFT 11
#define MONGOOSEV_UART1_IRQ_SHIFT 17

#define MONGOOSEV_UART_FRAME_ERROR_1 \
        (MONGOOSEV_UART_FRAME_ERROR << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_OVERRUN_ERROR_1 \
        (MONGOOSEV_UART_RX_OVERRUN_ERROR << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_EMPTY_1 \
        (MONGOOSEV_UART_TX_EMPTY << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_READY_1 \
        (MONGOOSEV_UART_TX_READY << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_READY_1 \
        (MONGOOSEV_UART_RX_READY << MONGOOSEV_UART1_IRQ_SHIFT)

#define MONGOOSEV_UART_FRAME_ERROR_0 \
        (MONGOOSEV_UART_FRAME_ERROR << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_OVERRUN_ERROR_0 \
        (MONGOOSEV_UART_RX_OVERRUN_ERROR << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_EMPTY_0 \
        (MONGOOSEV_UART_TX_EMPTY << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_READY_0 \
        (MONGOOSEV_UART_TX_READY << MONGOOSEV_UART1_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_READY_0 \
        (MONGOOSEV_UART_RX_READY << MONGOOSEV_UART1_IRQ_SHIFT)

/*
 *  Bits in the Peripheral Interrupt Mask Register
 */

/*
** Interrupt Status/Cause/Mask register bits - from 31 to 0
*/
#define MONGOOSEV_EDAC_SERR_BIT          0x80000000
#define MONGOOSEV_EDAC_MERR_BIT          0x40000000
/* 29 - 24 reserved */
#define MONGOOSEV_UART_0_RX_READY        0x00008000
#define MONGOOSEV_UART_0_TX_READY        0x00004000
#define MONGOOSEV_UART_0_TX_EMPTY        0x00002000
#define MONGOOSEV_UART_0_RX_OVERRUN      0x00001000
#define MONGOOSEV_UART_0_FRAME_ERROR     0x00000800
#define MONGOOSEV_UART_0_RESERVED        0x00000400
#define MONGOOSEV_UART_1_RX_READY        0x00200000
#define MONGOOSEV_UART_1_TX_READY        0x00100000
#define MONGOOSEV_UART_1_TX_EMPTY        0x00080000
#define MONGOOSEV_UART_1_RX_OVERRUN      0x00040000
#define MONGOOSEV_UART_1_FRAME_ERROR     0x00020000
#define MONGOOSEV_UART_1_RESERVED        0x00010000
#define MONGOOSEV_MAVN_WRITE_ACCESS      0x00400000
#define MONGOOSEV_MAVN_READ_ACCESS       0x00800000
#define MONGOOSEV_EXTERN_INT_9           0x00000200
#define MONGOOSEV_EXTERN_INT_8           0x00000100
#define MONGOOSEV_EXTERN_INT_7           0x00000080
#define MONGOOSEV_EXTERN_INT_6           0x00000040
#define MONGOOSEV_EXTERN_INT_5           0x00000020
#define MONGOOSEV_EXTERN_INT_4           0x00000010
#define MONGOOSEV_EXTERN_INT_3           0x00000008
#define MONGOOSEV_EXTERN_INT_2           0x00000004
#define MONGOOSEV_EXTERN_INT_1           0x00000002
#define MONGOOSEV_EXTERN_INT_0           0x00000001


/*
 *  EDAC Registers
 */

#define MONGOOSEV_EDAC_ERROR_ADDRESS_REGISTER       0xFFFE0190
#define MONGOOSEV_EDAC_PARITY_TEST_MODE_REGISTER    0xFFFE0194

/*
 *  MAVN Registers
 */

#define MONGOOSEV_MAVN_TEST_REGISTER               0xFFFE01B4
#define MONGOOSEV_MAVN_ACCESS_PRIVILEGE_REGISTER   0xFFFE01B8
#define MONGOOSEV_MAVN_ACCESS_VIOLATION_REGISTER   0xFFFE01BC
#define MONGOOSEV_MAVN_RANGE_0_REGISTER            0xFFFE01C0
#define MONGOOSEV_MAVN_RANGE_1_REGISTER            0xFFFE01C4
#define MONGOOSEV_MAVN_RANGE_2_REGISTER            0xFFFE01C8
#define MONGOOSEV_MAVN_RANGE_3_REGISTER            0xFFFE01CC
#define MONGOOSEV_MAVN_RANGE_4_REGISTER            0xFFFE01D0
#define MONGOOSEV_MAVN_RANGE_5_REGISTER            0xFFFE01D4
#define MONGOOSEV_MAVN_RANGE_6_REGISTER            0xFFFE01D8
#define MONGOOSEV_MAVN_RANGE_7_REGISTER            0xFFFE01DC

/*
 *  Timer Base Addresses, Offsets, and Values
 */

#define MONGOOSEV_TIMER1_BASE    0xFFFE0000
#define MONGOOSEV_TIMER2_BASE    0xFFFE0008

#define MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER 0
#define MONGOOSEV_TIMER_CONTROL_REGISTER         4

/* Timer Control Register Constants */
#define MONGOOSEV_TIMER_CONTROL_COUNTER_ENABLE    0x04
#define MONGOOSEV_TIMER_CONTROL_INTERRUPT_ENABLE  0x02
#define MONGOOSEV_TIMER_CONTROL_TIMEOUT           0x01

/*
 *  UART Base Addresses and Offsets
 *
 *  Many bits in the peripheral command register are UART related
 *  and the bits are defined there.
 */

#define MONGOOSEV_UART0_BASE   0xFFFE01E8
#define MONGOOSEV_UART1_BASE   0xFFFE01F4

#define MONGOOSEV_RX_BUFFER    0
#define MONGOOSEV_TX_BUFFER    4
#define MONGOOSEV_BAUD_RATE    8

/*
 *  Interrupt Vector Numbers
 *
 *  NOTE: IRQ INT5 is logical or of peripheral cause register
 *        per p. 5-22 of Mongoose-V manual.
 */

#define MONGOOSEV_IRQ_INT0                    0
#define MONGOOSEV_IRQ_TIMER1                  MONGOOSEV_IRQ_INT0
#define MONGOOSEV_IRQ_INT1                    1
#define MONGOOSEV_IRQ_TIMER2                  MONGOOSEV_IRQ_INT1
#define MONGOOSEV_IRQ_INT2                    2
#define MONGOOSEV_IRQ_INT4                    3
/* MONGOOSEV_IRQ_INT5 indicates that a peripheral caused the IRQ. */
#define MONGOOSEV_IRQ_PERIPHERAL_BASE         4
#define MONGOOSEV_IRQ_XINT0                   4
#define MONGOOSEV_IRQ_XINT1                   5
#define MONGOOSEV_IRQ_XINT2                   6
#define MONGOOSEV_IRQ_XINT3                   7
#define MONGOOSEV_IRQ_XINT4                   8
#define MONGOOSEV_IRQ_XINT5                   9
#define MONGOOSEV_IRQ_XINT6                  10
#define MONGOOSEV_IRQ_XINT7                  11
#define MONGOOSEV_IRQ_XINT8                  12
#define MONGOOSEV_IRQ_XINT9                  13
#define MONGOOSEV_IRQ_READ_ACCESS_VIOLATION  14
#define MONGOOSEV_IRQ_WRITE_ACCESS_VIOLATION 15
#define MONGOOSEV_IRQ_RESERVED_BIT_12        16
#define MONGOOSEV_IRQ_UART1_RX_FRAME_ERROR   17
#define MONGOOSEV_IRQ_UART1_RX_OVERRUN_ERROR 18
#define MONGOOSEV_IRQ_UART1_TX_EMPTY         19
#define MONGOOSEV_IRQ_UART1_TX_READY         20
#define MONGOOSEV_IRQ_UART1_RX_READY         21
#define MONGOOSEV_IRQ_RESERVED_BIT_18        22
#define MONGOOSEV_IRQ_UART0_RX_FRAME_ERROR   23
#define MONGOOSEV_IRQ_UART0_RX_OVERRUN_ERROR 24
#define MONGOOSEV_IRQ_UART0_TX_EMPTY         25
#define MONGOOSEV_IRQ_UART0_TX_READY         26
#define MONGOOSEV_IRQ_UART0_RX_READY         27
#define MONGOOSEV_IRQ_RESERVED_24            28
#define MONGOOSEV_IRQ_RESERVED_25            29
#define MONGOOSEV_IRQ_RESERVED_26            30
#define MONGOOSEV_IRQ_RESERVED_27            31
#define MONGOOSEV_IRQ_RESERVED_28            32
#define MONGOOSEV_IRQ_RESERVED_29            33
#define MONGOOSEV_IRQ_UNCORRECTABLE_ERROR    34
#define MONGOOSEV_IRQ_CORRECTABLE_ERROR      35

#define MONGOOSEV_IRQ_SOFTWARE_1             36
#define MONGOOSEV_IRQ_SOFTWARE_2             37

#endif
