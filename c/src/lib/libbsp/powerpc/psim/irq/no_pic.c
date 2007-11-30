/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Dummy support for just the decrementer interrupt but no PIC.
 *
 *  T. Straumann, 2007/11/30
 *
 *  irq.c,v 1.4.2.8 2003/09/04 18:45:20 joel Exp
 */
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq_supp.h>
#include <libcpu/raw_exception.h>

static rtems_irq_connect_data *rtems_hdl_tbl;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (struct _BSP_Exception_frame *frame, unsigned int excNum)
{
	register unsigned msr;
	register unsigned new_msr;

	if (excNum == ASM_DEC_VECTOR) {
		_CPU_MSR_GET(msr);
		new_msr = msr | MSR_EE;
		_CPU_MSR_SET(new_msr);

		rtems_hdl_tbl[BSP_DECREMENTER].hdl(rtems_hdl_tbl[BSP_DECREMENTER].handle);

		_CPU_MSR_SET(msr);
		return;

	}
}

void
BSP_enable_irq_at_pic(const rtems_irq_number irq)
{
}

void
BSP_disable_irq_at_pic(const rtems_irq_number irq)
{
}

int
BSP_setup_the_pic(rtems_irq_global_settings *config)
{
	rtems_hdl_tbl = config->irqHdlTbl;
	return 1;
}
