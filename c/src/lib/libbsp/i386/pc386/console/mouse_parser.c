/*
 * Copyright (c) 1999 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 1991 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * UNIX Serial Port Mouse Driver
 *
 * This driver opens a serial port directly, and interprets serial data.
 * Microsoft, PC, Logitech and PS/2 mice are supported.
 * The PS/2 mouse is only supported if the OS runs the mouse
 * byte codes through the serial port.
 *
 * The following environment variables control the mouse type expected
 * and the serial port to open.
 *
 * Environment Var	Default		Allowed
 * MOUSE_TYPE		pc		ms, pc, logi, ps2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	/* strcmp */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <rtems.h>
#include <bsp.h>
#include "keyboard.h"
#include "mouse_parser.h"
#include "serial_mouse.h"

/* NOTE NOTE NOTE NOTE:
   Select here the mouse type !!!!!
*/
#ifndef  MOUSE_TYPE
#define	MOUSE_TYPE	"ms"  /* default mouse type "ms","pc","ps2" */
#endif

/* states for the mouse*/
#define	IDLE			0		/* start of byte sequence */
#define	XSET			1		/* setting x delta */
#define	YSET			2		/* setting y delta */
#define	XADD			3		/* adjusting x delta */
#define	YADD			4		/* adjusting y delta */

/* values in the bytes returned by the mouse for the buttons*/
#define PC_LEFT_BUTTON		4
#define PC_MIDDLE_BUTTON	2
#define PC_RIGHT_BUTTON		1

#define MS_LEFT_BUTTON		2
#define MS_RIGHT_BUTTON		1

#define PS2_CTRL_BYTE		0x08
#define PS2_LEFT_BUTTON		1
#define PS2_RIGHT_BUTTON	2

/* Bit fields in the bytes sent by the mouse.*/
#define TOP_FIVE_BITS		0xf8
#define BOTTOM_THREE_BITS	0x07
#define TOP_BIT			0x80
#define SIXTH_BIT		0x40
#define BOTTOM_TWO_BITS		0x03
#define THIRD_FOURTH_BITS	0x0c
#define BOTTOM_SIX_BITS  	0x3f

/* local data*/
static int		state;		/* IDLE, XSET, ... */
static BUTTON	buttons;	/* current mouse buttons pressed*/
static BUTTON	availbuttons;	/* which buttons are available */
static COORD	xd;		/* change in x */
static COORD	yd;		/* change in y */

static int		left;		   /* because the button values change */
static int		middle;		/* between mice, the buttons are */
static int		right;		/* redefined */

static int		(*parse)( int );	/* parse routine */

/* local routines*/
static int  	ParsePC(int);		/* routine to interpret PC mouse */
static int  	ParseMS(int);		/* routine to interpret MS mouse */
static int  	ParsePS2(int);		/* routine to interpret PS/2 mouse */

extern void uart_set_driver_handler( int port, void ( *handler )( void *,  char *, int ) );
extern void kbd_set_driver_handler( void ( *handler )( void *,  unsigned short, unsigned long ) );
extern void ps2_set_driver_handler( int port, void ( *handler )( void *,  char *, int ) );

/*
 * Open up the mouse device.
 * Returns the fd if successful, or negative if unsuccessful.
 */
int MOU_Init(void)
{
	char	*type;

	/* get mouse type and port*/
	type = MOUSE_TYPE;

	/* set button bits and parse procedure*/
	if(!strcmp(type, "pc") || !strcmp(type, "logi")) {
		/* pc or logitech mouse*/
		left = PC_LEFT_BUTTON;
		middle = PC_MIDDLE_BUTTON;
		right = PC_RIGHT_BUTTON;
		parse = ParsePC;
	} else if (strcmp(type, "ms") == 0) {
		/* microsoft mouse*/
		left = MS_LEFT_BUTTON;
		right = MS_RIGHT_BUTTON;
		middle = 0;
		parse = ParseMS;
	} else if (strcmp(type, "ps2") == 0) {
		/* PS/2 mouse*/
		left = PS2_LEFT_BUTTON;
		right = PS2_RIGHT_BUTTON;
		middle = 0;
		parse = ParsePS2;
	} else
		return -1;

   printk("Device: /dev/mouse -- mouse type is: %s\n", MOUSE_TYPE );

	/* initialize data*/
	availbuttons = left | middle | right;
	state = IDLE;
	buttons = 0;
	xd = 0;
	yd = 0;
	return 0;
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
int MOU_Data( int ch, COORD *dx, COORD *dy, COORD *dz, BUTTON *bptr)
{
	int b;

	/*
	 * Loop over all the bytes read in the buffer, parsing them.
	 * When a complete state has been read, return the results,
	 * leaving further bytes in the buffer for later calls.
	 */
	if( (*parse)( ch ) )
	{
		*dx = xd;
		*dy = yd;
		*dz = 0;
		b = 0;
		if(buttons & left)
			b |= LBUTTON;
		if(buttons & right)
			b |= RBUTTON;
		if(buttons & middle)
			b |= MBUTTON;
		*bptr = b;
		return 1;
	}
	return 0;
}

/*
 * Input routine for PC mouse.
 * Returns nonzero when a new mouse state has been completed.
 */
static int ParsePC(int byte)
{
	int	sign;			/* sign of movement */

	switch (state) {
		case IDLE:
			if ((byte & TOP_FIVE_BITS) == TOP_BIT) {
				buttons = ~byte & BOTTOM_THREE_BITS;
				state = XSET;
			}
			break;

		case XSET:
			sign = 1;
			if (byte > 127) {
				byte = 256 - byte;
				sign = -1;
			}
			xd = byte * sign;
			state = YSET;
			break;

		case YSET:
			sign = 1;
			if (byte > 127) {
				byte = 256 - byte;
				sign = -1;
			}
			yd = -byte * sign;
			state = XADD;
			break;

		case XADD:
			sign = 1;
			if (byte > 127) {
				byte = 256 - byte;
				sign = -1;
			}
			xd += byte * sign;
			state = YADD;
			break;

		case YADD:
			sign = 1;
			if (byte > 127) {
				byte = 256 - byte;
				sign = -1;
			}
			yd -= byte * sign;
			state = IDLE;
			return 1;
	}
	return 0;
}

/*
 * Input routine for Microsoft mouse.
 * Returns nonzero when a new mouse state has been completed.
 */
static int ParseMS(int byte)
{
	switch (state) {
		case IDLE:
			if (byte & SIXTH_BIT) {
				buttons = (byte >> 4) & BOTTOM_TWO_BITS;
				yd = ((byte & THIRD_FOURTH_BITS) << 4);
				xd = ((byte & BOTTOM_TWO_BITS) << 6);
				state = XADD;
			}
			break;

		case XADD:
			xd |= (byte & BOTTOM_SIX_BITS);
			state = YADD;
			break;

		case YADD:
			yd |= (byte & BOTTOM_SIX_BITS);
			state = IDLE;
			if (xd > 127)
				xd -= 256;
			if (yd > 127)
				yd -= 256;
			return 1;
	}
	return 0;
}

/*
 * Input routine for PS/2 mouse.
 * Returns nonzero when a new mouse state has been completed.
 */
static int ParsePS2(int byte)
{
	switch (state) {
		case IDLE:
			if (byte & PS2_CTRL_BYTE) {
				buttons = byte &
					(PS2_LEFT_BUTTON|PS2_RIGHT_BUTTON);
				state = XSET;
			}
			break;

		case XSET:
			if(byte > 127)
				byte -= 256;
			xd = byte;
			state = YSET;
			break;

		case YSET:
			if(byte > 127)
				byte -= 256;
			yd = -byte;
			state = IDLE;
			return 1;
	}
	return 0;
}

static rtems_id queue_id = 0;

/* generic keyboard parser */
static void mouse_parser( void *ptr, char *buffer, int size )
{
   COORD dx;
   COORD dy;
   COORD dz;
   BUTTON bptr;
   while( size-- )
   {
      if( MOU_Data( *buffer++, &dx, &dy, &dz, &bptr ) )
      {
         struct MW_UID_MESSAGE m;
         m.type = MV_UID_REL_POS;
         /* buttons definitons have been selected to match */
         m.m.pos.btns = bptr;
         m.m.pos.x  = dx;
         m.m.pos.y  = dy;
         m.m.pos.z  = dz;
/*       printk( "Mouse: msg: dx=%d, dy=%d, btn=%X\n", dx, dy, bptr ); */
         rtems_message_queue_send( queue_id, ( void * )&m, sizeof( struct MW_UID_MESSAGE ) );
      }
   }
}

/* enable the mouse to add messages to the queue */
void register_mou_msg_queue( char * q_name, int port )
{
   rtems_name queue_name;
   rtems_status_code status;
   queue_name = rtems_build_name( q_name[0],
                                  q_name[1],
                                  q_name[2],
                                  q_name[3] );
   status = rtems_message_queue_ident( queue_name, RTEMS_LOCAL, &queue_id );
   if( status != RTEMS_SUCCESSFUL )
   {
      printk( "UID_Queue: error open queue: %d\n", status );
      return;
   }
   MOU_Init();
   if( port == -1 )
   {
      /* we know the mouse type in this case, let's initialize everything */
		left = PS2_LEFT_BUTTON;
		right = PS2_RIGHT_BUTTON;
		middle = 0;
		parse = ParsePS2;
      ps2_set_driver_handler( port, mouse_parser );
   }
   else
   {
      uart_set_driver_handler( port, mouse_parser );
   }
}

/* stop the mouse from adding messages to the queue */
void unregister_mou_msg_queue( int port )
{
   if( port == -1 )
   {
      ps2_set_driver_handler( port, NULL );
   }
   else
   {
      uart_set_driver_handler( port, NULL );
   }
}

/* adds a kbd message to the queue */
static void kbd_parser( void *ptr, unsigned short keycode, unsigned long mods )
{
    struct MW_UID_MESSAGE m;
    struct kbd_struct * kbd = ( struct kbd_struct *)ptr;

    m.type = MV_UID_KBD;
    m.m.kbd.code       = keycode;
    m.m.kbd.modifiers  = kbd->ledflagstate;
    m.m.kbd.mode       = kbd->kbdmode;
    /*  printk( "kbd: msg: keycode=%X, mod=%X\n", keycode, mods );  */
    rtems_message_queue_send( queue_id, ( void * )&m,
                              sizeof( struct MW_UID_MESSAGE ) );
}

void register_kbd_msg_queue( char *q_name, int port )
{
   rtems_name queue_name;
   rtems_status_code status;

   queue_name = rtems_build_name( q_name[0],
                                  q_name[1],
                                  q_name[2],
                                  q_name[3] );
   status = rtems_message_queue_ident( queue_name, RTEMS_LOCAL, &queue_id );
   if( status != RTEMS_SUCCESSFUL )
   {
      printk( "UID_Queue: error open queue: %d\n", status );
      return;
   }
   kbd_set_driver_handler( kbd_parser );
}

void unregister_kbd_msg_queue( int port )
{
   kbd_set_driver_handler( NULL );
}
