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
#include <efilib.h>

#include <rtems/bspIo.h>

#include <efistop.h>

#include <stdio.h>
#include <string.h>

#ifdef BSP_USE_EFI_BOOT_SERVICES

static void
print_stop_info(SIMPLE_TEXT_OUTPUT_INTERFACE* stop);

void
efi_init_text_output(int hint)
{
    int maxval = ST->ConOut->Mode->MaxMode - 1;
    int curval = ST->ConOut->Mode->Mode;
    ST->ConOut->Reset(ST->ConOut, true);
    if (hint != -1) {
        /* hint got from command-line does have highest priority */
        ST->ConOut->SetMode(ST->ConOut, hint);
    }
    else if (strcmp(BSP_EFI_SIMPLE_TEXT_OUTPUT_MODE_VALUE, "MAX") == 0) {
        ST->ConOut->SetMode(ST->ConOut, maxval);
    }
    else if (strcmp(BSP_EFI_SIMPLE_TEXT_OUTPUT_MODE_VALUE, "AUTO") == 0) {
        ST->ConOut->SetMode(ST->ConOut, curval);
    }
    else {
        ST->ConOut->SetMode(ST->ConOut, atoi(BSP_EFI_SIMPLE_TEXT_OUTPUT_MODE_VALUE));
    }
    ST->ConOut->EnableCursor(ST->ConOut, TRUE);
    print_stop_info(ST->ConOut);
    ST->ConOut->ClearScreen(ST->ConOut);
}

void
efi_text_output_char(char c)
{
    EFI_STATUS status;
    char tocpy[2];
    tocpy[0] = c;
    tocpy[1] = 0;
    CHAR16 str[2];
    cpy8to16(tocpy, str, 2);
    status = ST->ConOut->TestString(ST->ConOut, str);
    if (EFI_ERROR(status))
        str[0] = '?';
    ST->ConOut->OutputString(ST->ConOut, str);
}

static void
print_stop_info(SIMPLE_TEXT_OUTPUT_INTERFACE* stop)
{
    EFI_STATUS status;
    int max_mode = stop->Mode->MaxMode;
    int current_mode = stop->Mode->Mode;
    printf("RTEMS: text output: current mode: %d, max mode: %d\n", current_mode, (max_mode - 1));
    UINTN columns = 0;
    UINTN rows = 0;
    for (int i = 0; i < max_mode; i++) {
        if (current_mode == i)
            printf(" -> ");
        else
            printf("    ");
        status = stop->QueryMode(stop, i, &columns, &rows);
        if (EFI_ERROR(status))
            printf("%d. mode: error: can't obtain column x row values.\n", i);
        else {
            printf("%d. mode: %ld * %ld\n", i, columns, rows);
        }            
    }
}

#endif
