/*
 * console.c :  ARM GBA BSP
 *
 * This file contains the GBA console I/O package.
 *
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <irq.h>
#include <s3c2400.h>
#include <conio.h>

/*
 * BSP initialization
 */

static int gp32_pollRead(int minor)
{
    return(gpconio_getch());
}

static int gp32_write(int minor, const char *buf, int len)
{
    int i;

    for(i = 0; i < len; i++) {
        gpconio_putch((unsigned short)buf[i]);
    }

    return len;
}


/* for printk support */
BSP_output_char_function_type     BSP_output_char = (BSP_output_char_function_type) gpconio_putch;
BSP_polling_getchar_function_type BSP_poll_char   = (BSP_polling_getchar_function_type) gpconio_getch;

/*-------------------------------------------------------------------------+
| Console device driver INITIALIZE entry point.
+--------------------------------------------------------------------------+
| Initilizes the I/O console (keyboard + VGA display) driver.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
    rtems_status_code status;

    /*
     * Set up TERMIOS
     */
    rtems_termios_initialize ();

    /*
     * Do device-specific initialization
     */
    InitConIO();

    /*
     * Register the device
     */
    status = rtems_io_register_name ("/dev/console", major, 0);
    if (status != RTEMS_SUCCESSFUL) {
        printk("Error registering console device!\n");
        rtems_fatal_error_occurred (status);
    }

    return RTEMS_SUCCESSFUL;
}


/*-------------------------------------------------------------------------+
| Console device driver OPEN entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_open(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
    rtems_status_code              status;
    static rtems_termios_callbacks cb =
        {
            NULL,         /* firstOpen     */
            NULL,         /* lastClose     */
            gp32_pollRead,               /* pollRead      */
            gp32_write,                  /* write         */
            NULL,          /* setAttributes */
            NULL,                       /* stopRemoteTx  */
            NULL,                       /* startRemoteTx */
            TERMIOS_POLLED              /* 1 = outputUsesInterrupts */
        };

    status = rtems_termios_open (major, minor, arg, &cb);
    
    if(status != RTEMS_SUCCESSFUL) {
        printk("Error openning console device\n");
        return status;
    }

    return RTEMS_SUCCESSFUL;
}

/*-------------------------------------------------------------------------+
| Console device driver CLOSE entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
    rtems_device_driver res = RTEMS_SUCCESSFUL;

    res =  rtems_termios_close (arg);
    return res;
}


/*-------------------------------------------------------------------------+
| Console device driver READ entry point.
+--------------------------------------------------------------------------+
| Read characters from the I/O console. We only have stdin.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
    return rtems_termios_read (arg);

}


/*-------------------------------------------------------------------------+
| Console device driver WRITE entry point.
+--------------------------------------------------------------------------+
| Write characters to the I/O console. Stderr and stdout are the same.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                    * arg)
{
    return rtems_termios_write (arg);
}



/*
 * Handle ioctl request.
 */
rtems_device_driver
console_control(rtems_device_major_number major,
        rtems_device_minor_number minor,
        void                      * arg
)
{
    return rtems_termios_ioctl (arg);
}
