/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Implementation of the STM32U5 early start hooks.
 */

/*
 * Copyright (c) 2024 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/armv7m.h>
#include <rtems/score/isr.h>
#include <stm32u5/hal.h>

/*
 * We use some HAL stuff before the system tick is initialized. As a
 * workarround, a function is used that just counts calls.
 *
 * Usually HAL_GetTick would provide something in a millisecond scale. We run at
 * 160MHz. So we need 160000 CPU instructions per millisecond. Let's guess that
 * the HAL_GetTick is used for timeouts in loops reading a few registers in the
 * HAL. So maybe 10 instructions. So just return (calls / 2**14) as an estimate.
 */
uint32_t BSP_FAST_DATA_SECTION startup_delay_call_counter;
uint32_t BSP_FAST_DATA_SECTION ( *HAL_GetTick_ptr )( void );

static uint32_t BSP_START_TEXT_SECTION Startup_HAL_GetTick( void )
{
  ++startup_delay_call_counter;
  return startup_delay_call_counter >> 14;
}

static uint32_t BSP_START_TEXT_SECTION Booted_HAL_GetTick( void )
{
  return rtems_clock_get_ticks_since_boot() *
         rtems_configuration_get_milliseconds_per_tick();
}

/* Get number of milliseconds elapsed since startup */
uint32_t BSP_START_TEXT_SECTION HAL_GetTick( void )
{
  return HAL_GetTick_ptr();
}

void BSP_START_TEXT_SECTION stm32u5_rcc_power_clock_enable( void )
{
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddA();
}

void BSP_START_TEXT_SECTION stm32u5_init_power( void )
{
  HAL_PWREx_DisableUCPDDeadBattery();

  if ( HAL_PWREx_ConfigSupply( PWR_SMPS_SUPPLY ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_INIT_POWER );
  }
}

void BSP_START_TEXT_SECTION stm32u5_init_oscillator( void )
{
  HAL_StatusTypeDef status;

  /*
   * Voltage scaling range 1 is used when CPU Clock frequency is up to
   * 160 MHz. See STM32U5 Reference Manual (RM0456) section 10.5.4.
   */
  if ( HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 ) !=
       HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_INIT_CLOCKS );
  }

  status = HAL_RCC_OscConfig( &stm32u5_config_oscillator );
  if ( status != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_INIT_CLOCKS );
  }
}

void BSP_START_TEXT_SECTION stm32u5_init_clocks( void )
{
  HAL_StatusTypeDef status;

  /*
   * Flash latency must be configured according to the frequency of the CPU
   * Clock (HCLK). In high-performance range (voltage scaling range 1), with
   * a CPU clock of 160 MHz, 4 wait states are required (FLASH_LATENCY_4).
   * See STM32U5 Reference Manual (RM0456) section 7.3.3.
   */
  status = HAL_RCC_ClockConfig( &stm32u5_config_clocks, FLASH_LATENCY_4 );
  if ( status != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_INIT_CLOCKS );
  }
}

void BSP_START_TEXT_SECTION stm32u5_init_peripheral_clocks( void )
{
  HAL_StatusTypeDef status;

  status = HAL_RCCEx_PeriphCLKConfig( &stm32u5_config_peripheral_clocks );
  if ( status != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_INIT_CLOCKS );
  }
}

void BSP_START_TEXT_SECTION bsp_start_hook_0( void )
{
  HAL_GetTick_ptr            = Startup_HAL_GetTick;
  startup_delay_call_counter = 0;

  /*
   * If we are running from OctoSPI, we must not touch the clocks and pins.
   * Otherwise the OSPI RAM won't work any more.
   *
   * Doing this check here instead of using a compile time option has the
   * advantage, that only one BSP is necessary to compile (for example) a
   * bootloader running from Flash and an application that runs from OSPI RAM.
   * They would only use a different linker command file.
   *
   * It's not really relevant, which symbol is checked. So pick
   * stm32u5_init_octospi at random.
   */
  if ( (void*) stm32u5_init_octospi < (void*) stm32u5_memory_octospi_1_begin ||
       (void*) stm32u5_init_octospi > (void*) stm32u5_memory_octospi_1_end ) {
    SystemInit();
    SystemCoreClockUpdate();
    stm32u5_rcc_power_clock_enable();
    stm32u5_init_oscillator();
    stm32u5_init_clocks();
    stm32u5_init_power();
    stm32u5_init_peripheral_clocks();
  }
  HAL_Init();
}

void BSP_START_TEXT_SECTION bsp_start_hook_1( void )
{
  /*
   * Init OctoSPI only if we are not running from it. See start hook 0 for
   * details.
   */
  if ( (void*) stm32u5_init_octospi < (void*) stm32u5_memory_octospi_1_begin ||
       (void*) stm32u5_init_octospi > (void*) stm32u5_memory_octospi_1_end ) {
    stm32u5_init_octospi();
  }
  bsp_start_copy_sections();
  bsp_start_clear_bss();

  HAL_GetTick_ptr = Booted_HAL_GetTick;
}
