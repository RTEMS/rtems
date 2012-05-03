/*****************************************************************************
* File:     sci.c
*
* Desc:     This file contains the console IO routines for the SCI port.
*           There are two interfaces in this module. One is for the rtems
*           termios/console code and the other is a device driver interface.
*           This module works together with the termio module which is
*           sometimes referred to as the "line disciplines" which implements
*           terminal i/o processing like tabs, backspaces, and newlines.
*           The rtems printf uses interrupt io and the rtems printk routine
*           uses polled io which is better for debugging.
*
* Index:    Documentation
*           Section A  - Include Files
*           Section B  - Manifest Constants
*           Section C  - External Data
*           Section D  - External Functions
*           Section E  - Local Functions
*           Section F  - Local Variables
*           Section G  - A circular data buffer for rcv chars
*           Section H  - RTEMS termios callbacks for the interrupt api
*           Section I  - RTEMS termios callbacks for the polled api

*           Section 0  - Miscellaneous routines
*           Section 1  - Routines to manipulate the circular buffer
*           Section 2  - Interrupt based entry points for the termios module
*           Section 3  - Polling based entry points for the termios module
*           Section 4  - Device driver public api entry points
*           Section 5  - Hardware level routines
*           Section 6  - Testing and debugging code
*
* Refer:    Motorola QSM Reference Manual - Chapter 5 - SCI sub-module
*
* Note:     See bsp.h,confdefs.h,system.h for installing drivers into RTEMS.
*
*****************************************************************************/

/*****************************************************************************
  Overview of serial port console terminal input/output
*****************************************************************************/

/*
   +-----------+                               +---------+
   |    app    |                               |   app   |
   +-----------+                               +---------+
         |                                          |
         | (printf,scanf,etc.)                      |
         v                                          |
   +-----------+                                    |
   |    libc   |                                    |
   +-----------+                                    |
         |                                          |
         |                                          |
         |     (open,close,read,write,ioctl)        |
   ======|==========================================|========================
         | /dev/console                             | /dev/sci
         | (stdin,stdout,stderr)                    |
   ======|==========================================|========================
         |                                          |
         |                                          |
         v                                          v
   +-----------+         +-----------+         +---------+
   |  console  |  <--->  |  termios  |  <--->  |   sci   |
   |  driver   |         |  module   |         |  driver |
   +-----------+         +-----------+         +---------+
                                                    |
                                                    |
                                                    v
                                               +---------+
                                               |         |
                                               |  uart   |
                                               |         |
                                               +---------+
*/


/*****************************************************************************
  Section A - Include Files
*****************************************************************************/

#include <rtems.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <stdio.h>
#include <rtems/libio.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include "sci.h"
/*#include "../misc/include/cpu332.h" */


/*****************************************************************************
  Section B - Manifest Constants
*****************************************************************************/

#define SCI_MINOR       0                   /* minor device number */

/* IMPORTANT - if the device driver api is opened, it means the sci is being
 * used for direct hardware access, so other users (like termios) get ignored
 */
#define DRIVER_CLOSED   0                   /* the device driver api is closed */
#define DRIVER_OPENED   1                   /* the device driver api is opened */

/* system clock definitions, i dont have documentation on this... */

#if 0 /* Not needed, this is provided in mrm332.h */
#define XTAL            32768.0    /* crystal frequency in Hz */
#define NUMB_W          0          /* system clock parameters */
#define NUMB_X          1
#define NUMB_Y          0x38       /* for 14.942 Mhz */
#define NUMB_Y          0x3F       /* for 16.777 Mhz */

#define SYS_CLOCK       (XTAL * 4.0 * (NUMB_Y+1) * (1 << (2 * NUMB_W + NUMB_X)))

#endif


/*****************************************************************************
  Section C - External Data
*****************************************************************************/



/*****************************************************************************
  Section D - External Functions
*****************************************************************************/



/*****************************************************************************
  Section E - Local Functions
*****************************************************************************/

void SCI_output_char(char c);

/*rtems_isr SciIsr( rtems_vector_number vector );   interrupt handler */

const rtems_termios_callbacks * SciGetTermiosHandlers( int32_t   polled );

rtems_device_driver SciInitialize(                     /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciOpen(                           /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciClose(                          /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciRead(                           /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciWrite(                          /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciControl(                        /* device driver api */
    rtems_device_major_number, rtems_device_minor_number, void *);
rtems_device_driver SciRead (
    rtems_device_major_number, rtems_device_minor_number, void *);

int     SciInterruptOpen(int, int, void *);               /* termios api */
int     SciInterruptClose(int, int, void *);              /* termios api */
ssize_t SciInterruptWrite(int, const char *, size_t);     /* termios api */

int     SciSetAttributes(int, const struct termios*);     /* termios api */
int     SciPolledOpen(int, int, void *);                  /* termios api */
int     SciPolledClose(int, int, void *);                 /* termios api */
int     SciPolledRead(int);                               /* termios api */
ssize_t SciPolledWrite(int, const char *, size_t);        /* termios api */

static void SciSetBaud(uint32_t   rate);                /* hardware routine */
static void SciSetDataBits(uint16_t   bits);            /* hardware routine */
static void SciSetParity(uint16_t   parity);            /* hardware routine */

static void inline SciDisableAllInterrupts( void );     /* hardware routine */
static void inline SciDisableTransmitInterrupts( void );/* hardware routine */
static void inline SciDisableReceiveInterrupts( void ); /* hardware routine */

static void inline SciEnableTransmitInterrupts( void ); /* hardware routine */
static void inline SciEnableReceiveInterrupts( void );  /* hardware routine */

static void inline SciDisableReceiver( void );          /* hardware routine */
static void inline SciDisableTransmitter( void );       /* hardware routine */

static void inline SciEnableReceiver( void );           /* hardware routine */
static void inline SciEnableTransmitter( void );        /* hardware routine */

void SciWriteCharWait  ( uint8_t );                     /* hardware routine */
void SciWriteCharNoWait( uint8_t );                     /* hardware routine */

uint8_t   inline SciCharAvailable( void );              /* hardware routine */

uint8_t   inline SciReadCharWait( void );               /* hardware routine */
uint8_t   inline SciReadCharNoWait( void );             /* hardware routine */

void SciSendBreak( void );                              /* test routine */

static int8_t   SciRcvBufGetChar(void);                 /* circular rcv buf */
static void    SciRcvBufPutChar( uint8_t);              /* circular rcv buf */
#if 0
static void    SciRcvBufFlush( void );                  /* unused routine */
#endif

void SciUnitTest(void);                                 /* test routine */
void SciPrintStats(void);                               /* test routine */


/*****************************************************************************
  Section F - Local Variables
*****************************************************************************/

static struct rtems_termios_tty *SciTermioTty;

static uint8_t   SciInited = 0;             /* has the driver been inited */

static uint8_t   SciOpened;                 /* has the driver been opened */

static uint8_t   SciMajor;                  /* major device number */

static uint16_t   SciBaud;                  /* current value in baud register */

static uint32_t   SciBytesIn  = 0;          /* bytes received */
static uint32_t   SciBytesOut = 0;          /* bytes transmitted */

static uint32_t   SciErrorsParity  = 0;     /* error counter */
static uint32_t   SciErrorsNoise   = 0;     /* error counter */
static uint32_t   SciErrorsFraming = 0;     /* error counter */
static uint32_t   SciErrorsOverrun = 0;     /* error counter */

#if defined(CONSOLE_SCI)

/* this is what rtems printk uses to do polling based output */

BSP_output_char_function_type      BSP_output_char = SCI_output_char;
BSP_polling_getchar_function_type  BSP_poll_char   = NULL;

#endif

/*****************************************************************************
  Section G - A circular buffer for rcv chars when the driver interface is used.
*****************************************************************************/

/* it is trivial to wrap your buffer pointers when size is a power of two */

#define SCI_RCV_BUF_SIZE        256         /* must be a power of 2 !!! */

/* if someone opens the sci device using the device driver interface,
 * then the receive data interrupt handler will put characters in this buffer
 * instead of sending them up to the termios module for the console
 */
static uint8_t   SciRcvBuffer[SCI_RCV_BUF_SIZE];

static uint8_t   SciRcvBufPutIndex = 0; /* array index to put in next char */

static uint8_t   SciRcvBufGetIndex = 0; /* array index to take out next char */

static uint16_t  SciRcvBufCount = 0;   /* how many bytes are in the buffer */



/*****************************************************************************
  Section H - RTEMS termios callbacks for the interrupt version of the driver
*****************************************************************************/

static const rtems_termios_callbacks SciInterruptCallbacks =
{
    SciInterruptOpen,                       /* first open */
    SciInterruptClose,                      /* last close */
    NULL,                                   /* polled read (not required) */
    SciInterruptWrite,                      /* write */
    SciSetAttributes,                       /* set attributes */
    NULL,                                   /* stop remote xmit */
    NULL,                                   /* start remote xmit */
    TRUE                                    /* output uses interrupts */
};

/*****************************************************************************
  Section I - RTEMS termios callbacks for the polled version of the driver
*****************************************************************************/

static const rtems_termios_callbacks SciPolledCallbacks =
{
    SciPolledOpen,                          /* first open */
    SciPolledClose,                         /* last close */
    SciPolledRead,                          /* polled read */
    SciPolledWrite,                         /* write */
    SciSetAttributes,                       /* set attributes */
    NULL,                                   /* stop remote xmit */
    NULL,                                   /* start remote xmit */
    FALSE                                   /* output uses interrupts */
};


/*
 *                              SECTION 0
 *                        MISCELLANEOUS ROUTINES
 */

/****************************************************************************
 * Func:     SCI_output_char
 * Desc:     used by rtems printk function to send a char to the uart
 * Inputs:   the character to transmit
 * Outputs:  none
 * Errors:   none
 * Scope:    public
 ****************************************************************************/

void SCI_output_char(char c)
{
/*  ( minor device number, pointer to the character, length ) */

    SciPolledWrite( SCI_MINOR, &c, 1);

    return;
}


/****************************************************************************
* Func:     SciGetTermiosHandlers
* Desc:     returns a pointer to the table of serial io functions
*           this is called from console_open with polled set to false
* Inputs:   flag indicating whether we want polled or interrupt driven io
* Outputs:  pointer to function table
* Errors:   none
* Scope:    public
****************************************************************************/

const rtems_termios_callbacks * SciGetTermiosHandlers( int32_t   polled )
{
    if ( polled )
    {
        return &SciPolledCallbacks;             /* polling based */
    }
    else
    {
        return &SciInterruptCallbacks;          /* interrupt driven */
    }
}


/****************************************************************************
* Func:     SciIsr
* Desc:     interrupt handler for serial communications interface
* Inputs:   vector number - unused
* Outputs:  none
* Errors:   none
* Scope:    public API
****************************************************************************/

rtems_isr SciIsr( rtems_vector_number vector )
{
    uint8_t   ch;

    if ( (*SCSR) & SCI_ERROR_PARITY  )   SciErrorsParity  ++;
    if ( (*SCSR) & SCI_ERROR_FRAMING )   SciErrorsFraming ++;
    if ( (*SCSR) & SCI_ERROR_NOISE   )   SciErrorsNoise   ++;
    if ( (*SCSR) & SCI_ERROR_OVERRUN )   SciErrorsOverrun ++;

    /* see if it was a transmit interrupt */

    if ( (*SCSR) & SCI_XMTR_AVAILABLE )         /* data reg empty, xmt complete */
    {
        SciDisableTransmitInterrupts();

        /* tell termios module that the charcter was sent */
        /* he will call us later to transmit more if there are any */

        if (rtems_termios_dequeue_characters( SciTermioTty, 1 ))
        {
            /* there are more bytes to transmit so enable TX interrupt */

            SciEnableTransmitInterrupts();
        }
    }

    /* see if it was a receive interrupt */
    /* on the sci uart we just get one character per interrupt */

    while (  SciCharAvailable() )               /* char in data register? */
    {
        ch = SciReadCharNoWait();               /* get the char from the uart */

        /* IMPORTANT!!! */
        /* either send it to the termios module or keep it locally */

        if ( SciOpened == DRIVER_OPENED )       /* the driver is open */
        {
            SciRcvBufPutChar(ch);               /* keep it locally */
        }
        else                                    /* put in termios buffer */
        {
            char c = (char) ch;
            rtems_termios_enqueue_raw_characters( SciTermioTty, &c, 1 );
        }

        *SCSR &= SCI_CLEAR_RX_INT;              /* clear the interrupt */
    }
}


/*
 *                              SECTION 1
 *                ROUTINES TO MANIPULATE THE CIRCULAR BUFFER
 */

/****************************************************************************
* Func:     SciRcvBufGetChar
* Desc:     read a character from the circular buffer
*           make sure there is data before you call this!
* Inputs:   none
* Outputs:  the character or -1
* Errors:   none
* Scope:    private
****************************************************************************/

static int8_t   SciRcvBufGetChar(void)
{
    rtems_interrupt_level level;
    uint8_t   ch;

    if ( SciRcvBufCount == 0 )
    {
        rtems_fatal_error_occurred(0xDEAD);     /* check the count first! */
    }

    rtems_interrupt_disable( level );           /* disable interrupts */

    ch = SciRcvBuffer[SciRcvBufGetIndex];       /* get next byte */

    SciRcvBufGetIndex++;                        /* bump the index */

    SciRcvBufGetIndex &= SCI_RCV_BUF_SIZE - 1;  /* and wrap it */

    SciRcvBufCount--;                           /* decrement counter */

    rtems_interrupt_enable( level );            /* restore interrupts */

    return ch;                                  /* return the char */
}


/****************************************************************************
* Func:     SciRcvBufPutChar
* Desc:     put a character into the rcv data circular buffer
* Inputs:   the character
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void SciRcvBufPutChar( uint8_t   ch )
{
    rtems_interrupt_level level;

    if ( SciRcvBufCount == SCI_RCV_BUF_SIZE )   /* is there room? */
    {
        return;                                 /* no, throw it away */
    }

    rtems_interrupt_disable( level );           /* disable interrupts */

    SciRcvBuffer[SciRcvBufPutIndex] = ch;       /* put it in the buf */

    SciRcvBufPutIndex++;                        /* bump the index */

    SciRcvBufPutIndex &= SCI_RCV_BUF_SIZE - 1;  /* and wrap it */

    SciRcvBufCount++;                           /* increment counter */

    rtems_interrupt_enable( level );            /* restore interrupts */

    return;                                     /* return */
}


/****************************************************************************
* Func:     SciRcvBufFlush
* Desc:     completely reset and clear the rcv buffer
* Inputs:   none
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

#if 0                                           /* prevents compiler warning */
static void SciRcvBufFlush( void )
{
    rtems_interrupt_level level;

    rtems_interrupt_disable( level );           /* disable interrupts */

    memset( SciRcvBuffer, 0, sizeof(SciRcvBuffer) );

    SciRcvBufPutIndex = 0;                      /* clear */

    SciRcvBufGetIndex = 0;                      /* clear */

    SciRcvBufCount = 0;                         /* clear */

    rtems_interrupt_enable( level );            /* restore interrupts */

    return;                                     /* return */
}
#endif


/*
 *
 *                              SECTION 2
 *            INTERRUPT BASED ENTRY POINTS FOR THE TERMIOS MODULE
 */

/****************************************************************************
* Func:     SciInterruptOpen
* Desc:     open routine for the interrupt based device driver
*           Default state is 9600 baud, 8 bits, No parity, and 1 stop bit. ??
**CHANGED** Default baud rate is now 19200, 8N1
*           called from rtems_termios_open which is called from console_open
* Inputs:   major - device number
*           minor - device number
*           args - points to terminal info
* Outputs:  success/fail
* Errors:   none
* Scope:    public API
****************************************************************************/

int   SciInterruptOpen(
    int    major,
    int    minor,
    void  *arg
)
{
    rtems_libio_open_close_args_t * args = arg;
    rtems_isr_entry old_vector;

    if ( minor != SCI_MINOR )                   /* check minor device num */
    {
        return -1;
    }

    if ( !args )                                /* must have args */
    {
        return -1;
    }

    SciTermioTty = args->iop->data1;            /* save address of struct */

    SciDisableAllInterrupts();                  /* turn off sci interrupts */

    /* THIS IS ACTUALLY A BAD THING - SETTING LINE PARAMETERS HERE */
    /* IT SHOULD BE DONE THROUGH TCSETATTR() WHEN THE CONSOLE IS OPENED!!! */

/*  SciSetBaud(115200);                         set the baud rate */
/*  SciSetBaud( 57600);                         set the baud rate */
/*  SciSetBaud( 38400);                         set the baud rate */
/*  SciSetBaud( 19200);                         set the baud rate */
    SciSetBaud(  9600);                      /* set the baud rate */

    SciSetParity(SCI_PARITY_NONE);              /* set parity to none */

    SciSetDataBits(SCI_8_DATA_BITS);            /* set data bits to 8 */

    /* Install our interrupt handler into RTEMS, where does 66 come from? */

    rtems_interrupt_catch( SciIsr, 66, &old_vector );

    *QIVR  = 66;
    *QIVR &= 0xf8;
    *QILR |= 0x06 & 0x07;

    SciEnableTransmitter();                     /* enable the transmitter */

    SciEnableReceiver();                        /* enable the receiver */

    SciEnableReceiveInterrupts();               /* enable rcv interrupts */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciInterruptClose
* Desc:     close routine called by the termios module
* Inputs:   major - device number
*           minor - device number
*           args - unused
* Outputs:  success/fail
* Errors:   none
* Scope:    public - termio entry point
****************************************************************************/

int   SciInterruptClose(
    int    major,
    int    minor,
    void  *arg
)
{
    SciDisableAllInterrupts();

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciInterruptWrite
* Desc:     writes data to the uart using transmit interrupts
* Inputs:   minor - device number
*           buf - points to the data
*           len - number of bytes to send
* Outputs:  success/fail
* Errors:   none
* Scope:    public API
****************************************************************************/

ssize_t   SciInterruptWrite(
    int         minor,
    const char *buf,
    size_t      len
)
{
    /* We are using interrupt driven output so termios only sends us */
    /* one character at a time. The sci does not have a fifo. */

    if ( !len )                                 /* no data? */
    {
        return -1;                              /* return error */
    }

    if ( minor != SCI_MINOR )                   /* check the minor dev num */
    {
        return -1;                              /* return error */
    }

    if ( SciOpened == DRIVER_OPENED )           /* is the driver api open? */
    {
        return -1;                              /* yep, throw this away */
    }

    SciWriteCharNoWait(*buf);                   /* try to send a char */

    *SCSR &= SCI_CLEAR_TDRE;                    /* clear tx data reg empty flag */

    SciEnableTransmitInterrupts();              /* enable the tx interrupt */

    return 0;                                   /* return success */
}


/****************************************************************************
* Func:     SciSetAttributes
* Desc:     setup the uart based on the termios modules requests
* Inputs:   minor - device number
*           t - pointer to the termios info struct
* Outputs:  none
* Errors:   none
* Scope:    public API
****************************************************************************/

int   SciSetAttributes(
    int                   minor,
    const struct termios *t
)
{
    uint32_t    baud_requested;
    uint32_t    sci_rate = 0;
    uint16_t    sci_parity = 0;
    uint16_t    sci_databits = 0;

    if ( minor != SCI_MINOR )                   /* check the minor dev num */
    {
        return -1;                              /* return error */
    }

    /* if you look closely you will see this is the only thing we use */
    /* set the baud rate */

    baud_requested = t->c_cflag & CBAUD;        /* baud rate */

    if (!baud_requested)
    {
        baud_requested = B9600;                /* default to 9600 baud */
       /* baud_requested = B19200;                default to 19200 baud */
    }

    sci_rate = rtems_termios_baud_to_number( baud_requested );

    /* parity error detection */

    if (t->c_cflag & PARENB)                    /* enable parity detection? */
    {
        if (t->c_cflag & PARODD)
        {
            sci_parity = SCI_PARITY_ODD;        /* select odd parity */
        }
        else
        {
            sci_parity = SCI_PARITY_EVEN;       /* select even parity */
        }
    }
    else
    {
        sci_parity = SCI_PARITY_NONE;           /* no parity, most common */
    }

    /*  set the number of data bits, 8 is most common */

    if (t->c_cflag & CSIZE)                     /* was it specified? */
    {
        switch (t->c_cflag & CSIZE)
        {
            case CS8:   sci_databits = SCI_8_DATA_BITS;   break;
            default :   sci_databits = SCI_9_DATA_BITS;   break;
        }
    }
    else
    {
        sci_databits = SCI_8_DATA_BITS;         /* default to 8 data bits */
    }

    /*  the number of stop bits; always 1 for SCI */

    if (t->c_cflag & CSTOPB)
    {
        /* do nothing */
    }

    /* setup the hardware with these serial port parameters */

    SciSetBaud(sci_rate);                       /* set the baud rate */
    SciSetParity(sci_parity);                   /* set the parity type */
    SciSetDataBits(sci_databits);               /* set the data bits */

    return RTEMS_SUCCESSFUL;
}


/*
 *
 *                              SECTION 3
 *            POLLING BASED ENTRY POINTS FOR THE TERMIOS MODULE
 */

/****************************************************************************
* Func:     SciPolledOpen
* Desc:     open routine for the polled i/o version of the driver
*           called from rtems_termios_open which is called from console_open
* Inputs:   major - device number
*           minor - device number
*           args - points to terminal info struct
* Outputs:  success/fail
* Errors:   none
* Scope:    public - termios entry point
****************************************************************************/

int   SciPolledOpen(
    int   major,
    int   minor,
    void *arg
)
{
    rtems_libio_open_close_args_t * args = arg;

    if ( minor != SCI_MINOR )                   /* check minor device num */
    {
        return -1;
    }

    if ( !args )                                /* must have args */
    {
        return -1;
    }

    SciTermioTty = args->iop->data1;            /* Store tty pointer */

    SciDisableAllInterrupts();                  /* don't generate interrupts */

    /* THIS IS ACTUALLY A BAD THING - SETTING LINE PARAMETERS HERE */
    /* IT SHOULD BE DONE THROUGH TCSETATTR() WHEN THE CONSOLE IS OPENED!!! */

/*  SciSetBaud(115200);                            set the baud rate */
/*  SciSetBaud( 57600);                            set the baud rate */
/*  SciSetBaud( 38400);                            set the baud rate */
/*  SciSetBaud( 19200);                          * set the baud rate */
    SciSetBaud(  9600);                         /* set the baud rate */

    SciSetParity(SCI_PARITY_NONE);              /* set no parity */

    SciSetDataBits(SCI_8_DATA_BITS);            /* set 8 data bits */

    SciEnableTransmitter();                     /* enable the xmitter */

    SciEnableReceiver();                        /* enable the rcvr */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciPolledClose
* Desc:     close routine for the device driver, same for both
* Inputs:   major - device number
*           minor - device number
*           args - unused
* Outputs:  success/fail
* Errors:   none
* Scope:    public termios API
****************************************************************************/

int   SciPolledClose(
    int    major,
    int    minor,
    void  *arg
)
{
    SciDisableAllInterrupts();

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciPolledRead
* Desc:     polling based read routine for the uart
* Inputs:   minor - device number
* Outputs:  error or the character read
* Errors:   none
* Scope:    public API
****************************************************************************/

int   SciPolledRead(
    int   minor
)
{
    if ( minor != SCI_MINOR )               /* check the type-punned dev num */
    {
        return -1;                          /* return error */
    }

    if ( SciCharAvailable() )               /* if a char is available */
    {
        return SciReadCharNoWait();         /* read the rx data register */
    }

    return -1;                              /* return error */
}


/****************************************************************************
* Func:     SciPolledWrite
* Desc:     writes out characters in polled mode, waiting for the uart
*           check in console_open, but we only seem to use interrupt mode
* Inputs:   minor - device number
*           buf - points to the data
*           len - how many bytes
* Outputs:  error or number of bytes written
* Errors:   none
* Scope:    public termios API
****************************************************************************/

ssize_t SciPolledWrite(
    int         minor,
    const char *buf,
    size_t      len
)
{
    ssize_t written = 0;

    if ( minor != SCI_MINOR )                   /* check minor device num */
    {
        return -1;
    }

    if ( SciOpened == DRIVER_OPENED )           /* is the driver api open? */
    {
        return -1;                              /* toss the data */
    }

    /* send each byte in the string out the port */

    while ( written < len )
    {
        SciWriteCharWait(*buf++);               /* send a byte */

        written++;                              /* increment counter */
    }

    return written;                             /* return count */
}


/*
 *
 *                              SECTION 4
 *                 DEVICE DRIVER PUBLIC API ENTRY POINTS
 */

/****************************************************************************
* Func:     SciInit
* Desc:     Initialize the lasers device driver and hardware
* Inputs:   major - the major device number which is assigned by rtems
*           minor - the minor device number which is undefined at this point
*           arg - ?????
* Outputs:  RTEMS_SUCCESSFUL
* Errors:   None.
* Scope:    public API
****************************************************************************/

rtems_device_driver SciInitialize (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void * arg
)
{
/*     rtems_status_code status; */

/*printk("%s\r\n", __FUNCTION__); */

    /* register the SCI device name for termios console i/o
     * this is done over in console.c which doesn't seem exactly right
     * but there were problems doing it here...
     */

/*  status = rtems_io_register_name( "/dev/sci", major, 0 ); */

/*  if (status != RTEMS_SUCCESSFUL) */
/*      rtems_fatal_error_occurred(status); */

    SciMajor = major;                           /* save the rtems major number */

    SciOpened = DRIVER_CLOSED;                  /* initial state is closed */

    /* if you have an interrupt handler, install it here */

    SciInited = 1;                              /* set the inited flag */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciOpen
* Desc:     device driver open routine
*           you must open a device before you can anything else
*           only one process can have the device opened at a time
*           you could look at the task id to restrict access if you want
* Inputs:   major - the major device number assigned by rtems
*           minor - the minor device number assigned by us
*           arg - ?????
* Outputs:  see below
* Errors:   none
* Scope:    public API
****************************************************************************/

rtems_device_driver SciOpen (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void * arg
)
{
/*printk("%s major=%d minor=%d\r\n", __FUNCTION__,major,minor); */

    if (SciInited == 0)                         /* must be initialized first! */
    {
        return RTEMS_NOT_CONFIGURED;
    }

    if (minor != SCI_MINOR)
    {
        return RTEMS_INVALID_NAME;              /* verify minor number */
    }

    if (SciOpened == DRIVER_OPENED)
    {
        return RTEMS_RESOURCE_IN_USE;           /* already opened! */
    }

    SciOpened = DRIVER_OPENED;                  /* set the opened flag */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciClose
* Desc:     device driver close routine
*           the device must be opened before you can close it
*           the device must be closed before someone (else) can open it
* Inputs:   major - the major device number
*           minor - the minor device number
*           arg - ?????
* Outputs:  see below
* Errors:   none
* Scope:    public API
****************************************************************************/

rtems_device_driver SciClose (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void * arg
)
{
/*printk("%s major=%d minor=%d\r\n", __FUNCTION__,major,minor); */

    if (minor != SCI_MINOR)
    {
        return RTEMS_INVALID_NAME;              /* check the minor number */
    }

    if (SciOpened != DRIVER_OPENED)
    {
        return RTEMS_INCORRECT_STATE;           /* must be opened first */
    }

    SciOpened = DRIVER_CLOSED;                  /* set the flag */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciRead
* Desc:     device driver read routine
*           this function is not meaningful for the laser devices
* Inputs:   major - the major device number
*           minor - the minor device number
*           arg - read/write arguments
* Outputs:  see below
* Errors:   none
* Scope:    public API
****************************************************************************/

rtems_device_driver SciRead (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
{
    rtems_libio_rw_args_t *rw_args;             /* ptr to argument struct */
    char      *buffer;
    uint16_t   length;

    rw_args = (rtems_libio_rw_args_t *) arg;    /* arguments to read() */

    if (minor != SCI_MINOR)
    {
        return RTEMS_INVALID_NAME;              /* check the minor number */
    }

    if (SciOpened == DRIVER_CLOSED)
    {
        return RTEMS_INCORRECT_STATE;           /* must be opened first */
    }

    buffer = rw_args->buffer;                   /* points to user's buffer */

    length = rw_args->count;                    /* how many bytes they want */

/*  *buffer = SciReadCharWait();                   wait for a character */

    /* if there isn't a character available, wait until one shows up */
    /* or the timeout period expires, which ever happens first */

    if ( SciRcvBufCount == 0 )                  /* no chars */
    {
        /* wait for someone to wake me up... */
        /*rtems_task_wake_after(SciReadTimeout); */
    }

    if ( SciRcvBufCount )                       /* any characters locally? */
    {
        *buffer = SciRcvBufGetChar();           /* get the character */

        rw_args->bytes_moved = 1;               /* how many we actually read */
    }

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciWrite
* Desc:     device driver write routine
*           this function is not meaningful for the laser devices
* Inputs:   major - the major device number
*           minor - the minor device number
*           arg - read/write arguments
* Outputs:  see below
* Errors:   non3
* Scope:    public API
****************************************************************************/

rtems_device_driver SciWrite (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void * arg
)
{
    rtems_libio_rw_args_t *rw_args;             /* ptr to argument struct */
    uint8_t   *buffer;
    uint16_t   length;

    rw_args = (rtems_libio_rw_args_t *) arg;

    if (minor != SCI_MINOR)
    {
        return RTEMS_INVALID_NAME;              /* check the minor number */
    }

    if (SciOpened == DRIVER_CLOSED)
    {
        return RTEMS_INCORRECT_STATE;           /* must be opened first */
    }

    buffer = (uint8_t*)rw_args->buffer;       /* points to data */

    length = rw_args->count;                    /* how many bytes */

    while (length--)
    {
        SciWriteCharWait(*buffer++);            /* send the bytes out */
    }

    rw_args->bytes_moved = rw_args->count;      /* how many we wrote */

    return RTEMS_SUCCESSFUL;
}


/****************************************************************************
* Func:     SciControl
* Desc:     device driver control routine
*           see below for an example of how to use the ioctl interface
* Inputs:   major - the major device number
*           minor - the minor device number
*           arg - io control args
* Outputs:  see below
* Errors:   none
* Scope:    public API
****************************************************************************/

rtems_device_driver SciControl (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void * arg
)
{
    rtems_libio_ioctl_args_t *args = arg;       /* rtems arg struct */
    uint16_t   command;                         /* the cmd to execute */
    uint16_t   unused;                          /* maybe later */
    uint16_t   *ptr;                            /* ptr to user data */

/*printk("%s major=%d minor=%d\r\n", __FUNCTION__,major,minor); */

    /* do some sanity checking */

    if (minor != SCI_MINOR)
    {
        return RTEMS_INVALID_NAME;              /* check the minor number */
    }

    if (SciOpened == DRIVER_CLOSED)
    {
        return RTEMS_INCORRECT_STATE;           /* must be open first */
    }

    if (args == 0)
    {
        return RTEMS_INVALID_ADDRESS;           /* must have args */
    }

    args->ioctl_return = -1;                    /* assume an error */

    command = args->command;                    /* get the command */
    ptr     = args->buffer;                     /* this is an address */
    unused  = *ptr;                             /* brightness */

    if (command == SCI_SEND_BREAK)              /* process the command */
    {
        SciSendBreak();                         /* send break char */
    }

    args->ioctl_return = 0;                     /* return status */

    return RTEMS_SUCCESSFUL;
}


/*
 *
 *                              SECTION 5
 *                       HARDWARE LEVEL ROUTINES
 */

/****************************************************************************
* Func:     SciSetBaud
* Desc:     setup the uart based on the termios modules requests
* Inputs:   baud rate
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void SciSetBaud(uint32_t   rate)
{
    uint16_t   value;
    uint16_t   save_sccr1;

/* when you open the console you need to set the termio struct baud rate */
/* it has a default value of 9600, when someone calls tcsetattr it reverts! */

    SciBaud = rate;                             /* save the rate */

    /* calculate the register value as a float and convert to an int */
    /* set baud rate - you must define the system clock constant */
    /* see mrm332.h for an example */

    value = ( (uint16_t) ( SYS_CLOCK / rate / 32.0 + 0.5 ) & 0x1fff );

    save_sccr1 = *SCCR1;                        /* save register */

    /* also turns off the xmtr and rcvr */

    *SCCR1 &= SCI_DISABLE_INT_ALL;              /* disable interrupts */

    *SCCR0 = value;                             /* write the register */

    *SCCR1 = save_sccr1;                        /* restore register */

    return;
}


/****************************************************************************
* Func:     SciSetParity
* Desc:     setup the uart based on the termios modules requests
* Inputs:   parity
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void SciSetParity(uint16_t   parity)
{
    uint16_t   value;

    value = *SCCR1;                             /* get the register */

    if (parity == SCI_PARITY_ODD)
    {
        value |= SCI_PARITY_ENABLE;             /* parity enabled */
        value |= SCI_PARITY_ODD;                /* parity odd */
    }

    else if (parity == SCI_PARITY_EVEN)
    {
        value |= SCI_PARITY_ENABLE;             /* parity enabled */
        value &= ~SCI_PARITY_ODD;               /* parity even */
    }

    else if (parity == SCI_PARITY_NONE)
    {
        value &= ~SCI_PARITY_ENABLE;            /* disabled, most common */
    }

    /* else no changes */

    *SCCR1 = value;                             /* write the register */

    return;
}


/****************************************************************************
* Func:     SciSetDataBits
* Desc:     setup the uart based on the termios modules requests
* Inputs:   data bits
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void SciSetDataBits(uint16_t   bits)
{
    uint16_t   value;

    value = *SCCR1;                             /* get the register */

    /* note - the parity setting affects the number of data bits */

    if (bits == SCI_9_DATA_BITS)
    {
        value |= SCI_9_DATA_BITS;               /* 9 data bits */
    }

    else if (bits == SCI_8_DATA_BITS)
    {
        value &= SCI_8_DATA_BITS;               /* 8 data bits */
    }

    /* else no changes */

    *SCCR1 = value;                             /* write the register */

    return;
}


/****************************************************************************
* Func:     SciDisableAllInterrupts
* Func:     SciEnableTransmitInterrupts
* Func:     SciEnableReceiveInterrupts
* Desc:     handles generation of interrupts by the sci module
* Inputs:   none
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void inline SciDisableAllInterrupts( void )
{
    /* this also turns off the xmtr and rcvr */

    *SCCR1 &= SCI_DISABLE_INT_ALL;
}

static void inline SciEnableReceiveInterrupts( void )
{
    *SCCR1 |= SCI_ENABLE_INT_RX;
}

static void inline SciDisableReceiveInterrupts( void )
{
    *SCCR1 &= SCI_DISABLE_INT_RX;
}

static void inline SciEnableTransmitInterrupts( void )
{
    *SCCR1 |= SCI_ENABLE_INT_TX;
}

static void inline SciDisableTransmitInterrupts( void )
{
    *SCCR1 &= SCI_DISABLE_INT_TX;
}


/****************************************************************************
* Func:     SciEnableTransmitter, SciDisableTransmitter
* Func:     SciEnableReceiver,    SciDisableReceiver
* Desc:     turns the transmitter and receiver on and off
* Inputs:   none
* Outputs:  none
* Errors:   none
* Scope:    private
****************************************************************************/

static void inline SciEnableTransmitter( void )
{
    *SCCR1 |= SCI_ENABLE_XMTR;
}

static void inline SciDisableTransmitter( void )
{
    *SCCR1 &= SCI_DISABLE_XMTR;
}

static void inline SciEnableReceiver( void )
{
    *SCCR1 |= SCI_ENABLE_RCVR;
}

static void inline SciDisableReceiver( void )
{
    *SCCR1 &= SCI_DISABLE_RCVR;
}


/****************************************************************************
* Func:     SciWriteCharWait
* Desc:     wait for room in the fifo and then put a char in
* Inputs:   a byte to send
* Outputs:  none
* Errors:   none
* Scope:    public
****************************************************************************/

void SciWriteCharWait(uint8_t   c)
{
    /* poll the fifo, waiting for room for another character */

    while ( ( *SCSR & SCI_XMTR_AVAILABLE ) == 0 )
    {
        /* Either we are writing to the fifo faster than
         * the uart can clock bytes out onto the cable,
         * or we are in flow control (actually no, we
         * are ignoring flow control from the other end).
         * In the first case, higher baud rates will help.
         */
      /* relinquish processor while waiting */
      rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }

    *SCDR = c;                                  /* send the charcter */

    SciBytesOut++;                              /* increment the counter */

    return;
}

/****************************************************************************
* Func:     SciWriteCharNoWait
* Desc:     if no room in the fifo throw the char on the floor
* Inputs:   a byte to send
* Outputs:  none
* Errors:   none
* Scope:    public
****************************************************************************/

void SciWriteCharNoWait(uint8_t   c)
{
    if ( ( *SCSR & SCI_XMTR_AVAILABLE ) == 0 )
    {
        return;                                 /* no room, throw it away */
    }

    *SCDR = c;                                  /* put the char in the fifo */

    SciBytesOut++;                              /* increment the counter */

    return;
}


/****************************************************************************
* Func:     SciReadCharWait
* Desc:     read a character, waiting for one to show up, if need be
* Inputs:   none
* Outputs:  a character
* Errors:   none
* Scope:    public
****************************************************************************/

uint8_t   inline SciReadCharWait( void )
{
    uint8_t   ch;

    while ( SciCharAvailable() == 0 )           /* anything there? */
    {
      /* relinquish processor while waiting */
      rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    }

    /* if you have rcv ints enabled, then the isr will probably */
    /* get the character before you will unless you turn off ints */
    /* ie polling and ints don't mix that well */

    ch = *SCDR;                                 /* get the charcter */

    SciBytesIn++;                               /* increment the counter */

    return ch;                                  /* return the char */
}

/****************************************************************************
* Func:     SciReadCharNoWait
* Desc:     try to get a char but dont wait for one
* Inputs:   none
* Outputs:  a character or -1 if none
* Errors:   none
* Scope:    public
****************************************************************************/

uint8_t   inline SciReadCharNoWait( void )
{
    uint8_t   ch;

    if ( SciCharAvailable() == 0 )              /* anything there? */
        return -1;

    ch = *SCDR;                                 /* get the character */

    SciBytesIn++;                               /* increment the count */

    return ch;                                  /* return the char */
}


/****************************************************************************
* Func:     SciCharAvailable
* Desc:     is there a receive character in the data register
* Inputs:   none
* Outputs:  false if no char available, else true
* Errors:   none
* Scope:    public
****************************************************************************/

uint8_t   inline SciCharAvailable( void )
{
    return ( *SCSR & SCI_RCVR_READY );          /* char in data register? */
}


/****************************************************************************
* Func:     SciSendBreak
* Desc:     send 1 or tow breaks (all zero bits)
* Inputs:   none
* Outputs:  none
* Errors:   none
* Scope:    public
****************************************************************************/

void SciSendBreak( void )
{
    /* From the Motorola QSM reference manual - */

    /* "if SBK is toggled by writing it first to a one and then immediately */
    /* to a zero (in less than one serial frame interval), the transmitter */
    /* sends only one or two break frames before reverting to mark (idle) */
    /* or before commencing to send more data" */

    *SCCR1 |=  SCI_SEND_BREAK;                  /* set the bit */

    *SCCR1 &= ~SCI_SEND_BREAK;                  /* clear the bit */

    return;
}


/*
 *
 *                             SECTION 6
 *                             TEST CODE
 */

/****************************************************************************
* Func:     SciUnitTest
* Desc:     test the device driver
* Inputs:   nothing
* Outputs:  nothing
* Scope:    public
****************************************************************************/

#if 0
#define O_RDWR LIBIO_FLAGS_READ_WRITE           /* dont like this but... */

void SciUnitTest()
{
    uint8_t   byte;                             /* a character */
    uint16_t   fd;                              /* file descriptor for device */
    uint16_t   result;                          /* result of ioctl */

    fd = open("/dev/sci",O_RDWR);               /* open the device */

printk("SCI open fd=%d\r\n",fd);

    result = write(fd, "abcd\r\n", 6);          /* send a string */

printk("SCI write result=%d\r\n",result);

    result = read(fd, &byte, 1);                /* read a byte */

printk("SCI read result=%d,byte=%x\r\n",result,byte);

    return;
}
#endif


/****************************************************************************
* Func:     SciPrintStats
* Desc:     print out some driver information
* Inputs:   nothing
* Outputs:  nothing
* Scope:    public
****************************************************************************/

void SciPrintStats ( void )
{
    printk("\r\n");

    printk( "SYS_CLOCK is %2.6f Mhz\r\n\n", SYS_CLOCK / 1000000.0 );

    printk( "Current baud rate is %d bps or %d cps\r\n\n", SciBaud, SciBaud / 10 );

    printk( "SCI Uart chars in       %8d\r\n", SciBytesIn       );
    printk( "SCI Uart chars out      %8d\r\n", SciBytesOut      );
    printk( "SCI Uart framing errors %8d\r\n", SciErrorsFraming );
    printk( "SCI Uart parity  errors %8d\r\n", SciErrorsParity  );
    printk( "SCI Uart overrun errors %8d\r\n", SciErrorsOverrun );
    printk( "SCI Uart noise   errors %8d\r\n", SciErrorsNoise   );

    return;
}
