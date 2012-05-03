/*
 * AT91RM9200 GPIO definitions
 *
 * Copyright (c) 2002 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef AT91RM9200_GPIO_H
#define AT91RM9200_GPIO_H

#include <bits.h>

/* Register Offsets */
#define PIO_PER         0x00    /* PIO Enable Register */
#define PIO_PDR         0x04    /* PIO Disable Register */
#define PIO_PSR         0x08    /* PIO Status Register */
#define PIO_OER         0x10    /* Output Enable Register */
#define PIO_ODR         0x14    /* Output Disable Registerr */
#define PIO_OSR         0x18    /* Output Status Register */
#define PIO_IFER        0x20    /* Input Filter Enable Register */
#define PIO_IFDR        0x24    /* Input Filter Disable Register */
#define PIO_IFSR        0x28    /* Input Filter Status Register */
#define PIO_SODR        0x30    /* Set Output Data Register */
#define PIO_CODR        0x34    /* Clear Output Data Register */
#define PIO_ODSR        0x38    /* Output Data Status Register */
#define PIO_PDSR        0x3c    /* Pin Data Status Register */
#define PIO_IER         0x40    /* Interrupt Enable Register */
#define PIO_IDR         0x44    /* Interrupt Disable Register */
#define PIO_IMR         0x48    /* Interrupt Mask Register */
#define PIO_ISR         0x4c    /* Interrupt Status Register */
#define PIO_MDER        0x50    /* Multi-driver Enable Register */
#define PIO_MDDR        0x54    /* Multi-driver Disable Register */
#define PIO_MDSR        0x58    /* Multi-driver Status Register */
#define PIO_PUDR        0x60    /* Pull-up Disable Register */
#define PIO_PUER        0x64    /* Pull-up Enable Register */
#define PIO_PUSR        0x68    /* Pad Pull-up Status Register */
#define PIO_ASR         0x70    /* Select A Register */
#define PIO_BSR         0x74    /* Select B Register */
#define PIO_ABSR        0x78    /* AB Select Status Register */
#define PIO_OWER        0xA0    /* Output Write Enable Register */
#define PIO_OWDR        0xA4    /* Output Write Disable Register */
#define PIO_OWSR        0xA8    /* Output Write Status Register */


/*
 * The AT91RM9200 GPIO's are spread across four 32-bit ports A-D.
 * To make it easier to interface with them and to eliminate the need
 * to track which GPIO is in which port, we     convert the Port x, Bit y
 * into a single GPIO number 0 - 127.
 *
 * Board specific defines will assign the board level signal to a
 * virutal GPIO.
 *
 * PORT A
 */
#define GPIO_0          BIT0
#define GPIO_1          BIT1
#define GPIO_2          BIT2
#define GPIO_3          BIT3
#define GPIO_4          BIT4
#define GPIO_5          BIT5
#define GPIO_6          BIT6
#define GPIO_7          BIT7
#define GPIO_8          BIT8
#define GPIO_9          BIT9
#define GPIO_10         BIT10
#define GPIO_11         BIT11
#define GPIO_12         BIT12
#define GPIO_13         BIT13
#define GPIO_14         BIT14
#define GPIO_15         BIT15
#define GPIO_16         BIT16
#define GPIO_17         BIT17
#define GPIO_18         BIT18
#define GPIO_19         BIT19
#define GPIO_20         BIT20
#define GPIO_21         BIT21
#define GPIO_22         BIT22
#define GPIO_23         BIT23
#define GPIO_24         BIT24
#define GPIO_25         BIT25
#define GPIO_26         BIT26
#define GPIO_27         BIT27
#define GPIO_28         BIT28
#define GPIO_29         BIT29
#define GPIO_30         BIT30
#define GPIO_31         BIT31
/* PORT B */
#define GPIO_32         BIT0
#define GPIO_33         BIT1
#define GPIO_34         BIT2
#define GPIO_35         BIT3
#define GPIO_36         BIT4
#define GPIO_37         BIT5
#define GPIO_38         BIT6
#define GPIO_39         BIT7
#define GPIO_40         BIT8
#define GPIO_41         BIT9
#define GPIO_42         BIT10
#define GPIO_43         BIT11
#define GPIO_44         BIT12
#define GPIO_45         BIT13
#define GPIO_46         BIT14
#define GPIO_47         BIT15
#define GPIO_48         BIT16
#define GPIO_49         BIT17
#define GPIO_50         BIT18
#define GPIO_51         BIT19
#define GPIO_52         BIT20
#define GPIO_53         BIT21
#define GPIO_54         BIT22
#define GPIO_55         BIT23
#define GPIO_56         BIT24
#define GPIO_57         BIT25
#define GPIO_58         BIT26
#define GPIO_59         BIT27
#define GPIO_60         BIT28
#define GPIO_61         BIT29
#define GPIO_62         BIT30
#define GPIO_63         BIT31
/* PORT C */
#define GPIO_64         BIT0
#define GPIO_65         BIT1
#define GPIO_66         BIT2
#define GPIO_67         BIT3
#define GPIO_68         BIT4
#define GPIO_69         BIT5
#define GPIO_70         BIT6
#define GPIO_71         BIT7
#define GPIO_72         BIT8
#define GPIO_73         BIT9
#define GPIO_74         BIT10
#define GPIO_75         BIT11
#define GPIO_76         BIT12
#define GPIO_77         BIT13
#define GPIO_78         BIT14
#define GPIO_79         BIT15
#define GPIO_80         BIT16
#define GPIO_81         BIT17
#define GPIO_82         BIT18
#define GPIO_83         BIT19
#define GPIO_84         BIT20
#define GPIO_85         BIT21
#define GPIO_86         BIT22
#define GPIO_87         BIT23
#define GPIO_88         BIT24
#define GPIO_89         BIT25
#define GPIO_90         BIT26
#define GPIO_91         BIT27
#define GPIO_92         BIT28
#define GPIO_93         BIT29
#define GPIO_94         BIT30
#define GPIO_95         BIT31
/* PORT D */
#define GPIO_96         BIT0
#define GPIO_97         BIT1
#define GPIO_98         BIT2
#define GPIO_99         BIT3
#define GPIO_100        BIT4
#define GPIO_101        BIT5
#define GPIO_102        BIT6
#define GPIO_103        BIT7
#define GPIO_104        BIT8
#define GPIO_105        BIT9
#define GPIO_106        BIT10
#define GPIO_107        BIT11
#define GPIO_108        BIT12
#define GPIO_109        BIT13
#define GPIO_110        BIT14
#define GPIO_111        BIT15
#define GPIO_112        BIT16
#define GPIO_113        BIT17
#define GPIO_114        BIT18
#define GPIO_115        BIT19
#define GPIO_116        BIT20
#define GPIO_117        BIT21
#define GPIO_118        BIT22
#define GPIO_119        BIT23
#define GPIO_120        BIT24
#define GPIO_121        BIT25
#define GPIO_122        BIT26
#define GPIO_123        BIT27
#define GPIO_124        BIT28
#define GPIO_125        BIT29
#define GPIO_126        BIT30
#define GPIO_127        BIT31

/*
 * Most of the GPIO pins can have one of two alternate functions
 * in addition to being GPIO
 *
 * Port A, Alternate Function A
 */
#define PIOA_ASR_MISO   BIT0    /* SPI Master In (RX), Slave out */
#define PIOA_ASR_MOSI   BIT1    /* SPI Master Out (TX), Slave In */
#define PIOA_ASR_SPCK   BIT2    /* SPI Clock */
#define PIOA_ASR_NPCS0  BIT3    /* SPI Chip Select 0 */
#define PIOA_ASR_NPCS1  BIT4    /* SPI Chip Select 1 */
#define PIOA_ASR_NPCS2  BIT5    /* SPI Chip Select 2 */
#define PIOA_ASR_NPCS3  BIT6    /* SPI Chip Select 3 */
#define PIOA_ASR_ETXCK  BIT7    /* EMAC TX Clock */
#define PIOA_ASR_ETXEN  BIT8    /* EMAC TXEN */
#define PIOA_ASR_ETX0   BIT9    /* EMAC TXD0 */
#define PIOA_ASR_ETX1   BIT10   /* EMAC TXD1  */
#define PIOA_ASR_ECRS   BIT11   /* EMAC CRS */
#define PIOA_ASR_ERX0   BIT12   /* EMAC RXD0 */
#define PIOA_ASR_ERX1   BIT13   /* EMAC RXD1 */
#define PIOA_ASR_ERXER  BIT14   /* EMAC RXER */
#define PIOA_ASR_EMDC   BIT15   /* EMAC MDC */
#define PIOA_ASR_EMDIO  BIT16   /* EMAC MDIO */
#define PIOA_ASR_TXD0   BIT17   /* USART 0 Receive */
#define PIOA_ASR_RXD0   BIT18   /* USART 0 Transmit */
#define PIOA_ASR_SCK0   BIT19   /* USART 0 Clock */
#define PIOA_ASR_CTS0   BIT20   /* USART 0 CTS */
#define PIOA_ASR_RTS0   BIT21   /* USART 0 RTS */
#define PIOA_ASR_RXD2   BIT22   /* USART 2 Receive */
#define PIOA_ASR_TXD2   BIT23   /* USART 2 Transmit */
#define PIOA_ASR_SCK2   BIT24   /* USART 2 Clock */
#define PIOA_ASR_TWD    BIT25   /* Two-Wire (I2C) Data */
#define PIOA_ASR_TWCK   BIT26   /* Two-Wire (I2C) Clock */
#define PIOA_ASR_MCCK   BIT27   /* MMC/SD Card Clock */
#define PIOA_ASR_MCCDA  BIT28   /* MMC/SD Card A Command */
#define PIOA_ASR_MCDA0  BIT29   /* MMC/SD Card A Data 0 */
#define PIOA_ASR_DRXD   BIT30   /* Debug Uart Receive */
#define PIOA_ASR_DTXD   BIT31   /* Debug Uart Transmit */

/* Port A, Alternate Function B */
#define PIOA_BSR_PCK3   BIT0    /* Peripheral Clock 3 */
#define PIOA_BSR_PCK0   BIT1    /* Peripheral Clock 0 */
#define PIOA_BSR_IRQ4   BIT2    /* IRQ4 */
#define PIOA_BSR_IRQ5   BIT3    /* IRQ5 */
/*#define PIOA_BSR_PCK1 BIT4     Peripheral Clock 1 ***DUPLICATED at BIT24 ??? */
#define PIOA_BSR_TXD3   BIT5    /* USART 3 Transmit */
#define PIOA_BSR_RXD3   BIT6    /* USART 3 Receive */
#define PIOA_BSR_PCK2   BIT7    /* Peripheral Clock 2 */
#define PIOA_BSR_MCCDB  BIT8    /* MMC/SD Card B Command */
#define PIOA_BSR_MCDB0  BIT9    /* MMC/SD Card B Data 0 */
#define PIOA_BSR_MCDB1  BIT10   /* MMC/SD Card B Data 1 */
#define PIOA_BSR_MCDB2  BIT11   /* MMC/SD Card B Data 2 */
#define PIOA_BSR_MCDB3  BIT12   /* MMC/SD C ard B Data 3 */
#define PIOA_BSR_TCLK0  BIT13   /* Timer 0 Clock */
#define PIOA_BSR_TCLK1  BIT14   /* Timer 1 Clck */
#define PIOA_BSR_TCLK2  BIT15   /* Timer 2 Clock */
#define PIOA_BSR_IRQ6   BIT16   /* IRQ6 */
#define PIOA_BSR_TIOA0  BIT17   /* Timer 0 I/O A */
#define PIOA_BSR_TIOB0  BIT18   /* Timer 0 I/O B */
#define PIOA_BSR_TIOA1  BIT19   /* Timer 1 I/O A */
#define PIOA_BSR_TIOB1  BIT20   /* Timer 1 I/O B */
#define PIOA_BSR_TIOA2  BIT21   /* Timer 2 I/O A */
#define PIOA_BSR_TIOB2  BIT22   /* Timer 2 I/O B */
#define PIOA_BSR_IRQ3   BIT23   /* IRQ3 */
#define PIOA_BSR_PCK1   BIT24   /* Peripheral Clock 1    */
#define PIOA_BSR_IRQ2   BIT25   /* IRQ2 */
#define PIOA_BSR_IRQ1   BIT26   /* IRQ1 */
#define PIOA_BSR_TCLK3  BIT27   /* Timer Block Clock 3 (docs only show 0-2?) */
#define PIOA_BSR_TCLK4  BIT28   /* Timer Block Clock 4 */
#define PIOA_BSR_TCLK5  BIT29   /* Timer Block Clock 5 */
#define PIOA_BSR_CTS2   BIT30   /* USART 2 CTS */
#define PIOA_BSR_RTS2   BIT31   /* USART 2 RTS */

/* Port B, Function A */
#define PIOB_ASR_TF0    BIT0    /* AC'97/I2S 0 Transmit Frame  */
#define PIOB_ASR_TK0    BIT1    /* AC'97/I2S 0 Transmit Clock  */
#define PIOB_ASR_TD0    BIT2    /* AC'97/I2S 0 Transmit Data */
#define PIOB_ASR_RD0    BIT3    /* AC'97/I2S 0 Receive Data */
#define PIOB_ASR_RK0    BIT4    /* AC'97/I2S 0 Receive Clock */
#define PIOB_ASR_RF0    BIT5    /* AC'97/I2S 0 Receive Frame */
#define PIOB_ASR_TF1    BIT6    /* AC'97/I2S 1 Transmit Frame  */
#define PIOB_ASR_TK1    BIT7    /* AC'97/I2S 1 Transmit Clock  */
#define PIOB_ASR_TD1    BIT8    /* AC'97/I2S 1 Transmit Data  */
#define PIOB_ASR_RD1    BIT9    /* AC'97/I2S 1 Receive Data  */
#define PIOB_ASR_RK1    BIT10   /* AC'97/I2S 1 Receive Clock  */
#define PIOB_ASR_RF1    BIT11   /* AC'97/I2S 1 Receive Frame  */
#define PIOB_ASR_TF2    BIT12   /* AC'97/I2S 1 Transmit Frame  */
#define PIOB_ASR_TK2    BIT13   /* AC'97/I2S 1 Transmit Clock  */
#define PIOB_ASR_TD2    BIT14   /* AC'97/I2S 1 Transmit Data   */
#define PIOB_ASR_RD2    BIT15   /* AC'97/I2S 1 Receive Data   */
#define PIOB_ASR_RK2    BIT16   /* AC'97/I2S 1 Receive Clock   */
#define PIOB_ASR_RF2    BIT17   /* AC'97/I2S 1 Receive Frame   */
#define PIOB_ASR_RI1    BIT18   /* USART 1 RI  */
#define PIOB_ASR_DTR1   BIT19   /* USART 1 DTR */
#define PIOB_ASR_TXD1   BIT20   /* USART 1 TXD */
#define PIOB_ASR_RXD1   BIT21   /* USART 1 RXD */
#define PIOB_ASR_SCK1   BIT22   /* USART 1 SCK */
#define PIOB_ASR_DCD1   BIT23   /* USART 1 DCD */
#define PIOB_ASR_CTS1   BIT24   /* USART 1 CTS */
#define PIOB_ASR_DSR1   BIT25   /* USART 1 DSR */
#define PIOB_ASR_RTS1   BIT26   /* USART 1 RTS */
#define PIOB_ASR_PCK0   BIT27   /* Peripheral Clock 0  */
#define PIOB_ASR_FIQ    BIT28   /* FIQ */
#define PIOB_ASR_IRQ0   BIT29   /* IRQ0 */

/* Port B, Function B */
#define PIOB_BSR_RTS3   BIT0    /* USART 3 */
#define PIOB_BSR_CTS3   BIT1    /* USART 3 */
#define PIOB_BSR_SCK3   BIT2    /* USART 3 */
#define PIOB_BSR_MCDA1  BIT3    /* MMC/SD Card A, Data 1 */
#define PIOB_BSR_MCDA2  BIT4    /* MMC/SD Card A, Data 2 */
#define PIOB_BSR_MCDA3  BIT5    /* MMC/SD Card A, Data 3 */
#define PIOB_BSR_TIOA3  BIT6    /* Timer 3 IO A */
#define PIOB_BSR_TIOB3  BIT7    /* Timer 3 IO B */
#define PIOB_BSR_TIOA4  BIT8    /* Timer 4 IO A */
#define PIOB_BSR_TIOB4  BIT9    /* Timer 4 IO B */
#define PIOB_BSR_TIOA5  BIT10   /* Timer 5 IO A */
#define PIOB_BSR_TIOB5  BIT11   /* Timer 5 IO B */
#define PIOB_BSR_ETX2   BIT12   /* EMAC TXD2 */
#define PIOB_BSR_ETX3   BIT13   /* EMAC TXD3 */
#define PIOB_BSR_ETXER  BIT14   /* EMAC TXER */
#define PIOB_BSR_ERX2   BIT15   /* EMAC RXD2 */
#define PIOB_BSR_ERX3   BIT16   /* EMAC RXD3 */
#define PIOB_BSR_ERXDV  BIT17   /* EMAC RXDV */
#define PIOB_BSR_ECOL   BIT18   /* EMAC COL */
#define PIOB_BSR_ERXCK  BIT19   /* EMAC RX Clock */
#define PIOB_BSR_EF100  BIT25   /* EMAC Speed 100 (RMII Only) */

/* Port C, Alternate Function A */
#define PIOC_ASR_BFCK   BIT0    /* Burst Flash Clock */
#define PIOC_ASR_BFRDY  BIT1    /* Burst Flash Ready or SMC Card OE */
#define PIOC_ASR_BFAVD  BIT2    /* Burst Flash Address Valid */
#define PIOC_ASR_BFBAA  BIT3    /* Burst Flash Address Advance or SMC Card WE */
#define PIOC_ASR_BFOE   BIT4    /* Burst Flash OE */
#define PIOC_ASR_BFWE   BIT5    /* Burst Flash WE */
#define PIOC_ASR_NWAIT  BIT6    /* WAIT Input */
#define PIOC_ASR_A23    BIT7    /* A23 */
#define PIOC_ASR_A24    BIT8    /* A24 */
#define PIOC_ASR_A25    BIT9    /* A25 or Compact Flash R/W */
#define PIOC_ASR_NCS4   BIT10   /* CS4 or Compact Flash CS */
#define PIOC_ASR_NCS5   BIT11   /* CS5 or Compact Flash CE1 */
#define PIOC_ASR_NCS6   BIT12   /* CS6 or Compact Flash CE2 */
#define PIOC_ASR_NCS7   BIT13   /* CS7 */
#define PIOC_ASR_D16    BIT16   /* Databus Bit 16 */
#define PIOC_ASR_D17    BIT17   /* Databus Bit 17 */
#define PIOC_ASR_D18    BIT18   /* Databus Bit 18 */
#define PIOC_ASR_D19    BIT19   /* Databus Bit 19 */
#define PIOC_ASR_D20    BIT20   /* Databus Bit 20 */
#define PIOC_ASR_D21    BIT21   /* Databus Bit 21 */
#define PIOC_ASR_D22    BIT22   /* Databus Bit 22 */
#define PIOC_ASR_D23    BIT23   /* Databus Bit 23 */
#define PIOC_ASR_D24    BIT24   /* Databus Bit 24 */
#define PIOC_ASR_D25    BIT25   /* Databus Bit 25 */
#define PIOC_ASR_D26    BIT26   /* Databus Bit 26 */
#define PIOC_ASR_D27    BIT27   /* Databus Bit 27 */
#define PIOC_ASR_D28    BIT28   /* Databus Bit 28 */
#define PIOC_ASR_D29    BIT29   /* Databus Bit 29 */
#define PIOC_ASR_D30    BIT30   /* Databus Bit 30 */
#define PIOC_ASR_D31    BIT31   /* Databus Bit 31 */

/* Port C, Alternate Function B - None */

/* Port D, Alternate Function A */
#define PIOD_ASR_ETX0   BIT0    /* EMAC TXD0 */
#define PIOD_ASR_ETX1   BIT1    /* EMAC TXD1 */
#define PIOD_ASR_ETX2   BIT2    /* EMAC TXD2 */
#define PIOD_ASR_ETX3   BIT3    /* EMAC TXD3 */
#define PIOD_ASR_ETXEN  BIT4    /* EMAC TXEN */
#define PIOD_ASR_ETXER  BIT5    /* EMAC TXER */
#define PIOD_ASR_DTXD   BIT6    /* Debug UART Transmit */
#define PIOD_ASR_PCK0   BIT7    /* Peripheral Clock 0 */
#define PIOD_ASR_PCK1   BIT8    /* Peripheral Clock 1 */
#define PIOD_ASR_PCK2   BIT9    /* Peripheral Clock 2 */
#define PIOD_ASR_PCK3   BIT10   /* Peripheral Clock 3 */
#define PIOD_ASR_TD0    BIT15   /* AC'97/I2S 0 Transmit Data */
#define PIOD_ASR_TD1    BIT16   /* AC'97/I2S 1 Transmit Data */
#define PIOD_ASR_TD2    BIT17   /* AC'97/I2S 2 Transmit Data */
#define PIOD_ASR_NPCS1  BIT18   /* SPI Chip Select 1 */
#define PIOD_ASR_NPCS2  BIT19   /* SPI Chip Select 2 */
#define PIOD_ASR_NPCS3  BIT20   /* SPI Chip Select 3 */
#define PIOD_ASR_RTS0   BIT21   /* USART 0 RTS */
#define PIOD_ASR_RTS1   BIT22   /* USART 1 RTS */
#define PIOD_ASR_RTS2   BIT23   /* USART 2 RTS */
#define PIOD_ASR_RTS3   BIT24   /* USART 3 RTS */
#define PIOD_ASR_DTR1   BIT25   /* USART 1 DTR */

/* Port D, Alternate Function B */

#define PIOC_ASR_TSYNC  BIT7    /* ETM Sync      */
#define PIOC_ASR_TCLK   BIT8    /* ETM Clock */
#define PIOC_ASR_TPS0   BIT9    /* ETM Processor Status 0 */
#define PIOC_ASR_TPS1   BIT10   /* ETM Processor Status 1 */
#define PIOC_ASR_TPS2   BIT11   /* ETM Processor Status 2 */
#define PIOC_ASR_TPK0   BIT12   /* ETM Packet Data 0 */
#define PIOC_ASR_TPK1   BIT13   /* ETM Packet Data 1 */
#define PIOC_ASR_TPK2   BIT14   /* ETM Packet Data 2 */
#define PIOC_ASR_TPK3   BIT15   /* ETM Packet Data 3 */
#define PIOC_ASR_TPK4   BIT16   /* ETM Packet Data 4 */
#define PIOC_ASR_TPK5   BIT17   /* ETM Packet Data 5 */
#define PIOC_ASR_TPK6   BIT18   /* ETM Packet Data 6 */
#define PIOC_ASR_TPK7   BIT19   /* ETM Packet Data 7 */
#define PIOC_ASR_TPK8   BIT20   /* ETM Packet Data 8 */
#define PIOC_ASR_TPK9   BIT21   /* ETM Packet Data 9 */
#define PIOC_ASR_TPK10  BIT22   /* ETM Packet Data 10 */
#define PIOC_ASR_TPK11  BIT23   /* ETM Packet Data 11 */
#define PIOC_ASR_TPK12  BIT24   /* ETM Packet Data 12 */
#define PIOC_ASR_TPK13  BIT25   /* ETM Packet Data 13 */
#define PIOC_ASR_TPK14  BIT26   /* ETM Packet Data 14 */
#define PIOC_ASR_TPK15  BIT27   /* ETM Packet Data 15 */

#endif
