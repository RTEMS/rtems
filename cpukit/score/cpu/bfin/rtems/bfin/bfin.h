/*  bfin.h
 *
 *  This file defines Macros for MMR register common to all Blackfin
 *  Processors.
 *
 *  COPYRIGHT (c) 2006 by Atos Automacao Industrial Ltda.
 *             modified by Alain Schaefer <alain.schaefer@easc.ch>
 *                     and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_BFIN_BFIN_H
#define _RTEMS_BFIN_BFIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Scratchpad SRAM */

#define SCRATCH                0xFFB00000
#define SCRATCH_SIZE           0x1000
#define SCRATCH_TOP            0xFFB00ffc


/* System Interrupt Controller Chapter 4*/
#define SIC_RVECT              0xFFC00108
#define SIC_IMASK              0xFFC0010C
#define SIC_IAR0               0xFFC00110
#define SIC_IAR1               0xFFC00114
#define SIC_IAR2               0xFFC00118
#define SIC_ISR                0xFFC00120
#define SIC_IWR                0xFFC00124

/* Event Vector Table        Chapter 4 */

#define EVT0                   0xFFE02000
#define EVT1                   0xFFE02004
#define EVT2                   0xFFE02008
#define EVT3                   0xFFE0200C
#define EVT4                   0xFFE02010
#define EVT5                   0xFFE02014
#define EVT6                   0xFFE02018
#define EVT7                   0xFFE0201C
#define EVT8                   0xFFE02020
#define EVT9                   0xFFE02024
#define EVT10                  0xFFE02028
#define EVT11                  0xFFE0202C
#define EVT12                  0xFFE02030
#define EVT13                  0xFFE02034
#define EVT14                  0xFFE02038
#define EVT15                  0xFFE0203C
#define IMASK                  0xFFE02104
#define IPEND                  0xFFE02108
#define ILAT                   0xFFE0210C
#define IPRIO                  0xFFE02110


#define TCNTL                  0xFFE03000
#define TPERIOD                0xFFE03004
#define TSCALE                 0xFFE03008
#define TCOUNT                 0xFFE0300C

/* Masks for Timer Control */
#define TMPWR                  0x00000001
#define TMREN                  0x00000002
#define TAUTORLD               0x00000004
#define TINT                   0x00000008

/* Event Bit Positions */
#define EVT_IVTMR_P            0x00000006

#define EVT_IVTMR              (1 << EVT_IVTMR_P)

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_BFIN_H */
