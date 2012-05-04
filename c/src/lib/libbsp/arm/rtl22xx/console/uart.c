/*
 *  console driver for RTL22xx UARTs
 *
 *  This driver uses the shared console driver in
 *  ...../libbsp/shared/console.c
 *
 *  If you want the driver to be interrupt driven, you
 *  need to write the ISR, and in the ISR insert the
 *  chars into termios's queue.
 *  Copyright (c) By ray <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
*/
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

/* Put the CPU (or UART) specific header file #include here */
#include <lpc22xx.h>
#include "lpc22xx_uart.h"

#include <libchip/serial.h>
#include <libchip/sersupp.h>

/* How many serial ports? */
#define NUM_DEVS       1

int     uart_poll_read(int minor);

int dbg_dly;

/* static function prototypes */
static int     uart_first_open(int major, int minor, void *arg);
static int     uart_last_close(int major, int minor, void *arg);
static int     uart_read(int minor);
static ssize_t uart_write(int minor, const char *buf, size_t len);
static void    uart_init(int minor);
static void    uart_write_polled(int minor, char c);
static int     uart_set_attributes(int minor, const struct termios *t);

/* These are used by code in console.c */
unsigned long Console_Configuration_Count = NUM_DEVS;

/* Pointers to functions for handling the UART. */
console_fns uart_fns =
{
    libchip_serial_default_probe,
    uart_first_open,
    uart_last_close,
    uart_read,
    uart_write,
    uart_init,
    uart_write_polled,   /* not used in this driver */
    uart_set_attributes,
    FALSE      /* TRUE if interrupt driven, FALSE if not. */
};

/*
 * There's one item in array for each UART.
 *
 * Some of these fields are marked "NOT USED". They are not used
 * by console.c, but may be used by drivers in libchip
 *
 */
console_tbl Console_Configuration_Ports[] = {
    {
        "/dev/com0",                      /* sDeviceName */
        SERIAL_CUSTOM,                    /* deviceType */
        &uart_fns,                        /* pDeviceFns */
        NULL,                             /* deviceProbe */
        NULL,                             /* pDeviceFlow */
        0,                                /* ulMargin - NOT USED */
        0,                                /* ulHysteresis - NOT USED */
        NULL,                             /* pDeviceParams */
        0,                                /* ulCtrlPort1  - NOT USED */
        0,                                /* ulCtrlPort2  - NOT USED */
        0,                                /* ulDataPort  - NOT USED */
        NULL,                             /* getRegister - NOT USED */
        NULL,                             /* setRegister - NOT USED */
        NULL,                             /* getData - NOT USED */
        NULL,                             /* setData - NOT USED */
        0,                                /* ulClock - NOT USED */
        0                                 /* ulIntVector - NOT USED */
    }
    #if 0
    {
        "/dev/com1",                      /* sDeviceName */
        SERIAL_CUSTOM,                    /* deviceType */
        &uart_fns,                        /* pDeviceFns */
        NULL,                             /* deviceProbe */
        NULL,                             /* pDeviceFlow */
        0,                                /* ulMargin - NOT USED */
        0,                                /* ulHysteresis - NOT USED */
        NULL,                             /* pDeviceParams */
        0,                                /* ulCtrlPort1  - NOT USED */
        0,                                /* ulCtrlPort2  - NOT USED */
        0,                                /* ulDataPort  - NOT USED */
        NULL,                             /* getRegister - NOT USED */
        NULL,                             /* setRegister - NOT USED */
        NULL,                             /* getData - NOT USED */
        NULL,                             /* setData - NOT USED */
        0,                                /* ulClock - NOT USED */
        0                                 /* ulIntVector - NOT USED */
    }
    #endif
};

/*********************************************************************/
/* Functions called via termios callbacks (i.e. the ones in uart_fns */
/*********************************************************************/

/*
 * This is called the first time each device is opened. If the driver
 * is interrupt driven, you should enable interrupts here. Otherwise,
 * it's probably safe to do nothing.
 *
 * Since micromonitor already set up the UART, we do nothing.
 */
static int uart_first_open(int major, int minor, void *arg)
{
    return 0;
}


/*
 * This is called the last time each device is closed. If the driver
 * is interrupt driven, you should disable interrupts here. Otherwise,
 * it's probably safe to do nothing.
 */
static int uart_last_close(int major, int minor, void *arg)
{
    return 0;
}


/*
 * Read one character from UART.
 *
 * Return -1 if there's no data, otherwise return
 * the character in lowest 8 bits of returned int.
 */
static int uart_read(int minor)
{
    char c;

    if (minor == 0) {
        if(U0LSR & ULSR_RDR) {
            c = U0RBR;
            return c;
        } else {
            return -1;
        }
    } else if (minor == 1) {
        if (U1LSR & ULSR_RDR) {
            c = U1RBR;
            return c;
        } else {
            return -1;
        }
    } else {
        printk("Unknown console minor number %d\n", minor);
        return -1;
    }

}


/*
 * Write buffer to UART
 *
 * return 1 on success, -1 on error
 */
static ssize_t uart_write(int minor, const char *buf, size_t len)
{
    size_t i;

    if (minor == 0) {
        for (i = 0; i < len; i++) {
            while(!(U0LSR & ULSR_THRE)) 	/* wait for TX buffer to empty*/
                  continue;						/* also either WDOG() or swap()*/
            U0THR = (char) buf[i];
        }
    } else if (minor == 1) {
            for (i = 0; i < len; i++) {
                while(!(U0LSR & ULSR_THRE)) 	/* wait for TX buffer to empty*/
                      continue;						/* also either WDOG() or swap()*/
                U0THR = (char) buf[i];
            }
        } else {
          printk("Unknown console minor number %d\n", minor);
          return -1;
    }

    return 1;
}


/* Set up the UART. */
static void uart_init(int minor)
{
#if 0 //init will be done in bspstart.c
	int baud=6;
	int mode =0x03;
	if(minor==0){
		// set port pins for UART0
		PINSEL0 =  (PINSEL0 & ~U0_PINMASK) | U0_PINSEL;

		U0IER = 0x00;                         // disable all interrupts

		// set the baudrate
		U0LCR = 1<<7;             // select divisor latches
		U0DLL = (uint8_t)baud;                // set for baud low byte
		U0DLM = (uint8_t)(baud >> 8);         // set for baud high byte

		// set the number of characters and other
		// user specified operating parameters
		U0LCR = (mode & ~ULCR_DLAB_ENABLE);
		U0FCR = mode>>8; /*fifo mode*/

		// set port pins for UART1
		PINSEL0 = (PINSEL0 & ~U1_PINMASK) | U1_PINSEL;

		U1IER = 0x00;						  // disable all interrupts
	}else if(minor==1){
		// set the baudrate
		U1LCR = ULCR_DLAB_ENABLE;			  // select divisor latches
		U1DLL = (uint8_t)baud;				  // set for baud low byte
		U1DLM = (uint8_t)(baud >> 8);		  // set for baud high byte

		// set the number of characters and other
		// user specified operating parameters
		U1LCR = (mode & ~ULCR_DLAB_ENABLE);
		U1FCR = mode>>8;/*fifo mode*/
	}

#endif
}

/* I'm not sure this is needed for the shared console driver. */
static void    uart_write_polled(int minor, char c)
{
    uart_write(minor, &c, 1);
}

/* This is for setting baud rate, bits, etc. */
static int     uart_set_attributes(int minor, const struct termios *t)
{
    return 0;
}

/***********************************************************************/
/*
 * The following functions are not used by TERMIOS, but other RTEMS
 * functions use them instead.
 */
/***********************************************************************/
/*
 * Read from UART. This is used in the exit code, and can't
 * rely on interrupts.
*/
int uart_poll_read(int minor)
{
    return uart_read(minor);
}


/*
 * Write a character to the console. This is used by printk() and
 * maybe other low level functions. It should not use interrupts or any
 * RTEMS system calls. It needs to be very simple
 */
static void _BSP_put_char( char c ) {
    uart_write_polled(0, c);
    if (c == '\n') {
        uart_write_polled(0, '\r');
    }
}

BSP_output_char_function_type BSP_output_char = _BSP_put_char;

static int _BSP_get_char(void)
{
  return uart_poll_read(0);
}

BSP_polling_getchar_function_type BSP_poll_char = _BSP_get_char;


/****************************************************************************
* init USART 0¡£8 bit, 1 Stop,No checkout, BPS115200
****************************************************************************/
void  UART0_Ini(void)
{
   long Fdiv;
   int i;
   PINSEL0 = 0x00000005;		    // I/O to UART0
   U0LCR = 0x83;		            // DLAB = 1
   Fdiv = (Fpclk >>4) / UART_BPS;  // configure BPS
   U0DLM = Fdiv/256;
   U0DLL = Fdiv%256;
   U0LCR = 0x03;

   for(i=0;i<10;i++){
     U0THR = 67;		//send a C to see if is OK
     while( (U0LSR&0x40)==0 );
   }
}


/****************************************************************************
*Send a Byte
****************************************************************************/
void  UART0_SendByte(char data)
{
    U0THR = data;

    while( (U0LSR&0x40)==0 );
}


/****************************************************************************
*Send a string
****************************************************************************/
void  UART0_SendStr(char const *str)
{  while(1)
   {  if( *str == '\0' ) break;
      UART0_SendByte(*str++);	    // Send the char
   }
}
