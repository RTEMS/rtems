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
  volatile uint32_t    DMALAR;             /* 0x00 */
  volatile uint32_t    DMAVAR;             /* 0x04 */
  volatile uint32_t    DMATC;              /* 0x08 */
  volatile uint32_t    DCSR;               /* 0x0c */
  volatile uint32_t    VMEBAR;             /* 0x10 */
  volatile uint32_t    RXDATA;             /* 0x14 */
  volatile uint32_t    RXADDR;             /* 0x18 */
  volatile uint32_t    RXCTL;              /* 0x1c */
  volatile uint32_t    BUSSEL;             /* 0x20 */
  volatile uint32_t    IVECT;              /* 0x24 */
  volatile uint32_t    APBR;               /* 0x28 */
  volatile uint32_t    TXDATA;             /* 0x2c */
  volatile uint32_t    TXADDR;             /* 0x30 */
  volatile uint32_t    TXCTL;              /* 0x34 */
  volatile uint32_t    LMFIFO;             /* 0x38 */
  volatile uint32_t    MODE;               /* 0x3c */
  volatile uint32_t    SA64BAR;            /* 0x40 */
  volatile uint32_t    MA64BAR;            /* 0x44 */
  volatile uint32_t    LAG;                /* 0x48 */
  volatile uint32_t    DMAVTC;             /* 0x4c */

  /* Reserved */
  volatile uint32_t    reserved_50_7F[12];

  /* ACC Registers */
  volatile uint8_t     STAT0_pad[3];       /* 0x80 */
  volatile uint8_t     STAT0;
  volatile uint8_t     STAT1_pad[3];       /* 0x84 */
  volatile uint8_t     STAT1;
  volatile uint8_t     GENCTL_pad[3];      /* 0x88 */
  volatile uint8_t     GENCTL;
  volatile uint8_t     VINT_pad[3];        /* 0x8c */
  volatile uint8_t     VINT;
  volatile uint8_t     VREQ_pad[3];        /* 0x90 */
  volatile uint8_t     VREQ;
  volatile uint8_t     VARB_pad[3];        /* 0x94 */
  volatile uint8_t     VARB;
  volatile uint8_t     ID_pad[3];          /* 0x98 */
  volatile uint8_t     ID;
  volatile uint8_t     NA_pad[3];          /* 0x9c */
  volatile uint8_t     NA;
  volatile uint8_t     _7IS_pad[3];        /* 0xa0 */
  volatile uint8_t     _7IS;
  volatile uint8_t     LIS_pad[3];         /* 0xa4 */
  volatile uint8_t     LIS;
  volatile uint8_t     UIE_pad[3];         /* 0xa8 */
  volatile uint8_t     UIE;
  volatile uint8_t     LIE_pad[3];         /* 0xac */
  volatile uint8_t     LIE;
  volatile uint8_t     VIE_pad[3];         /* 0xb0 */
  volatile uint8_t     VIE;
  volatile uint8_t     IC10_pad[3];        /* 0xb4 */
  volatile uint8_t     IC10;
  volatile uint8_t     IC32_pad[3];        /* 0xb8 */
  volatile uint8_t     IC32;
  volatile uint8_t     IC54_pad[3];        /* 0xbc */
  volatile uint8_t     IC54;
  /* Utility Registers */
  volatile uint32_t    MISC;
  volatile uint32_t    delay_line[3];
  volatile uint32_t    MBOX0;
  volatile uint32_t    MBOX1;
  volatile uint32_t    MBOX2;
  volatile uint32_t    MBOX3;
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

  uint8_t   data;

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
uint32_t   SCV64_Get_Interrupt()
{
  uint8_t   data;

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
uint32_t   SCV64_Get_Interrupt_Enable()
{
  /*
   * Return the set of interrupts enabled.
   */
  return SCV64->LIE;
}
