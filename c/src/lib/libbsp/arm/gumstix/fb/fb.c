 /*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include <pxa255.h>
#include <bsp.h>
#include <rtems/libio.h>

#include <rtems/fb.h>
#include <rtems/framebuffer.h>
#include <rtems.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BPP 16
#define LCD_DMA_POINTER (0xa0000000 + 62*(1<<20) + 0x1000)
#define LCD_BUFFER_SIZE (SCREEN_WIDTH*SCREEN_HEIGHT*2)

/* mutex attribure */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static struct fb_var_screeninfo fb_var =
  {
    .xres = SCREEN_WIDTH,
    .yres = SCREEN_HEIGHT,
    .bits_per_pixel = BPP
  };

static struct fb_fix_screeninfo fb_fix =
  {
    .smem_start = (volatile char *)LCD_DMA_POINTER,
    .smem_len = LCD_BUFFER_SIZE,
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_TRUECOLOR,
    .line_length = SCREEN_WIDTH * (BPP/8)
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

static void enable_fbskyeye(void)
{
  LCCR1 &= ~LCCR1_PPL;
  LCCR1 |= (SCREEN_WIDTH -1 ) & LCCR1_PPL;
  LCCR2 &= ~LCCR2_LPP;
  LCCR2 |= (SCREEN_HEIGHT/2 -1) & LCCR2_LPP;
  LCCR3 &= ~LCCR3_BPP;
  LCCR3 |= 4<<24;
  FDADR0 = LCD_DMA_POINTER - 0x1000;
  LCCR0 |= LCCR0_ENB;
}


static void disable_fbskyeye(void)
{
  LCCR0 &= ~LCCR0_ENB;
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

rtems_device_driver
frame_buffer_initialize(rtems_device_major_number major,
		    rtems_device_minor_number minor,
		    void                      *arg)
{
  rtems_status_code status;

  printk( "FBSKYEYE -- driver initializing..\n" );
  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/fb0", major, 0);
  if (status != RTEMS_SUCCESSFUL)
    {
      printk("Error registering FBSKYEYE device!\n");
      rtems_fatal_error_occurred( status );
    }
  return RTEMS_SUCCESSFUL;
}


rtems_device_driver
frame_buffer_open( rtems_device_major_number major,
	       rtems_device_minor_number minor,
	       void                      *arg)
{
  if (pthread_mutex_trylock(&mutex)== 0){
      /* restore previous state.  for VGA this means return to text mode.
       * leave out if graphics hardware has been initialized in
       * frame_buffer_initialize()
       */
     printk( "FBSKYEYE open called.\n" );
     enable_fbskyeye();
     return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}


rtems_device_driver
frame_buffer_close(rtems_device_major_number major,
	       rtems_device_minor_number minor,
	       void                      *arg)
{
  if (pthread_mutex_unlock(&mutex) == 0){
    /* restore previous state.  for VGA this means return to text mode.
     * leave out if graphics hardware has been initialized in
     * frame_buffer_initialize() */
    printk( "fbskyeye close called.\n" );
    disable_fbskyeye();
    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}

rtems_device_driver
frame_buffer_read( rtems_device_major_number major,
	       rtems_device_minor_number minor,
	       void                      *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy(rw_args->buffer, (const void *) (fb_fix.smem_start + rw_args->offset), rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver
frame_buffer_write( rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                    * arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy( (void *) (fb_fix.smem_start + rw_args->offset), rw_args->buffer, rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
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


rtems_device_driver
frame_buffer_control( rtems_device_major_number major,
		  rtems_device_minor_number minor,
		  void                      * arg
		  )
{
  rtems_libio_ioctl_args_t *args = arg;
  printk( "FBSKYEYE ioctl called, cmd=%x\n", args->command  );
  switch( args->command )
    {
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
