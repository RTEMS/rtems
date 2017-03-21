/*  This file contains the driver for the GRLIB APBUART serial port. The driver
 *  is implemented by using the cons.c console layer. Interrupt/Polling/Task
 *  driven mode can be configured using driver resources:
 *
 *  - mode   (0=Polling, 1=Interrupt, 2=Task-Driven-Interrupt Mode)
 *  - syscon (0=Force not Ssystem Console, 1=Suggest System Console)
 *
 *  The BSP define APBUART_INFO_AVAIL in order to add the info routine
 *  used for debugging.
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/******************* Driver manager interface ***********************/
#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <string.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <bsp/apbuart.h>
#include <ambapp.h>
#include <grlib.h>
#include <bsp/cons.h>
#include <rtems/termiostypes.h>
#include <bsp/apbuart_cons.h>

/*#define DEBUG 1  */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

/* LEON3 Low level transmit/receive functions provided by debug-uart code */
extern void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent);
extern int apbuart_inbyte_nonblocking(struct apbuart_regs *regs);
extern struct apbuart_regs *dbg_uart; /* The debug UART */

struct apbuart_priv {
	struct console_dev condev;
	struct drvmgr_dev *dev;
	struct apbuart_regs *regs;
	char devName[32];
	void *cookie;
	int sending;
	int mode;
};

/* TERMIOS Layer Callback functions */
void apbuart_get_attributes(struct console_dev *condev, struct termios *t);
int apbuart_set_attributes(int minor, const struct termios *t);
ssize_t apbuart_write_polled(int minor, const char *buf, size_t len);
int apbuart_pollRead(int minor);
ssize_t apbuart_write_intr(int minor, const char *buf, size_t len);
int apbuart_pollRead_task(int minor);
int apbuart_firstOpen(int major, int minor, void *arg);
int apbuart_lastClose(int major, int minor, void *arg);

void apbuart_isr(void *arg);
int apbuart_get_baud(struct apbuart_priv *uart);

int apbuart_init1(struct drvmgr_dev *dev);
#ifdef APBUART_INFO_AVAIL
static int apbuart_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int, char *argv[]);
#define APBUART_INFO_FUNC apbuart_info
#else
#define APBUART_INFO_FUNC NULL
#endif

struct drvmgr_drv_ops apbuart_ops =
{
	.init = {apbuart_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = APBUART_INFO_FUNC
};

static struct amba_dev_id apbuart_ids[] =
{
	{VENDOR_GAISLER, GAISLER_APBUART},
	{0, 0}		/* Mark end of table */
};

static struct amba_drv_info apbuart_drv_info =
{
	{
		DRVMGR_OBJ_DRV,				/* Driver */
		NULL,					/* Next driver */
		NULL,					/* Device list */
		DRIVER_AMBAPP_GAISLER_APBUART_ID,	/* Driver ID */
		"APBUART_DRV",				/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,			/* Bus Type */
		&apbuart_ops,
		NULL,					/* Funcs */
		0,					/* No devices yet */
		sizeof(struct apbuart_priv),		/*DrvMgr alloc private*/
	},
	&apbuart_ids[0]
};

void apbuart_cons_register_drv (void)
{
	DBG("Registering APBUART Console driver\n");
	drvmgr_drv_register(&apbuart_drv_info.general);
}

/* Interrupt mode routines */
static const rtems_termios_callbacks Callbacks_intr = {
    apbuart_firstOpen,           /* firstOpen */
    apbuart_lastClose,           /* lastClose */
    NULL,                        /* pollRead */
    apbuart_write_intr,          /* write */
    apbuart_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_IRQ_DRIVEN           /* outputUsesInterrupts */
};

/* Polling mode routines */
static const rtems_termios_callbacks Callbacks_task = {
    apbuart_firstOpen,           /* firstOpen */
    apbuart_lastClose,           /* lastClose */
    apbuart_pollRead_task,       /* pollRead */
    apbuart_write_intr,          /* write */
    apbuart_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_TASK_DRIVEN          /* outputUsesInterrupts */
};

/* Polling mode routines */
static const rtems_termios_callbacks Callbacks_poll = {
    apbuart_firstOpen,           /* firstOpen */
    apbuart_lastClose,           /* lastClose */
    apbuart_pollRead,            /* pollRead */
    apbuart_write_polled,        /* write */
    apbuart_set_attributes,      /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    TERMIOS_POLLED               /* outputUsesInterrupts */
};

int apbuart_init1(struct drvmgr_dev *dev)
{
	struct apbuart_priv *priv;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;
	char prefix[32];
	unsigned int db;
	static int first_uart = 1;

	/* The default operation in AMP is to use APBUART[0] for CPU[0],
	 * APBUART[1] for CPU[1] and so on. The remaining UARTs is not used
	 * since we don't know how many CPU-cores there are. Note this only
	 * affects the on-chip amba bus (the root bus). The user can override
	 * the default resource sharing by defining driver resources for the
	 * APBUART devices on each AMP OS instance.
	 */
#if defined(RTEMS_MULTIPROCESSING) && defined(LEON3)
	if (drvmgr_on_rootbus(dev) && dev->minor_drv != LEON3_Cpu_Index &&
	    drvmgr_keys_get(dev, NULL) != 0) {
		/* User hasn't configured on-chip APBUART, leave it untouched */
		return DRVMGR_EBUSY;
	}
#endif

	DBG("APBUART[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	/* Private data was allocated and zeroed by driver manager */
	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if (ambadev == NULL)
		return -1;
	pnpinfo = &ambadev->info;
	priv->regs = (struct apbuart_regs *)pnpinfo->apb_slv->start;

	/* Clear HW regs, leave baudrate register as it is */
	priv->regs->status = 0;

	/* leave Transmitter/receiver if this is the RTEMS debug UART (assume
	 * it has been setup by boot loader).
	 */
	db = 0;
#ifdef LEON3
	if (priv->regs == dbg_uart) {
		db = priv->regs->ctrl & (LEON_REG_UART_CTRL_RE |
					LEON_REG_UART_CTRL_TE |
					LEON_REG_UART_CTRL_PE |
					LEON_REG_UART_CTRL_PS);
	}
#endif
	/* Let UART debug tunnelling be untouched if Flow-control is set.
	 *
	 * With old APBUARTs debug is enabled by setting LB and FL, since LB or
	 * DB are not reset we can not trust them. However since FL is reset we
	 * guess that we are debugging if FL is already set, the debugger set
	 * either LB or DB depending on UART capabilities.
	 */
	if (priv->regs->ctrl & LEON_REG_UART_CTRL_FL) {
		db |= priv->regs->ctrl & (LEON_REG_UART_CTRL_DB |
		      LEON_REG_UART_CTRL_LB | LEON_REG_UART_CTRL_FL);
	}

	priv->regs->ctrl = db;

	/* The system console and Debug console may depend on this device, so
	 * initialize it straight away.
	 *
	 * We default to have System Console on first APBUART, user may override
	 * this behaviour by setting the syscon option to 0.
	 */
	if (drvmgr_on_rootbus(dev) && first_uart) {
		priv->condev.flags = CONSOLE_FLAG_SYSCON;
		first_uart = 0;
	} else {
		priv->condev.flags = 0;
	}

	value = drvmgr_dev_key_get(priv->dev, "syscon", DRVMGR_KT_INT);
	if (value) {
		if (value->i)
			priv->condev.flags |= CONSOLE_FLAG_SYSCON;
		else
			priv->condev.flags &= ~CONSOLE_FLAG_SYSCON;
	}

	priv->condev.fsname = NULL;
	priv->condev.ops.get_uart_attrs = apbuart_get_attributes;

	/* Select 0=Polled, 1=IRQ, 2=Task-Driven UART Mode */
	value = drvmgr_dev_key_get(priv->dev, "mode", DRVMGR_KT_INT);
	if (value)
		priv->mode = value->i;
	else
		priv->mode = TERMIOS_POLLED;
	if (priv->mode == TERMIOS_IRQ_DRIVEN) {
		priv->condev.callbacks = &Callbacks_intr;
	} else if (priv->mode == TERMIOS_TASK_DRIVEN) {
		priv->condev.callbacks = &Callbacks_task;
	} else {
		priv->condev.callbacks = &Callbacks_poll;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if (drvmgr_get_dev_prefix(dev, prefix)) {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sapbuart%d", prefix, dev->minor_bus);
		priv->condev.fsname = priv->devName;
	}

	/* Register it as a console device, the console driver will register
	 * a termios device as well
	 */
	console_dev_register(&priv->condev);

	return DRVMGR_OK;
}

#ifdef APBUART_INFO_AVAIL
static int apbuart_info(
	struct drvmgr_dev *dev,
	void (*print_line)(void *p, char *str),
	void *p, int argc, char *argv[])
{
	struct apbuart_priv *priv = dev->priv;
	char *str1;
	char buf[64];

	if (dev->priv == NULL)
		return -DRVMGR_EINVAL;

	if (priv->mode == TERMIOS_POLLED)
		str1 = "TERMIOS_POLLED";
	else if (priv->mode == TERMIOS_TASK_DRIVEN)
		str1 = "TERMIOS_TASK_DRIVEN";
	else if (priv->mode == TERMIOS_TASK_DRIVEN)
		str1 = "TERMIOS_TASK_DRIVEN";
	else
		str1 = "BAD MODE";

	sprintf(buf, "UART Mode:   %s", str1);
	print_line(p, buf);
	if (priv->condev.fsname) {
		sprintf(buf, "FS Name:     %s", priv->condev.fsname);
		print_line(p, buf);
	}
	sprintf(buf, "STATUS REG:  0x%x", priv->regs->status);
	print_line(p, buf);
	sprintf(buf, "CTRL REG:    0x%x", priv->regs->ctrl);
	print_line(p, buf);
	sprintf(buf, "SCALER REG:  0x%x  baud rate %d",
				priv->regs->scaler, apbuart_get_baud(priv));
	print_line(p, buf);

	return DRVMGR_OK;
}
#endif

#ifndef LEON3
/* This routine transmits a character, it will busy-wait until on character
 * fits in the APBUART Transmit FIFO
 */
void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent)
{
send:
	while ((regs->status & LEON_REG_UART_STATUS_THE) == 0) {
		/* Lower bus utilization while waiting for UART */
		asm volatile ("nop"::);	asm volatile ("nop"::);
		asm volatile ("nop"::);	asm volatile ("nop"::);
		asm volatile ("nop"::);	asm volatile ("nop"::);
		asm volatile ("nop"::);	asm volatile ("nop"::);
	}
	regs->data = (unsigned int) ch;

	if ((ch == '\n') && do_cr_on_newline) {
		ch = '\r';
		goto send;
	}

	/* Wait until the character has been sent? */
	if (wait_sent) {
		while ((regs->status & LEON_REG_UART_STATUS_THE) == 0)
			;
	}
}

/* This routine polls for one character, return EOF if no character is available */
int apbuart_inbyte_nonblocking(struct apbuart_regs *regs)
{
	if (regs->status & LEON_REG_UART_STATUS_ERR) {
		regs->status = ~LEON_REG_UART_STATUS_ERR;
	}

	if ((regs->status & LEON_REG_UART_STATUS_DR) == 0)
		return EOF;

	return (int)regs->data;
}
#endif

int apbuart_firstOpen(int major, int minor, void *arg)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;
	rtems_libio_open_close_args_t *ioarg = arg;

	if ( ioarg && ioarg->iop )
		uart->cookie = ioarg->iop->data1;
	else
		uart->cookie = NULL;

	/* Enable TX/RX */
	uart->regs->ctrl |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;

	if (uart->mode != TERMIOS_POLLED) {
		/* Register interrupt and enable it */
		drvmgr_interrupt_register(uart->dev, 0, "apbuart",
						apbuart_isr, uart);

		uart->sending = 0;
		/* Turn on RX interrupts */
		uart->regs->ctrl |= LEON_REG_UART_CTRL_RI;
	}

	return 0;
}

int apbuart_lastClose(int major, int minor, void *arg)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;

	if (uart->mode != TERMIOS_POLLED) {
		/* Turn off RX interrupts */
		uart->regs->ctrl &= ~(LEON_REG_UART_CTRL_RI);

		/**** Flush device ****/
		while (uart->sending) {
			/* Wait until all data has been sent */
		}

		/* Disable and unregister interrupt handler */
		drvmgr_interrupt_unregister(uart->dev, 0, apbuart_isr, uart);
	}

#ifdef LEON3
	/* Disable TX/RX if not used for DEBUG */
	if (uart->regs != dbg_uart)
		uart->regs->ctrl &= ~(LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE);
#endif

	return 0;
}

int apbuart_pollRead(int minor)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;

	return apbuart_inbyte_nonblocking(uart->regs);
}

int apbuart_pollRead_task(int minor)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;
	int c, tot;
	char buf[32];

	tot = 0;
	while ((c=apbuart_inbyte_nonblocking(uart->regs)) != EOF) {
		buf[tot] = c;
		tot++;
		if (tot > 31) {
			rtems_termios_enqueue_raw_characters(uart->cookie, buf, tot);
			tot = 0;
		}
	}
	if (tot > 0)
		rtems_termios_enqueue_raw_characters(uart->cookie, buf, tot);

	return EOF;
}

struct apbuart_baud {
	unsigned int num;
	unsigned int baud;
};
static struct apbuart_baud apbuart_baud_table[] = {
	{B50, 50},
	{B75, 75},
	{B110, 110},
	{B134, 134},
	{B150, 150},
	{B200, 200},
	{B300, 300},
	{B600, 600},
	{B1200, 1200},
	{B1800, 1800},
	{B2400, 2400},
	{B4800, 4800},
	{B9600, 9600},
	{B19200, 19200},
	{B38400, 38400},
	{B57600, 57600},
	{B115200, 115200},
	{B230400, 230400},
	{B460800, 460800},
};
#define BAUD_NUM (sizeof(apbuart_baud_table)/sizeof(struct apbuart_baud))

static int apbuart_baud_num2baud(unsigned int num)
{
	int i;

	for(i=0; i<BAUD_NUM; i++)
		if (apbuart_baud_table[i].num == num)
			return apbuart_baud_table[i].baud;
	return -1;
}

static struct apbuart_baud *apbuart_baud_find_closest(unsigned int baud)
{
	int i, diff;

	for(i=0; i<BAUD_NUM-1; i++) {
		diff = apbuart_baud_table[i+1].baud -
			apbuart_baud_table[i].baud;
		if (baud < (apbuart_baud_table[i].baud + diff/2))
			return &apbuart_baud_table[i];
	}
	return &apbuart_baud_table[BAUD_NUM-1];
}

int apbuart_get_baud(struct apbuart_priv *uart)
{
	unsigned int core_clk_hz;
	unsigned int scaler;

	/* Get current scaler setting */
	scaler = uart->regs->scaler;

	/* Get APBUART core frequency */
	drvmgr_freq_get(uart->dev, DEV_APB_SLV, &core_clk_hz);

	/* Calculate baud rate from generator "scaler" number */
	return core_clk_hz / ((scaler + 1) * 8);
}

static struct apbuart_baud *apbuart_get_baud_closest(struct apbuart_priv *uart)
{
	return apbuart_baud_find_closest(apbuart_get_baud(uart));
}

int apbuart_set_attributes(int minor, const struct termios *t)
{
	unsigned int core_clk_hz;
	unsigned int scaler;
	unsigned int ctrl;
	int baud;
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;

	switch(t->c_cflag & CSIZE) {
		default:
		case CS5:
		case CS6:
		case CS7:
			/* Hardware doesn't support other than CS8 */
			return -1;
		case CS8:
			break;
	}

	/* Read out current value */
	ctrl = uart->regs->ctrl;

	switch(t->c_cflag & (PARENB|PARODD)){
		case (PARENB|PARODD):
			/* Odd parity */
			ctrl |= LEON_REG_UART_CTRL_PE|LEON_REG_UART_CTRL_PS;
			break;

		case PARENB:
			/* Even parity */
			ctrl &= ~LEON_REG_UART_CTRL_PS;
			ctrl |= LEON_REG_UART_CTRL_PE;
			break;

		default:
		case 0:
		case PARODD:
			/* No Parity */
			ctrl &= ~(LEON_REG_UART_CTRL_PS|LEON_REG_UART_CTRL_PE);
	}

	if (!(t->c_cflag & CLOCAL))
		ctrl |= LEON_REG_UART_CTRL_FL;
	else
		ctrl &= ~LEON_REG_UART_CTRL_FL;

	/* Update new settings */
	uart->regs->ctrl = ctrl;

	/* Baud rate */
  baud = apbuart_baud_num2baud(t->c_ospeed);
	if (baud > 0){
		/* Get APBUART core frequency */
		drvmgr_freq_get(uart->dev, DEV_APB_SLV, &core_clk_hz);

		/* Calculate Baud rate generator "scaler" number */
		scaler = (((core_clk_hz*10)/(baud*8))-5)/10;

		/* Set new baud rate by setting scaler */
		uart->regs->scaler = scaler;
	}

	return 0;
}

void apbuart_get_attributes(struct console_dev *condev, struct termios *t)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)condev;
	unsigned int ctrl;
	struct apbuart_baud *baud;

  t->c_cflag = t->c_cflag & ~(CSIZE|PARENB|PARODD|CLOCAL);

	/* Hardware support only CS8 */
	t->c_cflag |= CS8;

	/* Read out current parity */
	ctrl = uart->regs->ctrl;
	if (ctrl & LEON_REG_UART_CTRL_PE) {
		if (ctrl & LEON_REG_UART_CTRL_PS)
			t->c_cflag |= PARENB|PARODD; /* Odd parity */
		else
			t->c_cflag |= PARENB; /* Even parity */
	}

	if ((ctrl & LEON_REG_UART_CTRL_FL) == 0)
		t->c_cflag |= CLOCAL;

	baud = apbuart_get_baud_closest(uart);
	t->c_cflag |= baud->num;
}

ssize_t apbuart_write_polled(int minor, const char *buf, size_t len)
{
	int nwrite = 0;
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;

	while (nwrite < len) {
		apbuart_outbyte_polled(uart->regs, *buf++, 0, 0);
		nwrite++;
	}
	return nwrite;
}

ssize_t apbuart_write_intr(int minor, const char *buf, size_t len)
{
	struct apbuart_priv *uart = (struct apbuart_priv *)minor;
	unsigned int oldLevel;
	unsigned int ctrl;

	rtems_interrupt_disable(oldLevel);

	/* Enable TX interrupt */
	ctrl = uart->regs->ctrl;
	uart->regs->ctrl = ctrl | LEON_REG_UART_CTRL_TI;

	if (ctrl & LEON_REG_UART_CTRL_FA) {
		/* APBUART with FIFO.. Fill as many as FIFO allows */
		uart->sending = 0;
		while (((uart->regs->status & LEON_REG_UART_STATUS_TF) == 0) &&
		       (uart->sending < len)) {
			uart->regs->data = *buf;
			buf++;
			uart->sending++;
		}
	} else {
		/* start UART TX, this will result in an interrupt when done */
		uart->regs->data = *buf;

		uart->sending = 1;
	}

	rtems_interrupt_enable(oldLevel);

	return 0;
}

/* Handle UART interrupts */
void apbuart_isr(void *arg)
{
	struct apbuart_priv *uart = arg;
	unsigned int status;
	char data;
	int cnt;

	/* Get all received characters */
	if (uart->mode == TERMIOS_TASK_DRIVEN) {
		if ((status=uart->regs->status) & LEON_REG_UART_STATUS_DR)
			rtems_termios_rxirq_occured(uart->cookie);
	} else {
		while ((status=uart->regs->status) & LEON_REG_UART_STATUS_DR) {
			/* Data has arrived, get new data */
			data = uart->regs->data;

			/* Tell termios layer about new character */
			rtems_termios_enqueue_raw_characters(uart->cookie, &data, 1);
		}
	}

	if (uart->sending && (status & LEON_REG_UART_STATUS_THE)) {
		/* Sent the one char, we disable TX interrupts */
		uart->regs->ctrl &= ~LEON_REG_UART_CTRL_TI;

		/* Tell close that we sent everything */
		cnt = uart->sending;
		uart->sending = 0;

		/* apbuart_write_intr() will get called from this function */
		rtems_termios_dequeue_characters(uart->cookie, cnt);
	}
}
