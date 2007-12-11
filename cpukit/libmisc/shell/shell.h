/**
 * @file rtems/shell.h
 *
 *  Instantatiate a new terminal shell.
 */

/*
 *  Author:
 *
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 *   Thanks at:
 *    Chris John
 *
 *  $Id$
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include <rtems.h>
#include <stdio.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*shell_command_t)(int argc,char * argv[]);

struct shell_cmd_tt;
typedef struct shell_cmd_tt shell_cmd_t;

struct shell_cmd_tt {
  char            *name;
  char            *usage;
  char            *topic;
  shell_command_t  command;
  shell_cmd_t     *alias;
  shell_cmd_t     *next;
};

typedef struct {
  char            *name;
  char            *alias;
} shell_alias_t;

shell_cmd_t * shell_lookup_cmd(char * cmd);

shell_cmd_t *shell_add_cmd_struct(
  shell_cmd_t *shell_cmd
);

shell_cmd_t * shell_add_cmd(
  char            *cmd,
  char            *topic,
  char            *usage,
  shell_command_t  command
);

shell_cmd_t * shell_alias_cmd(
  char *cmd,
  char *alias
);

int shell_make_args(
  char  *commandLine,
  int   *argc_p, 
  char **argv_p, 
  int    max_args
);

int shell_scanline(char * line,int size,FILE * in,FILE * out) ;
void cat_file(FILE * out,char *name);
void write_file(char *name,char * content);

/**
 * Initialise the shell creating tasks to login and run the shell
 * sessions.
 *
 * @param task_name Name of the shell task.
 * @param task_stacksize The size of the stack. If 0 the default size is used.
 * @param task_priority The priority the shell runs at.
 * @param tcflag The termios c_cflag value. If 0 the default is used, if
 *               not 0 the value is ORed with CLOCAL and CREAD.
 * @param forever Repeat logins.
 *
 * @todo CCJ I am not sure this termios flag setting is a good idea. The shell
 *           needs to adjust the termios for its use but it should assume the
 *           settings are set by the user for things like baudrate etc.
 */
rtems_status_code shell_init(
  char                *task_name,
  uint32_t             task_stacksize,  /*0 default*/
  rtems_task_priority  task_priority,
  char                *devname,
  tcflag_t             tcflag,
  int                  forever
);

/*
 *  Things that are useful to external entities developing commands and plugging
 *  them in.
 */
int str2int(char * s);

typedef struct  {
  rtems_name  magic; /* 'S','E','N','V': Shell Environment */
  char       *devname;
  char       *taskname;
  tcflag_t    tcflag;
  /* user extensions */
  int         exit_shell; /* logout */
  int         forever   ; /* repeat login */
  int         errorlevel;
  uintptr_t   mdump_addr;
} shell_env_t;

rtems_boolean shell_shell_loop(
  shell_env_t *shell_env
);

extern shell_env_t  global_shell_env;
extern shell_env_t *current_shell_env;

#ifdef __cplusplus
}
#endif

#endif
