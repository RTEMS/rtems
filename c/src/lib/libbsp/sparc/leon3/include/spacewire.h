/*
 *  Macros used for Spacewire bus 
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __SPACEWIRE_h
#define __SPACEWIRE_h

typedef struct {
  volatile unsigned int ctrl;
  volatile unsigned int status;
  volatile unsigned int nodeaddr;
  volatile unsigned int clockdiv;
  volatile unsigned int destkey;
  volatile unsigned int pad1,pad2,pad3;
  
  volatile unsigned int dma0ctrl; /* 0x20 */
  volatile unsigned int dma0rxmax;
  volatile unsigned int dma0txdesc;
  volatile unsigned int dma0rxdesc;
} LEON3_SPACEWIRE_Regs_Map;

typedef struct {
  volatile unsigned int ctrl;
  volatile unsigned int addr;
} SPACEWIRE_RXBD;

typedef struct {
  volatile unsigned int ctrl;
  volatile unsigned int addr_header;
  volatile unsigned int len;
  volatile unsigned int addr_data;
} SPACEWIRE_TXBD;

#define SPACEWIRE_BDTABLE_SIZE 0x400
#define SPACEWIRE_TXPCK_SIZE 1024
#define SPACEWIRE_RXPCK_SIZE 1024
#define SPACEWIRE_TXBUFS_NR 1
#define SPACEWIRE_RXBUFS_NR 2
#define SPW_BUFMALLOC 1

#define SPW_ALIGN(p,c) ((((unsigned int)(p))+((c)-1))&~((c)-1))

int spacewire_setattibutes(int minor, int nodeaddr, int proto, int dest);

typedef struct {
  unsigned int nodeaddr;
  unsigned int destnodeaddr;
  unsigned int proto;
  unsigned int destkey;
  unsigned int maxfreq;
  unsigned int clkdiv;
  unsigned int rxmaxlen;
  
  unsigned int is_rmap,is_rxunaligned,is_rmapcrc;

  unsigned int txcur,rxcur,rxbufcur;
  unsigned int txbufsize,rxbufsize;
  unsigned int txbufcnt,rxbufcnt;
  char *ptr_rxbuf0,*ptr_txbuf0;
  unsigned int irq;
  
  SPACEWIRE_RXBD *rx;
  SPACEWIRE_TXBD *tx;
  
  char _rxtable[SPACEWIRE_BDTABLE_SIZE*2];
  char _txtable[SPACEWIRE_BDTABLE_SIZE*2];

#ifndef SPW_BUFMALLOC  
  char _rxbuf0[SPACEWIRE_RXPCK_SIZE*SPACEWIRE_RXBUFS_NR];
  char _txbuf0[SPACEWIRE_TXPCK_SIZE*SPACEWIRE_TXBUFS_NR];
#endif  
  
  volatile LEON3_SPACEWIRE_Regs_Map *regs;
} SPACEWIRE_PARAM;

int _SPW_READ(void *addr);
#define SPW_READ(addr) _SPW_READ((void *)(addr))
#define SPW_WRITE(addr,v) *addr=v

#define SPACEWIRE_MAX_CORENR 2
extern SPACEWIRE_PARAM LEON3_Spacewire[SPACEWIRE_MAX_CORENR];  

#define SPW_PARAM(c) (LEON3_Spacewire[c])
#define SPW_REG(c,r) (SPW_PARAM(c).regs->r)
#define SPW_REG_CTRL(c) SPW_REG(c,ctrl)
#define SPW_REG_STATUS(c) SPW_REG(c,status)
#define SPW_REG_NODEADDR(c) SPW_REG(c,nodeaddr)

#define SPW_CTRL_READ(c)      SPW_READ(&SPW_REG_CTRL(c))
#define SPW_CTRL_WRITE(c,v)   SPW_WRITE(&SPW_REG_CTRL(c),v)
#define SPW_STATUS_READ(c)    SPW_READ(&SPW_REG_STATUS(c))
#define SPW_STATUS_WRITE(c,v) SPW_WRITE(&SPW_REG_STATUS(c),v)

#define SPW_LINKSTATE(c) (((c) >> 21) & 0x7)

#define SPW_NODEADDR_READ(c)    SPW_BYPASSCACHE(&SPW_NODEADDR(c))
#define SPW_NODEADDR_WRITE(c,v) SPW_NODEADDR(c) = v

#define SPACEWIRE_RXNR(c) ((c&~(SPACEWIRE_BDTABLE_SIZE-1))>>3)
#define SPACEWIRE_TXNR(c) ((c&~(SPACEWIRE_BDTABLE_SIZE-1))>>4)

#define SPACEWIRE_RXBD_LENGTH 0x1ffffff
#define SPACEWIRE_RXBD_EN (1<<25)
#define SPACEWIRE_RXBD_WR (1<<26)
#define SPACEWIRE_RXBD_IE (1<<27)

#define SPACEWIRE_RXBD_EEP (1<<28)
#define SPACEWIRE_RXBD_EHC (1<<29)
#define SPACEWIRE_RXBD_EDC (1<<30)
#define SPACEWIRE_RXBD_ETR (1<<31)

#define SPACEWIRE_RXBD_ERROR (SPACEWIRE_RXBD_EEP | \
                              SPACEWIRE_RXBD_EHC | \
                              SPACEWIRE_RXBD_EDC | \
                              SPACEWIRE_RXBD_ETR)

#define SPACEWIRE_RXBD_RMAPERROR (SPACEWIRE_RXBD_EHC | SPACEWIRE_RXBD_EDC)

#define SPACEWIRE_RXBD_LENGTH(c) ((c)&0xffffff)
#define SPACEWIRE_PCKHEAD 2

#define SPACEWIRE_TXBD_LENGTH 0xffffff

#define SPACEWIRE_TXBD_EN (1<<12)
#define SPACEWIRE_TXBD_WR (1<<13)
#define SPACEWIRE_TXBD_IE (1<<14)

#define SPACEWIRE_TXBD_LE (1<<15)

#define SPACEWIRE_TXBD_ERROR (SPACEWIRE_TXBD_LE)

#define SPACEWIRE_CTRL_RA   (1<<31)
#define SPACEWIRE_CTRL_RX   (1<<30)
#define SPACEWIRE_CTRL_RC   (1<<29)

#define SPACEWIRE_CTRL_RESET         (1<<6)
#define SPACEWIRE_CTRL_LINKSTART     (1<<1)
#define SPACEWIRE_CTRL_LINKDISABLE   (1<<0)

#define SPACEWIRE_DMACTRL_TXEN (1<<0)
#define SPACEWIRE_DMACTRL_RXEN (1<<1)
#define SPACEWIRE_DMACTRL_TXIE (1<<2)
#define SPACEWIRE_DMACTRL_RXIE (1<<3)

#define SPACEWIRE_DMACTRL_AI (1<<4)
#define SPACEWIRE_DMACTRL_PS (1<<5)
#define SPACEWIRE_DMACTRL_PR (1<<6)
#define SPACEWIRE_DMACTRL_TA (1<<7)
#define SPACEWIRE_DMACTRL_RA (1<<8)

#define SPACEWIRE_DMACTRL_RD (1<<11)
#define SPACEWIRE_DMACTRL_NS (1<<12)

#define SPACEWIRE_PREPAREMASK_TX (SPACEWIRE_DMACTRL_RXEN | SPACEWIRE_DMACTRL_RXIE | SPACEWIRE_DMACTRL_PS | SPACEWIRE_DMACTRL_TA | SPACEWIRE_DMACTRL_RD)
#define SPACEWIRE_PREPAREMASK_RX (SPACEWIRE_DMACTRL_TXEN | SPACEWIRE_DMACTRL_TXIE | SPACEWIRE_DMACTRL_AI | SPACEWIRE_DMACTRL_PR | SPACEWIRE_DMACTRL_RA)


#define SPACEWIRE_IOCTRL_SET_NODEADDR     1
#define SPACEWIRE_IOCTRL_SET_PROTOCOL     2
#define SPACEWIRE_IOCTRL_SET_DESTNODEADDR 3
#define SPACEWIRE_IOCTRL_GET_COREBASEADDR 4
#define SPACEWIRE_IOCTRL_GET_COREIRQ      5
#define SPACEWIRE_IOCTRL_SET_PACKETSIZE   6
#define SPACEWIRE_IOCTRL_GETPACKET        7
#define SPACEWIRE_IOCTRL_PUTPACKET        8

typedef struct {
  unsigned int txsize, rxsize;
} spw_ioctl_packetsize;

typedef struct {
  char *buf;
  int buf_size;
  int ret_size;
} spw_ioctl_packet;

#endif
