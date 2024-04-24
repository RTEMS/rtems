/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceIMXSPIGPIO
 *
 * @brief This file provides the implementation of @ref RTEMSDeviceIMXSPIGPIO.
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

#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/imx-gpio.h>

#include <dev/spi/spi.h>
#include <libfdt.h>
#include <dev/spi/spi-gpio.h>
#include <dev/spi/imx-spi-gpio.h>

struct imx_spi_gpio_bus {
  spi_bus base;
  struct spi_gpio_params p;

  struct imx_gpio_pin sck;
  struct imx_gpio_pin mosi;
  struct imx_gpio_pin miso;
  struct imx_gpio_pin cs[SPI_GPIO_MAX_CS];
};

static void imx_spi_gpio_set(void *arg, bool level)
{
  struct imx_gpio_pin *pin = arg;
  imx_gpio_set_output(pin, level ? 1 : 0);
}

static bool imx_spi_gpio_get(void *arg)
{
  struct imx_gpio_pin *pin = arg;
  return (imx_gpio_get_input(pin) != 0);
}

rtems_status_code imx_spi_gpio_init(void)
{
  const void *fdt;
  int node;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  fdt = bsp_fdt_get();
  node = -1;

  do {
    node = fdt_node_offset_by_compatible(fdt, node, "spi-gpio");

    if (node >= 0 && imxrt_fdt_node_is_enabled(fdt, node)) {
      struct imx_spi_gpio_bus *bus;
      const char *bus_path;
      const uint32_t *val;
      int num_cs;

      if (sc == RTEMS_SUCCESSFUL) {
        bus = (struct imx_spi_gpio_bus*) spi_bus_alloc_and_init(sizeof(*bus));
        if (bus == NULL) {
          sc = RTEMS_UNSATISFIED;
        }
      }

      if (sc == RTEMS_SUCCESSFUL) {
        bus_path = fdt_getprop(fdt, node, "rtems,path", NULL);
        if (bus_path == NULL) {
          sc = RTEMS_UNSATISFIED;
        }
      }

      if (sc == RTEMS_SUCCESSFUL) {
        sc = imx_gpio_init_from_fdt_property(
            &bus->sck, node, "sck-gpios", IMX_GPIO_MODE_OUTPUT, 0);
        bus->p.set_clk = imx_spi_gpio_set;
        bus->p.set_clk_arg = &bus->sck;
      }

      if (sc == RTEMS_SUCCESSFUL) {
        sc = imx_gpio_init_from_fdt_property(
            &bus->miso, node, "miso-gpios", IMX_GPIO_MODE_INPUT, 0);
        bus->p.get_miso = imx_spi_gpio_get;
        bus->p.get_miso_arg = &bus->miso;
      }

      if (sc == RTEMS_SUCCESSFUL) {
        sc = imx_gpio_init_from_fdt_property(
            &bus->mosi, node, "mosi-gpios", IMX_GPIO_MODE_OUTPUT, 0);
        bus->p.set_mosi = imx_spi_gpio_set;
        bus->p.set_mosi_arg = &bus->mosi;
      }

      if (sc == RTEMS_SUCCESSFUL) {
        val = fdt_getprop(fdt, node, "num-chipselects", NULL);
        if (val != NULL) {
          ssize_t i;

          num_cs = fdt32_to_cpu(val[0]);

          for (i = 0; i < num_cs && sc == RTEMS_SUCCESSFUL; ++i) {
            sc = imx_gpio_init_from_fdt_property(
                &bus->cs[i], node, "cs-gpios", IMX_GPIO_MODE_OUTPUT, 0);
            bus->p.set_cs[i] = imx_spi_gpio_set;
            bus->p.set_cs_arg[i] = &bus->cs[i];
            bus->p.cs_idle[i] = (imx_gpio_get_active_level(&bus->cs[i]) == 0);
          }
        }
      }

      if (sc == RTEMS_SUCCESSFUL) {
        sc = spi_gpio_init(bus_path, &bus->p);
      }

      if (sc != RTEMS_SUCCESSFUL && bus != NULL) {
        spi_bus_destroy_and_free(&bus->base);
      }
    }
  } while (node >= 0 && sc == RTEMS_SUCCESSFUL);

  return sc;
}
