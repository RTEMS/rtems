/*  GPIOLIB interface implementation
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <grlib/gpiolib.h>

#include <grlib/grlib_impl.h>

struct gpiolib_port;

struct gpiolib_port {
	struct gpiolib_port	*next;
	int			minor;
	struct gpiolib_drv	*drv;
	void			*handle;

	int			open;
};

/* Root of GPIO Ports */
struct gpiolib_port *gpiolib_ports;

/* Number of GPIO ports registered */
static int port_nr;

/* 1 if libraray initialized */
static int gpiolib_initied = 0;

/* Insert a port first in ports list */
static void gpiolib_list_add(struct gpiolib_port *port)
{
	port->next = gpiolib_ports;
	gpiolib_ports = port;
}

static struct gpiolib_port *gpiolib_find(int minor)
{
	struct gpiolib_port *p;

	p = gpiolib_ports;
	while ( p && (p->minor != minor) ) {
		p = p->next;
	}
	return p;
}

static struct gpiolib_port *gpiolib_find_by_name(char *name)
{
	struct gpiolib_port *p;
	struct gpiolib_info info;
	int (*get_info)(void *, struct gpiolib_info *);

	p = gpiolib_ports;
	while ( p ) {
		get_info = p->drv->ops->get_info;
		if ( get_info && (get_info(p->handle, &info) == 0) ) {
			if ( strncmp(name, (char *)&info.devName[0], 64) == 0 ) {
				break;
			}
		}
		p = p->next;
	}
	return p;
}

int gpiolib_drv_register(struct gpiolib_drv *drv, void *handle)
{
	struct gpiolib_port *port;

	if ( !drv || !drv->ops )
		return -1;

	port = grlib_calloc(1, sizeof(*port));
	if ( port == NULL )
		return -1;

	port->handle = handle;
	port->minor = port_nr++;
	port->drv = drv;

	gpiolib_list_add(port);

	return 0;
}

void gpiolib_show(int port, void *handle)
{
	struct gpiolib_port *p;

	if ( port == -1 ) {
		p = gpiolib_ports;
		while (p != NULL) {
			if ( p->drv->ops->show )
				p->drv->ops->show(p->handle);
			p = p->next;
		}
	} else {
		if ( handle ) {
			p = handle;
		} else {
			p = gpiolib_find(port);
		}
		if ( p == NULL ) {
			printf("PORT %d NOT FOUND\n", port);
			return;
		}
		if ( p->drv->ops->show )
			p->drv->ops->show(p->handle);
	}
}

static void *gpiolib_open_internal(int port, char *devName)
{
	struct gpiolib_port *p;

	if ( gpiolib_initied == 0 )
		return NULL;

	/* Find */
	if ( port >= 0 ) {
		p = gpiolib_find(port);
	} else {
		p = gpiolib_find_by_name(devName);
	}
	if ( p == NULL )
		return NULL;

	if ( p->open )
		return NULL;

	p->open = 1;
	return p;
}

void *gpiolib_open(int port)
{
	return gpiolib_open_internal(port, NULL);
}

void *gpiolib_open_by_name(char *devName)
{
	return gpiolib_open_internal(-1, devName);
}

void gpiolib_close(void *handle)
{
	struct gpiolib_port *p = handle;

	if ( p && p->open ) {
		p->open = 0;
	}
}

int gpiolib_set_config(void *handle, struct gpiolib_config *cfg)
{
	struct gpiolib_port *port = handle;

	if ( !port || !cfg )
		return -1;

	if ( !port->drv->ops->config )
		return -1;

	return port->drv->ops->config(port->handle, cfg);
}

int gpiolib_set(void *handle, int dir, int outval)
{
	struct gpiolib_port *port = handle;

	if ( !port )
		return -1;

	if ( !port->drv->ops->set )
		return -1;

	return port->drv->ops->set(port->handle, dir, outval);	
}

int gpiolib_get(void *handle, int *inval)
{
	struct gpiolib_port *port = handle;

	if ( !port || !inval)
		return -1;

	if ( !port->drv->ops->get )
		return -1;

	return port->drv->ops->get(port->handle, inval);
}

/*** IRQ Functions ***/
int gpiolib_irq_register(void *handle, void *func, void *arg)
{
	struct gpiolib_port *port = handle;

	if ( !port )
		return -1;

	if ( !port->drv->ops->irq_register )
		return -1;

	return port->drv->ops->irq_register(port->handle, func, arg);
}

static int gpiolib_irq_opts(void *handle, unsigned int options)
{
	struct gpiolib_port *port = handle;

	if ( !port )
		return -1;

	if ( !port->drv->ops->irq_opts )
		return -1;

	return port->drv->ops->irq_opts(port->handle, options);
}

int gpiolib_irq_clear(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_CLEAR);
}

int gpiolib_irq_force(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_FORCE);
}

int gpiolib_irq_enable(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_ENABLE);
}

int gpiolib_irq_disable(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_DISABLE);
}

int gpiolib_irq_mask(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_MASK);
}

int gpiolib_irq_unmask(void *handle)
{
	return gpiolib_irq_opts(handle, GPIOLIB_IRQ_UNMASK);
}


/*** Initialization ***/
int gpiolib_initialize(void)
{
	if ( gpiolib_initied != 0 )
		return 0;

	/* Initialize Libarary */
	port_nr = 0;
	gpiolib_ports = 0;
	gpiolib_initied = 1;
	return 0;
}
