/*  scv64.c
 *
 *  This set of routines control the scv64 chip on the DMV177 board.
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

typedef struct {
  /* DARF Registers */
  volatile rtems_unsigned32  DMALAR;             /* 0x00 */       
  volatile rtems_unsigned32  DMAVAR;             /* 0x04 */       
  volatile rtems_unsigned32  DMATC;              /* 0x08 */      
  volatile rtems_unsigned32  DCSR;               /* 0x0c */     
  volatile rtems_unsigned32  VMEBAR;             /* 0x10 */       
  volatile rtems_unsigned32  RXDATA;             /* 0x14 */       
  volatile rtems_unsigned32  RXADDR;             /* 0x18 */                           
  volatile rtems_unsigned32  RXCTL;              /* 0x1c */      
  volatile rtems_unsigned32  BUSSEL;             /* 0x20 */       
  volatile rtems_unsigned32  IVECT;              /* 0x24 */      
  volatile rtems_unsigned32  APBR;               /* 0x28 */     
  volatile rtems_unsigned32  TXDATA;             /* 0x2c */       
  volatile rtems_unsigned32  TXADDR;             /* 0x30 */       
  volatile rtems_unsigned32  TXCTL;              /* 0x34 */      
  volatile rtems_unsigned32  LMFIFO;             /* 0x38 */       
  volatile rtems_unsigned32  MODE;               /* 0x3c */     
  volatile rtems_unsigned32  SA64BAR;            /* 0x40 */        
  volatile rtems_unsigned32  MA64BAR;            /* 0x44 */
  volatile rtems_unsigned32  LAG;                /* 0x48 */
  volatile rtems_unsigned32  DMAVTC;             /* 0x4c */

  /* Reserved */
  volatile rtems_unsigned32  reserved_50_7F[12];

  /* ACC Registers */
  volatile rtems_unsigned8   STAT0_pad[3];       /* 0x80 */       
  volatile rtems_unsigned8   STAT0;                        
  volatile rtems_unsigned8   STAT1_pad[3];       /* 0x84 */       
  volatile rtems_unsigned8   STAT1;                        
  volatile rtems_unsigned8   GENCTL_pad[3];      /* 0x88 */        
  volatile rtems_unsigned8   GENCTL;                        
  volatile rtems_unsigned8   VINT_pad[3];        /* 0x8c */      
  volatile rtems_unsigned8   VINT;                        
  volatile rtems_unsigned8   VREQ_pad[3];        /* 0x90 */      
  volatile rtems_unsigned8   VREQ;                        
  volatile rtems_unsigned8   VARB_pad[3];        /* 0x94 */      
  volatile rtems_unsigned8   VARB;                        
  volatile rtems_unsigned8   ID_pad[3];          /* 0x98 */    
  volatile rtems_unsigned8   ID;                        
  volatile rtems_unsigned8   NA_pad[3];          /* 0x9c */    
  volatile rtems_unsigned8   NA;                        
  volatile rtems_unsigned8   _7IS_pad[3];        /* 0xa0 */      
  volatile rtems_unsigned8   _7IS;                        
  volatile rtems_unsigned8   LIS_pad[3];         /* 0xa4 */     
  volatile rtems_unsigned8   LIS;                        
  volatile rtems_unsigned8   UIE_pad[3];         /* 0xa8 */     
  volatile rtems_unsigned8   UIE;                        
  volatile rtems_unsigned8   LIE_pad[3];         /* 0xac */     
  volatile rtems_unsigned8   LIE;                        
  volatile rtems_unsigned8   VIE_pad[3];         /* 0xb0 */     
  volatile rtems_unsigned8   VIE;                        
  volatile rtems_unsigned8   IC10_pad[3];        /* 0xb4 */      
  volatile rtems_unsigned8   IC10;                        
  volatile rtems_unsigned8   IC32_pad[3];        /* 0xb8 */      
  volatile rtems_unsigned8   IC32;                        
  volatile rtems_unsigned8   IC54_pad[3];        /* 0xbc */      
  volatile rtems_unsigned8   IC54;                        
  /* Utility Registers */
  volatile rtems_unsigned32  MISC;
  volatile rtems_unsigned32  delay_line[3];
  volatile rtems_unsigned32  MBOX0;
  volatile rtems_unsigned32  MBOX1;
  volatile rtems_unsigned32  MBOX2;
  volatile rtems_unsigned32  MBOX3;
} SCV64_Registers;

/*
 * LIE Register
 */
#define LOCAL_INTERRUPT_ENABLE_0  0x01
#define LOCAL_INTERRUPT_ENABLE_1  0x02
#define LOCAL_INTERRUPT_ENABLE_2  0x04
#define LOCAL_INTERRUPT_ENABLE_3  0x08
#define LOCAL_INTERRUPT_ENABLE_4  0x10
#define LOCAL_INTERRUPT_ENABLE_5  0x20

/* 
 * IC54 Register
 */
#define AUTOVECTOR_5  0x80


/*
 * Set the registers pointer to the base address of the SCV64
 */
SCV64_Registers *SCV64 =  (void *)DMV170_SCV64_BASE_ADDRESS;

/*PAGE
 *
 *  SCV64_Initialize
 *
 *  This routine initializes the SCV64.
 */
void SCV64_Initialize() {
  SCV64->LIE = 0;
}
  
/*PAGE
 *
 *  SCV64_Generate_DUART_Interrupts
 *
 *  This sets the SCV64 to generate duart interrupts for
 *  the DMV177 board.
 */
void SCV64_Generate_DUART_Interrupts() {

  rtems_unsigned8 data;
 
  /*
   * Set Local Interrupt 5 enable
   */
  data = SCV64->LIE;
  data |= LOCAL_INTERRUPT_ENABLE_5;
  SCV64->LIE = data;

  /*
   * Set Autovector.
   */
  data = SCV64->IC54;
  data |= AUTOVECTOR_5;
  SCV64->IC54 = data;
}

/*PAGE
 *
 *  SCV64_Get_Interrupt
 *
 *  This routine returns the SCV64 status register.
 */
rtems_unsigned32 SCV64_Get_Interrupt()
{
  rtems_unsigned8 data;
 
  /*
   * Put the LIS data into the lower byte of the result
   */
  data = SCV64->LIS;

  return data;
}

/*PAGE
 *
 * SCV64_Get_Interrupt_Enable
 *
 * This routine returns the interrupt enable mask.
 */
rtems_unsigned32 SCV64_Get_Interrupt_Enable()
{
  /*
   * Return the set of interrupts enabled.
   */
  return SCV64->LIE;
}
