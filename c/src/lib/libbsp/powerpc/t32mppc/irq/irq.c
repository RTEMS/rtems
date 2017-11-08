/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}

void bsp_interrupt_dispatch(uintptr_t exception_number)
{
	if (exception_number == 10) {
		t32mppc_decrementer_dispatch();
	} else {
		bsp_interrupt_handler_default(0);
	}
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	return RTEMS_SUCCESSFUL;
}
