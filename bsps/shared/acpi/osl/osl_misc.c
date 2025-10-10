/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <stdio.h>
#include <sys/time.h>

UINT64 AcpiOsGetTimer(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 10000000) + (tv.tv_usec * 10);
}

ACPI_STATUS AcpiOsSignal(UINT32 Function, void* Info)
{
  switch (Function) {
  case ACPI_SIGNAL_FATAL:
    ACPI_SIGNAL_FATAL_INFO* fatal = (ACPI_SIGNAL_FATAL_INFO*) Info;
    printf("ACPI: Fatal signal, type 0x%x code 0x%x argument 0x%x",
      fatal->Type, fatal->Code, fatal->Argument);
    break;
  case ACPI_SIGNAL_BREAKPOINT:
    break;
  default:
    return (AE_BAD_PARAMETER);
  }

  return (AE_OK);
}

ACPI_STATUS AcpiOsEnterSleep(
  UINT8 SleepState,
  UINT32 RegaValue,
  UINT32 RegbValue
)
{
#ifdef __rtems__
    (void) SleepState;
    (void) RegaValue;
    (void) RegbValue;
#endif
  return (AE_OK);
}
