/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Implementation of OctoSPI initialization
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

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/start.h>
#include <stm32u5/hal.h>

void BSP_START_TEXT_SECTION stm32u5_init_octospi( void )
{
  OSPI_HandleTypeDef hospi1 = {
    .Instance = OCTOSPI1,
    .Init     = {
          .FifoThreshold         = 2,
          .DualQuad              = HAL_OSPI_DUALQUAD_DISABLE,
          .MemoryType            = HAL_OSPI_MEMTYPE_HYPERBUS,
          .DeviceSize            = 24,
          .ChipSelectHighTime    = 1, /* min 6ns -> 1 works for up to 160MHz */
          .FreeRunningClock      = HAL_OSPI_FREERUNCLK_DISABLE,
          .ClockMode             = HAL_OSPI_CLOCK_MODE_0,
      /*
       * FIXME: Enable Hybrid burst in CR0 of the RAM and set to a sensible
       * WrapSize
       */
          .WrapSize              = HAL_OSPI_WRAP_NOT_SUPPORTED,
          .ClockPrescaler        = 2,
      /*
       * Reference Manual: The firmware must clear SSHIFT when the data phase is
       * configured in DTR mode (DDTR = 1).
       */
          .SampleShifting        = HAL_OSPI_SAMPLE_SHIFTING_NONE,
      /*
       * Reference Manual: In DTR mode, it is recommended to set DHQC of
       * OCTOSPI_TCR, to shift the outputs by a quarter of cycle and avoid
       * holding issues on the memory side.
       */
          .DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE,
          .ChipSelectBoundary    = 7,
          .DelayBlockBypass      = HAL_OSPI_DELAY_BLOCK_USED,
          .MaxTran               = 0,
          .Refresh               = 241, /* based on AN5050 */
    },
  };

  OSPIM_CfgTypeDef sOspiManagerCfg = {
    .ClkPort    = 1,
    .DQSPort    = 1,
    .NCSPort    = 1,
    .IOLowPort  = HAL_OSPIM_IOPORT_1_LOW,
    .IOHighPort = HAL_OSPIM_IOPORT_1_HIGH,
  };

  OSPI_HyperbusCfgTypeDef sHyperBusCfg = {
    /*
     * S70KL1282 in 166MHz version needs at least 36ns. At 100MHz that's 3.6
     * clock cycles.
     */
    .RWRecoveryTime   = 4,
    /*
     * FIXME: That's the Clock latency. We could set the RAM config register to
     * 4 clock latencies for 100 MHz.
     */
    .AccessTime       = 7,
    .WriteZeroLatency = HAL_OSPI_LATENCY_ON_WRITE,
    /* 128-Mb version of S70KL1282 only supports fixed latency */
    .LatencyMode      = HAL_OSPI_FIXED_LATENCY,
  };

  OSPI_HyperbusCmdTypeDef sCommand = {
    .AddressSpace = HAL_OSPI_MEMORY_ADDRESS_SPACE,
    .AddressSize  = HAL_OSPI_ADDRESS_32_BITS,
    .DQSMode      = HAL_OSPI_DQS_ENABLE,
    .Address      = 0,
    .NbData       = 1,
  };

  OSPI_MemoryMappedTypeDef sMemMappedCfg = {
    .TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE,
    .TimeOutPeriod     = 0x20,
  };

  HAL_OSPI_DLYB_CfgTypeDef HAL_OSPI_DLYB_Cfg_Struct = { 0 };

  if ( HAL_OSPIM_Config(
         &hospi1,
         &sOspiManagerCfg,
         HAL_OSPI_TIMEOUT_DEFAULT_VALUE
       ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }

  if ( HAL_OSPI_Init( &hospi1 ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }

  if ( HAL_OSPI_HyperbusCfg(
         &hospi1,
         &sHyperBusCfg,
         HAL_OSPI_TIMEOUT_DEFAULT_VALUE
       ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }

  /* Delay block tuning */
  if ( HAL_OSPI_DLYB_GetClockPeriod( &hospi1, &HAL_OSPI_DLYB_Cfg_Struct ) !=
       HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }
  /* From ST example: Set at one quarter. */
  HAL_OSPI_DLYB_Cfg_Struct.PhaseSel /= 4;
  if ( HAL_OSPI_DLYB_SetConfig( &hospi1, &HAL_OSPI_DLYB_Cfg_Struct ) !=
       HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }

  /* Memory-mapped mode configuration */
  if ( HAL_OSPI_HyperbusCmd(
         &hospi1,
         &sCommand,
         HAL_OSPI_TIMEOUT_DEFAULT_VALUE
       ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }

  /* OctoSPI activation of memory-mapped mode */
  if ( HAL_OSPI_MemoryMapped( &hospi1, &sMemMappedCfg ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_OSPI );
  }
}

void BSP_START_TEXT_SECTION HAL_OSPI_MspInit( OSPI_HandleTypeDef *hospi )
{
  GPIO_InitTypeDef         GPIO_InitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit   = { 0 };
  if ( hospi->Instance == OCTOSPI1 ) {
    /* Initializes the peripherals clock */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInit.OspiClockSelection   = RCC_OSPICLKSOURCE_PLL2;
    PeriphClkInit.PLL2.PLL2Source      = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLL2.PLL2M           = 3;
    PeriphClkInit.PLL2.PLL2N           = 12;
    PeriphClkInit.PLL2.PLL2P           = 2;
    PeriphClkInit.PLL2.PLL2Q           = 1;
    PeriphClkInit.PLL2.PLL2R           = 2;
    PeriphClkInit.PLL2.PLL2RGE         = RCC_PLLVCIRANGE_1;
    PeriphClkInit.PLL2.PLL2FRACN       = 4096.0;
    PeriphClkInit.PLL2.PLL2ClockOut    = RCC_PLL2_DIVQ;
    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit ) != HAL_OK ) {
      bsp_fatal( STM32U5_FATAL_OSPI );
    }

    /* Peripheral clock enable */
    __HAL_RCC_OSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* OCTOSPI1 GPIO Configuration */
    GPIO_InitStruct.Pin       = GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_OCTOSPI1;
    HAL_GPIO_Init( GPIOE, &GPIO_InitStruct );

    GPIO_InitStruct.Pin       = GPIO_PIN_0;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_OCTOSPI1;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );

    GPIO_InitStruct.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                          GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init( GPIOE, &GPIO_InitStruct );
  }
}
