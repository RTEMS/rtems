/**************************************************************************************************
*			file: port.h
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#ifndef __PORT_H
#define __PORT_H


#define   __I     volatile const     /*!< Defines 'read only' permissions                 */
#define   __O     volatile           /*!< Defines 'write only' permissions                */
#define   __IO    volatile           /*!< Defines 'read / write' permissions              */

/** PORT - Register Layout Typedef */
typedef struct {
  __IO unsigned int pcr[32];                           /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  __O  unsigned int gpclr;                             /**< Global Pin Control Low Register, offset: 0x80 */
  __O  unsigned int gpchr;                             /**< Global Pin Control High Register, offset: 0x84 */
       unsigned char reserved_0[24];
  __IO unsigned int isfr;                              /**< Interrupt Status Flag Register, offset: 0xA0 */
       unsigned char reserved_1[28];
  __IO unsigned int dfer;                              /**< Digital Filter Enable Register, offset: 0xC0 */
  __IO unsigned int dfcr;                              /**< Digital Filter Clock Register, offset: 0xC4 */
  __IO unsigned int dfwr;                              /**< Digital Filter Width Register, offset: 0xC8 */
} kinetis_port_t;

/* PCR Bit Fields */
#define PORT_PCR_PS_MASK                         0x1u
#define PORT_PCR_PS_SHIFT                        0
#define PORT_PCR_PE_MASK                         0x2u
#define PORT_PCR_PE_SHIFT                        1
#define PORT_PCR_SRE_MASK                        0x4u
#define PORT_PCR_SRE_SHIFT                       2
#define PORT_PCR_PFE_MASK                        0x10u
#define PORT_PCR_PFE_SHIFT                       4
#define PORT_PCR_ODE_MASK                        0x20u
#define PORT_PCR_ODE_SHIFT                       5
#define PORT_PCR_DSE_MASK                        0x40u
#define PORT_PCR_DSE_SHIFT                       6
#define PORT_PCR_MUX_MASK                        0x700u
#define PORT_PCR_MUX_SHIFT                       8
#define PORT_PCR_MUX(x)                          (((unsigned int)(((unsigned int)(x))<<PORT_PCR_MUX_SHIFT))&PORT_PCR_MUX_MASK)
#define PORT_PCR_LK_MASK                         0x8000u
#define PORT_PCR_LK_SHIFT                        15
#define PORT_PCR_IRQC_MASK                       0xF0000u
#define PORT_PCR_IRQC_SHIFT                      16
#define PORT_PCR_IRQC(x)                         (((unsigned int)(((unsigned int)(x))<<PORT_PCR_IRQC_SHIFT))&PORT_PCR_IRQC_MASK)
#define PORT_PCR_ISF_MASK                        0x1000000u
#define PORT_PCR_ISF_SHIFT                       24
/* GPCLR Bit Fields */
#define PORT_GPCLR_GPWD_MASK                     0xFFFFu
#define PORT_GPCLR_GPWD_SHIFT                    0
#define PORT_GPCLR_GPWD(x)                       (((unsigned int)(((unsigned int)(x))<<PORT_GPCLR_GPWD_SHIFT))&PORT_GPCLR_GPWD_MASK)
#define PORT_GPCLR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCLR_GPWE_SHIFT                    16
#define PORT_GPCLR_GPWE(x)                       (((unsigned int)(((unsigned int)(x))<<PORT_GPCLR_GPWE_SHIFT))&PORT_GPCLR_GPWE_MASK)
/* GPCHR Bit Fields */
#define PORT_GPCHR_GPWD_MASK                     0xFFFFu
#define PORT_GPCHR_GPWD_SHIFT                    0
#define PORT_GPCHR_GPWD(x)                       (((unsigned int)(((unsigned int)(x))<<PORT_GPCHR_GPWD_SHIFT))&PORT_GPCHR_GPWD_MASK)
#define PORT_GPCHR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCHR_GPWE_SHIFT                    16
#define PORT_GPCHR_GPWE(x)                       (((unsigned int)(((unsigned int)(x))<<PORT_GPCHR_GPWE_SHIFT))&PORT_GPCHR_GPWE_MASK)
/* ISFR Bit Fields */
#define PORT_ISFR_ISF_MASK                       0xFFFFFFFFu
#define PORT_ISFR_ISF_SHIFT                      0
#define PORT_ISFR_ISF(x)                         (((unsigned int)(((unsigned int)(x))<<PORT_ISFR_ISF_SHIFT))&PORT_ISFR_ISF_MASK)
/* DFER Bit Fields */
#define PORT_DFER_DFE_MASK                       0xFFFFFFFFu
#define PORT_DFER_DFE_SHIFT                      0
#define PORT_DFER_DFE(x)                         (((unsigned int)(((unsigned int)(x))<<PORT_DFER_DFE_SHIFT))&PORT_DFER_DFE_MASK)
/* DFCR Bit Fields */
#define PORT_DFCR_CS_MASK                        0x1u
#define PORT_DFCR_CS_SHIFT                       0
/* DFWR Bit Fields */
#define PORT_DFWR_FILT_MASK                      0x1Fu
#define PORT_DFWR_FILT_SHIFT                     0
#define PORT_DFWR_FILT(x)                        (((unsigned int)(((unsigned int)(x))<<PORT_DFWR_FILT_SHIFT))&PORT_DFWR_FILT_MASK)

/** Peripheral PORTA base address */
#define PORTA_BASE                               (0x40049000u)
/** Peripheral PORTA base pointer */
#define PORTA                                    ((kinetis_port_t *)PORTA_BASE)
/** Peripheral PORTB base address */
#define PORTB_BASE                               (0x4004A000u)
/** Peripheral PORTB base pointer */
#define PORTB                                    ((kinetis_port_t *)PORTB_BASE)
/** Peripheral PORTC base address */
#define PORTC_BASE                               (0x4004B000u)
/** Peripheral PORTC base pointer */
#define PORTC                                    ((kinetis_port_t *)PORTC_BASE)
/** Peripheral PORTD base address */
#define PORTD_BASE                               (0x4004C000u)
/** Peripheral PORTD base pointer */
#define PORTD                                    ((kinetis_port_t *)PORTD_BASE)
/** Peripheral PORTE base address */
#define PORTE_BASE                               (0x4004D000u)
/** Peripheral PORTE base pointer */
#define PORTE                                    ((kinetis_port_t *)PORTE_BASE)

#define PIN_0 	((unsigned char)0x00)
#define PIN_1 	((unsigned char)0x01)
#define PIN_2 	((unsigned char)0x02)
#define PIN_3 	((unsigned char)0x03)
#define PIN_4 	((unsigned char)0x04)
#define PIN_5 	((unsigned char)0x05)
#define PIN_6 	((unsigned char)0x06)
#define PIN_7 	((unsigned char)0x07)
#define PIN_8 	((unsigned char)0x08)
#define PIN_9 	((unsigned char)0x09)
#define PIN_10 	((unsigned char)0x0A)
#define PIN_11 	((unsigned char)0x0B)
#define PIN_12 	((unsigned char)0x0C)
#define PIN_13 	((unsigned char)0x0D)
#define PIN_14 	((unsigned char)0x0E)
#define PIN_15 	((unsigned char)0x0F)
#define PIN_16 	((unsigned char)0x10)
#define PIN_17 	((unsigned char)0x11)
#define PIN_18 	((unsigned char)0x12)
#define PIN_19 	((unsigned char)0x13)
#define PIN_20 	((unsigned char)0x14)
#define PIN_21 	((unsigned char)0x15)
#define PIN_22 	((unsigned char)0x16)
#define PIN_23 	((unsigned char)0x17)
#define PIN_24 	((unsigned char)0x18)
#define PIN_25 	((unsigned char)0x19)
#define PIN_26 	((unsigned char)0x1A)
#define PIN_27 	((unsigned char)0x1B)
#define PIN_28 	((unsigned char)0x1C)
#define PIN_29 	((unsigned char)0x1D)
#define PIN_30 	((unsigned char)0x1E)
#define PIN_31 	((unsigned char)0x1F)

#define FN_0 	((unsigned char)0x00)
#define FN_1 	((unsigned char)0x01)
#define FN_2 	((unsigned char)0x02)
#define FN_3 	((unsigned char)0x03)
#define FN_4 	((unsigned char)0x04)
#define FN_5 	((unsigned char)0x05)
#define FN_6 	((unsigned char)0x06)
#define FN_7 	((unsigned char)0x07)

#define DS_EN 	((unsigned char)0x01)
#define DS_DIS 	((unsigned char)0x00)
#define OD_EN 	((unsigned char)0x01)
#define OD_DIS 	((unsigned char)0x00)
#define PF_EN 	((unsigned char)0x01)
#define PF_DIS 	((unsigned char)0x00)
#define SR_EN 	((unsigned char)0x01)
#define SR_DIS 	((unsigned char)0x00)
#define P_EN 	((unsigned char)0x01)
#define P_DIS 	((unsigned char)0x00)
#define P_DOWN 	((unsigned char)0x00)
#define P_UP 	((unsigned char)0x01)

unsigned char port_clock_enable(kinetis_port_t *port);
unsigned char port_clock_disable(kinetis_port_t *port);

//mode	0000 Interrupt/DMA Request disabled
//		0001 DMA Request on rising edge
//		0010 DMA Request on falling edge
//		0011 DMA Request on either edge
//		0100 Reserved
//		1000 Interrupt when logic zero
//		1001 Interrupt on rising edge
//		1010 Interrupt on falling edge
//		1011 Interrupt on either edge
//		1100 Interrupt when logic one
unsigned char port_init_config(kinetis_port_t *port, unsigned int pin_n, unsigned int mode_n);
unsigned char port_bit_lock(kinetis_port_t *port, unsigned int pin_n);
unsigned char port_bit_function(kinetis_port_t *port, unsigned int pin_n, unsigned int fn_n);

//dse: drive strength enable			ode: open drain enable
//pfe: passive filter enable			sre: slew rate enable
//pe : pull enable						ps : pull select(0:pull-down 1:pull-up)
unsigned char port_bit_drive(kinetis_port_t *port, unsigned int pin_n, unsigned int dse, unsigned int ode,
					unsigned int pfe, unsigned int sre, unsigned int pe, unsigned int ps);

unsigned char port_global_lock(kinetis_port_t *port);

unsigned char port_global_function(kinetis_port_t *port, unsigned int fn_n);

unsigned char port_global_drive(kinetis_port_t *port, unsigned int dse, unsigned int ode,
					unsigned int pfe, unsigned int sre, unsigned int pe, unsigned int ps);

unsigned char port_bit_filter_enable(kinetis_port_t *port, unsigned int bit_n);

unsigned char port_bit_filter_disable(kinetis_port_t *port, unsigned int bit_n);

unsigned char port_global_filter_enable(kinetis_port_t *port);

unsigned char port_global_filter_disable(kinetis_port_t *port);

//PORTx : port register base address
//cs : clock source						flen: filter length
unsigned char port_filter_config(kinetis_port_t *port, unsigned int cs, unsigned int flen);
#endif
