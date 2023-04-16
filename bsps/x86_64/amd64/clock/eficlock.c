/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 Karel Gardas
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

#include <bsp.h>
#include <efi.h>

#include <stdio.h>

extern EFI_BOOT_SERVICES *BS;

static EFI_EVENT clock_event = 0;

extern void
Clock_isr( void* );

#ifndef EFIAPI
#error "EFIAPI not defined!"
#endif


/* no-sse attribute helps with enforcing GCC to generate code which does not use
   SSE instructions requiring 16 byte alligned access on unaligned data hence
   producing perfect GP fault. The code with SSE enabled looks:

EFIAPI void
efi_clock_tick_notify(EFI_EVENT e, VOID* ctx)
{
  10b2ba:       55                      push   rbp
  10b2bb:       48 89 e5                mov    rbp,rsp
  10b2be:       57                      push   rdi
  10b2bf:       56                      push   rsi
  10b2c0:       48 81 ec a0 00 00 00    sub    rsp,0xa0
  10b2c7:       8b 02                   mov    eax,DWORD PTR [rdx]
->10b2c9:       0f 29 b5 50 ff ff ff    movaps XMMWORD PTR [rbp-0xb0],xmm6
  10b2d0:       0f 29 bd 60 ff ff ff    movaps XMMWORD PTR [rbp-0xa0],xmm7
  10b2d7:       83 c0 01                add    eax,0x1

and we get GP @ 10b2c9.

CAVEAT: This function is to be called from the UEFI which means it needs to callable
by using MS ABI!

 */
__attribute__((target("no-sse")))
EFIAPI void
efi_clock_tick_notify(EFI_EVENT e, VOID* ctx);

__attribute__((target("no-sse")))
EFIAPI void
efi_clock_tick_notify(EFI_EVENT e, VOID* ctx)
{
    Clock_isr(NULL);
}

static void
efi_clock_initialize( void )
{
    EFI_STATUS status = BS->CreateEvent
        (EVT_TIMER | EVT_NOTIFY_SIGNAL,
         TPL_CALLBACK, (EFI_EVENT_NOTIFY)efi_clock_tick_notify,
         (VOID*)NULL, &clock_event);
    if (EFI_ERROR(status)) {
        printf("EFI: error while creating event in clock initialization!\n");
    }
    else {
        printf("EFI: clock event created.\n");
        /* set periodic timer to signal Clock_isr every 10 milisecond. Value provided
           here is following UEFI spec in hundred of nanoseconds. */
        status = BS->SetTimer(clock_event, TimerPeriodic, (10 * 1000 * 10));
        if (EFI_ERROR(status)) {
            printf("EFI: error while creating timer in clock initialization!\n");
            BS->CloseEvent(clock_event);
        }
        else {
            printf("EFI: timer for clock event is set.\n");
        }
    }
}

#define Clock_driver_support_initialize_hardware() efi_clock_initialize()

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
