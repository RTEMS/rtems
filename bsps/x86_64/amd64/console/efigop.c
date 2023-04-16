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

#include <rtems/framebuffer.h>
#include <rtems/fb.h>
#include <rtems/score/atomic.h>
#include <rtems/libio.h>

#include <efigop.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef BSP_USE_EFI_BOOT_SERVICES

static Atomic_Flag driver_mutex;

extern EFI_SYSTEM_TABLE         *ST;
extern EFI_BOOT_SERVICES        *BS;

static struct fb_var_screeninfo gopfb_var = {
                                             .xres = 0,
                                             .yres = 0,
                                             .bits_per_pixel = 32
};

static struct fb_fix_screeninfo gopfb_fix = {
                                             .smem_start = NULL,
                                             .smem_len = 0,
                                             .type = FB_TYPE_PACKED_PIXELS,
                                             .visual = FB_VISUAL_TRUECOLOR,
                                             .line_length = 0
};

static EFI_GRAPHICS_OUTPUT*
find_gop(void);

static int
init_gop(EFI_GRAPHICS_OUTPUT*, int);

static int
init_fb_from_gop(EFI_GRAPHICS_OUTPUT *gop, struct fb_var_screeninfo* fbvar, struct fb_fix_screeninfo* fbfix);

extern void rpi_fb_outch(char);
extern void rpi_video_init(void);

void
efi_graphic_output_char(char c)
{
    rpi_fb_outch(c);
}

rtems_device_driver
frame_buffer_initialize
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    rtems_status_code status;

    status = rtems_io_register_name(FRAMEBUFFER_DEVICE_0_NAME, major, 0);
    if (status != RTEMS_SUCCESSFUL) {
        printf("EFI/GOP: error: can't register /dev/fb0 device.\n");
        rtems_fatal_error_occurred(status);
    }
    _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
    if (_Atomic_Flag_test_and_set(&driver_mutex, ATOMIC_ORDER_ACQUIRE) != 0) {
        printf("EFI/GOP: error: can't lock device mutex.\n" );
        return RTEMS_UNSATISFIED;
    }
    
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_open
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    if (_Atomic_Flag_test_and_set(&driver_mutex, ATOMIC_ORDER_ACQUIRE) != 0) {
        printf("EFI/GOP: error: can't lock device mutex.\n" );
        return RTEMS_UNSATISFIED;
    }
    if (gopfb_fix.smem_start == NULL) {
        _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
        printf( "EFI/GOP: framebuffer initialization failed.\n" );
            return RTEMS_UNSATISFIED;
    }
    memset( (void *) gopfb_fix.smem_start, 255, gopfb_fix.smem_len );
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_close
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_read
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    rtems_libio_rw_args_t* rw_args = (rtems_libio_rw_args_t*)arg;

    rw_args->bytes_moved =
        (( rw_args->offset + rw_args->count) > gopfb_fix.smem_len) ?
        (gopfb_fix.smem_len - rw_args->offset) : rw_args->count;
    memcpy( rw_args->buffer,
            (const void *)(gopfb_fix.smem_start + rw_args->offset),
            rw_args->bytes_moved);
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_write
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    rtems_libio_rw_args_t* rw_args = (rtems_libio_rw_args_t*)arg;

    rw_args->bytes_moved =
        ((rw_args->offset + rw_args->count) > gopfb_fix.smem_len) ?
        (gopfb_fix.smem_len - rw_args->offset) : rw_args->count;
    memcpy((void *)(gopfb_fix.smem_start + rw_args->offset),
           rw_args->buffer,
           rw_args->bytes_moved);
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_control
(rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg)
{
    rtems_libio_ioctl_args_t* args = (rtems_libio_ioctl_args_t*)arg;

    switch (args->command) {
    case FBIOGET_VSCREENINFO:
        memcpy(args->buffer, &gopfb_var, sizeof(struct fb_var_screeninfo));
        args->ioctl_return = 0;
        break;
    case FBIOGET_FSCREENINFO:
        memcpy(args->buffer, &gopfb_fix, sizeof(struct fb_fix_screeninfo));
        args->ioctl_return = 0;
        break;
    case FBIOGETCMAP:
        /* no palette - truecolor mode */
        args->ioctl_return = -1;
        return RTEMS_UNSATISFIED;
    case FBIOPUTCMAP:
        /* no palette - truecolor mode */
        args->ioctl_return = -1;
        return RTEMS_UNSATISFIED;
    default:
        args->ioctl_return = -1;
        return RTEMS_UNSATISFIED;
    }
    return RTEMS_SUCCESSFUL;
}

int
efi_init_graphic_output(int hint)
{
    EFI_GRAPHICS_OUTPUT* gop = find_gop();
    if (gop == NULL) {
        printf("EFI: GOP is not available\n");
        return RTEMS_UNSATISFIED;
    }
    if (init_gop(gop, hint) < 0) {
        return RTEMS_UNSATISFIED;
    }        
    init_fb_from_gop(gop, &gopfb_var, &gopfb_fix);
    /* init RPi based character output */
    rpi_video_init();
    memset( (void *) gopfb_fix.smem_start, 255, gopfb_fix.smem_len );
    return RTEMS_SUCCESSFUL;
}

static int
init_gop(EFI_GRAPHICS_OUTPUT *gop, int hint)
{
    EFI_STATUS status;
    if (gop == NULL)
        return -1;
    
    int imax = gop->Mode->MaxMode - 1;
    
    if (hint != -1) {
        /* hint got from command-line does have highest priority */
        status = gop->SetMode(gop, hint);
        if (EFI_ERROR(status)) {
            printf("EFI/GOP: can't set mode to: %d which was set on command line.\n", hint);
            return -1;
        }
    }
    else if (strcmp(BSP_EFI_GRAPHICS_OUTPUT_MODE_VALUE, "MAX") == 0) {
        status = gop->SetMode(gop, imax);
        if (EFI_ERROR(status)) {
            printf("EFI/GOP: can't set mode to: %d which should be MAX\n", imax);
            return -1;
        }
    }
    else if (strcmp(BSP_EFI_GRAPHICS_OUTPUT_MODE_VALUE, "AUTO") == 0) {
        status = gop->SetMode(gop, gop->Mode->Mode);
        if (EFI_ERROR(status)) {
            printf("EFI/GOP: can't set mode to: %d which should be AUTO (platform preferred value)\n", imax);
            return -1;
        }
    }
    else {
        int vmode = atoi(BSP_EFI_GRAPHICS_OUTPUT_MODE_VALUE);
        status = gop->SetMode(gop, vmode);
        if (EFI_ERROR(status)) {
            printf("EFI/GOP: can't set mode to: %d which is used supplied value.\n", vmode);
            return -1;
        }
    }
    return 0;
}

static int
init_fb_from_gop(EFI_GRAPHICS_OUTPUT *gop, struct fb_var_screeninfo* fbvar, struct fb_fix_screeninfo* fbfix)
{
    int i, imax;
    EFI_STATUS status;

    if (gop == NULL)
        return -1;
    imax = gop->Mode->MaxMode;
    
    printf("RTEMS: graphic output: current mode: %d, max mode: %d.\n", gop->Mode->Mode, (imax - 1));
 
    for (i = 0; i < imax; i++) {
         EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = NULL;
         UINTN SizeOfInfo = 0;
 
         status = gop->QueryMode(gop, i, &SizeOfInfo, &Info);
         if (EFI_ERROR(status) && status == EFI_NOT_STARTED) {
             gop->SetMode(gop, gop->Mode->Mode);
             status = gop->QueryMode(gop, i, &SizeOfInfo, &Info);
         }
 
         if (EFI_ERROR(status)) {
             printf("ERROR: Bad response from QueryMode: %ld\n", status);
             continue;
         }
         printf("%s%d: %dx%d ", memcmp(Info,gop->Mode->Info,sizeof(*Info)) == 0 ? " -> " : "    ", i,
                Info->HorizontalResolution,
                Info->VerticalResolution);
         switch(Info->PixelFormat) {
             case PixelRedGreenBlueReserved8BitPerColor:
                 printf("RGBR format, FB @ %p, size: %ld", (void*)gop->Mode->FrameBufferBase, gop->Mode->FrameBufferSize);
                 break;
             case PixelBlueGreenRedReserved8BitPerColor:
                 printf("BGRR format, FB @ %p, size: %ld", (void*)gop->Mode->FrameBufferBase, gop->Mode->FrameBufferSize);
                 break;
             case PixelBitMask:
                 printf("Red:%08x Green:%08x Blue:%08x Reserved:%08x",
                        Info->PixelInformation.RedMask,
                        Info->PixelInformation.GreenMask,
                        Info->PixelInformation.BlueMask,
                        Info->PixelInformation.ReservedMask);
                 break;
             case PixelBltOnly:
                 printf("(linear fb not available)");
                 break;
             default:
                 printf("(invalid format)");
                 break;
        }
        printf(", %d pixels per line\n", Info->PixelsPerScanLine);
    }
    fbvar->xres = gop->Mode->Info->HorizontalResolution;
    fbvar->yres = gop->Mode->Info->VerticalResolution;
    fbfix->smem_start = (void*)gop->Mode->FrameBufferBase;
    fbfix->smem_len = gop->Mode->FrameBufferSize;
    fbfix->line_length = gop->Mode->Info->PixelsPerScanLine;
    return EFI_SUCCESS;
}

static EFI_GRAPHICS_OUTPUT*
find_gop()
{
    EFI_HANDLE *HandleBuffer = NULL;
    UINTN HandleCount = 0;
    EFI_STATUS status = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT *gop = NULL;
    
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    status = BS->HandleProtocol(ST->ConsoleOutHandle, 
                                &gop_guid,
                                (VOID **)&gop);
    if (!EFI_ERROR (status) && gop != NULL) {
        return gop;
    }
    status = BS->LocateProtocol(&gop_guid, NULL, (void**)&gop);
    if (!EFI_ERROR (status) && gop != NULL) {
        return gop;
    }
    // try locating by handle
    status = BS->LocateHandleBuffer(ByProtocol,
                                    &gop_guid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer);
    if (!EFI_ERROR (status)) {
        for (int i = 0; i < HandleCount; i++) {
            status = BS->HandleProtocol( HandleBuffer[i],
                                         &gop_guid,
                                         (VOID*)&gop);
            if (!EFI_ERROR (status)) {
                break;
            }
        }
        BS->FreePool(HandleBuffer);
        return gop;
    }
    return NULL;
}

void
rpi_get_var_screen_info(struct fb_var_screeninfo* info)
{
    *info = gopfb_var;
}

void
rpi_get_fix_screen_info(struct fb_fix_screeninfo* info)
{
    *info = gopfb_fix;
}

#endif
