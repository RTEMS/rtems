@c
@c  COPYRIGHT (c) 1988-2009.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Frame Buffer Driver

In this chapter, we present the basic functionality implemented by a
frame buffer driver: @code{frame_buffer_initialize()}, @code{frame_buffer_open()},
@code{frame_buffer_close()}, @code{frame_buffer_read()}, @code{frame_buffer_write()}
and @code{frame_buffer_control()}.

@section Introduction

The purpose of the frame buffer driver is to provide an abstraction for 
graphics hardware.
By using the frame buffer interface, an application can display graphics 
without knowing anything about the low-level details of interfacing to a
particular graphics adapter. The parameters governing the mapping of
memory to displayed pixels (planar or linear, bit depth, etc) is still
implementation-specific, but device-independent methods are provided to
determine and potentially modify these parameters.

The frame buffer driver is commonly located in the @code{console}
directory of the BSP and registered by the name @b{/dev/fb0}.
Additional frame buffers (if available) are named @b{/dev/fb1},
@b{/dev/fb2}, etc.

To work with the frame buffer, the following operation sequence is used:
@code{open()}, @code{ioctls()} to get the frame buffer info, @code{read()} and/or
@code{write()}, and @code{close()}.  

@section Driver Function Overview

@subsection Initialization

The driver initialization is called once during the RTEMS initialization
process and returns RTEMS_SUCCESSFUL when the device driver is successfully
initialized. During the initialization, a name is assigned to the frame
buffer device.  If the graphics hardware supports console text output,
as is the case with the pc386 VGA hardware, initialization into graphics
mode may be deferred until the device is @code{open()}ed.

The @code{frame_buffer_initialize()} function may look like this:

@example
@group
rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg)
@{
  rtems_status_code status;

  printk( "frame buffer driver initializing..\n" );

  /*
   * Register the device
   */
  status = rtems_io_register_name("/dev/fb0", major, 0);
  if (status != RTEMS_SUCCESSFUL) 
  @{
    printk("Error registering frame buffer device!\n");
    rtems_fatal_error_occurred( status );
  @}

   /*
    * graphics hardware initialization goes here for non-console
    * devices
    */

    return RTEMS_SUCCESSFUL;
@}
@end group
@end example

@subsection Opening the Frame Buffer Device

The @code{frame_buffer_open()} function is called whenever a frame buffer device is opened.
If the frame buffer is registered as "/dev/fb0", the @code{frame_buffer_open} entry point
will be called as the result of an  @code{open("/dev/fb0", mode)} in the application.

Thread safety of the frame buffer driver is implementation-dependent.
The VGA driver shown below uses a mutex to prevent multiple open()
operations of the frame buffer device.

The @code{frame_buffer_open()} function returns RTEMS_SUCCESSFUL when the device driver
is successfully opened, and RTEMS_UNSATISFIED if the device is already open:

@example
@group
rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  if (pthread_mutex_unlock(&mutex) == 0)@{
    /* restore previous state.  for VGA this means return to text mode.
     * leave out if graphics hardware has been initialized in
     * frame_buffer_initialize() */
    ega_hwterm();
    printk( "FBVGA close called.\n" );
    return RTEMS_SUCCESSFUL;
  @}

  return RTEMS_UNSATISFIED;
@}
@end group
@end example

In the previous example, the function @code{ega_hwinit()} takes care of
hardware-specific initialization.

@subsection Closing the Frame Buffer Device

The @code{frame_buffer_close()} is invoked when the frame buffer device
is closed.  It frees up any resources allocated in
@code{frame_buffer_open()}, and should restore previous hardware state.
The entry point corresponds to the device driver close entry point.

Returns RTEMS_SUCCESSFUL when the device driver is successfully closed:

@example
@group
rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg)
@{
  pthread_mutex_unlock(&mutex);
  /* TODO check mutex return value, RTEMS_UNSATISFIED if it failed.  we
   * don't want to unconditionally call ega_hwterm()... */
  /* restore previous state.  for VGA this means return to text mode.
   * leave out if graphics hardware has been initialized in
   * frame_buffer_initialize() */
  ega_hwterm();
  printk( "frame buffer close called.\n" );
  return RTEMS_SUCCESSFUL;
@}

@end group
@end example


@subsection Reading from the Frame Buffer Device

The @code{frame_buffer_read()} is invoked from a @code{read()} operation
on the frame buffer device.
Read functions should allow normal and partial reading at the end of frame buffer memory.  
This method returns RTEMS_SUCCESSFUL when the device is successfully read from:

@example
@group
rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy(rw_args->buffer, (const void *) (fb_fix.smem_start + rw_args->offset), rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;   
@}
@end group
@end example

@subsection Writing to the Frame Buffer Device

The @code{frame_buffer_write()} is invoked from a @code{write()}
operation on the frame buffer device.
The frame buffer write function is similar to the read function, and
should handle similar cases involving partial writes.

This method returns RTEMS_SUCCESSFUL when the device is successfully
written to:

@example
@group
rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy( (void *) (fb_fix.smem_start + rw_args->offset), rw_args->buffer, rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;          
@}
@end group
@end example


@subsection Frame Buffer IO Control

The frame buffer driver allows several ioctls, partially compatible with
the Linux kernel,
to obtain information about the hardware.

All @code{ioctl()} operations on the frame buffer device invoke
@code{frame_buffer_control()}.

Ioctls supported:
@itemize @bullet
@item ioctls to get the frame buffer screen info (fixed and variable).
@item ioctl to set and get palette.
@end itemize

@example
@group
rtems_device_driver frame_buffer_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  rtems_libio_ioctl_args_t *args = arg;

  printk( "FBVGA ioctl called, cmd=%x\n", args->command  );

  switch( args->command ) @{
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
  @}
  return RTEMS_SUCCESSFUL;
@}
@end group
@end example

See @code{rtems/fb.h} for more information on the list of ioctls and
data structures they work with.
