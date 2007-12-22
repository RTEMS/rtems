/*
 *  Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/shell.h>
#include <rtems/rtems_bsdnet.h>

/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_inet(int argc,char * argv[]) {
 rtems_bsdnet_show_inet_routes ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_mbuf(int argc,char * argv[]) {
 rtems_bsdnet_show_mbuf_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_if(int argc,char * argv[]) {
 rtems_bsdnet_show_if_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_ip(int argc,char * argv[]) {
 rtems_bsdnet_show_ip_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_icmp(int argc,char * argv[]) {
 rtems_bsdnet_show_icmp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_tcp(int argc,char * argv[]) {
 rtems_bsdnet_show_tcp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
static int main_udp(int argc,char * argv[]) {
 rtems_bsdnet_show_udp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/

void _rtems_telnetd_register_icmds(void) {
 rtems_shell_add_cmd("inet"    ,"net","inet routes"               ,main_inet);   
 rtems_shell_add_cmd("mbuf"    ,"net","mbuf stats"                ,main_mbuf);   
 rtems_shell_add_cmd("if"      ,"net","if   stats"                ,main_if  );   
 rtems_shell_add_cmd("ip"      ,"net","ip   stats"                ,main_ip  );   
 rtems_shell_add_cmd("icmp"    ,"net","icmp stats"                ,main_icmp);   
 rtems_shell_add_cmd("tcp"     ,"net","tcp  stats"                ,main_tcp );   
 rtems_shell_add_cmd("udp"     ,"net","udp  stats"                ,main_udp );   
}
