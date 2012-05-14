/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/mw_uid.h>
#include <stdio.h>

const char *uid_buttons(
  unsigned short  btns,
  char           *buffer,
  size_t          max
)
{
  snprintf(
    buffer,
    max,
    "LEFT=%s CENTER=%s RIGHT=%s",
    ((btns & MV_BUTTON_LEFT) ? "down" : "up"),
    ((btns & MV_BUTTON_CENTER) ? "down" : "up"),
    ((btns & MV_BUTTON_RIGHT) ? "down" : "up")
  );
  return buffer;
}

void uid_print_message(
  struct MW_UID_MESSAGE *uid
)
{
  uid_print_message_with_plugin( NULL, printk_plugin, uid );
}

void uid_print_message_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  handler,
  struct MW_UID_MESSAGE *uid
)
{
  char buttons[80];

  switch (uid->type) {
    case MV_UID_INVALID:
      (*handler)( context, "MV_UID_INVALID\n" );
      break;
    case MV_UID_REL_POS:
      (*handler)(
        context,
        "MV_UID_REL_POS - %s x=%d y=%d z=%d\n",
        uid_buttons( uid->m.pos.btns, buttons, sizeof(buttons)),
        uid->m.pos.x,    /* x location */
        uid->m.pos.y,    /* y location */
        uid->m.pos.z     /* z location, 0 for 2D */
      );
      break;
    case MV_UID_ABS_POS:
      (*handler)(
        context,
        "MV_UID_ABS_POS - %s x=%d y=%d z=%d\n",
        uid_buttons( uid->m.pos.btns, buttons, sizeof(buttons)),
        uid->m.pos.x,    /* x location */
        uid->m.pos.y,    /* y location */
        uid->m.pos.z     /* z location, 0 for 2D */
      );
      break;
    case MV_UID_KBD:
      (*handler)( context,
        "MV_UID_KBD - code=0x%04x modifiers=0x%02x mode=0x%02x\n",
        uid->m.kbd.code,        /* keycode or scancode        */
        uid->m.kbd.modifiers,   /* key modifiers              */
        uid->m.kbd.mode         /* current Kbd mode           */
      );
      break;
   case MV_UID_TIMER:
      (*handler)( context, "MV_UID_TIMER\n" );
      break;
    default:
      (*handler)( context, "Invalid device type\n" );
      break;
  }
 
}

