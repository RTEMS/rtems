/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return RTEMS_SUCCESSFUL;
}

/*
 *  Open entry point
 */

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return RTEMS_SUCCESSFUL;
}

/*
 *  Close entry point
 */

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return RTEMS_SUCCESSFUL;
}

/*
 * read bytes from the serial port. We only have stdin.
 */

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return RTEMS_SUCCESSFUL;
}

/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return 0;
}

/*
 *  IO Control entry point
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  return RTEMS_SUCCESSFUL;
}

BSP_output_char_function_type     BSP_output_char = NULL;
BSP_polling_getchar_function_type BSP_poll_char   = NULL;
