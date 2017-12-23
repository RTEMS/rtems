/* irq.h
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  Common file, merged from s3c2400/irq/irq.h and s3c2410/irq/irq.h
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <s3c24xx.h>

#ifdef CPU_S3C2400
  /* possible interrupt sources */
#define BSP_EINT0             0
#define BSP_EINT1             1
#define BSP_EINT2             2
#define BSP_EINT3             3
#define BSP_EINT4             4
#define BSP_EINT5             5
#define BSP_EINT6             6
#define BSP_EINT7             7
#define BSP_INT_TICK          8
#define BSP_INT_WDT           9
#define BSP_INT_TIMER0       10
#define BSP_INT_TIMER1       11
#define BSP_INT_TIMER2       12
#define BSP_INT_TIMER3       13
#define BSP_INT_TIMER4       14
#define BSP_INT_UERR01       15
#define _res0                16
#define BSP_INT_DMA0         17
#define BSP_INT_DMA1         18
#define BSP_INT_DMA2         19
#define BSP_INT_DMA3         20
#define BSP_INT_MMC          21
#define BSP_INT_SPI          22
#define BSP_INT_URXD0        23
#define BSP_INT_URXD1        24
#define BSP_INT_USBD         25
#define BSP_INT_USBH         26
#define BSP_INT_IIC          27
#define BSP_INT_UTXD0        28
#define BSP_INT_UTXD1        29
#define BSP_INT_RTC          30
#define BSP_INT_ADC          31
#define BSP_MAX_INT          32

#elif defined CPU_S3C2410
  /* possible interrupt sources */
#define BSP_EINT0             0
#define BSP_EINT1             1
#define BSP_EINT2             2
#define BSP_EINT3             3
#define BSP_EINT4_7           4
#define BSP_EINT8_23          5
#define BSP_nBATT_FLT         7
#define BSP_INT_TICK          8
#define BSP_INT_WDT           9
#define BSP_INT_TIMER0       10
#define BSP_INT_TIMER1       11
#define BSP_INT_TIMER2       12
#define BSP_INT_TIMER3       13
#define BSP_INT_TIMER4       14
#define BSP_INT_UART2        15
#define BSP_INT_LCD          16
#define BSP_INT_DMA0         17
#define BSP_INT_DMA1         18
#define BSP_INT_DMA2         19
#define BSP_INT_DMA3         20
#define BSP_INT_SDI          21
#define BSP_INT_SPI0         22
#define BSP_INT_UART1        23
#define BSP_INT_USBD         25
#define BSP_INT_USBH         26
#define BSP_INT_IIC          27
#define BSP_INT_UART0        28
#define BSP_INT_SPI1         29
#define BSP_INT_RTC          30
#define BSP_INT_ADC          31
#define BSP_MAX_INT          32
#endif

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX (BSP_MAX_INT - 1)

#endif /* _IRQ_H_ */
/* end of include file */
