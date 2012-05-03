/*
 * RTEMS for Nintendo DS input parser.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>

#include <nds.h>

#include <rtems/mw_uid.h>

/*
 * from console/console.c
 */

extern void console_push (char c);

/*
 * from reco.c
 */

extern char PA_CheckLetter (int down, int x, int y);

/*
 * message queue for touchscreen and graffiti events.
 */

static rtems_id mou_queue_id = 0;
static rtems_id kbd_queue_id = 0;

/*
 * old position.
 */

static int old_x = 0;
static int old_y = 0;
static int old_btns = 0;

/*
 * hand mode.
 */

static int hand = 0;

/*
 * update touchscreen position.
 */

void
update_touchscreen (void)
{
  static int graffiti = 0;
  struct MW_UID_MESSAGE m;
  int x, y, k, kh, btns = 0;
  touchPosition pos;
  char c;

  /* update keypad & touchscreen */
  scanKeys ();
  pos = touchReadXY ();
  x = pos.px;
  y = pos.py;
  k = keysDown ();
  kh = keysHeld ();

  /* check for character recognition */
  if ((kh & KEY_L) || (kh & KEY_R)) {
    graffiti = 1;
    c = PA_CheckLetter ((kh & KEY_TOUCH ? 1 : 0), x, y);
    if (c) {
      /* signal the console driver */
      console_push (c);
      if (kbd_queue_id != 0) {
        /* send the read character */
        m.type = MV_UID_KBD;
        m.m.kbd.code = c;
        m.m.kbd.modifiers = 0;
        m.m.kbd.mode = MV_KEY_MODE_ASCII;
        rtems_message_queue_send (kbd_queue_id, (void *) &m,
                                  sizeof (struct MW_UID_MESSAGE));
      }
    }
  } else {
    if (graffiti == 1) {
      x = old_x;
      y = old_y;
    }
    graffiti = 0;
  }

  if (mou_queue_id == 0)
    return;

  if (hand == 1) {
    if (k & KEY_LEFT) {
      btns = MV_BUTTON_LEFT;
    }
    if (k & KEY_RIGHT) {
      btns = MV_BUTTON_RIGHT;
    }
  } else {
    if (k & KEY_A) {
      btns = MV_BUTTON_LEFT;
    }
    if (k & KEY_B) {
      btns = MV_BUTTON_RIGHT;
    }
  }

  if (!((kh & KEY_L) || (kh & KEY_R)) && (kh & KEY_TOUCH)
      && (x != old_x || y != old_y || btns)) {
    /* send the read position */
    m.type = MV_UID_ABS_POS;
    old_btns = m.m.pos.btns = btns;
    old_x = m.m.pos.x = x;
    old_y = m.m.pos.y = y;
    m.m.pos.z = 0;
    rtems_message_queue_send (mou_queue_id, (void *) &m,
                              sizeof (struct MW_UID_MESSAGE));
  }
}

/*
 * register a message queue for touchscreen events.
 */

void
register_mou_msg_queue (char *q_name)
{
  rtems_name queue_name;
  rtems_status_code status;

  queue_name = rtems_build_name (q_name[0], q_name[1], q_name[2], q_name[3]);
  status = rtems_message_queue_ident (queue_name, RTEMS_LOCAL, &mou_queue_id);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] cannot create queue %d\n", status);
    return;
  }

  update_touchscreen ();
}

/*
 * unregister the message queue.
 */

void
unregister_mou_msg_queue (void)
{
  /* nothing here */
}

/*
 * register a message queue for graffiti events.
 */

void
register_kbd_msg_queue (char *q_name)
{
  rtems_name queue_name;
  rtems_status_code status;

  queue_name = rtems_build_name (q_name[0], q_name[1], q_name[2], q_name[3]);
  status = rtems_message_queue_ident (queue_name, RTEMS_LOCAL, &kbd_queue_id);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] cannot create queue %d\n", status);
    return;
  }
}

/*
 * unregister the message queue.
 */

void
unregister_kbd_msg_queue (void)
{
  /* nothing here */
}

/*
 * set hand mode.
 */

void
touchscreen_sethand (int h)
{
  hand = h;
}
