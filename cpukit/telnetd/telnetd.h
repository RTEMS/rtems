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

int rtems_initialize_telnetd(void);
int main_telnetd(int argc,char * argv[]);
int register_telnetd(void);
 
#ifdef __cplusplus
}
#endif	

#endif

