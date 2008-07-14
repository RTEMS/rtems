/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief IRQ
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

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#include <rtems/irq-extension.h>
#include <rtems/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Interrupt numbers
 */

/* Basics */
#define MPC55XX_IRQ_MIN 0
#define MPC55XX_IRQ_MAX 328
#define MPC55XX_IRQ_BASE MPC55XX_IRQ_MIN
#define MPC55XX_IRQ_NUMBER (MPC55XX_IRQ_MAX + 1)

/* Software interrupts */
#define MPC55XX_IRQ_SOFTWARE_MIN 0
#define MPC55XX_IRQ_SOFTWARE_MAX 7
#define MPC55XX_IRQ_SOFTWARE_NUMBER (MPC55XX_IRQ_SOFTWARE_MAX + 1)

/* eDMA interrupts */
#define MPC55XX_IRQ_EDMA_ERROR_LOW 10
#define MPC55XX_IRQ_EDMA_ERROR_HIGH 210
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN 11
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MAX 42
#define MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN 211
#define MPC55XX_IRQ_EDMA_REQUEST_HIGH_MAX 242
#define MPC55XX_IRQ_EDMA_GET_CHANNEL( i) (((i) > MPC55XX_IRQ_EDMA_REQUEST_LOW_MAX) ? ((i) - MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN + 32) : ((i) - MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN))
#define MPC55XX_IRQ_EDMA_GET_REQUEST( c) (((c) > 31) ? ((c) + MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN - 32) : ((c) + MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN))

/* Checks */
#define MPC55XX_IRQ_IS_VALID(i) ((i) >= MPC55XX_IRQ_MIN && (i) <= MPC55XX_IRQ_MAX)
#define MPC55XX_IRQ_IS_SOFTWARE(i) ((i) >= MPC55XX_IRQ_SOFTWARE_MIN && (i) <= MPC55XX_IRQ_SOFTWARE_MAX)

/*
 * Interrupt controller
 */

#define MPC55XX_INTC_INVALID_PRIORITY -1
#define MPC55XX_INTC_DISABLED_PRIORITY 0
#define MPC55XX_INTC_MIN_PRIORITY 1
#define MPC55XX_INTC_MAX_PRIORITY 15
#define MPC55XX_INTC_DEFAULT_PRIORITY MPC55XX_INTC_MIN_PRIORITY
#define MPC55XX_INTC_IS_VALID_PRIORITY(p) ((p) >= MPC55XX_INTC_DISABLED_PRIORITY && (p) <= MPC55XX_INTC_MAX_PRIORITY)

rtems_status_code mpc55xx_interrupt_handler_install(
	rtems_vector_number vector,
	int priority,
	const char *info,
	rtems_option options,
	rtems_interrupt_handler handler,
	void *arg
);

rtems_status_code mpc55xx_intc_get_priority( int i, int *p);

rtems_status_code mpc55xx_intc_set_priority( int i, int p);

rtems_status_code mpc55xx_intc_raise_software_irq( int i);

rtems_status_code mpc55xx_intc_clear_software_irq( int i);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_IRQ_H */
