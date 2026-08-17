/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief This source file contains the x86/HVM direct boot support.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <acpi/acpi.h>
#include <acpi/acpica/acpi.h>
#include <pvh.h>

#define PVH_START_INFO_MAGIC 0x336ec578

/* Boot information of the x86/HVM direct boot ABI */
typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t flags;
  uint32_t nr_modules;
  uint64_t modlist_paddr;
  uint64_t cmdline_paddr;
  uint64_t rsdp_paddr;
  uint64_t memmap_paddr;
  uint32_t memmap_entries;
  uint32_t reserved;
} pvh_start_info;

void pvh_retrieve_boot_information( void )
{
  const pvh_start_info *info;
  ACPI_PHYSICAL_ADDRESS rsdp;

  if ( pvh_start_info_address == 0 ) {
    return;
  }

  info = (const pvh_start_info *) (uintptr_t) pvh_start_info_address;

  if ( info->magic != PVH_START_INFO_MAGIC ) {
    return;
  }

  if ( info->rsdp_paddr != 0 ) {
    acpi_rsdp_addr = info->rsdp_paddr;
    return;
  }

  /*
   * The boot information carries no table location, for example because the
   * Qemu PVH loader failed to locate it.  Search the standard areas.
   */
  rsdp = 0;

  if ( ACPI_SUCCESS( AcpiFindRootPointer( &rsdp ) ) ) {
    acpi_rsdp_addr = rsdp;
  }
}
