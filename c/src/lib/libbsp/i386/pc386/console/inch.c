/*-------------------------------------------------------------------------+
| inch.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   inch.c,v 1.3 1995/12/19 20:07:25 joel Exp - go32 BSP
| With the following copyright notice:
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define KBD_CTL      0x61  /* -------------------------------- */
#define KBD_DATA     0x60  /* Ports for PC keyboard controller */
#define KBD_STATUS   0x64  /* -------------------------------- */

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

static unsigned short   kbd_buffer[KBD_BUF_SIZE];
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
|          Returns: TRUE in case a valid character has been read,
|                   FALSE otherwise.
+--------------------------------------------------------------------------*/
static bool
_IBMPC_scankey(char *outChar)
{
  unsigned char inChar;
  static int alt_pressed   = 0;
  static int ctrl_pressed  = 0;
  static int shift_pressed = 0;
  static int caps_pressed  = 0;
  static int extended      = 0;

  *outChar = '\0'; /* default value if we return false */

  /* Read keyboard controller, toggle enable */
  inport_byte(KBD_CTL, inChar);
  outport_byte(KBD_CTL, inChar & ~0x80);
  outport_byte(KBD_CTL, inChar | 0x80);
  outport_byte(KBD_CTL, inChar & ~0x80);

  /* See if it has data */
  inport_byte(KBD_STATUS, inChar);
  if ((inChar & 0x01) == 0)
    return false;

  /* Read the data.  Handle nonsense with shift, control, etc. */
  inport_byte(KBD_DATA, inChar);

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
|         Function: _IBMPC_chrdy
|      Description: Check keyboard ISR buffer and return character if not empty.
| Global Variables: kbd_buffer, kbd_first, kbd_last.
|        Arguments: c - character read if keyboard buffer not empty, otherwise
|                   unchanged.
|          Returns: TRUE if keyboard buffer not empty, FALSE otherwise.
+--------------------------------------------------------------------------*/
static bool
_IBMPC_chrdy(char *c)
{
  /* FIX ME!!! It doesn't work without something like the following line.
     Find out why! */
  printk("");

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

int
BSP_wait_polled_input(void)
{
  char c;
  while (!_IBMPC_scankey(&c))
    continue;

  return c;
}

/*
 * Check if a key has been pressed. This is a non-destructive
 * call, meaning, it keeps the key in the buffer.
 */
int rtems_kbpoll( void )
{
  int                    rc;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  rc = ( kbd_first != kbd_last ) ? TRUE : FALSE;

  rtems_interrupt_enable(level);

  return rc;
}

int getch( void )
{
  int c;

  while( kbd_first == kbd_last )
  {
     rtems_task_wake_after( 10 );
  }
  c = kbd_buffer[ kbd_first ];
  kbd_first = (kbd_first + 1) % KBD_BUF_SIZE;
  return c;
}

void add_to_queue( unsigned short b )
{
 unsigned int next;
 kbd_buffer[ kbd_last ] = b;
 next = (kbd_last == kbd_end) ? 0 : kbd_last + 1;
 if( next != kbd_first )
 {
    kbd_last = next;
 }
}
