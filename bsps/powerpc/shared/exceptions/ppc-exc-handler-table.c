/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
