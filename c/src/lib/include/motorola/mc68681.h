/*
 * mc68681-duart.h -- Low level support code for the Motorola mc68681
 *                   DUART. This one is one the mc68ec0x0 board.
 *		     Written by rob@cygnus.com (Rob Savoye)
 *
 *  $Id$
 */

#ifndef __MC68681_H__
#define __MC68681_H__
#define DUART_ADDR	0xb00003		/* base address of the DUART */

/*
 * mc68681 register offsets
 */
#define DUART_MR1A	0x00			/* Mode Register A */
#define DUART_MR2A	0x00			/* Mode Register A */
#define DUART_SRA	0x04			/* Status Register A */
#define DUART_CSRA	0x04			/* Clock-Select Register A */
#define DUART_CRA	0x08			/* Command Register A */
#define DUART_RBA	0x0c			/* Receive Buffer A */
#define DUART_TBA	0x0c			/* Transmit Buffer A */
#define DUART_IPCR	0x10			/* Input Port Change Register */
#define DUART_ACR	0x10    		/* Auxiliary Control Register */
#define DUART_ISR	0x14			/* Interrupt Status Register */
#define DUART_IMR	0x14			/* Interrupt Mask Register */
#define DUART_CUR	0x18			/* Counter Mode: current MSB */
#define DUART_CTUR	0x18			/* Counter/Timer upper reg */
#define DUART_CLR	0x1c			/* Counter Mode: current LSB */
#define DUART_CTLR	0x1c			/* Counter/Timer lower reg */
#define DUART_MR1B	0x20			/* Mode Register B */
#define DUART_MR2B	0x20    		/* Mode Register B */
#define DUART_SRB	0x24			/* Status Register B */
#define DUART_CSRB	0x24			/* Clock-Select Register B */
#define DUART_CRB	0x28			/* Command Register B */
#define DUART_RBB	0x2c			/* Receive Buffer B */
#define DUART_TBB	0x2c			/* Transmit Buffer A */
#define DUART_IVR	0x30			/* Interrupt Vector Register */
#define DUART_IP	0x34			/* Input Port */
#define DUART_OPCR	0x34			/* Output Port Configuration Reg. */
#define DUART_STRTCC	0x38			/* Start-Counter command */
#define DUART_OPRSET	0x38			/* Output Port Reg, SET bits */
#define DUART_STOPCC	0x3c			/* Stop-Counter command */
#define DUART_OPRRST	0x3c			/* Output Port Reg, ReSeT bits */

/* this is just if you want a copy of the chip's registers */
struct duart_regs {
  unsigned char mr1a_reg;			/* Mode Register A */
  unsigned char mr2a_reg;			/* Mode Register A */
  unsigned char sra_reg;			/* Status Register A */
  unsigned char csra_reg;			/* Clock-Select Register A */
  unsigned char cra_reg;			/* Command Register A */
  unsigned char ipcr_reg;			/* Input Port Change Register */
  unsigned char acr_reg;			/* Auxiliary Control Register */
  unsigned char isr_reg;			/* Interrupt Status Register */
  unsigned char imr_reg;			/* Interrupt Mask Register */
  unsigned char cur_reg;			/* Counter Mode: current MSB */
  unsigned char ctur_reg;			/* Counter/Timer upper reg */
  unsigned char clr_reg;			/* Counter Mode: current LSB */
  unsigned char ctlr_reg;			/* Counter/Timer lower reg */
  unsigned char mr1b_reg;			/* Mode Register B */
  unsigned char mr2b_reg;			/* Mode Register B */
  unsigned char srb_reg;			/* Status Register B */
  unsigned char csrb_reg;			/* Clock-Select Register B */
  unsigned char crb_reg;			/* Command Register B */
  unsigned char ivr_reg;			/* Interrupt Vector Register */
  unsigned char ip_reg;				/* Input Port */
  unsigned char opcr_reg;			/* Output Port Configuration Reg. */
  unsigned char oprset_reg;			/* Output Port Reg; bit SET */
  unsigned char strtcc_reg;			/* Start-Counter command */
  unsigned char oprrst_reg;			/* Output Port Reg; bit ReSeT */
  unsigned char stopcc_reg;			/* Stop-Counter command */
  unsigned char pad;
};

/* Some RTEMS style defines: */
#ifndef VOL8
#define VOL8( ptr )   ((volatile rtems_unsigned8 *)(ptr))
#endif

#define MC68681_WRITE( reg, data ) \
   *(VOL8(DUART_ADDR+reg)) = (data)

#define MC68681_READ( reg, data ) \
   (data) = *(VOL8(DUART_ADDR+reg))

#endif
