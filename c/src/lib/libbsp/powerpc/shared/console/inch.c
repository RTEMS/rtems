/*
 *  inch.c  -- keyboard minimal driver
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 * This code is based on the pc386 BSP inch.c so the following
 * copyright also applies :
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#ifdef BSP_KBD_IOBASE
#include <bsp/irq.h>

#include "console.inl"

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define KBD_CTL      0x1  /* -------------------------------- */
#define KBD_DATA     0x0  /* Port offsets for PC keyboard controller */
#define KBD_STATUS   0x4  /* -------------------------------- */

#define KBD_BUF_SIZE 256

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
static char key_map[] =
{
  0,033,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
  'q','w','e','r','t','y','u','i','o','p','[',']',015,0x80,
  'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
  0134,'z','x','c','v','b','n','m',',','.','/',0x80,
  '*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,'0',0177
}; /* Keyboard scancode -> character map with no modifiers.       */

static char shift_map[] =
{
  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
  'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
  'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
  '|','Z','X','C','V','B','N','M','<','>','?',0x80,
  '*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
  '1','2','3','0',177
}; /* Keyboard scancode -> character map with SHIFT key modifier. */

static char             kbd_buffer[KBD_BUF_SIZE];
static uint16_t         kbd_first = 0;
static uint16_t         kbd_last  = 0;
static uint16_t         kbd_end   = KBD_BUF_SIZE - 1;

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_scankey
|      Description: This function can be called during a poll for input, or by
|                   an ISR. Basically any time you want to process a keypress.
| Global Variables: key_map, shift_map.
|        Arguments: outChar - character read in case of a valid reading,
|                   otherwise unchanged.
|          Returns: true in case a valid character has been read,
|                   false otherwise.
+--------------------------------------------------------------------------*/
bool
_IBMPC_scankey(char *outChar)
{
  unsigned char inChar;
  static int alt_pressed   = 0;
  static int ctrl_pressed  = 0;
  static int shift_pressed = 0;
  static int caps_pressed  = 0;
  static int extended      = 0;

  *outChar = 0; /* default value if we return false */

  /* Read keyboard controller, toggle enable */
  inChar=kbd_inb(KBD_CTL);
  kbd_outb(KBD_CTL, inChar & ~0x80);
  kbd_outb(KBD_CTL, inChar | 0x80);
  kbd_outb(KBD_CTL, inChar & ~0x80);

  /* See if it has data */
  inChar=kbd_inb(KBD_STATUS);
  if ((inChar & 0x01) == 0)
    return false;

  /* Read the data.  Handle nonsense with shift, control, etc. */
  inChar=kbd_inb(KBD_DATA);

  if (extended)
    extended--;

  switch (inChar)
  {
    case 0xe0:
      extended = 2;
      return false;
      break;

    case 0x38:
      alt_pressed = 1;
      return false;
      break;
    case 0xb8:
      alt_pressed = 0;
      return false;
      break;

    case 0x1d:
      ctrl_pressed = 1;
      return false;
      break;
    case 0x9d:
      ctrl_pressed = 0;
      return false;
      break;

    case 0x2a:
      if (extended)
        return false;
    case 0x36:
      shift_pressed = 1;
      return false;
      break;
    case 0xaa:
      if (extended)
        return false;
    case 0xb6:
      shift_pressed = 0;
      return false;
      break;

    case 0x3a:
      caps_pressed = 1;
      return false;
      break;
    case 0xba:
      caps_pressed = 0;
      return false;
      break;

    case 0x53:
      if (ctrl_pressed && alt_pressed)
        bsp_reset(); /* ctrl+alt+del -> reboot */
      break;

    /*
     * Ignore unrecognized keys--usually arrow and such
     */
    default:
      if ((inChar & 0x80) || (inChar > 0x39))
      /* High-bit on means key is being released, not pressed */
        return false;
      break;
  } /* switch */

  /* Strip high bit, look up in our map */
  inChar &= 0x7f;
  if (ctrl_pressed)
  {
    *outChar = key_map[inChar];
    *outChar &= 037;
  }
  else
  {
    *outChar = shift_pressed ? shift_map[inChar] : key_map[inChar];
    if (caps_pressed)
    {
      if (*outChar >= 'A' && *outChar <= 'Z')
        *outChar += 'a' - 'A';
      else if (*outChar >= 'a' && *outChar <= 'z')
        *outChar -= 'a' - 'A';
    }
  }

  return true;
} /* _IBMPC_scankey */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_keyboard_isr
|      Description: Interrupt Service Routine for keyboard (0x01) IRQ.
| Global Variables: kbd_buffer, kbd_first, kbd_last.
|        Arguments: vector - standard RTEMS argument - see documentation.
|          Returns: standard return value - see documentation.
+--------------------------------------------------------------------------*/
void _IBMPC_keyboard_isr(void)
{
  if (_IBMPC_scankey(&kbd_buffer[kbd_last]))
  {
    /* Got one; save it if there is enough room in buffer. */
    unsigned int next = (kbd_last == kbd_end) ? 0 : kbd_last + 1;

    if (next != kbd_first)
      {
	kbd_last = next;
      }
  }
} /* _IBMPC_keyboard_isr */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_chrdy
|      Description: Check keyboard ISR buffer and return character if not empty.
| Global Variables: kbd_buffer, kbd_first, kbd_last.
|        Arguments: c - character read if keyboard buffer not empty, otherwise
|                   unchanged.
|          Returns: true if keyboard buffer not empty, false otherwise.
+--------------------------------------------------------------------------*/
bool
_IBMPC_chrdy(char *c)
{
  /* Check buffer our ISR builds */
  if (kbd_first != kbd_last)
  {
    *c = kbd_buffer[kbd_first];

    kbd_first = (kbd_first + 1) % KBD_BUF_SIZE;
    return true;
  }
  else
    return false;
} /* _IBMPC_chrdy */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_inch
|      Description: Poll keyboard until a character is ready and return it.
| Global Variables: None.
|        Arguments: None.
|          Returns: character read from keyboard.
+--------------------------------------------------------------------------*/
char
_IBMPC_inch(void)
{
    char c;
    while (!_IBMPC_chrdy(&c))
      continue;

    return c;
} /* _IBMPC_inch */

 /*
  * Routine that can be used before interrupt management is initialized.
  */

char
BSP_wait_polled_input(void)
{
  char c;
  while (!_IBMPC_scankey(&c))
    continue;

  return c;
}

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_inch_sleep
|      Description: If charcter is ready return it, otherwise sleep until
|                   it is ready
| Global Variables: None.
|        Arguments: None.
|          Returns: character read from keyboard.
+--------------------------------------------------------------------------*/
char
_IBMPC_inch_sleep(void)
{
    char           c;
    rtems_interval ticks_per_second;

    ticks_per_second = 0;

    for(;;)
      {
	if(_IBMPC_chrdy(&c))
	  {
	    return c;
	  }

        if(ticks_per_second == 0)
          {
            rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND,
                            &ticks_per_second);
          }
	rtems_task_wake_after((ticks_per_second+24)/25);
      }

    return c;
} /* _IBMPC_inch */
#endif
