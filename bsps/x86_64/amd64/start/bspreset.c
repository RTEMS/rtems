/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @ingroup RTEMSBSPsX8664AMD64EFI
 *
 * @brief BSP reset code
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
#include <bsp/bootcard.h>

#define KEYBOARD_CONTROLLER_PORT 0x64
#define PULSE_RESET_LINE         0xFE

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

  ACPI_STATUS status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);

  if (status == AE_OK) {
    amd64_disable_interrupts();
    AcpiEnterSleepState(ACPI_STATE_S5);
  }

  /* Should be unreachable. As a fallback try the keyboard controller method */
  outport_byte(KEYBOARD_CONTROLLER_PORT, PULSE_RESET_LINE);
  RTEMS_UNREACHABLE();
}
