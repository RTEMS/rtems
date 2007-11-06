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

/*
 *  Initialize the telnetd subsystem.
 *
 *  cmd        - function which is the "shell" telnetd invokes
 *  arg        - context pointer to cmd
 *  dontSpawn  - TRUE if telnetd takes over this task.
 *               FALSE to create another task for the shell.
 *  stack      - stack size of spawned task 
 *  priority   - initial priority of spawned task
 */
int rtems_telnetd_initialize(
  void               (*cmd)(char *, void *),
  void                *arg,
  int                  dontSpawn,
  size_t               stack,
  rtems_task_priority  priority
);

#ifdef __cplusplus
}
#endif	

#endif
