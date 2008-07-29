/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Source file for generic BSP interrupt support legacy code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <stdbool.h>
#include <stdlib.h>

#include <rtems/irq.h>

#include <bsp/irq-generic.h>

typedef struct {
	rtems_irq_hdl handler;
	void *arg;
} bsp_interrupt_legacy_entry;

typedef struct {
	rtems_irq_hdl handler;
	void *handler_arg;
	bsp_interrupt_legacy_entry *legacy_handler_arg;
} bsp_interrupt_legacy_iterate_entry;

static void bsp_interrupt_legacy_dispatch( rtems_vector_number vector, void *arg)
{
	bsp_interrupt_legacy_entry *e = (bsp_interrupt_legacy_entry *) arg;
	e->handler( e->arg);
}

static void bsp_interrupt_legacy_per_handler_routine(
	void *arg,
	const char *info,
	rtems_option options,
	rtems_interrupt_handler handler,
	void *handler_arg
)
{
	bsp_interrupt_legacy_iterate_entry *ie = (bsp_interrupt_legacy_iterate_entry *) arg;
	bsp_interrupt_legacy_entry *e = NULL;

	if (handler == bsp_interrupt_legacy_dispatch) {
		e = (bsp_interrupt_legacy_entry *) handler_arg;
		if (e->handler == ie->handler && e->arg == ie->handler_arg) {
			ie->legacy_handler_arg = e;
		}
	}
}

/**
 * @deprecated Obsolete.
 */
int BSP_get_current_rtems_irq_handler( rtems_irq_connect_data *cd)
{
	cd->hdl = NULL;
	cd->handle = NULL;
	cd->on = NULL;
	cd->off = NULL;
	cd->isOn = NULL;

	return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_install() instead.
 */
int BSP_install_rtems_irq_handler( const rtems_irq_connect_data *cd)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	bsp_interrupt_legacy_entry *e = malloc( sizeof( bsp_interrupt_legacy_entry));

	if (e == NULL) {
		return 0;
	}

	e->handler = cd->hdl;
	e->arg = cd->handle;

	sc = rtems_interrupt_handler_install(
		cd->name,
		"Unique interrupt handler (installed with obsolete BSP_install_rtems_irq_handler())",
		RTEMS_INTERRUPT_UNIQUE,
		bsp_interrupt_legacy_dispatch,
		e
	);
	if (sc != RTEMS_SUCCESSFUL) {
		free( e);
		return 0;
	}

	if (cd->on != NULL) {
		cd->on( cd);
	}

	return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_install() instead.
 */
int BSP_install_rtems_shared_irq_handler( const rtems_irq_connect_data *cd)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	bsp_interrupt_legacy_entry *e = malloc( sizeof( bsp_interrupt_legacy_entry));

	if (e == NULL) {
		return 0;
	}

	e->handler = cd->hdl;
	e->arg = cd->handle;

	sc = rtems_interrupt_handler_install(
		cd->name,
		"Shared interrupt handler (installed with obsolete BSP_install_rtems_shared_irq_handler())",
		RTEMS_INTERRUPT_SHARED,
		bsp_interrupt_legacy_dispatch,
		e
	);
	if (sc != RTEMS_SUCCESSFUL) {
		free( e);
		return 0;
	}

	if (cd->on != NULL) {
		cd->on( cd);
	}

	return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_remove() instead.
 */
int BSP_remove_rtems_irq_handler( const rtems_irq_connect_data *cd)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	bsp_interrupt_legacy_iterate_entry e = {
		.handler = cd->hdl,
		.handler_arg = cd->handle,
		.legacy_handler_arg = NULL
	};

	sc = rtems_interrupt_handler_iterate( cd->name, bsp_interrupt_legacy_per_handler_routine, &e);
	if (sc != RTEMS_SUCCESSFUL) {
		return 0;
	}

	if (e.legacy_handler_arg == NULL) {
		return 0;
	}

	if (cd->off != NULL) {
		cd->off( cd);
	}

	sc = rtems_interrupt_handler_remove( cd->name, bsp_interrupt_legacy_dispatch, e.legacy_handler_arg);

	free( e.legacy_handler_arg);

	if (sc != RTEMS_SUCCESSFUL) {
		return 0;
	}

	return 1;
}

/**
 * @deprecated Use bsp_interrupt_initialize() instead.
 */
int BSP_rtems_irq_mngt_set( rtems_irq_global_settings *config)
{
	return 0;
}

/**
 * @deprecated Obsolete.
 */
int BSP_rtems_irq_mngt_get( rtems_irq_global_settings **config)
{
	*config = NULL;
	return 0;
}
