/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @ingroup RTEMSBSPsX8664AMD64EFI
 *
 * @brief ACPI header file
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

#ifndef _AMD64_ACPI_H_
#define _AMD64_ACPI_H_

#include <bsp.h>
#include <acpi/acpica/acpi.h>

#define ACPI_MAX_INIT_TABLES 16

extern uint64_t acpi_rsdp_addr;

/**
 * @brief Initializes the ACPICA Table Manager. Requires dynamic memory.
 *
 * The ACPICA Table Manager is independent of the rest of the ACPICA subsystem
 * and only requires dynamic memory to be initialized (unless a statically
 * allocated array for the tables is provided). Since access to ACPI tables
 * may be required before the entire ACPICA subsystem can be initialized
 * this routine can be used earlier in the system intialization.
 *
 * @return true if successful.
 */
bool acpi_tables_initialize(void);

/**
 * @brief Walks through the subtables of an ACPI table.
 *
 * @param table Pointer to the table.
 * @param size_of_header Size of the table header (used to offset into first subtable).
 * @param handler Handler that is called for each subtable found.
 */
void acpi_walk_subtables(
  ACPI_TABLE_HEADER *table,
  size_t size_of_header,
  void (*handler)(ACPI_SUBTABLE_HEADER*)
);

#endif /* _AMD64_ACPI_H_ */
