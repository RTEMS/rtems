/**
 * @file
 *
 * @ingroup arm_kinetis
 *
 * @brief Console configuration.
 */


#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/console.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/uart.h>

/* number of serial ports */
#define NUM_DEVS       KINETIS_UART_MAX_NUM

extern void termios_uart_init(int minor);
extern rtems_termios_callbacks termios_uart_cbacks;

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
    rtems_status_code status;

    termios_uart_init(KINETIS_UART_4);

    rtems_termios_initialize();

    /* /dev/console and /dev/tty0 are the same */
    status = rtems_io_register_name(CONSOLE_DEVICE_NAME, major, KINETIS_UART_4);
    if (status != RTEMS_SUCCESSFUL) {
        rtems_panic("%s:%d Error registering /dev/console :: %d\n",
                    __FUNCTION__, __LINE__, status);
    }

    status = rtems_io_register_name("/dev/tty0", major, KINETIS_UART_4);
    if (status != RTEMS_SUCCESSFUL) {
        rtems_panic("%s:%d Error registering /dev/tty0 :: %d\n",
                    __FUNCTION__, __LINE__, status);
    }

    return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    rtems_status_code rc;

    if (minor > (NUM_DEVS - 1)) {
        return RTEMS_INVALID_NUMBER;
    }

    rc = rtems_termios_open(major, minor, arg, &termios_uart_cbacks);

    return rc;
}

rtems_device_driver console_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
    return rtems_termios_close(arg);
}

rtems_device_driver console_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_read(arg);
}

rtems_device_driver console_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_write(arg);
}

rtems_device_driver console_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
)
{
  return rtems_termios_ioctl(arg);
}

