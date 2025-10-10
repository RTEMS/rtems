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

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

/**
 * TODO: This routine will need to be implemented before using features such as:
 * General Purpose Events, Notify Events, ACPICA Debugger...
 */
ACPI_STATUS AcpiOsExecute(
  ACPI_EXECUTE_TYPE Type,
  ACPI_OSD_EXEC_CALLBACK Function,
  void* Context
)
{
#ifdef __rtems__
    (void) Type;
    (void) Function;
    (void) Context;
#endif
  return (AE_SUPPORT);
}

void AcpiOsSleep(UINT64 Milliseconds)
{
  usleep(Milliseconds * 1000);
}


void AcpiOsStall(UINT32 Microseconds)
{
  struct timeval tv;
  struct timeval stall_tv;

  gettimeofday(&stall_tv, NULL);
  stall_tv.tv_sec += Microseconds / 1000000;
  stall_tv.tv_usec += Microseconds % 1000000;

  do {
    gettimeofday(&tv, NULL);
  } while (tv.tv_sec < stall_tv.tv_sec ||
          (tv.tv_sec == stall_tv.tv_sec && tv.tv_usec < stall_tv.tv_usec));
}

ACPI_THREAD_ID AcpiOsGetThreadId(void)
{
  return (ACPI_THREAD_ID) pthread_self();
}
