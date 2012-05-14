/*
 *  GDB Support Routines for the Mongoose-V
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <libcpu/mongoose-v.h>
#include "gdb_if.h"

#include <rtems/libio.h>

#include <stdio.h>
#include <string.h>

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
   printf("gdbstub: UART 2 configured for 19200N82\n");

   debugUartEnabled  = -1;

   /* set up vectoring for gdb */
   mips_gdb_stub_install(-1);

   /*
      this is a rough approximation of our memory map.  Yours is
      probably different.  It only needs to be sufficient for the stub
      to know what it can and can't do and where.
   */
   gdbstub_add_memsegment(0         , 0x8001ffff, MEMOPT_READABLE );
   gdbstub_add_memsegment(0x80020000, 0x80afffff, MEMOPT_READABLE | MEMOPT_WRITEABLE );
   gdbstub_add_memsegment(0x80b00000, 0x814fffff, MEMOPT_READABLE );
   gdbstub_add_memsegment(0x81500000, 0x81ffffff, MEMOPT_READABLE | MEMOPT_WRITEABLE );

   if( breakoninit )
   {
      printf("gdbstub: GDB stub entered, connect host debugger now\n");
      /*
         break to gdb.  We'll wait there for the operator to get their gdb
         going, then they can 'continue' or do whatever.
      */
      mips_break(0);
      printf("gdbstub: User code running\n");
   }
   else
      printf("gdbstub: GDB stub ready for exceptions\n");

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

/*
   {
      * initialize hardware pc and data breakpoints to quiet state*
      uint32_t   dcic, reg, mask;

      reg = 0xffffffff;
      mask = 0xffffffff;

      mips_set_bpcrm( reg, mask );
      mips_set_bdarm( reg, mask );

      mips_get_dcic( dcic );
      * configure dcic for trapping, user & kernel mode, PC traps and enable it *
      dcic = DCIC_TR | DCIC_UD | DCIC_KD | DCIC_PCE | DCIC_DE;
      * dcic = DCIC_UD | DCIC_KD | DCIC_PCE | DCIC_DE; *
      mips_set_dcic( dcic );

      mips_get_bpcrm( reg, mask );
      mips_get_dcic( dcic );
      * printf("bpc is %08X, bpc_mask is %08X, dcic is now %08X\n", reg, mask, dcic ); *
   }
*/
