/*
 *  Console IO Support Routines
 *
 *  These provide UNIX-like read and write calls for the C library.
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

#include <unistd.h>
#include <errno.h>

rtems_device_driver
console_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                    * arg
)
{
    return RTEMS_SUCCESSFUL;
}

