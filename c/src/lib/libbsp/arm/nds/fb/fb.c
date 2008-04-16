/*
 * RTEMS for Nintendo DS framebuffer display driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 *
 * $Id$
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include "../irq/irq.h"
#include <rtems/libio.h>
#include <nds.h>

#include <rtems/mw_fb.h>

/*
 * screen information for the driver (fb0).
 */

static struct fb_screeninfo fb_info = {
  SCREEN_WIDTH, SCREEN_HEIGHT,  /* screen size x, y  */
  16,                           /* bits per pixel    */
  SCREEN_WIDTH,                 /* pixels per line (redundant with xres ?) */
  (void *) VRAM_A,              /* buffer pointer    */
  0x18000,                      /* buffer size       */
  FB_TYPE_PACKED_PIXELS,        /* type of dsplay    */
  FB_VISUAL_TRUECOLOR           /* color scheme used */
};

/*
 * screen information for the driver (fb1).
 */

static struct fb_screeninfo fb_info2 = {
  SCREEN_WIDTH, SCREEN_HEIGHT,  /* screen size x, y  */
  16,                           /* bits per pixel    */
  SCREEN_WIDTH,                 /* pixels per line (redundant with xres ?) */
  (void *) VRAM_B,              /* buffer pointer    */
  0x18000,                      /* buffer size       */
  FB_TYPE_PACKED_PIXELS,        /* type of dsplay    */
  FB_VISUAL_TRUECOLOR           /* color scheme used */
};

/*
 * fbds device driver initialize entry point.
 */

rtems_device_driver
fbds_initialize (rtems_device_major_number major,
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
fbds_open (rtems_device_major_number major,
           rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver close operation.
 */

rtems_device_driver
fbds_close (rtems_device_major_number major,
            rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver read operation.
 */

rtems_device_driver
fbds_read (rtems_device_major_number major,
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
fbds_write (rtems_device_major_number major,
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
fbds_control (rtems_device_major_number major,
              rtems_device_minor_number minor, void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;

  /* XXX check minor */

  switch (args->command) {
  case FB_SCREENINFO:
    memcpy (args->buffer, minor ? &fb_info2 : &fb_info, sizeof (fb_info));
    args->ioctl_return = 0;
    break;
  case FB_GETPALETTE:
    args->ioctl_return = 0;     /* XXX */
    break;
  case FB_SETPALETTE:
    args->ioctl_return = 0;     /* XXX */
    break;

    /*
     * this function would execute one of the routines of the
     * interface based on the operation requested.
     */
  case FB_EXEC_FUNCTION:
    {
      struct fb_exec_function *env = args->buffer;

      switch (env->func_no) {
      case FB_FUNC_ENTER_GRAPHICS:
        /* enter graphics mode */
        printk ("[#] entering graphic mode on fb%d\n", minor);
        if (!minor) {
          videoSetMode (MODE_FB0);
          vramSetBankA (VRAM_A_LCD);
          memset (fb_info.smem_start, 0, fb_info.smem_len);
        } else {
          videoSetModeSub (MODE_FB0);
          vramSetBankB (VRAM_B_LCD);
          memset (fb_info2.smem_start, 0, fb_info2.smem_len);
        }
        break;

      case FB_FUNC_EXIT_GRAPHICS:
        /* leave graphics mode, in fact we only clear screen */
        printk ("[#] leaving graphic mode on fb%d\n", minor);
        if (!minor) {
          memset (fb_info.smem_start, 0, fb_info.smem_len);
        } else {
          memset (fb_info2.smem_start, 0, fb_info2.smem_len);
          /* back to console */
          videoSetModeSub (MODE_0_2D | DISPLAY_BG0_ACTIVE);
          vramSetBankC (VRAM_C_SUB_BG);
        }
        break;

      case FB_FUNC_IS_DIRTY:
        break;

      case FB_FUNC_GET_MODE:
        break;

      default:
        break;
      }
    }
    /* no break on purpose */
  default:
    args->ioctl_return = 0;
    break;

  }
  return RTEMS_SUCCESSFUL;
}
