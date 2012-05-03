/*
 *  68340/68349 console serial I/O.
 *
 *  Author:
 *  Geoffroy Montel
 *  France Telecom - CNET/DSM/TAM/CAT
 *  4, rue du Clos Courtel
 *  35512 CESSON-SEVIGNE
 *  FRANCE
 *
 *  e-mail: g_montel@yahoo.com
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <m68340.h>
#include <m340uart.h>
#include <m340timer.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <rtems/termiostypes.h>

#define CONSOLE_VECTOR 121
#define CONSOLE_IRQ_LEVEL 3
#define CONSOLE_INTERRUPT_ARBITRATION 2

static void *ttypA;		/* to remember which tty has been opened on channel A
				   used when interrupts are enabled */

static void *ttypB;		/* to remember which tty has been opened on channel B
				   used when interrupts are enabled */

unsigned char DUIER_mirror = 0 ;  /* reflects the state of IER register, which is Write Only */
unsigned char Error_Status_A = 0; /* error status on Channel A */
unsigned char Error_Status_B = 0; /* error status on Channel A */

/*
 * Device-specific routines
 */

#define USE_INTERRUPTS_A (m340_uart_config[UART_CHANNEL_A].mode==UART_INTERRUPTS)
#define USE_INTERRUPTS_B (m340_uart_config[UART_CHANNEL_B].mode==UART_INTERRUPTS)
#define CHANNEL_ENABLED_A m340_uart_config[UART_CHANNEL_A].enable
#define CHANNEL_ENABLED_B m340_uart_config[UART_CHANNEL_B].enable

#define set_DUIER(a) DUIER_mirror |= (a); DUIER = DUIER_mirror
#define unset_DUIER(a) DUIER_mirror &= ~(a); DUIER = DUIER_mirror

#define Enable_Interrupts_Tx_A if (USE_INTERRUPTS_A) set_DUIER(m340_TxRDYA)
#define Disable_Interrupts_Tx_A if (USE_INTERRUPTS_A) unset_DUIER(m340_TxRDYA)

#define Enable_Interrupts_Tx_B if (USE_INTERRUPTS_B) set_DUIER(m340_TxRDYB)
#define Disable_Interrupts_Tx_B if (USE_INTERRUPTS_B) unset_DUIER(m340_TxRDYB)

/******************************************************
  Name: InterruptHandler
  Input parameters: vector number
  Output parameters: -
  Description: UART ISR Routine, called by _RTEMS_ISR
 *****************************************************/
rtems_isr
InterruptHandler (rtems_vector_number v)
{
 char ch;

 /*****************************************************************************
 **				CHANNEL A				     **
 *****************************************************************************/

    /* check Received Break*/
    if (DUSRA & m340_RB) {
       Error_Status_A |= m340_RB;
       /* reset error status */
       DUCRA = m340_Reset_Error_Status;
    }

    /* buffer received ? */
    if (DUSRA & m340_Rx_RDY) {
       do {
	   /* error encountered? */
	   if (DUSRA & (m340_OE | m340_PE | m340_FE | m340_RB)) {
	      Error_Status_A |= DUSRA;
	      /* reset error status */
    	      DUCRA = m340_Reset_Error_Status;
	      /* all the characters in the queue may not be good */
	      while (DUSRA & m340_Rx_RDY)
		    /* push them in a trash */
		    ch = DURBA;
	   }
	   else {
		 /* this is necessary, otherwise it blocks when FIFO is full */
		 ch = DURBA;
		 rtems_termios_enqueue_raw_characters(ttypA,&ch,1);
	   }
       } while (DUSRA & m340_Rx_RDY);
       Restart_Fifo_Full_A_Timer();	/* only if necessary (pointer to a fake function if
					   not in FIFO full mode) */
    }

    else /* if no character has been received */
       Restart_Check_A_Timer();		/* same remark */

    /* ready to accept a character ? */
    if (DUISR & DUIER_mirror & m340_TxRDYA) {
	Disable_Interrupts_Tx_A;
	/* one character has been transmitted */
	rtems_termios_dequeue_characters(ttypA,1);
    }

 /*****************************************************************************
 **				CHANNEL B				     **
 *****************************************************************************/

    /* check Received Break*/
    if (DUSRB & m340_RB) {
       Error_Status_B |= m340_RB;
       /* reset error status */
       DUCRB = m340_Reset_Error_Status;
    }

    /* buffer received ? */
    if (DUSRB & m340_Rx_RDY) {
       do {
	   if (DUSRB & (m340_OE | m340_PE | m340_FE | m340_RB)) {
	      Error_Status_B |= DUSRB;
	      /* reset error status */
    	      DUCRB = m340_Reset_Error_Status;
	      /* all the characters in the queue may not be good */
	      while (DUSRB & m340_Rx_RDY)
		    /* push them in a trash */
		    ch = DURBB;
	   }
	   else {
		 ch = DURBB;
		 rtems_termios_enqueue_raw_characters(ttypB,&ch,1);
	   }

       } while (DUSRB & m340_Rx_RDY);
       Restart_Fifo_Full_B_Timer();
    }
    else /* if no character has been received */
       Restart_Check_B_Timer();

    /* ready to accept a character ? */
    if (DUISR & DUIER_mirror & m340_TxRDYB) {
	Disable_Interrupts_Tx_B;
	/* one character has been transmitted */
	rtems_termios_dequeue_characters(ttypB,1);
    }
}

/******************************************************
  Name: InterruptWrite
  Input parameters: minor = channel, pointer to buffer,
		    and length of buffer to transmit
  Output parameters: -
  Description: write the first character of buf only
	       may be called by either console_write
	       or rtems_termios_enqueue_raw_characters
 *****************************************************/
static ssize_t
InterruptWrite (int minor, const char *buf, size_t len)
{
 if (minor==UART_CHANNEL_A) {
    if (len>0) DUTBA=*buf;
    Enable_Interrupts_Tx_A;
 }
 else if (minor==UART_CHANNEL_B) {
    if (len>0) DUTBB=*buf;
    Enable_Interrupts_Tx_B;
 }
 return 0;
}

/******************************************************
  Name: dbug_out_char
  Input parameters: channel, character to emit
  Output parameters: -
  Description: wait for the UART to be ready to emit
	       a character and send it
 *****************************************************/
void dbug_out_char( int minor, int ch )
{
 if (minor==UART_CHANNEL_A) {
    while (!(DUSRA & m340_Tx_RDY)) continue;
    DUTBA=ch;
 }
 else if (minor==UART_CHANNEL_B) {
    while (!(DUSRB & m340_Tx_RDY)) continue;
    DUTBB=ch;
 }
}

/******************************************************
  Name: dbug_in_char
  Input parameters: -
  Output parameters: received character
  Description: return the character in the UART
 *****************************************************/
int dbug_in_char( int minor )
{
 if (minor==UART_CHANNEL_A) {
    return DURBA;
 }
 else if (minor==UART_CHANNEL_B) {
    return DURBB;
 }
 return 0;
}

/******************************************************
  Name: dbug_char_present
  Input parameters: channel #
  Output parameters: TRUE or FALSE
  Description: return whether there's a character
	       in the receive buffer
 *****************************************************/
int dbug_char_present( int minor )
{
 if (minor==UART_CHANNEL_A) {
    return (DUSRA & m340_Rx_RDY);
 }
 else if (minor==UART_CHANNEL_B) {
    return (DUSRB & m340_Rx_RDY);
 }
 return 0;
}

/******************************************************
  Name: dbugInitialise
  Input parameters: -
  Output parameters: -
  Description: Init the UART
 *****************************************************/
static void
dbugInitialise (void)
{
     t_baud_speed_table uart_config;		/* configuration of UARTS */

     /*
      * Reset Receiver
      */
     DUCRA = m340_Reset_Receiver;
     DUCRB = m340_Reset_Receiver;

     /*
      * Reset Transmitter
      */
     DUCRA = m340_Reset_Transmitter;
     DUCRB = m340_Reset_Transmitter;

     /*
      * Enable serial module for normal operation, ignore FREEZE, select the crystal clock,
      * supervisor/user serial registers unrestricted
      * interrupt arbitration at priority CONSOLE_INTERRUPT_ARBITRATION
      * WARNING : 8 bits access only on this UART!
      */
     DUMCRH = 0x00;
     DUMCRL = CONSOLE_INTERRUPT_ARBITRATION;

     /*
      * Interrupt level register
      */
     DUILR = CONSOLE_IRQ_LEVEL;

     /* sets the IVR */
     DUIVR = CONSOLE_VECTOR;

     /* search for a correct m340 uart configuration */
     uart_config = Find_Right_m340_UART_Config(m340_uart_config[UART_CHANNEL_A].rx_baudrate,
					       m340_uart_config[UART_CHANNEL_A].tx_baudrate,
					       CHANNEL_ENABLED_A,
					       m340_uart_config[UART_CHANNEL_B].rx_baudrate,
					       m340_uart_config[UART_CHANNEL_B].tx_baudrate,
					       CHANNEL_ENABLED_B);

     /*****************************************************************************
     **				CHANNEL A				     	 **
     *****************************************************************************/
     if (CHANNEL_ENABLED_A) {

	if (USE_INTERRUPTS_A) {
	   rtems_isr_entry old_handler;
	   rtems_status_code sc;

	   sc = rtems_interrupt_catch (InterruptHandler,
	  			       CONSOLE_VECTOR,
				       &old_handler);

	   /* uncomment this if you want to pass control to your own ISR handler
	      it may be usefull to do so to check for performances with an oscilloscope */
	   /*
	   {
	    proc_ptr ignored;
	    _CPU_ISR_install_raw_handler( CONSOLE_VECTOR, _Debug_ISR_Handler_Console, &ignored );
	   }
	   */

	   /*
	    * Interrupt Enable Register
	    * Enable Interrupts on Channel A Receiver Ready
	    */
	   set_DUIER(m340_RxRDYA);
	}
	else {
		/*
		 * Disable Interrupts on channel A
 	 	 */
		unset_DUIER(m340_RxRDYA&m340_TxRDYA);
	}

	/*
	 * Change set of baud speeds
	 * disable input control
	 */
	/* no good uart configuration ? */
	if (uart_config.nb<1) rtems_fatal_error_occurred (-1);

	if (uart_config.baud_speed_table[UART_CHANNEL_A].set==1)
	   DUACR = m340_BRG_Set1;
	else
	   DUACR = m340_BRG_Set2;

	/*
	 * make OPCR an auxiliary function serving the communication channels
	 */
	DUOPCR = m340_OPCR_Aux;

	/* poll the XTAL_RDY bit until it is cleared to ensure that an unstable crystal
	   input is not applied to the baud rate generator */
	while (DUISR & m340_XTAL_RDY) continue;

	/*
	 * Serial Channel Baud Speed
	 */
	DUCSRA = (uart_config.baud_speed_table[UART_CHANNEL_A].rcs << 4)
	       | (uart_config.baud_speed_table[UART_CHANNEL_A].tcs);

	/*
	 * Serial Channel Configuration
	 */
	DUMR1A = m340_uart_config[UART_CHANNEL_A].parity_mode
	       | m340_uart_config[UART_CHANNEL_A].bits_per_char
	       | m340_RxRTS;

	if (m340_uart_config[UART_CHANNEL_A].rx_mode==UART_FIFO_FULL) DUMR1A |= m340_R_F | m340_ERR;

	/*
	 * Serial Channel Configuration 2
	 */
	DUMR2A |= m340_normal;

	/*
	 * Enable Channel A: transmitter and receiver
	 */
	DUCRA = m340_Transmitter_Enable | m340_Receiver_Enable;
     } /* channel A enabled */

     /*****************************************************************************
     **				CHANNEL B				     	 **
     *****************************************************************************/
     if (CHANNEL_ENABLED_B) {

	/* we mustn't set the console vector twice! */
	if ((USE_INTERRUPTS_B && !(CHANNEL_ENABLED_A))
	   || (USE_INTERRUPTS_B && CHANNEL_ENABLED_A && !USE_INTERRUPTS_A)) {
	   rtems_isr_entry old_handler;
	   rtems_status_code sc;

	   sc = rtems_interrupt_catch (InterruptHandler,
	  			       CONSOLE_VECTOR,
				       &old_handler);

	   /* uncomment this if you want to pass control to your own ISR handler
	      it may be usefull to do so to check for performances with an oscilloscope */
	   /*
	   {
	    proc_ptr ignored;
	    _CPU_ISR_install_raw_handler( CONSOLE_VECTOR, _Debug_ISR_Handler_Console, &ignored );
	   }
	   */

	   /*
	    * Interrupt Enable Register
	    * Enable Interrupts on Channel A Receiver Ready
	    */
	   set_DUIER(m340_RxRDYB);
	}
	else {
		/*
		 * Disable Interrupts on channel B
 	 	 */
		unset_DUIER(m340_RxRDYB&m340_TxRDYB);
	}

	/*
	 * Change set of baud speeds
	 * disable input control
	 */

	/* no good uart configuration ? */
	if (uart_config.nb<2) rtems_fatal_error_occurred (-1);

	/* don't set DUACR twice! */
	if (!CHANNEL_ENABLED_A) {
	   if (uart_config.baud_speed_table[UART_CHANNEL_B].set==1)
              DUACR = m340_BRG_Set1;
	   else
              DUACR = m340_BRG_Set2;
        }

	/*
	 * make OPCR an auxiliary function serving the communication channels
	 */
	if (!CHANNEL_ENABLED_A) DUOPCR = m340_OPCR_Aux;

	/* poll the XTAL_RDY bit until it is cleared to ensure that an unstable crystal
	   input is not applied to the baud rate generator */
	while (DUISR & m340_XTAL_RDY) continue;

	/*
	 * Serial Channel Baud Speed
	 */
	DUCSRB = (uart_config.baud_speed_table[UART_CHANNEL_B].rcs << 4)
	       | (uart_config.baud_speed_table[UART_CHANNEL_B].tcs);

	/*
	 * Serial Channel Configuration
	 */
	DUMR1B = m340_uart_config[UART_CHANNEL_B].parity_mode
	       | m340_uart_config[UART_CHANNEL_B].bits_per_char
	       | m340_RxRTS;

	if (m340_uart_config[UART_CHANNEL_B].rx_mode==UART_FIFO_FULL) DUMR1B |= m340_R_F | m340_ERR;

	/*
	 * Serial Channel Configuration 2
	 */
	DUMR2B |= m340_normal;

	/*
	 * Enable Channel A: transmitter and receiver
	 */
	DUCRB = m340_Transmitter_Enable | m340_Receiver_Enable;
     } /* channel B enabled */
}

/******************************************************
  Name: SetAttributes
  Input parameters: termios structure, channel
  Output parameters: -
  Description: return whether there's a character
	       in the receive buffer
  TO DO: add the channel # to check for!!
 *****************************************************/
static int
SetAttributes (int minor, const struct termios *t)
{
 rtems_interrupt_level level;
 float ispeed, ospeed;
 int isp, osp;

 /* output speed */
 if (t->c_cflag & CBAUDEX)
    osp = (t->c_cflag & CBAUD) + CBAUD + 1;
 else
    osp = t->c_cflag & CBAUD;

 /* input speed */
 isp = (t->c_cflag / (CIBAUD / CBAUD)) &  CBAUD;

 /* convert it */
 ispeed = rtems_termios_baud_to_number(isp);
 ospeed = rtems_termios_baud_to_number(osp);

 if (ispeed || ospeed) {
       /* update config table */
       m340_uart_config[UART_CHANNEL_A].rx_baudrate = ((minor==UART_CHANNEL_A)&&(ispeed!=0)) ? ispeed  : m340_uart_config[UART_CHANNEL_A].rx_baudrate;
       m340_uart_config[UART_CHANNEL_A].tx_baudrate = ((minor==UART_CHANNEL_A)&&(ospeed!=0)) ? ospeed  : m340_uart_config[UART_CHANNEL_A].tx_baudrate;
       m340_uart_config[UART_CHANNEL_B].rx_baudrate = ((minor==UART_CHANNEL_B)&&(ispeed!=0)) ? ispeed  : m340_uart_config[UART_CHANNEL_B].rx_baudrate;
       m340_uart_config[UART_CHANNEL_B].tx_baudrate = ((minor==UART_CHANNEL_B)&&(ospeed!=0)) ? ospeed  : m340_uart_config[UART_CHANNEL_B].tx_baudrate;
 }

 /* change parity */
 if (t->c_cflag & PARENB) {
    if (t->c_cflag & PARODD) m340_uart_config[minor].parity_mode = m340_Odd_Parity;
    else m340_uart_config[minor].parity_mode = m340_Even_Parity;
 }

 /* change bits per character */
 if (t->c_cflag & CSIZE) {
    switch (t->c_cflag & CSIZE) {
	default:	break;
	case CS5:	m340_uart_config[minor].bits_per_char = m340_5bpc;	break;
	case CS6:	m340_uart_config[minor].bits_per_char = m340_6bpc;	break;
	case CS7:	m340_uart_config[minor].bits_per_char = m340_7bpc;	break;
	case CS8:	m340_uart_config[minor].bits_per_char = m340_8bpc;	break;
    }
 }

 /* if serial module configuration has been changed */
 if (t->c_cflag & (CBAUD | CIBAUD | CSIZE | PARENB)) {
    rtems_interrupt_disable(level);
    /* reinit the UART */
    dbugInitialise();
    rtems_interrupt_enable (level);
 }

 return 0;
}

/******************************************************
  Name: console_initialize
  Input parameters: MAJOR # of console_driver,
		    minor is always 0,
		    args are always NULL
  Output parameters: -
  Description: Reserve resources consumed by this driver
  TODO: We should pass m340_uart_config table in arg
 *****************************************************/
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
	rtems_status_code status;
	int i;

	/*
	 * Set up TERMIOS
	 */
	rtems_termios_initialize ();

	/*
	 * Do device-specific initialization
	 */
        Init_UART_Table();
	dbugInitialise ();
	Fifo_Full_benchmark_timer_initialize();

	/*
	 * Register the devices
	 */
	for (i=0; i<UART_NUMBER_OF_CHANNELS; i++) {
	    if (m340_uart_config[i].enable) {
		status = rtems_io_register_name (m340_uart_config[i].name, major, i);
		if (status != RTEMS_SUCCESSFUL)
			rtems_fatal_error_occurred (status);
	    }
	}

	return RTEMS_SUCCESSFUL;
}

/******************************************************
  Name: console_open
  Input parameters: channel #, arg
  Output parameters: -
  Description: open the device
 *****************************************************/
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
 rtems_status_code sc = 0;

 static const rtems_termios_callbacks intrCallbacks = {
		NULL,			/* firstOpen */
		NULL,			/* lastClose */
		NULL,			/* pollRead */
		InterruptWrite,		/* write */
		SetAttributes,		/* setAttributes */
		NULL,			/* stopRemoteTx */
		NULL,			/* startRemoteTx */
		1			/* outputUsesInterrupts */
 };

 static const rtems_termios_callbacks pollCallbacks = {
		NULL,			/* firstOpen */
		NULL,			/* lastClose */
		dbugRead,		/* pollRead */
		dbugWrite,		/* write */
		SetAttributes,		/* setAttributes */
		NULL,			/* stopRemoteTx */
		NULL,			/* startRemoteTx */
		0			/* outputUsesInterrupts */
 };

 if (minor==UART_CHANNEL_A) {
	if (USE_INTERRUPTS_A) {
		rtems_libio_open_close_args_t *args = arg;

		sc |= rtems_termios_open (major, minor, arg, &intrCallbacks);
		ttypA = args->iop->data1;
	}
	else {
		sc |= rtems_termios_open (major, minor, arg, &pollCallbacks);
	}
 }

 else if (minor==UART_CHANNEL_B) {
	if (USE_INTERRUPTS_B) {
		rtems_libio_open_close_args_t *args = arg;

		sc |= rtems_termios_open (major, minor, arg, &intrCallbacks);
		ttypB = args->iop->data1;
	}
	else {
		sc |= rtems_termios_open (major, minor, arg, &pollCallbacks);
	}
 }

 else return RTEMS_INVALID_NUMBER;

 return sc;
}

/******************************************************
  Name: console_close
  Input parameters: channel #, termios args
  Output parameters: -
  Description: close the device
 *****************************************************/
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_close (arg);
}

/******************************************************
  Name: console_read
  Input parameters: channel #, termios args
  Output parameters: -
  Description: read the device
 *****************************************************/
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_read (arg);
}

/******************************************************
  Name: console_write
  Input parameters: channel #, termios args
  Output parameters: -
  Description: write to the device
 *****************************************************/
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_write (arg);
}

/******************************************************
  Name: console_control
  Input parameters: channel #, termios args
  Output parameters: -
  Description: Handle ioctl request
 *****************************************************/
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
 	rtems_libio_ioctl_args_t *args = arg;

 	if (args->command == RTEMS_IO_SET_ATTRIBUTES)
 		SetAttributes (minor, (struct termios *)args->buffer);

	return rtems_termios_ioctl (arg);
}
