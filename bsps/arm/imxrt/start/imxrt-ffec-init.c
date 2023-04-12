/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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
#include <bsp/fdt.h>
#include <bsp/imx-gpio.h>
#include <chip.h>
#include <fsl_clock.h>
#include <libfdt.h>
#include <rtems/counter.h>

void imxrt_ffec_init(void)
{
  volatile IOMUXC_GPR_Type *iomuxc_gpr = IOMUXC_GPR;
  const void *fdt;
  int node;

  fdt = bsp_fdt_get();

#if IMXRT_IS_MIMXRT10xx
  const clock_enet_pll_config_t config = {
    .enableClkOutput = true,
    .enableClkOutput25M = false,
    .loopDivider = 1
  };

  CLOCK_InitEnetPll(&config);

  iomuxc_gpr->GPR1 |= IOMUXC_GPR_GPR1_ENET_REF_CLK_DIR_MASK;
#else
  iomuxc_gpr->GPR4 |= IOMUXC_GPR_GPR4_ENET_REF_CLK_DIR_MASK;
#endif

  node = fdt_node_offset_by_compatible(fdt, -1, "fsl,imxrt-fec");
  if (node >= 0) {
    struct imx_gpio_pin reset;
    struct imx_gpio_pin interrupt;
    rtems_status_code sc;

    sc = imx_gpio_init_from_fdt_property(
        &reset, node, "phy-reset-gpios",
        IMX_GPIO_MODE_OUTPUT, 0);

    if (sc == RTEMS_SUCCESSFUL) {
      sc = imx_gpio_init_from_fdt_property(
          &interrupt, node, "rtems,phy-interrupt-gpios",
          IMX_GPIO_MODE_INPUT, 0);

      imx_gpio_set_output(&reset, 0);
      if (sc == RTEMS_SUCCESSFUL) {
        /* Force interrupt GPIO to high. Otherwise we
         * get NAND_TREE mode of the PHY. */
        interrupt.mode = IMX_GPIO_MODE_OUTPUT;
        imx_gpio_init(&interrupt);
        imx_gpio_set_output(&interrupt, 1);
      }
      rtems_counter_delay_nanoseconds(100000);
      imx_gpio_set_output(&reset, 1);
      rtems_counter_delay_nanoseconds(5);
      if (sc == RTEMS_SUCCESSFUL) {
        interrupt.mode = IMX_GPIO_MODE_INPUT;
        imx_gpio_init(&interrupt);
      }
    }
  }
}
