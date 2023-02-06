/**
 * @file 
 *
 * @ingroup arm_termios
 *
 * @brief termios callback based on uart.
 */

#include <bsp/irq.h>
#include <bsp/uart.h>
//#include <bsp/termios-config.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <rtems/console.h>

#include <inttypes.h>
#include <assert.h>

//* Define this to use interrupt driver UART driver */
#define USE_INTERRUPTS 1


static int termios_uart_first_open(int major, int minor, void *arg);
static int termios_uart_last_close(int major, int minor, void *arg);

static int termios_uart_set_attrs(int minor, const struct termios *);


#if USE_INTERRUPTS
static void termios_uart_tx_rx_isr(void *param);
static ssize_t termios_uart_intr_write(int, const char *, size_t);
static rtems_vector_number termios_uart_vector_number(int minor);
#else
static ssize_t termios_uart_poll_write(int minor, const char * buf, size_t len);
static int termios_uart_poll_read(int minor);
static void termios_uart_poll_write_char(int minor, char c);
#endif

/* TERMIOS callbacks */
#if USE_INTERRUPTS
rtems_termios_callbacks termios_uart_cbacks = {
    .firstOpen            = termios_uart_first_open,
    .lastClose            = termios_uart_last_close,
    .pollRead             = NULL,
    .write                = termios_uart_intr_write,
    .setAttributes        = termios_uart_set_attrs,
    .stopRemoteTx         = NULL,
    .startRemoteTx        = NULL,
    .outputUsesInterrupts = 1,
};
#else
rtems_termios_callbacks termios_uart_cbacks = {
    .firstOpen            = termios_uart_first_open,
    .lastClose            = termios_uart_last_close,
    .pollRead             = termios_uart_poll_read,
    .write                = termios_uart_poll_write,
    .setAttributes        = termios_uart_set_attrs,
    .stopRemoteTx         = NULL,
    .startRemoteTx        = NULL,
    .outputUsesInterrupts = 0,
};
#endif

typedef struct {
    int minor;
    volatile const char *buf;
    volatile int len;
    volatile int idx;
    void *tty;
} termios_uart_data_t;

static termios_uart_data_t termios_uart_data[KINETIS_UART_MAX_NUM];

extern void termios_uart_init(int minor);

void termios_uart_init(int minor)
{
    termios_uart_data[minor].minor = minor;
    termios_uart_data[minor].buf   = NULL;
    termios_uart_data[minor].len   = 0;
    termios_uart_data[minor].idx   = 0;
}

static rtems_vector_number termios_uart_vector_number(int minor)
{
    rtems_vector_number vector = BSP_INTERRUPT_VECTOR_MAX + 1;
    switch (minor) {
		case KINETIS_UART_0:
			vector = KINETIS_IRQ_UART0_RX_TX;
			break;
		case KINETIS_UART_1:
			vector = KINETIS_IRQ_UART1_RX_TX;
			break;
		case KINETIS_UART_2:
			vector = KINETIS_IRQ_UART2_RX_TX;
			break;
		case KINETIS_UART_3:
			vector = KINETIS_IRQ_UART3_RX_TX;
			break;
		case KINETIS_UART_4:
			vector = KINETIS_IRQ_UART4_RX_TX;
			break;
		case KINETIS_UART_5:
			vector = KINETIS_IRQ_UART5_RX_TX;
			break;
		default:
			break;
	}
    return vector;
}

static void termios_uart_tx_rx_isr(void *param)
{
    termios_uart_data_t *uart_data = param;
    char buf[1];
    int i = 0;
    int len;
    int minor = uart_data->minor;

    if (UART_GET_DATA == kinetis_uart_read_char(minor, &buf[i]))
    {
        i++;
        rtems_termios_enqueue_raw_characters(uart_data->tty, buf, i);
    }
    
    if (UART_HAVE_SENT == kinetis_uart_write_char_in_isr(minor, uart_data->buf[uart_data->idx]))
    {   
        uart_data->idx++;
        if (uart_data->idx >= uart_data->len)
        {
            len = uart_data->len;
            uart_data->len = 0;
            kinetis_uart_tx_interrupt_disable(minor);
            kinetis_uart_release();
            rtems_termios_dequeue_characters(uart_data->tty, len);
        }
    }
}

static int termios_uart_first_open(int major, int minor, void *arg)
{
    rtems_libio_open_close_args_t *args = arg;
    rtems_status_code status = RTEMS_SUCCESSFUL;

    termios_uart_data[minor].tty   = args->iop->data1;

#if USE_INTERRUPTS
    status = rtems_interrupt_handler_install(
        termios_uart_vector_number(minor),
        "UART",
        RTEMS_INTERRUPT_UNIQUE,
        termios_uart_tx_rx_isr,
        &termios_uart_data[minor]
    );
    assert(status == RTEMS_SUCCESSFUL);

    kinetis_uart_rx_interrupt_enable(minor);
    kinetis_uart_tx_interrupt_disable(minor);
#endif

    return 0;
}

static int termios_uart_last_close(int major, int minor, void *arg)
{
#if USE_INTERRUPTS
    rtems_status_code status = RTEMS_SUCCESSFUL;

    status = rtems_interrupt_handler_remove(
        termios_uart_vector_number(minor),
        termios_uart_tx_rx_isr,
        &termios_uart_data[minor]
    );
    assert(status == RTEMS_SUCCESSFUL);
#endif

    return 0;
}

#if USE_INTERRUPTS
static ssize_t termios_uart_intr_write(int minor, const char *buf, size_t len)
{
    if (len > 0) {
        termios_uart_data[minor].buf = buf;
        termios_uart_data[minor].len = len;
        termios_uart_data[minor].idx = 0;

        kinetis_uart_tx_interrupt_enable(minor);
        kinetis_uart_obtain();
    }

    return 1;
}
#else
static int termios_uart_poll_read(int minor)
{
    return kinetis_uart_read_char_in_polling(minor);
}

static ssize_t termios_uart_poll_write(int minor, const char *buf, size_t len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        kinetis_uart_write_char_in_polling(minor, buf[i]);
    }
    return 1;

}
#endif

/* This is for setting baud rate, bits, etc. */
static int termios_uart_set_attrs(int minor, const struct termios *t)
{
    int baud;

    baud = rtems_termios_baud_to_number(t->c_ospeed);
    kinetis_uart_set_bauderate(minor, baud);

    return 0;
}