/*
 *  GDB Support Routines for the Mongoose-V
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <libcpu/mongoose-v.h>

#include <rtems/libio.h>



/*

We're going to call right down into the uart driver because we're
operating within an exception.  if things are broken because something
bad happened, this may be our last chance to debug before RTEMS goes
mad, so we won't rely on the I/O subsystem to be operating.  This is a
little messy, but at least we're not talking right to the hardware.

*/

extern int mg5uart_set_attributes(int minor,const struct termios *t);
extern int mg5uart_open(int major,int minor, void *arg);
extern int mg5uart_close(int major,int minor, void *arg);
extern void mg5uart_write_polled(int minor, char c );
extern int mg5uart_inbyte_nonblocking_polled(int minor);


extern void mips_gdb_stub_install(void);


static int debugUartEnabled = 0;



int mg5rdbgOpenGDBuart(int breakoninit)
{
   struct termios t;
   memset(&t,0,sizeof(struct termios));

   if( mg5uart_open(0,1,NULL) != RTEMS_SUCCESSFUL )
   {
      printf("gdbstub: Failed to open UART port 2\n");
      return -1;
   }

   t.c_cflag |= B19200;
   t.c_cflag |= CS8;
   if( mg5uart_set_attributes(1,&t) != 0 )
   {
      printf("gdbstub: Failed to configure UART 2 for 19200N82\n");
      return -1;
   }

   debugUartEnabled  = -1;

   /* set up vectoring for gdb */
   mips_gdb_stub_install();

   printf("gdbstub: Remote GDB stub listening on UART 2 at 19200N82\n");

   if( breakoninit ) 
   {
      /* 
         break to gdb.  We'll wait there for the operator to get their gdb
         going, then they can 'continue' or do whatever.
      */
      mips_break(0);
   }

   printf("gdbstub: User code running\n");

   return RTEMS_SUCCESSFUL;
}


void mg5rdbgCloseGDBuart(void)
{
   mg5uart_close(0,1,NULL);
   debugUartEnabled  = 0;
}




char getDebugChar (void)
{
   if( debugUartEnabled )
   {
      int rv;

      while( (rv = mg5uart_inbyte_nonblocking_polled(1)) < 0 );
      return (char)rv;
   }

   return 0;
}


void putDebugChar (char c)
{
   if( debugUartEnabled )
      return mg5uart_write_polled(1,c);
}

