/*
 * umon.h - RTEMS specific interface to MicroMonitor.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 *  from NCB - Sistemas Embarcados Ltda. (Brazil)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/

#ifndef __rtems_umon_h
#define __rtems_umon_h

#include <umon/monlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Connect RTEMS Application to MicroMonitor
 *
 * RTEMS uMonitor wrapper for monConnect().  This deals with
 * getting MONCOMPTR as well as providing the locking routines.
 */
void rtems_umon_connect(void);

/**
 * @brief Obtain BSP specific pointer to MicroMonitor
 *
 * BSP specific routine to help when calling monConnect().  This
 * returns the value known to uMon as MONCOMPTR.
 *
 * @return This method returns the address of the base area of MicroMonitor.
 */
void *rtems_bsp_get_umon_monptr(void);

/**
 * @brief Initialize the TFS-RTEMS file system
 *
 * This method initializes and mounts the TFS-RTEMS file system.
 *
 * @param[in] path is the mount point
 *
 * @return This method returns 0 on success.
 */
int rtems_initialize_tfs_filesystem(
  const char *path
);

/**
 * @brief Read from MicroMonitor Port
 *
 * This method reads a character from the MicroMonitor console.
 *
 * @return This method returns a character.
 */
int umoncons_poll_read(int minor);

#ifdef __cplusplus
}
#endif

#endif /* __rtems_umon_h */
