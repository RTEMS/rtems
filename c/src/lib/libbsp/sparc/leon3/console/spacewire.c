/*
 *  This file contains the TTY driver for the spacewire port on the LEON.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2005.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  spacewire.c,v 1.1.2.1 2005/11/02 19:25:59 jiri Exp
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sched.h>
#include <ctype.h>
#include <rtems/bspIo.h>

#define DBGSPW_IOCALLS 1
#define DBGSPW_TX 2
#define DBGSPW_RX 4
#define DBGSPW_IOCTRL 8
#define DBGSPW_DUMP 16
#define DEBUG_SPACEWIRE_FLAGS -1

/*#define DEBUG_SPACEWIRE_ONOFF*/
 
#ifdef DEBUG_SPACEWIRE_ONOFF
int DEBUG_printf(const char *fmt, ...);
#define SPACEWIRE_DBG(fmt, args...) \
  do { \
   { printf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); } \
  } while(0)
#define SPACEWIRE_DBG2(fmt) \
 do { \
   { printf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); } \
 } while(0)
#define SPACEWIRE_DBGC(c,fmt, args...) \
  do { \
    if (DEBUG_SPACEWIRE_FLAGS&c) { \
      printf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); \
    } \
  } while(0)
#else
#define SPACEWIRE_DBG(fmt, args...)
#define SPACEWIRE_DBG2(fmt, args...)
#define SPACEWIRE_DBGC(fmt, args...)
#endif

int spacewire_hw_init(int minor);
void spacewire_hw_send(int minor,unsigned char *b,int c);
int spacewire_hw_receive(int minor,unsigned char *b,int c);
int spacewire_hw_startup (int minor);
int spacewire_hw_stop (int minor);
void spacewire_hw_waitlink (int minor);
void spacewire_rxnext(int minor);

int _SPW_READ(void *addr) {
        int tmp;        
        asm(" lda [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
}

rtems_device_driver spacewire_console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);
rtems_device_driver spacewire_console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
);
rtems_device_driver spacewire_console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
);
rtems_device_driver spacewire_console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
);
rtems_device_driver spacewire_console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
);
rtems_device_driver spacewire_console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
);

#define SPWCEWIRE_CONSOLE_DRIVER_TABLE_ENTRY \
  { spacewire_console_initialize, spacewire_console_open, \
    spacewire_console_close, spacewire_console_read, \
    spacewire_console_write, spacewire_console_control }

static rtems_driver_address_table 
  spacewire_driver = SPWCEWIRE_CONSOLE_DRIVER_TABLE_ENTRY;

void spacewire_register() 
{
  rtems_status_code r;
  rtems_device_major_number m;

  SPACEWIRE_DBG2("register driver\n");
  if ((r = rtems_io_register_driver(
				    0,
				    &spacewire_driver,
				    &m)) == RTEMS_SUCCESSFUL) {
    SPACEWIRE_DBG2("success\n");
  } else {
    switch(r) {
    case RTEMS_TOO_MANY:
      SPACEWIRE_DBG2("failed RTEMS_TOO_MANY\n"); break;
    case RTEMS_INVALID_NUMBER:
      SPACEWIRE_DBG2("failed RTEMS_INVALID_NUMBER\n"); break;
    case RTEMS_RESOURCE_IN_USE:
      SPACEWIRE_DBG2("failed RTEMS_RESOURCE_IN_USE\n"); break;
    default:
      SPACEWIRE_DBG("failed %i\n",r); break;
    }
  }
}

#ifdef SPW_BUFMALLOC  
void spacewire_buffer_alloc(int minor) {
  if (SPW_PARAM(minor).ptr_rxbuf0) {
    free(SPW_PARAM(minor).ptr_rxbuf0);
  }
  if (SPW_PARAM(minor).ptr_txbuf0) {
    free(SPW_PARAM(minor).ptr_txbuf0);
  }
  SPW_PARAM(minor).ptr_rxbuf0 = 
    (char *) malloc(SPW_PARAM(minor).rxbufsize * SPW_PARAM(minor).rxbufcnt);
  SPW_PARAM(minor).ptr_txbuf0 = 
    (char *) malloc(SPW_PARAM(minor).txbufsize * SPW_PARAM(minor).txbufcnt);
}
#endif

/*
 *  Console Device Driver Entry Points
 *
 */

SPACEWIRE_PARAM LEON3_Spacewire[SPACEWIRE_MAX_CORENR];  

rtems_device_driver spacewire_console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  unsigned int iobar, conf;
  int i, uarts;
  char *console_name = "/dev/spacewire_a";
  SPACEWIRE_DBG2("spacewire driver inizialisation\n");

  rtems_termios_initialize();

  /* Find spacewire cores */
  i = 0; uarts = 0;
  while (i < amba_conf.apbslv.devnr && uarts < SPACEWIRE_MAX_CORENR) 
  {
    conf = amba_get_confword(amba_conf.apbslv, i, 0);
    if ((amba_vendor(conf) == VENDOR_GAISLER) && 
        (amba_device(conf) == GAISLER_SPACEWIRE))
    {
      iobar = amba_apb_get_membar(amba_conf.apbslv, i);      
      LEON3_Spacewire[uarts].regs = 
        (LEON3_SPACEWIRE_Regs_Map *) amba_iobar_start(amba_conf.apbmst, iobar);
      LEON3_Spacewire[uarts].irq = amba_irq(conf);

      SPACEWIRE_DBG("spacewire code at [0x%x]\n",
           (unsigned int)LEON3_Spacewire[uarts].regs);
      
      /* initialize the code with some resonable values,
	 actual initialization is done later using ioctl(fd)
         on the opened device */
      LEON3_Spacewire[uarts].nodeaddr = 0x14;
      LEON3_Spacewire[uarts].destkey = 0xBF;
      LEON3_Spacewire[uarts].maxfreq = 1;
      LEON3_Spacewire[uarts].clkdiv = 0;
      LEON3_Spacewire[uarts].rxmaxlen = SPACEWIRE_RXPCK_SIZE;
      LEON3_Spacewire[uarts].txbufsize = SPACEWIRE_TXPCK_SIZE;
      LEON3_Spacewire[uarts].rxbufsize = SPACEWIRE_RXPCK_SIZE;
      LEON3_Spacewire[uarts].txbufcnt = SPACEWIRE_TXBUFS_NR;
      LEON3_Spacewire[uarts].rxbufcnt = SPACEWIRE_RXBUFS_NR;

#ifndef SPW_BUFMALLOC  
      LEON3_Spacewire[uarts].ptr_rxbuf0 = &LEON3_Spacewire[uarts]._rxbuf0;
      LEON3_Spacewire[uarts].ptr_txbuf0 = &LEON3_Spacewire[uarts]._txbuf0;
#else
      LEON3_Spacewire[uarts].ptr_rxbuf0 = 0;
      LEON3_Spacewire[uarts].ptr_txbuf0 = 0;
      spacewire_buffer_alloc(uarts);
#endif      
      uarts++;
    }
    i++;
  }
  
  /*  Register Device Names, /dev/spacewire, /dev/spacewire_b  ... */
  if (uarts) {  
    SPACEWIRE_DBG2("registering minor 0 as /dev/spacewire\n");
    status = rtems_io_register_name( "/dev/spacewire", major, 0 );
    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
    for (i = 1; i < uarts; i++) {
      console_name[15]++;
      SPACEWIRE_DBG("registering minor %i as %s\n",i,console_name);
      status = rtems_io_register_name( console_name, major, i);
    }
  }
  
  /* Initialize Hardware */
  for (i = 0; i < uarts; i++) {
    spacewire_hw_init(i);
  }
  
  return RTEMS_SUCCESSFUL;
}

int spacewire_setattibutes(int minor, int nodeaddr, int proto, int dest) {
  if ( minor >= SPACEWIRE_MAX_CORENR ) {
    printf("minor %i too big\n",minor);
    return RTEMS_INVALID_NUMBER;
  }
  SPW_PARAM(minor).nodeaddr = nodeaddr;
  SPW_PARAM(minor).proto = proto;
  SPW_PARAM(minor).destnodeaddr = dest;
  /*set node address*/
  SPW_WRITE(&SPW_REG(minor,nodeaddr),SPW_PARAM(minor).nodeaddr);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver spacewire_console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
){
  SPACEWIRE_DBGC(DBGSPW_IOCALLS,"open [%i,%i]\n",major, minor);
  if ( minor >= SPACEWIRE_MAX_CORENR ) {
    SPACEWIRE_DBG("minor %i too big\n",minor);
    return RTEMS_INVALID_NUMBER;
  }
  return spacewire_hw_startup(minor);
}
 
rtems_device_driver spacewire_console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  SPACEWIRE_DBGC(DBGSPW_IOCALLS,"close [%i,%i]\n",major, minor);
  spacewire_hw_stop(minor);
  return RTEMS_SUCCESSFUL;
}
 
rtems_device_driver spacewire_console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  unsigned32 count = 0;
  rw_args = (rtems_libio_rw_args_t *) arg;

  SPACEWIRE_DBGC(DBGSPW_IOCALLS,"read  [%i,%i]: buf:0x%x len:%i \n",
       major, minor, (unsigned int)rw_args->buffer,rw_args->count);
  
  count =  spacewire_hw_receive(minor,rw_args->buffer,rw_args->count);

#ifdef DEBUG_SPACEWIRE_ONOFF  
  if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
    int k;
    for (k = 0;k < count;k++){
      if (k % 16 == 0) {
	printf ("\n");
      }
      printf ("%.2x(%c) ",rw_args->buffer[k] & 0xff,
         isprint(rw_args->buffer[k] & 0xff) ? rw_args->buffer[k] & 0xff : ' ');
    }
    printf ("\n");
  }
#endif
  
  rw_args->bytes_moved = count;
  return RTEMS_SUCCESSFUL;
  
}
 
rtems_device_driver spacewire_console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  int count = 0;
  rw_args = (rtems_libio_rw_args_t *) arg;
  SPACEWIRE_DBGC(DBGSPW_IOCALLS,"write [%i,%i]: buf:0x%x len:%i\n",
         major, minor, (unsigned int)rw_args->buffer,rw_args->count);
  
  while (rw_args->count > 0) {
    int c = rw_args->count;
    if (c > SPW_PARAM(minor).txbufsize-2) {
      c = SPW_PARAM(minor).txbufsize-2;
    }
    spacewire_hw_send(minor,rw_args->buffer,c);
    rw_args->count -= c;
    rw_args->buffer += c;
    count += c;
  }
  
  if (count >= 0) {
    rw_args->bytes_moved = count;
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}

rtems_device_driver spacewire_console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_ioctl_args_t	*ioarg = (rtems_libio_ioctl_args_t *)arg;
  SPACEWIRE_DBGC(DBGSPW_IOCALLS,"ctrl [%i,%i]\n",major, minor);
  
  if (!ioarg)
    return RTEMS_INVALID_ADDRESS;

  switch(ioarg->command) {
  case SPACEWIRE_IOCTRL_SET_NODEADDR:
    SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_NODEADDR %i\n",
          ioarg->buffer);
    SPW_PARAM(minor).nodeaddr = (unsigned int) ioarg->buffer;
    /*set node address*/
    SPW_WRITE(&SPW_REG(minor,nodeaddr),SPW_PARAM(minor).nodeaddr);
    break;
  case SPACEWIRE_IOCTRL_SET_PROTOCOL:
    SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_PROTOCOL %i\n",
      ioarg->buffer);
    SPW_PARAM(minor).proto = (unsigned int)  ioarg->buffer;
    break;
  case SPACEWIRE_IOCTRL_SET_DESTNODEADDR:
    SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_DESTNODEADDR %i\n",
      ioarg->buffer);
    SPW_PARAM(minor).destnodeaddr = (unsigned int) ioarg->buffer;
    break;
  case SPACEWIRE_IOCTRL_GET_COREBASEADDR:
    SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_GET_BASEADDR=%i\n",
      (unsigned int)SPW_PARAM(minor).regs);
    *(unsigned int *)ioarg->buffer = (unsigned int )SPW_PARAM(minor).regs;
    break;
  case SPACEWIRE_IOCTRL_GET_COREIRQ:
    SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_GET_COREIRQ=%i\n",
      (unsigned int)SPW_PARAM(minor).irq);
    *(unsigned int *)ioarg->buffer = (unsigned int )SPW_PARAM(minor).irq;
    break;
#ifdef SPW_BUFMALLOC
  case SPACEWIRE_IOCTRL_SET_PACKETSIZE:
    {
      spw_ioctl_packetsize *ps = (spw_ioctl_packetsize*)ioarg->buffer;
      SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RXPACKETSIZE \n",
          ioarg->buffer);
      spacewire_hw_stop(minor);
      SPW_PARAM(minor).rxbufsize = ps->rxsize;
      SPW_PARAM(minor).txbufsize = ps->txsize;
      SPW_PARAM(minor).rxmaxlen = SPW_PARAM(minor).rxbufsize;
      spacewire_buffer_alloc(minor);
      spacewire_hw_startup(minor);
    }
    break;
#endif
  case SPACEWIRE_IOCTRL_GETPACKET: {
      spw_ioctl_packet *p = (spw_ioctl_packet*)ioarg->buffer;
      p->ret_size = spacewire_hw_receive(minor,p->buf,p->buf_size);
    }
    break;
  case SPACEWIRE_IOCTRL_PUTPACKET:{
      spw_ioctl_packet *p = (spw_ioctl_packet*)ioarg->buffer;
      spacewire_hw_send(minor,p->buf,p->buf_size);
    }
    break;
  default:
    return RTEMS_NOT_DEFINED;
  }
  return RTEMS_SUCCESSFUL;
}


/* ========================================================================== */

int spacewire_set_rxmaxlen(int minor) {
  unsigned int rxmax;
  /*set rx maxlength*/
  SPW_WRITE(&SPW_REG(minor,dma0rxmax),SPW_PARAM(minor).rxmaxlen);
  rxmax = SPW_READ(&SPW_REG(minor,dma0rxmax));
  if (rxmax != SPW_PARAM(minor).rxmaxlen) {
    printf("spacewire: error initializing rx max len (0x%x, read: 0x%x)\n",
        SPW_PARAM(minor).rxmaxlen,rxmax);
    return 0;
  }
  return 1;
}

int spacewire_hw_init(int minor) {
  unsigned int dma0ctrl;
  int ctrl = SPW_CTRL_READ(minor);

  SPW_PARAM(minor).rx = (SPACEWIRE_RXBD *)
    SPW_ALIGN(&SPW_PARAM(minor)._rxtable,SPACEWIRE_BDTABLE_SIZE);
  SPW_PARAM(minor).tx = (SPACEWIRE_TXBD *)
    SPW_ALIGN(&SPW_PARAM(minor)._txtable,SPACEWIRE_BDTABLE_SIZE);
  
  SPACEWIRE_DBG("hw_init [minor %i]\n", minor);

  SPW_PARAM(minor).is_rmap = ctrl & SPACEWIRE_CTRL_RA;
  SPW_PARAM(minor).is_rxunaligned = ctrl & SPACEWIRE_CTRL_RX;
  SPW_PARAM(minor).is_rmapcrc = ctrl & SPACEWIRE_CTRL_RC;
  
  /*set node address*/
  SPW_WRITE(&SPW_REG(minor,nodeaddr),SPW_PARAM(minor).nodeaddr);
  
  /*set clock divisor*/
  SPW_WRITE(&SPW_REG(minor,clockdiv),SPW_PARAM(minor).clkdiv);
  if (SPW_PARAM(minor).is_rmap) {
    SPW_WRITE(&SPW_REG(minor,destkey),SPW_PARAM(minor).destkey);  
  }
  SPW_WRITE(
    &SPW_REG(minor,dma0ctrl),
    /*clear status, set ctrl for dma chan*/
    SPACEWIRE_DMACTRL_PS|SPACEWIRE_DMACTRL_PR|
      SPACEWIRE_DMACTRL_TA|SPACEWIRE_DMACTRL_RA
  );
  if ((dma0ctrl = SPW_READ(&SPW_REG(minor,dma0ctrl))) != 0) {
    printf("spacewire: error initializing dma ctrl (0x%x)\n",dma0ctrl);
  }
  
  SPW_CTRL_WRITE(minor, SPACEWIRE_CTRL_RESET ); /*set ctrl*/
  SPW_CTRL_WRITE(minor,
     SPACEWIRE_CTRL_LINKSTART | (SPW_PARAM(minor).maxfreq << 5)); /*set ctrl*/
  return 0;
}

void spacewire_hw_waitlink (int minor) {
  int j;
  while (SPW_LINKSTATE(SPW_STATUS_READ(minor)) != 5) {
    rtems_task_wake_after(100);
    sched_yield();
    printf("timeout loop dev %i\n",minor);
    j++;
  }
}

int spacewire_hw_startup (int minor) {
  int i,j;
  unsigned int dmactrl;
  SPW_STATUS_WRITE(minor,0xFF); /*clear status*/
  SPW_CTRL_WRITE(minor, SPACEWIRE_CTRL_RESET ); /*set ctrl*/
  SPW_CTRL_WRITE(minor,
     SPACEWIRE_CTRL_LINKSTART | (SPW_PARAM(minor).maxfreq << 5)); /*set ctrl*/
  sched_yield();
  j = 0; i = 0;
  spacewire_hw_waitlink(minor);
  
  /* prepare transmit buffers */
  for (i = 0; i < SPW_PARAM(minor).txbufcnt;i++) {
    if (i+1 == SPW_PARAM(minor).txbufcnt) {
      SPW_PARAM(minor).tx[i].ctrl = 0|SPACEWIRE_TXBD_WR;
    } else {
      SPW_PARAM(minor).tx[i].ctrl = 0;
    }
    SPW_PARAM(minor).tx[i].addr_data =
     ((unsigned int)&SPW_PARAM(minor).ptr_txbuf0[0]) +
         (i * SPW_PARAM(minor).txbufsize);
  }
  SPW_PARAM(minor).txcur = 0;
  
  /* prepare receive buffers */
  for (i = 0; i < SPW_PARAM(minor).rxbufcnt;i++) {
    if (i+1 == SPW_PARAM(minor).rxbufcnt) {
      SPW_PARAM(minor).rx[i].ctrl = SPACEWIRE_RXBD_EN|SPACEWIRE_RXBD_WR;
    } else {
      SPW_PARAM(minor).rx[i].ctrl = SPACEWIRE_RXBD_EN;
    }
    SPW_PARAM(minor).rx[i].addr =
       ((unsigned int)&SPW_PARAM(minor).ptr_rxbuf0[0]) +
       (i * SPW_PARAM(minor).rxbufsize);
  }
  SPW_PARAM(minor).rxcur = 0;
  SPW_PARAM(minor).rxbufcur = -1;
  spacewire_set_rxmaxlen(minor);
  
  SPW_WRITE(&SPW_REG(minor,dma0txdesc),(unsigned int)SPW_PARAM(minor).tx); 
  SPW_WRITE(&SPW_REG(minor,dma0rxdesc),(unsigned int)SPW_PARAM(minor).rx); 
  
  /* start RX */
  dmactrl = SPW_READ(&SPW_REG(minor,dma0ctrl));
  SPW_WRITE(&SPW_REG(minor,dma0ctrl),
	    (dmactrl & SPACEWIRE_PREPAREMASK_RX) |
               SPACEWIRE_DMACTRL_RD | SPACEWIRE_DMACTRL_RXEN);

  SPACEWIRE_DBGC(DBGSPW_TX,"0x%x: setup complete\n",SPW_PARAM(minor).regs);
  return RTEMS_SUCCESSFUL;
}

int spacewire_hw_stop (int minor) {
  unsigned int dmactrl;
  
  /* stop RX */
  dmactrl = SPW_READ(&SPW_REG(minor,dma0ctrl));
  SPW_WRITE(&SPW_REG(minor,dma0ctrl),
	    (dmactrl & SPACEWIRE_PREPAREMASK_RX) &
               ~(SPACEWIRE_DMACTRL_RD |SPACEWIRE_DMACTRL_RXEN));

  /* stop link */
  SPW_CTRL_WRITE(minor,
      (SPW_CTRL_READ(minor) & 0x3FD) | SPACEWIRE_CTRL_LINKDISABLE);
  
  return RTEMS_SUCCESSFUL;
}

void spacewire_hw_send(int minor,unsigned char *b,int c) {

  unsigned int dmactrl, ctrl;
  unsigned int cur = SPW_PARAM(minor).txcur;
  char *txb = SPW_PARAM(minor).ptr_txbuf0 + (cur * SPW_PARAM(minor).txbufsize);

  sched_yield();
  memcpy(&txb[2],b,c);
  
  txb[0] = SPW_PARAM(minor).destnodeaddr;
  txb[1] = SPW_PARAM(minor).proto;

#ifdef DEBUG_SPACEWIRE_ONOFF  
  if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
    int k;
    for (k = 0;k < c+2;k++){
      if (k % 16 == 0) {
	printf ("\n");
      }
      printf ("%.2x(%c) ",txb[k] & 0xff,
        isprint(txb[k] & 0xff) ? txb[k] & 0xff : ' ');
    }
    printf ("\n");
  }
#endif
  
  do {
    SPW_PARAM(minor).tx[0].addr_header = 0;
    SPW_PARAM(minor).tx[0].len = c+2;
    SPW_PARAM(minor).tx[0].addr_data = (unsigned int)txb;
    SPW_PARAM(minor).tx[0].ctrl = SPACEWIRE_TXBD_WR|SPACEWIRE_TXBD_EN;
    
    dmactrl = SPW_READ(&SPW_REG(minor,dma0ctrl));
    SPW_WRITE(&SPW_REG(minor,dma0ctrl),
	      (dmactrl & SPACEWIRE_PREPAREMASK_TX) | SPACEWIRE_DMACTRL_TXEN);
    
    while((ctrl = SPW_READ((volatile void *)&SPW_PARAM(minor).tx[0].ctrl)) &
               SPACEWIRE_TXBD_EN) {
      sched_yield();
    }
    if (ctrl & SPACEWIRE_TXBD_LE) {
      printf("tx error: SPACEWIRE_TXBD_LE, link error\n");
    }
  } while(ctrl & SPACEWIRE_TXBD_ERROR);
  
  SPACEWIRE_DBGC(DBGSPW_TX,"0x%x: transmitted <%i> bytes\n",
    SPW_PARAM(minor).regs,c+2);
}

int spacewire_hw_receive(int minor,unsigned char *b,int c) {
  unsigned int count = 0, len, rxlen, ctrl;
  unsigned int cur = SPW_PARAM(minor).rxcur;
  char *rxb = SPW_PARAM(minor).ptr_rxbuf0 + (cur * SPW_PARAM(minor).rxbufsize);

  SPACEWIRE_DBGC(DBGSPW_RX,"0x%x: waitin packet at pos %i\n",
    SPW_PARAM(minor).regs,cur);
  
  sched_yield();
  
  while(1) {
    ctrl = SPW_READ((volatile void *)&SPW_PARAM(minor).rx[cur].ctrl);
    if (!(ctrl & SPACEWIRE_RXBD_EN)) {
      break;
    }
    if (SPW_LINKSTATE(SPW_STATUS_READ(minor)) != 5) {
      return 0;
    }
    sched_yield();
  }
  SPACEWIRE_DBGC(DBGSPW_RX,"checking packet\n",0);
  
  len = SPACEWIRE_RXBD_LENGTH(ctrl);
  if (!(ctrl & (SPACEWIRE_RXBD_ERROR & ~SPACEWIRE_RXBD_RMAPERROR))) {
  
    if (SPW_PARAM(minor).rxbufcur == -1) {
      SPACEWIRE_DBGC(DBGSPW_RX,"incoming packet len %i\n",len);
      SPW_PARAM(minor).rxbufcur = 2;
    }
    rxlen = len - SPW_PARAM(minor).rxbufcur;
    if (rxlen > c) {
      rxlen = c;
    }
    memcpy(b,rxb+SPW_PARAM(minor).rxbufcur,rxlen);
    count += rxlen;
    b += rxlen;
    c -= rxlen;
    SPW_PARAM(minor).rxbufcur += rxlen;
  } else {
    if (ctrl & SPACEWIRE_RXBD_EEP) {
      printf("rx error: SPACEWIRE_RXBD_EEP, error end of packet\n");
    }
    if (ctrl & SPACEWIRE_RXBD_EHC) {
      printf("rx error: SPACEWIRE_RXBD_EHC, header crc error\n");
    }  
    if (ctrl & SPACEWIRE_RXBD_EDC) {
      printf("rx error: SPACEWIRE_RXBD_EDC, crc error\n");
    }
    if (ctrl & SPACEWIRE_RXBD_ETR) {
      printf("rx error: SPACEWIRE_RXBD_ETR, truncated\n");
    }
  }
  
  if (SPW_PARAM(minor).rxbufcur == len ||
      (ctrl & (SPACEWIRE_RXBD_ERROR & ~SPACEWIRE_RXBD_RMAPERROR))) {
    spacewire_rxnext(minor);
  }
  return count;
}

void spacewire_rxnext(int minor) {
  
  unsigned int dmactrl;
  unsigned int cur = SPW_PARAM(minor).rxcur;
  unsigned int ctrl = SPW_READ((volatile void *)&SPW_PARAM(minor).rx[cur].ctrl);
  int cur2 = cur;
  cur++;
  if (cur >= SPACEWIRE_RXBUFS_NR) {
    cur = 0;
    ctrl |= SPACEWIRE_RXBD_WR;
  }
  SPW_PARAM(minor).rx[cur2].ctrl = ctrl | SPACEWIRE_RXBD_EN;
  
  SPW_PARAM(minor).rxcur = cur;
  SPW_PARAM(minor).rxbufcur = -1;
  
  /* start RX */
  dmactrl = SPW_READ(&SPW_REG(minor,dma0ctrl));
  SPW_WRITE(&SPW_REG(minor,dma0ctrl),
	    (dmactrl & SPACEWIRE_PREPAREMASK_RX) | SPACEWIRE_DMACTRL_RD |
               SPACEWIRE_DMACTRL_RXEN);
  
}
