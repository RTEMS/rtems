/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @ingroup RTEMSBSPsX8664AMD64EFI
 *
 * @brief ACPICA OS Services Layer interfaces
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void)
{
  return acpi_rsdp_addr;
}

ACPI_STATUS AcpiOsTableOverride(
  ACPI_TABLE_HEADER* ExistingTable,
  ACPI_TABLE_HEADER** NewTable
)
{
#ifdef __rtems__
    (void) ExistingTable;
    (void) NewTable;
#endif
  return (AE_SUPPORT);
}

ACPI_STATUS AcpiOsPhysicalTableOverride(
  ACPI_TABLE_HEADER* ExistingTable,
  ACPI_PHYSICAL_ADDRESS* NewAddress,
  UINT32* NewTableLength
)
{
#ifdef __rtems__
    (void) ExistingTable;
    (void) NewAddress;
    (void) NewTableLength;
#endif
  return (AE_SUPPORT);
}
