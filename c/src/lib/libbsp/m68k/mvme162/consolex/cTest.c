/*
 *  Test program for consolex.
 *
 *  NOTE:  This program must be put together as an executable. :)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$ 
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <consolex.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#if ! defined(O_NDELAY)
# if defined(solaris2)
#  define O_NDELAY O_NONBLOCK
# elif defined(RTEMS_NEWLIB)
#  define O_NDELAY _FNBIO
# endif
#endif

rtems_driver_address_table Device_drivers[] = {
    CONSOLEX_DRIVER_TABLE_ENTRY
};

rtems_task	Init(rtems_task_argument arg)
{
    char	buf[128];
    int		fd;
    struct termios	t;

    printf("Console test.\n");
    
    if((fd = open("/dev/tty00",O_RDWR)) < 0){
	printf("Can't open device.\n");
	return;
    }
    tcgetattr(fd,&t);
    t.c_cflag = B9600|CS8;
    tcsetattr(fd,TCSANOW,&t);
    printf("iflag=%07o, oflag=%07o, cflag=%07o, lflag=%07o\n",
	   t.c_iflag,t.c_oflag,t.c_cflag,t.c_lflag);

    do{
	write(fd,"Your name? ",11);
	read(fd,buf,sizeof(buf));
	write(fd,"Hi ",3);
	write(fd,buf,strlen(buf));
    }while(*buf != '!');
    
    close(fd);
    
    printf("Done.\n");

    exit(0);
}

#define	CONFIGURE_INIT
#define	CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define	CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
#include <confdefs.h>
