/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 Jan Sommer, German Aerospace Center (DLR)
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

#ifndef LIBBSP_ARM_XILINX_AXI_SPI_H
#define LIBBSP_ARM_XILINX_AXI_SPI_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * Register Xilinx AXI SPI device
 *
 * Note:
 *   The Xilinx Quad SPI device is very versatile and
 *   supports many options. This driver assumes the 
 *   following setup:
 *     - Standard SPI mode and AXI Lite interface
 *     - FIFO available (driver might also work without FIFOs)
 *
 * @param bus_path path for the new device node (e.g. "/dev/spi0")
 * @param register_base base address of the device
 * @param fifo_size Configured fifo size. Either 0, 16 or 256
 * @param num_cs Number of configured CS lines (0-32)
 *
 * @return 0 on success. Negative number otherwise.
 *
 */
int spi_bus_register_xilinx_axi(
  const char *bus_path,
  uintptr_t register_base,
  uint32_t fifo_size,
  uint32_t num_cs,
  rtems_vector_number irq
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_AXI_SPI_H */
