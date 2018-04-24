/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/vectors.h>

#ifdef PPC_EXC_CONFIG_USE_FIXED_HANDLER

#ifndef PPC_EXC_CONFIG_BOOKE_ONLY
static int ppc_exc_interrupt_dispatch(BSP_Exception_frame *f, unsigned vector)
{
  bsp_interrupt_dispatch(vector);

  return 0;
}
#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */

const ppc_exc_handler_t ppc_exc_handler_table [LAST_VALID_EXC + 1] = {
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
#ifndef PPC_EXC_CONFIG_BOOKE_ONLY
  ppc_exc_interrupt_dispatch,
#else /* PPC_EXC_CONFIG_BOOKE_ONLY */
  ppc_exc_handler_default,
#endif /* PPC_EXC_CONFIG_BOOKE_ONLY */
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default,
  ppc_exc_handler_default
};

#endif /* PPC_EXC_CONFIG_USE_FIXED_HANDLER */
