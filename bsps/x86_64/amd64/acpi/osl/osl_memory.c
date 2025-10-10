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

#include <acpi/acpica/acpi.h>

#include <stdint.h>

void* AcpiOsAllocate(ACPI_SIZE Size)
{
  return malloc(Size);
}

void AcpiOsFree(void* Memory)
{
  free(Memory);
}

void* AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE Length)
{
#ifdef __rtems__
    (void) Length;
#endif
  /* We have an identity map set up, simply return the address */
  return (void*) PhysicalAddress;
}

void AcpiOsUnmapMemory(void* LogicalAddress, ACPI_SIZE Length)
{
#ifdef __rtems__
    (void) LogicalAddress;
    (void) Length;
#endif
  /* We have an identity map set up, do nothing */
  return;
}

ACPI_STATUS AcpiOsGetPhysicalAddress(
  void* LogicalAddress,
  ACPI_PHYSICAL_ADDRESS* PhysicalAddress
)
{
  /* We have an identity map set up, simply return the address */
  *PhysicalAddress = (uint64_t) LogicalAddress;
  return (AE_OK);
}

BOOLEAN AcpiOsReadable(void* Pointer, ACPI_SIZE Length)
{
#ifdef __rtems__
    (void) Pointer;
    (void) Length;
#endif
  return (TRUE);
}

BOOLEAN AcpiOsWritable(void* Pointer, ACPI_SIZE Length)
{
#ifdef __rtems__
    (void) Pointer;
    (void) Length;
#endif
  return (TRUE);
}

ACPI_STATUS AcpiOsReadMemory(
  ACPI_PHYSICAL_ADDRESS Address,
  UINT64* Value,
  UINT32 Width
)
{
  /* We have an identity map set up, simply use the physical address */
  switch (Width) {
  case 8:
    *Value = *(volatile uint8_t*) Address;
    break;
  case 16:
    *Value = *(volatile uint16_t*) Address;
    break;
  case 32:
    *Value = *(volatile uint32_t*) Address;
    break;
  case 64:
    *Value = *(volatile uint64_t*) Address;
    break;
  }

  return (AE_OK);
}

ACPI_STATUS AcpiOsWriteMemory(
  ACPI_PHYSICAL_ADDRESS Address,
  UINT64 Value,
  UINT32 Width
)
{
  /* We have an identity map set up, simply use the physical address */
  switch (Width) {
  case 8:
    *(volatile uint8_t*) Address = (uint8_t) Value;
    break;
  case 16:
    *(volatile uint16_t*) Address = (uint16_t) Value;
    break;
  case 32:
    *(volatile uint32_t*) Address = (uint32_t) Value;
    break;
  case 64:
    *(volatile uint64_t*) Address = (uint64_t) Value;
    break;
  }

  return (AE_OK);
}
