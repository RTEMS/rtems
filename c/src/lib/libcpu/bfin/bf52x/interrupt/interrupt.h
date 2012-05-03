/**
 *@file interrupt.h
 *
 *@brief
 *  - This file implements interrupt dispatcher. The init code is taken from
 *  the 533 implementation for blackfin. Since 52X supports 56 line and 2 ISR
 *  registers some portion is written twice.
 *
 * Target:   TLL6527v1-0
 * Compiler:
 *
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license
 *
 * @author Rohan Kangralkar, ECE, Northeastern University
 *         (kangralkar.r@husky.neu.edu)
 *
 * LastChange:
 */

#ifndef _BFIN_INTERRUPT_H_
#define _BFIN_INTERRUPT_H_


#ifdef __cplusplus
extern "C" {
#endif

/** The type of interrupts handled by the SIC
 */
typedef enum {
    IRQ_PLL_WAKEUP_INTERRUPT,                 /* 0 */
    IRQ_DMA_ERROR_0,                          /* 1 */
    IRQ_DMAR0_BLOCK_INTERRUPT,                /* 2 */
    IRQ_DMAR1_BLOCK_INTERRUPT,                /* 3 */
    IRQ_DMAR0_OVERFLOW_ERROR,                 /* 4 */
    IRQ_DMAR1_OVERFLOW_ERROR,                 /* 5 */
    IRQ_PPI_STATUS,                           /* 6 */
    IRQ_MAC_STATUS,                           /* 7 */
    IRQ_SPORT0_STATUS,                        /* 8 */
    IRQ_SPORT1_STATUS,                        /* 9 */
    IRQ_RESERVED_10,                          /* 10 */
    IRQ_RESERVED_11,                          /* 11 */
    IRQ_UART0_STATUS,                         /* 12 */
    IRQ_UART1_STATUS,                         /* 13 */
    IRQ_REAL_TIME_CLOCK,                      /* 14 */
    IRQ_DMA0_PPI_NFC,                         /* 15 */
    IRQ_DMA3_SPORT0_RX,                       /* 16 */
    IRQ_DMA4_SPORT0_TX,                       /* 17 */
    IRQ_DMA5_SPORT1_RX,                       /* 18 */
    IRQ_DMA6_SPORT1_TX,                       /* 19 */
    IRQ_TWI_INTERRUPT,                        /* 20 */
    IRQ_DMA7_SPI,                             /* 21 */
    IRQ_DMA8_UART0_RX,                        /* 22 */
    IRQ_DMA9_UART0_TX,                        /* 23 */
    IRQ_DMA10_UART1_RX,                       /* 24 */
    IRQ_DMA11_UART1_TX,                       /* 25 */
    IRQ_OTP,                                  /* 26 */
    IRQ_GP_COUNTER,                           /* 27 */
    IRQ_DMA1_MAC_RX_HOSTDP,                   /* 28 */
    IRQ_PORT_H_INTERRUPT_A,                   /* 29 */
    IRQ_DMA2_MAC_TX_NFC,                      /* 30 */
    IRQ_PORT_H_INTERRUPT_B,                   /* 31 */
    SIC_ISR0_MAX,                             /* 32 ***/
    IRQ_TIMER0 = SIC_ISR0_MAX,                /* 32 */
    IRQ_TIMER1,                               /* 33 */
    IRQ_TIMER2,                               /* 34 */
    IRQ_TIMER3,                               /* 35 */
    IRQ_TIMER4,                               /* 36 */
    IRQ_TIMER5,                               /* 37 */
    IRQ_TIMER6,                               /* 38 */
    IRQ_TIMER7,                               /* 39 */
    IRQ_PORT_G_INTERRUPT_A,                   /* 40 */
    IRQ_PORT_G_INTERRUPT_B,                   /* 41 */
    IRQ_MDMA0_STREAM_0_INTERRUPT,             /* 42 */
    IRQ_MDMA1_STREAM_0_INTERRUPT,             /* 43 */
    IRQ_SOFTWARE_WATCHDOG_INTERRUPT,          /* 44 */
    IRQ_PORT_F_INTERRUPT_A,                   /* 45 */
    IRQ_PORT_F_INTERRUPT_B,                   /* 46 */
    IRQ_SPI_STATUS,                           /* 47 */
    IRQ_NFC_STATUS,                           /* 48 */
    IRQ_HOSTDP_STATUS,                        /* 49 */
    IRQ_HOREAD_DONE_INTERRUPT,                /* 50 */
    IRQ_RESERVED_19,                          /* 51 */
    IRQ_USB_INT0_INTERRUPT,                   /* 52 */
    IRQ_USB_INT1_INTERRUPT,                   /* 53 */
    IRQ_USB_INT2_INTERRUPT,                   /* 54 */
    IRQ_USB_DMAINT,                           /* 55 */
    IRQ_MAX,                                  /* 56 */
} e_isr_t;




/* source is the source to the SIC (the bit number in SIC_ISR).  isr is
   the function that will be called when the interrupt is active. */
typedef struct bfin_isr_s {
#if INTERRUPT_USE_TABLE
  e_isr_t source;
  void (*pFunc)(void *arg);
  void *pArg;
  int priority; /** not used */
#else
  int source;
  void (*isr)(void *arg);
  void *_arg;
  /* the following are for internal use only */
  uint32_t mask0;
  uint32_t mask1;
  uint32_t vector;
  struct bfin_isr_s *next;
#endif
} bfin_isr_t;

/**
 * This routine registers a new ISR. It will write a new entry to the IVT table
 * @param isr contains a callback function and source
 * @return rtems status code
 */
rtems_status_code bfin_interrupt_register(bfin_isr_t *isr);

/**
 * This function unregisters a registered interrupt handler.
 * @param isr
 */
rtems_status_code bfin_interrupt_unregister(bfin_isr_t *isr);

/**
 * blackfin interrupt initialization routine. It initializes the bfin ISR
 * dispatcher. It will also create SIC CEC map which will be used for
 * identifying the ISR.
 */
void bfin_interrupt_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _BFIN_INTERRUPT_H_ */

