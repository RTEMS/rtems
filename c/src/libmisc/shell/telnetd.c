/*
 *  $Id$
 */

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/shell.h>
#include <rtems/telnetd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
/***********************************************************/
rtems_id            telnetd_task_id      =0;
rtems_unsigned32    telnetd_stack_size   =16384;
rtems_task_priority telnetd_task_priority=100;
/***********************************************************/
rtems_task rtems_task_telnetd(rtems_task_argument task_argument) {
	int des_socket,
	    acp_socket;
	struct sockaddr_in srv;
	char * devname;
	int i=1;
	int size_adr;
	if ((des_socket=socket(PF_INET,SOCK_STREAM,0))<0) {
		perror("socket");
		rtems_task_delete(RTEMS_SELF);
	};
	setsockopt(des_socket,SOL_SOCKET,0,&i,sizeof(i));
	memset(&srv,0,sizeof(srv));
	srv.sin_family=AF_INET;
	srv.sin_port=htons(23);
	size_adr=sizeof(srv);
	if ((bind(des_socket,(struct sockaddr *)&srv,size_adr))<0) {
		perror("bind");
	        close(des_socket);
		rtems_task_delete(RTEMS_SELF);
	};
	if ((listen(des_socket,5))<0) {
		perror("listen");
	        close(des_socket);
		rtems_task_delete(RTEMS_SELF);
	};
	do {
	  acp_socket=accept(des_socket,(struct sockaddr*)&srv,&size_adr);
	  if (acp_socket<0) {
		perror("accept");
		break;
	  };
	  if (devname = get_pty(acp_socket) ) {
	   shell_init(&devname[5],
		      telnetd_stack_size,
		      telnetd_task_priority,
		      devname,B9600|CS8,FALSE);
	  } else {
           close(acp_socket);
	  };
	} while(1);
	close(des_socket);
	rtems_task_delete(RTEMS_SELF);
}
/***********************************************************/
int rtems_initialize_telnetd(void) {
	rtems_status_code sc;
	if (telnetd_task_id         ) return RTEMS_RESOURCE_IN_USE;
	if (telnetd_stack_size<=0   ) telnetd_stack_size   =16384;
	if (telnetd_task_priority<=2) telnetd_task_priority=100;
	sc=rtems_task_create(new_rtems_name("TLND"),
			     100,RTEMS_MINIMUM_STACK_SIZE,	
			     RTEMS_DEFAULT_MODES,
			     RTEMS_DEFAULT_ATTRIBUTES,
			     &telnetd_task_id);
        if (sc!=RTEMS_SUCCESSFUL) {
		rtems_error(sc,"creating task telnetd");
		return (int)sc;
	};
	sc=rtems_task_start(telnetd_task_id,
			    rtems_task_telnetd,
			    (rtems_task_argument)NULL);
        if (sc!=RTEMS_SUCCESSFUL) {
		rtems_error(sc,"starting task telnetd");
	};
	return (int)sc;
}
/***********************************************************/
int main_telnetd(int argc,char * argv[]) {
	rtems_status_code sc;
	if (telnetd_task_id) {
		printf("ERROR:telnetd already started\n");
		return 1;
	};
	if (argc>1) telnetd_stack_size   =str2int(argv[1]);
	if (argc>2) telnetd_task_priority=str2int(argv[2]);
	sc=rtems_initialize_telnetd();
        if (sc!=RTEMS_SUCCESSFUL) return sc;
	printf("rtems_telnetd() started with stacksize=%u,priority=%d\n",
                        telnetd_stack_size,telnetd_task_priority);
	return 0;
}
/***********************************************************/
int register_telnetd(void) {
	shell_add_cmd("telnetd","telnet","telnetd [stacksize [tsk_priority]]",main_telnetd);
	return 0;
}
/***********************************************************/
