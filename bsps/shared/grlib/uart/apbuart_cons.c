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
#include <stdlib.h>
#include <assert.h>
#include <rtems/bspIo.h>
#include <string.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/apbuart.h>
#include <grlib/ambapp.h>
#include <grlib/grlib.h>
#include <grlib/cons.h>
#include <rtems/termiostypes.h>
#include <grlib/apbuart_cons.h>

/*#define DEBUG 1  */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

/* LEON3 Low level transmit/receive functions provided by debug-uart code */
#ifdef LEON3
extern struct apbuart_regs *leon3_debug_uart; /* The debug UART */
#endif

/* Probed hardware capabilities */
enum {
	CAP_FIFO = 0x01, /* FIFO available */
	CAP_DI   = 0x02, /* RX delayed interrupt available */
};
struct apbuart_priv {
	struct console_dev condev;
	struct drvmgr_dev *dev;
	struct apbuart_regs *regs;
	struct rtems_termios_tty *tty;
	char devName[32];
	volatile int sending;
	int mode;
	int cap;
};

/* Getters for different interfaces. It happens to be just casting which we do
 * in one place to avoid getting cast away. */
static struct console_dev *base_get_condev(rtems_termios_device_context *base)
{
	return (struct console_dev *) base;
}

static struct apbuart_priv *condev_get_priv(struct console_dev *condev)
{
	return (struct apbuart_priv *) condev;
}

static struct apbuart_priv *base_get_priv(rtems_termios_device_context *base)
{
	return condev_get_priv(base_get_condev(base));
}

/* TERMIOS Layer Callback functions */
static bool first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
);
static void last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
);
static void write_interrupt(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
);
static bool set_attributes(
	rtems_termios_device_context *base,
	const struct termios *t
);
static void get_attributes(
	rtems_termios_device_context *base,
	struct termios *t
);
static int read_polled(rtems_termios_device_context *base);
static int read_task(rtems_termios_device_context *base);
static void write_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
);

static void apbuart_cons_isr(void *arg);
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

static const rtems_termios_device_handler handler_interrupt = {
	.first_open     = first_open,
	.last_close     = last_close,
	.write          = write_interrupt,
	.set_attributes = set_attributes,
	.mode           = TERMIOS_IRQ_DRIVEN
};

static const rtems_termios_device_handler handler_task = {
	.first_open     = first_open,
	.last_close     = last_close,
	.poll_read      = read_task,
	.write          = write_interrupt,
	.set_attributes = set_attributes,
	.mode           = TERMIOS_TASK_DRIVEN
};

static const rtems_termios_device_handler handler_polled = {
	.first_open     = first_open,
	.last_close     = last_close,
	.poll_read      = read_polled,
	.write          = write_polled,
	.set_attributes = set_attributes,
	.mode           = TERMIOS_POLLED
};

/*
 * APBUART hardware instantiation is flexible. Probe features here and driver
 * can select appropriate routines for the hardware. probecap() return value
 * is a CAP_ bitmask.
 */
static int probecap(struct apbuart_regs *regs)
{
	int cap = 0;

	/* Probe FIFO */
	if (regs->ctrl & APBUART_CTRL_FA) {
		cap |= CAP_FIFO;

		/* Probe RX delayed interrupt */
		regs->ctrl |= APBUART_CTRL_DI;
		if (regs->ctrl & APBUART_CTRL_DI) {
			regs->ctrl &= ~APBUART_CTRL_DI;
			cap |= CAP_DI;
		}
	}

	return cap;
}

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
	if (priv->regs == leon3_debug_uart) {
		db = priv->regs->ctrl & (APBUART_CTRL_RE |
					APBUART_CTRL_TE |
					APBUART_CTRL_PE |
					APBUART_CTRL_PS);
	}
#endif
	/* Let UART debug tunnelling be untouched if Flow-control is set.
	 *
	 * With old APBUARTs debug is enabled by setting LB and FL, since LB or
	 * DB are not reset we can not trust them. However since FL is reset we
	 * guess that we are debugging if FL is already set, the debugger set
	 * either LB or DB depending on UART capabilities.
	 */
	if (priv->regs->ctrl & APBUART_CTRL_FL) {
		db |= priv->regs->ctrl & (APBUART_CTRL_DB |
		      APBUART_CTRL_LB | APBUART_CTRL_FL);
	}

	priv->regs->ctrl = db;

	priv->cap = probecap(priv->regs);

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

	/* Select 0=Polled, 1=IRQ, 2=Task-Driven UART Mode */
	value = drvmgr_dev_key_get(priv->dev, "mode", DRVMGR_KT_INT);
	if (value)
		priv->mode = value->i;
	else
		priv->mode = TERMIOS_POLLED;
	/* TERMIOS device handlers */
	if (priv->mode == TERMIOS_IRQ_DRIVEN) {
		priv->condev.handler = &handler_interrupt;
	} else if (priv->mode == TERMIOS_TASK_DRIVEN) {
		priv->condev.handler = &handler_task;
	} else {
		priv->condev.handler = &handler_polled;
	}

	priv->condev.fsname = NULL;
	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if (drvmgr_get_dev_prefix(dev, prefix)) {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sapbuart%d", prefix, dev->minor_bus);
		priv->condev.fsname = priv->devName;
	} else {
		sprintf(priv->devName, "/dev/apbuart%d", dev->minor_drv);
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
	else if (priv->mode == TERMIOS_IRQ_DRIVEN)
		str1 = "TERMIOS_IRQ_DRIVEN";
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

static bool first_open(
	rtems_termios_tty *tty,
	rtems_termios_device_context *base,
	struct termios *term,
	rtems_libio_open_close_args_t *args
)
{
	struct apbuart_priv *uart = base_get_priv(base);

	uart->tty = tty;

	/* Inherit UART hardware parameters from bootloader on system console */
	if (uart->condev.flags & CONSOLE_FLAG_SYSCON_GRANT) {
		get_attributes(base, term);
		term->c_oflag |= ONLCR;
		set_attributes(base, term);
	}

	/* Enable TX/RX */
	uart->regs->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;

	if (uart->mode != TERMIOS_POLLED) {
		int ret;
		uint32_t ctrl;

		/* Register interrupt and enable it */
		ret = drvmgr_interrupt_register(
			uart->dev, 0, uart->devName, apbuart_cons_isr, tty
		);
		if (ret) {
			return false;
		}

		uart->sending = 0;

		/* Turn on RX interrupts */
		ctrl = uart->regs->ctrl;
		ctrl |= APBUART_CTRL_RI;
		if (uart->cap & CAP_DI) {
			/* Use RX FIFO interrupt only if delayed interrupt available. */
			ctrl |= (APBUART_CTRL_DI | APBUART_CTRL_RF);
		}
		uart->regs->ctrl = ctrl;
	}

	return true;
}

static void last_close(
	rtems_termios_tty *tty,
	rtems_termios_device_context *base,
	rtems_libio_open_close_args_t *args
)
{
	struct apbuart_priv *uart = base_get_priv(base);
	rtems_interrupt_lock_context lock_context;

	if (uart->mode != TERMIOS_POLLED) {
		/* Turn off RX interrupts */
		rtems_termios_device_lock_acquire(base, &lock_context);
		uart->regs->ctrl &=
			~(APBUART_CTRL_DI | APBUART_CTRL_RI | APBUART_CTRL_RF);
		rtems_termios_device_lock_release(base, &lock_context);

		/**** Flush device ****/
		while (uart->sending) {
			/* Wait until all data has been sent */
		}
		while (
			(uart->regs->ctrl & APBUART_CTRL_TE) &&
			!(uart->regs->status & APBUART_STATUS_TS)
		) {
			/* Wait until all data has left shift register */
		}

		/* Disable and unregister interrupt handler */
		drvmgr_interrupt_unregister(uart->dev, 0, apbuart_cons_isr, tty);
	}

#ifdef LEON3
	/* Disable TX/RX if not used for DEBUG */
	if (uart->regs != leon3_debug_uart)
		uart->regs->ctrl &= ~(APBUART_CTRL_RE | APBUART_CTRL_TE);
#endif
}

static int read_polled(rtems_termios_device_context *base)
{
	struct apbuart_priv *uart = base_get_priv(base);

	return apbuart_inbyte_nonblocking(uart->regs);
}

/* This function is called from TERMIOS rxdaemon task without device lock. */
static int read_task(rtems_termios_device_context *base)
{
	rtems_interrupt_lock_context lock_context;
	struct apbuart_priv *uart = base_get_priv(base);
	struct apbuart_regs *regs = uart->regs;
	int cnt;
	char buf[33];
	struct rtems_termios_tty *tty;
	uint32_t ctrl_add;

	ctrl_add = APBUART_CTRL_RI;
	if (uart->cap & CAP_DI) {
		ctrl_add |= (APBUART_CTRL_DI | APBUART_CTRL_RF);
	}
	tty = uart->tty;
	do {
		cnt = 0;
		while (
			(regs->status & APBUART_STATUS_DR) &&
			(cnt < sizeof(buf))
		) {
			buf[cnt] = regs->data;
			cnt++;
		}
		if (0 < cnt) {
			/* Tell termios layer about new characters */
			rtems_termios_enqueue_raw_characters(tty, &buf[0], cnt);
		}

		/*
		 * Turn on RX interrupts. A new character in FIFO now may not
		 * cause interrupt so we must check data ready again
		 * afterwards.
		 */
		rtems_termios_device_lock_acquire(base, &lock_context);
		regs->ctrl |= ctrl_add;
		rtems_termios_device_lock_release(base, &lock_context);
	} while (regs->status & APBUART_STATUS_DR);

	return EOF;
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

static bool set_attributes(
	rtems_termios_device_context *base,
	const struct termios *t
)
{
	unsigned int core_clk_hz;
	unsigned int scaler;
	unsigned int ctrl;
	int baud;
	struct apbuart_priv *uart = base_get_priv(base);
	rtems_interrupt_lock_context lock_context;

	switch(t->c_cflag & CSIZE) {
		default:
		case CS5:
		case CS6:
		case CS7:
			/* Hardware doesn't support other than CS8 */
			return false;
		case CS8:
			break;
	}

	rtems_termios_device_lock_acquire(base, &lock_context);

	/* Read out current value */
	ctrl = uart->regs->ctrl;

	switch(t->c_cflag & (PARENB|PARODD)){
		case (PARENB|PARODD):
			/* Odd parity */
			ctrl |= APBUART_CTRL_PE|APBUART_CTRL_PS;
			break;

		case PARENB:
			/* Even parity */
			ctrl &= ~APBUART_CTRL_PS;
			ctrl |= APBUART_CTRL_PE;
			break;

		default:
		case 0:
		case PARODD:
			/* No Parity */
			ctrl &= ~(APBUART_CTRL_PS|APBUART_CTRL_PE);
	}

	if (!(t->c_cflag & CLOCAL))
		ctrl |= APBUART_CTRL_FL;
	else
		ctrl &= ~APBUART_CTRL_FL;

	/* Update new settings */
	uart->regs->ctrl = ctrl;

	rtems_termios_device_lock_release(base, &lock_context);

	/* Baud rate */
	baud = rtems_termios_baud_to_number(t->c_ospeed);
	if (baud > 0){
		/* Get APBUART core frequency */
		drvmgr_freq_get(uart->dev, DEV_APB_SLV, &core_clk_hz);

		/* Calculate Baud rate generator "scaler" number */
		scaler = (((core_clk_hz*10)/(baud*8))-5)/10;

		/* Set new baud rate by setting scaler */
		uart->regs->scaler = scaler;
	}

	return true;
}

static void get_attributes(
	rtems_termios_device_context *base,
	struct termios *t
)
{
	struct apbuart_priv *uart = base_get_priv(base);
	unsigned int ctrl;

	t->c_cflag = t->c_cflag & ~(CSIZE|PARENB|PARODD|CLOCAL);

	/* Hardware support only CS8 */
	t->c_cflag |= CS8;

	/* Read out current parity */
	ctrl = uart->regs->ctrl;
	if (ctrl & APBUART_CTRL_PE) {
		if (ctrl & APBUART_CTRL_PS)
			t->c_cflag |= PARENB|PARODD; /* Odd parity */
		else
			t->c_cflag |= PARENB; /* Even parity */
	}

	if ((ctrl & APBUART_CTRL_FL) == 0)
		t->c_cflag |= CLOCAL;

	rtems_termios_set_best_baud(t, apbuart_get_baud(uart));
}

static void write_polled(
	rtems_termios_device_context *base,
	const char *buf,
	size_t len
)
{
	struct apbuart_priv *uart = base_get_priv(base);
	int nwrite = 0;

	while (nwrite < len) {
		apbuart_outbyte_polled(uart->regs, *buf++, 0, 0);
		nwrite++;
	}
}

static void write_interrupt(
	rtems_termios_device_context *base,
	const char *buf,
	size_t len
)
{
	struct apbuart_priv *uart = base_get_priv(base);
	struct apbuart_regs *regs = uart->regs;
	int sending;
	unsigned int ctrl;

	ctrl = regs->ctrl;

	if (len > 0) {
		/*
		 * sending is used to remember how much we have outstanding so
		 * we can tell termios later.
		 */
		/* Enable TX interrupt (interrupt is edge-triggered) */
		regs->ctrl = ctrl | APBUART_CTRL_TI;

		if (ctrl & APBUART_CTRL_FA) {
			/* APBUART with FIFO.. Fill as many as FIFO allows */
			sending = 0;
			while (
				((regs->status & APBUART_STATUS_TF) == 0) &&
				(sending < len)
			) {
				regs->data = *buf;
				buf++;
				sending++;
			}
		} else {
			/* start UART TX, this will result in an interrupt when done */
			regs->data = *buf;

			sending = 1;
		}
	} else {
		/* No more to send, disable TX interrupts */
		regs->ctrl = ctrl & ~APBUART_CTRL_TI;

		/* Tell close that we sent everything */
		sending = 0;
	}

	uart->sending = sending;
}

/* Handle UART interrupts */
static void apbuart_cons_isr(void *arg)
{
	rtems_termios_tty *tty = arg;
	rtems_termios_device_context *base;
	struct console_dev *condev = rtems_termios_get_device_context(tty);
	struct apbuart_priv *uart = condev_get_priv(condev);
	struct apbuart_regs *regs = uart->regs;
	unsigned int status;
	char buf[33];
	int cnt;

	if (uart->mode == TERMIOS_TASK_DRIVEN) {
		if ((status = regs->status) & APBUART_STATUS_DR) {
			rtems_interrupt_lock_context lock_context;

			/* Turn off RX interrupts */
			base = rtems_termios_get_device_context(tty);
			rtems_termios_device_lock_acquire(base, &lock_context);
			regs->ctrl &=
			    ~(APBUART_CTRL_DI | APBUART_CTRL_RI |
			      APBUART_CTRL_RF);
			rtems_termios_device_lock_release(base, &lock_context);
			/* Activate termios RX daemon task */
			rtems_termios_rxirq_occured(tty);
		}
	} else {
		/*
		 * Get all new characters from APBUART RX (FIFO) and store them
		 * on the stack. Then tell termios about the new characters.
		 * Maximum APBUART RX FIFO size is 32 characters.
		 */
		cnt = 0;
		while (
			((status=regs->status) & APBUART_STATUS_DR) &&
			(cnt < sizeof(buf))
		) {
			buf[cnt] = regs->data;
			cnt++;
		}
		if (0 < cnt) {
			/* Tell termios layer about new characters */
			rtems_termios_enqueue_raw_characters(tty, &buf[0], cnt);
		}
	}

	if (uart->sending && (status & APBUART_STATUS_TE)) {
		/* Tell close that we sent everything */
		cnt = uart->sending;

		/*
		 * Tell termios how much we have sent. dequeue() may call
		 * write_interrupt() to refill the transmitter.
		 * write_interrupt() will eventually be called with 0 len to
		 * disable TX interrupts.
		 */
		rtems_termios_dequeue_characters(tty, cnt);
	}
}

