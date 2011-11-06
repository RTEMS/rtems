/*
 *  COPYRIGHT (c) 2010 Eugen Leontie.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <bsp.h>

#include <libchip/serial.h>

extern void ofw_write(const char *,const int );
extern void ofw_read(void *,int );

int sun4v_console_device_first_open(int major, int minor, void *arg)
{
	return 0;
}

static ssize_t sun4v_console_poll_write(int minor, const char *buf, size_t n)
{
	ofw_write(buf, n);
	return 0;
}

void sun4v_console_deviceInitialize (int minor)
{
	
}

int sun4v_console_poll_read(int minor){
	int a;
	ofw_read(&a,1);
	if(a!=0){
		return a>>24;
	}
	return -1;
}


bool sun4v_console_deviceProbe (int minor){
	return true;
}

/*
 *  Polled mode functions
 */
console_fns pooled_functions={
 sun4v_console_deviceProbe, /*bool    (*deviceProbe)(int minor);*/
 sun4v_console_device_first_open, /*int     (*deviceFirstOpen)(int major, int minor, void *arg);*/
 NULL, /*int     (*deviceLastClose)(int major, int minor, void *arg);*/
 sun4v_console_poll_read, /*int     (*deviceRead)(int minor);*/
 sun4v_console_poll_write, /*ssize_t     (*deviceWrite)(int minor, const char *buf, size_t len);*/
 sun4v_console_deviceInitialize, /*void    (*deviceInitialize)(int minor);*/
 NULL, /*void    (*deviceWritePolled)(int minor, char cChar);*/
 NULL, /*int     (*deviceSetAttributes)(int minor, const struct termios *t);*/
 NULL /*bool    deviceOutputUsesInterrupts;*/
};

console_flow sun4v_console_console_flow={
  NULL,/*int (*deviceStopRemoteTx)(int minor);*/
  NULL/*int (*deviceStartRemoteTx)(int minor);*/
};


console_tbl     Console_Configuration_Ports[] = {
{
   "/dev/ttyS0",                                   /* sDeviceName */
   SERIAL_CUSTOM,                    	   /* deviceType */
   &pooled_functions,                      /* pDeviceFns */
   NULL,              						/* deviceProbe, assume it is there */
   &sun4v_console_console_flow,             /* pDeviceFlow */
   0,                                      /* ulMargin */
   0,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   0,  									   /* ulCtrlPort1 */
   0,                   				   /* ulCtrlPort2 */
   1,                          	           /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   0,                             		   /* ulClock */
   0      							   	   /* ulIntVector -- base for port */
},
};



/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS 1

unsigned long  Console_Configuration_Count = NUM_CONSOLE_PORTS;


/* putchar/getchar for printk */

static void bsp_out_char (char c)
{
  ofw_write(&c, 1);
}

BSP_output_char_function_type BSP_output_char = bsp_out_char;

static int bsp_in_char( void ){
	int tmp;
	ofw_read( &tmp, 1 ); /* blocks */
	if( tmp != 0 ) {
		return tmp>>24;
	}
	return -1;
}

BSP_polling_getchar_function_type BSP_poll_char = bsp_in_char;

