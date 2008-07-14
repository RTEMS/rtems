/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Source file for MPC55XX interrupt support.
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

#include <mpc55xx/regs.h>

#include <libcpu/raw_exception.h>

#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/ppc_exc_bspsupp.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

/**
 * @brief Returns the priority @a p of IRQ @a i from the INTC.
 */
rtems_status_code mpc55xx_intc_get_priority( int i, int *p)
{
	if (MPC55XX_IRQ_IS_VALID( i)) {
		*p = INTC.PSR [i].B.PRI;
		return RTEMS_SUCCESSFUL;
	} else {
		*p = MPC55XX_INTC_INVALID_PRIORITY;
		return RTEMS_INVALID_NUMBER;
	}	       
}

/**
 * @brief Sets the priority of IRQ @a i to @a p at the INTC.
 */
rtems_status_code mpc55xx_intc_set_priority( int i, int p)
{
	if (MPC55XX_IRQ_IS_VALID( i) && MPC55XX_INTC_IS_VALID_PRIORITY( p)) {
		INTC.PSR [i].B.PRI = p;
		if (INTC.PSR [i].B.PRI == p) {
			return RTEMS_SUCCESSFUL;
		} else {
			return RTEMS_IO_ERROR;
		}
	} else {
		return RTEMS_INVALID_NUMBER;
	}	       
}

/**
 * @brief Raises the software IRQ with number @a i.
 */
rtems_status_code mpc55xx_intc_raise_software_irq( int i)
{
	if (MPC55XX_IRQ_IS_SOFTWARE( i)) {
		INTC.SSCIR [i].B.SET = 1;
		return RTEMS_SUCCESSFUL;
	} else {
		return RTEMS_INVALID_NUMBER;
	}	       
}

/**
 * @brief Clears the software IRQ with number @a i.
 */
rtems_status_code mpc55xx_intc_clear_software_irq( int i)
{
	if (MPC55XX_IRQ_IS_SOFTWARE( i)) {
		INTC.SSCIR [i].B.CLR = 1;
		return RTEMS_SUCCESSFUL;
	} else {
		return RTEMS_INVALID_NUMBER;
	}	       
}

/**
 * @brief Installs interrupt handler and sets priority.
 */
rtems_status_code mpc55xx_interrupt_handler_install(
	rtems_vector_number vector,
	int priority,
	const char *info,
	rtems_option options,
	rtems_interrupt_handler handler,
	void *arg
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (MPC55XX_IRQ_IS_VALID( vector) && MPC55XX_INTC_IS_VALID_PRIORITY( priority)) {
		sc = rtems_interrupt_handler_install( vector, info, options, handler, arg);
		CHECK_SC( sc, "Install interrupt handler");
		return mpc55xx_intc_set_priority( vector, priority);
	} else {
		return RTEMS_INVALID_NUMBER;
	}
}

/**
 * @brief External exception handler.
 */
static int mpc55xx_external_exception_handler( BSP_Exception_frame *frame, unsigned exception_number)
{
	/* Acknowlege interrupt request */
	rtems_vector_number vector_number = INTC.IACKR.B.INTVEC;

	/* Save current interrupt level */
	uint32_t level = _ISR_Get_level();

	/* Enable all interrupts */
	_ISR_Set_level( 0);

	/* Dispatch interrupt handlers */
	bsp_interrupt_handler_dispatch( vector_number);

	/* Restore interrupt level */
	_ISR_Set_level( level);

	/* End of interrupt */
	INTC.EOIR.R = 1;

	return 0;
}

rtems_status_code bsp_interrupt_facility_initialize()
{
	/* Install exception handler */
	if (ppc_exc_set_handler( ASM_EXT_VECTOR, mpc55xx_external_exception_handler)) {
		return RTEMS_IO_ERROR;
	}

	/* Initialize interrupt controller */

	/* Software vector mode */
	INTC.MCR.B.VTES = 0;
	INTC.MCR.B.HVEN = 0;

	/* Set current priority to 0 */
	INTC.CPR.B.PRI = 0;

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_VALID( vector)) {
		return mpc55xx_intc_set_priority( vector, MPC55XX_INTC_DEFAULT_PRIORITY);
	} else {
		return RTEMS_SUCCESSFUL;
	}
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector)
{
	if (MPC55XX_IRQ_IS_VALID( vector)) {
		return mpc55xx_intc_set_priority( vector, MPC55XX_INTC_DISABLED_PRIORITY);
	} else {
		return RTEMS_SUCCESSFUL;
	}
}

void bsp_interrupt_handler_default( rtems_vector_number vector)
{
	/* Do nothing */
}
