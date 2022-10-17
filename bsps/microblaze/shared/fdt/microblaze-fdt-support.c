/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <bspopts.h>
#include <bsp/microblaze-fdt-support.h>
#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
#include <bsp/fdt.h>

#include <libfdt.h>

#include BSP_MICROBLAZE_FPGA_DTB_HEADER_PATH

const void *bsp_fdt_get(void)
{
  return system_dtb;
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[0];
}
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

uint32_t try_get_prop_from_device_tree(
  const char *compatible,
  const char *prop_name,
  uint32_t default_value
)
{
  uint32_t value = default_value;

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
  const void *fdt = bsp_fdt_get();
  int node = fdt_node_offset_by_compatible( fdt, -1, compatible );
  if ( node < 0 ) {
    return default_value;
  }

  const uint32_t *prop = fdt_getprop( fdt, node, prop_name, NULL );
  if ( prop == NULL ) {
    return default_value;
  }

  value = fdt32_to_cpu( prop[0] );
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

   return value;
}
