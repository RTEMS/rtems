/*
 * Copyright (c) 2000 - Rosimildo da Silva ( rdasilva@connecttel.com )
 *
 * MODULE DESCRIPTION:
 * This module implements FB driver for "Bare VGA". It uses the
 * routines for "bare hardware" found in vgainit.c.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>

#include <rtems/fb.h>

/* these routines are defined in vgainit.c.*/
extern void ega_hwinit( void );
extern void ega_hwterm( void );

/* mutex attribure */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

/* Functionality to support multiple VGA frame buffers can be added easily,
 * but is not supported at this moment because there is no need for two or
 * more "classic" VGA adapters.  Multiple frame buffer drivers may be
 * implemented and If we had implement it they would be named as "/dev/fb0",
 * "/dev/fb1", "/dev/fb2" and so on.
 */
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
    printk("Error registering /dev/fb0 FBVGA framebuffer device!\n");
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
  if (pthread_mutex_trylock(&mutex)== 0){
      /* restore previous state.  for VGA this means return to text mode.
       * leave out if graphics hardware has been initialized in
       * frame_buffer_initialize()
       */
      ega_hwinit();
      printk( "FBVGA open called.\n" );
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
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
  if (pthread_mutex_unlock(&mutex) == 0){
    /* restore previous state.  for VGA this means return to text mode.
     * leave out if graphics hardware has been initialized in
     * frame_buffer_initialize() */
    ega_hwterm();
    printk( "FBVGA close called.\n" );
    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}

/*
 * fbvga device driver READ entry point.
 */
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

/*
 * frame_buffer device driver WRITE entry point.
 */
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
      args->ioctl_return =  get_fix_screen_info( ( struct fb_fix_screeninfo * ) args->buffer );
      break;
    case FBIOGET_VSCREENINFO:
      args->ioctl_return =  get_var_screen_info( ( struct fb_var_screeninfo * ) args->buffer );
      break;
    case FBIOPUT_VSCREENINFO:
      /* not implemented yet*/
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
    case FBIOGETCMAP:
      args->ioctl_return =  get_palette( ( struct fb_cmap * ) args->buffer );
      break;
    case FBIOPUTCMAP:
      args->ioctl_return =  set_palette( ( struct fb_cmap * ) args->buffer );
      break;

    default:
     args->ioctl_return = 0;
     break;
  }
  return RTEMS_SUCCESSFUL;
}
