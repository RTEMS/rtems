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
 *    Chris Johns
 *
 *  $Id$
 */

#ifndef __RTEMS_SHELL_H__
#define __RTEMS_SHELL_H__

#include <rtems.h>
#include <stdio.h>
#include <termios.h>
#include <rtems/fs.h>
#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rtems_shell_command_t)(int argc,char * argv[]);

struct rtems_shell_cmd_tt;
typedef struct rtems_shell_cmd_tt rtems_shell_cmd_t;

struct rtems_shell_cmd_tt {
  char                  *name;
  char                  *usage;
  char                  *topic;
  rtems_shell_command_t  command;
  rtems_shell_cmd_t     *alias;
  rtems_shell_cmd_t     *next;
};

typedef struct {
  char            *name;
  char            *alias;
} rtems_shell_alias_t;

rtems_shell_cmd_t * rtems_shell_lookup_cmd(char * cmd);

rtems_shell_cmd_t *rtems_shell_add_cmd_struct(
  rtems_shell_cmd_t *shell_cmd
);

rtems_shell_cmd_t * rtems_shell_add_cmd(
  char                  *cmd,
  char                  *topic,
  char                  *usage,
  rtems_shell_command_t  command
);

rtems_shell_cmd_t * rtems_shell_alias_cmd(
  char *cmd,
  char *alias
);

int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p, 
  char **argv_p, 
  int    max_args
);

int rtems_shell_scanline(char * line,int size,FILE * in,FILE * out) ;
void rtems_shell_cat_file(FILE * out,char *name);
void rtems_shell_write_file(char *name,char * content);

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
rtems_status_code rtems_shell_init(
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
int rtems_shell_str2int(char * s);

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
} rtems_shell_env_t;

rtems_boolean rtems_shell_shell_loop(
  rtems_shell_env_t *rtems_shell_env
);

extern rtems_shell_env_t  rtems_global_shell_env;
extern rtems_shell_env_t *rtems_current_shell_env;

/*
 * The types of file systems we can mount. We have them broken out
 * out like this so they can be configured by shellconfig.h. The
 * mount command needs special treatment due to some file systems
 * being dependent on the network stack and some not. If we had
 * all possible file systems being included it would force the
 * networking stack into the applcation and this may not be
 * required.
 */ 
struct rtems_shell_filesystems_tt;
typedef struct rtems_shell_filesystems_tt rtems_shell_filesystems_t;
typedef int (*rtems_shell_filesystems_mounter_t)(const char*                driver,
                                                 const char*                path,
                                                 rtems_shell_filesystems_t* fs,
                                                 rtems_filesystem_options_t options);
struct rtems_shell_filesystems_tt {
  const char*                        name;
  int                                driver_needed;
  rtems_filesystem_operations_table* fs_ops;
  rtems_shell_filesystems_mounter_t  mounter;
};

#ifdef __cplusplus
}
#endif

#endif
