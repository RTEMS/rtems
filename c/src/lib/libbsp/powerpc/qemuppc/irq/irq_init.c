/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file integrates the IPIC irq controller                    |
\*===============================================================*/

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

int qemuppc_exception_handler( BSP_Exception_frame *frame, unsigned exception_number)
{
  BSP_panic("Unexpected interrupt occured");
  return 0;
}

/*
 * functions to enable/disable a source at the ipic
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	/* Install exception handler */
	if (ppc_exc_set_handler( ASM_EXT_VECTOR, qemuppc_exception_handler)) {
		return RTEMS_IO_ERROR;
	}

	return RTEMS_SUCCESSFUL;
}
