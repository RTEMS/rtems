/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019, 2020 embedded brains GmbH & Co. KG
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

#include <assert.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/imx-gpio.h>
#include <libfdt.h>
#include <rtems.h>
#include <rtems/sysinit.h>

/*
 * Most of the time it's gpio1 or gpio13.
 */
#define IMX_GPIO_ALIAS_NAME "gpioXY"

/*
 * i.MX6ULL has 5, i.MX7D has 7, i.MXRT1160 has 13 (base) + 2 (core-specific).
 *
 * Be careful when changing this. The attach() does a simple ASCII conversion.
 */
#define IMX_MAX_GPIO_MODULES 15

struct imx_gpio_regs {
  uint32_t dr;
  uint32_t gdir;
  uint32_t psr;
  uint32_t icr1;
#define IMX_GPIO_ICR_LOW_LEVEL 0
#define IMX_GPIO_ICR_HIGH_LEVEL 1
#define IMX_GPIO_ICR_RISING_EDGE 2
#define IMX_GPIO_ICR_FALLING_EDGE 3
  uint32_t icr2;
  uint32_t imr;
  uint32_t isr;
  uint32_t edge_sel;
};

struct imx_gpio {
  char name[sizeof(IMX_GPIO_ALIAS_NAME)];
  struct imx_gpio_regs *regs;
  rtems_interrupt_lock lock;
};

/* The GPIO modules. These will be initialized based on the FDT alias table. */
struct imx_gpio imx_gpio[IMX_MAX_GPIO_MODULES];

const char *imx_gpio_get_name(struct imx_gpio *imx_gpio)
{
  return imx_gpio->name;
}

static void imx_gpio_attach(void)
{
  size_t i;
  const void *fdt;

  fdt = bsp_fdt_get();

  memset(imx_gpio, 0, sizeof(imx_gpio));

  for (i = 0; i < IMX_MAX_GPIO_MODULES; ++i) {
    const char *path;
    int node;
    const uint32_t *val;
    uint32_t gpio_regs = 0;
    int len;

    memcpy(imx_gpio[i].name, IMX_GPIO_ALIAS_NAME, sizeof(IMX_GPIO_ALIAS_NAME));
    if (i < 10) {
      imx_gpio[i].name[sizeof(IMX_GPIO_ALIAS_NAME)-3] = (char)('0' + i);
      imx_gpio[i].name[sizeof(IMX_GPIO_ALIAS_NAME)-2] = '\0';
    } else {
      imx_gpio[i].name[sizeof(IMX_GPIO_ALIAS_NAME)-3] = (char)('0' + i / 10);
      imx_gpio[i].name[sizeof(IMX_GPIO_ALIAS_NAME)-2] = (char)('0' + i % 10);
      imx_gpio[i].name[sizeof(IMX_GPIO_ALIAS_NAME)-1] = '\0';
    }

    path = fdt_get_alias(fdt, imx_gpio[i].name);
    if (path == NULL) {
      continue;
    }

    node = fdt_path_offset(fdt, path);
    if (node < 0) {
      bsp_fatal(IMX_FATAL_GPIO_UNEXPECTED_FDT);
    }

    val = fdt_getprop(fdt, node, "reg", &len);
    if (len > 0) {
      gpio_regs = fdt32_to_cpu(val[0]);
    } else {
      bsp_fatal(IMX_FATAL_GPIO_UNEXPECTED_FDT);
    }

    imx_gpio[i].regs = (struct imx_gpio_regs *)gpio_regs;
    rtems_interrupt_lock_initialize(&imx_gpio[i].lock, imx_gpio[i].name);
  }
}

struct imx_gpio *imx_gpio_get_by_index(unsigned idx)
{
  if ((idx < IMX_MAX_GPIO_MODULES) && (imx_gpio[idx].regs != NULL)) {
    return &imx_gpio[idx];
  }
  return NULL;
}

struct imx_gpio *imx_gpio_get_by_register(void *regs)
{
  size_t i;

  for (i = 0; i < IMX_MAX_GPIO_MODULES; ++i) {
    if (imx_gpio[i].regs == regs) {
      return &imx_gpio[i];
    }
  }
  return NULL;
}

static void imx_gpio_direction_input(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->gdir &= ~pin->mask;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

static void imx_gpio_direction_output(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->gdir |= pin->mask;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

static void imx_gpio_set_interrupt_any_edge(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->edge_sel |= pin->mask;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

static void imx_gpio_set_interrupt_mode(struct imx_gpio_pin *pin, uint32_t mode)
{
  size_t i;

  for (i=0; i < 32; ++i) {
    if ((pin->mask & (1u << i)) != 0) {
      volatile uint32_t *icr;
      size_t shift;
      rtems_interrupt_lock_context lock_context;

      if (i < 16) {
        icr = &pin->gpio->regs->icr1;
        shift = 2 * i;
      } else {
        icr = &pin->gpio->regs->icr2;
        shift = 2 * (i - 16);
      }

      rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
      *icr = (*icr & ~(3u << shift)) | (mode << shift);
      rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
    }
  }
}

rtems_status_code imx_gpio_init_from_fdt_property_pointer (
  struct imx_gpio_pin *pin,
  const uint32_t *prop_pointer,
  enum imx_gpio_mode mode,
  const uint32_t **next_prop_pointer
)
{
  int len;
  const uint32_t *val;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const void *fdt;
  uint32_t gpio_regs;
  const unsigned pin_length_dwords = 3;
  uint32_t gpio_phandle;
  uint32_t pin_nr;
  int cfgnode;

  memset(pin, 0, sizeof(*pin));

  fdt = bsp_fdt_get();
  if (sc == RTEMS_SUCCESSFUL) {
    pin_nr = fdt32_to_cpu(prop_pointer[1]);
    gpio_phandle = fdt32_to_cpu(prop_pointer[0]);

    cfgnode = fdt_node_offset_by_phandle(fdt, gpio_phandle);
    /* FIXME: Check compatible strings here. */
    val = fdt_getprop(fdt, cfgnode, "reg", &len);
    if (len > 0) {
      gpio_regs = fdt32_to_cpu(val[0]);
    } else {
      sc = RTEMS_UNSATISFIED;
    }
  }
  if (sc == RTEMS_SUCCESSFUL) {
    pin->gpio = imx_gpio_get_by_register((void *)gpio_regs);
    pin->mask = 1u << pin_nr;
    pin->shift = pin_nr;
    pin->mode = mode;
  }
  if (sc == RTEMS_SUCCESSFUL) {
    imx_gpio_init(pin);
  }
  if (sc == RTEMS_SUCCESSFUL && next_prop_pointer != NULL) {
    *next_prop_pointer = prop_pointer + pin_length_dwords;
  }

  return sc;
}

rtems_status_code imx_gpio_init_from_fdt_property (
  struct imx_gpio_pin *pin,
  int node_offset,
  const char *property,
  enum imx_gpio_mode mode,
  size_t index
)
{
  int len;
  const uint32_t *val;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const void *fdt;
  const unsigned pin_length_dwords = 3;
  const unsigned pin_length_bytes = pin_length_dwords * 4;

  memset(pin, 0, sizeof(*pin));

  fdt = bsp_fdt_get();
  val = fdt_getprop(fdt, node_offset, property, &len);
  if (val == NULL || (len % pin_length_bytes != 0) ||
      (index >= len / pin_length_bytes)) {
    sc = RTEMS_UNSATISFIED;
  }
  if (sc == RTEMS_SUCCESSFUL) {
    sc = imx_gpio_init_from_fdt_property_pointer(
      pin,
      val + index * pin_length_dwords,
      mode,
      NULL);
  }

  return sc;
}

rtems_vector_number imx_gpio_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
)
{
  const uint32_t *val;
  uint32_t pin;
  int parent;
  size_t parent_index;
  int len;

  val = fdt_getprop(fdt, node, "interrupts", &len);
  if (val == NULL || len < (int) ((index + 1) * 8)) {
    return UINT32_MAX;
  }
  pin = fdt32_to_cpu(val[index * 2]);
  if (pin < 16) {
    parent_index = 0;
  } else {
    parent_index = 1;
  }

  val = fdt_getprop(fdt, node, "interrupt-parent", &len);
  if (len != 4) {
    return UINT32_MAX;
  }
  parent = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));

  return imx_get_irq_of_node(fdt, parent, parent_index);
}

void imx_gpio_init (struct imx_gpio_pin *pin)
{
  switch (pin->mode) {
  case (IMX_GPIO_MODE_INTERRUPT_LOW):
    imx_gpio_direction_input(pin);
    imx_gpio_set_interrupt_mode(pin, IMX_GPIO_ICR_LOW_LEVEL);
    break;
  case (IMX_GPIO_MODE_INTERRUPT_HIGH):
    imx_gpio_direction_input(pin);
    imx_gpio_set_interrupt_mode(pin, IMX_GPIO_ICR_HIGH_LEVEL);
    break;
  case (IMX_GPIO_MODE_INTERRUPT_RISING):
    imx_gpio_direction_input(pin);
    imx_gpio_set_interrupt_mode(pin, IMX_GPIO_ICR_RISING_EDGE);
    break;
  case (IMX_GPIO_MODE_INTERRUPT_FALLING):
    imx_gpio_direction_input(pin);
    imx_gpio_set_interrupt_mode(pin, IMX_GPIO_ICR_FALLING_EDGE);
    break;
  case (IMX_GPIO_MODE_INTERRUPT_ANY_EDGE):
    imx_gpio_direction_input(pin);
    imx_gpio_set_interrupt_any_edge(pin);
    /* Interrupt mode isn't really relevant here. Just set it to get
     * a defined behaviour in case of a bug. */
    imx_gpio_set_interrupt_mode(pin, IMX_GPIO_ICR_FALLING_EDGE);
    break;
  case (IMX_GPIO_MODE_INPUT):
    imx_gpio_direction_input(pin);
    break;
  case (IMX_GPIO_MODE_OUTPUT):
    imx_gpio_direction_output(pin);
    break;
  default:
    assert(false);
    break;
  }
}

void imx_gpio_set_output(struct imx_gpio_pin *pin, uint32_t set)
{
  rtems_interrupt_lock_context lock_context;
  set <<= pin->shift;
  set &= pin->mask;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->dr = (pin->gpio->regs->dr & ~pin->mask) | set;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

void imx_gpio_toggle_output(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->dr = (pin->gpio->regs->dr ^ pin->mask);
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

uint32_t imx_gpio_get_input(struct imx_gpio_pin *pin)
{
  return (pin->gpio->regs->dr & pin->mask) >> pin->shift;
}

void imx_gpio_int_disable(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->imr &= ~pin->mask;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

void imx_gpio_int_enable(struct imx_gpio_pin *pin)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&pin->gpio->lock, &lock_context);
  pin->gpio->regs->imr |= pin->mask;
  rtems_interrupt_lock_release(&pin->gpio->lock, &lock_context);
}

uint32_t imx_gpio_get_isr(struct imx_gpio_pin *pin)
{
  return (pin->gpio->regs->isr & pin->mask) >> pin->shift;
}

void imx_gpio_clear_isr(struct imx_gpio_pin *pin, uint32_t clr)
{
  pin->gpio->regs->isr = (clr << pin->shift) & pin->mask;
}

RTEMS_SYSINIT_ITEM(
  imx_gpio_attach,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_FIRST
);
