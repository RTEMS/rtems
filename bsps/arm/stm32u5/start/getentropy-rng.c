/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Implementation of an entropy source for STM32U5.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include <stm32u5/hal.h>

#include <rtems.h>
#include <rtems/sysinit.h>

#include <string.h>
#include <sys/param.h>
#include <unistd.h>

static RNG_HandleTypeDef stm32u5_rng_instance = {
  .Instance                 = RNG,
  .Init.ClockErrorDetection = RNG_CED_DISABLE
};

static void stm32u5_rng_enable( void )
{
  stm32u5_clk_enable( STM32U5_MODULE_RNG );
  HAL_RNG_Init( &stm32u5_rng_instance );
}

int getentropy( void *ptr, size_t n )
{
  while ( n > 0 ) {
    rtems_interrupt_level level;
    uint32_t              random;
    bool                  ok;
    size_t                copy;

    do {
      rtems_interrupt_disable( level );

      ok = ( RNG->SR & RNG_FLAG_DRDY ) != 0;
      if ( ok ) {
        random = RNG->DR;
      }

      rtems_interrupt_enable( level );
    } while ( !ok );

    copy  = MIN( sizeof( random ), n );
    ptr   = memcpy( ptr, &random, copy );
    n    -= copy;
    ptr  += copy;
  }

  return 0;
}

RTEMS_SYSINIT_ITEM(
  stm32u5_rng_enable,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
