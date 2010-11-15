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

#include <assert.h>

#include "bestcomm_api.h"
#include "bestcomm_glue.h"
#include "task_api/tasksetup_bdtable.h"
#include "include/mgt5200/mgt5200.h"

#ifdef __MWERKS__
__declspec(section ".text") extern const uint32 taskTable;
__declspec(section ".text") extern const uint32 taskTableBytes;
__declspec(section ".text") extern const uint32 taskTableTasks;
__declspec(section ".text") extern const uint32 offsetEntry;
#else
extern const uint32 taskTable;
extern const uint32 taskTableBytes;
extern const uint32 taskTableTasks;
extern const uint32 offsetEntry;
#endif

TaskBDIdxTable_t TaskBDIdxTable[MAX_TASKS];

void TaskSetup_BDTable(volatile uint32 *BasePtr, volatile uint32 *LastPtr, volatile uint32 *StartPtr,
					   int TaskNum, uint32 NumBD, uint16 MaxBD,
					   uint8 NumPtr, ApiConfig_t ApiConfig, uint32 Status)
{
	int i, j;
	uint32 *ptr;

	/*
	 * First time through the Buffer Descriptor table configuration
	 * set the buffer descriptor table with parameters that will not
	 * change since they are determined by the task itself.	The
	 * SramOffsetGlobal variable must be updated to reflect the new SRAM
	 * space used by the buffer descriptor table.  The next time through
	 * this function (i.e. TaskSetup called again) the only parameters
	 * that should be changed are the LastPtr pointers and the NumBD part
	 * of the table.
	 */
	if (TaskBDIdxTable[TaskNum].BDTablePtr == 0) {
		size_t AllocSize = 0;
		void *AllocBegin = NULL;

		switch (NumPtr) {
			case 1:
				AllocSize += MaxBD*sizeof(TaskBD1_t);
				break;
			case 2:
				AllocSize += MaxBD*sizeof(TaskBD2_t);
				break;
			default:
				assert(0);
				break;
		}

		AllocBegin = bestcomm_malloc(AllocSize);
		assert(AllocBegin != NULL);

		TaskBDIdxTable[TaskNum].BDTablePtr  = AllocBegin;
		TaskBDIdxTable[TaskNum].numPtr      = NumPtr;
		TaskBDIdxTable[TaskNum].apiConfig   = ApiConfig;
		TaskBDIdxTable[TaskNum].BDStartPtr  = StartPtr;

		*StartPtr = *BasePtr  = (uint32)((uint32)TaskBDIdxTable[TaskNum].BDTablePtr
			 		+ MBarPhysOffsetGlobal);
	}

	TaskBDIdxTable[TaskNum].currBDInUse	= 0;
	TaskBDIdxTable[TaskNum].numBD		= (uint16)NumBD;
	switch (NumPtr) {
		case 1:
			*LastPtr = (uint32)(*BasePtr + sizeof(TaskBD1_t) * (NumBD - 1));
			break;
		case 2:
			*LastPtr = (uint32)(*BasePtr + sizeof(TaskBD2_t) * (NumBD - 1));
			break;
		default:
			/* error */
			break;
	}

	/*
	 * Set the status bits. Clear the data pointers.
	 */
	if (MaxBD > 0) {
		ptr = TaskBDIdxTable[TaskNum].BDTablePtr;
		for (i = 0; i < NumBD; i++) {
			*(ptr++) = Status;
			for (j = 0; j < NumPtr; j++) {
				*(ptr++) = 0x0;
			}
		}
	}
}
