/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @ingroup RTEMSBSPsX8664AMD64EFI
 *
 * @brief ACPI implementation
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

#include <assert.h>
#include <acpi/acpi.h>
#include <rtems/sysinit.h>

uint64_t acpi_rsdp_addr = 0;

static bool acpi_tables_initialized = false;

bool acpi_tables_initialize(void)
{
  ACPI_STATUS status;

  status = AcpiInitializeTables(NULL, ACPI_MAX_INIT_TABLES, FALSE);

  if (status == (AE_OK)) {
    acpi_tables_initialized = true;
    return true;
  }
  return false;
}

void acpi_walk_subtables(
  ACPI_TABLE_HEADER* table,
  size_t size_of_header,
  void (*handler)(ACPI_SUBTABLE_HEADER*)
)
{
  ACPI_SUBTABLE_HEADER* entry;
  ACPI_SUBTABLE_HEADER* end;

  if (table == NULL) {
    return;
  }

  entry = (ACPI_SUBTABLE_HEADER*) ((uint8_t*) table + size_of_header);
  end = (ACPI_SUBTABLE_HEADER*) ((uint8_t*) table + table->Length);

  while (entry < end) {
    handler(entry);
    entry = (ACPI_SUBTABLE_HEADER*) ((uint8_t*) entry + entry->Length);
  }
}

static void initialize_acpi(void)
{
  ACPI_STATUS status;
  status = AcpiInitializeSubsystem();
  assert(status == (AE_OK));

  if (acpi_tables_initialized == false) {
    status = AcpiInitializeTables(NULL, ACPI_MAX_INIT_TABLES, FALSE);
    assert(status == (AE_OK));
  }

  status = AcpiLoadTables();
  assert(status == (AE_OK));

  /* System Control Interrupts not supported */
  status = AcpiEnableSubsystem(ACPI_NO_HANDLER_INIT);
  assert(status == (AE_OK));

  /* General Purpose Events not supported */
  status = AcpiInitializeObjects(ACPI_NO_EVENT_INIT);
  assert(status == (AE_OK));
}

RTEMS_SYSINIT_ITEM(
  initialize_acpi,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
