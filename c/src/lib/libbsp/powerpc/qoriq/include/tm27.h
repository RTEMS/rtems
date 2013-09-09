/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Support file for Timer Test 27.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
  #error "This is an RTEMS internal file you must not include directly."
#endif /* _RTEMS_TMTEST27 */

#ifndef TMTESTS_TM27_H
#define TMTESTS_TM27_H

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#define MUST_WAIT_FOR_INTERRUPT 1

static void (*tm27_interrupt_handler)(rtems_vector_number);

static int tm27_exception_handler( BSP_Exception_frame *frame, unsigned number)
{
	(*tm27_interrupt_handler)( 0);

	return 0;
}

void Install_tm27_vector( void (*handler)(rtems_vector_number))
{
  int rv = 0;

  tm27_interrupt_handler = handler;

  rv = ppc_exc_set_handler( ASM_DEC_VECTOR, tm27_exception_handler);
  if (rv < 0) {
    printk( "Error installing clock interrupt handler!\n");
  }
}

#define Cause_tm27_intr() \
  ppc_set_decrementer_register( 8)

#define Clear_tm27_intr() \
  ppc_set_decrementer_register( UINT32_MAX)

#define Lower_tm27_intr() \
  (void) ppc_external_exceptions_enable()

#endif /* TMTESTS_TM27_H */
