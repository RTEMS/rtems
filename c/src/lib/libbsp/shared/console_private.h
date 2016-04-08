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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
