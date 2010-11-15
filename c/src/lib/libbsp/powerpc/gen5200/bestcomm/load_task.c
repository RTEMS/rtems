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

#if defined(__rtems__) || defined(MPC5200_BAPI_LIBC_HEADERS)
#include <string.h>
#endif

#include "include/ppctypes.h"
#include "include/mgt5200/sdma.h"
#include "include/mgt5200/mgt5200.h"

#include "dma_image.h"
#include "bestcomm_api.h"

#ifdef __MWERKS__
__declspec(section ".text") extern const uint32 taskTable;
__declspec(section ".text") extern const uint32 taskTableBytes;
__declspec(section ".text") extern const uint32 taskTableTasks;
__declspec(section ".text") extern const uint32 offsetEntry;
#else
extern const uint32 taskTable [];
extern const uint32 taskTableBytes;
extern const uint32 taskTableTasks;
extern const uint32 offsetEntry;
#endif

typedef struct SCTDT {
	uint32 start;
	uint32 stop;
	uint32 var;
	uint32 fdt;
	uint32 rsvd1;
	uint32 rsvd2;
	uint32 context;
	uint32 litbase;
} SCTDT_T;


/*!
 * \brief	Load BestComm tasks into SRAM.
 * \param	sdma	Base address of the BestComm register set
 *
 * The BestComm tasks must be loaded before any task can be setup,
 * enabled, etc. This might be called as part of a boot sequence before
 * any BestComm drivers are required.
 */
void TasksLoadImage(sdma_regs *sdma)
{
	uint32 i;
	SCTDT_T *tt;

	/* copy task table from source to destination */
	memcpy((void *)((uint8 *)(sdma->taskBar) - MBarPhysOffsetGlobal), taskTable, taskTableBytes);
	/* adjust addresses in task table */
	for (i=0; i < (uint32) taskTableTasks; i++) {
		tt = (SCTDT_T *)(((uint8 *)(sdma->taskBar) - MBarPhysOffsetGlobal) + (uint32) offsetEntry + (i * sizeof (SCTDT_T)));
		tt->start 	+= sdma->taskBar;
		tt->stop 	+= sdma->taskBar;
		tt->var		+= sdma->taskBar;
		tt->fdt		= (sdma->taskBar & 0xFFFFFF00) + tt->fdt;
		tt->context	+= sdma->taskBar;
	}
}
