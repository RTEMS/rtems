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
#include <rtems/score/cpu.h>

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS InPort, UINT32* Value, UINT32 Width)
{
  switch (Width) {
    case 8:
      *Value = inport_byte((uint16_t) InPort);
      break;
    case 16:
      *Value = inport_word((uint16_t) InPort);
      break;
    case 32:
      *Value = inport_long((uint16_t) InPort);
      break;
    default:
      return (AE_BAD_PARAMETER);
  }

  return (AE_OK);
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS OutPort, UINT32 Value, UINT32 Width)
{
  switch (Width) {
    case 8:
      outport_byte((uint16_t) OutPort, (uint8_t) Value);
      break;
    case 16:
      outport_word((uint16_t) OutPort, (uint16_t) Value);
      break;
    case 32:
      outport_long((uint16_t) OutPort, Value);
      break;
    default:
      return (AE_BAD_PARAMETER);
  }

  return (AE_OK);
}

ACPI_STATUS AcpiOsReadPciConfiguration(
  ACPI_PCI_ID *PciId,
  UINT32 Register,
  UINT64 *Value,
  UINT32 Width
)
{
#ifdef __rtems__
    (void) PciId;
    (void) Register;
    (void) Value;
    (void) Width;
#endif
  return (AE_SUPPORT);
}

ACPI_STATUS AcpiOsWritePciConfiguration(
  ACPI_PCI_ID *PciId,
  UINT32 Register,
  UINT64 Value,
  UINT32 Width
)
{
#ifdef __rtems__
    (void) PciId;
    (void) Register;
    (void) Value;
    (void) Width;
#endif
  return (AE_SUPPORT);
}
