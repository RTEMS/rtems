/*
/////////////////////////////////////////////////////////////////////////////
// $Header$
//
// Copyright (c) 2000 - Rosimildo da Silva ( rdasilva@connecttel.com )
//
// MODULE DESCRIPTION:
// This module implements the micro FB driver for "Bare VGA". It uses the
// routines for "bare hardware" that comes with MicroWindows.
//
// MODIFICATION/HISTORY:
//
// $Log$
// Revision 1.2  2004/04/21 10:42:44  ralf
// Remove stray white spaces.
//
// Revision 1.1  2000/08/30 08:15:30  joel
// 2000-08-26  Rosimildo da Silva  <rdasilva@connecttel.com>
//
// 	* Major rework of the "/dev/console" driver.
// 	* Added termios support for stdin ( keyboard ).
// 	* Added ioctls() to support modes similar to Linux( XLATE,
// 	RAW, MEDIUMRAW ).
// 	* Added Keyboard mapping and handling of the keyboard's leds.
// 	* Added Micro FrameBuffer driver ( "/dev/fb0" ) for bare VGA
// 	controller ( 16 colors ).
// 	* Added PS/2 and Serial mouse support for PC386 BSP.
// 	* console/defkeymap.c: New file.
// 	* console/fb_vga.c: New file.
// 	* console/fb_vga.h: New file.
// 	* console/i386kbd.h: New file.
// 	* console/kd.h: New file.
// 	* console/keyboard.c: New file.
// 	* console/keyboard.h: New file.
// 	* console/mouse_parser.c: New file.
// 	* console/mouse_parser.h: New file.
// 	* console/pc_keyb.c: New file.
// 	* console/ps2_drv.h: New file.
// 	* console/ps2_mouse.c: New file.
// 	* console/ps2_mouse.h: New file.
// 	* console/serial_mouse.c: New file.
// 	* console/serial_mouse.h: New file.
// 	* console/vgainit.c: New file.
// 	* console/vt.c: New file.
// 	* console/Makefile.am: Reflect new files.
// 	* console/console.c, console/inch.c, console/outch.c: Console
// 	functionality modifications.
// 	* startup/Makefile.am: Pick up tty_drv.c and gdb_glue.c
//
//
/////////////////////////////////////////////////////////////////////////////
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <irq.h>
#include <rtems/libio.h>

#include <rtems/mw_fb.h>

/* these routines are defined in the microwindows code. This
   driver is here more as an example of how to implement and
   use the micro FB interface
*/
extern void ega_hwinit( void );
extern void ega_hwterm( void );

/* screen information for the VGA driver */
static struct fb_screeninfo fb_info =
{
   640, 480,                     /* screen size x, y  */
   4,                            /* bits per pixel    */
   80,                           /* chars per line    */
   (volatile char *)0xA0000,     /* buffer pointer    */
   0x10000,                      /* buffer size       */
   FB_TYPE_VGA_PLANES,           /* type of dsplay    */
	FB_VISUAL_PSEUDOCOLOR         /* color scheme used */
};

static __u16 red16[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
    0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};
static __u16 green16[] = {
    0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa,
    0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};
static __u16 blue16[] = {
    0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
    0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};

/*
 * fbvga device driver INITIALIZE entry point.
 */
rtems_device_driver
fbvga_initialize(  rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
   rtems_status_code status;

  printk( "FBVGA -- driver initializing..\n" );
  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/fb0", major, 0);
  if (status != RTEMS_SUCCESSFUL)
  {
      printk("Error registering FBVGA device!\n");
      rtems_fatal_error_occurred( status );
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver OPEN entry point
 */
rtems_device_driver
fbvga_open( rtems_device_major_number major,
            rtems_device_minor_number minor,
            void                      *arg)
{
/*  rtems_status_code status; */
  printk( "FBVGA open called.\n" );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver CLOSE entry point
 */
rtems_device_driver
fbvga_close(rtems_device_major_number major,
            rtems_device_minor_number minor,
            void                      *arg)
{
  printk( "FBVGA close called.\n" );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver READ entry point.
 * Read characters from the PS/2 mouse.
 */
rtems_device_driver
fbvga_read( rtems_device_major_number major,
            rtems_device_minor_number minor,
            void                      *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  printk( "FBVGA read called.\n" );
  rw_args->bytes_moved = 0;
  return RTEMS_SUCCESSFUL;
}

/*
 * fbvga device driver WRITE entry point.
 * Write characters to the PS/2 mouse.
 */
rtems_device_driver
fbvga_write( rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                    * arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  printk( "FBVGA write called.\n" );
  rw_args->bytes_moved = 0;
  return RTEMS_SUCCESSFUL;
}

static int get_screen_info( struct fb_screeninfo *info )
{
  *info = fb_info;
  return 0;
}

static int get_palette( struct fb_cmap *cmap )
{
  __u32 i;

  if( cmap->start + cmap->len >= 16 )
      return 1;

  for( i = 0; i < cmap->len; i++ )
  {
    cmap->red[ cmap->start + i ]   = red16[ cmap->start + i ];
    cmap->green[ cmap->start + i ] = green16[ cmap->start + i ];
    cmap->blue[ cmap->start + i ]  = blue16[ cmap->start + i ];
  }
  return 0;
}

static int set_palette( struct fb_cmap *cmap )
{
  __u32 i;

  if( cmap->start + cmap->len >= 16 )
      return 1;

  for( i = 0; i < cmap->len; i++ )
  {
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
rtems_device_driver
fbvga_control( rtems_device_major_number major,
               rtems_device_minor_number minor,
               void                      * arg
)
{
	rtems_libio_ioctl_args_t *args = arg;
   printk( "FBVGA ioctl called, cmd=%x\n", args->command  );
   switch( args->command )
   {
      case FB_SCREENINFO:
      args->ioctl_return =  get_screen_info( args->buffer );
      break;
      case FB_GETPALETTE:
      args->ioctl_return =  get_palette( args->buffer );
      break;
      case FB_SETPALETTE:
      args->ioctl_return =  set_palette( args->buffer );
      break;

      /* this function would execute one of the routines of the
       * interface based on the operation requested
       */
      case FB_EXEC_FUNCTION:
      {
         struct fb_exec_function *env = args->buffer;
         switch( env->func_no )
         {
           case FB_FUNC_ENTER_GRAPHICS:
           /* enter graphics mode*/
           ega_hwinit();
           break;

           case FB_FUNC_EXIT_GRAPHICS:
           /* leave graphics mode*/
           ega_hwterm();
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
