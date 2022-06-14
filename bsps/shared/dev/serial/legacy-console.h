/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Console
 *
 * @brief  Extension of the generic libchip console driver shell
 */

/*
 *  COPYRIGHT (c) 1989-2011, 2016.
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

#ifndef _PC386_CONSOLE_PRIVATE_h
#define _PC386_CONSOLE_PRIVATE_h

#include <rtems.h>

#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

extern rtems_device_minor_number  BSPPrintkPort;

/**
 *  @brief
 *
 *  This function is the ioctl() support for a VGA buffer driver.
 *
 *  @return This method returns 0 on success.
 */
int vt_ioctl(
  unsigned int cmd,
  unsigned long arg
);

/**
 *  @brief console_find_console_entry
 *
 *  This method is used to search the console entries for a
 *  specific device entry and return it. If match is NULL the
 *  minor number provided is matched.
 */
console_tbl* console_find_console_entry(
  const char                *match,
  size_t                     length,
  rtems_device_minor_number *match_minor
);

/**
 *  @brief console_initialize_data
 *
 *  This must be called before dynamic registration of devices can occur.
 *  It is normally called as a side-effect of @a console_initialize() but
 *  if a probe and dynamic registration occurs before that, then this method
 *  should be explicitly invoked.
 */
void console_initialize_data(void);

/**
 *  @brief console_register_devices
 *
 *  This function expands the console table to include previous
 *  ports and the array of new ports specified.
 *
 *  @param[in] new_ports specifies an array of new ports to register
 *  @param[in] number_of_ports specifies the number of elements
 *         in the new_ports array
 *
 */
void console_register_devices(
  console_tbl *new_ports,
  size_t       number_of_ports
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
