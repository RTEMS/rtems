/*
 *  Shell Internal Information
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SHELL_INTERNAL_H
#define _RTEMS_SHELL_INTERNAL_H

#include <rtems/shell.h>

extern rtems_shell_cmd_t   * rtems_shell_first_cmd;
extern rtems_shell_topic_t * rtems_shell_first_topic;

extern void rtems_shell_register_monitor_commands(void);

extern void rtems_shell_print_heap_info(
  const char             *c,
  const Heap_Information *h
);

extern void rtems_shell_print_heap_stats(
  const Heap_Statistics *s
);

extern void rtems_shell_print_unified_work_area_message(void);

#include <sys/types.h>

extern void strmode(mode_t mode, char *p);
extern const char *user_from_uid(uid_t uid, int nouser);
extern char *group_from_gid(gid_t gid, int nogroup);

#endif
