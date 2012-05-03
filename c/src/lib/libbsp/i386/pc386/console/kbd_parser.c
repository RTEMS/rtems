/*
 *  The contents of this file were formerly in console.c which
 *  had the following copyright notice:
 *
 *  (C) Copyright 1997
 *  NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *  http://pandora.ist.utl.pt
 *  Instituto Superior Tecnico * Lisboa * PORTUGAL
 *
 *  The original code and subsequent modifications are:
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/keyboard.h>
#include <rtems/mw_uid.h>

/* adds a kbd message to the queue */
static void kbd_parser( void *ptr, unsigned short keycode, unsigned long mods )
{
  struct MW_UID_MESSAGE m;
  struct kbd_struct * kbd = (struct kbd_struct *)ptr;

  m.type = MV_UID_KBD;
  m.m.kbd.code       = keycode;
  m.m.kbd.modifiers  = kbd->ledflagstate;
  m.m.kbd.mode       = kbd->kbdmode;
  /*  printk( "kbd: msg: keycode=%X, mod=%X\n", keycode, mods );  */

  uid_send_message( &m );
}

void register_kbd_msg_queue( char *q_name, int port )
{
  kbd_set_driver_handler( kbd_parser );
}

void unregister_kbd_msg_queue( int port )
{
   kbd_set_driver_handler( NULL );
}
