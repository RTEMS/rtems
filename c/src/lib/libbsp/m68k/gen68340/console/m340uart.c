/*
 *  M68340/349 uart management tools
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
#include <stdarg.h>
#include <string.h>

/* this table shows compatible speed configurations for the MC68340:
   the first row shows baud rates for baud speed set 1
   the second row shows baud rates for baud speed set 2
   look at Motorola's MC68340 Integrated Processor User's Manual
   page 7-30 for more infos */

float m340_Baud_Rates_Table[16][2] = {\
      { 50,    75	},   \
      { 110,   110	},   \
      { 134.5, 134.5	},   \
      { 200,   150	},   \
      { 300,   300	},   \
      { 600,   600	},   \
      { 1200,  1200	},   \
      { 1050,  2000	},   \
      { 2400,  2400	},   \
      { 4800,  4800	},   \
      { 7200,  1800	},   \
      { 9600,  9600	},   \
      { 38400, 19200	},   \
      { 76800, 38400	},   \
      { SCLK/16, SCLK/16},   \
      { SCLK,  SCLK	},   \
};

/* config on both 340 channels */
uart_channel_config m340_uart_config[UART_NUMBER_OF_CHANNELS];

/*
 * Init UART table
 */

#define NOT_IMPLEMENTED_YET 0

/******************************************************
  Name: Init_UART_Table
  Input parameters: -
  Output parameters: -
  Description: Init the m340_uart_config
	       THIS SHOULD NOT BE HERE!
	       Its aim was to let the user configure
	       UARTs for each application.
	       As we can't pass args to the console
	       driver initialisation routine at the
	       moment, this was not done.
  ATTENTION: TERMIOS init presupposes that the channel
	     baud rates is 9600/9600.
	     -> risks when using IOCTL
 *****************************************************/
void Init_UART_Table(void)
{
 m340_uart_config[UART_CHANNEL_A].enable = TRUE;
 strcpy(m340_uart_config[UART_CHANNEL_A].name, UART_CONSOLE_NAME);
 m340_uart_config[UART_CHANNEL_A].parity_mode = m340_No_Parity;
 m340_uart_config[UART_CHANNEL_A].bits_per_char = m340_8bpc;
 m340_uart_config[UART_CHANNEL_A].rx_baudrate = 9600;
 m340_uart_config[UART_CHANNEL_A].tx_baudrate = 9600;
 m340_uart_config[UART_CHANNEL_A].rx_mode = UART_CRR;
 m340_uart_config[UART_CHANNEL_A].mode = UART_POLLING;

 m340_uart_config[UART_CHANNEL_A].termios.enable = TRUE;
 m340_uart_config[UART_CHANNEL_A].termios.rx_buffer_size = NOT_IMPLEMENTED_YET;
 m340_uart_config[UART_CHANNEL_A].termios.tx_buffer_size = NOT_IMPLEMENTED_YET;

 m340_uart_config[UART_CHANNEL_B].enable = FALSE;
 strcpy(m340_uart_config[UART_CHANNEL_B].name, UART_RAW_IO_NAME);
 m340_uart_config[UART_CHANNEL_B].parity_mode = m340_No_Parity;
 m340_uart_config[UART_CHANNEL_B].bits_per_char = m340_8bpc;
 m340_uart_config[UART_CHANNEL_B].rx_baudrate = 38400;
 m340_uart_config[UART_CHANNEL_B].tx_baudrate = 38400;
 m340_uart_config[UART_CHANNEL_B].rx_mode = UART_CRR;
 m340_uart_config[UART_CHANNEL_B].mode = UART_INTERRUPTS;

 m340_uart_config[UART_CHANNEL_B].termios.enable = TRUE;
 m340_uart_config[UART_CHANNEL_B].termios.rx_buffer_size = NOT_IMPLEMENTED_YET;
 m340_uart_config[UART_CHANNEL_B].termios.tx_buffer_size = NOT_IMPLEMENTED_YET;
}

/******************************************************
  Name: Find_Right_m340_UART_Channel_Config
  Input parameters: Send/Receive baud rates for a
		    given channel
  Output parameters: UART compatible configs for this
		    channel
  Description: returns which uart configurations fit
	       Receiver Baud Rate and Transmitter Baud
	       Rate for a given channel
	       For instance, according to the
	       m340_Baud_Rates_Table:
               - Output Speed = 50, Input Speed = 75
		 is not a correct config, because
		 50 bauds implies set 1 and 75 bauds
		 implies set 2
               - Output Speed = 9600, Input Speed = 9600
		 two correct configs for this:
		 RCS=11, TCS=11, Set=1 or 2
 *****************************************************/
t_baud_speed_table
Find_Right_m340_UART_Channel_Config(float ReceiverBaudRate, float TransmitterBaudRate)
{
 t_baud_speed_table return_value;

 struct {
	 int cs;
	 int set;
	} Receiver[2], Transmitter[2];

 int Receiver_nb_of_config = 0;
 int Transmitter_nb_of_config = 0;

 int i,j;

 /* Receiver and Transmitter baud rates must be compatible, ie in the same set */

 /* search for configurations for ReceiverBaudRate - there can't be more than two (only two sets) */
 for (i=0;i<16;i++)
     for (j=0;j<2;j++)
	 if (m340_Baud_Rates_Table[i][j]==ReceiverBaudRate) {
	    Receiver[Receiver_nb_of_config].cs=i;
	    Receiver[Receiver_nb_of_config].set=j;
	    Receiver_nb_of_config++;
	 }

 /* search for configurations for TransmitterBaudRate - there can't be more than two (only two sets) */
 for (i=0;i<16;i++)
     for (j=0;j<2;j++)
	 if (m340_Baud_Rates_Table[i][j]==TransmitterBaudRate) {
	    Transmitter[Transmitter_nb_of_config].cs=i;
	    Transmitter[Transmitter_nb_of_config].set=j;
	    Transmitter_nb_of_config++;
	 }

 /* now check if there's a compatible config */
 return_value.nb=0;

 for (i=0; i<Receiver_nb_of_config; i++)
     for (j=0;j<Transmitter_nb_of_config;j++)
	 if (Receiver[i].set == Transmitter[j].set) {
	    return_value.baud_speed_table[return_value.nb].set = Receiver[i].set + 1; /* we want set 1 or set 2, not 0 or 1 */
	    return_value.baud_speed_table[return_value.nb].rcs = Receiver[i].cs;
	    return_value.baud_speed_table[return_value.nb].tcs = Transmitter[j].cs;
	    return_value.nb++;
	 }

 return return_value;
}

/******************************************************
  Name: Find_Right_m340_UART_Config
  Input parameters: Send/Receive baud rates for both
		    channels
  Output parameters: UART compatible configs for
		     BOTH channels
  Description: returns which uart configurations fit
	       Receiver Baud Rate and Transmitter Baud
	       Rate for both channels
	       For instance, if we want 9600/38400 on
	       channel A and 9600/19200 on channel B,
	       this is not a good m340 uart config
	       (channel A needs set 1 and channel B
	       needs set 2)
 *****************************************************/
t_baud_speed_table
Find_Right_m340_UART_Config(float ChannelA_ReceiverBaudRate, float ChannelA_TransmitterBaudRate, uint8_t         enableA,
			    float ChannelB_ReceiverBaudRate, float ChannelB_TransmitterBaudRate, uint8_t         enableB)
{
 t_baud_speed_table tableA, tableB;
 t_baud_speed_table return_value, tmp;
 int i,j;

 memset( &return_value, '\0', sizeof(return_value) );
 return_value.nb=0;

 if (enableA && enableB) {
  tableA = Find_Right_m340_UART_Channel_Config(ChannelA_ReceiverBaudRate, ChannelA_TransmitterBaudRate);
  tableB = Find_Right_m340_UART_Channel_Config(ChannelB_ReceiverBaudRate, ChannelB_TransmitterBaudRate);

  for (i=0;i<tableA.nb;i++)
      for (j=0;j<tableB.nb;j++)
          if (tableA.baud_speed_table[i].set==tableB.baud_speed_table[j].set) {
	     return_value.baud_speed_table[UART_CHANNEL_A].set=tableA.baud_speed_table[i].set;
	     return_value.baud_speed_table[UART_CHANNEL_A].rcs=tableA.baud_speed_table[i].rcs;
	     return_value.baud_speed_table[UART_CHANNEL_A].tcs=tableA.baud_speed_table[i].tcs;
	     return_value.baud_speed_table[UART_CHANNEL_B].set=tableB.baud_speed_table[j].set;
	     return_value.baud_speed_table[UART_CHANNEL_B].rcs=tableB.baud_speed_table[j].rcs;
	     return_value.baud_speed_table[UART_CHANNEL_B].tcs=tableB.baud_speed_table[j].tcs;
	     return_value.nb=2;
	     break;
	  }
   return return_value;
 }

 if (enableA) {
    return_value = Find_Right_m340_UART_Channel_Config(ChannelA_ReceiverBaudRate, ChannelA_TransmitterBaudRate);
    return return_value;
 }

 if (enableB) {
    tmp = Find_Right_m340_UART_Channel_Config(ChannelB_ReceiverBaudRate, ChannelB_TransmitterBaudRate);
    if (tmp.nb!=0) {
       return_value.nb = 2;
       return_value.baud_speed_table[1].set = tmp.baud_speed_table[0].set;
       return_value.baud_speed_table[1].rcs = tmp.baud_speed_table[0].rcs;
       return_value.baud_speed_table[1].tcs = tmp.baud_speed_table[0].tcs;
    }
 }
 return return_value;
}

/****************************************************************************************************/

/*
 * very low level fmted output
 */

extern void dbug_out_char( int minor, int ch );
extern int dbug_in_char( int minor );
extern int dbug_char_present( int minor );

/******************************************************
  Name: dbugRead
  Input parameters: channel
  Output parameters: char read
  Description: polled read
 *****************************************************/
int dbugRead (int minor)
{
	if (dbug_char_present(minor) == 0)
		return -1;
	return dbug_in_char(minor);
}

/******************************************************
  Name: dbugWrite
  Input parameters: channel, buffer and its length
  Output parameters: always successfull
  Description: polled write
 *****************************************************/
ssize_t dbugWrite (int minor, const char *buf, size_t len)
{
	static char txBuf;
	size_t retval = len;

	while (len--) {
 		txBuf = *buf++;
        	dbug_out_char( minor, (int)txBuf );
	}
	return retval;
}

static void fmt_num( int minor, unsigned long, unsigned );
static void fmt_str( int minor, const char* );

/******************************************************
  Name: RAW_GETC
  Input parameters: channel, buffer and its length
  Output parameters:
  Description: a light blocking "getc"
 *****************************************************/
char RAW_GETC(int minor)
{
 while (!dbug_char_present(minor)) continue;
 return dbug_in_char(minor);
}

/******************************************************
  Name: RAW_FMT
  Input parameters: channel, buffer and its length
  Output parameters: always successfull
  Description: a light polled "printf"
	       useful when there's a serious pb and
	       there are no more interrupts
 *****************************************************/
void RAW_FMT( int minor, const char* fmt, ... )
{
   int ch;
   va_list va;

   va_start( va, fmt );

   while( (ch = *fmt++) )
      if( ch != '%' || (ch = *fmt++) == '%' )
      {
	 if( ch == '\n' )
	    dbug_out_char( minor, '\r' );
	 dbug_out_char( minor, ch );
      }
      else switch( ch )
      {
	 case 'c':
	 dbug_out_char( minor, va_arg( va, int ) );
	 continue;
	 case 's':
	 fmt_str( minor, va_arg( va, char* ) );
	 continue;
	 case 'd':
	 ch = va_arg( va, int );
	 if( ch >= 0 )
	    fmt_num( minor, ch, 10 );
	 else
	 {
	    dbug_out_char( minor, '-' );
	    fmt_num( minor, -ch, 10 );
	 }
	 continue;
	 case 'u':
	 fmt_num( minor, va_arg( va, unsigned ), 10 );
	 continue;
	 case 'o':
	 fmt_num( minor, va_arg( va, unsigned ), 8 );
	 continue;
	 case 'x':
	 case 'p':
	 fmt_num( minor, va_arg( va, unsigned ), 16 );
	 continue;
	 default: continue;
	 return;
      }
   va_end( va );
}

static void fmt_num( int minor, unsigned long num, unsigned base )
{
   char buf[33];
   int  ib = sizeof(buf);

   buf[--ib] = 0;
   do
   {
      buf[--ib] = "0123456789ABCDEF"[num%base];
      num /= base;
   }
   while( num != 0 );

   fmt_str( minor, buf+ib );
}

static void fmt_str( int minor, const char* str )
{
   if( str )
      while( *str )
	 dbug_out_char( minor, *str++ );
}
