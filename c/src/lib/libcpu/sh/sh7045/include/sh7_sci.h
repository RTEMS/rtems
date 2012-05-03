/*
 * Bit values for the serial control registers of the Hitachi SH704X
 *
 * From Hitachi tutorials
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _sh7_sci_h
#define _sh7_sci_h

#include <rtems/score/iosh7045.h>

/*
 * Serial mode register bits
 */

#define SCI_SYNC_MODE               0x80
#define SCI_SEVEN_BIT_DATA          0x40
#define SCI_PARITY_ON               0x20
#define SCI_ODD_PARITY              0x10
#define SCI_STOP_BITS_2             0x08
#define SCI_ENABLE_MULTIP           0x04
#define SCI_PHI_64                  0x03
#define SCI_PHI_16                  0x02
#define SCI_PHI_4                   0x01
#define SCI_PHI_0                   0x00

/*
 * Serial register offsets, relative to SCI0_SMR or SCI1_SMR
 */

#define SCI_SMR			0x00
#define SCI_BRR			0x01
#define SCI_SCR			0x02
#define SCI_TDR			0x03
#define SCI_SSR			0x04
#define SCI_RDR			0x05

/*
 * Serial control register bits
 */
#define SCI_TIE                 0x80    /* Transmit interrupt enable */
#define SCI_RIE                 0x40    /* Receive interrupt enable */
#define SCI_TE                  0x20    /* Transmit enable */
#define SCI_RE                  0x10    /* Receive enable */
#define SCI_MPIE                0x08    /* Multiprocessor interrupt enable */
#define SCI_TEIE                0x04    /* Transmit end interrupt enable */
#define SCI_CKE1                0x02    /* Clock enable 1 */
#define SCI_CKE0                0x01    /* Clock enable 0 */

/*
 * Serial status register bits
 */
#define SCI_TDRE                0x80    /* Transmit data register empty */
#define SCI_RDRF                0x40    /* Receive data register full */
#define SCI_ORER                0x20    /* Overrun error */
#define SCI_FER                 0x10    /* Framing error */
#define SCI_PER                 0x08    /* Parity error */
#define SCI_TEND                0x04    /* Transmit end */
#define SCI_MPB                 0x02    /* Multiprocessor bit */
#define SCI_MPBT                0x01    /* Multiprocessor bit transfer */

/*
 * INTC Priority Settings
 */

#define SCI0_IPMSK	0x00F0
#define SCI0_LOWIP	0x0010
#define SCI1_IPMSK	0x000F
#define SCI1_LOWIP	0x0001

#endif /* _sh7_sci_h */
