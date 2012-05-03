/*
 * Console driver for MC9328XML UARTs.
 *
 * Written Jay Monkman <jtm@lopingdog.com>
 * Copyright (c) 2005 by Loping Dog Embedded Systems
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *    http://www.rtems.com/license
 */
#include <bsp.h>
#include <rtems/libio.h>
#include <libchip/sersupp.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <termios.h>
#include <rtems/irq.h>
#include <bsp/irq.h>
#include <mc9328mxl.h>


/* Define this to use interrupt driver UART driver */
#define USE_INTERRUPTS 1

/* How many serial ports? */
#define NUM_DEVS       2
#define poll_write(c)  imx_uart_poll_write_char(0, c)
#define poll_read()  imx_uart_poll_read_char(0)

static int imx_uart_first_open(int, int, void *);
static int imx_uart_last_close(int, int, void *);
static int imx_uart_poll_read(int);
static int imx_uart_set_attrs(int, const struct termios *);
static void imx_uart_init(int minor);
static void imx_uart_set_baud(int, int);
static ssize_t imx_uart_poll_write(int, const char *, size_t);

#if defined(USE_INTERRUPTS)
static void imx_uart_tx_isr(rtems_irq_hdl_param);
static void imx_uart_rx_isr(rtems_irq_hdl_param);
static void imx_uart_isr_on(const rtems_irq_connect_data *irq);
static void imx_uart_isr_off(const rtems_irq_connect_data *irq);
static int imx_uart_isr_is_on(const rtems_irq_connect_data *irq);
static ssize_t imx_uart_intr_write(int, const char *, size_t);
#endif



/* TERMIOS callbacks */
#if defined(USE_INTERRUPTS)
rtems_termios_callbacks imx_uart_cbacks = {
    .firstOpen            = imx_uart_first_open,
    .lastClose            = imx_uart_last_close,
    .pollRead             = NULL,
    .write                = imx_uart_intr_write,
    .setAttributes        = imx_uart_set_attrs,
    .stopRemoteTx         = NULL,
    .startRemoteTx        = NULL,
    .outputUsesInterrupts = 1,
};
#else
rtems_termios_callbacks imx_uart_cbacks = {
    .firstOpen            = imx_uart_first_open,
    .lastClose            = imx_uart_last_close,
    .pollRead             = imx_uart_poll_read,
    .write                = imx_uart_poll_write,
    .setAttributes        = imx_uart_set_attrs,
    .stopRemoteTx         = NULL,
    .startRemoteTx        = NULL,
    .outputUsesInterrupts = 0,
};
#endif

#if defined(USE_INTERRUPTS)
static rtems_irq_connect_data imx_uart_tx_isr_data[NUM_DEVS];
static rtems_irq_connect_data imx_uart_rx_isr_data[NUM_DEVS];
#endif

typedef struct {
    int minor;
    mc9328mxl_uart_regs_t * regs;
    volatile const char *buf;
    volatile int len;
    volatile int idx;
    void *tty;
} imx_uart_data_t;

static imx_uart_data_t imx_uart_data[NUM_DEVS];

rtems_device_driver console_initialize(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
    rtems_status_code status;
    int i;

    for (i = 0; i < NUM_DEVS; i++) {
        imx_uart_init(i);
    }

    rtems_termios_initialize();

    /* /dev/console and /dev/tty0 are the same */
    status = rtems_io_register_name("/dev/console", major, 0);
    if (status != RTEMS_SUCCESSFUL) {
        rtems_panic("%s:%d Error registering /dev/console :: %d\n",
                    __FUNCTION__, __LINE__, status);
    }

    status = rtems_io_register_name("/dev/tty0", major, 0);
    if (status != RTEMS_SUCCESSFUL) {
        rtems_panic("%s:%d Error registering /dev/tty0 :: %d\n",
                    __FUNCTION__, __LINE__, status);
    }

    status = rtems_io_register_name("/dev/tty1", major, 1);
    if (status != RTEMS_SUCCESSFUL) {
        rtems_panic("%s:%d Error registering /dev/tty1 :: %d\n",
                    __FUNCTION__, __LINE__, status);
    }
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    rtems_status_code rc;

    if (minor > (NUM_DEVS - 1)) {
        return RTEMS_INVALID_NUMBER;
    }

    rc = rtems_termios_open(major, minor, arg, &imx_uart_cbacks);

    return rc;
}

rtems_device_driver console_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    return rtems_termios_close(arg);
}

rtems_device_driver console_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_ioctl(arg);
}

static void imx_uart_init(int minor)
{
    imx_uart_data[minor].minor = minor;
    imx_uart_data[minor].buf   = NULL;
    imx_uart_data[minor].len   = 0;
    imx_uart_data[minor].idx   = 0;

    if (minor == 0) {
#if defined(USE_INTERRUPTS)
        imx_uart_tx_isr_data[minor].name = BSP_INT_UART1_TX;
        imx_uart_rx_isr_data[minor].name = BSP_INT_UART1_RX;
#endif
        imx_uart_data[minor].regs =
            (mc9328mxl_uart_regs_t *) MC9328MXL_UART1_BASE;
    } else if (minor == 1) {
#if defined(USE_INTERRUPTS)
        imx_uart_tx_isr_data[minor].name = BSP_INT_UART2_TX;
        imx_uart_rx_isr_data[minor].name = BSP_INT_UART2_RX;
#endif
        imx_uart_data[minor].regs =
            (mc9328mxl_uart_regs_t *) MC9328MXL_UART2_BASE;
    } else {
        rtems_panic("%s:%d Unknown UART minor number %d\n",
                    __FUNCTION__, __LINE__, minor);
    }

#if defined(USE_INTERRUPTS)
    imx_uart_tx_isr_data[minor].hdl    = imx_uart_tx_isr;
    imx_uart_tx_isr_data[minor].handle = &imx_uart_data[minor];
    imx_uart_tx_isr_data[minor].on     = imx_uart_isr_on;
    imx_uart_tx_isr_data[minor].off    = imx_uart_isr_off;
    imx_uart_tx_isr_data[minor].isOn   = imx_uart_isr_is_on;

    imx_uart_rx_isr_data[minor].hdl    = imx_uart_rx_isr;
    imx_uart_rx_isr_data[minor].handle  = &imx_uart_data[minor];
    imx_uart_rx_isr_data[minor].on     = imx_uart_isr_on;
    imx_uart_rx_isr_data[minor].off    = imx_uart_isr_off;
    imx_uart_rx_isr_data[minor].isOn   = imx_uart_isr_is_on;
#endif

    imx_uart_data[minor].regs->cr1 = (
        MC9328MXL_UART_CR1_UARTCLKEN |
        MC9328MXL_UART_CR1_UARTEN);

    imx_uart_data[minor].regs->cr2 = (
        MC9328MXL_UART_CR2_IRTS |
        MC9328MXL_UART_CR2_WS   |
        MC9328MXL_UART_CR2_TXEN |
        MC9328MXL_UART_CR2_RXEN |
        MC9328MXL_UART_CR2_SRST);

    imx_uart_data[minor].regs->cr3 = 0;

    imx_uart_data[minor].regs->cr4 = 0;

    imx_uart_data[minor].regs->fcr = (
        MC9328MXL_UART_FCR_TXTL(32) |
        MC9328MXL_UART_FCR_RFDIV_1 |
        MC9328MXL_UART_FCR_RXTL(1));

    imx_uart_set_baud(minor, 38400);

}

static int imx_uart_first_open(int major, int minor, void *arg)
{
    rtems_libio_open_close_args_t *args = arg;

    imx_uart_data[minor].tty   = args->iop->data1;

#if defined(USE_INTERRUPTS)
    BSP_install_rtems_irq_handler(&imx_uart_tx_isr_data[minor]);
    BSP_install_rtems_irq_handler(&imx_uart_rx_isr_data[minor]);

    imx_uart_data[minor].regs->cr1 |= MC9328MXL_UART_CR1_RRDYEN;
#endif

    return 0;
}

static int imx_uart_last_close(int major, int minor, void *arg)
{
#if defined(USE_INTERRUPTS)
    BSP_remove_rtems_irq_handler(&imx_uart_tx_isr_data[minor]);
    BSP_remove_rtems_irq_handler(&imx_uart_rx_isr_data[minor]);
#endif

    return 0;
}

static int imx_uart_poll_read(int minor)
{
    if (imx_uart_data[minor].regs->sr2 & MC9328MXL_UART_SR2_RDR) {
        return imx_uart_data[minor].regs->rxd & 0xff;
    } else {
        return -1;
    }
}


static ssize_t imx_uart_poll_write(int minor, const char *buf, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        /* Wait for there to be room in the fifo */
        while (!(imx_uart_data[minor].regs->sr2 & MC9328MXL_UART_SR2_TXDC)) {
            continue;
        }

        imx_uart_data[minor].regs->txd = buf[i];
    }
    return 1;

}

#if defined(USE_INTERRUPTS)
static ssize_t imx_uart_intr_write(int minor, const char *buf, size_t len)
{
    imx_uart_data[minor].buf = buf;
    imx_uart_data[minor].len = len;
    imx_uart_data[minor].idx = 0;

    imx_uart_data[minor].regs->cr1 |= MC9328MXL_UART_CR1_TXMPTYEN;

    return 1;
}
#endif


/* This is for setting baud rate, bits, etc. */
static int imx_uart_set_attrs(int minor, const struct termios *t)
{
    int baud;

    baud = rtems_termios_baud_to_number(t->c_cflag & CBAUD);
    imx_uart_set_baud(minor, baud);

    return 0;
}

#if defined(USE_INTERRUPTS)
static void imx_uart_isr_on(const rtems_irq_connect_data *irq)
{
    MC9328MXL_AITC_INTENNUM = irq->name;
}
static void imx_uart_isr_off(const rtems_irq_connect_data *irq)
{
    MC9328MXL_AITC_INTDISNUM = irq->name;
}
static int imx_uart_isr_is_on(const rtems_irq_connect_data *irq)
{
    int irq_num = (int)irq->name;
    if (irq_num < 32) {
        return MC9328MXL_AITC_INTENABLEL & (1 << irq_num);
    } else {
        return MC9328MXL_AITC_INTENABLEH & (1 << (irq_num - 32));
    }
}

static void imx_uart_rx_isr(rtems_irq_hdl_param param)
{
    imx_uart_data_t *uart_data = param;
    char buf[32];
    int i=0;

    while (uart_data->regs->sr2 & MC9328MXL_UART_SR2_RDR) {
        buf[i] = uart_data->regs->rxd & 0xff;
        i++;
    }

    rtems_termios_enqueue_raw_characters(uart_data->tty, buf, i);
}

static void imx_uart_tx_isr(rtems_irq_hdl_param param)
{
    imx_uart_data_t *uart_data = param;
    int len;
    int minor = uart_data->minor;


    if (uart_data->idx < uart_data->len) {
        while ( (uart_data->regs->sr1 & MC9328MXL_UART_SR1_TRDY) &&
                (uart_data->idx < uart_data->len)) {
            uart_data->regs->txd = uart_data->buf[uart_data->idx];
            uart_data->idx++;
        }
    } else {
        len = uart_data->len;
        uart_data->len = 0;
        imx_uart_data[minor].regs->cr1 &= ~MC9328MXL_UART_CR1_TXMPTYEN;
        rtems_termios_dequeue_characters(uart_data->tty, len);
    }
}
#endif

/*
 * Set the UART's baud rate. The calculation is:
 *   (baud * 16) / ref_freq  = num/demom
 *
 *   ref_freq = perclk1 / RFDIV[2:0]
 *   BIR = num - 1
 *   BMR = demom - 1
 *
 * Setting 'num' to 16 yields this equation:
 *    demom = ref_freq / baud
 */
static void imx_uart_set_baud(int minor, int baud)
{
    unsigned int perclk1;
    unsigned int denom;
    unsigned int ref_freq = 0;
    uint32_t fcr;

    perclk1 = get_perclk1_freq();
    fcr = imx_uart_data[minor].regs->fcr;

    switch(fcr & MC9328MXL_UART_FCR_RFDIV_MASK) {
    case MC9328MXL_UART_FCR_RFDIV_1:  ref_freq = perclk1/1; break;
    case MC9328MXL_UART_FCR_RFDIV_2:  ref_freq = perclk1/2; break;
    case MC9328MXL_UART_FCR_RFDIV_3:  ref_freq = perclk1/3; break;
    case MC9328MXL_UART_FCR_RFDIV_4:  ref_freq = perclk1/4; break;
    case MC9328MXL_UART_FCR_RFDIV_5:  ref_freq = perclk1/5; break;
    case MC9328MXL_UART_FCR_RFDIV_6:  ref_freq = perclk1/6; break;
    case MC9328MXL_UART_FCR_RFDIV_7:  ref_freq = perclk1/7; break;
    default:
        rtems_panic("%s:%d Unknown RFDIV: 0x%x",
                    __FUNCTION__, __LINE__,
                    fcr & MC9328MXL_UART_FCR_RFDIV_MASK);
        break;
    }

    denom = ref_freq / baud;

    imx_uart_data[minor].regs->bir = 0xf;
    imx_uart_data[minor].regs->bmr = denom;
}


/*
 * Polled, non-blocking read from UART
 */
int imx_uart_poll_read_char(int minor)
{
    return imx_uart_poll_read(minor);
}

/*
 * Polled, blocking write from UART
 */
void  imx_uart_poll_write_char(int minor, char c)
{
    imx_uart_poll_write(minor, &c, 1);
}

/*
 * Functions for printk() and friends.
 */
void _BSP_output_char(char c)
{
    poll_write(c);
    if (c == '\n') {
        poll_write('\r');
    }
}

BSP_output_char_function_type BSP_output_char = _BSP_output_char;

int _BSP_poll_char(void)
{
    return poll_read();
}

BSP_polling_getchar_function_type BSP_poll_char = _BSP_poll_char;


