/*
 *  This file contains the MVME162LX extended console IO package.
 *
 *  This file was created originally by 
 *  On-Line Applications Research Corporation (OAR)
 *  and modified by:
 *
 *  Katsutoshi Shibuya - BU-Denken Co.,Ltd. - Sapporo, JAPAN
 *
 *  featuring support of:
 *
 *     - Multi-SCC chip handling
 *     - Non-blocking I/O (O_NDELAY flag in libc)
 *     - Raw mode device (no CR/LF detection)
 *     - RTS/CTS flow control
 *
 *  REMARKS: This routine requires multiple interrupt vectors
 *           from SCC_VECTOR (normaly 0x40)
 *           to SCC_VECTOR+(number of SCC chips)
 *
 *  The original copyright follows;
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#define M162_INIT

#include "consolex.h"
#include <bsp.h>
#include <rtems/libio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define	NPORTS	4	/* Number of ports */
#define	DEVICEPREFIX	"tty"
#define	RAWDEVICEPREFIX	"rtty"
#define	CONSOLEPORT 0	/* port# of console:
			   undef this if you do not want /dev/console */
#define	READRETRY 1	/* Maximum retry count for read one char */
#define	WRITERETRY 8	/* Maximum retry count for write one char */

#define	PORTFROM 0	/* for debug */

static unsigned char	opencount[NPORTS];

/***********************************************************************
   Ring buffer for device
 ***********************************************************************/

#define QUEUE_LENGTH 128	/* Must be 2^n number */

typedef struct {
  char buffer[QUEUE_LENGTH];
  volatile int  head;
  volatile int  tail;
} ReceiverBuffer;

#define ReceiverBufferInitialize( _buffer ) \
  do { \
    (_buffer)->head = (_buffer)->tail = 0; \
  } while ( 0 ) 
  
#define ReceiverBufferIsEmpty( _buffer ) \
   ( (_buffer)->tail == (_buffer)->head )

#define ReceiverBufferIsNearEmpty( _buffer ) \
   ( (_buffer)->tail == (((_buffer)->head + 3) & (QUEUE_LENGTH-1)) )

#define ReceiverBufferIsFull( _buffer ) \
   ( (_buffer)->head == (((_buffer)->tail + 1) & (QUEUE_LENGTH-1)) )

#define ReceiverBufferIsNearFull( _buffer ) \
   ( (_buffer)->head == (((_buffer)->tail + 3) & (QUEUE_LENGTH-1)) )

#define ReceiverBufferAdd( _buffer, _ch ) \
  do { \
    rtems_unsigned32 isrlevel; \
    \
    rtems_interrupt_disable( isrlevel ); \
      (_buffer)->tail = ((_buffer)->tail+1) & (QUEUE_LENGTH-1); \
      (_buffer)->buffer[ (_buffer)->tail ] = (_ch); \
    rtems_interrupt_enable( isrlevel ); \
  } while ( 0 ) 

#define ReceiverBufferRemove( _buffer, _ch ) \
  do { \
    rtems_unsigned32 isrlevel; \
    \
    rtems_interrupt_disable( isrlevel ); \
      (_buffer)->head = ((_buffer)->head+1) & (QUEUE_LENGTH-1); \
      (_ch) = (_buffer)->buffer[ (_buffer)->head ]; \
    rtems_interrupt_enable( isrlevel ); \
  } while ( 0 ) 


/***********************************************************************
   DEVICE DEPENDED PART
 ***********************************************************************/


/* Time constant parameters
   CAUTION: These parameters are for MVME162LX-213 board.
 */

#define	TC38400	0x0006
#define	TC19200	0x000e
#define	TC9600	0x001e

/* Re-defining SCC register control macros
   to support Multi SCC chips */

#undef	scc
#if defined(mvme162lx)
static scc_regs	*scc[NPORTS] = {
    ((scc_regs * const) 0xFFF45004),
    ((scc_regs * const) 0xFFF45000),
    ((scc_regs * const) 0xFFF45804),
    ((scc_regs * const) 0xFFF45800)
};
#else
/* XXX fix me */
#warning "MVME162 BSP -- unknown address for SCC's"
static scc_regs	*scc[NPORTS] = {
    ((scc_regs * const) 0xFFF45004),
    ((scc_regs * const) 0xFFF45000),
    ((scc_regs * const) 0xFFF45804),
    ((scc_regs * const) 0xFFF45800)
};
#endif

#undef	ZWRITE0
#define ZWRITE0(port, v)  (scc[port]->csr = (unsigned char)(v))
#undef	ZREAD0
#define ZREAD0(port)  (scc[port]->csr)

#undef	ZREAD
#define ZREAD(port, n)  (ZWRITE0(port, n), (scc[port]->csr))
#undef	ZREADD
#define ZREADD(port)  (scc[port]->csr=0x08, scc[port]->csr )

#undef	ZWRITE
#define ZWRITE(port, n, v) (ZWRITE0(port, n), ZWRITE0(port, v))
#undef	ZWRITED
#define ZWRITED(port, v)  (scc[port]->csr = 0x08, \
                           scc[port]->csr = (unsigned char)(v))

static ReceiverBuffer	receiverBuffer[NPORTS];

/*
 *  Control flags (DTR/DCD/RTS/CTS)
 */

static unsigned char	wr4[NPORTS];
static unsigned char	wr5[NPORTS];
#define	SCCSetDTR(port) ZWRITE(port, 5, (wr5[port] |= 0x80))
#define	SCCResetDTR(port) ZWRITE(port, 5, (wr5[port] &= ~0x80))
#define	SCCSetRTS(port) ZWRITE(port, 5, (wr5[port] |= 0x02))
#define	SCCResetRTS(port) ZWRITE(port,5, (wr5[port] &= ~0x02))
#define	SCCGetDCD(port) (ZREAD0(port)&0x08)
#define	SCCGetCTS(port) (ZREAD0(port)&0x20)


/*
 *  Interrupt handler for receiver interrupts
 */

static rtems_isr	SCCReceiverISR(rtems_vector_number vector)
{
    register int    ipend, port;

    port = (vector-SCC_VECTOR)*2;
    ZWRITE0(port, 0x38);	/* reset highest IUS */

    ipend = ZREAD(port, 3);	/* read int pending from A side */

    if(ipend == 0x04)
	port++; /* channel B intr pending */
    else if(ipend == 0x20)
	;	/* channel A intr pending */
    else
	return;
    
    ReceiverBufferAdd(&receiverBuffer[port], ZREADD(port));
    
    if(ZREAD(port,1) & 0x70){ /* check error stat */
	ZWRITE0(port, 0x30);	/* reset error */
    }

    if(ReceiverBufferIsNearFull(&receiverBuffer[port]))
	SCCResetRTS(port);
}

/*
 * Initialize
 */

void	SCCInitialize()
{
    int     i;

    for(i = PORTFROM; i < NPORTS; i+=2)
	ZWRITE(i, 9,0xc0);	/* Reset SCC Chip */
    
    for(i = PORTFROM; i < NPORTS; i++){
	ReceiverBufferInitialize(&(receiverBuffer[i]));
	wr4[i] = 0x44;
	ZWRITE(i, 4, wr4[i]);	/* x16 clock, 1 stop, parity none */
	ZWRITE(i, 1, 0);	/* disable interrupts */
	ZWRITE(i, 2, SCC_VECTOR+(i/2));
	ZWRITE(i, 3, 0xc1);	/* receiver enable, 8bits */
	wr5[i] = 0x68;
	ZWRITE(i, 5, wr5[i]);	/* transmitter enable, 8bits, DTR&RTS off */
	ZWRITE(i,14, 0);	/* stop baudrate gen. */
	ZWRITE(i,11,0x50);	/* use baurate gen. */
	ZWRITE(i,15, 1);	/* Select WR7' */
	ZWRITE(i, 7, 0);	/* Disable all special interrupts */
	ZWRITE(i,10, 0);
	ZWRITE(i, 1, 0x10);     /* int on all Rx chars or special condition */
	set_vector(SCCReceiverISR, SCC_VECTOR+(i/2), 1); /* install ISR */
	ZWRITE(i, 9, 8);        /* master interrupt enable */
	ZWRITE(i,12,TC38400&0xff);	/* set 38400 baud */
	ZWRITE(i,13,TC38400>>8);
	ZWRITE(i,14, 3);	/* start baudrate gen. */
				/* CAUTION: If your SCC use XTAL on RTxC,
				   write 1 */
    }
    mcchip->vector_base = 0;
    mcchip->gen_control = 2;	/* MIEN */
    mcchip->SCC_int_ctl = 0x13;	/* SCC IEN, IPL3 */
}

/*
 *   Non-blocking char input
 */

rtems_boolean	SCCGetOne(int port, char *ch)
{
    int	retry = READRETRY;
    while(ReceiverBufferIsEmpty(&receiverBuffer[port]))
	if(--retry <= 0)
	    return FALSE;

    ReceiverBufferRemove(&receiverBuffer[port],*ch);

    if(ReceiverBufferIsNearEmpty(&receiverBuffer[port]))
	SCCSetRTS(port);
    return TRUE;
}

/*
 *   Blocking char input
 */

char	SCCGetOneBlocked(int port)
{
    unsigned char tmp_char;
 
    while (!SCCGetOne(port, &tmp_char))
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
    return tmp_char;
}

/*
 *   Non-blocking char input
 *   No longer supports XON/XOFF flow control.
 */

rtems_boolean	SCCSendOne(int port, char ch)
{
    int	retry = WRITERETRY;

    if(!SCCGetCTS(port))
	return FALSE;
    while(!(ZREAD0(port) & TX_BUFFER_EMPTY))
	if(--retry <= 0)
	    return FALSE;
    ZWRITED(port, ch);
    return TRUE;
}


/*  
 *   Blocking char output
 *   No longer supports XON/XOFF flow control.
 */

void	SCCSendOneBlocked(int port, char ch)
{
    while(!SCCSendOne(port, ch))
	rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

/*
 * Set parameters for transmission
 */

unsigned32	SCCSetAttributes(int port, struct termios *t)
{
    unsigned char	wr3;
    
    ZWRITE(port,1,0);	/* Disable interrupt */
    ZWRITE(port,3,0);	/* Disable receiver */
    /* Baud */
    switch(t->c_cflag & CBAUD){
    case B38400:
	ZWRITE(port,12,TC38400&0xff);
	ZWRITE(port,13,TC38400>>8);
	break;
    case B19200:
	ZWRITE(port,12,TC19200&0xff);
	ZWRITE(port,13,TC19200>>8);
	break;
    case B9600:
	ZWRITE(port,12,TC9600&0xff);
	ZWRITE(port,13,TC9600>>8);
	break;
    }
    
    /* Code size */
    wr5[port] &= 0x8f;
    wr3 = 0;	/* receiver control */
    switch(t->c_cflag & CSIZE){
    case CS8:
	wr5[port] |= 0x60;
	wr3 |= 0xc0;
	break;
    case CS7:
	wr5[port] |= 0x20;
	wr3 |= 0x40;
	break;
    }

    /* Parity */
    wr4[port] &= 0xf0;
    if(t->c_cflag & PARENB)
	wr4[port] |= 0x01;
    if(!(t->c_cflag & PARODD))
	wr4[port] |= 0x02;
    /* ZWRITE(port,4,wr4[port]);*/

    /* Stop bits */
    /* wr4[port] = ZREAD(port,4) & 0xfc;*/
    if(t->c_cflag & CSTOPB)
	wr4[port] |= 0x0c;
    else
	wr4[port] |= 0x04;
    
    ZWRITE(port,4,wr4[port]);	/* TxRx parameters */
    ZWRITE(port,5,wr5[port]);	/* Transmission parameters */
    ZWRITE(port,3,wr3|0x01);	/* Enable receiver */
    ZWRITE(port,1,0x10);	/* Enable interrupt */

    return 0;
}

/*
 * Get parameters for transmission
 */

unsigned32	SCCGetAttributes(int port, struct termios *t)
{
    unsigned32	b;

    t->c_cflag = 0;
    
    /* Baud */
    b = ZREAD(port,13);
    b <<= 8;
    b |= ZREAD(port,12);
    switch(b){
    case TC38400:
	t->c_cflag |= B38400;
	break;
    case TC19200:
	t->c_cflag |= B19200;
	break;
    case TC9600:
	t->c_cflag |= B9600;
	break;
    }
    
    /* Code size */
    /* wr = ZREAD(port,5);*/
    t->c_cflag &= ~CSIZE;
    switch(wr5[port]&0x60){
    case 0x60:
	t->c_cflag |= CS8;
	break;
    case 0x20:
	t->c_cflag |= CS7;
	break;
    }

    /* Parity */
    /* wr = ZREAD(port,4);*/
    if(wr4[port] & 0x01)
	t->c_cflag |= PARENB;
    else
	t->c_cflag &= ~PARENB;
    if(wr4[port] & 0x02)
	t->c_cflag &= ~PARODD;
    else
	t->c_cflag |= PARODD;

    /* Stop bits */
    /* wr = ZREAD(port,4);*/
    if((wr4[port]&0xc0) == 0xc0)
	t->c_cflag |= CSTOPB;
    else
	t->c_cflag &= ~CSTOPB;

    return 0;
}

/***********************************************************************
   DEVICE INDEPENDED PART
 ***********************************************************************/

#define	LOCAL_ISTRIP	0x0001
#define	LOCAL_INLCR	0x0002
#define	LOCAL_IGNCR	0x0004
#define	LOCAL_ICRNL	0x0008
#define	LOCAL_IUCLC	0x0010
#define	LOCAL_OLCUC	0x0020
#define	LOCAL_ONLCR	0x0040
#define	LOCAL_OCRNL	0x0080
#define	LOCAL_ICANON	0x0100
#define	LOCAL_ECHO	0x0200

/*
 *  Device initialize entry point
 */

rtems_device_driver consolex_initialize(rtems_device_major_number  major,
					rtems_device_minor_number  minor,
					void *arg)
{
    rtems_status_code	status;
    char	devname[16];
    int		i;
    
    SCCInitialize();

#ifdef	CONSOLEPORT
    status = rtems_io_register_name("/dev/console",major,
				    (rtems_device_minor_number) CONSOLEPORT);
    if (status != RTEMS_SUCCESSFUL)
	rtems_fatal_error_occurred(status);
#endif
    
    for(i = PORTFROM; i < NPORTS; i++){
	/* Register cooked ttys */
	sprintf(devname,"/dev/%s%02d",DEVICEPREFIX,i);
	status = rtems_io_register_name(strdup(devname),major,
					(rtems_device_minor_number) i);
	if (status != RTEMS_SUCCESSFUL)
	    rtems_fatal_error_occurred(status);
	/* Register raw ttys */
	sprintf(devname,"/dev/%s%02d",RAWDEVICEPREFIX,i);
	status = rtems_io_register_name(strdup(devname),major,
					(rtems_device_minor_number) i+NPORTS);
	if (status != RTEMS_SUCCESSFUL)
	    rtems_fatal_error_occurred(status);
    }

    for(i = 0; i < NPORTS; i++){
	opencount[i] = 0;
    }
    
    return RTEMS_SUCCESSFUL;
}

/*
 *  Open entry point
 */

rtems_device_driver consolex_open(rtems_device_major_number major,
				  rtems_device_minor_number minor,
				  void *arg)
{
    rtems_libio_open_close_args_t *openargs = (rtems_libio_open_close_args_t *) arg;
    if(minor >= NPORTS)
	minor -= NPORTS;
    if(minor >= NPORTS)
	return RTEMS_INVALID_NUMBER;

    if(opencount[minor]++ == 0){
	/* first open */
	SCCSetDTR(minor);
	SCCSetRTS(minor);
    }
    openargs->iop->data0 = LOCAL_ICRNL|LOCAL_ONLCR|LOCAL_ICANON|LOCAL_ECHO;
    return RTEMS_SUCCESSFUL;
}

/*
 *  Close entry point
 */

rtems_device_driver consolex_close(rtems_device_major_number major,
				   rtems_device_minor_number minor,
				   void *arg)
{
    if(minor >= NPORTS)
	minor -= NPORTS;
    if(minor >= NPORTS)
	return RTEMS_INVALID_NUMBER;

    if(--(opencount[minor]) == 0){
	/* closed all */
	SCCResetRTS(minor);
	SCCResetDTR(minor);
    }
    return RTEMS_SUCCESSFUL;
}

/*
 * read bytes from the serial port.
 */

rtems_device_driver consolex_read_raw(rtems_device_major_number major,
				      rtems_device_minor_number minor,
				      void *arg)
{
    rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
    char *buffer;
    int count;

    if(minor >= NPORTS)
	return RTEMS_INVALID_NUMBER;

    buffer = rw_args->buffer;
    count = rw_args->count;

    if(rw_args->flags & LIBIO_FLAGS_NO_DELAY){
	/* Non blocking read */
	while(count){
	    if(!SCCGetOne(minor,buffer))
		break;
	    buffer++;
	    count--;
	}
    }else{
	/* Blocking read */
	while(count){
	    *buffer = SCCGetOneBlocked(minor);
	    buffer++;
	    count--;
	}
    }

    rw_args->bytes_moved = rw_args->count-count;
    return count ? RTEMS_UNSATISFIED : RTEMS_SUCCESSFUL;
}

rtems_device_driver consolex_read(rtems_device_major_number major,
				      rtems_device_minor_number minor,
				      void *arg)
{
    rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
    char *buffer;
    int count;
    unsigned32	mode;

    if(minor >= NPORTS)
	return consolex_read_raw(major,minor-NPORTS,arg);

    buffer = rw_args->buffer;
    count = rw_args->count;
    mode = rw_args->iop->data0;
    
    /* Cooked read */
    while(count){
	if(rw_args->flags & LIBIO_FLAGS_NO_DELAY){
	    /* Non blocking read */
	    if(!SCCGetOne(minor,buffer))
		break;
	}else{
	    /* Blocking read */
	    *buffer = SCCGetOneBlocked(minor);
	}
	if((mode&LOCAL_ICANON) && (*buffer == '\b')){
	    if(buffer > (char *)(rw_args->buffer)){
		buffer--;
		count++;
		if(mode&LOCAL_ECHO){
		    SCCSendOneBlocked(minor,'\b');
		    SCCSendOneBlocked(minor,' ');
		    SCCSendOneBlocked(minor,'\b');
		}
	    }
	    continue;
	}
	if((mode&LOCAL_IGNCR) && (*buffer == '\r'))
	    continue;
	if((mode&LOCAL_INLCR) && (*buffer == '\n'))
	    *buffer = '\r';
	if((mode&LOCAL_ICRNL) && (*buffer == '\r'))
	    *buffer = '\n';
	if((mode&LOCAL_IUCLC) && isupper((int)*buffer))
	    *buffer = tolower(*buffer);
	if(mode&LOCAL_ISTRIP)
	    *buffer &= 0x7f;
	if(mode&LOCAL_ECHO){
	    /* Caution: Echo back is blocking output */
	    SCCSendOneBlocked(minor,*buffer);
	}
	if((mode&LOCAL_ICANON) && (*buffer == '\n')){
	    buffer++;
	    count--;
	    if(count)
		*buffer = 0;
	    if((mode&LOCAL_ECHO)&&(mode&LOCAL_ONLCR))
		SCCSendOneBlocked(minor,'\r');
	    break;	/* finish reading */
	}
	buffer++;
	count--;
    }
    rw_args->bytes_moved = rw_args->count-count;
    return count ? RTEMS_UNSATISFIED : RTEMS_SUCCESSFUL;
}

/*
 * write bytes to the serial port.
 */

rtems_device_driver consolex_write_raw(rtems_device_major_number major,
				       rtems_device_minor_number minor,
				       void * arg)
{
    rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
    char *buffer;
    int count;

    if(minor >= NPORTS)
	return RTEMS_INVALID_NUMBER;

    buffer = rw_args->buffer;
    count = rw_args->count;

    if(rw_args->flags & LIBIO_FLAGS_NO_DELAY){
	/* Non blocking write */
	while(count){
	    if(!SCCSendOne(minor,*buffer))
		break;
	    buffer++;
	    count--;
	}
    }else{
	/* Blocking write */
	while(count){
	    SCCSendOneBlocked(minor,*buffer);
	    buffer++;
	    count--;
	}
    }
    
    rw_args->bytes_moved = rw_args->count-count;
    return count ? RTEMS_UNSATISFIED : RTEMS_SUCCESSFUL;
}

rtems_device_driver consolex_write(rtems_device_major_number major,
				   rtems_device_minor_number minor,
				   void * arg)
{
    rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
    char *buffer;
    int count;
    char	ch;
    unsigned32	mode;

    if(minor >= NPORTS)
	return consolex_write_raw(major,minor-NPORTS,arg);

    buffer = rw_args->buffer;
    count = rw_args->count;
    mode = rw_args->iop->data0;

    /* Cooked write */
    while(count){
	ch = *buffer;
	if((mode&LOCAL_ONLCR) && (ch == '\n')){	/* Output CRLF */
	    if(rw_args->flags & LIBIO_FLAGS_NO_DELAY){
		/* Non blocking write */
		if(!SCCSendOne(minor,'\r'))
		    break;
	    }else{
		SCCSendOneBlocked(minor,'\r');
	    }
	}
	if((mode&LOCAL_OCRNL) && (ch == '\r'))
	    ch = '\n';
	if((mode&OLCUC) && (islower((int)ch)))
	    ch = toupper(ch);
	if(rw_args->flags & LIBIO_FLAGS_NO_DELAY){
	    /* Non blocking write */
	    if(!SCCSendOne(minor,ch))
		break;
	}else{
	    SCCSendOneBlocked(minor,ch);
	}
	buffer++;
	count--;
    }
    
    rw_args->bytes_moved = rw_args->count-count;
    return count ? RTEMS_UNSATISFIED : RTEMS_SUCCESSFUL;
}

/*
 *  IO Control entry point
 */

rtems_device_driver consolex_control(rtems_device_major_number major,
				     rtems_device_minor_number minor,
				     void * arg)
{
    rtems_libio_ioctl_args_t	*ioarg = (rtems_libio_ioctl_args_t *)arg;
    struct termios	*tm = ioarg->buffer;
    unsigned32	*mode = &(ioarg->iop->data0);

    if(minor >= NPORTS)
	minor -= NPORTS;
    if(minor >= NPORTS)
	return RTEMS_INVALID_NUMBER;

    switch(ioarg->command){
    case RTEMS_IO_GET_ATTRIBUTES:
	tm->c_iflag = tm->c_oflag = tm->c_cflag = tm->c_lflag = 0;
	if(*mode & LOCAL_ISTRIP)
	    tm->c_iflag |= ISTRIP;
	if(*mode & LOCAL_INLCR)
	    tm->c_iflag |= INLCR;
	if(*mode & LOCAL_IGNCR)
	    tm->c_iflag |= IGNCR;
	if(*mode & LOCAL_ICRNL)
	    tm->c_iflag |= ICRNL;
	if(*mode & LOCAL_IUCLC)
	    tm->c_iflag |= IUCLC;
	if(*mode & LOCAL_OLCUC)
	    tm->c_oflag |= OLCUC;
	if(*mode & LOCAL_ONLCR)
	    tm->c_oflag |= ONLCR;
	if(*mode & LOCAL_OCRNL)
	    tm->c_oflag |= OCRNL;
	if(*mode & LOCAL_ICANON)
	    tm->c_lflag |= ICANON;
	if(*mode & LOCAL_ECHO)
	    tm->c_lflag |= ECHO;
	ioarg->ioctl_return = SCCGetAttributes(minor,tm);
	break;
    case RTEMS_IO_SET_ATTRIBUTES:
	*mode = 0;
	if(tm->c_iflag & ISTRIP)
	    *mode |= LOCAL_ISTRIP;
	if(tm->c_iflag & INLCR)
	    *mode |= LOCAL_INLCR;
	if(tm->c_iflag & IGNCR)
	    *mode |= LOCAL_IGNCR;
	if(tm->c_iflag & ICRNL)
	    *mode |= LOCAL_ICRNL;
	if(tm->c_iflag & IUCLC)
	    *mode |= LOCAL_IUCLC;
	if(tm->c_oflag & OLCUC)
	    *mode |= LOCAL_OLCUC;
	if(tm->c_oflag & ONLCR)
	    *mode |= LOCAL_ONLCR;
	if(tm->c_oflag & OCRNL)
	    *mode |= LOCAL_OCRNL;
	if(tm->c_lflag & ICANON)
	    *mode |= LOCAL_ICANON;
	if(tm->c_lflag & ECHO)
	    *mode |= LOCAL_ECHO;
	ioarg->ioctl_return = SCCSetAttributes(minor,tm);
	break;
    default:
	return RTEMS_NOT_DEFINED;
    }
    
    return RTEMS_SUCCESSFUL;
}
