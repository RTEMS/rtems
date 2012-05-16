/*
 *  This file contains the driver for the APBUART serial port.
 *  No console driver, only char driver.
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  2007-07-11, Daniel Hellstrom <daniel@gaisler.com>
 *    Added ioctl command APBUART_CLR_STATS
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <string.h>

#include <ambapp.h>
#include <grlib.h>
#include <apbuart.h>

#ifndef DEFAULT_TXBUF_SIZE
 #define DEFAULT_TXBUF_SIZE 32
#endif
#ifndef DEFAULT_RXBUF_SIZE
 #define DEFAULT_RXBUF_SIZE 32
#endif

#ifndef APBUART_PREFIX
 #define APBUART_PREFIX(name) apbuart##name
#endif

#if !defined(APBUART_DEVNAME) || !defined(APBUART_DEVNAME_NO)
 #undef APBUART_DEVNAME
 #undef APBUART_DEVNAME_NO
 #define APBUART_DEVNAME "/dev/apbuart0"
 #define APBUART_DEVNAME_NO(devstr,no) ((devstr)[12]='0'+(no))
#endif

#ifndef APBUART_REG_INT
	#define APBUART_REG_INT(handler,irq,arg) set_vector(handler,irq+0x10,1)
  #undef APBUART_DEFINE_INTHANDLER
  #define APBUART_DEFINE_INTHANDLER
#endif

/* Default to 40MHz system clock */
/*#ifndef SYS_FREQ_HZ
 #define SYS_FREQ_HZ 40000000
#endif*/

typedef struct {
	int size;
	unsigned char *buf,
	              *tail,
		            *head,
								*max;
	int full; /* no more place in fifo */
} apbuart_fifo;

static apbuart_fifo *apbuart_fifo_create(int size);
static void apbuart_fifo_free(apbuart_fifo *fifo);
static inline int apbuart_fifo_isFull(apbuart_fifo *fifo);
static inline int apbuart_fifo_isEmpty(apbuart_fifo *fifo);
static int apbuart_fifo_put(apbuart_fifo *fifo, unsigned char c);
static int apbuart_fifo_get(apbuart_fifo *fifo, unsigned char *c);
static int inline apbuart_fifo_peek(apbuart_fifo *fifo, unsigned char **c);
static void inline apbuart_fifo_skip(apbuart_fifo *fifo);

static rtems_device_driver apbuart_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor,  void *arg);
static rtems_device_driver apbuart_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver apbuart_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver apbuart_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver apbuart_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver apbuart_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

typedef struct {
	struct apbuart_regs *regs;
	int irq;
	int minor;
	int scaler;
	unsigned int baud;

	int txblk;    /* Make write block until at least 1 char has
	               * been put into software send fifo
								 */
	int tx_flush; /* Set this to block until all data has
	               * placed into the hardware send fifo
								 */
	int rxblk;    /* Make read block until at least 1 char has
	               * been received (or taken from software fifo).
								 */
	int started;  /* Set to 1 when in running mode */

	int ascii_mode; /* Set to 1 to make \n be printed as \r\n */

	/* TX/RX software FIFO Buffers */
	apbuart_fifo *txfifo;
	apbuart_fifo *rxfifo;

	apbuart_stats stats;

	rtems_id dev_sem;
	rtems_id rx_sem;
	rtems_id tx_sem;
} apbuart_priv;

static int dev_cnt;
static apbuart_priv *apbuarts;
static unsigned int sys_freq_hz;

#define APBUART_DRIVER_TABLE_ENTRY { apbuart_initialize, apbuart_open, apbuart_close, apbuart_read, apbuart_write, apbuart_control }

static rtems_driver_address_table apbuart_driver = APBUART_DRIVER_TABLE_ENTRY;
static struct ambapp_bus *amba_bus;

static void apbuart_interrupt(apbuart_priv *uart);
#ifdef APBUART_DEFINE_INTHANDLER
static void apbuart_interrupt_handler(rtems_vector_number v);
#endif
static void apbuart_hw_close(apbuart_priv *uart);
static void apbuart_hw_open(apbuart_priv *uart);

/* Uncomment for debug output */
/* #define DEBUG 1
 #define FUNCDEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif
#ifdef FUNCDEBUG
#define FUNCDBG(x...) printk(x)
#else
#define FUNCDBG(x...)
#endif

#ifndef READ_REG
	#define READ_REG(address) _APBUART_READ_REG((unsigned int)(address))
  static __inline__ unsigned int _APBUART_READ_REG(unsigned int addr) {
        unsigned int tmp;
        __asm__ (" lda [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
	}
#endif

#if 0
static int apbuart_outbyte_try(struct apbuart_regs *regs, unsigned char ch)
{
	if ( (READ_REG(&regs->status) & LEON_REG_UART_STATUS_THE) == 0 )
		return -1; /* Failed */

	/* There is room in fifo, put ch in it */
	regs->data = (unsigned int) ch;
	return 0;
}


static int apbuart_inbyte_try(struct apbuart_regs *regs)
{
	unsigned int status;
	/* Clear errors if any */
	if ( (status=READ_REG(&regs->status)) & LEON_REG_UART_STATUS_ERR) {
		regs->status = status & ~LEON_REG_UART_STATUS_ERR;
	}

	/* Is Data available? */
	if ( (READ_REG(&regs->status) & LEON_REG_UART_STATUS_DR) == 0 )
		return -1; /* No data avail */

	/* Return Data */
	return (int)READ_REG(&regs->data);
}

static int apbuart_write_support(apbuart_priv *uart, const char *buf, int len)
{
	int nwrite = 0;

	while (nwrite < len) {
		if ( apbuart_outbyte_try(minor, *buf++) ){
			/* TX Fifo full */

		}
		nwrite++;
	}
	return nwrite;
}
#endif

static void apbuart_hw_open(apbuart_priv *uart){
	unsigned int scaler;

	/* Calculate Baudrate */
	if ( uart->scaler > 0 ) {
		scaler = uart->scaler;
	}else{
		scaler = (((sys_freq_hz*10)/(uart->baud*8))-5)/10;
	}

	/* Set new baud rate */
	uart->regs->scaler = scaler;

	/* Enable receiver & Transmitter */
	uart->regs->ctrl = APBUART_CTRL_RE | APBUART_CTRL_RF | APBUART_CTRL_RI | APBUART_CTRL_TI;
}

static void apbuart_hw_close(apbuart_priv *uart){
	/* disable receiver & transmitter & all IRQs */
	uart->regs->ctrl = 0;
}

#ifdef APBUART_DEFINE_INTHANDLER
/* interrupt handler */
static void apbuart_interrupt_handler(rtems_vector_number v){
	int minor;

	/* convert to */
  for(minor = 0; minor < dev_cnt; minor++) {
  	if ( v == (apbuarts[minor].irq+0x10) ) {
			apbuart_interrupt(&apbuarts[minor]);
			return;
		}
	}
}
#endif

/* The interrupt handler, taking care of the
 * APBUART hardware
 */
static void apbuart_interrupt(apbuart_priv *uart){
	unsigned int status;
	int empty;
	unsigned char c, *next_char = NULL;
	int signal;

	/* Clear & record any error */
	status = READ_REG(&uart->regs->status);
	if ( status & (APBUART_STATUS_OV|APBUART_STATUS_PE|APBUART_STATUS_FE) ){
		/* Data overrun */
		if ( status & APBUART_STATUS_OV ){
			uart->stats.hw_dovr++;
		}
		/* Parity error */
		if ( status & APBUART_STATUS_PE ){
			uart->stats.hw_parity++;
		}
		/* Framing error */
		if ( status & APBUART_STATUS_FE ){
			uart->stats.hw_frame++;
		}
		uart->regs->status = status & ~(APBUART_STATUS_OV|APBUART_STATUS_PE|APBUART_STATUS_FE);
	}

	/* Empty RX fifo into software fifo */
	signal = 0;
	while ( (status=READ_REG(&uart->regs->status)) & APBUART_STATUS_DR ){
		c = READ_REG(&uart->regs->data);
		if ( apbuart_fifo_isFull(uart->rxfifo) ){
			uart->stats.sw_dovr++;
      DBG("]");
			break;
		}
		/* put into fifo */
		apbuart_fifo_put(uart->rxfifo,c);

		/* bump RX counter */
		uart->stats.rx_cnt++;

		signal = 1;
	}

	/* Wake RX thread if any */
	if ( signal )
		rtems_semaphore_release(uart->rx_sem);

	/* If room in HW fifo and we got more chars to be sent */
	if ( !(status & APBUART_STATUS_TF) ){

		if ( apbuart_fifo_isEmpty(uart->txfifo) ){
			/* Turn off TX interrupt when no data is to be sent */
			if ( status & APBUART_STATUS_TE ){
				uart->regs->ctrl = READ_REG(&uart->regs->ctrl) & ~APBUART_CTRL_TF;
				DBG("?");
			}
			return;
		}

		/* signal when there will be more room in SW fifo */
		if ( apbuart_fifo_isFull(uart->txfifo) )
			signal = 1;

		do{
			/* Put data into HW TX fifo */
			apbuart_fifo_peek(uart->txfifo,&next_char);
			c = *next_char;
			if ( uart->ascii_mode && ( c == '\n') ){
				uart->regs->data = '\n';
				*next_char = '\r'; /* avoid sending mutiple '\n' or '\r' */
			}else{
				uart->regs->data = c;
				apbuart_fifo_skip(uart->txfifo); /* remove sent char from fifo */
			}
			uart->regs->ctrl = READ_REG(&uart->regs->ctrl) | APBUART_CTRL_TE | APBUART_CTRL_TF;
			DBG("!");
		}while(!(empty=apbuart_fifo_isEmpty(uart->txfifo)) &&
		       !((status=READ_REG(&uart->regs->status))&APBUART_STATUS_TF) );

		/* Wake userspace thread, on empty or full fifo
		 * This makes tx_flush and block work.
		 */
		if ( signal || empty ){
			rtems_semaphore_release(uart->tx_sem);
		}
	}
}

int APBUART_PREFIX(_register)(struct ambapp_bus *bus) {
	rtems_status_code r;
	rtems_device_major_number m;

	amba_bus = bus;

	FUNCDBG("apbuart_register:\n");

	if ((r = rtems_io_register_driver(0, &apbuart_driver, &m)) == RTEMS_SUCCESSFUL) {
		DBG("APBUART driver successfully registered, major: %d\n", m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("APBUART rtems_io_register_driver failed: RTEMS_TOO_MANY\n"); return -1;
		case RTEMS_INVALID_NUMBER:
			printk("APBUART rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n"); return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("APBUART rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n"); return -1;
		default:
		   printk("APBUART rtems_io_register_driver failed\n");
			 return -1;
		}
	}
	return 0;
}

static rtems_device_driver apbuart_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor,  void *arg)
{

	rtems_status_code status;
	int i;
	struct ambapp_apb_info dev;
	char fs_name[20];

	FUNCDBG("apbuart_initialize\n");

	/* Find all APB UART devices */
	dev_cnt = ambapp_get_number_apbslv_devices(amba_bus, VENDOR_GAISLER,
	                                           GAISLER_APBUART);
	if ( dev_cnt < 1 ){
		/* Failed to find any CAN cores! */
		printk("APBUART: Failed to find any APBUART cores\n\r");
		return -1;
	}

	strcpy(fs_name,APBUART_DEVNAME);

	DBG("Found %d APBUART(s)\n\r",dev_cnt);

	/* Allocate memory for device structures */
	apbuarts = malloc(sizeof(apbuart_priv) * dev_cnt);
	if ( !apbuarts ){
		printk("APBUART: Failed to allocate SW memory\n\r");
		return -1;
	}

  memset(apbuarts,0,sizeof(sizeof(apbuart_priv) * dev_cnt));

	/* Detect System Frequency from initialized timer */
#ifndef SYS_FREQ_HZ
#if defined(LEON3)
	/* LEON3: find timer address via AMBA Plug&Play info */
	{
		struct ambapp_apb_info gptimer;
		struct gptimer_regs *tregs;

		if ( ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER,
		                        GAISLER_GPTIMER, &gptimer) == 1 ){
			tregs = (struct gptimer_regs *)gptimer.start;
			sys_freq_hz = (tregs->scaler_reload+1)*1000*1000;
			DBG("APBUART: detected %dHZ system frequency\n\r",sys_freq_hz);
		}else{
			sys_freq_hz = 40000000; /* Default to 40MHz */
			printk("APBUART: Failed to detect system frequency\n\r");
		}

	}
#elif defined(LEON2)
	/* LEON2: use hardcoded address to get to timer */
	{
		LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;
		sys_freq_hz = (regs->Scaler_Reload+1)*1000*1000;
	}
#else
  #error CPU not supported for OC_CAN driver
#endif
#else
	/* Use hardcoded frequency */
	sys_freq_hz = SYS_FREQ_HZ;
#endif

	for(i=0; i<dev_cnt; i++){
		/* Get AMBA AHB device info from Plug&Play */
		ambapp_find_apbslv_next(amba_bus,VENDOR_GAISLER,GAISLER_APBUART,&dev,i);

		printk("APBUART[%d]: at 0x%x irq %d (0x%x)\n\r",i,dev.start,dev.irq,(unsigned int)&apbuarts[i]);

		apbuarts[i].regs = (struct apbuart_regs *)dev.start;
		apbuarts[i].irq = dev.irq;
		apbuarts[i].minor = i;

		/* Clear HW regs */
		apbuarts[i].regs->status = 0;
		apbuarts[i].regs->ctrl = 0;

		/* Allocate default software buffers */
		apbuarts[i].txfifo = apbuart_fifo_create(DEFAULT_TXBUF_SIZE);
		apbuarts[i].rxfifo = apbuart_fifo_create(DEFAULT_RXBUF_SIZE);
		if ( !apbuarts[i].txfifo || !apbuarts[i].rxfifo )
			rtems_fatal_error_occurred(RTEMS_NO_MEMORY);

		APBUART_DEVNAME_NO(fs_name,i);

		/* Bind name to device */
		DBG("APBUART[%d]: binding to name %s\n\r",i,fs_name);
		status = rtems_io_register_name(fs_name, major, i);
		if (status != RTEMS_SUCCESSFUL)
			rtems_fatal_error_occurred(status);

		/* Setup interrupt handler for each channel */
   	APBUART_REG_INT(APBUART_PREFIX(_interrupt_handler), apbuarts[i].irq, &apbuarts[i]);

		/* Device A Semaphore created with count = 1 */
		if ( rtems_semaphore_create(rtems_build_name('A', 'U', 'D', '0'+i),
		     1,
		     RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		     0,
		     &apbuarts[i].dev_sem) != RTEMS_SUCCESSFUL )
			return RTEMS_INTERNAL_ERROR;

		if ( rtems_semaphore_create(rtems_build_name('A', 'U', 'T', '0'+i),
		     1,
		     RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		     0,
		     &apbuarts[i].tx_sem) != RTEMS_SUCCESSFUL )
			return RTEMS_INTERNAL_ERROR;

		if ( rtems_semaphore_create(rtems_build_name('A', 'U', 'R', '0'+i),
		     1,
		     RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		     0,
		     &apbuarts[i].rx_sem) != RTEMS_SUCCESSFUL )
			return RTEMS_INTERNAL_ERROR;

	}
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver apbuart_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	apbuart_priv *uart;

	FUNCDBG("apbuart_open: major %d, minor %d\n", major, minor);

	if ( (minor < 0) || (minor >= dev_cnt) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NAME;
	}

	uart = &apbuarts[minor];

	if (rtems_semaphore_obtain(uart->dev_sem, RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL) {
		DBG("apbuart_open: resource in use\n");
		return RTEMS_RESOURCE_IN_USE;
	}

	/* Clear HW regs */
	uart->regs->status = 0;
	uart->regs->ctrl = 0;

	/* Set Defaults */

	/* 38400 baudrate */
	uart->scaler = 0; /* use uart->baud */
	uart->baud = 38400;

	/* Default to Blocking mode */
	uart->txblk = 1;
	uart->rxblk = 1;

	/* Default to no flush mode */
	uart->tx_flush = 0;

	/* non-ascii mode */
	uart->ascii_mode = 0;

  /* not started */
  uart->started = 0;

	if ( !uart->txfifo || (uart->txfifo->size!=DEFAULT_TXBUF_SIZE) ){
		apbuart_fifo_free(uart->txfifo);
		uart->txfifo = apbuart_fifo_create(DEFAULT_TXBUF_SIZE);
	}

	if ( !uart->rxfifo || (uart->rxfifo->size!=DEFAULT_RXBUF_SIZE) ){
		apbuart_fifo_free(uart->rxfifo);
		uart->rxfifo = apbuart_fifo_create(DEFAULT_RXBUF_SIZE);
	}

	if ( !uart->rxfifo || !uart->txfifo ){
		/* Failed to get memory */
		return RTEMS_NO_MEMORY;
	}

	/* Now user must call ioctl(START,0) to begin */

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver apbuart_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	apbuart_priv *uart = &apbuarts[minor];

	FUNCDBG("apbuart_close[%d]:\n",minor);

	apbuart_hw_close(uart);

	/* Software state will be set when open is called again */
	rtems_semaphore_release(uart->rx_sem);
	rtems_semaphore_release(uart->tx_sem);
	uart->started = 0;

	rtems_semaphore_release(uart->dev_sem);

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver apbuart_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_rw_args_t *rw_args;
	unsigned int count = 0, oldLevel;
	unsigned char *buf;
	apbuart_priv *uart = &apbuarts[minor];

	rw_args = (rtems_libio_rw_args_t *) arg;

	FUNCDBG("apbuart_read\n");

	buf = (unsigned char *)rw_args->buffer;
	if ( (rw_args->count < 1) || !buf )
		return RTEMS_INVALID_NAME; /* EINVAL */

	rtems_interrupt_disable(oldLevel);
	do {
		if ( (unsigned int)uart < 0x40000000 ) {
      printk("UART %x is screwed\n",uart);
    }
		/* Read from SW fifo */
		if ( apbuart_fifo_get(uart->rxfifo,&buf[count]) != 0 ){
			/* non blocking or read at least 1 byte */
			if ( (count > 0) || (!uart->rxblk) )
				break; /* Return */

			rtems_interrupt_enable(oldLevel);

			/* Block thread until a char is received */
			rtems_semaphore_obtain(uart->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

			rtems_interrupt_disable(oldLevel);
			continue;
		}

		/* Got char from SW FIFO */
		count++;

	} while (count < rw_args->count );

	rtems_interrupt_enable(oldLevel);

	rw_args->bytes_moved = count;

	if (count == 0)
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver apbuart_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_rw_args_t *rw_args;
	unsigned int count, oldLevel, ctrl;
	char *buf;
	apbuart_priv *uart = &apbuarts[minor];
	int direct=0;


	rw_args = (rtems_libio_rw_args_t *) arg;

	FUNCDBG("apbuart_write\n");

	buf = rw_args->buffer;

	if ( rw_args->count < 1 || !buf )
		return RTEMS_INVALID_NAME; /* EINVAL */

	count = 0;
	rtems_interrupt_disable(oldLevel);
	/* Do we need to start to send first char direct via HW
	 * to get IRQ going.
	 */

	ctrl = READ_REG(&uart->regs->ctrl);
	if ( (ctrl & APBUART_CTRL_TF) == 0 ){
		/* TX interrupt is disabled ==>
		 * SW FIFO is empty and,
		 * HW FIFO empty
		 */
		uart->regs->ctrl = ctrl | APBUART_CTRL_TF;
		if ( uart->ascii_mode && (buf[0] == '\n') ){
			uart->regs->data = '\r';
		}else{
			uart->regs->data = buf[0];
			count++;
		}
		uart->regs->ctrl = ctrl | APBUART_CTRL_TE | APBUART_CTRL_TF;
		direct = 1;
	}

	while( count < rw_args->count ) {
		/* write to HW FIFO direct skipping SW FIFO */
		if ( direct && ((READ_REG(&uart->regs->status) & APBUART_STATUS_TF) == 0) ){
			uart->regs->data = buf[count];
		}
		/* write to SW FIFO */
		else if ( apbuart_fifo_put(uart->txfifo,buf[count]) ){
			direct = 0;
			DBG("APBUART[%d]: write: SW FIFO Full\n\r",minor);

			/* is full, block? */
			if ( ((count < 1) && uart->txblk) || uart->tx_flush ){

				rtems_interrupt_enable(oldLevel);

				rtems_semaphore_obtain(uart->tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

				rtems_interrupt_disable(oldLevel);

				/* Do we need to start to send first char direct via HW
				 * to get IRQ going.
				 */

				ctrl = READ_REG(&uart->regs->ctrl);
				if ( (ctrl & APBUART_CTRL_TF) == 0 ){
					/* TX interrupt is disabled ==>
					 * SW FIFO is empty and,
					 * HW FIFO empty
					 */
					uart->regs->ctrl = ctrl | APBUART_CTRL_TF;
					if ( uart->ascii_mode && (buf[count] == '\n') ){
						uart->regs->data = '\r';
					}else{
						uart->regs->data = buf[count];
						count++;
					}
					uart->regs->ctrl = ctrl | APBUART_CTRL_TF | APBUART_CTRL_TE;
					direct = 1;
				}

				continue;
			}
			/* don't block, return current status */
			break;
		}else{
			direct = 0;
		}

		count++;

	}

	rtems_interrupt_enable(oldLevel);

	rw_args->bytes_moved = count;

	if (count == 0)
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver apbuart_control(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *)arg;
	unsigned int *data = ioarg->buffer;
	apbuart_priv *uart = &apbuarts[minor];
	int size;
	unsigned int baudrate, blocking;
	apbuart_stats *stats;

	FUNCDBG("apbuart_control [%i,%i]\n",major, minor);

	if (!ioarg)
		return RTEMS_INVALID_NAME;

  ioarg->ioctl_return = 0;
	switch(ioarg->command) {

	/* Enable Receiver & transmitter */
	case APBUART_START:
		if ( uart->started )
			return RTEMS_INVALID_NAME;
		apbuart_hw_open(uart);
		uart->started = 1;
		break;

	/* Close Receiver & transmitter */
	case APBUART_STOP:
		if ( !uart->started )
			return RTEMS_INVALID_NAME;
		apbuart_hw_close(uart);
		uart->started = 0;
		break;

	/* Set RX FIFO Software buffer length
	 * It is only possible to change buffer size in
	 * non-running mode.
	 */
	case APBUART_SET_RXFIFO_LEN:
		if ( uart->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */

		size = (int)ioarg->buffer;
		if ( size < 1 )
			return RTEMS_INVALID_NAME; /* EINVAL */

		/* Free old buffer */
		apbuart_fifo_free(uart->rxfifo);

		/* Allocate new buffer & init it */
		uart->rxfifo = apbuart_fifo_create(size);
		if ( !uart->rxfifo )
			return RTEMS_NO_MEMORY;
		break;

	/* Set TX FIFO Software buffer length
	 * It is only possible to change buffer size
	 * while in non-running mode.
	 */
	case APBUART_SET_TXFIFO_LEN:
		if ( uart->started )
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */

		size = (int)ioarg->buffer;
		if ( size < 1 )
			return RTEMS_INVALID_NAME; /* EINVAL */

		/* Free old buffer */
		apbuart_fifo_free(uart->txfifo);

		/* Allocate new buffer & init it */
		uart->txfifo = apbuart_fifo_create(size);
		if ( !uart->txfifo )
			return RTEMS_NO_MEMORY;
		break;

	case APBUART_SET_BAUDRATE:
		/* Set baud rate of */
		baudrate = (int)ioarg->buffer;
		if ( (baudrate < 1) || (baudrate > 115200) ){
			return RTEMS_INVALID_NAME;
		}
		uart->scaler = 0; /* use uart->baud */
		uart->baud = baudrate;
		break;

	case APBUART_SET_SCALER:
		/* use uart->scaler not uart->baud */
		uart->scaler = data[0];
		break;

	case APBUART_SET_BLOCKING:
		blocking = (unsigned int)ioarg->buffer;
		uart->rxblk = ( blocking & APBUART_BLK_RX );
		uart->txblk = ( blocking & APBUART_BLK_TX );
		uart->tx_flush = ( blocking & APBUART_BLK_FLUSH );
		break;

	case APBUART_GET_STATS:
		stats = (void *)ioarg->buffer;
		if ( !stats )
			return RTEMS_INVALID_NAME;

		/* Copy Stats */
		*stats = uart->stats;
		break;

  case APBUART_CLR_STATS:
		/* Clear/reset Stats */
    memset(&uart->stats,0,sizeof(uart->stats));
		break;

	case APBUART_SET_ASCII_MODE:
		uart->ascii_mode = (int)ioarg->buffer;
		break;

	default:
		return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}


/******************* APBUART FIFO implementation ***********************/

static apbuart_fifo *apbuart_fifo_create(int size){
	apbuart_fifo *fifo;
	fifo = (apbuart_fifo *) malloc(size + sizeof(apbuart_fifo));
	if ( fifo ) {
		/* Init fifo */
		fifo->size = size;
		fifo->buf = (unsigned char *)(fifo+1);
		fifo->tail = fifo->buf;
		fifo->head = fifo->buf;
		fifo->max = &fifo->buf[size-1];
		fifo->full=0;
	}
	return fifo;
}

static void apbuart_fifo_free(apbuart_fifo *fifo){
	if ( fifo )
		free(fifo);
}

static inline int apbuart_fifo_isFull(apbuart_fifo *fifo){
	return fifo->full;
}

static inline int apbuart_fifo_isEmpty(apbuart_fifo *fifo){
	if ( (fifo->head == fifo->tail) && !fifo->full )
		return -1;
	return 0;
}

static int apbuart_fifo_put(apbuart_fifo *fifo, unsigned char c){
	if ( !fifo->full ){
		*fifo->head = c;
		fifo->head = (fifo->head >= fifo->max ) ? fifo->buf : fifo->head+1;
		if ( fifo->head == fifo->tail )
			fifo->full = -1;
		return 0;
	}
	return -1;
}

static int apbuart_fifo_get(apbuart_fifo *fifo, unsigned char *c){
	if ( apbuart_fifo_isEmpty(fifo) )
		return -1;
	if ( c )
		*c = *fifo->tail;
	fifo->tail = (fifo->tail >= fifo->max ) ? fifo->buf : fifo->tail+1;
	fifo->full = 0;
	return 0;
}

static int inline apbuart_fifo_peek(apbuart_fifo *fifo, unsigned char **c){
	if ( apbuart_fifo_isEmpty(fifo) )
		return -1;
	if ( c )
		*c = fifo->tail;
	return 0;
}

static void inline apbuart_fifo_skip(apbuart_fifo *fifo){
	if ( !apbuart_fifo_isEmpty(fifo) ){
		fifo->tail = (fifo->tail >= fifo->max ) ? fifo->buf : fifo->tail+1;
		fifo->full = 0;
	}
}
