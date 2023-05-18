/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI GPIO definitions
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_MICROBLAZE_FPGA_MICROBLAZE_GPIO_H
#define LIBBSP_MICROBLAZE_FPGA_MICROBLAZE_GPIO_H

#include <bspopts.h>
#include <bsp/utility.h>
#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  /* Channel 1 data values */

  /*
   * Used to read general purpose input ports and write to general purpose
   * output ports from channel 1.
   */
  volatile uint32_t gpio_data;

  /*
   * The 3-state control register for channel 1 is used for the dynamic
   * configuration of ports as input or output. When a bit is set to 1, the
   * corresponding I/O port is an input port. When a bit is set to 0, it is an
   * output port.
   */
  volatile uint32_t gpio_tri;

  /* Channel 2 data values */

  /*
   * Used to read general purpose input ports and write to general purpose
   * output ports from channel 2.
   */
  volatile uint32_t gpio2_data;

  /*
   * The 3-state control register for channel 2 is used for the dynamic
   * configuration of ports as input or output. When a bit is set to 1, the
   * corresponding I/O port is an input port. When a bit is set to 0, it is an
   * output port.
   */
  volatile uint32_t gpio2_tri;

  char _unused[272];

  /* Only the 31st bit is used to enable interrupts globally */
#define GLOBAL_INTERRUPT_REGISTER_ENABLE BSP_BIT32(31)

  /*
   * Global Interrupt Enable Register
   *
   * Determines whether interrupts are enabled or disabled.
   *
   * 0 - Disabled
   * 1 - Enabled
   */
  volatile uint32_t gier;

  char _unused2[12];

  /* Used with ip_isr and ip_ier member variables */
#define CHANNEL_1_INTERRUPT_REGISTER BSP_BIT32(0)
#define CHANNEL_2_INTERRUPT_REGISTER BSP_BIT32(1)

  /*
   * IP Status Registers
   *
   * Contains the status bit for each channel.
   *
   * 0 - Disabled
   * 1 - Enabled
   */
  volatile uint32_t ip_isr;

  char _unused3[4];

  /*
   * IP Interrupt Enable Register
   *
   * Provides the ability to independtly control whether interrupts for each
   * channel are enabled or disabled.
   *
   * 0 - No Channel input interrupt
   * 1 - Channel input interrupt
   */
  volatile uint32_t ip_ier;
} Microblaze_GPIO_registers;

typedef struct {
  Microblaze_GPIO_registers *regs;
  bool                       is_dual;
  uint32_t                   irq;
  bool                       has_interrupts;
} Microblaze_GPIO_context;

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
/**
 * @brief Initialize GPIO context from FDT.
 *
 * @param[in] context the GPIO context to initialize
 * @param[in] index the zero-based GPIO index in the FDT
 *
 * @retval RTEMS_SUCCESSFUL on success
 * @retval RTEMS_INVALID_NUMBER if the index is invalid or the node is missing a
 *         required property
 * @retval RTEMS_INVALID_ADDRESS if the context is NULL
 */
rtems_status_code microblaze_gpio_init_context_from_fdt(
  Microblaze_GPIO_context *context,
  int index
);
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

/**
 * @brief Set pin configuration for the specified GPIO channel.
 *
 * Changes the pin configuration for a channel. Bits set to 0 are output, and
 * bits set to 1 are input.
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the GPIO channel
 * @param[in] mask the mask to be applied to @ channel
 *
 * @retval None
 */
void microblaze_gpio_set_data_direction(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
);

/**
 * @brief Get pin configuration for specified GPIO channel.
 *
 * Gets the current pin configuration for a specified GPIO channel. Bits set to
 * 0 are output, and bits set to 1 are input.
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the GPIO channel
 *
 * @retval bitmask specifiying which pins on a channel are input or output
 */
uint32_t microblaze_gpio_get_data_direction(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
);

/**
 * @brief Reads data for specified GPIO channel.
 *
 * @param[in] channel the GPIO channel
 *
 * @retval Current values in discretes register.
 */
uint32_t microblaze_gpio_discrete_read(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
);

/**
 * @brief Writes to data register for specified GPIO channel.
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the GPIO channel
 * @param[in] mask the mask to be applied to @ channel
 *
 * @retval None
 */
void microblaze_gpio_discrete_write(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
);

/**
 * @brief Set bits to 1 on specified GPIO channel.
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the GPIO channel
 * @param[in] mask the mask to be applied to @ channel
 *
 * @retval None
 */
void microblaze_gpio_discrete_set(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
);

/**
 * @brief Set bits to 0 on specified GPIO channel.
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the GPIO channel
 * @param[in] mask the mask to be applied to @ channel
 *
 * @retval None
 */
void microblaze_gpio_discrete_clear(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel,
  uint32_t                 mask
);

/**
 * @brief Returns the vector number of the interrupt handler.
 *
 * @param[in] ctx the GPIO context
 *
 * @retval the vector number
 */
rtems_vector_number microblaze_gpio_get_irq( Microblaze_GPIO_context *ctx );

/**
 * @brief Turns on interrupts globally.
 *
 * @param[in] ctx the GPIO context
 *
 * @retval None
 */
void microblaze_gpio_interrupt_global_enable( Microblaze_GPIO_context *ctx );

/**
 * @brief Turns off interrupts globally.
 *
 * @param[in] ctx the GPIO context
 *
 * @retval None
 */
void microblaze_gpio_interrupt_global_disable( Microblaze_GPIO_context *ctx );

/**
 * @brief Enables interrupts on specified channel
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the channel to enable interrupts on
 *
 * @retval None
 */
void microblaze_gpio_interrupt_enable(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
);

/**
 * @brief Disables interrupts on specified channel
 *
 * @param[in] ctx the GPIO context
 * @param[in] channel the channel to turn interrupts on for
 *
 * @retval None
 */
void microblaze_gpio_interrupt_disable(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
);

/**
 * @brief Clear status of interrupt signals on a specific channel
 *
 * @param[in] ctx     the GPIO context
 * @param[in] channel the channel to clear the interrupt pending status from
 *
 * @retval None
 */
void microblaze_gpio_interrupt_clear(
  Microblaze_GPIO_context *ctx,
  uint32_t                 channel
);

/**
 * @brief Return a bitmask of the interrupts that are enabled
 *
 * @param[in] ctx the GPIO context
 *
 * @retval the bitmask of enabled interrupts
 */
uint32_t microblaze_gpio_interrupt_get_enabled( Microblaze_GPIO_context *ctx );

/**
 * @brief Return a bitmask of the status of the interrupt signals
 *
 * @param[in] ctx the GPIO context
 *
 * @retval bitmask containing statuses of interrupt signals
 */
uint32_t microblaze_gpio_interrupt_get_status( Microblaze_GPIO_context *ctx );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_MICROBLAZE_FPGA_MICROBLAZE_GPIO_H */
