/**
 * @file
 *
 * @ingroup Console
 *
 * @brief  Extension of the generic libchip console driver shell
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _PC386_CONSOLE_PRIVATE_h
#define _PC386_CONSOLE_PRIVATE_h

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

extern rtems_device_minor_number  BSPPrintkPort;

/**
 *  @brief bsp_console_select
 *
 *  This function selects the port to be used as console
 *
 */
void bsp_console_select(void);

/**
 *  @brief bsp_com_outch
 *
 *  This function puts a character out of the console port.
 *
 *  @param[in] ch specifies the character to write
 */
extern void bsp_com_outch(char ch);

/**
 *  @brief bsp_com_inch
 *
 *  This function gets a character from the console
 *  port.
 *
 *  @return This method returns the character that
 *    was retrieved from the console port.
 */
extern int bsp_com_inch(void);

/**
 *  @brief
 *
 *  This function
 *
 *  @return This method returns
 */
int vt_ioctl( unsigned int cmd, unsigned long arg);

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
