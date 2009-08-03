@c
@c  COPYRIGHT (c) 1988-2009.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Frame Buffer Driver

@section Introduction

The purpose of the frame buffer driver is to provide an abstraction for the
graphics hardware.
By using the frame buffer interface the application can access the graphics 
without knowing anything about the low-level communication.

The frame buffer driver is usually located in the @code{console} directory
of the BSP and registered by the name @b{/dev/fb}

@section Driver Functioning Overview

@subsection Initialization

The driver initialization is called once during the RTEMS initialization
process and returns RTEMS_SUCCESSFUL when the device driver is successfully
initialized

The @code{frame_buffer_initialize} function may look like this:

@example
@group
rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  rtems_status_code status;

  printk( "frame buffer driver initializing..\n" );

  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/fb", major, 0);
  if (status != RTEMS_SUCCESSFUL) 
 @{
    printk("Error registering frame buffer device!\n");
    rtems_fatal_error_occurred( status );
 @}

    return RTEMS_SUCCESSFUL;
@}
@end group
@end example

@subsection Opening the Frame Buffer Device

The @code{frame_buffer_open} function is called whenever a frame buffer device is opened.
The device registered as @code{"/dev/fb"} (@code{FRAMEBUFFER_DEVICE_NAME}) is
opened automatically during RTEMS initialization.  For instance, if the frame buffer is
registered as "/dev/fb", the @code{frame_buffer_open} entry point will be called as the
result of an  @code{open("/dev/fb", mode)} in the application.

The @code{frame_buffer_open} function returns RTEMS_SUCCESSFUL when the device driver
is successfully opened:

@example
@group
rtems_device_driver frame_buffer_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  ega_hwinit();
  printk( "frame buffer open called.\n" );

  return RTEMS_SUCCESSFUL;
@}
@end group
@end example

The @code{ega_hwinit()} which takes care of the hardware initialization.

@subsection Closing the Frame Buffer Device

The @code{frame_buffer_close} is invoked when the serial device is to be closed.
This entry point corresponds to the device driver close entry point.

The @code{frame_buffer_close} function freeing up the hardware resources.
Returns RTEMS_SUCCESSFUL when the device driver is successfully closed:

@example
@group
rtems_device_driver frame_buffer_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  ega_hwterm();
  printk( "frame buffer close called.\n" );

  return RTEMS_SUCCESSFUL;
@}

@end group
@end example


@subsection Reading from the Frame Buffer Device

The @code{frame_buffer_read} is invoked when the serial device is to be read from.
This method returns RTEMS_SUCCESSFUL when the device is successfully read from:

@example
@group
rtems_device_driver frame_buffer_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  /*TBD*/
  return RTEMS_SUCCESSFUL;
@}
@end group
@end example

@subsection Writing to the Frame Buffer Device

The @code{frame_buffer_write} is invoked when the serial device is to be written to.
This entry point corresponds to the device driver write entry point.


@example
@group
rtems_device_driver frame_buffer_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  /*TBD*/
  return RTEMS_SUCCESSFUL;
@}
@end group
@end example


@section Frame Buffer IO Control

The frame buffer driver allows several ioctls partial compatible with the linux ones
to obtain information about the hardware.
Basically `ioctl' commands call `console_control' with the
serial line configuration wich handeles all the services on the interface.

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
  @}
  return RTEMS_SUCCESSFUL;
@}
@end group
@end example

Look into @code{rtems/fb.h} for more information on what ioctls exist and on
which data structures they work.
