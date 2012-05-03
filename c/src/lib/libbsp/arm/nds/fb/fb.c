/*
 * RTEMS for Nintendo DS framebuffer display driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/libio.h>
#include <nds.h>

#include <rtems/fb.h>


/*
 * screen information for the driver (fb0).
 */

static struct fb_var_screeninfo fb_var_info = {
  .xres                = SCREEN_WIDTH,		/* screen size x, y  */
  .yres                = SCREEN_HEIGHT,
  .bits_per_pixel      = 16			/* bits per pixel    */
};

static struct fb_fix_screeninfo fb_fix_info = {
  .smem_start          = (void *) VRAM_A,      	     /* buffer pointer    */
  .smem_len            = 0x18000,                    /* buffer size       */
  .type                = FB_TYPE_PACKED_PIXELS,      /* type of dsplay    */
  .visual              = FB_VISUAL_TRUECOLOR,        /* color scheme used */
  .line_length         = SCREEN_WIDTH               /* pixels per line (redundant with xres ?) */
};

/*
 * screen information for the driver (fb1).
 */

static struct fb_var_screeninfo fb_var_info2 = {
  .xres                = SCREEN_WIDTH,		/* screen size x, y  */
  .yres                = SCREEN_HEIGHT,
  .bits_per_pixel      = 16			/* bits per pixel    */
};

static struct fb_fix_screeninfo fb_fix_info2 = {
  .smem_start          = (void *) VRAM_B,      	     /* buffer pointer    */
  .smem_len            = 0x18000,                    /* buffer size       */
  .type                = FB_TYPE_PACKED_PIXELS,      /* type of dsplay    */
  .visual              = FB_VISUAL_TRUECOLOR,        /* color scheme used */
  .line_length         = SCREEN_WIDTH               /* pixels per line (redundant with xres ?) */
};


/*
 * fbds device driver initialize entry point.
 */

rtems_device_driver
frame_buffer_initialize (rtems_device_major_number major,
                 rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;

  printk ("[+] framebuffer started\n");

  /* register the devices */
  status = rtems_io_register_name ("/dev/fb0", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] error registering framebuffer\n");
    rtems_fatal_error_occurred (status);
  }
  status = rtems_io_register_name ("/dev/fb1", major, 1);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] error registering framebuffer\n");
    rtems_fatal_error_occurred (status);
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver open operation.
 */

rtems_device_driver
frame_buffer_open (rtems_device_major_number major,
           rtems_device_minor_number minor, void *arg)
{
   printk ("[#] entering graphic mode on fb%d\n", minor);
   if (!minor) {
      videoSetMode (MODE_FB0);
      vramSetBankA (VRAM_A_LCD);
      memset ((void *)fb_fix_info.smem_start, 0, fb_fix_info.smem_len);
   } else {
      videoSetModeSub (MODE_FB0);
      vramSetBankB (VRAM_B_LCD);
      memset ((void *)fb_fix_info2.smem_start, 0, fb_fix_info2.smem_len);
    }
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver close operation.
 */

rtems_device_driver
frame_buffer_close (rtems_device_major_number major,
            rtems_device_minor_number minor, void *arg)
{
  printk ("[#] leaving graphic mode on fb%d\n", minor);
  if (!minor) {
      memset ((void *)fb_fix_info.smem_start, 0, fb_fix_info.smem_len);
  }
  else {
      memset ((void *)fb_fix_info2.smem_start, 0, fb_fix_info2.smem_len);
      /* back to console */
      videoSetModeSub (MODE_0_2D | DISPLAY_BG0_ACTIVE);
      vramSetBankC (VRAM_C_SUB_BG);
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver read operation.
 */

rtems_device_driver
frame_buffer_read (rtems_device_major_number major,
           rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  rw_args->bytes_moved = 0;

  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver write operation.
 */

rtems_device_driver
frame_buffer_write (rtems_device_major_number major,
            rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  rw_args->bytes_moved = 0;

  return RTEMS_SUCCESSFUL;
}

/*
 * ioctl entry point.
 */

rtems_device_driver
frame_buffer_control (rtems_device_major_number major,
              rtems_device_minor_number minor, void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;

  /* XXX check minor */

  switch (args->command) {
  case FBIOGET_VSCREENINFO:
    memcpy (args->buffer, minor ? &fb_var_info2 : &fb_var_info, sizeof (fb_var_info));
    args->ioctl_return = 0;
    break;
  case FBIOGET_FSCREENINFO:
    memcpy (args->buffer, minor ? &fb_fix_info2 : &fb_fix_info, sizeof (fb_fix_info));
    args->ioctl_return = 0;
    break;
  case FBIOGETCMAP:
    args->ioctl_return = 0;     /* XXX */
    break;
  case FBIOPUTCMAP:
    args->ioctl_return = 0;     /* XXX */
    break;

    /* no break on purpose */
  default:
    args->ioctl_return = 0;
    break;

  }
  return RTEMS_SUCCESSFUL;
}
