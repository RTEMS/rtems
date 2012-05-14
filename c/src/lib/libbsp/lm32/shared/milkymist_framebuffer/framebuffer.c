/*  framebuffer.c
 *
 *  This file is the framebuffer driver for the Milkymist VGA IP-core
 *  This VGA Core is a part of Milkymist System-on-Chip
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 *  Copyright (c) 2011 Sebastien Bourdeauducq
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <bsp.h>
#include "../include/system_conf.h"
#include <rtems/libio.h>
#include <rtems/fb.h>

#define FRAMEBUFFER_DEVICE_NAME "/dev/fb"

static unsigned short int framebufferA[1024*768]
  __attribute__((aligned(32)));
static unsigned short int framebufferB[1024*768]
  __attribute__((aligned(32)));
static unsigned short int framebufferC[1024*768]
  __attribute__((aligned(32)));

static unsigned short int *frontbuffer;
static unsigned short int *backbuffer;
static unsigned short int *lastbuffer;

static struct fb_var_screeninfo fb_var = {
  .xres                = 640,
  .yres                = 480,
  .bits_per_pixel      = 16
};

static struct fb_fix_screeninfo fb_fix = {
  .smem_len            = 1024 * 768 * 2,
  .type                = FB_TYPE_VGA_PLANES,
  .visual              = FB_VISUAL_TRUECOLOR,
  .line_length         = 80
};

static int get_fix_screen_info( struct fb_fix_screeninfo *info )
{
  *info = fb_fix;
  return 0;
}

static int get_var_screen_info( struct fb_var_screeninfo *info )
{
  *info =  fb_var;
  return 0;
}

static void init_buffers(void)
{
  frontbuffer = framebufferA;
  backbuffer = framebufferB;
  lastbuffer = framebufferC;
}

static void swap_buffers(void)
{
  unsigned short int *p;

  /* Make sure last buffer swap has been executed */
  while (MM_READ(MM_VGA_BASEADDRESS_ACT) != MM_READ(MM_VGA_BASEADDRESS));

  p = frontbuffer;
  frontbuffer = backbuffer;
  backbuffer = lastbuffer;
  lastbuffer = p;

  fb_fix.smem_start = (volatile char *)backbuffer;
  MM_WRITE(MM_VGA_BASEADDRESS, (unsigned int)frontbuffer);
}

static void set_video_mode(int mode)
{
  int hres, vres;
  
  MM_WRITE(MM_VGA_RESET, VGA_RESET);
  hres = vres = 0;
  switch(mode) {
    case 0: // 640x480, pixel clock: 25MHz
      hres = 640;
      vres = 480;
      MM_WRITE(MM_VGA_HSYNC_START, 656);
      MM_WRITE(MM_VGA_HSYNC_END, 752);
      MM_WRITE(MM_VGA_HSCAN, 799);
      MM_WRITE(MM_VGA_VSYNC_START, 492);
      MM_WRITE(MM_VGA_VSYNC_END, 494);
      MM_WRITE(MM_VGA_VSCAN, 524);
      MM_WRITE(MM_VGA_CLKSEL, 0);
      break;
    case 1: // 800x600, pixel clock: 50MHz
      hres = 800;
      vres = 600;
      MM_WRITE(MM_VGA_HSYNC_START, 848);
      MM_WRITE(MM_VGA_HSYNC_END, 976);
      MM_WRITE(MM_VGA_HSCAN, 1040);
      MM_WRITE(MM_VGA_VSYNC_START, 636);
      MM_WRITE(MM_VGA_VSYNC_END, 642);
      MM_WRITE(MM_VGA_VSCAN, 665);
      MM_WRITE(MM_VGA_CLKSEL, 1);
      break;
    case 2: // 1024x768, pixel clock: 65MHz
      hres = 1024;
      vres = 768;
      MM_WRITE(MM_VGA_HSYNC_START, 1048);
      MM_WRITE(MM_VGA_HSYNC_END, 1184);
      MM_WRITE(MM_VGA_HSCAN, 1344);
      MM_WRITE(MM_VGA_VSYNC_START, 772);
      MM_WRITE(MM_VGA_VSYNC_END, 778);
      MM_WRITE(MM_VGA_VSCAN, 807);
      MM_WRITE(MM_VGA_CLKSEL, 2);
      break;
  }
  if((hres != 0) && (vres != 0)) {
    MM_WRITE(MM_VGA_HRES, hres);
    MM_WRITE(MM_VGA_VRES, vres);
    fb_var.xres = hres;
    fb_var.yres = vres;
    memset(framebufferA, 0, hres*vres*2);
    memset(framebufferB, 0, hres*vres*2);
    memset(framebufferC, 0, hres*vres*2);
    MM_WRITE(MM_VGA_BURST_COUNT, hres*vres/16);
    MM_WRITE(MM_VGA_RESET, 0);
  } /* otherwise, leave the VGA controller in reset */
}

rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code status;

  init_buffers();
  fb_fix.smem_start = (volatile char *)frontbuffer;
  MM_WRITE(MM_VGA_BASEADDRESS, (unsigned int)frontbuffer);

  status = rtems_io_register_name(FRAMEBUFFER_DEVICE_NAME, major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk("Error registering frame buffer device!\n");
    rtems_fatal_error_occurred( status );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len)
    ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy(rw_args->buffer, (const void *)(fb_fix.smem_start + rw_args->offset),
    rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len)
     ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy((void *)(fb_fix.smem_start + rw_args->offset), rw_args->buffer,
    rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
    case FBIOGET_FSCREENINFO:
      args->ioctl_return =
        get_fix_screen_info((struct fb_fix_screeninfo *)args->buffer);
      return RTEMS_SUCCESSFUL;
    case FBIOGET_VSCREENINFO:
      args->ioctl_return =
        get_var_screen_info((struct fb_var_screeninfo *)args->buffer);
      return RTEMS_SUCCESSFUL;
    case FBIOSWAPBUFFERS:
      swap_buffers();
      args->ioctl_return = 0;
      return RTEMS_SUCCESSFUL;
    case FBIOSETBUFFERMODE:
      args->ioctl_return = 0;
      switch ((unsigned int)args->buffer) {
        case FB_SINGLE_BUFFERED:
          init_buffers();
          fb_fix.smem_start = (volatile char *)frontbuffer;
          MM_WRITE(MM_VGA_BASEADDRESS, (unsigned int)frontbuffer);
          return RTEMS_SUCCESSFUL;
        case FB_TRIPLE_BUFFERED:
          fb_fix.smem_start = (volatile char *)backbuffer;
          return RTEMS_SUCCESSFUL;
        default:
          return RTEMS_UNSATISFIED;
      }
    case FBIOSETVIDEOMODE:
      set_video_mode((int)args->buffer);
      return RTEMS_SUCCESSFUL;
    default:
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
  }
}

