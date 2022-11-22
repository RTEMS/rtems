/**
 * @file
 * @ingroup kinetis_interrupt
 * @brief Interrupt definitions.
 */

#ifndef LIBBSP_ARM_KINETIS_IRQ_H
#define LIBBSP_ARM_KINETIS_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/**
 * @defgroup Kinetis Interrupt Support
 * @ingroup RTEMSBSPsARMKINETIS
 * @brief Device specific interrupts
 * @{
 */

#define KINETIS_IRQ_DMA0                0                 /**< DMA Channel 0 Transfer Complete */
#define KINETIS_IRQ_DMA1                1                 /**< DMA Channel 1 Transfer Complete */
#define KINETIS_IRQ_DMA2                2                 /**< DMA Channel 2 Transfer Complete */
#define KINETIS_IRQ_DMA3                3                 /**< DMA Channel 3 Transfer Complete */
#define KINETIS_IRQ_DMA4                4                 /**< DMA Channel 4 Transfer Complete */
#define KINETIS_IRQ_DMA5                5                 /**< DMA Channel 5 Transfer Complete */
#define KINETIS_IRQ_DMA6                6                 /**< DMA Channel 6 Transfer Complete */
#define KINETIS_IRQ_DMA7                7                 /**< DMA Channel 7 Transfer Complete */
#define KINETIS_IRQ_DMA8                8                 /**< DMA Channel 8 Transfer Complete */
#define KINETIS_IRQ_DMA9                9                 /**< DMA Channel 9 Transfer Complete */
#define KINETIS_IRQ_DMA10               10                /**< DMA Channel 10 Transfer Complete */
#define KINETIS_IRQ_DMA11               11                /**< DMA Channel 11 Transfer Complete */
#define KINETIS_IRQ_DMA12               12                /**< DMA Channel 12 Transfer Complete */
#define KINETIS_IRQ_DMA13               13                /**< DMA Channel 13 Transfer Complete */
#define KINETIS_IRQ_DMA14               14                /**< DMA Channel 14 Transfer Complete */
#define KINETIS_IRQ_DMA15               15                /**< DMA Channel 15 Transfer Complete */
#define KINETIS_IRQ_DMA_Error           16                /**< DMA Error Interrupt */
#define KINETIS_IRQ_MCM                 17                /**< Normal Interrupt */
#define KINETIS_IRQ_FTFL                18                /**< FTFL Interrupt */
#define KINETIS_IRQ_READ_COLLISION      19                /**< Read Collision Interrupt */
#define KINETIS_IRQ_LVD_LVW             20                /**< Low Voltage Detect, Low Voltage Warning */
#define KINETIS_IRQ_LLW                 21                /**< Low Leakage Wakeup */
#define KINETIS_IRQ_WATCHDOG            22                /**< WDOG Interrupt */
#define KINETIS_IRQ_RESERVED39          23                /**< Reserved Interrupt 39 */
#define KINETIS_IRQ_I2C0                24                /**< I2C0 interrupt */
#define KINETIS_IRQ_I2C1                25                /**< I2C1 interrupt */
#define KINETIS_IRQ_SPI0                26                /**< SPI0 Interrupt */
#define KINETIS_IRQ_SPI1                27                /**< SPI1 Interrupt */
#define KINETIS_IRQ_SPI2                28                /**< SPI2 Interrupt */
#define KINETIS_IRQ_CAN0_ORED_MSG_BUF   29                /**< CAN0 OR'd message buffers interrupt */
#define KINETIS_IRQ_CAN0_BUS_OFF        30                /**< CAN0 bus off interrupt */
#define KINETIS_IRQ_CAN0_ERR            31                /**< CAN0 error interrupt */
#define KINETIS_IRQ_CAN0_TX_WARNING     32                /**< CAN0 Tx warning interrupt */
#define KINETIS_IRQ_CAN0_RX_WARNING     33                /**< CAN0 Rx warning interrupt */
#define KINETIS_IRQ_CAN0_WAKE_UP        34                /**< CAN0 wake up interrupt */
#define KINETIS_IRQ_I2S0_TX             35                /**< I2S0 transmit interrupt */
#define KINETIS_IRQ_I2S0_RX             36                /**< I2S0 receive interrupt */
#define KINETIS_IRQ_CAN1_ORED_MSG_BUF   37                /**< CAN1 OR'd message buffers interrupt */
#define KINETIS_IRQ_CAN1_BUS_OFF        38                /**< CAN1 bus off interrupt */
#define KINETIS_IRQ_CAN1_ERR            39                /**< CAN1 error interrupt */
#define KINETIS_IRQ_CAN1_TX_WARNING     40                /**< CAN1 Tx warning interrupt */
#define KINETIS_IRQ_CAN1_RX_WARNING     41                /**< CAN1 Rx warning interrupt */
#define KINETIS_IRQ_CAN1_WAKE_UP        42                /**< CAN1 wake up interrupt */
#define KINETIS_IRQ_RESERVED59          43                /**< Reserved interrupt 59 */
#define KINETIS_IRQ_UART0_LON           44                /**< UART0 LON interrupt */
#define KINETIS_IRQ_UART0_RX_TX         45                /**< UART0 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART0_ERR           46                /**< UART0 Error interrupt */
#define KINETIS_IRQ_UART1_RX_TX         47                /**< UART1 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART1_ERR           48                /**< UART1 Error interrupt */
#define KINETIS_IRQ_UART2_RX_TX         49                /**< UART2 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART2_ERR           50                /**< UART2 Error interrupt */
#define KINETIS_IRQ_UART3_RX_TX         51                /**< UART3 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART3_ERR           52                /**< UART3 Error interrupt */
#define KINETIS_IRQ_UART4_RX_TX         53                /**< UART4 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART4_ERR           54                /**< UART4 Error interrupt */
#define KINETIS_IRQ_UART5_RX_TX         55                /**< UART5 Receive/Transmit interrupt */
#define KINETIS_IRQ_UART5_ERR           56                /**< UART5 Error interrupt */
#define KINETIS_IRQ_ADC0                57                /**< ADC0 interrupt */
#define KINETIS_IRQ_ADC1                58                /**< ADC1 interrupt */
#define KINETIS_IRQ_CMP0                59                /**< CMP0 interrupt */
#define KINETIS_IRQ_CMP1                60                /**< CMP1 interrupt */
#define KINETIS_IRQ_CMP2                61                /**< CMP2 interrupt */
#define KINETIS_IRQ_FTM0                62                /**< FTM0 fault, overflow and channels interrupt */
#define KINETIS_IRQ_FTM1                63                /**< FTM1 fault, overflow and channels interrupt */
#define KINETIS_IRQ_FTM2                64                /**< FTM2 fault, overflow and channels interrupt */
#define KINETIS_IRQ_CMT                 65                /**< CMT interrupt */
#define KINETIS_IRQ_RTC                 66                /**< RTC interrupt */
#define KINETIS_IRQ_RTC_SECONDS         67                /**< RTC seconds interrupt */
#define KINETIS_IRQ_PIT0                68                /**< PIT timer channel 0 interrupt */
#define KINETIS_IRQ_PIT1                69                /**< PIT timer channel 1 interrupt */
#define KINETIS_IRQ_PIT2                70                /**< PIT timer channel 2 interrupt */
#define KINETIS_IRQ_PIT3                71                /**< PIT timer channel 3 interrupt */
#define KINETIS_IRQ_PDB0                72                /**< PDB0 Interrupt */
#define KINETIS_IRQ_USB0                73                /**< USB0 interrupt */
#define KINETIS_IRQ_USBDCD              74                /**< USBDCD Interrupt */
#define KINETIS_IRQ_RESERVED91          75                /**< Reserved interrupt 91 */
#define KINETIS_IRQ_RESERVED92          76                /**< Reserved interrupt 92 */
#define KINETIS_IRQ_RESERVED93          77                /**< Reserved interrupt 93 */
#define KINETIS_IRQ_RESERVED94          78                /**< Reserved interrupt 94 */
#define KINETIS_IRQ_RESERVED95          79                /**< Reserved interrupt 95 */
#define KINETIS_IRQ_SDHC                80                /**< SDHC Interrupt */
#define KINETIS_IRQ_DAC0                81                /**< DAC0 interrupt */
#define KINETIS_IRQ_DAC1                82                /**< DAC1 interrupt */
#define KINETIS_IRQ_TSI0                83                /**< TSI0 Interrupt */
#define KINETIS_IRQ_MCG                 84                /**< MCG Interrupt */
#define KINETIS_IRQ_LPTIMER             85                /**< LPTimer interrupt */
#define KINETIS_IRQ_RESERVED102         86                /**< Reserved interrupt 102 */
#define KINETIS_IRQ_PORTA               87                /**< Port A interrupt */
#define KINETIS_IRQ_PORTB               88                /**< Port B interrupt */
#define KINETIS_IRQ_PORTC               89                /**< Port C interrupt */
#define KINETIS_IRQ_PORTD               90                /**< Port D interrupt */
#define KINETIS_IRQ_PORTE               91                /**< Port E interrupt */
#define KINETIS_IRQ_RESERVED108         92                /**< Reserved interrupt 108 */
#define KINETIS_IRQ_RESERVED109         93                /**< Reserved interrupt 109 */
#define KINETIS_IRQ_SWI                 94                /**< Software interrupt */

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX 239

/** @} */

#endif /* LIBBSP_ARM_KINETIS_IRQ_H */
