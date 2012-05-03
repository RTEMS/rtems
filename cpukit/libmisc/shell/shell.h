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
 */

#ifndef __RTEMS_SHELL_H__
#define __RTEMS_SHELL_H__

#include <rtems.h>
#include <stdio.h>
#include <termios.h>
#include <rtems/fs.h>
#include <rtems/libio.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some key labels to define special keys.
 */

#define RTEMS_SHELL_KEYS_EXTENDED    (0x8000)
#define RTEMS_SHELL_KEYS_NORMAL_MASK (0x00ff)
#define RTEMS_SHELL_KEYS_INS         (0)
#define RTEMS_SHELL_KEYS_DEL         (1)
#define RTEMS_SHELL_KEYS_UARROW      (2)
#define RTEMS_SHELL_KEYS_DARROW      (3)
#define RTEMS_SHELL_KEYS_LARROW      (4)
#define RTEMS_SHELL_KEYS_RARROW      (5)
#define RTEMS_SHELL_KEYS_HOME        (6)
#define RTEMS_SHELL_KEYS_END         (7)
#define RTEMS_SHELL_KEYS_F1          (8)
#define RTEMS_SHELL_KEYS_F2          (9)
#define RTEMS_SHELL_KEYS_F3          (10)
#define RTEMS_SHELL_KEYS_F4          (11)
#define RTEMS_SHELL_KEYS_F5          (12)
#define RTEMS_SHELL_KEYS_F6          (13)
#define RTEMS_SHELL_KEYS_F7          (14)
#define RTEMS_SHELL_KEYS_F8          (15)
#define RTEMS_SHELL_KEYS_F9          (16)
#define RTEMS_SHELL_KEYS_F10         (17)

typedef bool (*rtems_shell_login_check_t)(
  const char * /* user */,
  const char * /* passphrase */
);

extern bool rtems_shell_login_prompt(
  FILE *in,
  FILE *out,
  const char *device,
  rtems_shell_login_check_t check
);

extern bool rtems_shell_login_check(
  const char *user,
  const char *passphrase
);

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

/*
 * The return value has RTEMS_SHELL_KEYS_EXTENDED set if the key
 * is extended, ie a special key.
 */
extern unsigned int rtems_shell_getchar(FILE *in);

extern rtems_shell_cmd_t * rtems_shell_lookup_cmd(const char *cmd);

extern rtems_shell_cmd_t *rtems_shell_add_cmd_struct(
  rtems_shell_cmd_t *shell_cmd
);

rtems_shell_cmd_t * rtems_shell_add_cmd(
  const char            *cmd,
  const char            *topic,
  const char            *usage,
  rtems_shell_command_t  command
);

extern rtems_shell_cmd_t * rtems_shell_alias_cmd(
  const char *cmd,
  const char *alias
);

extern int rtems_shell_make_args(
  char  *commandLine,
  int   *argc_p,
  char **argv_p,
  int    max_args
);

extern int rtems_shell_cat_file(
  FILE *out,
  const char *name
);

extern void rtems_shell_write_file(
  const char *name,
  const char *content
);

extern int rtems_shell_script_file(
  int    argc,
  char **argv
);

/**
 * Initialise the shell creating tasks to login and run the shell
 * sessions.
 *
 * @param task_name Name of the shell task.
 * @param task_stacksize The size of the stack. If 0 the default size is used.
 * @param task_priority The priority the shell runs at.
 * @param forever Repeat logins.
 * @param wait Caller should block until shell exits.
 * @param login_check User login check function, NULL disables login checks.
 *
 */
extern rtems_status_code rtems_shell_init(
  const char *task_name,
  size_t task_stacksize,
  rtems_task_priority task_priority,
  const char *devname,
  bool forever,
  bool wait,
  rtems_shell_login_check_t login_check
);

/**
 * Run a shell script creating a shell tasks to execute the command under.
 *
 * @param task_name Name of the shell task.
 * @param task_stacksize The size of the stack. If 0 the default size is used.
 * @param task_priority The priority the shell runs at.
 * @param input The file of commands. Can be 'stdin' to use stdin.
 * @param output The output file to write commands to. Can be 'stdout',
 *              'stderr' or '/dev/null'.
 * @param output_append Append the output to the file or truncate the file.
 *                      Create if it does not exist.
 * @param wait Wait for the script to finish.
 */
extern rtems_status_code rtems_shell_script(
  const char          *task_name,
  size_t               task_stacksize,  /* 0 default*/
  rtems_task_priority  task_priority,
  const char          *input,
  const char          *output,
  bool                 output_append,
  bool                 wait,
  bool                 echo
);

/**
 *  Private environment associated with each shell instance.
 */
typedef struct {
  /** 'S','E','N','V': Shell Environment */
  rtems_name magic;
  const char *devname;
  const char *taskname;
  bool exit_shell; /* logout */
  bool forever; /* repeat login */
  int errorlevel;
  bool echo;
  char cwd[256];
  const char *input;
  const char *output;
  bool output_append;
  rtems_id wake_on_end;
  rtems_shell_login_check_t login_check;
} rtems_shell_env_t;

bool rtems_shell_main_loop(
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

typedef int (*rtems_shell_filesystems_mounter_t)(
  const char*                driver,
  const char*                path,
  rtems_shell_filesystems_t* fs,
  rtems_filesystem_options_t options
);

struct rtems_shell_filesystems_tt {
  rtems_chain_node                         link;
  const char                              *name;
  int                                      driver_needed;
  const rtems_filesystem_operations_table *fs_ops;
  rtems_shell_filesystems_mounter_t        mounter;
};

/**
 *  This method dynamically builds the command line prompt string
 *  and places it in @a prompt.
 *
 *  @param[in] shell_env is the shell execution environment
 *  @param[in] prompt is a pointer to a string buffer area
 *  @param[in] size is length of the prompt buffer area
 *
 *  @return This method fills in the memory pointed to by @a prompt.
 *
 *  @note An application specific implementation can be provided
 *        by the user.
 */
extern void rtems_shell_get_prompt(
  rtems_shell_env_t *shell_env,
  char              *prompt,
  size_t             size
);

/**
 * Helper for the mount command.
 *
 * @param[in] driver The path to the driver.
 * @param[in] path The path to mount on.
 * @param[in] fs The file system definition.
 * @param[in] options Special file system options.
 */
extern int rtems_shell_libc_mounter(
  const char*                driver,
  const char*                path,
  rtems_shell_filesystems_t* fs,
  rtems_filesystem_options_t options
);

/**
 * Add a new file system mount configuration to the mount command.
 *
 * @param[in] fs The file system mount data.
 */
extern void rtems_shell_mount_add_fsys(rtems_shell_filesystems_t* fs);

/**
 * Delete file system mount configuration from the mount command.
 *
 * @param[in] fs The file system mount data to remove.
 */
extern void rtems_shell_mount_del_fsys(rtems_shell_filesystems_t* fs);

typedef void (*rtems_shell_wait_for_input_notification)(
  int fd,
  int seconds_remaining,
  void *arg
);

/**
 * @brief Waits for input.
 *
 * @retval RTEMS_SUCCESSFUL Input detected.
 * @retval RTEMS_TIMEOUT Timeout expired.
 * @retval RTEMS_UNSATISFIED Cannot change or restore termios attributes.
 */
extern rtems_status_code rtems_shell_wait_for_input(
  int fd,
  int timeout_in_seconds,
  rtems_shell_wait_for_input_notification notification,
  void *notification_arg
);

extern int rtems_shell_main_monitor(int argc, char **argv);


#ifdef __cplusplus
}
#endif

#endif
