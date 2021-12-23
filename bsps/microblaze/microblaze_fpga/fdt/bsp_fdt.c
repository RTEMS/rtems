/*
 *  COPYRIGHT (c) 2021.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fdt.h>

#include BSP_MICROBLAZE_FPGA_DTB_HEADER_PATH

const void *bsp_fdt_get(void)
{
  return system_dtb;
}

uint32_t bsp_fdt_map_intr(const uint32_t *intr, size_t icells)
{
  return intr[0];
}
