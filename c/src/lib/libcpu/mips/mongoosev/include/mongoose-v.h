/**
 *  @file
 *  
 *  MIPS Mongoose-V specific information
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MONGOOSEV_h
#define __MONGOOSEV_h

/*
 *  Macros to assist in accessing memory mapped Mongoose registers
 */


#define MONGOOSEV_READ( _base ) \
  ( *((volatile uint32_t*)(_base)) )

#define MONGOOSEV_WRITE( _base, _value ) \
  ( *((volatile uint32_t*)(_base)) = (_value) )

#define MONGOOSEV_READ_REGISTER( _base, _register ) \
  ( *((volatile uint32_t*)((_base) + (_register))) )

#define MONGOOSEV_WRITE_REGISTER( _base, _register, _value ) \
  ( *((volatile uint32_t*)((_base) + (_register))) = (_value) )





/*
 * Macros to read/write the Mongoose FPU control register.
 */




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

#define MONGOOSEV_WATCHDOG			0xBE000000

/* UART Bits in Peripheral Command Register Bits (TX/RX tied together here) */
#define MONGOOSEV_UART_CMD_RESET_BOTH_PORTS	0x0001
#define MONGOOSEV_UART_CMD_LOOPBACK_CTSN	0x0002
#define MONGOOSEV_UART_CMD_LOOPBACK_RXTX	0x0004

#define MONGOOSEV_UART_CMD_RX_ENABLE		0x001
#define MONGOOSEV_UART_CMD_RX_DISABLE		0x000
#define MONGOOSEV_UART_CMD_TX_ENABLE		0x002
#define MONGOOSEV_UART_CMD_TX_DISABLE		0x000
#define MONGOOSEV_UART_CMD_TX_READY		0x004
#define MONGOOSEV_UART_CMD_PARITY_ENABLE	0x008
#define MONGOOSEV_UART_CMD_PARITY_DISABLE	0x000
#define MONGOOSEV_UART_CMD_PARITY_EVEN		0x010
#define MONGOOSEV_UART_CMD_PARITY_ODD		0x000

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
        (MONGOOSEV_UART_CMD_TX_ENABLE << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_RX_ENABLE_1 \
        (MONGOOSEV_UART_CMD_RX_ENABLE << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_TX_READY_1 \
        (MONGOOSEV_UART_CMD_TX_READY << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ENABLE_1 \
        (MONGOOSEV_UART_CMD_PARITY_ENABLE << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_DISABLE_1 \
        (MONGOOSEV_UART_CMD_PARITY_DISABLE << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_EVEN_1 \
        (MONGOOSEV_UART_CMD_PARITY_EVEN << MONGOOSEV_UART1_CMD_SHIFT)
#define MONGOOSEV_UART_CMD_PARITY_ODD_1 \
        (MONGOOSEV_UART_CMD_PARITY_ODD << MONGOOSEV_UART1_CMD_SHIFT)

/* UART Bits in Peripheral Status and Interrupt Cause Register */
#define MONGOOSEV_UART_RX_FRAME_ERROR              0x0001
#define MONGOOSEV_UART_RX_OVERRUN_ERROR            0x0002
#define MONGOOSEV_UART_TX_EMPTY                    0x0004
#define MONGOOSEV_UART_TX_READY                    0x0008
#define MONGOOSEV_UART_RX_READY                    0x0010

#define MONGOOSEV_UART_ALL_RX_STATUS_BITS          0x0013
#define MONGOOSEV_UART_ALL_STATUS_BITS             0x001F

/*
 *  The Peripheral Interrupt Status, Cause, and Mask registers have the
 *  same bit assignments although some revisions of the document have
 *  the Cause and Status registers incorrect.
 */

#define MONGOOSEV_UART0_IRQ_SHIFT 11
#define MONGOOSEV_UART1_IRQ_SHIFT 17

#define MONGOOSEV_UART_FRAME_ERROR_0 \
        (MONGOOSEV_UART_FRAME_ERROR << MONGOOSEV_UART0_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_OVERRUN_ERROR_0 \
        (MONGOOSEV_UART_RX_OVERRUN_ERROR << MONGOOSEV_UART0_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_EMPTY_0 \
        (MONGOOSEV_UART_TX_EMPTY << MONGOOSEV_UART0_IRQ_SHIFT)
#define MONGOOSEV_UART_TX_READY_0 \
        (MONGOOSEV_UART_TX_READY << MONGOOSEV_UART0_IRQ_SHIFT)
#define MONGOOSEV_UART_RX_READY_0 \
        (MONGOOSEV_UART_RX_READY << MONGOOSEV_UART0_IRQ_SHIFT)

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

/*
 *  Bits in the Peripheral Interrupt Mask Register
 */

/*
** Interrupt Status/Cause/Mask register bits - from 31 to 0
*/
#define MONGOOSEV_EDAC_SERR_BIT          0x80000000
#define MONGOOSEV_EDAC_MERR_BIT          0x40000000
/* 29 - 24 reserved */
#define MONGOOSEV_MAVN_WRITE_ACCESS      0x00800000
#define MONGOOSEV_MAVN_READ_ACCESS       0x00400000
#define MONGOOSEV_UART_1_RX_READY        0x00200000
#define MONGOOSEV_UART_1_TX_READY        0x00100000
#define MONGOOSEV_UART_1_TX_EMPTY        0x00080000
#define MONGOOSEV_UART_1_RX_OVERRUN      0x00040000
#define MONGOOSEV_UART_1_FRAME_ERROR     0x00020000
#define MONGOOSEV_RESERVED_16            0x00010000
#define MONGOOSEV_UART_0_RX_READY        0x00008000
#define MONGOOSEV_UART_0_TX_READY        0x00004000
#define MONGOOSEV_UART_0_TX_EMPTY        0x00002000
#define MONGOOSEV_UART_0_RX_OVERRUN      0x00001000
#define MONGOOSEV_UART_0_FRAME_ERROR     0x00000800
#define MONGOOSEV_RESERVED_10            0x00000400
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
** Peripheral Command bits (non-uart, those are defined above)
*/
#define MONGOOSEV_COMMAND_ENABLE_EDAC	MONGOOSEV_EDAC_SERR_BIT
#define MONGOOSEV_COMMAND_OVERRIDE_EDAC MONGOOSEV_EDAC_MERR_BIT



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
 *  Status Register Bits
 */

#define SR_CUMASK	0xf0000000	/* coproc usable bits */
#define SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define SR_CU0		0x10000000	/* Coprocessor 0 usable */
#define SR_BEV		0x00400000	/* use boot exception vectors */
#define SR_TS		0x00200000	/* TLB shutdown */
#define SR_PE		0x00100000	/* cache parity error */
#define SR_CM		0x00080000	/* cache miss */
#define SR_PZ		0x00040000	/* cache parity zero */
#define SR_SWC		0x00020000	/* swap cache */
#define SR_ISC		0x00010000	/* Isolate data cache */
#define SR_IMASK	0x0000ff00	/* Interrupt mask */
#define SR_IMASK8	0x00000000	/* mask level 8 */
#define SR_IMASK7	0x00008000	/* mask level 7 */
#define SR_IMASK6	0x0000c000	/* mask level 6 */
#define SR_IMASK5	0x0000e000	/* mask level 5 */
#define SR_IMASK4	0x0000f000	/* mask level 4 */
#define SR_IMASK3	0x0000f800	/* mask level 3 */
#define SR_IMASK2	0x0000fc00	/* mask level 2 */
#define SR_IMASK1	0x0000fe00	/* mask level 1 */
#define SR_IMASK0	0x0000ff00	/* mask level 0 */

#define SR_IBIT8	0x00008000	/* bit level 8 */
#define SR_IBIT7	0x00004000	/* bit level 7 */
#define SR_IBIT6	0x00002000	/* bit level 6 */
#define SR_IBIT5	0x00001000	/* bit level 5 */
#define SR_IBIT4	0x00000800	/* bit level 4 */
#define SR_IBIT3	0x00000400	/* bit level 3 */
#define SR_IBIT2	0x00000200	/* bit level 2 */
#define SR_IBIT1	0x00000100	/* bit level 1 */

#define SR_KUO		0x00000020	/* old kernel/user, 0 => k, 1 => u */
#define SR_IEO		0x00000010	/* old interrupt enable, 1 => enable */
#define SR_KUP		0x00000008	/* prev kernel/user, 0 => k, 1 => u */
#define SR_IEP		0x00000004	/* prev interrupt enable, 1 => enable */
#define SR_KUC		0x00000002	/* cur kernel/user, 0 => k, 1 => u */
#define SR_IEC		0x00000001	/* cur interrupt enable, 1 => enable */
#define SR_KUMSK	(SR_KUO|SR_IEO|SR_KUP|SR_IEP|SR_KUC|SR_IEC)

#define SR_IMASKSHIFT	8



#define MONGOOSEV_IC_SIZE	0x1000		/* instruction cache = 4Kbytes */
#define MONGOOSEV_DC_SIZE	0x800		/* data cache 2Kbytes */

#endif
