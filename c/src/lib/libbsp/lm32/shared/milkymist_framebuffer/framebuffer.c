/*  framebuffer.c
 *
 *  This file is the framebuffer driver for the Milkymist VGA IP-core
 *  This VGA Core is a part of Milkymist System-on-Chip
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <rtems.h>
#include <bsp.h>
#include "../include/system_conf.h"
#include <rtems/libio.h>
#include <rtems/fb.h>

#define FRAMEBUFFER_DEVICE_NAME "/dev/fb"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static unsigned short int framebufferA[640*480] __attribute__((aligned(32)));
static unsigned short int framebufferB[640*480] __attribute__((aligned(32)));
static unsigned short int framebufferC[640*480] __attribute__((aligned(32)));

static unsigned short int *frontbuffer = framebufferA;
static unsigned short int *backbuffer = framebufferB;
static unsigned short int *lastbuffer = framebufferC;

static inline void fb_write(unsigned int reg, int value)
{
  *((int*)reg) = value;
}

static inline int fb_read(unsigned int reg)
{
  return *((int*)(reg));
}

/* screen information for the VGA driver */
static struct fb_var_screeninfo fb_var =
{
  .xres                = 640,
  .yres                = 480,
  .bits_per_pixel      = 16
};

static struct fb_fix_screeninfo fb_fix =
{
// this is initialized at start-up
  .smem_len            = 640 * 480 * 2,                      /* buffer size       */
// 2 bytes per pixels
  .type                = FB_TYPE_VGA_PLANES,           /* type of display    */
  .visual              = FB_VISUAL_TRUECOLOR,        /* color scheme used */
  .line_length         = 80                            /* chars per line    */
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


rtems_device_driver frame_buffer_initialize(
rtems_device_major_number  major,
rtems_device_minor_number  minor,
void *arg)
{
  rtems_status_code status;

  printk( "frame buffer driver initializing..\n" );

  fb_fix.smem_start = (volatile char *)frontbuffer;

  fb_write(MM_VGA_BASEADDRESS, (unsigned int)frontbuffer);
  fb_write(MM_VGA_RESET, (unsigned int)0);

  /*
  * Register the device
  */
  status = rtems_io_register_name(FRAMEBUFFER_DEVICE_NAME, major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
    printk("Error registering frame buffer device!\n");
    rtems_fatal_error_occurred( status );
  }

  printk("VGA: initialized at resolution %dx%d\n", fb_var.xres, fb_var.yres);
  printk("VGA: framebuffers at 0x%08x 0x%08x 0x%08x\n",
  (unsigned int)frontbuffer, (unsigned int)backbuffer,
  (unsigned int)lastbuffer);

  /*
  * graphics hardware initialization goes here for non-console
  * devices
  */
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  if (pthread_mutex_unlock(&mutex) == 0){
  /* restore previous state.  for VGA this means return to text mode.
   * leave out if graphics hardware has been initialized in
   * frame_buffer_initialize() */
    fb_write(MM_VGA_RESET, MM_VGA_RESET_MODE);
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}

rtems_device_driver frame_buffer_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  if (pthread_mutex_trylock(&mutex) == 0){
    fb_write(MM_VGA_RESET, (unsigned int)0);
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}

static void vga_swap_buffers(void)
{
  unsigned short int *p;

  /*
  * Make sure last buffer swap has been executed.
  * Beware, DMA address registers of vgafb are incomplete
  * (only LSBs are present) so don't compare them directly
  * with CPU pointers.
  */
  while( fb_read(MM_VGA_BASEADDRESS_ACT) != fb_read(MM_VGA_BASEADDRESS) );

  p = frontbuffer;
  frontbuffer = backbuffer;
  backbuffer = lastbuffer;
  lastbuffer = p;

  fb_fix.smem_start = (volatile char *)backbuffer;

  fb_write(MM_VGA_BASEADDRESS, (unsigned int)frontbuffer);
}

rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy(rw_args->buffer, (const void *) (fb_fix.smem_start + rw_args->offset), rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy( (void *) (fb_fix.smem_start + rw_args->offset), rw_args->buffer, rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch( args->command ) {
    case FBIOGET_FSCREENINFO:
      args->ioctl_return =  get_fix_screen_info( ( struct fb_fix_screeninfo * ) args->buffer );
      break;
    case FBIOGET_VSCREENINFO:
      args->ioctl_return =  get_var_screen_info( ( struct fb_var_screeninfo * ) args->buffer );
      break;
    case FBIOPUT_VSCREENINFO:
      /* not implemented yet */
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
    case FBIOGETCMAP:
      /* not implemented yet */
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
      break;
    case FBIOPUTCMAP:
      /* not implemented yet */
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
      break;
    case FBIOSWAPBUFFERS:
      vga_swap_buffers();
      args->ioctl_return = 0;
      break;
    case FBIOSETBUFFERMODE:
      args->ioctl_return = 0;
      switch ( (unsigned int)args->buffer ) {
        case FB_SINGLE_BUFFERED:
          fb_fix.smem_start = (volatile char *)frontbuffer;
          break;
        case  FB_TRIPLE_BUFFERED:
          fb_fix.smem_start = (volatile char *)backbuffer;
          break;
        default:
          printk("[framebuffer] : error no such buffer mode\n");
      }
      break;
    default:
     args->ioctl_return = 0;
     break;
  }
  return RTEMS_SUCCESSFUL;
}

