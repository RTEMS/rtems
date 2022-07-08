/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libchip/serial.h>
#include "../../shared/dev/serial/legacy-console.h"

/* select which serial port the mouse is connected to */
#if defined(SERIAL_MOUSE_COM2)
  #define MOUSE_DEVICE "/dev/com2"
#else
  #define MOUSE_DEVICE "/dev/com1"
#endif

static const char *SerialMouseDevice = MOUSE_DEVICE;

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  const char *consname;

  *name = SerialMouseDevice;
  *type = "ms";

  /* Check if this port is not been used as console */
  consname = Console_Port_Tbl[ Console_Port_Minor ]->sDeviceName;
  if ( !strcmp(MOUSE_DEVICE, consname) ) {
    printk( "SERIAL MOUSE: port selected as console.(%s)\n", *name );
    rtems_fatal_error_occurred( -1 );
  }

  printk("Mouse Device: %s\n", *name );
  return name;
}
