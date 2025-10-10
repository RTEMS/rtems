/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  CAN_MUX driver. Present in GR712RC.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bsp.h>
#include <rtems/bspIo.h> /* printk */

#include <grlib/canmux.h>
#include <grlib/ambapp.h>

#include <grlib/grlib_impl.h>

#ifndef GAISLER_CANMUX
#define GAISLER_CANMUX 0x081
#endif

#if !defined(CANMUX_DEVNAME) 
 #undef CANMUX_DEVNAME
 #define CANMUX_DEVNAME "/dev/canmux"
#endif

/* Enable debug output? */
/* #define DEBUG */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

#define BUSA_SELECT (1 << 0)
#define BUSB_SELECT (1 << 1)

struct canmux_priv {
	volatile unsigned int *muxreg;
	rtems_id devsem;
	int open;
};

static struct canmux_priv *priv;

static rtems_device_driver canmux_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver canmux_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver canmux_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver canmux_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver canmux_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver canmux_initialize(rtems_device_major_number major, rtems_device_minor_number unused, void *arg);


static rtems_device_driver canmux_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;

	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t*)arg;
	
	DBG("CAN_MUX:  IOCTL %d\n\r", ioarg->command);

	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
	case CANMUX_IOC_BUSA_SATCAN: *priv->muxreg &= ~BUSA_SELECT; break;
	case CANMUX_IOC_BUSA_OCCAN1: *priv->muxreg |= BUSA_SELECT;  break;
	case CANMUX_IOC_BUSB_SATCAN: *priv->muxreg &= ~BUSB_SELECT; break;
	case CANMUX_IOC_BUSB_OCCAN2: *priv->muxreg |= BUSB_SELECT; break;
	default: return RTEMS_NOT_DEFINED;
	}

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver canmux_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;

	rtems_libio_rw_args_t *rw_args=(rtems_libio_rw_args_t*)arg;
	
	rw_args->bytes_moved = 0;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver canmux_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;

	rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t*)arg;
	
	rw_args->bytes_moved = 0;
	
	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver canmux_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;
	(void) arg;

	DBG("CAN_MUX: Closing %d\n\r",minor);

	priv->open = 0;
	return RTEMS_SUCCESSFUL;
}


static rtems_device_driver canmux_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) major;
	(void) minor;
	(void) arg;

	DBG("CAN_MUX: Opening %d\n\r",minor);
	
	rtems_semaphore_obtain(priv->devsem,RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (priv->open) {
		rtems_semaphore_release(priv->devsem);
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */
	}
	priv->open = 1;
	rtems_semaphore_release(priv->devsem);

	DBG("CAN_MUX: Opening %d success\n\r",minor);

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver canmux_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	(void) arg;

	struct ambapp_apb_info d;
	char fs_name[20];
	rtems_status_code status;

	DBG("CAN_MUX: Initialize..\n\r");

	strcpy(fs_name, CANMUX_DEVNAME);
	
	/* Find core and initialize register pointer */
	if (!ambapp_find_apbslv(ambapp_plb(), VENDOR_GAISLER, GAISLER_CANMUX, &d)) {
		printk("CAN_MUX: Failed to find CAN_MUX core\n\r");
		return -1;
	}
	
	status = rtems_io_register_name(fs_name, major, minor);
	if (RTEMS_SUCCESSFUL != status)
		rtems_fatal_error_occurred(status);

	/* Create private structure */
	if ((priv = grlib_malloc(sizeof(*priv))) == NULL) {
		printk("CAN_MUX driver could not allocate memory for priv structure\n\r");
		return -1;
	}
	
	priv->muxreg = (unsigned int*)d.start;

	status = rtems_semaphore_create(
		rtems_build_name('M', 'd', 'v', '0'),
		1,
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
		RTEMS_NO_PRIORITY_CEILING,
		0, 
		&priv->devsem);
	if (status != RTEMS_SUCCESSFUL) {
		printk("CAN_MUX: Failed to create dev semaphore (%d)\n\r", status);
		free(priv);
		return RTEMS_UNSATISFIED;
	}
	
	priv->open = 0;

	return RTEMS_SUCCESSFUL;
}


#define CANMUX_DRIVER_TABLE_ENTRY { canmux_initialize, canmux_open, canmux_close, canmux_read, canmux_write, canmux_ioctl }

static rtems_driver_address_table canmux_driver = CANMUX_DRIVER_TABLE_ENTRY;

int canmux_register(void)
{
	rtems_status_code r;
	rtems_device_major_number m;

	DBG("CAN_MUX: canmux_register called\n\r");

	if ((r = rtems_io_register_driver(0, &canmux_driver, &m)) == RTEMS_SUCCESSFUL) {
		DBG("CAN_MUX driver successfully registered, major: %d\n\r", m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("CAN_MUX rtems_io_register_driver failed: RTEMS_TOO_MANY\n\r"); break;
		case RTEMS_INVALID_NUMBER:  
			printk("CAN_MUX rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n\r"); break;
		case RTEMS_RESOURCE_IN_USE:
			printk("CAN_MUX rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n\r"); break;
		default:
			printk("CAN_MUX rtems_io_register_driver failed\n\r");
		}
		return 1;
	}

	return 0;
}
