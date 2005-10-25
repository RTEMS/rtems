/*
 * (A first version for telnetd)
 *
 *  Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  rtems_initialize_telnetd() starts the daemon.
 *  main_telnetd() is the main_proc for the command telnetd in the shell
 *  register_telnetd() add a new command in the shell to start
 *  interactively the telnetd daemon.
 * 
 *  $Id$
 */

#ifndef __TELNETD_H
#define __TELNETD_H

#ifdef __cplusplus
extern "C" {
#endif	

extern int rtems_telnetd_initialize(void);
extern int rtems_telnetd_main(int argc,char * argv[]);
extern int rtems_telnetd_register(void);

/* OBSOLETE */
#define rtems_initialize_telnetd	rtems_telnetd_initialize
#define main_telnetd  			rtems_telnetd_main
#define register_telnetd		rtems_telnetd_register

#ifdef __cplusplus
}
#endif	

#endif
