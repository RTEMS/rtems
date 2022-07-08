/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * umon.h - RTEMS specific interface to MicroMonitor.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 *  from NCB - Sistemas Embarcados Ltda. (Brazil)
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

#ifndef __rtems_umon_h
#define __rtems_umon_h

#include <umon/monlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Connect RTEMS Application to MicroMonitor
 *
 * RTEMS uMonitor wrapper for monConnect().  This will deal with the
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
