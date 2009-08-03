/*
 * Copyright (c) 2000 - Rosimildo da Silva ( rdasilva@connecttel.com )
 *
 * MODULE DESCRIPTION:
 * This module implements the micro FB driver for "Bare VGA". It uses the
 * routines for "bare hardware" that comes with MicroWindows.
 *
 *  $Id$
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>

#include <rtems/fb.h>

/* these routines are defined in the microwindows code. This
   driver is here more as an example of how to implement and
   use the micro FB interface
*/
extern void ega_hwinit( void );
extern void ega_hwterm( void );

/* screen information for the VGA driver */
static struct fb_var_screeninfo fb_var =
{
  .xres                = 640,
  .yres                = 480,
  .bits_per_pixel      = 4
};

static struct fb_fix_screeninfo fb_fix =
{
  .smem_start          = (volatile char *)0xA0000,     /* buffer pointer    */
  .smem_len            = 0x10000,                      /* buffer size       */
  .type                = FB_TYPE_VGA_PLANES,           /* type of dsplay    */
  .visual              = FB_VISUAL_PSEUDOCOLOR,        /* color scheme used */
  .line_length         = 80                            /* chars per line    */
};

static uint16_t red16[] = {
   0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
   0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};
static uint16_t green16[] = {
   0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa,
   0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};
static uint16_t blue16[] = {
   0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
   0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};

/*
 * fbvga device driver INITIALIZE entry point.
 */
rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  printk( "FBVGA -- driver initializing..\n" );

  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/fb0", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk("Error registering FBVGA device!\n");
    rtems_fatal_error_occurred( status );
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver OPEN entry point
 */
rtems_device_driver frame_buffer_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  ega_hwinit();
  printk( "FBVGA open called.\n" );

  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver CLOSE entry point
 */
rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  ega_hwterm();
  printk( "FBVGA close called.\n" );

  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver READ entry point.
 * Read characters from the PS/2 mouse.
 */
rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

  printk( "FBVGA read called.\n" );

  rw_args->bytes_moved = 0;

  return RTEMS_SUCCESSFUL;
}

/*
 * frame_buffer device driver WRITE entry point.
 * Write characters to the PS/2 mouse.
 */
rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

  printk( "FBVGA write called.\n" );
  rw_args->bytes_moved = 0;
  return RTEMS_SUCCESSFUL;
}

static int get_fix_screen_info( struct fb_fix_screeninfo *info )
{
  *info = fb_fix;
  return 0;
}

static int get_var_screen_info( struct fb_var_screeninfo *info )
{
  *info = fb_var;
  return 0;
}

static int get_palette( struct fb_cmap *cmap )
{
  uint32_t i;

  if ( cmap->start + cmap->len >= 16 )
    return 1;

  for( i = 0; i < cmap->len; i++ ) {
    cmap->red[ cmap->start + i ]   = red16[ cmap->start + i ];
    cmap->green[ cmap->start + i ] = green16[ cmap->start + i ];
    cmap->blue[ cmap->start + i ]  = blue16[ cmap->start + i ];
  }
  return 0;
}

static int set_palette( struct fb_cmap *cmap )
{
  uint32_t i;

  if ( cmap->start + cmap->len >= 16 )
    return 1;

  for( i = 0; i < cmap->len; i++ ) {
    red16[ cmap->start + i ] = cmap->red[ cmap->start + i ];
    green16[ cmap->start + i ] = cmap->green[ cmap->start + i ];
    blue16[ cmap->start + i ] = cmap->blue[ cmap->start + i ];
  }
  return 0;
}

/*
 * IOCTL entry point -- This method is called to carry
 * all services of this interface.
 */
rtems_device_driver frame_buffer_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  printk( "FBVGA ioctl called, cmd=%x\n", args->command  );

  switch( args->command ) {
    case FBIOGET_FSCREENINFO:
      args->ioctl_return =  get_fix_screen_info( args->buffer );
      break;
    case FBIOGET_VSCREENINFO:
      args->ioctl_return =  get_var_screen_info( args->buffer );
      break;
    case FBIOPUT_VSCREENINFO:
      /* not implemented yet*/
      break;
    case FBIOGETCMAP:
      args->ioctl_return =  get_palette( args->buffer );
      break;
    case FBIOPUTCMAP:
      args->ioctl_return =  set_palette( args->buffer );
      break;

    default:
     args->ioctl_return = 0;
     break;
  }
  return RTEMS_SUCCESSFUL;
}
