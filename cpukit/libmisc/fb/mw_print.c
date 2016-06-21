/**
 * @file
 *
 * @brief MicroWindows Print
 * @ingroup libmisc_fb_mw Input Devices for MicroWindows
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <rtems/mw_uid.h>
#include <rtems/printer.h>

static const char *uid_buttons(
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
  rtems_printer printer;
  rtems_print_printer_printk(&printer);
  uid_print_message_with_plugin( &printer, uid );
}

void uid_print_message_with_plugin(
  const rtems_printer   *printer,
  struct MW_UID_MESSAGE *uid
)
{
  char buttons[80];

  switch (uid->type) {
    case MV_UID_INVALID:
      rtems_printf( printer, "MV_UID_INVALID\n" );
      break;
    case MV_UID_REL_POS:
      rtems_printf(
        printer,
        "MV_UID_REL_POS - %s x=%d y=%d z=%d\n",
        uid_buttons( uid->m.pos.btns, buttons, sizeof(buttons)),
        uid->m.pos.x,    /* x location */
        uid->m.pos.y,    /* y location */
        uid->m.pos.z     /* z location, 0 for 2D */
      );
      break;
    case MV_UID_ABS_POS:
      rtems_printf(
        printer,
        "MV_UID_ABS_POS - %s x=%d y=%d z=%d\n",
        uid_buttons( uid->m.pos.btns, buttons, sizeof(buttons)),
        uid->m.pos.x,    /* x location */
        uid->m.pos.y,    /* y location */
        uid->m.pos.z     /* z location, 0 for 2D */
      );
      break;
    case MV_UID_KBD:
      rtems_printf( printer,
        "MV_UID_KBD - code=0x%04x modifiers=0x%02x mode=0x%02x\n",
        uid->m.kbd.code,        /* keycode or scancode        */
        uid->m.kbd.modifiers,   /* key modifiers              */
        uid->m.kbd.mode         /* current Kbd mode           */
      );
      break;
   case MV_UID_TIMER:
      rtems_printf( printer, "MV_UID_TIMER\n" );
      break;
    default:
      rtems_printf( printer, "Invalid device type\n" );
      break;
  }

}
