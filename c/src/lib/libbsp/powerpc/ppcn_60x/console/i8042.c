/*
 *  This file contains the PS2 keyboard driver for the i8042
 *
 *  Note that this driver will only handle a single i8042 device
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <libchip/serial.h>
#include <ringbuf.h>

#include "console.h"
#include "i8042_p.h"
#include "vga_p.h"

/*
 * UK style keyboard
 */
char pcUKNormalLookup[] = "1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'"
			  "`\0#zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0"
			  "\000789-456+1230.\0\0\\";
char pcUKShiftedLookup[]= "!\"£$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:"
			  "@\0\0~ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0"
			  "\0\000789-456+1230.\0\0|";
/*
 * US style keyboard
 */
char pcUSNormalLookup[] = "1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'"
			  "`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0"
			  "\0\000789-456+1230.\0\0\\";
char pcUSShiftedLookup[]= "!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:@~"
			  "\0|ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0"
			  "\000789-456+1230.\0\0|";

static char *pcNormalLookup;
static char *pcShiftedLookup;

/*
 * This is exported to vga.c to provide flow control
 */
volatile boolean bScrollLock=FALSE;

/*
 * If multiple devices are to be supported then a private copy of
 * the following will be required for each instance
 */
static boolean bCtrlPressed=FALSE;
static boolean bAltPressed=FALSE;
static boolean bAltGrPressed=FALSE;
static boolean bLShiftPressed=FALSE;
static boolean bRShiftPressed=FALSE;
static boolean bCapsLock=FALSE;
static boolean bNumLock=FALSE;
static boolean bTwoCode=FALSE;

#if CONSOLE_USE_INTERRUPTS
static volatile Ring_buffer_t KbdOutputBuffer;
static volatile boolean bProcessInterruptInput=FALSE;
static boolean bInterruptsEnabled=FALSE;
static volatile boolean bReceivedAck=TRUE;

static void i8042_process(
	int minor
);

static void i8042_scan_code(
	int minor,
	unsigned8   ucScan
);
#endif

static volatile Ring_buffer_t KbdInputBuffer;

/*
 * The following routines enable an interrupt driver to switch
 * to polled mode as required for command processing
 */
void i8042_polled_on(
	int minor 
)
{
#if CONSOLE_USE_INTERRUPTS
	bProcessInterruptInput=FALSE;
#endif
}

void i8042_polled_off(
	int minor 
)
{
#if CONSOLE_USE_INTERRUPTS
	unsigned32	Irql;
	unsigned8	ucScan;

	/*
	 * Make sure we have processed everything outstanding
	 */
	rtems_interrupt_disable(Irql);
	while(!Ring_buffer_Is_empty(&KbdInputBuffer))
	{
		rtems_interrupt_enable(Irql);
		Ring_buffer_Remove_character(&KbdInputBuffer,
					     ucScan);
		i8042_scan_code(minor, ucScan);
		rtems_interrupt_disable(Irql);
	}
	bProcessInterruptInput=TRUE;
	rtems_interrupt_enable(Irql);
#endif
}

/*
 * Send data to the keyboard
 */
static rtems_status_code
i8042_outbyte_raw(
	int minor,
	unsigned8 ucData
)
{
	unsigned32 i;
	unsigned8 Status;

#if CONSOLE_USE_INTERRUPTS
	unsigned32	Irql;

	if(bInterruptsEnabled)
	{
		Ring_buffer_Add_character(&KbdOutputBuffer,
					  ucData);
		if(!Console_Port_Data[minor].bActive)
		{
			/*
			 * Wake up the device
			 */
			rtems_interrupt_disable(Irql);
			Console_Port_Data[minor].bActive=TRUE;
			i8042_process(minor);
			rtems_interrupt_enable(Irql);

		}
		return RTEMS_SUCCESSFUL;
	}
#endif
	for (i=0; i<KBD_TIMEOUT; i++)
	{
		inport_byte(Console_Port_Tbl[minor].ulCtrlPort1, Status);
		if((Status & KBD_IBF_MASK)==0)
		{
			outport_byte(Console_Port_Tbl[minor].ulDataPort,
				     ucData);
			return RTEMS_SUCCESSFUL;
		}
	}
	return RTEMS_TIMEOUT;
}

/*
 * Read data from the keyboard
 */
static rtems_status_code
i8042_inbyte_polled(
	int minor,
	unsigned8 *pucData
)
{
	unsigned8 Status;

	inport_byte(Console_Port_Tbl[minor].ulCtrlPort1, Status);
	if(Status & KBD_OBF_MASK)
	{
		inport_byte(Console_Port_Tbl[minor].ulDataPort,
			    *pucData);
		return RTEMS_SUCCESSFUL;
	}
	return RTEMS_TIMEOUT;
}

/*
 * Blocking read data from the keyboard
 */
static rtems_status_code
i8042_inbyte_raw(
	int minor,
	unsigned8 *pucData
)
{
	int i;

#if CONSOLE_USE_INTERRUPTS
	if(bInterruptsEnabled)
	{
		i=0;
		while(Ring_buffer_Is_empty(&KbdInputBuffer))
		{
			rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
			if(i++==KBD_TIMEOUT)
			{
				return RTEMS_TIMEOUT;
			}
		}
		Ring_buffer_Remove_character(&KbdInputBuffer,
					     *pucData);
	}
	else
#endif
	{
		for (i=0; i<KBD_TIMEOUT; i++)
		{
			if(i8042_inbyte_polled(minor, pucData)==
			      RTEMS_SUCCESSFUL)
			{
				return RTEMS_SUCCESSFUL;
			}
			/*
			 * Wait for a character to be recieved
			 */
		}

		return RTEMS_TIMEOUT;
	}

	return RTEMS_SUCCESSFUL;
}

#if CONSOLE_USE_INTERRUPTS

/*
 * Send a command to the keyboard controller
 */
static rtems_status_code
i8042_outbyte_cmd_polled(
	int minor,
	unsigned8 ucCommand
)
{
	unsigned32 i;
	unsigned8 Status;

	/*
	 * This routine may be called when no clock driver is available
	 * so the timeout is dependant on the ISA bus timing used to access
	 * the i8042 which will not vary (much) across platforms.
	 */
	for (i=0; i<KBD_TIMEOUT; i++)
	{
		inport_byte(Console_Port_Tbl[minor].ulCtrlPort1, Status);
		if((Status & KBD_IBF_MASK)==0)
		{
			outport_byte(Console_Port_Tbl[minor].ulCtrlPort1,
				     ucCommand);
			return RTEMS_SUCCESSFUL;
		}
	}
	return RTEMS_TIMEOUT;
}
#endif

void EnqueueKbdChar(
	int minor,
	char cChar
)
{
#if CONSOLE_USE_INTERRUPTS
	rtems_termios_enqueue_raw_characters(
		Console_Port_Data[minor].termios_data,
		&cChar,
		1);
#else
	Ring_buffer_Add_character(&KbdInputBuffer, cChar);
#endif
}
/*
 * Process second code in a two code sequence
 */
static void i8042_process_two_code(
	int minor,
	unsigned8 ucScan,
	boolean bMakenBreak
)
{
	char cASCIICtrlCode;
	char cASCIICode;

	cASCIICtrlCode='\0';
	cASCIICode='\0';

	switch(ucScan)
	{
		case KEY_ALT:
		{
			bAltGrPressed=bMakenBreak;
			break;
		}

		case KEY_CONTROL:
		{
			bCtrlPressed=bMakenBreak;
			break;
		}

		case KEY_INSERT:
		{
			cASCIICtrlCode='@';
			break;
		}

		case KEY_DELETE:
		{
			cASCIICode=ASCII_DEL;
			break;
		}

		case KEY_LEFT_ARROW:
		{
			cASCIICtrlCode='D';
			break;
		}

		case KEY_HOME:
		{
			cASCIICtrlCode='H';
			break;
		}

		case KEY_END:
		{
			cASCIICtrlCode='K';
			break;
		}

		case KEY_UP_ARROW:
		{
			cASCIICtrlCode='A';
			break;
		}

		case KEY_DOWN_ARROW:
		{
			cASCIICtrlCode='B';
			break;
		}

		case KEY_PAGE_UP:
		{
			cASCIICtrlCode='?';
			break;
		}

		case KEY_PAGE_DOWN:
		{
			cASCIICtrlCode='/';
			break;
		}

		case KEY_RIGHT_ARROW:
		{
			cASCIICtrlCode='C';
			break;
		}

		case KEY_KEYPAD_SLASH:
		{
			cASCIICode='/';
			break;
		}

		case KEY_KEYPAD_ENTER:
		{
			cASCIICode=ASCII_CR;
			break;
		}

		case KEY_PRINT_SCREEN:
		{
			cASCIICode=ASCII_SYSRQ;
			break;
		}

		default:
		{
			/*
			 * Ignore this code
			 */
			break;
		}
	}

	if(bMakenBreak && cASCIICode)
	{
		EnqueueKbdChar(minor, cASCIICode);
	}
	else if(bMakenBreak && cASCIICtrlCode)
	{
		EnqueueKbdChar(minor, ASCII_CSI);
		EnqueueKbdChar(minor, cASCIICtrlCode);
	}
}

static boolean i8042_process_qualifiers(
	unsigned8 ucScan,
	boolean	bMakenBreak
)
{
	boolean	bProcessed;

	/*
	 * Check for scan codes for shift, control, or alt keys.
	 */
	bProcessed=TRUE;

	switch (ucScan)
	{
		case KEY_LEFT_SHIFT:
		{
			bLShiftPressed=bMakenBreak;
			break;
		}

		case KEY_RIGHT_SHIFT:
		{
			bRShiftPressed=bMakenBreak;
			break;
		}

		case KEY_CONTROL:
		{
			bCtrlPressed=bMakenBreak;
			break;
		}

		case KEY_ALT:
		{
			bAltPressed=bMakenBreak;
			break;
		}

		default:
		{
			/*
			 * Something else needs to process this code
			 */
			bProcessed=FALSE;
			break;
		}
	}

	return(bProcessed);
}

static boolean i8042_process_top_row(
	int		minor,
	unsigned8	ucScan
)
{
	boolean	bProcessed;
	char	cASCIIFnCode;
#if CONSOLE_USE_INTERRUPTS==0
	unsigned8	ucKeyboardAck;
#endif

	/*
	 * Check for keys on the top row
	 */
	bProcessed=TRUE;
	cASCIIFnCode='\0';

	switch (ucScan)
	{
		case KEY_ESC:
		{
			EnqueueKbdChar(minor, ASCII_ESC);
			break;
		}

		case KEY_F1:
		{
			cASCIIFnCode='P';
			break;
		}

		case KEY_F2:
		{
			cASCIIFnCode='Q';
			break;
		}

		case KEY_F3:
		{
			cASCIIFnCode='w';
			break;
		}

		case KEY_F4:
		{
			cASCIIFnCode='x';
			break;
		}

		case KEY_F5:
		{
			cASCIIFnCode='t';
			break;
		}

		case KEY_F6:
		{
			cASCIIFnCode='u';
			break;
		}

		case KEY_F7:
		{
			cASCIIFnCode='q';
			break;
		}

		case KEY_F8:
		{
			cASCIIFnCode='r';
			break;
		}

		case KEY_F9:
		{
			cASCIIFnCode='p';
			break;
		}

		case KEY_F10:
		{
			cASCIIFnCode='M';
			break;
		}

		case KEY_F11:
		{
			cASCIIFnCode='A';
			break;
		}

		case KEY_F12:
		{
			cASCIIFnCode='B';
			break;
		}

		case KEY_SYS_REQUEST:
		{
			EnqueueKbdChar(minor, ASCII_SYSRQ);
			break;
		}

		case KEY_CAPS_LOCK:
		case KEY_NUM_LOCK:
		case KEY_SCROLL_LOCK:
		{
			switch(ucScan)
			{
				case KEY_CAPS_LOCK:
				{
					bCapsLock=!bCapsLock;
					break;
				}

				case KEY_NUM_LOCK:
				{
					bNumLock=!bNumLock;
					break;
				}

				case KEY_SCROLL_LOCK:
				{
#if CONSOLE_USE_INTERRUPTS
					bScrollLock=!bScrollLock;
#endif
					break;
				}
			}

			i8042_outbyte_raw(minor, KBD_CMD_SET_LEDS);
#if CONSOLE_USE_INTERRUPTS==0
			i8042_inbyte_raw(minor, &ucKeyboardAck);
#endif
			i8042_outbyte_raw(minor,
					  (bCapsLock ? KBD_LED_CAPS : 0) |
					  (bNumLock ? KBD_LED_NUM : 0) |
					  (bScrollLock ? KBD_LED_SCROLL : 0));
#if CONSOLE_USE_INTERRUPTS==0
			i8042_inbyte_raw(minor, &ucKeyboardAck);
#endif

			break;
		}

		default:
		{
			/*
			 * Something else needs to process this code
			 */
			bProcessed=FALSE;
			break;
		}
	}

	if(cASCIIFnCode)
	{
		EnqueueKbdChar(minor, ASCII_CSI);
		EnqueueKbdChar(minor, 'O');
		EnqueueKbdChar(minor, cASCIIFnCode);
	}

	return(bProcessed);
}

static boolean i8042_process_keypad(
	int	minor,
	unsigned8 ucScan
)
{
	char cASCIICtrlCode;

	/*
	 * Process keys on the keypad
	 */
	cASCIICtrlCode='\0';

	switch(ucScan)
	{
		case KEY_UP_ARROW:
		{
			cASCIICtrlCode='A';
			break;
		}

		case KEY_DOWN_ARROW:
		{
			cASCIICtrlCode='B';
			break;
		}

		case KEY_RIGHT_ARROW:
		{
			cASCIICtrlCode='C';
			break;
		}

		case KEY_LEFT_ARROW:
		{
			cASCIICtrlCode='D';
			break;
		}

		case KEY_HOME:
		{
			cASCIICtrlCode='H';
			break;
		}

		case KEY_END:
		{
			cASCIICtrlCode='K';
			break;
		}

		case KEY_PAGE_UP:
		{
			cASCIICtrlCode='?';
			break;
		}

		case KEY_PAGE_DOWN:
		{
			cASCIICtrlCode='/';
			break;
		}

		case KEY_INSERT:
		{
			cASCIICtrlCode='@';
			break;
		}

		case KEY_DELETE:
		{
			/*
			 * This is a special case not requiring an ASCII_CSI
			 */
			EnqueueKbdChar(minor, ASCII_DEL);
			return(TRUE);
		}

		default:
		{
			/*
			 * Something else needs to process this code
			 */
			break;
		}
	}

	if(cASCIICtrlCode)
	{
		EnqueueKbdChar(minor, ASCII_CSI);
		EnqueueKbdChar(minor, cASCIICtrlCode);
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

/*
 * This routine translates the keyboard scan code into an
 * ASCII character (or sequence) and queues it
 */
static void i8042_scan_code(
	int minor,
	unsigned8   ucScan
)
{
	char cChar;
	boolean	bMakenBreak;

	/*
	 * Check for code 0xe0, which introduces a two key sequence.
	 */

	if(ucScan==KEY_TWO_KEY)
	{
		bTwoCode=TRUE;
		return;
	}

	/*
	 * Bit 7 of scan code indicates make or break
	 */
	if(ucScan & 0x80)
	{
		bMakenBreak=FALSE;
		ucScan&=0x7f;
	}
	else
	{
		bMakenBreak=TRUE;
	}

	/*
	 * If we are in a multikey sequence then process the second keypress
	 */
	if(bTwoCode)
	{
		i8042_process_two_code(minor, ucScan, bMakenBreak);

		/*
		 * Revert to prcessing single key sequences
		 */
		bTwoCode=FALSE;
		return;
	}

	if(i8042_process_qualifiers(ucScan, bMakenBreak))
	{
		/*
		 * We are all done
		 */
		return;
	}

	/*
	 * The remaining keys are only processed for make
	 */
	if(!bMakenBreak)
	{
		return;
	}

	if(i8042_process_top_row(minor, ucScan))
	{
		/*
		 * We are all done
		 */
		return;
	}

	if(!bNumLock && i8042_process_keypad(minor, ucScan))
	{
		/*
		 * We are all done
		 */
		return;
	}

	/*
	 * Process "normal" keys
	 */

	cChar=0;

	/*
	 * Check to see if the scan code corresponds to an ASCII
	 * character.
	 */
	if(((ucScan >= 16) && (ucScan <= 25)) ||
	   ((ucScan >= 30) && (ucScan <= 38)) ||
	   ((ucScan >= 44) && (ucScan <= 50)))
	{
		if(bCtrlPressed)
		{
			cChar=pcNormalLookup[ucScan - 2]-'a'+1;
		}
		else
		{
			if(((bLShiftPressed || bRShiftPressed) && !bCapsLock) ||
			   (!(bLShiftPressed || bRShiftPressed) && bCapsLock))
			{
				cChar=pcShiftedLookup[ucScan - 2];
			}
			else
			{
				cChar=pcNormalLookup[ucScan - 2];
			}
		}
	}
	else if((ucScan > 1) && (ucScan <= 0x56))
	{
		/*
		 * Its ASCII but not alpha, so don't shift on CapsLock.
		 */
		if(bLShiftPressed || bRShiftPressed)
		{
			cChar=pcShiftedLookup[ucScan - 2];
		}
		else
		{
			cChar=pcNormalLookup[ucScan - 2];
		}
	}

	/* 
	 * If we got a character then queue it
	 */
	if(cChar)
	{
		EnqueueKbdChar(minor, cChar);
	}
}

/*
 *  Console Device Driver Entry Points
 */
boolean i8042_probe(int minor)
{
	unsigned8 ucKeyboardAck;
	unsigned8 ucKeyboardID1, ucKeyboardID2;

	if(!vga_probe(minor))
	{
		/*
		 * There is no VGA adaptor so fail probe
		 */
		return(FALSE);
	}

	Ring_buffer_Initialize(&KbdInputBuffer);
#if CONSOLE_USE_INTERRUPTS
	Ring_buffer_Initialize(&KbdOutputBuffer);
#endif

	i8042_polled_on(minor);
	/*
	 * Determine keyboard type
	 */
	i8042_outbyte_raw(minor, KBD_CMD_READ_ID);
	ucKeyboardAck=0;
	if((i8042_inbyte_raw(minor, &ucKeyboardAck)==RTEMS_TIMEOUT) ||
	   (ucKeyboardAck==KBD_CMD_RESEND))
	{
		/*
		 * No or incorrect keyboard response so fail probe
		 */
		return(FALSE);
	}

	i8042_inbyte_raw(minor, &ucKeyboardID1);
	i8042_inbyte_raw(minor, &ucKeyboardID2);
	if((ucKeyboardID1==0xab) && (ucKeyboardID2==0x41))
	{
		pcNormalLookup=&pcUKNormalLookup[0];
		pcShiftedLookup=&pcUKShiftedLookup[0];
	}
	else
	{
		pcNormalLookup=&pcUSNormalLookup[0];
		pcShiftedLookup=&pcUSShiftedLookup[0];
	}
	i8042_polled_off(minor);

	return(TRUE);
}

void i8042_init(int minor)
{
	unsigned8 ucKeyboardAck;

	vga_init(minor);

	i8042_polled_on(minor);
	/*
	 * Switch all LEDs off
	 */
	i8042_outbyte_raw(minor, KBD_CMD_SET_LEDS);
	i8042_inbyte_raw(minor, &ucKeyboardAck);
	i8042_outbyte_raw(minor, 0);
	i8042_inbyte_raw(minor, &ucKeyboardAck);
	/*
	 * Select scan code set 1
	 */
	i8042_outbyte_raw(minor, KBD_CMD_SEL_SCAN_CODE);
	i8042_inbyte_raw(minor, &ucKeyboardAck);
	i8042_outbyte_raw(minor, 1);
	i8042_inbyte_raw(minor, &ucKeyboardAck);
	i8042_polled_off(minor);
}

/* PAGE
 *
 *  i8042_inbyte_nonblocking_polled 
 *
 *  Console Termios polling input entry point.
 */

int i8042_inbyte_nonblocking_polled( 
	int minor 
)
{
	unsigned8	ucScan;
	char	ucData;

	if(i8042_inbyte_polled(minor, &ucScan)==RTEMS_SUCCESSFUL)
	{
		i8042_scan_code(minor, ucScan);
	}

	if(!Ring_buffer_Is_empty(&KbdInputBuffer))
	{
		Ring_buffer_Remove_character(&KbdInputBuffer,
					     ucData);
		return(ucData);
	}

	return(-1);
}

#if CONSOLE_USE_INTERRUPTS
/*
 *  i8042_isr
 *
 *  This routine is the console interrupt handler for the keyboard
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */
static void i8042_process(
	int	minor
)
{
	unsigned8	Status;
	unsigned8	ucData;

	inport_byte(Console_Port_Tbl[minor].ulCtrlPort1, Status);

	if(Status & KBD_OBF_MASK)
	{
		inport_byte(Console_Port_Tbl[minor].ulDataPort, ucData);

		if(bProcessInterruptInput)
		{
			/*
			 * Every byte written to the keyboard should be followed
			 * by an acknowledge
			 */
			if(ucData==KBD_CMD_ACK)
			{
				bReceivedAck=TRUE;
			}
			else
			{
				i8042_scan_code(minor, ucData);
			}
		}
		else
		{
			/*
			 * Store the scan code into the ring buffer where it
			 * can be read using i8042_inbyte_raw()
			 */
			Ring_buffer_Add_character(&KbdInputBuffer, ucData);
		}
	}

	if(((Status & KBD_IBF_MASK)==0) &&
	   bReceivedAck)
	{
		if(Ring_buffer_Is_empty(&KbdOutputBuffer))
		{
			Console_Port_Data[minor].bActive=FALSE;
		}
		else
		{
			Ring_buffer_Remove_character(&KbdOutputBuffer,
						     ucData);
			outport_byte(Console_Port_Tbl[minor].ulDataPort,
				     ucData);
			bReceivedAck=FALSE;
		}
	}
}

static rtems_isr i8042_isr(
  rtems_vector_number vector
)
{
        int     minor;

        for(minor=0;minor<Console_Port_Count;minor++)
        {
                if(vector==Console_Port_Tbl[minor].ulIntVector)
                {
                        i8042_process(minor);
                }
        }
}

void i8042_initialize_interrupts(int minor)
{
	i8042_init(minor);

	Console_Port_Data[minor].bActive=FALSE;

	set_vector(i8042_isr,
		   Console_Port_Tbl[minor].ulIntVector,
		   1);

	i8042_outbyte_cmd_polled(minor, KBD_CTR_WRITE_COMMAND);
	i8042_outbyte_raw(minor, KBD_CMD_ENABLE_KBD_INT);

	/*
	 * At this point interrupts should spring into life
	 */
	bInterruptsEnabled=TRUE;
}

#endif /* CONSOLE_USE_INTERRUPTS */
