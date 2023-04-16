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

#include <rtems/bspIo.h>

#include <stdio.h>
#include <string.h>

#include <efistop.h>
#include <efigop.h>

#include <multiboot2impl.h>

#ifdef BSP_USE_EFI_BOOT_SERVICES

extern EFI_HANDLE               IH;
extern EFI_SYSTEM_TABLE         *ST;
extern EFI_BOOT_SERVICES        *BS;
extern EFI_RUNTIME_SERVICES     *RS;

extern void uart0_output_char(char c);

static bool is_efi_console_initialized = false;

static char output_buffer[4096];
static int output_buffer_index = 0;

void
efi_console_initialize(void);

EFI_STATUS
print_gop_info(EFI_GRAPHICS_OUTPUT *gop);

EFI_STATUS
check_gop(BOOLEAN verbose);

static void
both_output_char(char c);

static void
sync_output(void);

void
print_ccm_info(EFI_CONSOLE_CONTROL_SCREEN_MODE, BOOLEAN, BOOLEAN);

void
print_ccm_info(EFI_CONSOLE_CONTROL_SCREEN_MODE mode, BOOLEAN graphics, BOOLEAN locked)
{
    printf("RTEMS: EFI console default mode: ");
    switch (mode) {
    case EfiConsoleControlScreenText:
        printf("text");
        break;
    case EfiConsoleControlScreenGraphics:
        printf("graphics");
        break;
    case EfiConsoleControlScreenMaxValue:
        printf("max value");
        break;
    }
    if (graphics) {
        printf(", graphics is available");
    }
    if (locked) {
        printf(", stdin is locked");
    }
    printf("\n");
}

void
efi_console_initialize( void )
{
    EFI_STATUS status;
    EFI_GUID ConsoleControlGUID = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
    EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
    EFI_HANDLE *HandleBuffer = NULL;
    UINTN HandleCount = 0;
    BOOLEAN graphics_exists;
    BOOLEAN locked_stdin;
    EFI_CONSOLE_CONTROL_SCREEN_MODE mode;
    BOOLEAN use_text = false;
    BOOLEAN use_graphic = false;
    BOOLEAN use_auto = false;
    int text_mode = -1;
    int graphic_mode = -1;
 
    if (is_efi_console_initialized)
        return;
    if (ST == NULL)
        return;

    /* try hard to obtain console control */
    status = ST->BootServices->LocateProtocol(&ConsoleControlGUID, NULL,
                                              (VOID **)&ConsoleControl);
    if (EFI_ERROR(status)) {
        status = ST->BootServices->HandleProtocol( ST->ConsoleOutHandle,
                                                   &ConsoleControlGUID,
                                                   (VOID **)&ConsoleControl);
        if (EFI_ERROR(status)) {
            status = ST->BootServices->LocateHandleBuffer( ByProtocol,
                                                           &ConsoleControlGUID,
                                                           NULL,
                                                           &HandleCount,
                                                           &HandleBuffer);
            if (status == EFI_SUCCESS) {
                for (int i = 0; i < HandleCount; i++) {
                    status = ST->BootServices->HandleProtocol( HandleBuffer[i],
                                                               &ConsoleControlGUID,
                                                               (VOID*)&ConsoleControl);
                    if (!EFI_ERROR (status)) {
                        break;
                    }
                }
                ST->BootServices->FreePool(HandleBuffer);
            }
        }
    }
    if (strcmp(BSP_EFI_CONSOLE_KIND, "TEXT") == 0) {
        use_text = true;
    }
    if (strcmp(BSP_EFI_CONSOLE_KIND, "GRAPHIC") == 0) {
        use_graphic = true;
    }
    if (strcmp(BSP_EFI_CONSOLE_KIND, "BOTH") == 0) {
        use_text = true;
        use_graphic = true;
    }
    if (strcmp(BSP_EFI_CONSOLE_KIND, "AUTO") == 0) {
        use_auto = true;
    }
    if (ConsoleControl != NULL) {
        status = ConsoleControl->GetMode(ConsoleControl, &mode, &graphics_exists, &locked_stdin);
        if (!EFI_ERROR(status)) {
            print_ccm_info(mode, graphics_exists, locked_stdin);
            if (mode == EfiConsoleControlScreenText && use_auto) {
                use_text = true;
            }
            if (mode == EfiConsoleControlScreenGraphics && use_auto) {
                use_graphic = true;
            }
        }
        else {
            /* in case of error from console control, let's use both outputs */
            use_text = true;
            use_graphic = true;
        }
    }
    else {
        /* in case of missing console control, let's use both outputs */
        use_text = true;
        use_graphic = true;
    }
    if (get_boot_arg_int_value(boot_args(), "text_mode", &text_mode) == 0
        || get_boot_arg_int_value(boot_args(), "graphic_mode", &graphic_mode) == 0) {
        /* if there is any command-line arg passed to manage console, we give it max
           priority which means we reset any value set so far. */
        use_text = false;
        use_graphic = false;
    }
    if (get_boot_arg_int_value(boot_args(), "text_mode", &text_mode) == 0) {
        use_text = true;
    }
    if (get_boot_arg_int_value(boot_args(), "graphic_mode", &graphic_mode) == 0) {
        use_graphic = true;
    }    
    if (use_text)
        efi_init_text_output(text_mode);
    if (use_graphic) {
        efi_init_graphic_output(graphic_mode);
    }
    if (use_text && use_graphic) {
        BSP_output_char = both_output_char;
    }
    else if (use_text) {
        BSP_output_char = efi_text_output_char;
    }
    else if (use_graphic) {
        BSP_output_char = efi_graphic_output_char;
    }
    sync_output();
    is_efi_console_initialized = true;
}

void
buffered_output( char c );

void
buffered_output( char c )
{
    if (output_buffer_index < (4096 - 1)) {
        output_buffer[output_buffer_index] = c;
        output_buffer_index++;
    }
}

static void
both_output_char( char c )
{
    efi_text_output_char(c);
    efi_graphic_output_char(c);
}

void
sync_output()
{
    printf("EFI: console sync_output(): there are %d characters in the buffer.\n", output_buffer_index);
    for (int i = 0; i < output_buffer_index; i++) {
        BSP_output_char(output_buffer[i]);
    }
    printf("EFI: console sync_output() done.\n");
}

#if (BSP_EFI_EARLY_CONSOLE_KIND == BUFFER)
BSP_output_char_function_type BSP_output_char   = buffered_output;
#elif (BSP_EFI_EARLY_CONSOLE_KIND == SERIAL)
BSP_output_char_function_type BSP_output_char   = uart0_output_char;
#endif

BSP_polling_getchar_function_type BSP_poll_char = NULL;

#endif

