/*
 *  $Id$
 */

#include "serial.h"
#include "rtems.h"


typedef unsigned char uchar ;           /* Abbreviations */
typedef unsigned short ushort ;
typedef unsigned long ulong ;
int DBGConsole_make_sync = 0;
#define CONSOLE_CHANNELS	1 

#define MAX_CONSOLE 4
static int consoles[MAX_CONSOLE]; 
static int active_consoles = 0;
static struct{
	rtems_id sem;
	int rx;
	int cnt;
	char in_line[128];
}cons_input[MAX_CONSOLE];



/* This uses the message out and in buffers as serial emulator.
	Pretty stupid eh? 
*/

#define uart1	((volatile unsigned char *)0x1318)
#define uart1_rx	((volatile unsigned int *)0x1310)

#define NUM_UARTS 1
static volatile unsigned int * uart = { uart1 };
static volatile unsigned int * uart_rx = { uart1_rx };


extern void	display_msg(void);
/*extern int	sprintf();*/



int
console_uartinit(unsigned int BAUDRate)
{
#ifdef CONSOLE_CHANNELS
	void cons_isr();
	rpmu_attach_inmsg0(cons_isr);
#endif
	return(0);
}

	
/* Introduce a new console channel */
console_new(char * name)
{
#ifdef CONSOLE_CHANNELS
	unsigned int x, stat;
	x = 0xfe000000 | (name[0] << 16) | (name[1] << 8) | name[2];
	do {
		stat = *uart;
	} while (DBGConsole_make_sync && (stat != 0));
	*uart = x;
	x = ( name[3] << 24) | ( name[4] << 16) | ( name[5] << 8) |  name[6] ;
	do {
		stat = *uart;
	} while (DBGConsole_make_sync && (stat != 0));
	*uart = x;
	active_consoles += 1;
	rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &consoles[active_consoles] );
#endif
}
	


    /***********************************************************************
     ***  Transmit character to host.
     *** put the console ID in upper byte
     *** 
     ***********************************************************************/

int console_sps_putc(int cc)
{
	register unsigned char	stat;
	int rtid, i;
	unsigned int ch;
	unsigned int level;
#ifdef CONSOLE_CHANNELS
	rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &rtid );
	ch = cc & 0xff;
	for(i=1; i <= active_consoles; i++){
		if( rtid == consoles[i]){
			ch |= (i ) << 24 ;
			break;
		}
	}
#else
	ch = cc;
#endif

	/*
	 *  Pause until there is room in the UART transmit
	 *  buffer.
	 */

        if (ch == -1)
          return ch;

wait:
	do {
		stat = *uart;
	} while (DBGConsole_make_sync && (stat != 0));
	rtems_interrupt_disable(level);
	if( (*uart != 0) && DBGConsole_make_sync){
		rtems_interrupt_enable(level);
		goto wait;
	}

	/*
	 *  Transmit data. (Junk)
	 */

	*uart = ch;
	rtems_interrupt_enable(level);
	return cc;

}


    /*
      * putnum -- print a 32 bit number in hex
      */
     int
     putnum (num)
     unsigned int num;
     {
       char  buffer[9];
       int   count;
       int   digit;
     
       for (count = 7 ; count >= 0 ; count--) {
         digit = (num >> (count * 4)) & 0xf;
     
         if (digit <= 9)
           console_sps_putc( (char) ('0' + digit));
         else
           console_sps_putc( (char) ('A' - 10 + digit));
       }
     }

    /*
      * putmem -- print the specified memory block
      */
     void
     putmem (addr, num)
     char *addr;
     unsigned int num;
     {
       int i = 0;
       int j = 0;
       int val = 0;
       int digit = 0;
      
       console_sps_putc(13);
       console_sps_putc(10);
       putnum((unsigned int) addr);
       console_sps_putc(':');
       console_sps_putc(' ');
       while(num)
       {
         val = *addr;

         for (j = 0; j < 2; j++)
         {
           digit = (val & 0xf0) >> 4;
           val <<= 4;

           if (digit < 10) 
           {
             console_sps_putc(digit + '0');
           }
           else
           {
             console_sps_putc(digit - 10 + 'A');
           }
         }
         console_sps_putc(' ');

         num--;
         addr++;
         if (++i == 16)
         {
           console_sps_putc(13);
           console_sps_putc(10);
           putnum((unsigned int) addr);
           console_sps_putc(':');
           console_sps_putc(' ');
           i = 0;
         }
       }
       console_sps_putc(13);
       console_sps_putc(10);
     }

    /*
      * putcmem -- print the specified pci config memory block
      */
     void
     putcmem (addr, num)
     unsigned char *addr;
     unsigned int num;
     {
       int i = 0;
       int j = 0;
       unsigned short val = 0;
       int digit = 0;
       unsigned int *satucmd = (unsigned int *) 0x1298;
       unsigned int *soccar = (unsigned int *) 0x12a8;
       unsigned int *soccdp = (unsigned int *) 0x12b0;
      
       *satucmd = 4;

       console_sps_putc(13);
       console_sps_putc(10);
       putnum((unsigned int) addr);
       console_sps_putc(':');
       console_sps_putc(' ');
       while(num)
       {
         *soccar =  (unsigned int) addr;
         val = *soccdp;

         for (j = 0; j < 4; j++)
         {
           digit = (val & 0xf000) >> 12;
           val <<= 4;

           if (digit < 10) 
           {
             console_sps_putc(digit + '0');
           }
           else
           {
             console_sps_putc(digit - 10 + 'A');
           }
         }
         console_sps_putc(' ');

         num -= 2;
         addr += 2;
         if (++i == 8)
         {
           console_sps_putc(13);
           console_sps_putc(10);
           putnum((unsigned int) addr);
           console_sps_putc(':');
           console_sps_putc(' ');
           i = 0;
         }
       }
       console_sps_putc(13);
       console_sps_putc(10);
     }

    /***********************************************************************
     ***  Read character from host.
     ***********************************************************************/
#ifdef CONSOLE_CHANNELS
int console_sps_getc()
{
	
	int consinx;
	int rtid, i;
	unsigned int level, level2;
	char ch;
	consinx = 0;
	rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &rtid );
	for(i=1; i <= active_consoles; i++){
		if( rtid == consoles[i]){
			consinx = i ;
			break;
		}
	}
	if( i > active_consoles)
		consinx = 0;
	if( cons_input[consinx].sem == 0){
		rtems_name sname;
		sname = rtems_build_name('S','U','X',(char)(consinx + '0'));
		rtems_semaphore_create(sname, 0, RTEMS_DEFAULT_ATTRIBUTES, 0, &cons_input[consinx].sem);
		cons_input[consinx].rx = 0;
	}
	while( cons_input[consinx].cnt == cons_input[consinx].rx){
		rtems_semaphore_obtain(cons_input[consinx].sem, RTEMS_WAIT, 0);
/*	rtems_task_wake_after( RTEMS_YIELD_PROCESSOR);*/
}
	rtems_interrupt_disable(level);
	i = cons_input[consinx].rx;
	ch = cons_input[consinx].in_line[i];
	i++;
	if( i >= sizeof( cons_input[consinx].in_line))
		i = 0;
	cons_input[consinx].rx = i;
	rtems_interrupt_enable(level);
	return ch;
}
	

void cons_isr()
{
	unsigned int i, chin,  consinx, st;
	chin = *uart_rx;
	consinx = chin >> 24;
	if( consinx > active_consoles)
		goto release;
	i = cons_input[consinx].cnt;
	cons_input[consinx].in_line[i] = chin & 0xff;
	i++;
	if( i >= sizeof( cons_input[consinx].in_line))
		i = 0;
	cons_input[consinx].cnt = i;
	st = rtems_semaphore_release( cons_input[consinx].sem);
release:
	*uart_rx = 0;
}
	
#else
volatile int console_foo = 0;
int console_sps_getc()
{
	volatile unsigned int	stat;
	register int	ch;

	stat = *uart_rx;
	while (stat == 0)
        {
                rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
		stat = *uart_rx;
		console_foo++;
        }
	*uart_rx = 0;

	ch = stat;

	return ch;
}
#endif

    /***********************************************************************
     ***  check character from host.
     ***********************************************************************/

int console_sps_kbhit()
{
        register unsigned short stat;

        stat = *uart;
        return ( stat != 0);
}




