/*
 *  BASED UPON SOURCE IN RTEMS, MODIFIED FOR SIMULATOR
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#ifndef __SHELL_h
#define __SHELL_h

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rtems_shell_command_t)(int argc, char **argv);

struct rtems_shell_cmd_tt;
typedef struct rtems_shell_cmd_tt rtems_shell_cmd_t;

struct rtems_shell_cmd_tt {
  const char            *name;
  const char            *usage;
  const char            *topic;
  rtems_shell_command_t  command;
  rtems_shell_cmd_t     *alias;
  rtems_shell_cmd_t     *next;
};

typedef struct {
  const char *name;
  const char *alias;
} rtems_shell_alias_t;

void rtems_shell_initialize_command_set(void);

rtems_shell_cmd_t * rtems_shell_lookup_cmd(const char *cmd);

rtems_shell_cmd_t * rtems_shell_alias_cmd(
  const char *cmd,
  const char *alias
);

int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p,
  char **argv_p,
  int    max_args
);

#ifdef __cplusplus
}
#endif

#endif
