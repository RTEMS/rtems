/*
/////////////////////////////////////////////////////////////////////////////
// $Header$
//
// Copyright (c) 2000 - Rosimildo da Silva.  All Rights Reserved.
//  
// MODULE DESCRIPTION:
// 
// IO Functions for the PC platform equivalent to DOS/Linux. They make
// eaiser the porting of code from these platforms. 
//
//  by: Rosimildo da Silva:  rdasilva@connecttel.com
//
// MODIFICATION/HISTORY:
// $Log$
// Revision 1.1  2000/08/30 08:18:56  joel
// 2000-08-26  Rosimildo da Silva  <rdasilva@connecttel.com>
//
// 	* shared/comm: Added "/dev/ttyS1" & "/dev/ttyS2" support for
// 	the i386 BSPs.
// 	* shared/comm/gdb_glue.c: New file.
// 	* shared/comm/i386_io.c: New file.
// 	* shared/comm/tty_drv.c: New file.
// 	* shared/comm/tty_drv.h: New file.
// 	* shared/comm/Makefile.am: Account for new files.
// 	* shared/comm/uart.c: Adds support for sending characters to
// 	another "line discipline."
//
//
/////////////////////////////////////////////////////////////////////////////
*/

#ifndef i386_io_h__
#define i386_io_h__

#define rtems_inb(port)							\
({									\
        register int _inb_result;					\
							\
        asm volatile ("xorl %%eax,%%eax; inb %%dx,%%al" :		\
            "=a" (_inb_result) : "d" (port));				\
        _inb_result;							\
})

#define rtems_inw(port)							\
({									\
        register int _inbw_result;					\
									\
        asm volatile ("xorl %%eax,%%eax; inw %%dx,%%ax" :		\
            "=a" (_inbw_result) : "d" (port));				\
        _inbw_result;							\
})

#define rtems_outb(port, data)						\
        asm volatile ("outb %%al,%%dx" : : "a" (data), "d" (port))

#define rtems_outw(port, data)						\
        asm volatile ("outw %%ax,%%dx" : : "a" (data), "d" (port))

#define outp(port, val)	rtems_outb(port,val)
#define inp(port)	      rtems_inb(port)

#define outb(val, port)	rtems_outb(port,val)
#define inb(port)	      rtems_inb(port)

#define outb_p(val, port)	rtems_outb(port,val)
#define inb_p(port)	      rtems_inb(port)

#define outportb(port,val)	rtems_outb(port,val)
#define inportb(port)	rtems_inb(port)

#define outw(val, port)	rtems_outw(port,val)
#define inw(port)	      rtems_inw(port)

#define cli()   __asm__ __volatile__("cli")
#define sti()   __asm__ __volatile__("sti");

#endif /* i386_io_h__ */
