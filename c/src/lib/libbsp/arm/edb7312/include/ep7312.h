/*
 * Cirrus EP7312 register declarations
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 * Notes: The PLL registers (pll_ro and pll_wo) are either read only
 *        or write only. The data sheet says not to write the read
 *        only one or read the write only one. I'm not sure what will
 *        happen if you do.
*/
#ifndef __EP7312_H__
#define __EP7312_H__

#define EP7312_REG_BASE 0x80000000

#define EP7312_PADR    ((volatile uint8_t*)(EP7312_REG_BASE + 0x0000))
#define EP7312_PBDR    ((volatile uint8_t*)(EP7312_REG_BASE + 0x0001))
#define EP7312_PDDR    ((volatile uint8_t*)(EP7312_REG_BASE + 0x0003))
#define EP7312_PADDR   ((volatile uint8_t*)(EP7312_REG_BASE + 0x0040))
#define EP7312_PBDDR   ((volatile uint8_t*)(EP7312_REG_BASE + 0x0041))
#define EP7312_PDDDR   ((volatile uint8_t*)(EP7312_REG_BASE + 0x0043))
#define EP7312_PEDR    ((volatile uint8_t*)(EP7312_REG_BASE + 0x0080))
#define EP7312_PEDDR   ((volatile uint8_t*)(EP7312_REG_BASE + 0x00C0))
#define EP7312_SYSCON1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x0100))
#define EP7312_SYSFLG1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x0140))
#define EP7312_MEMCFG1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x0180))
#define EP7312_MEMCFG2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x01C0))
#define EP7312_INTSR1  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0240))
#define EP7312_INTMR1  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0280))
#define EP7312_LCDCON  ((volatile uint32_t*)(EP7312_REG_BASE + 0x02C0))
#define EP7312_TC1D    ((volatile uint32_t*)(EP7312_REG_BASE + 0x0300))
#define EP7312_TC2D    ((volatile uint32_t*)(EP7312_REG_BASE + 0x0340))
#define EP7312_RTCDR   ((volatile uint32_t*)(EP7312_REG_BASE + 0x0380))
#define EP7312_RTCMR   ((volatile uint32_t*)(EP7312_REG_BASE + 0x03C0))
#define EP7312_PMPCON  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0400))
#define EP7312_CODR    ((volatile uint8_t*)(EP7312_REG_BASE + 0x0440))
#define EP7312_UARTDR1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x0480))
#define EP7312_UARTCR1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x04C0))
#define EP7312_SYNCIO  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0500))
#define EP7312_PALLSW  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0540))
#define EP7312_PALMSW  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0580))
#define EP7312_STFCLR  ((volatile uint32_t*)(EP7312_REG_BASE + 0x05C0))
#define EP7312_BLEOI   ((volatile uint32_t*)(EP7312_REG_BASE + 0x0600))
#define EP7312_MCEOI   ((volatile uint32_t*)(EP7312_REG_BASE + 0x0640))
#define EP7312_TEOI    ((volatile uint32_t*)(EP7312_REG_BASE + 0x0680))
#define EP7312_TC1EOI  ((volatile uint32_t*)(EP7312_REG_BASE + 0x06C0))
#define EP7312_TC2EOI  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0700))
#define EP7312_RTCEOI  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0740))
#define EP7312_UMSEOI  ((volatile uint32_t*)(EP7312_REG_BASE + 0x0780))
#define EP7312_COEOI   ((volatile uint32_t*)(EP7312_REG_BASE + 0x07C0))
#define EP7312_HALT    ((volatile uint32_t*)(EP7312_REG_BASE + 0x0800))
#define EP7312_STDBY   ((volatile uint32_t*)(EP7312_REG_BASE + 0x0840))
#define EP7312_FBADDR  ((volatile uint8_t*)(EP7312_REG_BASE + 0x1000))
#define EP7312_SYSCON2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x1100))
#define EP7312_SYSFLG2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x1140))
#define EP7312_INTSR2  ((volatile uint32_t*)(EP7312_REG_BASE + 0x1240))
#define EP7312_INTMR2  ((volatile uint32_t*)(EP7312_REG_BASE + 0x1280))
#define EP7312_UARTDR2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x1480))
#define EP7312_UARTCR2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x14C0))
#define EP7312_SS2DR   ((volatile uint32_t*)(EP7312_REG_BASE + 0x1500))
#define EP7312_SRXEOF  ((volatile uint32_t*)(EP7312_REG_BASE + 0x1600))
#define EP7312_SS2POP  ((volatile uint32_t*)(EP7312_REG_BASE + 0x16C0))
#define EP7312_KBDEOI  ((volatile uint32_t*)(EP7312_REG_BASE + 0x1700))
#define EP7312_DAIR    ((volatile uint32_t*)(EP7312_REG_BASE + 0x2000))
#define EP7312_DAIDR0  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2040))
#define EP7312_DAIDR1  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2080))
#define EP7312_DAIDR2  ((volatile uint32_t*)(EP7312_REG_BASE + 0x20C0))
#define EP7312_DAISR   ((volatile uint32_t*)(EP7312_REG_BASE + 0x2100))
#define EP7312_SYSCON3 ((volatile uint32_t*)(EP7312_REG_BASE + 0x2200))
#define EP7312_INTSR3  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2240))
#define EP7312_INTMR3  ((volatile uint8_t*)(EP7312_REG_BASE + 0x2280))
#define EP7312_LEDFLSH ((volatile uint8_t*)(EP7312_REG_BASE + 0x22C0))
#define EP7312_SDCONF  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2300))
#define EP7312_SDRFPR  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2340))
#define EP7312_UNIQID  ((volatile uint32_t*)(EP7312_REG_BASE + 0x2440))
#define EP7312_DAI64Fs ((volatile uint32_t*)(EP7312_REG_BASE + 0x2600))
#define EP7312_PLLW    ((volatile uint8_t*)(EP7312_REG_BASE + 0x2610))
#define EP7312_PLLR    ((volatile uint8_t*)(EP7312_REG_BASE + 0xA5A8))
#define EP7312_RANDID0 ((volatile uint32_t*)(EP7312_REG_BASE + 0x2700))
#define EP7312_RANDID1 ((volatile uint32_t*)(EP7312_REG_BASE + 0x2704))
#define EP7312_RANDID2 ((volatile uint32_t*)(EP7312_REG_BASE + 0x2708))
#define EP7312_RANDID3 ((volatile uint32_t*)(EP7312_REG_BASE + 0x270C))

/* serial port bits */
/* BITS in UBRLCR1 */
#define EP7312_UART_WRDLEN5    0x00000000
#define EP7312_UART_WRDLEN6    0x00020000
#define EP7312_UART_WRDLEN7    0x00040000
#define EP7312_UART_WRDLEN8    0x00060000
#define EP7312_UART_FIFOEN     0x00010000
#define EP7312_UART_XSTOP      0x00008000
#define EP7312_UART_EVENPRT    0x00004000
#define EP7312_UART_PRTEN      0x00002000
#define EP7312_UART_BREAK      0x00001000

/* BITS in INTSR1 */
#define EP7312_UART_UTXINT1    0x00002000
#define	EP7312_UART_URXINT1    0x00001000

/* BITS in UARTTDR1 */
#define EP7312_UART_FRMERR     0x00000100
#define EP7312_UART_PARERR     0x00000200
#define EP7312_UART_OVERR      0x00000400

/* BITS in system status flag register 1 */
#define EP7312_UART_UBUSY1     0x00000800
#define EP7312_UART_URXFE1     0x00400000
#define EP7312_UART_UTXFF1     0x00800000

/* system configuration bits */
/* BITS in SYSCON1 */
#define EP7312_SYSCON1_UART1EN       0x00000100
#define EP7312_SYSCON1_TC1_PRESCALE  0x00000010
#define EP7312_SYSCON1_TC1_512KHZ    0x00000020
#define EP7312_SYSCON1_TC2_PRESCALE  0x00000040
#define EP7312_SYSCON1_TC2_512KHZ    0x00000080

/* INTR1 (Interrupt 1) mask/status register bits */
#define EP7312_INTR1_EXTFIQ  0x00000001
#define EP7312_INTR1_BLINT   0x00000002
#define EP7312_INTR1_WEINT   0x00000004
#define EP7312_INTR1_MCINT   0x00000008
#define EP7312_INTR1_CSINT   0x00000010
#define EP7312_INTR1_EINT1   0x00000020
#define EP7312_INTR1_EINT2   0x00000040
#define EP7312_INTR1_EINT3   0x00000080
#define EP7312_INTR1_TC1OI   0x00000100
#define EP7312_INTR1_TC2OI   0x00000200
#define EP7312_INTR1_RTCMI   0x00000400
#define EP7312_INTR1_TINT    0x00000800
#define EP7312_INTR1_URXINT1 0x00001000
#define EP7312_INTR1_UTXINT1 0x00002000
#define EP7312_INTR1_UMSINT  0x00004000
#define EP7312_INTR1_SSEOTI  0x00008000

/* INTR2 (Interrupt 2) mask/status register bits */
#define EP7312_INTR2_KBDINT  0x00000001
#define EP7312_INTR2_SS2RX   0x00000002
#define EP7312_INTR2_SS2TX   0x00000004
#define EP7312_INTR2_URXINT2 0x00001000
#define EP7312_INTR2_UTXINT2 0x00002000

/* INTR3 (Interrupt 3) mask/status register bits */
#define EP7312_INTR2_DAIINT  0x00000001

#endif /* __EP7312_H__ */
