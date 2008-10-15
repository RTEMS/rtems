/*
 *  Original Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *  Reworked by Till Straumann and .h overhauled by Joel Sherrill.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_TELNETD_H
#define _RTEMS_TELNETD_H

#ifdef __cplusplus
extern "C" {
#endif	

/**
 *  This method initializes the telnetd subsystem.
 *
 *  @param[in] cmd is the function which is the "shell" telnetd invokes
 *  @param[in] arg is the context pointer to cmd
 *  @param[in] remainOnCallerSTDIO is set to TRUE if telnetd takes over the
 *    standard in, out and error associated with task.  In this case,
 *    it will be NOT be listening on any sockets.  When this parameters
 *    is FALSE the telnetd will create other tasks for the shell
 *    which listen on sockets.
 *  @param[in] stack is stack size of spawned task.
 *  @param[in] priority is the initial priority of spawned task(s).  If
 *    this parameter is less than 2, then the default priority of 100 is used.
 *  @param[in] askForPassword is set to TRUE if telnetd is to ask for a
 *    password. This is set to FALSE to invoke "cmd" with no password check.
 *    This may be OK if "cmd" includes its own check and indeed the RTEMS Shell
 *    uses a login with a user name and password so this is the usual case.
 */
int rtems_telnetd_initialize(
  void               (*cmd)(char *, void *),
  void                *arg,
  bool                 remainOnCallerSTDIO,
  size_t               stack,
  rtems_task_priority  priority,
  bool                 askForPassword
);

#ifdef __cplusplus
}
#endif	

#endif
