/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 * Stuff for Time Test 27
 *
 * The following require that IRQ7 be jumpered to ground.  On the SS555,
 * this can be done by shorting together CN5 pin 48 and CN5 pin 50.
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler )					\
{									\
  extern rtems_irq_connect_data tm27IrqData;                            \
  usiu.siel |= (1 << 17); 						\
  usiu.sipend |= (1 << 17); 						\
									\
  tm27IrqData.hdl = (rtems_irq_hdl)handler;				\
  BSP_install_rtems_irq_handler (&tm27IrqData);				\
}

#define Cause_tm27_intr()						\
{									\
  usiu.siel &= ~(1 << 17); 						\
}

#define Clear_tm27_intr()						\
{									\
  usiu.siel |= (1 << 17); 						\
  usiu.sipend |= (1 << 17); 						\
}

#define Lower_tm27_intr()						\
{									\
  ppc_cached_irq_mask |= (1 << 17);					\
  usiu.simask = ppc_cached_irq_mask;					\
}

#endif
