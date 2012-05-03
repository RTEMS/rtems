/****************************************************************************
* File:     sci.h
*
* Desc:     This is the include file for the serial communications interface.
*
* Note:     See bsp.h,confdefs.h,system.h for installing drivers into RTEMS.
*
****************************************************************************/

#ifndef _sci_h_
#define _sci_h_

/*******************************************************************************
  IOCTL commands for the sci driver.
  I'm still working on these...
*******************************************************************************/

#define SCI_IOCTL_PARITY_NONE           0x00    /* no parity bit after the data bits */
#define SCI_IOCTL_PARITY_ODD            0x01    /* parity bit added after data bits */
#define SCI_IOCTL_PARITY_EVEN           0x02    /* parity bit added after data bits */
#define SCI_IOCTL_PARITY_MARK           0x03    /* parity bit is lo, -12 volts, logical 1 */
#define SCI_IOCTL_PARITY_SPACE          0x04    /* parity bit is hi, +12 volts, logical 0 */
#define SCI_IOCTL_PARITY_FORCED_ON      0x03    /* parity bit is forced hi or lo */
#define SCI_IOCTL_PARITY_FORCED_OFF     0x04    /* parity bit is forced hi or lo */

#define SCI_IOCTL_BAUD_RATE             0x20    /* set the baud rate, arg is baud */

#define SCI_IOCTL_DATA_BITS             0x30    /* set the data bits, arg is # bits */

#define SCI_IOCTL_STOP_BITS_1           0x40    /* 1 stop bit after char frame */
#define SCI_IOCTL_STOP_BITS_2           0x41    /* 2 stop bit after char frame */

#define SCI_IOCTL_MODE_NORMAL           0x50    /* normal operating mode */
#define SCI_IOCTL_MODE_LOOP             0x51    /* internal loopback mode */

#define SCI_IOCTL_FLOW_NONE             0x60    /* no flow control */
#define SCI_IOCTL_FLOW_RTS_CTS          0x61    /* hardware flow control */

#define SCI_IOCTL_SEND_BREAK            0x70    /* send an rs-232 break */

#define SCI_IOCTL_MODE_1200             0x80    /* 1200,n,8,1 download mode */
#define SCI_IOCTL_MODE_9600             0x81    /* 9600,n,8,1 download mode */
#define SCI_IOCTL_MODE_9_BIT            0x82    /* 9600,forced,8,1 command mode */


/*******************************************************************************
  SCI Registers
*******************************************************************************/

/* SCI Control Register 0  (SCCR0)  $FFFC08

    8 4 2 1 - 8 4 2 1 - 8 4 2 1 - 8 4 2 1
    ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^
    | | | |   | | | |   | | | |   | | | |
    | | | |   | | | |   | | | |   | | | +-----   0 baud rate divisor
    | | | |   | | | |   | | | |   | | +-------   1 baud rate divisor
    | | | |   | | | |   | | | |   | +---------   2 baud rate divisor
    | | | |   | | | |   | | | |   +-----------   3 baud rate divisor
    | | | |   | | | |   | | | |
    | | | |   | | | |   | | | +---------------   4 baud rate divisor
    | | | |   | | | |   | | +-----------------   5 baud rate divisor
    | | | |   | | | |   | +-------------------   6 baud rate divisor
    | | | |   | | | |   +---------------------   7 baud rate divisor
    | | | |   | | | |
    | | | |   | | | +-------------------------   8 baud rate divisor
    | | | |   | | +---------------------------   9 baud rate divisor
    | | | |   | +-----------------------------  10 baud rate divisor
    | | | |   +-------------------------------  11 baud rate divisor
    | | | |
    | | | +-----------------------------------  12 baud rate divisor
    | | +-------------------------------------  13 unused
    | +---------------------------------------  14 unused
    +-----------------------------------------  15 unused

    0 0 0 0 - 0 0 0 0 - 0 0 0 0 - 0 1 0 0       reset value - (64k baud?)
 */

#define SCI_BAUD_57_6K            9
#define SCI_BAUD_38_4K           14
#define SCI_BAUD_19_2K           27
#define SCI_BAUD_9600            55
#define SCI_BAUD_4800           109
#define SCI_BAUD_2400           218
#define SCI_BAUD_1200           437


/*  SCI Control Register 1  (SCCR1)  $FFFC0A

    8 4 2 1 - 8 4 2 1 - 8 4 2 1 - 8 4 2 1
    ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^
    | | | |   | | | |   | | | |   | | | |
    | | | |   | | | |   | | | |   | | | +-----   0 send a break
    | | | |   | | | |   | | | |   | | +-------   1 rcvr wakeup mode
    | | | |   | | | |   | | | |   | +---------   2 rcvr enable
    | | | |   | | | |   | | | |   +-----------   3 xmtr enable
    | | | |   | | | |   | | | |
    | | | |   | | | |   | | | +---------------   4 idle line intr enable
    | | | |   | | | |   | | +-----------------   5 rcvr intr enable
    | | | |   | | | |   | +-------------------   6 xmit complete intr enable
    | | | |   | | | |   +---------------------   7 xmtr intr enable
    | | | |   | | | |
    | | | |   | | | +-------------------------   8 wakeup on address mark
    | | | |   | | +---------------------------   9 mode 1=9 bits, 0=8 bits
    | | | |   | +-----------------------------  10 parity enable 1=on, 0=off
    | | | |   +-------------------------------  11 parity type 1=odd, 0=even
    | | | |
    | | | +-----------------------------------  12 idle line select
    | | +-------------------------------------  13 wired-or mode
    | +---------------------------------------  14 loop mode
    +-----------------------------------------  15 unused

    0 0 0 0 - 0 0 0 0 - 0 0 0 0 - 0 0 0 0       reset value
*/

#define SCI_SEND_BREAK          0x0001          /* 0000-0000-0000-0001 */
#define SCI_RCVR_WAKEUP         0x0002          /* 0000-0000-0000-0010 */
#define SCI_ENABLE_RCVR         0x0004          /* 0000-0000-0000-0100 */
#define SCI_ENABLE_XMTR         0x0008          /* 0000-0000-0000-1000 */

#define SCI_DISABLE_RCVR        0xFFFB          /* 1111-1111-1111-1011 */
#define SCI_DISABLE_XMTR        0xFFF7          /* 1111-1111-1111-0111 */

#define SCI_ENABLE_INT_IDLE     0x0010          /* 0000-0000-0001-0000 */
#define SCI_ENABLE_INT_RX       0x0020          /* 0000-0000-0010-0000 */
#define SCI_ENABLE_INT_TX_DONE  0x0040          /* 0000-0000-0100-0000 */
#define SCI_ENABLE_INT_TX       0x0080          /* 0000-0000-1000-0000 */

#define SCI_DISABLE_INT_ALL     0xFF00          /* 1111-1111-0000-0000 ??? */

#define SCI_DISABLE_INT_RX      0xFFDF          /* 1111-1111-1101-1111 */
#define SCI_CLEAR_RX_INT        0xFFBF          /* 1111-1111-1011-1111 */
#define SCI_DISABLE_INT_TX      0xFF7F          /* 1111-1111-0111-1111 */
#define SCI_CLEAR_TDRE          0xFEFF          /* 1111-1110-1111-1111 */

#define SCI_RCVR_WAKE_ON_MARK   0x0100          /* 0000-0001-0000-0000 */
#define SCI_9_DATA_BITS         0x0200          /* 0000-0010-0000-0000 */
#define SCI_PARITY_ENABLE       0x0400          /* 0000-0100-0000-0000 */
#define SCI_PARITY_ODD          0x0800          /* 0000-1000-0000-0000 */

#define SCI_RCVR_WAKE_ON_IDLE   0xFEFF          /* 1111-1110-1111-1111 */
#define SCI_8_DATA_BITS         0xFDFF          /* 1111-1101-1111-1111 */
#define SCI_PARITY_DISABLE      0xFBFF          /* 1111-1011-1111-1111 */
#define SCI_PARITY_EVEN         0xF7FF          /* 1111-0111-1111-1111 */

#define SCI_PARITY_NONE         0xF3FF          /* 1111-0011-1111-1111 */

#define SCI_IDLE_LINE_LONG      0x1000          /* 0001-0000-0000-0000 */
#define SCI_TXD_OPEN_DRAIN      0x2000          /* 0010-0000-0000-0000 */
#define SCI_LOOPBACK_MODE       0x4000          /* 0100-0000-0000-0000 */
#define SCI_SCCR1_UNUSED        0x8000          /* 1000-0000-0000-0000 */


/*  SCI Status Register  (SCSR)  $FFFC0C

    8 4 2 1 - 8 4 2 1 - 8 4 2 1 - 8 4 2 1
    ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^   ^ ^ ^ ^
    | | | |   | | | |   | | | |   | | | |
    | | | |   | | | |   | | | |   | | | +-----   0 PF - parity error
    | | | |   | | | |   | | | |   | | +-------   1 FE - framing error
    | | | |   | | | |   | | | |   | +---------   2 NF - noise flag
    | | | |   | | | |   | | | |   +-----------   3 OR - overrun flag
    | | | |   | | | |   | | | |
    | | | |   | | | |   | | | +---------------   4 IDLE - idle line detected
    | | | |   | | | |   | | +-----------------   5 RAF  - rcvr active flag
    | | | |   | | | |   | +-------------------   6 RDRF - rcv data reg full
    | | | |   | | | |   +---------------------   7 TC   - xmt complete flag
    | | | |   | | | |
    | | | |   | | | +-------------------------   8 TDRE - xmt data reg empty
    | | | |   | | +---------------------------   9 always zero
    | | | |   | +-----------------------------  10 always zero
    | | | |   +-------------------------------  11 always zero
    | | | |
    | | | +-----------------------------------  12 always zero
    | | +-------------------------------------  13 always zero
    | +---------------------------------------  14 always zero
    +-----------------------------------------  15 always zero

    0 0 0 0 - 0 0 0 1 - 1 0 0 0 - 0 0 0 0       reset value
*/

#define SCI_ERROR_PARITY        0x0001          /* 0000-0000-0000-0001 */
#define SCI_ERROR_FRAMING       0x0002          /* 0000-0000-0000-0010 */
#define SCI_ERROR_NOISE         0x0004          /* 0000-0000-0000-0100 */
#define SCI_ERROR_OVERRUN       0x0008          /* 0000-0000-0000-1000 */

#define SCI_IDLE_LINE           0x0010          /* 0000-0000-0001-0000 */
#define SCI_RCVR_ACTIVE         0x0020          /* 0000-0000-0010-0000 */
#define SCI_RCVR_READY          0x0040          /* 0000-0000-0100-0000 */
#define SCI_XMTR_IDLE           0x0080          /* 0000-0000-1000-0000 */

#define SCI_CLEAR_RX_INT        0xFFBF          /* 1111-1111-1011-1111 */

#define SCI_XMTR_READY          0x0100          /* 0000-0001-0000-0000 */

#define SCI_CLEAR_TDRE          0xFEFF          /* 1111-1110-1111-1111 */

#define SCI_XMTR_AVAILABLE      0x0180          /* 0000-0001-1000-0000 */



/*******************************************************************************
  Function prototypes
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* look at console_open to see how this is called */

const rtems_termios_callbacks * SciGetTermiosHandlers( int32_t   polled );

/* SCI interrupt */

/*rtems_isr SciIsr( rtems_vector_number vector ); */

/*int32_t   SciOpenPolled    ( int32_t   major, int32_t   minor, void *arg ); */
/*int32_t   SciOpenInterrupt ( int32_t   major, int32_t   minor, void *arg ); */

/*int32_t   SciClose         ( int32_t   major, int32_t   minor, void *arg ); */

/*int32_t   SciWritePolled   ( int32_t   minor, const char *buf, int32_t   len ); */
/*int32_t   SciWriteInterrupt( int32_t   minor, const char *buf, int32_t   len ); */

/*int32_t   SciReadPolled    ( int32_t   minor ); */

/*int32_t   SciSetAttributes ( int32_t   minor, const struct termios *t ); */

#ifdef __cplusplus
}
#endif

#endif  /* _sci_h_ */
