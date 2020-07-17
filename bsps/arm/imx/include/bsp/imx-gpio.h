/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef BSP_IMX_GPIO_H
#define BSP_IMX_GPIO_H

#include <rtems.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Hardware registers and locking mechanism for one hardware GPIO module. */
struct imx_gpio;

/** Mode of the pin. */
enum imx_gpio_mode {
  IMX_GPIO_MODE_OUTPUT,
  IMX_GPIO_MODE_INPUT,
  IMX_GPIO_MODE_INTERRUPT_LOW,
  IMX_GPIO_MODE_INTERRUPT_HIGH,
  IMX_GPIO_MODE_INTERRUPT_RISING,
  IMX_GPIO_MODE_INTERRUPT_FALLING,
  IMX_GPIO_MODE_INTERRUPT_ANY_EDGE,
};

/**
 * A i.MX GPIO pin or set of pins.
 *
 * Use this structures to handle pins in the application. You can either get
 * them from an FDT entry (with @ref imx_gpio_init_from_fde_property) or fill
 * them by hand.
 */
struct imx_gpio_pin {
  /** Management structure for the GPIO. Get with @ref imx_gpio_get_by_index. */
  volatile struct imx_gpio* gpio;
  /**
   * Select the pins you want to handle with this mask. The mask is not
   * influenced by the @a shift field.
   */
  uint32_t mask;
  /** If set to something != 0: Shift the pins that many bits. */
  unsigned int shift;
  /** Whether the pin is an input, output, interrupt, ... */
  enum imx_gpio_mode mode;
};

/**
 * Initialize a GPIO pin. Only necessary for manually filled imx_gpio
 * structures.
 */
void imx_gpio_init (struct imx_gpio_pin *pin);

/**
 * Initialize a GPIO pin from a FDT property.
 *
 * If you have for example the following property in an FDT node:
 *
 *     some-node {
 *         gpios = <&gpio5 1 GPIO_ACTIVE_LOW>, <&gpio4 22 GPIO_ACTIVE_LOW>;
 *     };
 *
 * you can use the following to initialize the second GPIO:
 *
 *     imx_gpio_init_from_fdt_property(&pin, node, "gpios",
 *         IMX_GPIO_INTERRUPT_LOW, 1);
 *
 * NOTE: The information from the third parameter in the FDT (GPIO_ACTIVE_LOW in
 * the example) is currently ignored.
 */
rtems_status_code imx_gpio_init_from_fdt_property(
  struct imx_gpio_pin *pin,
  int node_offset,
  const char *property,
  enum imx_gpio_mode mode,
  size_t index);

/**
 * Return the RTEMS interrupt vector belonging to the GPIO interrupt of a given
 * node. The node should look like follows:
 *
 *     some-node {
 *         interrupt-parent = <&gpio4>;
 *         interrupts = <15 IRQ_TYPE_EDGE_BOTH>, <22 IRQ_TYPE_EDGE_BOTH>;
 *     };
 *
 * To get the interrupt vector from the first GPIO in interrupts use
 *
 *     imx_gpio_get_irq_of_node(fdt, node, 0);
 *
 * @returns the interrupt vector if successful.
 * @returns BSP_INTERRUPT_VECTOR_INVALID on failure.
 */
rtems_vector_number imx_gpio_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index);

/**
 * Return the gpio management structure based on the GPIO index. The index is
 * the one used in the FDT alias list. So index 0 is GPIO1 in the i.MX docs for
 * most FDTs based on the Linux one.
 */
struct imx_gpio *imx_gpio_get_by_index(unsigned idx);

/**
 * Return the gpio management structure based on the GPIO registers.
 */
struct imx_gpio *imx_gpio_get_by_register(void *regs);

/**
 * Get the name of the gpio.
 */
const char *imx_gpio_get_name(struct imx_gpio *imx_gpio);

/**
 * Set the value of the output pin. @a set will be shifted and masked (in that
 * order) based on the values of @a pin.
 */
void imx_gpio_set_output(struct imx_gpio_pin *pin, uint32_t set);

/**
 * Toggle the value of the output pin.
 */
void imx_gpio_toggle_output(struct imx_gpio_pin *pin);

/**
 * Get the value of the input pin. The input value will be masked and shifted
 * (in that order) based on the values of @a pin.
 */
uint32_t imx_gpio_get_input(struct imx_gpio_pin *pin);

/**
 * Disable the interrupt of the given @a pin.
 */
void imx_gpio_int_disable(struct imx_gpio_pin *pin);

/**
 * Enable the interrupt of the given @a pin.
 */
void imx_gpio_int_enable(struct imx_gpio_pin *pin);

/**
 * Read the interrupt status register for the given @a pin.
 */
uint32_t imx_gpio_get_isr(struct imx_gpio_pin *pin);

/**
 * Clear the interrupt status register for the given @a pin.
 */
void imx_gpio_clear_isr(struct imx_gpio_pin *pin, uint32_t clr);

/**
 * Fast access macros for the GPIOs. Note that these assume a FDT based on the
 * Linux FDTs.
 */
/** @{ */
#define IMX_GPIO1 (imx_gpio_get_by_index(0))
#define IMX_GPIO2 (imx_gpio_get_by_index(1))
#define IMX_GPIO3 (imx_gpio_get_by_index(2))
#define IMX_GPIO4 (imx_gpio_get_by_index(3))
#define IMX_GPIO5 (imx_gpio_get_by_index(4))
#define IMX_GPIO6 (imx_gpio_get_by_index(5))
#define IMX_GPIO7 (imx_gpio_get_by_index(6))
/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_IMX_GPIO_H */
