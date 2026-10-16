/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief A GPIO controller with no hardware behind it, for gpio01.
 */

/*
 * Copyright (C) 2026 Samuel Price <thesamprice@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "test_gpio.h"

#include <errno.h>
#include <string.h>

/*
 * This models a part, not a board: what a pin can do is fixed in
 * test_gpio_pins, what a pin is currently set to lives in test_gpio_state.
 * Keeping them apart is the same split the API draws between
 * rtems_gpio_pin_get_info() and rtems_gpio_pin_get_configuration(), and
 * having the simulator obey it is most of what makes the test meaningful.
 */

typedef struct {
  uint32_t capabilities;
  uint32_t flags;
  rtems_gpio_pin_kind kind;
  const char* name;
} test_gpio_pin;

#define TEST_GPIO_CAP_FULL                                                     \
  (RTEMS_GPIO_CAP_INPUT | RTEMS_GPIO_CAP_OUTPUT |                              \
   RTEMS_GPIO_CAP_OUTPUT_READBACK | RTEMS_GPIO_CAP_PULL_UP |                   \
   RTEMS_GPIO_CAP_PULL_DOWN | RTEMS_GPIO_CAP_OPEN_DRAIN |                      \
   RTEMS_GPIO_CAP_DRIVE_STRENGTH | RTEMS_GPIO_CAP_DEBOUNCE |                   \
   RTEMS_GPIO_CAP_EDGE_RISING | RTEMS_GPIO_CAP_EDGE_FALLING |                  \
   RTEMS_GPIO_CAP_EDGE_BOTH)

static const test_gpio_pin test_gpio_pins[TEST_GPIO_PIN_COUNT] = {
    [TEST_GPIO_PIN_FULL] = {.capabilities = TEST_GPIO_CAP_FULL,
                            .flags = RTEMS_GPIO_PIN_AVAILABLE,
                            .kind = RTEMS_GPIO_PIN_PHYSICAL,
                            .name = "LED0"},
    [TEST_GPIO_PIN_FULL2] = {.capabilities = TEST_GPIO_CAP_FULL,
                             .flags = RTEMS_GPIO_PIN_AVAILABLE,
                             .kind = RTEMS_GPIO_PIN_PHYSICAL,
                             .name = ""},
    [TEST_GPIO_PIN_FULL3] = {.capabilities = TEST_GPIO_CAP_FULL,
                             .flags = RTEMS_GPIO_PIN_AVAILABLE,
                             .kind = RTEMS_GPIO_PIN_PHYSICAL,
                             .name = ""},
    /* Above bit 31, so a bulk operation has to reach the second word. */
    [TEST_GPIO_PIN_HIGH] = {.capabilities = TEST_GPIO_CAP_FULL,
                            .flags = RTEMS_GPIO_PIN_AVAILABLE,
                            .kind = RTEMS_GPIO_PIN_PHYSICAL,
                            .name = "HIGH33"},
    [TEST_GPIO_PIN_INPUT] = {.capabilities =
                                 RTEMS_GPIO_CAP_INPUT | RTEMS_GPIO_CAP_PULL_UP,
                             .flags = RTEMS_GPIO_PIN_AVAILABLE,
                             .kind = RTEMS_GPIO_PIN_PHYSICAL,
                             .name = ""},
    [TEST_GPIO_PIN_RESERVED] =
        {/*
          * Capable of everything and permitted for nothing, which is exactly
          * the case that matters: a flash pad is an ordinary pad electrically.
          * If the reserved flag were only set on a pin that could not do
          * anything anyway, the test would pass without the check it is there
          * to prove.
          */
         .capabilities = TEST_GPIO_CAP_FULL,
         .flags = RTEMS_GPIO_PIN_RESERVED,
         .kind = RTEMS_GPIO_PIN_PHYSICAL,
         .name = "FLASH_CLK"},
    [TEST_GPIO_PIN_NO_PAD] = {.capabilities = TEST_GPIO_CAP_FULL,
                              .flags = RTEMS_GPIO_PIN_NO_PAD,
                              .kind = RTEMS_GPIO_PIN_PHYSICAL,
                              .name = ""},
    [TEST_GPIO_PIN_STRAPPING] = {.capabilities = TEST_GPIO_CAP_FULL,
                                 .flags = RTEMS_GPIO_PIN_STRAPPING,
                                 .kind = RTEMS_GPIO_PIN_PHYSICAL,
                                 .name = "BOOT"},
    [TEST_GPIO_PIN_VIRTUAL] = {
        /* An expander behind a bus: it has pins and pulls, and nothing else. */
        .capabilities = RTEMS_GPIO_CAP_INPUT | RTEMS_GPIO_CAP_OUTPUT |
                        RTEMS_GPIO_CAP_PULL_UP,
        .flags = RTEMS_GPIO_PIN_AVAILABLE,
        .kind = RTEMS_GPIO_PIN_VIRTUAL,
        .name = "EXP0"}};

typedef struct {
  bool in_use;
  rtems_gpio_config config;
  int level; /* the pad, not the logical value */
  rtems_gpio_irq_handler handler;
  void* arg;
  rtems_gpio_irq_flags irq_flags;
} test_gpio_pin_state;

typedef struct {
  rtems_gpio_drv_ctrl base;
  test_gpio_pin_state pins[TEST_GPIO_PIN_COUNT];
  uint32_t active_low[RTEMS_GPIO_BITMAP_WORDS(TEST_GPIO_PIN_COUNT)];
  uint32_t scratch[RTEMS_GPIO_BITMAP_WORDS(TEST_GPIO_PIN_COUNT)];
} test_gpio_ctrl;

static test_gpio_ctrl test_gpio_instance;
static test_gpio_ctrl test_gpio_minimal_instance;

/*
 * One shot fault injection.  Two paths in the generic layer only run when a
 * driver fails after it has already been asked to do something, and no
 * amount of arranging the arguments reaches them from outside -- the driver
 * has to be made to fail on purpose.
 */
static int test_gpio_configure_err;
static int test_gpio_info_err;

void test_gpio_fail_next_configure(int err) {
  test_gpio_configure_err = err;
}

void test_gpio_fail_next_pin_get_info(int err) {
  test_gpio_info_err = err;
}

static test_gpio_ctrl* test_gpio_downcast(rtems_gpio_drv_ctrl* ctrl) {
  return RTEMS_CONTAINER_OF(ctrl, test_gpio_ctrl, base);
}

/*
 * There is deliberately no inversion anywhere in this driver.  A level here
 * is the level of the pad, and RTEMS_GPIO_FLAG_ACTIVE_LOW is applied above
 * by the generic layer, which is what lets a controller with no inversion
 * register support active low pins.  test_gpio_raw_level() is how the test
 * reads the pad, and it is the same number this file stores.
 */

static int test_gpio_pin_get_info(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                                  rtems_gpio_pin_info* info) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);

  if (test_gpio_info_err != 0) {
    int err = test_gpio_info_err;

    test_gpio_info_err = 0;

    return err;
  }

  info->pin = pin;
  info->kind = test_gpio_pins[pin].kind;
  info->capabilities = test_gpio_pins[pin].capabilities;
  info->flags = test_gpio_pins[pin].flags;

  if (self->pins[pin].in_use) {
    info->flags |= RTEMS_GPIO_PIN_IN_USE;
  }

  /*
   * Most pins of this controller have no table entry and so no name.  The
   * generic layer zeroed the structure, so an unnamed pin reports an empty
   * string rather than being copied from a null pointer.
   */
  if (test_gpio_pins[pin].name != NULL) {
    strncpy(info->name, test_gpio_pins[pin].name, sizeof(info->name) - 1);
  }

  /*
   * A reserved pad names what has it.  A real driver takes this from
   * whatever the BSP uses to arbitrate pads between its drivers; here it is
   * a constant, because the point under test is that the generic layer
   * carries the answer through to the caller.
   */
  if ((info->flags & RTEMS_GPIO_PIN_RESERVED) != 0) {
    strncpy(info->owner, "test-board", sizeof(info->owner) - 1);
  }

  return 0;
}

static int test_gpio_pin_configure(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                                   rtems_gpio_config* config) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  test_gpio_pin_state* state = &self->pins[pin];

  if (test_gpio_configure_err != 0) {
    int err = test_gpio_configure_err;

    test_gpio_configure_err = 0;

    return err;
  }

  /*
   * Rounded to what this "part" has, and written back, so that a caller
   * asking for 8000 uA is told it got 10000 rather than being left to
   * assume it got what it asked for.
   */
  if (config->drive_strength != 0) {
    config->drive_strength = TEST_GPIO_DRIVE_STRENGTH;
  }

  state->config = *config;
  state->in_use = true;

  if (config->direction == RTEMS_GPIO_DIRECTION_OUTPUT) {
    state->level = config->initial_value != 0 ? 1 : 0;
  }

  return 0;
}

static int test_gpio_pin_get_config(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                                    rtems_gpio_config* config) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);

  *config = self->pins[pin].config;

  return 0;
}

static int test_gpio_pin_release(rtems_gpio_drv_ctrl* ctrl, uint32_t pin) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);

  memset(&self->pins[pin], 0, sizeof(self->pins[pin]));

  return 0;
}

static int test_gpio_pin_get(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                             int* value) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  test_gpio_pin_state* state = &self->pins[pin];

  /*
   * An output reads back, because this controller reports
   * RTEMS_GPIO_CAP_OUTPUT_READBACK.  One that did not would return the last
   * value written instead, which here is the same number.
   */
  *value = state->level;

  return 0;
}

static int test_gpio_pin_set(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                             int value) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  test_gpio_pin_state* state = &self->pins[pin];

  if (state->config.direction == RTEMS_GPIO_DIRECTION_INPUT) {
    return ENOTSUP;
  }

  state->level = value != 0 ? 1 : 0;

  return 0;
}

static int test_gpio_pin_toggle(rtems_gpio_drv_ctrl* ctrl, uint32_t pin) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  test_gpio_pin_state* state = &self->pins[pin];

  if (state->config.direction == RTEMS_GPIO_DIRECTION_INPUT) {
    return ENOTSUP;
  }

  state->level = state->level != 0 ? 0 : 1;

  return 0;
}

/*
 * Bit n of the bitmaps is pin n of this controller, so the loop is over the
 * pins rather than over a list, and a real driver with banked registers
 * would work a word at a time instead.  The caller's word count is a limit
 * the driver has to respect: reading or writing past it would be touching
 * storage the caller never allocated.
 */
static int test_gpio_pin_get_multiple(rtems_gpio_drv_ctrl* ctrl,
                                      const uint32_t* mask, uint32_t* values,
                                      size_t words) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  uint32_t pins = (uint32_t)(words * RTEMS_GPIO_BITMAP_WORD_BITS);
  uint32_t pin;

  if (pins > TEST_GPIO_PIN_COUNT) {
    pins = TEST_GPIO_PIN_COUNT;
  }

  for (pin = 0; pin < pins; ++pin) {
    uint32_t word = pin / RTEMS_GPIO_BITMAP_WORD_BITS;
    uint32_t bit = 1u << (pin % RTEMS_GPIO_BITMAP_WORD_BITS);

    if ((mask[word] & bit) == 0) {
      continue;
    }

    if (self->pins[pin].level != 0) {
      values[word] |= bit;
    } else {
      values[word] &= ~bit;
    }
  }

  return 0;
}

static int test_gpio_pin_set_multiple(rtems_gpio_drv_ctrl* ctrl,
                                      const uint32_t* mask,
                                      const uint32_t* values, size_t words) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  uint32_t pins = (uint32_t)(words * RTEMS_GPIO_BITMAP_WORD_BITS);
  uint32_t pin;

  if (pins > TEST_GPIO_PIN_COUNT) {
    pins = TEST_GPIO_PIN_COUNT;
  }

  for (pin = 0; pin < pins; ++pin) {
    uint32_t word = pin / RTEMS_GPIO_BITMAP_WORD_BITS;
    uint32_t bit = 1u << (pin % RTEMS_GPIO_BITMAP_WORD_BITS);

    if ((mask[word] & bit) == 0) {
      continue;
    }

    self->pins[pin].level = (values[word] & bit) != 0 ? 1 : 0;
  }

  return 0;
}

static int test_gpio_pin_irq_enable(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                                    rtems_gpio_irq_flags flags,
                                    rtems_gpio_irq_handler handler, void* arg) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);
  test_gpio_pin_state* state = &self->pins[pin];

  /* Recorded so a test can show the options reached the driver rather than
   * being consumed above it. */
  state->irq_flags = flags;

  if (state->config.trigger == RTEMS_GPIO_TRIGGER_NONE) {
    return EINVAL;
  }

  state->handler = handler;
  state->arg = arg;

  return 0;
}

static int test_gpio_pin_irq_disable(rtems_gpio_drv_ctrl* ctrl, uint32_t pin) {
  test_gpio_ctrl* self = test_gpio_downcast(ctrl);

  self->pins[pin].handler = NULL;
  self->pins[pin].arg = NULL;

  return 0;
}

/*
 * Called when the device node is destroyed.  A real driver frees whatever
 * it allocated; this one only records that it was asked, because the point
 * under test is that the generic layer asks at all.
 */
static int test_gpio_destroy_count;

static void test_gpio_destroy(rtems_gpio_drv_ctrl* ctrl) {
  (void)ctrl;
  ++test_gpio_destroy_count;
}

int test_gpio_destroyed(void) {
  return test_gpio_destroy_count;
}

static const rtems_gpio_drv_handlers test_gpio_handlers = {
    .pin_get_info = test_gpio_pin_get_info,
    .pin_configure = test_gpio_pin_configure,
    .pin_get_config = test_gpio_pin_get_config,
    .pin_release = test_gpio_pin_release,
    .pin_get = test_gpio_pin_get,
    .pin_set = test_gpio_pin_set,
    .pin_toggle = test_gpio_pin_toggle,
    .pin_get_multiple = test_gpio_pin_get_multiple,
    .pin_set_multiple = test_gpio_pin_set_multiple,
    .pin_irq_enable = test_gpio_pin_irq_enable,
    .pin_irq_disable = test_gpio_pin_irq_disable,
    .destroy = test_gpio_destroy};

/*
 * Everything but pin_get_info() left NULL.  A driver is allowed to be this
 * incomplete, and the generic layer has to answer for it rather than call
 * through a null pointer, which is what the ENOTSUP half of the test
 * checks.
 */
static const rtems_gpio_drv_handlers test_gpio_minimal_handlers = {
    .pin_get_info = test_gpio_pin_get_info};

int test_gpio_register(const char* path) {
  int err;

  memset(&test_gpio_instance, 0, sizeof(test_gpio_instance));

  test_gpio_instance.base.handlers = &test_gpio_handlers;
  test_gpio_instance.base.pin_count = TEST_GPIO_PIN_COUNT;
  test_gpio_instance.base.name = "test-gpio";
  test_gpio_instance.base.active_low = test_gpio_instance.active_low;
  test_gpio_instance.base.scratch = test_gpio_instance.scratch;

  err = rtems_gpio_drv_ctrl_init(&test_gpio_instance.base);
  if (err != 0) {
    return err;
  }

  return rtems_gpio_drv_ctrl_register(&test_gpio_instance.base, path);
}

int test_gpio_register_minimal(const char* path) {
  int err;

  memset(&test_gpio_minimal_instance, 0, sizeof(test_gpio_minimal_instance));

  test_gpio_minimal_instance.base.handlers = &test_gpio_minimal_handlers;
  test_gpio_minimal_instance.base.pin_count = TEST_GPIO_PIN_COUNT;
  test_gpio_minimal_instance.base.name = "test-gpio-min";
  test_gpio_minimal_instance.base.active_low =
      test_gpio_minimal_instance.active_low;
  test_gpio_minimal_instance.base.scratch = test_gpio_minimal_instance.scratch;

  err = rtems_gpio_drv_ctrl_init(&test_gpio_minimal_instance.base);
  if (err != 0) {
    return err;
  }

  return rtems_gpio_drv_ctrl_register(&test_gpio_minimal_instance.base, path);
}

int test_gpio_raw_level(uint32_t pin) {
  return test_gpio_instance.pins[pin].level;
}

void test_gpio_set_raw_level(uint32_t pin, int value) {
  test_gpio_instance.pins[pin].level = value != 0 ? 1 : 0;
}

rtems_gpio_irq_flags test_gpio_irq_flags(uint32_t pin) {
  return test_gpio_instance.pins[pin].irq_flags;
}

void test_gpio_fire_irq(uint32_t pin) {
  test_gpio_pin_state* state = &test_gpio_instance.pins[pin];

  if (state->handler != NULL) {
    (*state->handler)(pin, state->arg);
  }
}
