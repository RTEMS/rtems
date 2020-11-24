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

#include <rtems/sysinit.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/fdt.h>
#include <bsp/imx-iomux.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/flash-headers.h>
#include <fsl/edma.h>

#include <fsl_clock.h>
#include <libfdt.h>

uint32_t imxrt_systick_frequency(void)
{
  return CLOCK_GetCpuClkFreq();
}

static void imxrt_disable_wait_mode(void)
{
  /*
   * Prevent processor from entering WAIT or SLEEP mode when a WFI is executed.
   * This would switch off the normal interrupt controller and activate an
   * alternative one. See "i.MX RT1050 Reference Manual, Rev. 4, 12/2019"
   * chapter 14.6.3.2.1 "Entering WAIT mode".
   *
   * FIXME: For saving energy it would be a better solution to support the
   * alternative interrupt controller. But that makes a bit of work necessary on
   * every WFI.
   */
  CLOCK_SetMode(kCLOCK_ModeRun);
}

void bsp_start(void)
{
  imxrt_disable_wait_mode();

  bsp_interrupt_initialize();
  rtems_cache_coherent_add_area(
    bsp_section_nocacheheap_begin,
    (uintptr_t) bsp_section_nocacheheap_size
  );
}

const void *bsp_fdt_get(void)
{
  return imxrt_dtb;
}

bool imxrt_fdt_node_is_enabled(const void *fdt, int node)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "status", &len);
  if (val != NULL &&
      (strcmp((char*)val, "ok") == 0 || strcmp((char*)val, "okay") == 0)) {
    return true;
  }

  return false;
}

void *imx_get_reg_of_node(const void *fdt, int node)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "reg", &len);
  if (val == NULL || len < 4) {
    return NULL;
  }

  return (void *) fdt32_to_cpu(val[0]);
}

rtems_vector_number imx_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
)
{
  int len;
  const uint32_t *val;

  val = fdt_getprop(fdt, node, "interrupts", &len);
  if (val == NULL || len < (int) ((index) * 4)) {
    return BSP_INTERRUPT_VECTOR_INVALID;
  }

  return fdt32_to_cpu(val[index]);
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[0];
}

/* Make sure to pull in the flash headers */
__attribute__((used)) static const void *hdr_dcd = &imxrt_dcd_data;
__attribute__((used)) static const void *hdr_ivt = &imxrt_image_vector_table;
__attribute__((used)) static const void *hdr_btd = &imxrt_boot_data;
__attribute__((used)) static const void *hdr_fsc = &imxrt_flexspi_config;

/* pull in some drivers */
__attribute__((used)) static const void *drv_iomux = &imx_iomux_configure_pins;

RTEMS_SYSINIT_ITEM(imxrt_lpspi_init, RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_MIDDLE);
RTEMS_SYSINIT_ITEM(imxrt_lpi2c_init, RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_MIDDLE);
RTEMS_SYSINIT_ITEM(imxrt_ffec_init, RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_MIDDLE);
RTEMS_SYSINIT_ITEM(fsl_edma_init, RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_FIRST);
