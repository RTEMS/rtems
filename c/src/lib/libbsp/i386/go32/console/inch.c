/*
 *  $Id$
 */

#include <pc.h>
#include <go32.h>
#include <bsp.h>
#include <cpu.h>

/*
 * Ports for PC keyboard
 */
#define KBD_CTL 0x61
#define KBD_DATA 0x60
#define KBD_STATUS 0x64

static char key_map[] = {
0,033,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
'q','w','e','r','t','y','u','i','o','p','[',']',015,0x80,
'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
0134,'z','x','c','v','b','n','m',',','.','/',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,'0',0177
};

static char shift_map[] = {
0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
'|','Z','X','C','V','B','N','M','<','>','?',0x80,
'*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
'1','2','3','0',177
};

extern int _IBMPC_Use_Go32_IO;

#define KBD_BUF_SIZE	256
static char		kbd_buffer[ KBD_BUF_SIZE ];
static unsigned int	kbd_first = 0;
static unsigned int	kbd_last  = 0;

/* This function can be called during a poll for input, or by an ISR.	*/
/* Basically any time you want to process a keypress.			*/
int _IBMPC_scankey( char * ch )
{
	unsigned char c;
	unsigned char outch;
	static int shift_pressed = 0;
	static int ctrl_pressed = 0;
	static int caps_pressed = 0;

	/* Read keyboard controller, toggle enable */
	inport_byte( KBD_CTL, c );
	outport_byte( KBD_CTL, c & ~0x80 );
	outport_byte( KBD_CTL, c | 0x80 );
	outport_byte( KBD_CTL, c & ~0x80 );

	/* See if it has data */
	inport_byte( KBD_STATUS, c );
	if ( ( c & 0x01 ) == 0 )
	    return 0;

	/* Read the data.  Handle nonsense with shift, control, etc. */
	inport_byte( KBD_DATA, c );
	switch ( c ) {
	case 0x36:
	case 0x2a:
		shift_pressed = 1;
		return 0;
	case 0x3a:
		caps_pressed = 1;
		return 0;
	case 0x1d:
		ctrl_pressed = 1;
		return 0;
	case 0xb6:
	case 0xaa:
		shift_pressed = 0;
		return 0;
	case 0xba:
		caps_pressed = 0;
		return 0;
	case 0x9d:
		ctrl_pressed = 0;
		return 0;
	/*
	 * Ignore unrecognized keys--usually arrow and such
	 */
	default:
		if ( c & 0x80 )
		    /* High-bit on means key is being released, not pressed */
		    return 0;
		if ( c == 88 )
		    /* F12 - abort */
		    exit( 1 );
		if ( c > 0x39 )  {
		    return 0;
		}
	}

	/* Strip high bit, look up in our map */
	c &= 127;
	if ( ctrl_pressed )  {
	    outch = key_map[c];
	    outch &= 037;
	} else {
	    outch = shift_pressed ? shift_map[c] : key_map[c];
	    if ( caps_pressed )  {
		if      ( outch >= 'A' && outch <= 'Z' )  outch += 'a' - 'A';
		else if ( outch >= 'a' && outch <= 'z' )  outch -= 'a' - 'A';
	    }
	}

	*ch = outch;
	return 1;
}


void _IBMPC_keyboard_isr( rtems_unsigned32 interrupt )
{
    if ( _IBMPC_scankey( & kbd_buffer[ kbd_last ] ) )  {
	/* Got one; save it if there is enough room in buffer. */
	unsigned int next = (kbd_last + 1) % KBD_BUF_SIZE;
	if ( next != kbd_first )
	    kbd_last = next;
    }

    /* Mark interrupt as handled */
    outport_byte( 0x20, 0x20 );
}


int _IBMPC_chrdy( char * ch )
{
    if ( _IBMPC_Use_Go32_IO )  {
	/* Read keyboard via BIOS: raw mode. */
	if ( kbhit() )  {
	    *ch = getkey();
	    return 1;
	} else {
	    return 0;
	}
    } else {
	/* Check buffer our ISR builds */
	if ( kbd_first != kbd_last )  {
	    *ch = kbd_buffer[ kbd_first ];
	    kbd_first = (kbd_first + 1) % KBD_BUF_SIZE;
	    return 1;
	} else {
	    return 0;
	}
    }
}

int _IBMPC_inch( void )
{
    char Ch;
    while ( ! _IBMPC_chrdy( & Ch ) )
	continue;
    return Ch;
}
