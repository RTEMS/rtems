/**
 * @file
 * @ingroup rtl22xx_uart
 * @brief UART support.
 */

#ifndef LPC22XX_UART_H
#define LPC22XX_UART_H

/**
 * @defgroup rtl22xx_uart UART Support
 * @ingroup RTEMSBSPsARMRTL22XX
 * @brief UART (Universal Asynchronous Reciever/Transmitter) Support
 * @{
 */

#define FIFODEEP    16

#define BD115200    115200
#define BD38400     38400
#define BD9600      9600

/** @brief PINSEL0 Value for UART0 */
#define U0_PINSEL       (0x00000005)
/** @brief PINSEL0 Mask for UART0 */
#define U0_PINMASK      (0x0000000F)
/** @brief PINSEL0 Value for UART1 */
#define U1_PINSEL       (0x00050000)
/** @brief PINSEL0 Mask for UART1 */
#define U1_PINMASK      (0x000F0000)

/**
 * @name Uart line control register bit descriptions
 * @{
 */

#define LCR_WORDLENTH_BIT         0
#define LCR_STOPBITSEL_BIT        2
#define LCR_PARITYENBALE_BIT      3
#define LCR_PARITYSEL_BIT         4
#define LCR_BREAKCONTROL_BIT      6
#define LCR_DLAB_BIT              7

/** @} */

/**
 * @name Line Control Register bit definitions
 * @{
 */

/** @brief 5-bit character length */
#define ULCR_CHAR_5         (0 << 0)
/** @brief 6-bit character length */
#define ULCR_CHAR_6         (1 << 0)
/** @brief 7-bit character length */
#define ULCR_CHAR_7         (2 << 0)
/** @brief 8-bit character length */
#define ULCR_CHAR_8         (3 << 0)
/** @brief no stop bits */
#define ULCR_STOP_0         (0 << 2)
/** @brief 1 stop bit */
#define ULCR_STOP_1         (1 << 2)
/** @brief No Parity */
#define ULCR_PAR_NO         (0 << 3)
/** @brief Odd Parity */
#define ULCR_PAR_ODD        (1 << 3)
/** @brief Even Parity */
#define ULCR_PAR_EVEN       (3 << 3)
/** @brief MARK "1" Parity */
#define ULCR_PAR_MARK       (5 << 3)
/** @brief SPACE "0" Paruty */
#define ULCR_PAR_SPACE      (7 << 3)
/** @brief Output BREAK line condition */
#define ULCR_BREAK_ENABLE   (1 << 6)
/** @brief Enable Divisor Latch Access */
#define ULCR_DLAB_ENABLE    (1 << 7)

/** @} */

/**
 * @name Modem Control Register bit definitions
 * @{
 */

/** @brief Data Terminal Ready */
#define UMCR_DTR            (1 << 0)
/** @brief Request To Send */
#define UMCR_RTS            (1 << 1)
/** @brief Loopback */
#define UMCR_LB             (1 << 4)

/** @} */

/**
 * @name  Line Status Register bit definitions
 * @{
 */

/** @brief Receive Data Ready */
#define ULSR_RDR            (1 << 0)
/** @brief Overrun Error */
#define ULSR_OE             (1 << 1)
/** @brief Parity Error */
#define ULSR_PE             (1 << 2)
/** @brief Framing Error */
#define ULSR_FE             (1 << 3)
/** @brief Break Interrupt */
#define ULSR_BI             (1 << 4)
/** @brief Transmit Holding Register Empty */
#define ULSR_THRE           (1 << 5)
/** @brief Transmitter Empty */
#define ULSR_TEMT           (1 << 6)
/** @brief Error in Receive FIFO */
#define ULSR_RXFE           (1 << 7)
#define ULSR_ERR_MASK       0x1E

/** @} */

/**
 * @name Modem Status Register bit definitions
 * @{
 */

/** @brief Delta Clear To Send */
#define UMSR_DCTS           (1 << 0)
/** @brief Delta Data Set Ready */
#define UMSR_DDSR           (1 << 1)
/** @brief Trailing Edge Ring Indicator */
#define UMSR_TERI           (1 << 2)
/** @brief Delta Data Carrier Detect */
#define UMSR_DDCD           (1 << 3)
/** @brief Clear To Send */
#define UMSR_CTS            (1 << 4)
/** @brief Data Set Ready */
#define UMSR_DSR            (1 << 5)
/** @brief Ring Indicator */
#define UMSR_RI             (1 << 6)
/** @brief Data Carrier Detect */
#define UMSR_DCD            (1 << 7)

/** @} */

/**
 * @name Uart Interrupt Identification
 * @{
 */

#define IIR_RSL                   0x3
#define IIR_RDA                   0x2
#define IIR_CTI                   0x6
#define IIR_THRE                  0x1

/** @} */

/**
 * @name  Uart Interrupt Enable Type
 * @{
 */

#define IER_RBR                   0x1
#define IER_THRE                  0x2
#define IER_RLS                   0x4

/** @} */

/**
 * @name Uart Receiver Errors
 * @{
 */

#define RC_FIFO_OVERRUN_ERR       0x1
#define RC_OVERRUN_ERR            0x2
#define RC_PARITY_ERR             0x4
#define RC_FRAMING_ERR            0x8
#define RC_BREAK_IND              0x10

/** @} */

typedef enum {
  UART0 = 0,
  UART1
} LPC_UartChanel_t;

/** @} */

#endif

