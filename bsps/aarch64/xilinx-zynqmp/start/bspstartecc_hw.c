/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the implementation of zynqmp_ecc_init().
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <bsp.h>
#include <bsp/ecc_priv.h>
#include <bsp/fatal.h>
#include <bsp/utility.h>

zynqmp_ecc_handler saved_handler = NULL;

void zynqmp_ecc_register_handler( zynqmp_ecc_handler handler )
{
  saved_handler = handler;
}

void zynqmp_invoke_ecc_handler( ECC_Event_Type event, void *data )
{
  if (saved_handler == NULL) {
    bsp_fatal( BSP_FATAL_MEMORY_ECC_ERROR );
  }

  saved_handler(event, data);
}

int zynqmp_ecc_enable( ECC_Event_Type event )
{
  rtems_status_code sc;

  switch (event) {
  case L1_CACHE:
  case L2_CACHE:
  case L1_L2_CACHE:
    sc = zynqmp_configure_cache_ecc();
    break;
  case OCM_RAM:
    sc = zynqmp_configure_ocm_ecc();
    break;
  case DDR_RAM:
    sc = zynqmp_configure_ddr_ecc();
    break;
  default:
    return 1;
  }

  if (sc != RTEMS_SUCCESSFUL) {
    return 1;
  }
  return 0;
}

void zynqmp_ecc_init( void )
{
  /* Do something on hardware */
  zynqmp_ecc_enable( L1_L2_CACHE );
  zynqmp_ecc_enable( OCM_RAM );

  /* Call BSP-specific init function */
  zynqmp_ecc_init_bsp();
}
