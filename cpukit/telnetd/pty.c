/*
 * /dev/ptyXX  (A first version for pseudo-terminals)
 *
 *  Author: Fernando RUIZ CASAS (fernando.ruiz@ctv.es)
 *  May 2001
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*-----------------------------------------*/
#include <termios.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <rtems/pty.h>
/*-----------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*-----------------------------------------*/
#define IAC_ESC    255
#define IAC_DONT   254
#define IAC_DO     253
#define IAC_WONT   252
#define IAC_WILL   251
#define IAC_SB     250
#define IAC_GA     249
#define IAC_EL     248
#define IAC_EC     247
#define IAC_AYT    246
#define IAC_AO     245
#define IAC_IP     244
#define IAC_BRK    243
#define IAC_DMARK  242
#define IAC_NOP    241
#define IAC_SE     240
#define IAC_EOR    239

struct pty_tt;
typedef struct pty_tt pty_t;

struct pty_tt {
 char                     *devname;
 struct rtems_termios_tty *ttyp;
 tcflag_t                  c_cflag;
 int                       opened;
 int                       socket;

 int                       last_cr;
 int                       iac_mode;   
};


int ptys_initted=FALSE;
pty_t ptys[MAX_PTYS];

/* This procedure returns the devname for a pty slot free.
 * If not slot availiable (field socket>=0) 
 *  then the socket argument is closed
 */

char *  get_pty(int socket) {
	int ndx;
	if (!ptys_initted) return NULL;
	for (ndx=0;ndx<MAX_PTYS;ndx++) {
		if (ptys[ndx].socket<0) {
			ptys[ndx].socket=socket;
			return ptys[ndx].devname;
		};
	};
	close(socket);
	return NULL;
}


/*-----------------------------------------------------------*/
/*
 * The NVT terminal is negociated in PollRead and PollWrite
 * with every BYTE sendded or received. 
 * A litle status machine in the pty_read_byte(int minor) 
 * 
 */
const char IAC_AYT_RSP[]="\r\nAYT? Yes, RTEMS-SHELL is here\r\n";
const char IAC_BRK_RSP[]="<*Break*>";
const char IAC_IP_RSP []="<*Interrupt*>";


static
int send_iac(int minor,unsigned char mode,unsigned char option) {
	unsigned char buf[3];
	buf[0]=IAC_ESC;
	buf[1]=mode;
	buf[2]=option;
	return write(ptys[minor].socket,buf,sizeof(buf));
}

int read_pty(int minor) { /* Characters writed in the client side*/
	 unsigned char value;
	 int count;
	 int result;
	 count=read(ptys[minor].socket,&value,sizeof(value));
	 if (count<1) {
          fclose(stdin);		 
          fclose(stdout);		 
          fclose(stderr);		 
	  /* If you don't read from the socket the system ends the task */
	  rtems_task_delete(RTEMS_SELF);
	 };
	 switch(ptys[minor].iac_mode) {
          case IAC_ESC:
	       ptys[minor].iac_mode=0;
               switch(value) {
                case IAC_ESC :
		     return IAC_ESC;
                case IAC_DONT:
                case IAC_DO  :
                case IAC_WONT:
                case IAC_WILL:
		     ptys[minor].iac_mode=value;
		     return -1;
                case IAC_SB  :
		     return -100;
                case IAC_GA  :
		     return -1;
                case IAC_EL  :
		     return 0x03; /* Ctrl-C*/
                case IAC_EC  :
		     return '\b';
		case IAC_AYT :
	             write(ptys[minor].socket,IAC_AYT_RSP,strlen(IAC_AYT_RSP));
		     return -1;
                case IAC_AO  :
		     return -1;
                case IAC_IP  :
	             write(ptys[minor].socket,IAC_IP_RSP,strlen(IAC_IP_RSP));
		     return -1;
		case IAC_BRK :
	             write(ptys[minor].socket,IAC_BRK_RSP,strlen(IAC_BRK_RSP));
		     return -1;
                case IAC_DMARK:
		     return -2;
                case IAC_NOP :
		     return -1;
                case IAC_SE  :
		     return -101;
                case IAC_EOR :
		     return -102;
		default      :
		     return -1;
	       };
	       break;
	  case IAC_WILL:
	       ptys[minor].iac_mode=0;
	       if (value==34){send_iac(minor,IAC_DONT,   34);       /*LINEMODE*/ 
	                      send_iac(minor,IAC_DO  ,    1);} else /*ECHO    */
	                     {send_iac(minor,IAC_DONT,value);};
	       return -1;
	  case IAC_DONT:
	       ptys[minor].iac_mode=0;
	       return -1;
	  case IAC_DO  :
	       ptys[minor].iac_mode=0;
	       if (value==3) {send_iac(minor,IAC_WILL,    3);} else /* GO AHEAD*/
	       if (value==1) {                               } else /* ECHO */
	                     {send_iac(minor,IAC_WONT,value);};
	       return -1;
	  case IAC_WONT:
	       ptys[minor].iac_mode=0;
	       if (value==1) {send_iac(minor,IAC_WILL,    1);} else /* ECHO */
	                     {send_iac(minor,IAC_WONT,value);};
	       return -1;
	  default:
	       ptys[minor].iac_mode=0;
	       if (value==IAC_ESC) {
                     ptys[minor].iac_mode=value;
		     return -1;
	       } else {
		     result=value;  
		     if ((value=='\n') && (ptys[minor].last_cr)) result=-1;
		     ptys[minor].last_cr=(value=='\r');
		     return result;
	       };
	 };
	
}

/*-----------------------------------------------------------*/
static int ptySetAttributes(int minor,const struct termios *t);
static int ptyPollInitialize(int major,int minor,void * arg) ;
static int ptyShutdown(int major,int minor,void * arg) ;
static int ptyPollWrite(int minor, const char * buf,int len) ;
static int ptyPollRead(int minor) ;
const rtems_termios_callbacks * pty_get_termios_handlers(int polled) ;
/*-----------------------------------------------------------*/
/* Set the 'Hardware'                                        */ 
/*-----------------------------------------------------------*/
static int
ptySetAttributes(int minor,const struct termios *t) {
	if (minor<MAX_PTYS) {	
	 ptys[minor].c_cflag=t->c_cflag;	
	} else {
	 return -1;
	};
	return 0;
}
/*-----------------------------------------------------------*/
static int 
ptyPollInitialize(int major,int minor,void * arg) {
	rtems_libio_open_close_args_t * args = arg;
	struct termios t;
        if (minor<MAX_PTYS) {	
         if (ptys[minor].socket<0) return -1;		
	 ptys[minor].opened=TRUE;
	 ptys[minor].ttyp=args->iop->data1;
	 t.c_cflag=B9600|CS8;/* termios default */
	 return ptySetAttributes(minor,&t);
	} else {
	 return -1;
	};
}
/*-----------------------------------------------------------*/
static int 
ptyShutdown(int major,int minor,void * arg) {
        if (minor<MAX_PTYS) {	
	 ptys[minor].opened=FALSE;
         if (ptys[minor].socket>=0) close(ptys[minor].socket);
	 ptys[minor].socket=-1;
	 chown(ptys[minor].devname,2,0);
	} else {
	 return -1;
	};
	return 0;
}
/*-----------------------------------------------------------*/
/* Write Characters into pty device                          */ 
/*-----------------------------------------------------------*/
static int
ptyPollWrite(int minor, const char * buf,int len) {
	int count;
        if (minor<MAX_PTYS) {	
         if (ptys[minor].socket<0) return -1;		
	 count=write(ptys[minor].socket,buf,len);
	} else {
	 count=-1;
	};
	return count;
}
/*-----------------------------------------------------------*/
static int
ptyPollRead(int minor) {
	int result;
        if (minor<MAX_PTYS) {	
         if (ptys[minor].socket<0) return -1;		
	 result=read_pty(minor);
	 return result;
	};
	return -1;
}
/*-----------------------------------------------------------*/
static const rtems_termios_callbacks pty_poll_callbacks = {
	ptyPollInitialize,	/* FirstOpen*/
	ptyShutdown,		/* LastClose*/
	ptyPollRead,		/* PollRead  */
	ptyPollWrite,		/* Write */
	ptySetAttributes,	/* setAttributes */
	NULL,			/* stopRemoteTX */
	NULL,			/* StartRemoteTX */
	0 			/* outputUsesInterrupts */
};
/*-----------------------------------------------------------*/
const rtems_termios_callbacks * pty_get_termios_handlers(int polled) {
	return &pty_poll_callbacks;
}
/*-----------------------------------------------------------*/
void init_ptys(void) {
	int ndx;
	for (ndx=0;ndx<MAX_PTYS;ndx++) {
		ptys[ndx].devname=malloc(strlen("/dev/ptyXX")+1);
		sprintf(ptys[ndx].devname,"/dev/pty%X",ndx);
		ptys[ndx].ttyp=NULL;
		ptys[ndx].c_cflag=CS8|B9600;
		ptys[ndx].socket=-1;
		ptys[ndx].opened=FALSE;

	};
	ptys_initted=TRUE;
}


/*-----------------------------------------------------------*/
/*  pty_initialize
 *
 *  This routine initializes the pty IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */
/*-----------------------------------------------------------*/
rtems_device_driver pty_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  int ndx;	
  rtems_status_code status ;

  /* 
   * Set up ptys
   */

  init_ptys();

  /*
   * Register the devices
   */
  for (ndx=0;ndx<MAX_PTYS;ndx++) {
   status = rtems_io_register_name(ptys[ndx].devname, major, ndx);
   if (status != RTEMS_SUCCESSFUL)
     rtems_fatal_error_occurred(status);
   chmod(ptys[ndx].devname,0660);
   chown(ptys[ndx].devname,2,0); /* tty,root*/
  };
  printk("Device: /dev/pty%X../dev/pty%X (%d)pseudo-terminals registered.\n",0,MAX_PTYS-1,MAX_PTYS);

  return RTEMS_SUCCESSFUL;
}


/*
 *  Open entry point
 */

rtems_device_driver pty_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc ;
  sc = rtems_termios_open(major,minor,arg,pty_get_termios_handlers(FALSE));
  return sc;
}
 
/*
 *  Close entry point
 */

rtems_device_driver pty_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close(arg);
}

/*
 * read bytes from the pty
 */

rtems_device_driver pty_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read(arg);
}

/*
 * write bytes to the pty
 */

rtems_device_driver pty_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write(arg);
}

/*
 *  IO Control entry point
 */

rtems_device_driver pty_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl(arg);
}
