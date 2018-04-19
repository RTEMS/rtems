/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief framebuffer support.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/mailbox.h>
#include <bsp/vc.h>
#include <bsp/rpi-fb.h>

#include <libcpu/arm-cp15.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/fb.h>
#include <rtems/framebuffer.h>
#include <rtems/score/atomic.h>
#include <rtems/bspIo.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BPP 32

/* flag to limit driver to protect against multiple opens */
static Atomic_Flag driver_mutex;

/*
 * screen information for the driver (fb0).
 */

static struct fb_var_screeninfo fb_var_info = {
  .xres                = SCREEN_WIDTH,
  .yres                = SCREEN_HEIGHT,
  .bits_per_pixel      = BPP
};

static struct fb_fix_screeninfo fb_fix_info = {
  .smem_start          = (void *) NULL,
  .smem_len            = 0,
  .type                = FB_TYPE_PACKED_PIXELS,
  .visual              = FB_VISUAL_TRUECOLOR,
  .line_length         = 0
};

typedef enum {
  NO_SUITABLE_MODE     = -1,
  BAD_FORMAT           = -2,
  AUTO_SELECT          = -3,
  DONT_INIT            = -4,
  NO_MODE_REQ          = -5,
} mode_err_ret_val;

int rpi_get_fix_screen_info( struct fb_fix_screeninfo *info )
{
  *info = fb_fix_info;
  return 0;
}

int rpi_get_var_screen_info( struct fb_var_screeninfo *info )
{
  *info = fb_var_info;
  return 0;
}

/**
 * @brief Find mode given in string format.
 *
 *  expected format
 *  <resX>x<resY>[-<bpp>]
 *  numbers <resX>, <resY> and <bpp> are decadic
 *
 * @param[out] fb_var_ptr pointer to variable mode part filled by function
 * @param[in] video_string string to be parsed
 * @retval video mode number to be set
 * @retval -1 no suitable mode found
 * @retval -2 bad format of the video_string
 * @retval -3 automatic mode selection requested
 * @retval -4 request to not initialize graphics
 * @retval -5 no mode requested/empty video string
 */

static int parse_mode_from_string(
  struct fb_var_screeninfo *fb_var_ptr,
  const char               *video_string
)
{
  const char *opt;
  char       *endptr;
  uint32_t    width;
  uint32_t    height;
  uint32_t    bpp = 0;

  opt = video_string;

  if ( opt == NULL )
    return NO_MODE_REQ;

  if ( strncmp( opt, "auto", 4 ) == 0 )
    return AUTO_SELECT;

  if ( strncmp( opt, "none", 4 ) == 0 ||
       strncmp( opt, "off", 3 ) == 0 )
    return DONT_INIT;

  width = strtol( opt, &endptr, 10 );

  if ( *endptr != 'x' ) {
    return BAD_FORMAT;
  }

  opt = endptr + 1;
  height = strtol( opt, &endptr, 10 );

  switch ( *endptr ) {
    case '-':
      opt = endptr + 1;
      endptr = NULL;
      bpp = strtol( opt, &endptr, 10 );

      if ( ( endptr == opt ) || ( endptr == NULL ) )
        return BAD_FORMAT;

      if ( *endptr && ( *endptr != ' ' ) )
        return BAD_FORMAT;

      break;
    case ' ':
    case 0:
      break;
    default:
      return BAD_FORMAT;
  }

  fb_var_ptr->xres = width;
  fb_var_ptr->yres = height;

  if ( bpp != 0 )
    fb_var_ptr->bits_per_pixel = bpp;

  return 0;
}

static int find_mode_from_vc( void )
{
  int res;
  unsigned int width;
  unsigned int height;
  bcm2835_get_display_size_entries entries;

  res = bcm2835_mailbox_get_display_size( &entries );

  width = entries.width;
  height = entries.height;

  if ( width == 0 || height == 0 ) {
    fb_var_info.xres = SCREEN_WIDTH;
    fb_var_info.yres = SCREEN_HEIGHT;
  } else {
    fb_var_info.xres = width;
    fb_var_info.yres = height;
  }
  printk("find_mode_from_vc %u x %u, res %d\n", width, height, res);

  return res;
}

bool rpi_fb_hdmi_is_present( void )
{
  bcm2835_get_display_size_entries entries;

  memset( &entries, 0, sizeof( entries ) );
  bcm2835_mailbox_get_display_size( &entries );

  /* Impossible display dimension */
  if ( ( entries.width < 10 ) || ( entries.height < 10 ) )
    return false;

  /* Know default values reported when monitor is not present */
  if ( ( entries.width == 0x290 ) && ( entries.height == 0x1A0 ) )
    return false;

  return true;
}

int rpi_fb_init( void )
{
  int res;
  int                               mode_from_cmdline;
  bcm2835_init_frame_buffer_entries init_frame_buffer_entries;

  if ( fb_fix_info.smem_start != NULL ) {
    return RPI_FB_INIT_ALREADY_INITIALIZED;
  }

  if ( rpi_fb_hdmi_is_present() == false ) {
    return RPI_FB_INIT_NO_DISPLAY;
  }

  mode_from_cmdline = parse_mode_from_string( &fb_var_info,
    rpi_cmdline_get_arg( "--video=" ) );

  switch ( mode_from_cmdline ) {
    case BAD_FORMAT:
      return RPI_FB_INIT_CMDLINE_BAD_FORMAT;
    case AUTO_SELECT:
      break;
    case DONT_INIT:
      return RPI_FB_INIT_CMDLINE_DONT_INIT;
    case NO_MODE_REQ:
      return RPI_FB_INIT_CMDLINE_NO_MODE_REQ;
  }

  if ( mode_from_cmdline ) {
    if ( find_mode_from_vc() )
      return RPI_FB_INIT_MODE_PROBE_ERROR;
  }

  memset( &init_frame_buffer_entries, 0, sizeof( init_frame_buffer_entries ) );
  init_frame_buffer_entries.xres = fb_var_info.xres;
  init_frame_buffer_entries.yres = fb_var_info.yres;
  init_frame_buffer_entries.xvirt = fb_var_info.xres;
  init_frame_buffer_entries.yvirt = fb_var_info.yres;
  init_frame_buffer_entries.depth = fb_var_info.bits_per_pixel;
  init_frame_buffer_entries.pixel_order = bcm2835_mailbox_pixel_order_rgb;
  init_frame_buffer_entries.alpha_mode = bcm2835_mailbox_alpha_mode_0_opaque;
  init_frame_buffer_entries.voffset_x = 0;
  init_frame_buffer_entries.voffset_y = 0;
  init_frame_buffer_entries.overscan_left = 0;
  init_frame_buffer_entries.overscan_right = 0;
  init_frame_buffer_entries.overscan_top = 0;
  init_frame_buffer_entries.overscan_bottom = 0;
  printk("bcm2835_mailbox_init_frame_buffer ...\n");
  res = bcm2835_mailbox_init_frame_buffer( &init_frame_buffer_entries );
  printk("bcm2835_mailbox_init_frame_buffer returned %d\n", res);
  if (res != 0) {
    printk("bcm2835_mailbox_init_frame_buffer retry ...\n");
    res = bcm2835_mailbox_init_frame_buffer( &init_frame_buffer_entries );
    printk("bcm2835_mailbox_init_frame_buffer returned %d\n", res);
    if (res != 0)
      return RPI_FB_INIT_SETUP_FAILED;
  }

  bcm2835_get_pitch_entries get_pitch_entries;
  bcm2835_mailbox_get_pitch( &get_pitch_entries );

  fb_var_info.xres = init_frame_buffer_entries.xres;
  fb_var_info.yres = init_frame_buffer_entries.yres;
  fb_var_info.bits_per_pixel = init_frame_buffer_entries.depth;
  fb_fix_info.smem_start = (void *) init_frame_buffer_entries.base;
  fb_fix_info.smem_len = init_frame_buffer_entries.size;
  fb_fix_info.line_length = get_pitch_entries.pitch;

  if ( fb_fix_info.smem_start == NULL )
    return RPI_FB_INIT_START_ADDR_UNKNOWN;

  printk("fb_fix_info.smem_start %p\n", fb_fix_info.smem_start);

  arm_cp15_set_translation_table_entries( (void *) fb_fix_info.smem_start,
    (void *) fb_fix_info.smem_start +
    fb_fix_info.smem_len,
    ARMV7_MMU_DATA_READ_WRITE_CACHED );

  return RPI_FB_INIT_OK;
}

/*
 * fbds device driver initialize entry point.
 */

rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  rtems_status_code status;

  /* register the devices */
  status = rtems_io_register_name( FRAMEBUFFER_DEVICE_0_NAME, major, 0 );

  if ( status != RTEMS_SUCCESSFUL ) {
    printk( "[!] error registering framebuffer\n" );
    rtems_fatal_error_occurred( status );
  }

  _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver open operation.
 */

rtems_device_driver frame_buffer_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  if ( _Atomic_Flag_test_and_set( &driver_mutex,
         ATOMIC_ORDER_ACQUIRE ) != 0 ) {
    printk( "RaspberryPi framebuffer could not lock driver_mutex\n" );
    return RTEMS_UNSATISFIED;
  }

  if ( fb_fix_info.smem_start == NULL ) {
    int res;
    res = rpi_fb_init();
    if ( (res < RPI_FB_INIT_OK) || (fb_fix_info.smem_start == NULL) ) {
      _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
      printk( "RaspberryPi framebuffer initialization failed\n" );
      return RTEMS_UNSATISFIED;
    }
  }

  memset( (void *) fb_fix_info.smem_start, 0, fb_fix_info.smem_len );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver close operation.
 */

rtems_device_driver frame_buffer_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  /* restore previous state.  for VGA this means return to text mode.
   * leave out if graphics hardware has been initialized in
   * frame_buffer_initialize() */
  memset( (void *) fb_fix_info.smem_start, 0, fb_fix_info.smem_len );
  _Atomic_Flag_clear( &driver_mutex, ATOMIC_ORDER_RELEASE );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver read operation.
 */

rtems_device_driver frame_buffer_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;

  rw_args->bytes_moved =
    ( ( rw_args->offset + rw_args->count ) > fb_fix_info.smem_len ) ?
    ( fb_fix_info.smem_len - rw_args->offset ) : rw_args->count;
  memcpy( rw_args->buffer,
    (const void *) ( fb_fix_info.smem_start + rw_args->offset ),
    rw_args->bytes_moved );
  return RTEMS_SUCCESSFUL;
}

/*
 * fbds device driver write operation.
 */

rtems_device_driver frame_buffer_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;

  rw_args->bytes_moved =
    ( ( rw_args->offset + rw_args->count ) > fb_fix_info.smem_len ) ?
    ( fb_fix_info.smem_len - rw_args->offset ) : rw_args->count;
  memcpy( (void *) ( fb_fix_info.smem_start + rw_args->offset ),
    rw_args->buffer,
    rw_args->bytes_moved );
  return RTEMS_SUCCESSFUL;
}

/*
 * ioctl entry point.
 */

rtems_device_driver frame_buffer_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  /* XXX check minor */

  switch ( args->command ) {
    case FBIOGET_VSCREENINFO:
      memcpy( args->buffer, &fb_var_info, sizeof( fb_var_info ) );
      args->ioctl_return = 0;
      break;
    case FBIOGET_FSCREENINFO:
      memcpy( args->buffer, &fb_fix_info, sizeof( fb_fix_info ) );
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
