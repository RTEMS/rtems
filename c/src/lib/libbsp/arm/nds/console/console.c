/*
 * RTEMS for Nintendo DS console driver.
 *
 * Copyright (c) 2008 by Renaud Voltz <renaud.voltz@gmail.com>
 *                       Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <bsp.h>
#include <nds.h>
#include <rtems/libio.h>
#include <nds/arm9/console.h>
#include <sys/iosupport.h>

#include <rtems/mw_uid.h>

/*
 * enables testsuite output to desmume. this is used to pass the rtems
 * testsuite.
 * comment the following line to disable (recommended).
 */

//#define TESTSUITE

/*
 * printk support.
 */

/* from NDS support library */
extern void consolePrintChar(char c);
void
nds_putch (char c)
{
#ifdef TESTSUITE
  __asm__ volatile ("swi $0x1");
#endif
  consolePrintChar (c);
}

static volatile char ch = 0;

void
console_push (char c)
{
  ch = c;
}

int
nds_getch (void)
{
  char c;

  while (ch == 0);
  c = ch;
  ch = 0;
  return c;
}

BSP_output_char_function_type BSP_output_char = nds_putch;
BSP_polling_getchar_function_type BSP_poll_char = nds_getch;

/*
 * console write operation.
 */

static ssize_t
nds_write (int minor, const char *buf, size_t len)
{
  int count;

  for (count = 0; count < len; count++) {
    nds_putch (buf[count]);
  }

  return 0;
}

/*
 * console read operation.
 */

static int
nds_read (int minor)
{
  return nds_getch ();
}

/*
 * from touchscreen/parser.c
 */

void register_kbd_msg_queue (char *q_name);
void unregister_kbd_msg_queue (void);

/*
 * Console driver
 */

rtems_device_driver
console_initialize (rtems_device_major_number major,
                    rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;

  printk ("[+] console started\n");

  rtems_termios_initialize ();

  status = rtems_io_register_name ("/dev/console", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] error registering console\n");
    rtems_fatal_error_occurred (status);
  }

  return (RTEMS_SUCCESSFUL);
}

rtems_device_driver
console_open (rtems_device_major_number major,
              rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;
  static rtems_termios_callbacks cb = {
    NULL,                       /* firstOpen     */
    NULL,                       /* lastClose     */
    nds_read,                   /* pollRead      */
    nds_write,                  /* write         */
    NULL,                       /* setAttributes */
    NULL,                       /* stopRemoteTx  */
    NULL,                       /* startRemoteTx */
    0                           /* 1 = outputUsesInterrupts */
  };

  status = rtems_termios_open (major, minor, arg, &cb);
  if (status != RTEMS_SUCCESSFUL)
    printk ("[!] error opening console\n");

  return (status);
}

rtems_device_driver
console_close (rtems_device_major_number major,
               rtems_device_minor_number minor, void *arg)
{
  rtems_device_driver res = RTEMS_SUCCESSFUL;

  res = rtems_termios_close (arg);

  return res;
}

rtems_device_driver
console_read (rtems_device_major_number major,
              rtems_device_minor_number minor, void *arg)
{
  return rtems_termios_read (arg);
}

rtems_device_driver
console_write (rtems_device_major_number major,
               rtems_device_minor_number minor, void *arg)
{
  return rtems_termios_write (arg);
}

rtems_device_driver
console_control (rtems_device_major_number major,
                 rtems_device_minor_number minor, void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
  case MW_UID_REGISTER_DEVICE:
    register_kbd_msg_queue (args->buffer);
    break;
  case MW_UID_UNREGISTER_DEVICE:
    unregister_kbd_msg_queue ();
    break;
  default:
    return rtems_termios_ioctl (arg);
  }
  args->ioctl_return = 0;

  return RTEMS_SUCCESSFUL;
}
