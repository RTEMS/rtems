/*
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
