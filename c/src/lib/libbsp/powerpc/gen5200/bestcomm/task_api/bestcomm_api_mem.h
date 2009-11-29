#ifndef __TASK_API_BESTCOMM_API_MEM_H
#define __TASK_API_BESTCOMM_API_MEM_H 1

/******************************************************************************
*
* Copyright (c) 2004 Freescale Semiconductor, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/

#include "../include/mgt5200/mgt5200.h"

/*
 * An extern global variable is used here for the MBAR since it must
 * be passed into the API for processes that use virtual memory.
 */
extern uint8 *MBarGlobal;

#define SDMA_TASK_BAR      (MBarGlobal+MBAR_SDMA+0x000)
#define SDMA_INT_PEND      (MBarGlobal+MBAR_SDMA+0x014)
#define SDMA_INT_MASK      (MBarGlobal+MBAR_SDMA+0x018)
#define SDMA_TCR           (MBarGlobal+MBAR_SDMA+0x01C)
#define SDMA_TASK_SIZE     (MBarGlobal+MBAR_SDMA+0x060)

#define PCI_TX_PKT_SIZE    (MBarGlobal+MBAR_SCPCI+0x000)
#define PCI_TX_NTBIT       (MBarGlobal+MBAR_SCPCI+0x01C)
#define PCI_TX_FIFO        (MBarGlobal+MBAR_SCPCI+0x040)
#define PCI_TX_FIFO_STAT   (MBarGlobal+MBAR_SCPCI+0x045)
#define PCI_TX_FIFO_GRAN   (MBarGlobal+MBAR_SCPCI+0x048)
#define PCI_TX_FIFO_ALARM  (MBarGlobal+MBAR_SCPCI+0x04E)

#define PCI_RX_PKT_SIZE    (MBarGlobal+MBAR_SCPCI+0x080)
#define PCI_RX_NTBIT       (MBarGlobal+MBAR_SCPCI+0x09C)
#define PCI_RX_FIFO        (MBarGlobal+MBAR_SCPCI+0x0C0)
#define PCI_RX_FIFO_STAT   (MBarGlobal+MBAR_SCPCI+0x0C5)
#define PCI_RX_FIFO_GRAN   (MBarGlobal+MBAR_SCPCI+0x0C8)
#define PCI_RX_FIFO_ALARM  (MBarGlobal+MBAR_SCPCI+0x0CE)


#define FEC_RX_FIFO        (MBarGlobal+MBAR_ETHERNET+0x184)
#define FEC_RX_FIFO_STAT   (MBarGlobal+MBAR_ETHERNET+0x188)
#define FEC_RX_FIFO_GRAN   (MBarGlobal+MBAR_ETHERNET+0x18C)
#define FEC_RX_FIFO_ALARM  (MBarGlobal+MBAR_ETHERNET+0x198)

#define FEC_TX_FIFO        (MBarGlobal+MBAR_ETHERNET+0x1A4)
#define FEC_TX_FIFO_STAT   (MBarGlobal+MBAR_ETHERNET+0x1A8)
#define FEC_TX_FIFO_GRAN   (MBarGlobal+MBAR_ETHERNET+0x1AC)
#define FEC_TX_FIFO_ALARM  (MBarGlobal+MBAR_ETHERNET+0x1B8)

#endif	/* __TASK_API_BESTCOMM_API_MEM_H */
