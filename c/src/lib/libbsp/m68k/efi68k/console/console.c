/*
 *  This file contains the efi68k console IO package.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/libio.h>


#define BAUD 38400
#define CLK_FREQ 8000000.0

/* BUFFER_LENGTH must be 2^n for n=1, 2, 3, .... */
#define BUFFER_LENGTH 256
#define RTS_STOP_SIZE BUFFER_LENGTH-64
#define RTS_START_SIZE 16

char xmt_buf[BUFFER_LENGTH];
char rcv_buf[BUFFER_LENGTH];
/* in: last entry into the buffer; always on a valid character */
/* out: points to the next character to be pull from the buffer */
/*    in+1=out => buffer empty */
/*    in+2=out => buffer full */
struct UART_buf {
  char *offset; 
  char *in; 
  char *out;
};
static volatile struct UART_buf  xmt = { xmt_buf, (char *)0, (char *)1};
static volatile struct UART_buf  rcv = { rcv_buf, (char *)0, (char *)1};
static volatile char _debug_flag = 0;
static volatile char _tx_stop = 0;

/* _catchUARTint is the interrupt front-end */
extern void _catchUARTint();
asm("   .text\n\
        .align 2\n\
        .globl _catchUARTint\n\
_catchUARTint:\n\
        lea    %sp@(4),%sp                /* pop return address */\n\
        moveml %d0-%d7/%a0-%a6,%sp@-       /* save registers */\n\
        jbsr    uart_interrupt\n\
        moveml  %sp@+,%d0-%d7/%a0-%a6			        \n\
        rte\n\
    ");

/* _fake_trap_1 will continue the UART interrupt (%sr *still*
   UART_ISR_LEVEL) as a trap #1 to enter the debugger */
asm("   .text\n\
        .align 2\n\
_fake_trap_1:\n\
        unlk %a6		/* clear interrupt frame */\n\
        lea %sp@(4),%sp		/* remove jbsr instruction */\n\
        moveml %sp@+,%d0-%d7/%a0-%a6 /* pop registers */\n\
        jmp (33*6-12+_VBR)	/* jump exception 1 */\n\
        ");

/* dispatch UART interrupt */
void xmit_interrupt(void);
void rcvr_interrupt(void);
void modem_status(void);
void _fake_trap_1(void);
void uart_interrupt(void) {
  register char a;

  a=*IIR & (NIP | IID_MASK); /* read interrupt id register */
  switch (a) {
  case 0x04: case 0x0c:
    rcvr_interrupt();
    break;
  case 0x02:
    xmit_interrupt();
    break;
  case 0x00:
    modem_status();
    break;
  default:
    break;
  }
  if (_debug_flag) {
    _debug_flag = 0;		/* reset the flag */
    _fake_trap_1();		/* fake a trap #1 */
  }
}

/* transfer received character to the buffer */
void rcvr_interrupt(void) {
  register char *a, c;
  register int length;

  while ( (*LSR & DR) != 0) {
    if ((c=*RBR) == 0x1a)	/* use ctl-z to reboot */
      reboot();
    else if (c == 0x03) {	/* use ctl-c to enter debugger */
      _debug_flag = 1;
      continue;
    }
    *(char *)((int)rcv.offset +(int)
	      (a=(char *)(((int)rcv.in+1) & ((int)BUFFER_LENGTH-1)))) = c;
    if ((char *)(((int)rcv.in+2) & ((int)BUFFER_LENGTH-1)) != rcv.out)
      rcv.in=a;
  }
  length = (BUFFER_LENGTH -1) & ( 
    ( ((int)rcv.out <= (int)rcv.in) ? 0 : BUFFER_LENGTH) - (int)rcv.out 
    + (int)rcv.in + 1);
  if (length >= RTS_STOP_SIZE)
    *MCR &= (char) (~RTS);
}

/* tranfer buffered characters to the UART */
void xmit_interrupt(void) {
  register short int i, oldsr;

  _CPU_ISR_Disable( oldsr ); /* for when outbyte calls */
  if ( (*LSR & THRE) != 0 && _tx_stop == 0 )
    for (i=0;i<16;i++) {
      if ((char *)(((int)xmt.in+1) & ((int)BUFFER_LENGTH-1))== xmt.out)
	break;
      *THR=*(char *)((int)xmt.offset+(int)xmt.out);
      xmt.out= (char *)(((int)xmt.out+1) & ((int)BUFFER_LENGTH-1));
    }
  _CPU_ISR_Enable( oldsr );
}

void modem_status(void) {
  register char a;
  
  if ( ((a=*MDSR) & DCTS) != 0 ) {
    if ( (a & CTS) == 0)
      _tx_stop = 1;
    else {
      _tx_stop = 0;
      xmit_interrupt();
    }
  }
}

/* transfer character from the buffer */
char inbyte(void) {
  register char a;
  register int length;

  while ((char *)(((int)rcv.in+1) & ((int)BUFFER_LENGTH-1))== rcv.out);
  a=*(char *)((int)rcv.offset+(int)rcv.out);
  rcv.out= (char *)(((int)rcv.out+1) & ((int)BUFFER_LENGTH-1));
  length = (BUFFER_LENGTH -1) & ( 
    ( ((int)rcv.out <= (int)rcv.in) ? 0 : BUFFER_LENGTH) - (int)rcv.out 
    + (int)rcv.in + 1);
  if (length < RTS_START_SIZE)
    *MCR |= (char) RTS;
  return (a);
}

/* once room is avaliable in the buffer, transfer
   the character into the buffer and enable 
   the xmtr interrupt */
void outbyte(char c) {
  register char *a;
  
  while ((char *)(((int)xmt.in+2) & ((int)BUFFER_LENGTH-1)) == xmt.out);
  *(char *)((int)xmt.offset+(int)
	    (a=(char *)(((int)xmt.in+1) & ((int)BUFFER_LENGTH-1))))=c;
  xmt.in=a;

  if ( (*LSR & THRE) != 0 ) 	/* if THRE, uart has already interrupted */
    xmit_interrupt();		/*   and was ignored. Need to restart. */
}

void _UART_flush(void) {
  /* loop till xmt buffer empty. Works with interrupts disabled */
  while ((char *)(((int)xmt.in+1) & ((int)BUFFER_LENGTH-1)) != xmt.out)
    xmit_interrupt();
  /* loop till UART buffer empty */
  while ( (*LSR & TEMT) == 0 );
}

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */
 
void console_init()
{
  /* set clock divisor */
  *LCR = (char)(DLAB);
  *DLL = (char)((int)(CLK_FREQ/BAUD/16.0+0.5) & 0xFF);
  *DLM = (char)(((int)(CLK_FREQ/BAUD/16.0+0.5) & 0xFF00) >> 8);

  /* Line control setup */
  *LCR = (char)(WL_8 | NSB);

  /* Interrupt setup */
  *IER = (char) 0x0b;		/* enable transmit, receive, modem stat int */
  
  /* FIFO setup */
  *FCR = (char)(FIFO_E | 0xc0);

  /* Modem control setup */
  *MCR = (char) RTS;

  /* init tx_stop with CTS */
  _tx_stop = ( (*MDSR & CTS) ? 0 : 1);

  set_vector(_catchUARTint, UART_ISR_LEVEL+24, 0);
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );
 
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);
 
  return RTEMS_SUCCESSFUL;

}


/*  is_character_ready
 *
 *  This routine returns TRUE if a character is available.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_boolean is_character_ready(
  char *ch
)
{
  if ((char *)(((int)rcv.in+1) & ((int)BUFFER_LENGTH-1))== rcv.out)
    return(FALSE);
  else
    return(TRUE);
}

/*
 *  Open entry point
 */
 
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
 
/*
 *  Close entry point
 */
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
 
/*
 * read bytes from the serial port. We only have stdin.
 */
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte();
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      break;
    }
  }
 
  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}
 
/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r');
    }
    outbyte( buffer[ count ] );
  }
 
  rw_args->bytes_moved = maximum;
  return 0;
}
 
/*
 *  IO Control entry point
 */
 
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
