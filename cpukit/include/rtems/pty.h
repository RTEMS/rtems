/*
 * /dev/ptyXX  (A first version for pseudo-terminals)
 *
 *  Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _RTEMS_PTY_H
#define _RTEMS_PTY_H

#ifdef __cplusplus
extern "C" {
#endif	

#include <rtems/termiostypes.h>

#define RTEMS_PTY_SB_MAX 16

typedef struct {
  rtems_termios_device_context  base;
  rtems_termios_tty            *ttyp;
  tcflag_t                      c_cflag;
  int                           socket;
  int                           last_cr;
  unsigned                      iac_mode;
  unsigned char                 sb_buf[RTEMS_PTY_SB_MAX];
  int                           sb_ind;
  int                           width;
  int                           height;
  char                          name[sizeof("/dev/pty18446744073709551615")];
} rtems_pty_context;

const char *rtems_pty_initialize(rtems_pty_context *pty, uintptr_t unique);

RTEMS_INLINE_ROUTINE const char *rtems_pty_get_path(const rtems_pty_context *pty)
{
  return pty->name;
}

void rtems_pty_close_socket(rtems_pty_context *pty);

void rtems_pty_set_socket(rtems_pty_context *pty, int socket);

#ifdef __cplusplus
}
#endif	

#endif
