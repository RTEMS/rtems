#include <rtems/shell.h>
#include <rtems/rtems_bsdnet.h>
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_inet(int argc,char * argv[]) {
 rtems_bsdnet_show_inet_routes ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_mbuf(int argc,char * argv[]) {
 rtems_bsdnet_show_mbuf_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_if(int argc,char * argv[]) {
 rtems_bsdnet_show_if_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_ip(int argc,char * argv[]) {
 rtems_bsdnet_show_ip_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_icmp(int argc,char * argv[]) {
 rtems_bsdnet_show_icmp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_tcp(int argc,char * argv[]) {
 rtems_bsdnet_show_tcp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
int main_udp(int argc,char * argv[]) {
 rtems_bsdnet_show_udp_stats ();
 return 0;
}
/*+++++++++++++++++++++++++++++++++++++++++++++*/
void register_icmds(void) {
 shell_add_cmd("inet"    ,"net","inet routes"               ,main_inet);   
 shell_add_cmd("mbuf"    ,"net","mbuf stats"                ,main_mbuf);   
 shell_add_cmd("if"      ,"net","if   stats"                ,main_if  );   
 shell_add_cmd("ip"      ,"net","ip   stats"                ,main_ip  );   
 shell_add_cmd("icmp"    ,"net","icmp stats"                ,main_icmp);   
 shell_add_cmd("tcp"     ,"net","tcp  stats"                ,main_tcp );   
 shell_add_cmd("udp"     ,"net","udp  stats"                ,main_udp );   
}

