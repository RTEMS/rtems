/*
 *  $Id$
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif	

#include <rtems.h>	
#include <stdio.h>	
#include <termios.h>	

typedef int (*shell_command_t)(int argc,char * argv[]);

struct shell_cmd_tt ;
typedef struct shell_cmd_tt shell_cmd_t;

struct shell_cmd_tt {
 char * name;
 char * usage;
 char * topic;
 shell_command_t command;
 shell_cmd_t * alias;
 shell_cmd_t * next;
};

rtems_unsigned32 new_rtems_name(char * rtems_name);
shell_cmd_t * shell_lookup_cmd(char * cmd);
shell_cmd_t * shell_add_cmd(char * cmd,
                            char * topic,
                            char * usage,
                            shell_command_t command);
shell_cmd_t * shell_alias_cmd(char * cmd, char * alias);

int shell_make_args(char * cmd,
                    int  * pargc,
                    char * argv[]);

typedef struct  {
  rtems_name magic; /* 'S','E','N','V': Shell Environment */	
  char * devname;
  char * taskname;
  tcflag_t tcflag;
  /* user extensions */
  int  exit_shell; /* logout */
  int  forever   ; /* repeat login */
  int  errorlevel;
  int  mdump_adr;
} shell_env_t;

int shell_scanline(char * line,int size,FILE * in,FILE * out) ;
void cat_file(FILE * out,char *name);
	
rtems_status_code shell_init(char * task_name      ,
                              rtems_unsigned32    task_stacksize,/*0 default*/
		              rtems_task_priority task_priority ,
                              char * devname      ,
			      tcflag_t tcflag     ,
			      int forever         );	

extern shell_env_t global_shell_env,
                *  current_shell_env;
/*--------*/
/* pty.c */ 
/*--------*/

char * get_pty(int socket);


rtems_device_driver pty_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg);
rtems_device_driver pty_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);
rtems_device_driver pty_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg);


#define PTY_DRIVER_TABLE_ENTRY \
       { pty_initialize , pty_open , pty_close , \
	 pty_read , pty_write , pty_control }
/*--------*/
/* cmds.c */ 
/*--------*/
int str2int(char * s);
void register_cmds(void);
 
#ifdef __cplusplus
}
#endif	

#endif
