/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <rtems/libcsupport.h>
#include <tmacros.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

const char rtems_test_name[] = "GPIO 1";

#define GPIO_PATH "/dev/gpio"
#define GPIO_MIN_PATH "/dev/gpio-min"

/*
 * A failed call must set errno and must not have touched the hardware.  The
 * second half is the one worth asserting: a refusal that has already muxed
 * the pad is the failure this API exists to prevent, and it looks exactly
 * like a refusal that has not.
 */
static void assert_fails(int rv, int expected_errno) {
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == expected_errno);
}

static void test_controller_info(int fd) {
  rtems_gpio_info info;

  rtems_test_assert(rtems_gpio_get_info(fd, &info) == 0);
  rtems_test_assert(info.pin_count == TEST_GPIO_PIN_COUNT);
  rtems_test_assert(strcmp(info.name, "test-gpio") == 0);
}

static void test_pin_info(int fd) {
  rtems_gpio_pin_info info;

  rtems_test_assert(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_FULL, &info) ==
                    0);
  rtems_test_assert(info.pin == TEST_GPIO_PIN_FULL);
  rtems_test_assert(info.kind == RTEMS_GPIO_PIN_PHYSICAL);
  rtems_test_assert(info.flags == RTEMS_GPIO_PIN_AVAILABLE);
  rtems_test_assert((info.capabilities & RTEMS_GPIO_CAP_OUTPUT) != 0);
  rtems_test_assert(strcmp(info.name, "LED0") == 0);

  /* Reserved, and capable of everything, which is the point of it. */
  rtems_test_assert(
      rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_RESERVED, &info) == 0);
  rtems_test_assert((info.flags & RTEMS_GPIO_PIN_RESERVED) != 0);
  rtems_test_assert((info.capabilities & RTEMS_GPIO_CAP_OUTPUT) != 0);

  rtems_test_assert(
      rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_STRAPPING, &info) == 0);
  rtems_test_assert((info.flags & RTEMS_GPIO_PIN_STRAPPING) != 0);

  /* Virtual pins report themselves so a caller can tell a bus from a bit. */
  rtems_test_assert(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_VIRTUAL, &info) ==
                    0);
  rtems_test_assert(info.kind == RTEMS_GPIO_PIN_VIRTUAL);
  rtems_test_assert((info.capabilities & RTEMS_GPIO_CAP_OPEN_DRAIN) == 0);

  /* One past the end is not a pin. */
  assert_fails(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_COUNT, &info), ENODEV);
}

static void test_pin_by_name(int fd) {
  uint32_t pin;

  rtems_test_assert(rtems_gpio_pin_by_name(fd, "BOOT", &pin) == 0);
  rtems_test_assert(pin == TEST_GPIO_PIN_STRAPPING);

  rtems_test_assert(rtems_gpio_pin_by_name(fd, "LED0", &pin) == 0);
  rtems_test_assert(pin == TEST_GPIO_PIN_FULL);

  assert_fails(rtems_gpio_pin_by_name(fd, "NOT_A_PIN", &pin), ENOENT);
}

/*
 * The reason this API exists.  A pin the board has spoken for is refused
 * before the driver is asked to do anything, and the refusal is EACCES
 * rather than EBUSY: nothing is going to release it.
 */
static void test_reserved_pin_is_refused(int fd) {
  rtems_gpio_config config;
  rtems_gpio_pin_info info;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;

  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_RESERVED, &config),
               EACCES);

  /* And the refusal left it alone, rather than configuring it and failing. */
  rtems_test_assert(
      rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_RESERVED, &info) == 0);
  rtems_test_assert((info.flags & RTEMS_GPIO_PIN_IN_USE) == 0);

  /*
   * And it says what has the pad.  EACCES on its own tells a caller it may
   * not have the pin; this tells it why, which is the difference between a
   * log line someone can act on and one they cannot.
   */
  rtems_test_assert(strcmp(info.owner, "test-board") == 0);

  /* A pin nobody has spoken for reports no owner rather than stale text. */
  rtems_test_assert(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_FULL, &info) ==
                    0);
  rtems_test_assert(info.owner[0] == '\0');

  /* A pin the package does not bring out is not there to be used either. */
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_NO_PAD, &config),
               ENODEV);

  /* Strapping is a warning, not a refusal: it configures. */
  rtems_test_assert(
      rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_STRAPPING, &config) == 0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_STRAPPING) == 0);
}

/*
 * A capability the pin did not report is ENOTSUP, and it is decided from
 * the pin's information before the driver is called.  So a driver need not
 * check, and more to the point cannot forget to.
 */
static void test_capabilities_are_enforced(int fd) {
  rtems_gpio_config config;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.drive = RTEMS_GPIO_DRIVE_OPEN_DRAIN;

  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_VIRTUAL, &config),
               ENOTSUP);

  /* An output on an input-only pin. */
  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_INPUT, &config),
               ENOTSUP);

  /* A level trigger on a part with only edges. */
  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.trigger = RTEMS_GPIO_TRIGGER_LEVEL_HIGH;
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               ENOTSUP);

  /* The same pin, an edge trigger, and it configures. */
  config.trigger = RTEMS_GPIO_TRIGGER_EDGE_RISING;
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);
}

static void test_configure_and_report(int fd) {
  rtems_gpio_config config;
  rtems_gpio_config read_back;

  /* Nothing has configured it, so it reports as unconfigured, not as an
   * error: that is the answer a caller deciding whether a pin is free
   * wants. */
  rtems_test_assert(rtems_gpio_pin_get_configuration(fd, TEST_GPIO_PIN_FULL,
                                                     &read_back) == 0);
  rtems_test_assert(read_back.direction == RTEMS_GPIO_DIRECTION_NONE);

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.bias = RTEMS_GPIO_BIAS_PULL_UP;
  config.initial_value = 1;
  config.debounce = 20000;

  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);

  rtems_test_assert(rtems_gpio_pin_get_configuration(fd, TEST_GPIO_PIN_FULL,
                                                     &read_back) == 0);
  rtems_test_assert(read_back.direction == RTEMS_GPIO_DIRECTION_OUTPUT);
  rtems_test_assert(read_back.bias == RTEMS_GPIO_BIAS_PULL_UP);
  rtems_test_assert(read_back.debounce == 20000);
  rtems_test_assert(read_back.initial_value == 1);

  /* Configuring it set the level, so the pad is already where it should be
   * rather than glitching low until someone writes it. */
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);

  /* A second claimant is told, rather than quietly winning. */
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               EBUSY);

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);

  /* Released, so it configures again. */
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);
}

/*
 * A driver is allowed to round.  What it may not do is round silently, so
 * what comes back from configure() and from get_configuration() is what was
 * applied and not what was asked for.
 */
static void test_rounding_is_reported(int fd) {
  rtems_gpio_config config;
  rtems_gpio_config read_back;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.drive_strength = 8000;

  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(config.drive_strength == TEST_GPIO_DRIVE_STRENGTH);

  rtems_test_assert(rtems_gpio_pin_get_configuration(fd, TEST_GPIO_PIN_FULL,
                                                     &read_back) == 0);
  rtems_test_assert(read_back.drive_strength == TEST_GPIO_DRIVE_STRENGTH);

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);
}

static void test_read_write(int fd) {
  rtems_gpio_config config;
  int value;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);

  rtems_test_assert(rtems_gpio_pin_set(fd, TEST_GPIO_PIN_FULL, 1) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);
  rtems_test_assert(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, &value) == 0);
  rtems_test_assert(value == 1);

  rtems_test_assert(rtems_gpio_pin_toggle(fd, TEST_GPIO_PIN_FULL) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 0);
  rtems_test_assert(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, &value) == 0);
  rtems_test_assert(value == 0);

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);

  /* Driving a pin nobody configured is how an off-by-one reaches a pad that
   * belongs to another driver. */
  assert_fails(rtems_gpio_pin_set(fd, TEST_GPIO_PIN_FULL, 1), EBADF);
  assert_fails(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, &value), EBADF);
}

/*
 * An active-low pin speaks logical levels, and the pad goes the other way.
 * Asserting against the pad rather than against another API call is the
 * only way to tell inversion from a pair of matching mistakes.
 */
static void test_active_low(int fd) {
  rtems_gpio_config config;
  int value;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.flags = RTEMS_GPIO_FLAG_ACTIVE_LOW;
  config.initial_value = 1;

  rtems_test_assert(
      rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL2, &config) == 0);

  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 0);
  rtems_test_assert(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL2, &value) == 0);
  rtems_test_assert(value == 1);

  rtems_test_assert(rtems_gpio_pin_set(fd, TEST_GPIO_PIN_FULL2, 0) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 1);

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL2) == 0);
}

#define TEST_GPIO_WORDS RTEMS_GPIO_BITMAP_WORDS(TEST_GPIO_PIN_COUNT)

static void test_bit_put(uint32_t* map, uint32_t pin, bool value) {
  uint32_t word = pin / RTEMS_GPIO_BITMAP_WORD_BITS;
  uint32_t bit = 1u << (pin % RTEMS_GPIO_BITMAP_WORD_BITS);

  if (value) {
    map[word] |= bit;
  } else {
    map[word] &= ~bit;
  }
}

static bool test_bit_get(const uint32_t* map, uint32_t pin) {
  return (map[pin / RTEMS_GPIO_BITMAP_WORD_BITS] &
          (1u << (pin % RTEMS_GPIO_BITMAP_WORD_BITS))) != 0;
}

/*
 * The multiple-pin calls exist so that a set of pins is written as one
 * transaction.  The part worth testing is the failure: a bitmap with one
 * unconfigured pin selected changes nothing at all, because a half-applied
 * bus is worse than a refused one.
 */
static void test_multiple(int fd) {
  rtems_gpio_config config;
  rtems_gpio_pin_bitmap map;
  uint32_t mask[TEST_GPIO_WORDS];
  uint32_t values[TEST_GPIO_WORDS];

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;

  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(
      rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL2, &config) == 0);

  memset(mask, 0, sizeof(mask));
  memset(values, 0, sizeof(values));
  map.word_count = TEST_GPIO_WORDS;
  map.mask = mask;
  map.values = values;

  test_bit_put(mask, TEST_GPIO_PIN_FULL, true);
  test_bit_put(mask, TEST_GPIO_PIN_FULL2, true);
  test_bit_put(values, TEST_GPIO_PIN_FULL, true);
  test_bit_put(values, TEST_GPIO_PIN_FULL2, true);

  rtems_test_assert(rtems_gpio_pin_set_multiple(fd, &map) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 1);

  test_bit_put(values, TEST_GPIO_PIN_FULL2, false);
  rtems_test_assert(rtems_gpio_pin_set_multiple(fd, &map) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 0);

  memset(values, 0, sizeof(values));
  rtems_test_assert(rtems_gpio_pin_get_multiple(fd, &map) == 0);
  rtems_test_assert(test_bit_get(values, TEST_GPIO_PIN_FULL));
  rtems_test_assert(!test_bit_get(values, TEST_GPIO_PIN_FULL2));

  /* A selected pin nobody configured, so none of the selection is written. */
  test_bit_put(mask, TEST_GPIO_PIN_FULL3, true);
  memset(values, 0, sizeof(values));
  assert_fails(rtems_gpio_pin_set_multiple(fd, &map), EBADF);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);
  test_bit_put(mask, TEST_GPIO_PIN_FULL3, false);

  /*
   * A pin in the second bitmap word.  A controller narrower than 33 pins
   * could not tell a bulk operation that spans words from one that does not.
   */
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_HIGH, &config) ==
                    0);

  memset(values, 0, sizeof(values));
  test_bit_put(mask, TEST_GPIO_PIN_HIGH, true);
  test_bit_put(values, TEST_GPIO_PIN_HIGH, true);
  test_bit_put(values, TEST_GPIO_PIN_FULL, true);

  rtems_test_assert(rtems_gpio_pin_set_multiple(fd, &map) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_HIGH) == 1);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);

  /*
   * And the caller's word count is a limit the driver works to, not a width
   * it must match.  One word covers pins 0 to 31, so pin 33 is not part of
   * this operation and keeps the level it already had -- even though its bit
   * is still set in a mask the caller has simply stopped describing.
   */
  map.word_count = 1;
  test_bit_put(values, TEST_GPIO_PIN_FULL, false);
  rtems_test_assert(rtems_gpio_pin_set_multiple(fd, &map) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_HIGH) == 1);
  map.word_count = TEST_GPIO_WORDS;

  /* More words than the controller has pins is still a caller that is wrong. */
  map.word_count = TEST_GPIO_WORDS + 1;
  assert_fails(rtems_gpio_pin_set_multiple(fd, &map), EINVAL);

  map.word_count = 0;
  assert_fails(rtems_gpio_pin_set_multiple(fd, &map), EINVAL);
  map.word_count = TEST_GPIO_WORDS;

  map.mask = NULL;
  assert_fails(rtems_gpio_pin_set_multiple(fd, &map), EINVAL);
  map.mask = mask;

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_HIGH) == 0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL2) == 0);
}

/*
 * Polarity and the bulk path are applied by different code, so exercising
 * each on its own leaves the case where they meet untested -- which is the
 * one a caller reaching for both at once actually runs.
 */
static void test_active_low_bulk(int fd) {
  rtems_gpio_config config;
  rtems_gpio_pin_bitmap map;
  uint32_t mask[TEST_GPIO_WORDS];
  uint32_t values[TEST_GPIO_WORDS];

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.flags = RTEMS_GPIO_FLAG_ACTIVE_LOW;
  config.initial_value = 0;

  rtems_test_assert(
      rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL2, &config) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 1);

  memset(mask, 0, sizeof(mask));
  memset(values, 0, sizeof(values));
  map.word_count = TEST_GPIO_WORDS;
  map.mask = mask;
  map.values = values;
  test_bit_put(mask, TEST_GPIO_PIN_FULL2, true);

  /* The pad is high, so the logical level a bulk read reports is 0. */
  rtems_test_assert(rtems_gpio_pin_get_multiple(fd, &map) == 0);
  rtems_test_assert(!test_bit_get(values, TEST_GPIO_PIN_FULL2));

  /* Writing a logical 1 through the bulk path drives the pad low. */
  test_bit_put(values, TEST_GPIO_PIN_FULL2, true);
  rtems_test_assert(rtems_gpio_pin_set_multiple(fd, &map) == 0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL2) == 0);

  /* And reads back as the 1 that was written, not as the pad. */
  memset(values, 0, sizeof(values));
  rtems_test_assert(rtems_gpio_pin_get_multiple(fd, &map) == 0);
  rtems_test_assert(test_bit_get(values, TEST_GPIO_PIN_FULL2));

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL2) == 0);
}

static uint32_t test_irq_pin;
static void* test_irq_arg;
static int test_irq_count;

static void test_irq_handler(uint32_t pin, void* arg) {
  test_irq_pin = pin;
  test_irq_arg = arg;
  ++test_irq_count;
}

static void test_interrupts(int fd) {
  rtems_gpio_config config;
  int token = 0;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.trigger = RTEMS_GPIO_TRIGGER_EDGE_BOTH;

  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);

  /* A handler is required, and a null one is caught here rather than at the
   * first edge. */
  assert_fails(rtems_gpio_pin_irq_enable(fd, TEST_GPIO_PIN_FULL,
                                         RTEMS_GPIO_IRQ_DEFAULT, NULL, NULL),
               EINVAL);

  /*
   * One bit is defined, so any other value is a caller built against a later
   * header.  Refused rather than ignored, which is the whole reason the
   * parameter is here: the first option to be defined has to be answerable.
   */
  assert_fails(rtems_gpio_pin_irq_enable(fd, TEST_GPIO_PIN_FULL, ~(uint32_t)0,
                                         test_irq_handler, &token),
               ENOTSUP);

  /*
   * And zero is refused too, which is the point of the bit not being zero: a
   * caller that left the field alone is asking for no option at all, not for
   * the one option there is, and is told so rather than being given it.
   */
  assert_fails(rtems_gpio_pin_irq_enable(fd, TEST_GPIO_PIN_FULL, 0,
                                         test_irq_handler, &token),
               ENOTSUP);

  rtems_test_assert(rtems_gpio_pin_irq_enable(fd, TEST_GPIO_PIN_FULL,
                                              RTEMS_GPIO_IRQ_DEFAULT,
                                              test_irq_handler, &token) == 0);

  /* And what was passed reached the driver rather than stopping above it. */
  rtems_test_assert(test_gpio_irq_flags(TEST_GPIO_PIN_FULL) ==
                    RTEMS_GPIO_IRQ_DEFAULT);

  test_irq_count = 0;
  test_gpio_fire_irq(TEST_GPIO_PIN_FULL);
  rtems_test_assert(test_irq_count == 1);
  rtems_test_assert(test_irq_pin == TEST_GPIO_PIN_FULL);
  rtems_test_assert(test_irq_arg == &token);

  rtems_test_assert(rtems_gpio_pin_irq_disable(fd, TEST_GPIO_PIN_FULL) == 0);

  test_gpio_fire_irq(TEST_GPIO_PIN_FULL);
  rtems_test_assert(test_irq_count == 1);

  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);
}

/*
 * A driver that implements only pin_get_info() is a legal driver.  Every
 * other operation answers ENOTSUP, and none of them calls through a null
 * pointer to get there.
 */
static void test_unimplemented(void) {
  rtems_gpio_config config;
  rtems_gpio_pin_info info;
  rtems_gpio_pin_bitmap map;
  uint32_t mask[TEST_GPIO_WORDS];
  uint32_t values[TEST_GPIO_WORDS];
  int fd;
  int value;

  fd = open(GPIO_MIN_PATH, O_RDWR);
  rtems_test_assert(fd >= 0);

  /* The one handler it has still works. */
  rtems_test_assert(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_FULL, &info) ==
                    0);

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;

  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               ENOTSUP);
  assert_fails(
      rtems_gpio_pin_get_configuration(fd, TEST_GPIO_PIN_FULL, &config),
      ENOTSUP);
  assert_fails(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL), ENOTSUP);
  assert_fails(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, &value), ENOTSUP);
  assert_fails(rtems_gpio_pin_set(fd, TEST_GPIO_PIN_FULL, 1), ENOTSUP);
  assert_fails(rtems_gpio_pin_toggle(fd, TEST_GPIO_PIN_FULL), ENOTSUP);
  assert_fails(rtems_gpio_pin_irq_disable(fd, TEST_GPIO_PIN_FULL), ENOTSUP);
  assert_fails(
      rtems_gpio_pin_irq_enable(fd, TEST_GPIO_PIN_FULL, RTEMS_GPIO_IRQ_DEFAULT,
                                test_irq_handler, NULL),
      ENOTSUP);

  memset(mask, 0, sizeof(mask));
  memset(values, 0, sizeof(values));
  map.word_count = TEST_GPIO_WORDS;
  map.mask = mask;
  map.values = values;
  test_bit_put(mask, TEST_GPIO_PIN_FULL, true);
  assert_fails(rtems_gpio_pin_set_multiple(fd, &map), ENOTSUP);
  assert_fails(rtems_gpio_pin_get_multiple(fd, &map), ENOTSUP);

  /* Something that is not one of ours. */
  assert_fails(ioctl(fd, _IOR('G', 200, int), &value), ENOTTY);

  /*
   * And a lookup by name on a descriptor that is not a controller at all
   * gives up at the first question rather than walking pins that do not
   * exist.
   */
  {
    uint32_t pin;
    int notgpio = open("/dev/console", O_RDWR);

    if (notgpio >= 0) {
      assert_fails(rtems_gpio_pin_by_name(notgpio, "LED0", &pin), ENOTTY);
      rtems_test_assert(close(notgpio) == 0);
    }
  }

  rtems_test_assert(close(fd) == 0);
}

static int test_gpio_stub_destroyed;

/*
 * Given to the controller below so that the registration failure path has a
 * destroy handler to call.  With a null one that branch is taken but the
 * call inside it never is, which is the difference between the check being
 * exercised and the thing it guards being exercised.
 */
static void test_gpio_stub_destroy(rtems_gpio_drv_ctrl* ctrl) {
  (void)ctrl;
  ++test_gpio_stub_destroyed;
}

static int test_gpio_stub_get_info(rtems_gpio_drv_ctrl* ctrl, uint32_t pin,
                                   rtems_gpio_pin_info* info) {
  (void)ctrl;
  (void)pin;
  (void)info;

  return 0;
}

/*
 * A controller that cannot answer what a pin is cannot be registered: every
 * check in the generic layer is built on that answer, and registering it
 * anyway would mean skipping them without saying so.
 */
/*
 * The guards in the convenience wrappers.  They are the only part of this
 * API an application can get wrong without the driver ever being asked:
 * each answers EINVAL and never reaches the ioctl, so a driver is never
 * handed a null to dereference.
 */
static void test_null_arguments(int fd) {
  uint32_t pin;
  int value;

  assert_fails(rtems_gpio_get_info(fd, NULL), EINVAL);
  assert_fails(rtems_gpio_pin_get_info(fd, TEST_GPIO_PIN_FULL, NULL), EINVAL);
  assert_fails(rtems_gpio_pin_by_name(fd, NULL, &pin), EINVAL);
  assert_fails(rtems_gpio_pin_by_name(fd, "LED0", NULL), EINVAL);
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, NULL), EINVAL);
  assert_fails(rtems_gpio_pin_get_configuration(fd, TEST_GPIO_PIN_FULL, NULL),
               EINVAL);
  assert_fails(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, NULL), EINVAL);
  (void)value;
}

/*
 * Every arm of the configuration-to-capability mapping.  The mapping runs
 * before the capability check, so an arm is exercised whether or not the
 * pin turns out to support what it asked for -- which is why the ones this
 * controller does not have are asserted to be refused rather than skipped.
 */
static void test_every_capability_arm(int fd) {
  rtems_gpio_config config;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.bias = RTEMS_GPIO_BIAS_PULL_DOWN;
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);

  /* Open source: the mapping runs, the pin does not have it. */
  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.drive = RTEMS_GPIO_DRIVE_OPEN_SOURCE;
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               ENOTSUP);

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.trigger = RTEMS_GPIO_TRIGGER_EDGE_FALLING;
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);

  /* Level triggers and wakeup: mapped, and not supported here. */
  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.trigger = RTEMS_GPIO_TRIGGER_LEVEL_LOW;
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               ENOTSUP);

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_INPUT;
  config.flags = RTEMS_GPIO_FLAG_WAKEUP;
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config),
               ENOTSUP);
}

/*
 * The two paths that only run when the driver itself fails, which cannot be
 * reached by arranging the arguments.  The first matters most: the generic
 * layer records a pin's polarity before it calls the driver, so a configure
 * that fails afterwards has to put that record back.  If it did not, the
 * next caller of the same pin would silently get inverted levels, and
 * nothing else in this file would notice.
 */
static void test_driver_failure_is_unwound(int fd) {
  rtems_gpio_config config;
  int value;

  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.flags = RTEMS_GPIO_FLAG_ACTIVE_LOW;
  config.initial_value = 1;

  test_gpio_fail_next_configure(EIO);
  assert_fails(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config), EIO);

  /* Now configure it active high and check the polarity did not survive. */
  memset(&config, 0, sizeof(config));
  config.direction = RTEMS_GPIO_DIRECTION_OUTPUT;
  config.initial_value = 1;
  rtems_test_assert(rtems_gpio_pin_configure(fd, TEST_GPIO_PIN_FULL, &config) ==
                    0);
  rtems_test_assert(test_gpio_raw_level(TEST_GPIO_PIN_FULL) == 1);
  rtems_test_assert(rtems_gpio_pin_get(fd, TEST_GPIO_PIN_FULL, &value) == 0);
  rtems_test_assert(value == 1);
  rtems_test_assert(rtems_gpio_pin_release(fd, TEST_GPIO_PIN_FULL) == 0);

  /* And a lookup by name gives up when the driver cannot describe a pin. */
  {
    uint32_t pin;

    test_gpio_fail_next_pin_get_info(EIO);
    assert_fails(rtems_gpio_pin_by_name(fd, "LED0", &pin), EIO);
  }
}

static void test_registration_is_checked(void) {
  static const rtems_gpio_drv_handlers no_info = {.pin_get_info = NULL};
  static const rtems_gpio_drv_handlers with_info = {
      .pin_get_info = test_gpio_stub_get_info,
      .destroy = test_gpio_stub_destroy};
  rtems_gpio_drv_ctrl ctrl;
  uint32_t words[1];

  memset(&ctrl, 0, sizeof(ctrl));
  rtems_test_assert(rtems_gpio_drv_ctrl_init(NULL) == EINVAL);

  ctrl.handlers = NULL;
  ctrl.pin_count = 4;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  ctrl.handlers = &no_info;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  /* A controller with no pins is not a controller. */
  ctrl.handlers = &no_info;
  ctrl.pin_count = 0;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  /*
   * And one that answers everything else but brought no storage for the
   * polarity the generic layer keeps on its behalf.  Refused rather than
   * accepted, because an active-low pin on such a controller would read
   * and drive the wrong way round with nothing to say why.
   */
  ctrl.handlers = &with_info;
  ctrl.pin_count = 4;
  ctrl.active_low = NULL;
  ctrl.scratch = words;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  ctrl.active_low = words;
  ctrl.scratch = NULL;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  /*
   * A table that answers everything, and no pins.  Reached only with a
   * usable pin_get_info, so the earlier no_info cases never get this far.
   */
  ctrl.handlers = &with_info;
  ctrl.pin_count = 0;
  ctrl.active_low = words;
  ctrl.scratch = words;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == EINVAL);

  /*
   * Registering over a path that already exists fails, and the failure path
   * is where the controller gives its own storage back -- the driver's
   * destroy handler and the lock this init just created.
   */
  ctrl.pin_count = 4;
  rtems_test_assert(rtems_gpio_drv_ctrl_init(&ctrl) == 0);
  rtems_test_assert(rtems_gpio_drv_ctrl_register(&ctrl, GPIO_PATH) != 0);
  rtems_test_assert(test_gpio_stub_destroyed == 1);
}

static void run_test(void) {
  rtems_resource_snapshot snapshot;
  int fd;
  int warm_up;

  rtems_test_assert(test_gpio_register(GPIO_PATH) == 0);
  rtems_test_assert(test_gpio_register_minimal(GPIO_MIN_PATH) == 0);

  /*
   * Both nodes are opened and closed once before the baseline is taken.
   * The first open of an IMFS node allocates things it keeps deliberately,
   * and counting those as a leak reports a failure that is not one.
   */
  warm_up = open(GPIO_PATH, O_RDWR);
  rtems_test_assert(warm_up >= 0);
  rtems_test_assert(close(warm_up) == 0);
  warm_up = open(GPIO_MIN_PATH, O_RDWR);
  rtems_test_assert(warm_up >= 0);
  rtems_test_assert(close(warm_up) == 0);

  rtems_resource_snapshot_take(&snapshot);

  fd = open(GPIO_PATH, O_RDWR);
  rtems_test_assert(fd >= 0);

  test_controller_info(fd);
  test_pin_info(fd);
  test_pin_by_name(fd);
  test_reserved_pin_is_refused(fd);
  test_capabilities_are_enforced(fd);
  test_configure_and_report(fd);
  test_rounding_is_reported(fd);
  test_read_write(fd);
  test_active_low(fd);
  test_multiple(fd);
  test_active_low_bulk(fd);
  test_null_arguments(fd);
  test_every_capability_arm(fd);
  test_driver_failure_is_unwound(fd);
  test_interrupts(fd);
  test_unimplemented();
  test_registration_is_checked();

  rtems_test_assert(close(fd) == 0);

  /*
   * Covers rather more than the heap: a descriptor left open by
   * test_unimplemented(), or a mutex left behind by a controller that
   * failed to register, shows up here and in no other assertion in this
   * file.
   */
  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  /*
   * Destroying the nodes comes last, and after the snapshot rather than
   * before it: the nodes existed when the baseline was taken, so removing
   * them legitimately drops below it and the check would fail for a reason
   * that is not a leak.  This is the one path nothing else here reaches --
   * where the driver's destroy handler runs and the controller lock is
   * destroyed.
   */
  rtems_test_assert(test_gpio_destroyed() == 0);
  rtems_test_assert(unlink(GPIO_PATH) == 0);
  rtems_test_assert(test_gpio_destroyed() == 1);
  rtems_test_assert(unlink(GPIO_MIN_PATH) == 0);

  /* And it is gone, rather than merely unlinked from the directory. */
  rtems_test_assert(open(GPIO_PATH, O_RDWR) == -1);
}

static rtems_task Init(rtems_task_argument arg) {
  (void)arg;

  TEST_BEGIN();

  run_test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 8

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
