/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial port on the MVME230x.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>

/* PC16550D register definitions */
#define UART_BASE_ADDR ((char *) IO_TO_LOCAL(0x03f8))

#define RBR 0
#define THR 0
#define DLL 0
#define DLM 1

#define LCR 3
#   define FIVE_BITS     0x2
#   define SIX_BITS      0x2
#   define SEVEN_BITS    0x2
#   define EIGHT_BITS    0x3
#   define ONE_STOP_BIT  0x0
#   define TWO_STOP_BITS 0x4
#   define NO_PARITY     0x00
#   define ODD_PARITY    0x08
#   define EVEN_PARITY   0x18
#   define DLAB          0x80

#define LSR 5
#   define DR 0x01
#   define OE 0x02
#   define PE 0x04
#   define FE 0x08
#   define BI 0x10
#   define THRE 0x20
#   define TEMT 0x40
#   define ERR_IN_FIFO 0x40

#define FCR 2
#   define ENABLE_FIFOS  0x01
#   define RESET_RC_FIFO 0x02
#   define RESET_TX_FIFO 0x04
#   define FIFO_MODE_1   0x08
#   define RX_TRIGGER_1  0x00
#   define RX_TRIGGER_4  0x40
#   define RX_TRIGGER_8  0x80
#   define RX_TRIGGER_14 0xC0

#define IIR 2
#   define NO_INTS_PENDING 0x01
#   define RX_ERROR        0x06
#   define RX_DATA_READY   0x04
#   define TX_NOT_FULL     0x02
#   define MODEM_STATUS    0x00
#   define INT_ID_MASK     0x06

#define IER 1
#   define ENA_RX_READY_INT     0x1
#   define ENA_TX_EMPTY_INT     0x2
#   define ENA_RX_ERROR_INT     0x4
#   define ENA_MODEM_STATUS_INT 0x8

#define MSR 6
#   define DELTA_CTS   0x01
#   define DELTA_DSR   0x02
#   define END_OF_RING 0x04
#   define DELTA_DCD   0x08
#   define CTS         0x10
#   define DSR         0x20
#   define RING        0x40
#   define DCD         0x80

#define MCR 4
#   define DTR      0x01
#   define RTS      0x02
#   define OUT1     0x04
#   define OUT2     0x08
#   define LOOPBACK 0x10

#define SCR 7

int use_polled_io = CONSOLE_USE_POLLED;
void *tty_ptr_for_irq;

/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
setAttributes (int minor, const struct termios *t)
{
	int divisor;
    volatile char *uart = UART_BASE_ADDR;
    char lcr_image = 0;

    /* set character size */
    switch (t->c_cflag & CSIZE) {
        case CS5:   lcr_image = FIVE_BITS;   break;
        case CS6:   lcr_image = SIX_BITS;    break;
        case CS7:   lcr_image = SEVEN_BITS;  break;
        case CS8:   lcr_image = EIGHT_BITS;  break;
    }

    /* set number of stop bits */
    if (t->c_cflag & CSTOPB) {
        lcr_image |= TWO_STOP_BITS;
    } else {
        lcr_image |= ONE_STOP_BIT;
    }

    /* set parity */
    if ((t->c_cflag & PARENB) == 0) {
        lcr_image |= NO_PARITY;
    } else if (t->c_cflag & PARODD) {
        lcr_image |= ODD_PARITY;
    } else {
        lcr_image |= EVEN_PARITY;
    }

    /* set the baud rate */
	switch (t->c_cflag & CBAUD) {
        default:	    divisor = -1;	break;
        case B50:	    divisor = 2304;	break;
        case B75:	    divisor = 1536;	break;
        case B110:	    divisor = 1047;	break;
        case B134:	    divisor = 857;	break;
        case B150:	    divisor = 768;	break;
        case B200:	    divisor = 576;	break;
        case B300:	    divisor = 384;	break;
        case B600:	    divisor = 192;	break;
        case B1200:	    divisor = 96;	break;
        case B1800:	    divisor = 64;	break;
        case B2400:	    divisor = 48;	break;
        case B4800:	    divisor = 24;	break;
        case B9600:	    divisor = 12;	break;
        case B19200:	divisor = 6;	break;
        case B38400:	divisor = 3;	break;
        case B57600:	divisor = 2;	break;
        case B115200:	divisor = -1;	break;
        case B230400:	divisor = -1;	break;
        case B460800:	divisor = -1;	break;
	}
    if (divisor > 0) {
		uart[LCR] = (lcr_image |= DLAB);
        uart[DLM] = divisor >> 8;
        uart[DLL] = divisor;
        lcr_image &= ~DLAB;
    }

    /* activate the new configuration */
    uart[LCR] = lcr_image;
	return 0;
}

/*
 * Interrupt handler
 */
static rtems_isr consoleInterruptHandler (rtems_vector_number v)
{
    volatile char *uart = UART_BASE_ADDR;
    volatile char ignore;
    char buffer[16], iir_image;
    int len;

    while (((iir_image = uart[IIR]) & NO_INTS_PENDING) == 0) {
        switch (iir_image & INT_ID_MASK) {
            case RX_ERROR:
                /* 
                 *  read the line status register to clear condition. 
                 *  nothing done with the information at this point.
                 */
                ignore = uart[LSR];
                break;
            case RX_DATA_READY:
                /* 
                 *  empty the receive fifo and transfer bytes to termios layer
                 */
                len = 0;
                do {
                    buffer[len++] = uart[RBR];
                } while (len < sizeof(buffer) && (uart[LSR] & DR));
                rtems_termios_enqueue_raw_characters(tty_ptr_for_irq, 
                                                     buffer, len);
                break;
            case TX_NOT_FULL:
                /*
                 *  notify termios layer that characters have been sent
                 */
                len = uart[SCR];
                /* we'll get one interrupt when we start - ignore it */
                if (len) {
                    rtems_termios_dequeue_characters(tty_ptr_for_irq, len);
                }
                break;
            case MODEM_STATUS:
                /* 
                 *  read the modem status register to clear condition. 
                 *  nothing done with the information at this point.
                 */
                ignore = uart[MSR];
                break;
        }
    }
}

/*
 *  console_inbyte_nonblocking 
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
    volatile char *uart = UART_BASE_ADDR;

    if (uart[LSR] & DR) {
        return uart[RBR];
    } else {
        return -1;
    }
}


/*
 *  Console Termios Support Entry Points
 *
 */

int console_write_polled (
    int minor,
    const char *buf,
    int len
)
{
    int nwrite = 0;
    volatile char *uart = UART_BASE_ADDR;

    while (nwrite < len) {
        /* wait as long as needed for transmit buffer to empty */
        while ((uart[LSR] & THRE) == 0) ;
        uart[THR] = *buf++;
        nwrite++;
    }

    /* return value seems to be ignored by termios layer, we'll return count */
    return nwrite;
}

int console_write_interrupts (
    int minor,
    const char *buf,
    int len
)
{
    const int FIFO_depth = 16;
    int nwrite;
    volatile char *uart = UART_BASE_ADDR;

    /* write up to len characters, stopping if FIFO fills */
    for (nwrite = 0; nwrite < len && nwrite < FIFO_depth; nwrite++) {
        uart[THR] = *buf++;
    }

    /* save count of bytes sent so interrupt can report it to termios layer */
    uart[SCR] = nwrite;

    /* return value seems to be ignored by termios layer, we'll return count */
    return nwrite;
}

/*
 *  Console Device Driver Entry Points
 *
 */
 
/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
    rtems_status_code status;
    volatile char *uart = UART_BASE_ADDR;

    rtems_termios_initialize();

    /*
    *  Register Device Names
    */

    status = rtems_io_register_name( "/dev/console", major, 0 );
    if (status != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred(status);
    }
    /* default mode:  9600 bps, 8 bits, 1 stop, no parity */
#   define DEFAULT_MODE (EIGHT_BITS | ONE_STOP_BIT | NO_PARITY)
    uart[LCR] = DEFAULT_MODE | DLAB;
    uart[DLM] = 0;
    uart[DLL] = 12;
    uart[LCR] = DEFAULT_MODE;
    
    /* enable the receive and transmit FIFOs */
    uart[FCR] = ENABLE_FIFOS | RESET_RC_FIFO | RESET_TX_FIFO | RX_TRIGGER_1;
    
    /* set modem control lines */
    uart[MCR] = DTR | RTS;

    /* zero the scratch register - this will hold a count of tx characters */
    uart[SCR] = 0;
    
    /* initialize interrupts */
    if (!use_polled_io) {
        rtems_isr_entry old_handler;
        rtems_status_code sc;

        sc = bsp_interrupt_catch(consoleInterruptHandler, IRQ_UART, 
                                 &old_handler);
        sc = bsp_interrupt_enable(IRQ_UART, PRIORITY_ISA_INT);
        uart[IER] = ENA_RX_READY_INT | ENA_TX_EMPTY_INT;
    } else {
        uart[IER] = 0;
    }
    return RTEMS_SUCCESSFUL;
}

/*
 * Open the device
 */
rtems_device_driver console_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    rtems_status_code sc;
    static const rtems_termios_callbacks pollCallbacks = {
        NULL,                        /* firstOpen */
        NULL,                        /* lastClose */
        console_inbyte_nonblocking,  /* pollRead */
        console_write_polled,        /* write */
        setAttributes,               /* setAttributes */
        NULL,                        /* stopRemoteTx */
        NULL,                        /* startRemoteTx */
        0                            /* outputUsesInterrupts */
    };
    static const rtems_termios_callbacks intrCallbacks = {
        NULL,                        /* firstOpen */
        NULL,                        /* lastClose */
        NULL,                        /* pollRead */
        console_write_interrupts,    /* write */
        setAttributes,               /* setAttributes */
        NULL,                        /* stopRemoteTx */
        NULL,                        /* startRemoteTx */
        1                            /* outputUsesInterrupts */
    };


    if ( minor > 1 )
        return RTEMS_INVALID_NUMBER;

    if (use_polled_io) {
        sc = rtems_termios_open (major, minor, arg, &pollCallbacks );
    } else {
        sc = rtems_termios_open (major, minor, arg, &intrCallbacks );
        tty_ptr_for_irq = ((rtems_libio_open_close_args_t *)arg)->iop->data1;
    }
    return RTEMS_SUCCESSFUL;
}
 
/*
 * Close the device
 */
rtems_device_driver console_close(
      rtems_device_major_number major,
      rtems_device_minor_number minor,
      void                    * arg
)
{
  return rtems_termios_close (arg);
}
 
/*
 * Read from the device
 */
rtems_device_driver console_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    return rtems_termios_read (arg);
}
 
/*
 * Write to the device
 */
rtems_device_driver console_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    return rtems_termios_write (arg);
}
 
/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    return rtems_termios_ioctl (arg);
}

/*
 * Reserve resources consumed by this driver
 */
void console_reserve_resources(
    rtems_configuration_table *configuration
)
{
    rtems_termios_reserve_resources( configuration, 1 );
}
