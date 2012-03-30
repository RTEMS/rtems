#include <rtems.h>
#include <rtems/libio.h>

#include <string.h>

ssize_t app_memory_write(int minor, const char* buf, size_t len)
__attribute__(( weak, alias("__bsp_memory_write") ));

ssize_t __bsp_memory_write(int minor, const char* buf, size_t len);
rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void*                     arg);
rtems_device_driver console_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void*                     arg);
rtems_device_driver console_close(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void*                     arg);
rtems_device_driver console_read(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void*                     arg);
rtems_device_driver console_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void*                     arg);
rtems_device_driver console_control(rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void*                     arg);


ssize_t __bsp_memory_write(int minor, const char* buf, size_t len)
{
  const char* const last = buf+len;
  while (buf < last)
  {
    BSP_output_char(*buf++);
  }
  return len;
}

static rtems_termios_callbacks gMemCallbacks = {
        0,                /* firstOpen */
        0,                /* lastClose */
        0,                /* PollRead */
        app_memory_write, /* write */
        0,                /* SetAttr */
        0,                /* stopRemoteTx */
        0,                /* startRemoteTx */
        0                 /* outputUsesInterrupts */
};

rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void*                     arg)
{
  rtems_status_code status;

  rtems_termios_initialize();

  status = rtems_io_register_name("/dev/console", major, 0);

  if (status != RTEMS_SUCCESSFUL)  rtems_fatal_error_occurred (status);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void*                     arg)
{
  rtems_status_code sc;

  sc = rtems_termios_open (major, minor, arg, &gMemCallbacks);

  return sc;
}

rtems_device_driver console_close(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void*                     arg)
{
  return rtems_termios_close(arg);
}

rtems_device_driver console_read(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void*                     arg)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void*                     arg)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void*                     arg)
{
  return rtems_termios_ioctl(arg);
}
