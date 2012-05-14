/**
 *  @file
 *  
 *  This file contains the termios TTY driver for the UART found
 *  on the Synova Mongoose-V.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/mg5uart.h>
#include <libchip/sersupp.h>
#include <libcpu/mongoose-v.h>

#include <bsp/irq.h>
#include <bsp.h>

/*
 *  Indices of registers
 */

/*
 *  Per chip context control
 */

typedef struct _mg5uart_context
{
  int            mate;
} mg5uart_context;

/*
 *  Define MG5UART_STATIC to nothing while debugging so the entry points
 *  will show up in the symbol table.
 */

#define MG5UART_STATIC

/* #define MG5UART_STATIC static */



#define MG5UART_SETREG( _base, _register, _value ) \
        MONGOOSEV_WRITE_REGISTER( _base, _register, _value )

#define MG5UART_GETREG( _base, _register ) \
        MONGOOSEV_READ_REGISTER( _base, _register )


/*
 *  Console Device Driver Support Functions
 */

MG5UART_STATIC int mg5uart_baud_rate(
  int        minor,
  uint32_t   baud,
  uint32_t  *code
);

MG5UART_STATIC void mg5uart_enable_interrupts(
  int minor,
  int mask
);

/*
 *  mg5uart_isr_XXX
 *
 *  This is the single interrupt entry point which parcels interrupts
 *  out to the handlers for specific sources and makes sure that the
 *  shared handler gets the right arguments.
 *
 *  NOTE: Yes .. this is ugly but it provides 5 interrupt source
 *  wrappers which are nearly functionally identical.
 */


extern void mips_default_isr(int vector);

#define __ISR(_TYPE, _OFFSET) \
  MG5UART_STATIC void mg5uart_process_isr_ ## _TYPE ( \
    int  minor \
  ); \
  \
  MG5UART_STATIC rtems_isr mg5uart_isr_ ## _TYPE ( \
    void *arg \
  ) \
  { \
    rtems_vector_number vector = (rtems_vector_number) arg; \
    int   minor; \
    \
    for(minor=0 ; minor<Console_Port_Count ; minor++) { \
      if( Console_Port_Tbl[minor]->deviceType == SERIAL_MG5UART && \
          vector == Console_Port_Tbl[minor]->ulIntVector + _OFFSET ) { \
        mg5uart_process_isr_ ## _TYPE (minor); \
	return; \
      } \
    } \
    mips_default_isr( vector ); \
  }

__ISR(rx_frame_error, MG5UART_IRQ_RX_FRAME_ERROR)
__ISR(rx_overrun_error, MG5UART_IRQ_RX_OVERRUN_ERROR)
__ISR(tx_empty, MG5UART_IRQ_TX_EMPTY)
__ISR(tx_ready, MG5UART_IRQ_TX_READY)
__ISR(rx_ready, MG5UART_IRQ_RX_READY)

/*
 *  mg5uart_set_attributes
 *
 *  This function sets the UART channel to reflect the requested termios
 *  port settings.
 */

MG5UART_STATIC int mg5uart_set_attributes(
  int minor,
  const struct termios *t
)
{
  uint32_t               pMG5UART_port;
  uint32_t               pMG5UART;
  uint32_t               cmd, cmdSave;
  uint32_t               baudcmd;
  uint32_t               shift;
  rtems_interrupt_level  Irql;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  /*
   *  Set the baud rate
   */

  if (mg5uart_baud_rate( minor, t->c_cflag, &baudcmd ) == -1)
    return -1;

  /*
   *  Base settings
   */

  /*
   *  Base settings
   */

  cmd = MONGOOSEV_UART_CMD_RX_ENABLE | MONGOOSEV_UART_CMD_TX_ENABLE;

  /*
   *  Parity
   */

  if (t->c_cflag & PARENB) {
    cmd |= MONGOOSEV_UART_CMD_PARITY_ENABLE;
    if (t->c_cflag & PARODD)
      cmd |= MONGOOSEV_UART_CMD_PARITY_ODD;
    else
      cmd |= MONGOOSEV_UART_CMD_PARITY_EVEN;
  } else {
    cmd |= MONGOOSEV_UART_CMD_PARITY_DISABLE;
  }

  /*
   *  Character Size
   */

  if (t->c_cflag & CSIZE) {
    switch (t->c_cflag & CSIZE) {
      case CS5:
      case CS6:
      case CS7:
        return -1;
        break;
      case CS8:
        /* Mongoose-V only supports CS8 */
        break;

    }
  } /* else default to CS8 */

  /*
   *  Stop Bits
   */

#if 0
  if (t->c_cflag & CSTOPB) {
    /* 2 stop bits not supported by Mongoose-V uart */
    return -1;
  }
#endif

  /*
   *  XXX what about CTS/RTS
   */

  /* XXX */

  /*
   *  Now write the registers
   */

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_CMD_SHIFT;
  else
    shift = MONGOOSEV_UART1_CMD_SHIFT;



  rtems_interrupt_disable(Irql);

  cmdSave = MG5UART_GETREG( pMG5UART, MG5UART_COMMAND_REGISTER );

  MG5UART_SETREG( pMG5UART,
		  MG5UART_COMMAND_REGISTER,
		  (cmdSave & ~(MONGOOSEV_UART_ALL_STATUS_BITS << shift)) | (cmd << shift) );

  MG5UART_SETREG( pMG5UART_port, MG5UART_BAUD_RATE, baudcmd );

  rtems_interrupt_enable(Irql);
  return 0;
}

/*
 *  mg5uart_initialize_context
 *
 *  This function sets the default values of the per port context structure.
 */

MG5UART_STATIC void mg5uart_initialize_context(
  int               minor,
  mg5uart_context  *pmg5uartContext
)
{
  int          port;
  unsigned int pMG5UART;
  unsigned int pMG5UART_port;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  pmg5uartContext->mate = -1;

  for (port=0 ; port<Console_Port_Count ; port++ ) {
    if ( Console_Port_Tbl[port]->ulCtrlPort1 == pMG5UART &&
         Console_Port_Tbl[port]->ulCtrlPort2 != pMG5UART_port ) {
      pmg5uartContext->mate = port;
      break;
    }
  }

}

/*
 *  mg5uart_init
 *
 *  This function initializes the DUART to a quiecsent state.
 */

MG5UART_STATIC void mg5uart_init(int minor)
{
  uint32_t              pMG5UART_port;
  uint32_t              pMG5UART;
  uint32_t  		cmdSave;
  uint32_t  		shift;

  mg5uart_context        *pmg5uartContext;

  pmg5uartContext = (mg5uart_context *) malloc(sizeof(mg5uart_context));

  Console_Port_Data[minor].pDeviceContext = (void *)pmg5uartContext;

  mg5uart_initialize_context( minor, pmg5uartContext );

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
     shift = MONGOOSEV_UART0_CMD_SHIFT;
  else
     shift = MONGOOSEV_UART1_CMD_SHIFT;

  /*
   *  Disable the uart and leave this port disabled.
   */

  cmdSave = MG5UART_GETREG(pMG5UART, MG5UART_COMMAND_REGISTER) & ~(MONGOOSEV_UART_ALL_STATUS_BITS << shift);

  MG5UART_SETREG( pMG5UART, MG5UART_COMMAND_REGISTER, cmdSave );

  /*
   *  Disable interrupts on RX and TX for this port
   */
  mg5uart_enable_interrupts( minor, MG5UART_DISABLE_ALL );
}

/*
 *  mg5uart_open
 *
 *  This function opens a port for communication.
 *
 *  Default state is 9600 baud, 8 bits, No parity, and 1 stop bit.
 */

MG5UART_STATIC int mg5uart_open(
  int      major,
  int      minor,
  void    *arg
)
{
  uint32_t      pMG5UART;
  uint32_t      pMG5UART_port;
  uint32_t  	vector;
  uint32_t      cmd, cmdSave;
  uint32_t      baudcmd;
  uint32_t      shift;

  rtems_interrupt_level  Irql;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;
  vector        = Console_Port_Tbl[minor]->ulIntVector;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_CMD_SHIFT;
  else
    shift = MONGOOSEV_UART1_CMD_SHIFT;


  /* XXX default baud rate could be from configuration table */

  (void) mg5uart_baud_rate( minor, B19200, &baudcmd );

  /*
   *  Set the DUART channel to a default useable state
   *  B19200, 8Nx since there is no stop bit control.
   */

  cmd = MONGOOSEV_UART_CMD_TX_ENABLE | MONGOOSEV_UART_CMD_RX_ENABLE;

  rtems_interrupt_disable(Irql);

  cmdSave = MG5UART_GETREG( pMG5UART, MG5UART_COMMAND_REGISTER );

  MG5UART_SETREG( pMG5UART_port, MG5UART_BAUD_RATE, baudcmd );

  MG5UART_SETREG( pMG5UART,
		  MG5UART_COMMAND_REGISTER,
		  cmd = (cmdSave & ~(MONGOOSEV_UART_ALL_STATUS_BITS << shift)) | (cmd << shift) );

  rtems_interrupt_enable(Irql);

  return RTEMS_SUCCESSFUL;
}

/*
 *  mg5uart_close
 *
 *  This function shuts down the requested port.
 */

MG5UART_STATIC int mg5uart_close(
  int      major,
  int      minor,
  void    *arg
)
{
  uint32_t      pMG5UART;
  uint32_t      pMG5UART_port;
  uint32_t  	cmd, cmdSave;
  uint32_t      shift;
  rtems_interrupt_level  Irql;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  /*
   *  Disable interrupts from this channel and then disable it totally.
   */

  /* XXX interrupts */

  cmd = MONGOOSEV_UART_CMD_TX_DISABLE | MONGOOSEV_UART_CMD_RX_DISABLE;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_CMD_SHIFT;
  else
    shift = MONGOOSEV_UART1_CMD_SHIFT;


  rtems_interrupt_disable(Irql);
  cmdSave = MG5UART_GETREG( pMG5UART, MG5UART_COMMAND_REGISTER );

  MG5UART_SETREG( pMG5UART,
		  MG5UART_COMMAND_REGISTER,
		  (cmdSave & ~(MONGOOSEV_UART_ALL_STATUS_BITS << shift)) | (cmd << shift) );
  rtems_interrupt_enable(Irql);

  return(RTEMS_SUCCESSFUL);
}




/*
 *  mg5uart_write_polled
 *
 *  This routine polls out the requested character.
 */

MG5UART_STATIC void mg5uart_write_polled(
  int   minor,
  char  c
)
{
  uint32_t                pMG5UART;
  uint32_t                pMG5UART_port;
  uint32_t                status;
  int                     shift;
  int                     timeout;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_IRQ_SHIFT;
  else
    shift = MONGOOSEV_UART1_IRQ_SHIFT;

  /*
   * wait for transmitter holding register to be empty
   */
  timeout = 2000;

  while( --timeout )
  {
    status = MG5UART_GETREG(pMG5UART, MG5UART_STATUS_REGISTER) >> shift;

    /*
    if ( (status & (MONGOOSEV_UART_TX_READY | MONGOOSEV_UART_TX_EMPTY)) ==
            (MONGOOSEV_UART_TX_READY | MONGOOSEV_UART_TX_EMPTY) )
      break;
    */

    if( (status & (MONGOOSEV_UART_TX_READY | MONGOOSEV_UART_TX_EMPTY)) )
       break;

    /*
     * Yield while we wait
     */

#if 0
     if(_System_state_Is_up(_System_state_Get()))
     {
       rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
     }
#endif
  }

  /*
   * transmit character
   */

  MG5UART_SETREG(pMG5UART_port, MG5UART_TX_BUFFER, c);
}

MG5UART_STATIC void mg5uart_process_isr_rx_error(
   int  minor,
   uint32_t   mask
)
{
  uint32_t                pMG5UART;
  int                     shift;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_IRQ_SHIFT;
  else
    shift = MONGOOSEV_UART1_IRQ_SHIFT;

  /* now clear the error */

  MG5UART_SETREG(
     pMG5UART,
     MG5UART_STATUS_REGISTER,
     mask << shift );
}


MG5UART_STATIC void mg5uart_process_isr_rx_frame_error(
  int  minor
)
{
   mg5uart_process_isr_rx_error( minor, MONGOOSEV_UART_RX_FRAME_ERROR );
}

MG5UART_STATIC void mg5uart_process_isr_rx_overrun_error(
  int  minor
)
{
   mg5uart_process_isr_rx_error( minor, MONGOOSEV_UART_RX_OVERRUN_ERROR );
}








MG5UART_STATIC void mg5uart_process_tx_isr(
   int        minor,
   uint32_t   source
)
{
   uint32_t        pMG5UART;
   int             shift;

   pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;

   mg5uart_enable_interrupts(minor, MG5UART_ENABLE_ALL_EXCEPT_TX);

   if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
      shift = MONGOOSEV_UART0_IRQ_SHIFT;
   else
      shift = MONGOOSEV_UART1_IRQ_SHIFT;

   MG5UART_SETREG(
      pMG5UART,
      MG5UART_STATUS_REGISTER,
      source << shift );

   if( rtems_termios_dequeue_characters( Console_Port_Data[minor].termios_data, 1) )
   {
      mg5uart_enable_interrupts(minor, MG5UART_ENABLE_ALL);
      return;
   }

   /*
    *  There are no more characters to transmit.  The tx interrupts are be cleared
    *  by writing data to the uart, so just disable the tx interrupt sources.
    */

   Console_Port_Data[minor].bActive = FALSE;

   /* mg5uart_enable_interrupts(minor, MG5UART_ENABLE_ALL_EXCEPT_TX); */
}



MG5UART_STATIC void mg5uart_process_isr_tx_empty(
  int  minor
)
{
   /* mg5uart_process_tx_isr( minor, MONGOOSEV_UART_TX_EMPTY ); */
}

MG5UART_STATIC void mg5uart_process_isr_tx_ready(
  int  minor
)
{
   mg5uart_process_tx_isr( minor, MONGOOSEV_UART_TX_READY );
}





MG5UART_STATIC void mg5uart_process_isr_rx_ready(
  int  minor
)
{
  uint32_t   pMG5UART_port;
  char       c;

  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  /* reading the RX buffer automatically resets the interrupt flag */

  c = (char) MG5UART_GETREG(pMG5UART_port, MG5UART_RX_BUFFER);

  rtems_termios_enqueue_raw_characters(
     Console_Port_Data[minor].termios_data,
     &c, 1 );
}

static rtems_irq_connect_data mg5uart_rx_frame_error_cd  = {  \
  0,                             /* filled in at initialization */
  mg5uart_isr_rx_frame_error,   /* filled in at initialization */
  NULL,   /* (void *) minor */
  NULL,
  NULL,
  NULL
};

static rtems_irq_connect_data mg5uart_rx_overrun_error_cd  = {  \
  0,                             /* filled in at initialization */
  mg5uart_isr_rx_overrun_error,   /* filled in at initialization */
  NULL,   /* (void *) minor */
  NULL,
  NULL,
  NULL
};

static rtems_irq_connect_data mg5uart_tx_empty_cd  = {  \
  0,                             /* filled in at initialization */
  mg5uart_isr_tx_empty,   /* filled in at initialization */
  NULL,   /* (void *) minor */
  NULL,
  NULL,
  NULL
};

static rtems_irq_connect_data mg5uart_tx_ready_cd  = {  \
  0,                             /* filled in at initialization */
  mg5uart_isr_tx_ready,   /* filled in at initialization */
  NULL,   /* (void *) minor */
  NULL,
  NULL,
  NULL
};

static rtems_irq_connect_data mg5uart_rx_ready_cd  = {  \
  0,                             /* filled in at initialization */
  mg5uart_isr_rx_ready,   /* filled in at initialization */
  NULL,   /* (void *) minor */
  NULL,
  NULL,
  NULL
};


/*
 *  mg5uart_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 */

MG5UART_STATIC void mg5uart_initialize_interrupts(int minor)
{
  unsigned long v;
  mg5uart_init(minor);

  Console_Port_Data[minor].bActive = FALSE;
  v = Console_Port_Tbl[minor]->ulIntVector;

  mg5uart_rx_frame_error_cd.name    =  v + MG5UART_IRQ_RX_FRAME_ERROR;
  mg5uart_rx_overrun_error_cd.name  =  v + MG5UART_IRQ_RX_OVERRUN_ERROR; 
  mg5uart_tx_empty_cd.name          =  v + MG5UART_IRQ_TX_EMPTY;  
  mg5uart_tx_ready_cd.name          =  v + MG5UART_IRQ_TX_READY; 
  mg5uart_rx_ready_cd.name          =  v + MG5UART_IRQ_RX_READY; 

  mg5uart_rx_frame_error_cd.handle    =  (void *)mg5uart_rx_frame_error_cd.name;
  mg5uart_rx_overrun_error_cd.handle  =  (void *)mg5uart_rx_overrun_error_cd.name; 
  mg5uart_tx_empty_cd.handle          =  (void *)mg5uart_tx_empty_cd.name;  
  mg5uart_tx_ready_cd.handle          =  (void *)mg5uart_tx_ready_cd.name; 
  mg5uart_rx_ready_cd.handle          =  (void *)mg5uart_rx_ready_cd.name; 

 
  BSP_install_rtems_irq_handler( &mg5uart_rx_frame_error_cd );
  BSP_install_rtems_irq_handler( &mg5uart_rx_overrun_error_cd );
  BSP_install_rtems_irq_handler( &mg5uart_tx_empty_cd );
  BSP_install_rtems_irq_handler( &mg5uart_tx_ready_cd );
  BSP_install_rtems_irq_handler( &mg5uart_rx_ready_cd );

  mg5uart_enable_interrupts(minor, MG5UART_ENABLE_ALL_EXCEPT_TX);
}

/*
 *  mg5uart_write_support_int
 *
 *  Console Termios output entry point when using interrupt driven output.
 */

MG5UART_STATIC int mg5uart_write_support_int(
  int         minor,
  const char *buf,
  size_t      len
)
{
  uint32_t        Irql;
  uint32_t        pMG5UART_port;

  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  /*
   *  We are using interrupt driven output and termios only sends us
   *  one character at a time.
   */

  if ( !len )
     return 0;

  /*
   *  Put the character out and enable interrupts if necessary.
   */

  rtems_interrupt_disable(Irql);

  MG5UART_SETREG(pMG5UART_port, MG5UART_TX_BUFFER, *buf);

  if( Console_Port_Data[minor].bActive == FALSE )
  {
     Console_Port_Data[minor].bActive = TRUE;
     mg5uart_enable_interrupts(minor, MG5UART_ENABLE_ALL);
  }

  rtems_interrupt_enable(Irql);
  return 1;
}




/*
 *  mg5uart_write_support_polled
 *
 *  Console Termios output entry point when using polled output.
 *
 */

MG5UART_STATIC ssize_t mg5uart_write_support_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
  int nwrite = 0;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len)
  {
    mg5uart_write_polled(minor, *buf++);
    nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/*
 *  mg5uart_inbyte_nonblocking_polled
 *
 *  Console Termios polling input entry point.
 */

MG5UART_STATIC int mg5uart_inbyte_nonblocking_polled(
  int minor
)
{
  uint32_t                pMG5UART;
  uint32_t                pMG5UART_port;
  uint32_t                status;
  uint32_t                tmp,shift;

  pMG5UART      = Console_Port_Tbl[minor]->ulCtrlPort1;
  pMG5UART_port = Console_Port_Tbl[minor]->ulCtrlPort2;

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_IRQ_SHIFT;
  else
    shift = MONGOOSEV_UART1_IRQ_SHIFT;

  /* reset overrrun or framing errors */
  status = MG5UART_GETREG(pMG5UART, MG5UART_STATUS_REGISTER) >> shift;

  if( (tmp = (status & 0x3)) )
  {
     MG5UART_SETREG(pMG5UART, MG5UART_STATUS_REGISTER, (tmp << shift) );
     status = MG5UART_GETREG(pMG5UART, MG5UART_STATUS_REGISTER) >> shift;
  }

  if ( status & MONGOOSEV_UART_RX_READY )
  {
     return (int) MG5UART_GETREG(pMG5UART_port, MG5UART_RX_BUFFER);
  }
  else
  {
     return -1;
  }
}

/*
 *  mg5uart_baud_rate
 */

MG5UART_STATIC int mg5uart_baud_rate(
  int           minor,
  uint32_t      baud,
  uint32_t     *code
)
{
  uint32_t   clock;
  uint32_t   tmp_code;
  uint32_t   baud_requested;

  baud_requested = baud & CBAUD;
  if (!baud_requested)
    baud_requested = B9600;              /* default to 9600 baud */

  baud_requested = rtems_termios_baud_to_number( baud_requested );

  clock = (uint32_t) Console_Port_Tbl[minor]->ulClock;
  if (!clock)
    rtems_fatal_error_occurred(RTEMS_INVALID_NUMBER);

  /*
   *  Formula is Code = round(ClockFrequency / Baud - 1).
   *
   *  Since this is integer math, we will divide by twice the baud and
   *  check the remaining odd bit.
   */

  tmp_code = (clock / baud_requested) - 1;

  /*
   *  From section 12.7, "Keep C>100 for best receiver operation."
   *  That is 100 cycles which is not a lot of instructions.  It is
   *  reasonable to think that the Mongoose-V could not keep
   *  up with C < 100.
   */

  if ( tmp_code < 100 )
    return RTEMS_INVALID_NUMBER;

  /*
   *  upper word is receiver baud and lower word is transmitter baud
   */

  *code = (tmp_code << 16) | tmp_code;

  return 0;
}




/*
 *  mg5uart_enable_interrupts
 *
 *  This function enables specific interrupt sources on the DUART.
 */

MG5UART_STATIC void mg5uart_enable_interrupts(
  int minor,
  int mask
)
{
  uint32_t              pMG5UART;
  uint32_t  		maskSave;
  uint32_t              shift;
  rtems_interrupt_level  Irql;

  pMG5UART = Console_Port_Tbl[minor]->ulCtrlPort1;

  /*
   *  Enable interrupts on RX and TX -- not break
   */

  if ( Console_Port_Tbl[minor]->ulDataPort == MG5UART_UART0 )
    shift = MONGOOSEV_UART0_IRQ_SHIFT;
  else
    shift = MONGOOSEV_UART1_IRQ_SHIFT;


  rtems_interrupt_disable(Irql);

  maskSave = MG5UART_GETREG( pMG5UART, MG5UART_INTERRUPT_MASK_REGISTER );

  MG5UART_SETREG(
     pMG5UART,
     MG5UART_INTERRUPT_MASK_REGISTER,
     (maskSave & ~(MONGOOSEV_UART_ALL_STATUS_BITS << shift)) | (mask << shift) );

  rtems_interrupt_enable(Irql);
}



/*
 * Flow control is only supported when using interrupts
 */

console_fns mg5uart_fns =
{
  libchip_serial_default_probe,   /* deviceProbe */
  mg5uart_open,                   /* deviceFirstOpen */
  NULL,                           /* deviceLastClose */
  NULL,                           /* deviceRead */
  mg5uart_write_support_int,      /* deviceWrite */
  mg5uart_initialize_interrupts,  /* deviceInitialize */
  mg5uart_write_polled,           /* deviceWritePolled */
  mg5uart_set_attributes,         /* deviceSetAttributes */
  TRUE                            /* deviceOutputUsesInterrupts */
};

console_fns mg5uart_fns_polled =
{
  libchip_serial_default_probe,        /* deviceProbe */
  mg5uart_open,                        /* deviceFirstOpen */
  mg5uart_close,                       /* deviceLastClose */
  mg5uart_inbyte_nonblocking_polled,   /* deviceRead */
  mg5uart_write_support_polled,        /* deviceWrite */
  mg5uart_init,                        /* deviceInitialize */
  mg5uart_write_polled,                /* deviceWritePolled */
  mg5uart_set_attributes,              /* deviceSetAttributes */
  FALSE,                               /* deviceOutputUsesInterrupts */
};
