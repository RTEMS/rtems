/*
 *  Shell Internal Information
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SHELL_INTERNAL_H
#define _RTEMS_SHELL_INTERNAL_H

#include "shell.h"

struct rtems_shell_topic_tt;
typedef struct rtems_shell_topic_tt rtems_shell_topic_t;

struct rtems_shell_topic_tt {
  const char          *topic;
  rtems_shell_topic_t *next;
};


extern rtems_shell_cmd_t   * rtems_shell_first_cmd;
extern rtems_shell_topic_t * rtems_shell_first_topic;

rtems_shell_topic_t * rtems_shell_lookup_topic(const char *topic);


extern void rtems_shell_register_monitor_commands(void);
extern void rtems_shell_initialize_command_set(void);

extern void rtems_shell_print_heap_info(
  const char       *c,
  Heap_Information *h
);


extern int rtems_shell_main_mdump(int argc, char *argv[]);

extern void rtems_shell_print_unified_work_area_message(void);

extern int rtems_shell_main_rm(int argc, char *argv[]);
extern int rtems_shell_main_cp(int argc, char *argv[]);

#include <sys/types.h>

extern void strmode(mode_t mode, char *p);
extern const char *user_from_uid(uid_t uid, int nouser);
extern char *group_from_gid(gid_t gid, int nogroup);

#endif
