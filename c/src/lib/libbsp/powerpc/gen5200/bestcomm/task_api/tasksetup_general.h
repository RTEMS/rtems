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

/*
 * Task builder generates a set #defines per configured task to
 * condition this templete file.
 */

/**********************************************************
 *
 * Required #defines:
 * ------------------
 * TASKSETUP_NAME:
 *   TaskSetup function name, set to TaskSetup_<TASK_NAME>
 * TASK_API:
 *   task API defined in dma_image.h
 * MAX_BD:
 *   <=0 : non-BD task
 *   else: number of BD in BD table
 * BD_FLAG:
 *   0   : no flag implemented for BD
 *   else: flags can be passed on a per BD basis
 * MISALIGNED:
 *   0   : task API supports Bytes%IncrBytes==0
 *   else: task API supports any parameter settings
 * AUTO_START:
 *   <-1       : do not start a task after task completion
 *   -1        : auto start the task after task completion
 *   <MAX_TASKS: auto start task with the TaskID = AUTO_START
 *    else     : do not start a task after task completion
 * INITIATOR_DATA:
 *   <0  : runtime configurable
 *   else: assume INITATOR_DATA equal hard-coded task initiator
 * TYPE_SRC: (needs to be consistent with Task API)
 *   FLEX_T  : Task API TYPE_SRC = flex, SzSrc defines size
 *   UINT8_T : Task API TYPE_SRC = char
 *   UINT16_T: Task API TASK_SRC = short
 *   UINT32_T: Task API TASK_SRC = int
 * INCR_TYPE_SRC:
 *   0   : FIFO address, do not implement data pointer
 *   1   : automatic, set INCR_SRC based on SzSrc parameter
 *   2   : runtime, set INCR_SRC to IncrSrc parameter
 *   else: used hard-coded INCR_SRC
 * INCR_SRC:
 *   INCR_TYPE_SRC=0: force INCR_SRC=0
 *   else           : use for src pointer increment
 * TYPE_DST: (needs to be consistent with Task API)
 *   FLEX_T  : Task API TYPE_DST = flex, SzDst defines size
 *   UINT8_T : Task API TYPE_DST = char
 *   UINT16_T: Task API TASK_DST = short
 *   UINT32_T: Task API TASK_DST = int
 * INCR_TYPE_DST:
 *   0   : FIFO address, do not implement data pointer
 *   1   : automatic, set INCR_DST based on SzDst parameter
 *   2   : runtime, set INCR_DST to IncrDst parameter
 *   else: used hard-coded INCR_DST
 * INCR_DST:
 *   INCR_TYPE_DST=0: force INCR_DST=0
 *   else           : use for dst pointer increment
 * PRECISE_INCREMENT:
 *   0   : increment when possible
 *   else: increment at end of iteration
 * NO_ERROR_RESET:
 *   0   : reset error flags on task enable
 *   else: leave error flags unmodified on task enable
 * PACK_DATA:
 *   0   : do not pack data
 *   else: pack data based on data type
 * INTEGER_MODE:
 *   0   : type conversions handle as fixed point numbers
 *   else: type conversions handle as integers
 * WRITE_LINE_BUFFER:
 *   0   : do not use write line buffers
 *   else: enable write line buffers
 * READ_LINE_BUFFER:
 *   0   : do not use read line buffers
 *   else: enable read line buffers
 * SPEC_READS:
 *   0   : do not speculatively read
 *   else: speculatively read data ahead of DMA engine
 *
 * Optional #defines:
 * ------------------
 * MAX_TASKS:
 *   1   : #define MAX_TASKS>0
 *   else: 16
 * ITERATIONS:
 *   1   : #define ITERATIONS>0
 *   else: 1
 * INCR_BYTES:
 *   This macro is defined based on following priority:
 *   1   : INCR_SRC != 0
 *   2   : DST_TYPE != 0
 *   3   : #defined INCR_BYTES<0
 *   else: -4 (SZ_UINT32)
 * DEBUG_BESTCOMM_API:
 *   >0  : print basic debug messages
 *   >=10: also print C-API interface variables
 *   >=20: also print task API interface variables
 *   else: do nothing
 *
 **********************************************************/

#if defined(__rtems__) || defined(MPC5200_BAPI_LIBC_HEADERS)
#include <stdlib.h>
#endif

#include "../dma_image.h"

#include "../bestcomm_api.h"
#include "tasksetup_bdtable.h"

#include "bestcomm_api_mem.h"
#include "bestcomm_cntrl.h"

#ifndef  DEBUG_BESTCOMM_API
 #define DEBUG_BESTCOMM_API 0
#endif

#ifdef FLEX_T
 #undef FLEX_T
#endif
#define FLEX_T SZ_FLEX

#ifdef UINT8_T
 #undef UINT8_T
#endif
#define UINT8_T SZ_UINT8

#ifdef UINT16_T
 #undef UINT16_T
#endif
#define UINT16_T SZ_UINT16

#ifdef UINT32_T
 #undef UINT32_T
#endif
#define UINT32_T SZ_UINT32

#if (INCR_TYPE_SRC==0)   /* FIFO address, no data pointer */
 #undef  INCR_SRC
 #define INCR_SRC     0
#endif

#if (INCR_TYPE_DST==0)   /* FIFO address, no data pointer */
 #undef  INCR_DST
 #define INCR_DST     0
#endif

#ifndef  MAX_TASKS
 #define MAX_TASKS   16
#else
 #if (MAX_TASKS<=0)
  #undef  MAX_TASKS
  #define MAX_TASKS  16
 #endif
#endif

#ifndef  ITERATIONS
 #define ITERATIONS   1
#else
 #if (ITERATIONS<=0)
  #undef  ITERATIONS
  #define ITERATIONS  1
 #endif
#endif

#ifndef   INCR_BYTES
  #define INCR_BYTES -4
#else
 #if (INCR_BYTES>=0)
  #undef  INCR_BYTES
  #define INCR_BYTES -4
 #endif
#endif

/*
 * These ifndefs will go away when support in task_capi wrappers
 * in the image directories
 */
#ifndef  PRECISE_INCREMENT
 #define PRECISE_INCREMENT 0  /* bit=6 SAS->1, increment 0=when possible, 1=at the end of interation */
#endif
#ifndef  NO_ERROR_RESET
 #define NO_ERROR_RESET    0  /* bit=5 SAS->0, do not reset error codes on task enable               */
#endif
#ifndef  PACK_DATA
 #define PACK_DATA         0  /* bit=4 SAS->0, pack data enable                                      */
#endif
#ifndef  INTEGER_MODE
 #define INTEGER_MODE      0  /* bit=3 SAS->0, 0=fractional(msb aligned), 1=integer(lsb aligned)     */
#endif
#ifndef  SPEC_READS
 #define SPEC_READS        1  /* bit=2 SAS->0, XLB speculative read enable                           */
#endif
#ifndef  WRITE_LINE_BUFFER
 #define WRITE_LINE_BUFFER 1  /* bit=1 SAS->0, write line buffer enable                              */
#endif
#ifndef  READ_LINE_BUFFER
 #define READ_LINE_BUFFER  1  /* bit=0 SAS->0, read line buffer enable                               */
#endif
#define  SDMA_PRAGMA (0                <<SDMA_PRAGMA_BIT_RSV         ) | \
                     (PRECISE_INCREMENT<<SDMA_PRAGMA_BIT_PRECISE_INC ) | \
                     (NO_ERROR_RESET   <<SDMA_PRAGMA_BIT_RST_ERROR_NO) | \
                     (PACK_DATA        <<SDMA_PRAGMA_BIT_PACK        ) | \
                     (INTEGER_MODE     <<SDMA_PRAGMA_BIT_INTEGER     ) | \
                     (SPEC_READS       <<SDMA_PRAGMA_BIT_SPECREAD    ) | \
                     (WRITE_LINE_BUFFER<<SDMA_PRAGMA_BIT_CW          ) | \
                     (READ_LINE_BUFFER <<SDMA_PRAGMA_BIT_RL          )

#ifndef  TASKSETUP_NAME
 #define PREPEND_TASKSETUP(name) TaskSetup_ ## name
 #define FUNC_PREPEND_TASKSETUP(name) PREPEND_TASKSETUP(name)
 #define TASKSETUP_NAME FUNC_PREPEND_TASKSETUP(TASK_BASE)
#endif

#ifndef  TASK_API
 #define APPEND_API(name) name ## _api_t
 #define FUNC_APPEND_API(name) APPEND_API(name)
 #define TASK_API FUNC_APPEND_API(TASK_BASE)
#endif

#ifndef  INIT_DMA_IMAGE
 #define PREPEND_INITDMA(name) init_dma_image_ ## name
 #define FUNC_PREPEND_INITDMA(name) PREPEND_INITDMA(name)
 #define INIT_DMA_IMAGE FUNC_PREPEND_INITDMA(TASK_BASE)
#endif

#define DRD_INIT_MASK	0xfc1fffff
#define DRD_EXT_FLAG	0x40000000
#define DRD_INIT_OFFSET	21

TaskId TASKSETUP_NAME(TASK_API            *TaskAPI,
                      TaskSetupParamSet_t *TaskSetupParams)
{
	TaskId TaskNum;
#if ((MAX_BD>0)||(DEBUG_BESTCOMM_API>0))
	uint32 Status = 0;
#endif
#if ((MAX_BD>0)&&((INCR_TYPE_SRC!=0)||(INCR_TYPE_DST!=0))||(DEBUG_BESTCOMM_API>0))
	uint8  NumPtr = 0;
#endif
#if (INITIATOR_DATA<0)	/* runtime configurable */
	uint32 i, ext;
#endif

	INIT_DMA_IMAGE((uint8 *)(((sdma_regs *)(SDMA_TASK_BAR))->taskBar), MBarPhysOffsetGlobal);

	TaskNum = (TaskId)SDMA_TASKNUM_EXT(TaskAPI->TaskNum);

	TaskRunning[TaskNum] = 0;

#if (DEBUG_BESTCOMM_API>0)
	printf("\nBestComm API Debug Display Mode Enabled\n\n");
	printf("TaskSetup: TaskID=%d\n", TaskNum);
	if (Status!=0) {
		printf("TaskSetup: Rx task\n");
	} else {
		printf("TaskSetup: Tx or DP task\n");
	}
#endif

	/* Set the task pragma settings */
	*(TaskAPI->TaskPragma)= (uint8) SDMA_PRAGMA;

#if (MAX_BD>0)	/* Buffer Descriptors */

 #if (INCR_TYPE_SRC!=0)
	++NumPtr;
 #endif
 #if (INCR_TYPE_DST!=0)
	++NumPtr;
 #endif

 #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Using %d buffer descriptors, each with %d data pointers\n", MAX_BD, NumPtr);
 #endif

	/* Allocate BD table SRAM storage,
	 * and pass addresses to task API */

	TaskSetup_BDTable(TaskAPI->BDTableBase,
					  TaskAPI->BDTableLast,
					  TaskAPI->BDTableStart,
					  TaskNum,
					  TaskSetupParams->NumBD,
					  MAX_BD, NumPtr,
					  BD_FLAG, Status);

	*TaskAPI->AddrEnable = (uint32)((uint32)(((uint16 *)SDMA_TCR)+TaskNum) + MBarPhysOffsetGlobal);

 #if BD_FLAG

  #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Buffer descriptor flags are enabled\n");
  #endif

	/* always assume 2nd to last DRD */
	*((TaskAPI->AddrDRD)) = (uint32)((uint32)TaskAPI->DRD[TaskAPI->AddrDRDIdx] + MBarPhysOffsetGlobal);
 #endif /* #if BD_FLAG */

#else	/* No Buffer Descriptors */

/* #error ATA should not be non-BD */

  #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Task will complete %d iterations before disabling\n");
  #endif

	*((TaskAPI->IterExtra)) = (uint32)(ITERATIONS-1);
#endif	/* #if (MAX_BD>0) */

/* Setup auto start */
#if (AUTO_START <= -2 )	/* do not start a task */
 #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Auto task start disabled\n");
 #endif
	SDMA_TASK_CFG(SDMA_TCR, TaskNum, 0, TaskNum);
#elif (AUTO_START <= -1 )	/* restart task */
 #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Auto start task\n");
 #endif
	SDMA_TASK_CFG(SDMA_TCR, TaskNum, 1, TaskNum);
#elif (AUTO_START < MAX_TASKS)	/* start specific task */
 #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Auto start task with TaskID=%d\n", AUTO_START);
 #endif
	SDMA_TASK_CFG(SDMA_TCR, TaskNum, 1, AUTO_START);
#else	/* do not start a task */
 #if (DEBUG_BESTCOMM_API>0)
	printf("TaskSetup: Auto task start disabled\n");
 #endif
	SDMA_TASK_CFG(SDMA_TCR, TaskNum, 0, TaskNum);
#endif

#if (INITIATOR_DATA<0)	/* runtime configurable */
	SDMA_SET_INIT(SDMA_TCR, TaskNum, TaskSetupParams->Initiator);

	/*
     * Hard-code the task initiator in the DRD to avoid a problem w/ the
     * hold initiator bit in the TCR.
	 */
	ext = 0;
	for (i = 0; i < TaskAPI->NumDRD; i++) {
		if (ext == 0)
		{
#if (DEBUG_BESTCOMM_API>=10)
				printf("TaskSetup: DRD[%d] initiator = %d\n", i, ((*(TaskAPI->DRD[i]) & ~DRD_INIT_MASK) >> DRD_INIT_OFFSET));
#endif
			if (((*(TaskAPI->DRD[i]) & ~DRD_INIT_MASK) >> DRD_INIT_OFFSET) != INITIATOR_ALWAYS) {
#if (DEBUG_BESTCOMM_API>=10)
				printf("TaskSetup: Replacing DRD[%d] initiator with %d\n", i, TaskSetupParams->Initiator);
#endif
				*(TaskAPI->DRD[i]) = (*(TaskAPI->DRD[i]) & DRD_INIT_MASK)
							| (TaskSetupParams->Initiator << DRD_INIT_OFFSET);
			}

			if ((*(TaskAPI->DRD[i]) & DRD_EXT_FLAG) != 0)
			{
				ext = 1;
			}
		}
		else
		{
			if ((*(TaskAPI->DRD[i]) & DRD_EXT_FLAG) == 0)
			{
				ext = 0;
			}
		}
	}

#else	/* INITIATOR_DATA >= 0 */
	TaskSetupParams->Initiator = INITIATOR_DATA;
#endif

#if (DEBUG_BESTCOMM_API>=10)
	printf("\nTaskSetup: C-API Parameter Settings Passed to TaskSetup:\n");
	printf("TaskSetup: NumBD        = %d\n", TaskSetupParams->NumBD);
 #if (MAX_BD>0)
	printf("TaskSetup: MaxBuf       = %d\n", TaskSetupParams->Size.MaxBuf);
 #else
	printf("TaskSetup: NumBytes     = %d\n", TaskSetupParams->Size.NumBytes);
 #endif
	printf("TaskSetup: Initiator    = %d\n", TaskSetupParams->Initiator);
	printf("TaskSetup: StartAddrSrc = 0x%08X\n", TaskSetupParams->StartAddrSrc);
	printf("TaskSetup: IncrSrc      = %d\n", TaskSetupParams->IncrSrc);
	printf("TaskSetup: SzSrc        = %d\n", TaskSetupParams->SzSrc);
	printf("TaskSetup: StartAddrDst = 0x%08X\n", TaskSetupParams->StartAddrDst);
	printf("TaskSetup: IncrDst      = %d\n", TaskSetupParams->IncrDst);
	printf("TaskSetup: SzDst        = %d\n", TaskSetupParams->SzDst);
#endif

#if (DEBUG_BESTCOMM_API>=20)
	printf("\nTaskSetup: Task-API Parameter Settings Before TaskSetup Initialization:\n");
	printf("TaskSetup: TaskNum      = %d\n", (TaskAPI->TaskNum));
	printf("TaskSetup: TaskPragma   = 0x%02X\n", *((TaskAPI->TaskPragma)));
	printf("TaskSetup: TCR          = 0x%04x\n", SDMA_TASK_STATUS(SDMA_TCR, TaskNum));

 #if (MAX_BD>0)
	printf("TaskSetup: BDTableBase  = 0x%08X\n", *((TaskAPI->BDTableBase)));
	printf("TaskSetup: BDTableLast  = 0x%08X\n", *((TaskAPI->BDTableLast)));
	printf("TaskSetup: BDTableStart = 0x%08X\n", *((TaskAPI->BDTableStart)));
	printf("TaskSetup: AddrEnable   = 0x%08X\n", *((TaskAPI->AddrEnable)));
  #if (INCR_TYPE_SRC==0)
	printf("TaskSetup: AddrSrcFIFO  = 0x%08X\n", *((TaskAPI->AddrSrcFIFO)));
  #endif
  #if (INCR_TYPE_DST==0)
	printf("TaskSetup: AddrDstFIFO  = 0x%08X\n", *((TaskAPI->AddrDstFIFO)));
  #endif
  #if (BD_FLAG)
	printf("TaskSetup: AddrDRD      = 0x%08X\n", *((TaskAPI->AddrDRD)));
	printf("TaskSetup: AddrDRDIdx   = %d\n", ((TaskAPI->AddrDRDIdx)));
  #endif
 #else
	printf("TaskSetup: IterExtra    = %d\n", *((TaskAPI->IterExtra)));
  #if (INCR_TYPE_SRC==0)
	printf("TaskSetup: AddrSrcFIFO  = 0x%08X\n", *((TaskAPI->AddrSrcFIFO)));
  #else
	printf("TaskSetup: StartAddrSrc = 0x%08X\n", *((TaskAPI->StartAddrSrc)));
  #endif
  #if (INCR_TYPE_DST==0)
	printf("TaskSetup: AddrDstFIFO  = 0x%08X\n", *((TaskAPI->AddrDstFIFO)));
  #else
	printf("TaskSetup: StartAddrDst = 0x%08X\n", *((TaskAPI->StartAddrDst)));
  #endif
 #endif
 #if (INCR_TYPE_SRC!=0)
	printf("TaskSetup: IncrSrc      = 0x%04X\n", *((TaskAPI->IncrSrc)));
  #if (MISALIGNED | MISALIGNED_START)
	printf("TaskSetup: IncrSrcMA    = 0x%04X\n", *((TaskAPI->IncrSrcMA)));
  #endif
 #endif
 #if (INCR_TYPE_DST!=0)
	printf("TaskSetup: IncrDst      = 0x%04X\n", *((TaskAPI->IncrDst)));
  #if (MISALIGNED | MISALIGNED_START)
	printf("TaskSetup: IncrDstMA    = 0x%04X\n", *((TaskAPI->IncrDstMA)));
  #endif
 #endif
	printf("TaskSetup: Bytes        = %d\n", *((TaskAPI->Bytes)));
	printf("TaskSetup: IncrBytes    = %d\n", *((TaskAPI->IncrBytes)));
#endif


	*((TaskAPI->Bytes)) = (uint32)TaskSetupParams->Size.MaxBuf;


#if (TYPE_SRC!=FLEX_T)	/* size fixed in task code */
	TaskSetupParams->SzSrc = TYPE_SRC;
#endif

#if (INCR_TYPE_SRC==0)	/* no data pointer */
	TaskSetupParams->IncrSrc = (sint16)0;
	*((TaskAPI->AddrSrcFIFO)) = (uint32)TaskSetupParams->StartAddrSrc;
#else

 #if (INCR_TYPE_SRC==1)	/* automatic */
	if (TaskSetupParams->IncrSrc!=0) {
		TaskSetupParams->IncrSrc = (sint16)+TaskSetupParams->SzSrc;
	} else {
		TaskSetupParams->IncrSrc = (sint16)+TaskSetupParams->IncrSrc;
	}
 #elif (INCR_TYPE_SRC!=2)	/* hard-coded */
	TaskSetupParams->IncrSrc = (sint16)INCR_SRC;
 #endif
 *((TaskAPI->IncrSrc)) = (sint16)TaskSetupParams->IncrSrc;

 #if (MAX_BD>0)	/* pointer in BD Table */
	/* pass back address of first BD */
	TaskSetupParams->StartAddrSrc = (uint32)TaskGetBDRing(TaskNum);
 #else
	*((TaskAPI->StartAddrSrc)) = (uint32)TaskSetupParams->StartAddrSrc;
 #endif

 #if MISALIGNED | MISALIGNED_START
	if (TaskSetupParams->IncrSrc < 0) {
		*((TaskAPI->IncrSrcMA)) = (sint16)-1;
	} else if (TaskSetupParams->IncrSrc > 0) {
		*((TaskAPI->IncrSrcMA)) = (sint16)+1;
	} else {
		*((TaskAPI->IncrSrcMA)) = (sint16)0;
	}
 #endif
#endif


#if (TYPE_DST!=FLEX_T)	/* size fixed in task code */
	TaskSetupParams->SzDst = TYPE_DST;
#endif

#if (INCR_TYPE_DST==0)	/* no data pointer */
	TaskSetupParams->IncrDst = (sint16)0;
	*((TaskAPI->AddrDstFIFO)) = (uint32)TaskSetupParams->StartAddrDst;
#else
 #if (INCR_TYPE_DST==1)	/* automatic */
	if (TaskSetupParams->IncrDst!=0) {
		TaskSetupParams->IncrDst = (sint16)+TaskSetupParams->SzDst;
	} else {
		TaskSetupParams->IncrDst = (sint16)+TaskSetupParams->IncrDst;
	}
 #elif (INCR_TYPE_DST!=2)	/* hard-coded */
	TaskSetupParams->IncrDst = (sint16)INCR_DST;
 #endif
	*((TaskAPI->IncrDst)) = (sint16)TaskSetupParams->IncrDst;

 #if (MAX_BD>0)
	/* pass back address of first BD */
	TaskSetupParams->StartAddrDst = (uint32)TaskGetBDRing(TaskNum);
 #else
	*((TaskAPI->StartAddrDst)) = (uint32)TaskSetupParams->StartAddrDst;
 #endif

 #if MISALIGNED | MISALIGNED_START
	if (TaskSetupParams->IncrDst < 0) {
		*((TaskAPI->IncrDstMA)) = (sint16)-1;
	} else if (TaskSetupParams->IncrDst > 0) {
		*((TaskAPI->IncrDstMA)) = (sint16)+1;
	} else {
		*((TaskAPI->IncrDstMA)) = (sint16)0;
	}
 #endif
#endif

/* always use macro, only affect code with #define TYPE_? flex */
	SDMA_SET_SIZE(SDMA_TASK_SIZE, TaskNum, TaskSetupParams->SzSrc, TaskSetupParams->SzDst);


	if (TaskSetupParams->IncrSrc != 0) {
		*((TaskAPI->IncrBytes)) = (sint16)-abs(TaskSetupParams->IncrSrc);
	} else if (TaskSetupParams->IncrDst != 0) {
		*((TaskAPI->IncrBytes)) = (sint16)-abs(TaskSetupParams->IncrDst);
	} else {
		*((TaskAPI->IncrBytes)) = (sint16)-abs(INCR_BYTES);
	}


#if (DEBUG_BESTCOMM_API>=10)
	printf("\nTaskSetup: C-API Parameter Settings Returned from TaskSetup:\n");
	printf("TaskSetup: NumBD        = %d\n", TaskSetupParams->NumBD);
 #if (MAX_BD>0)
	printf("TaskSetup: MaxBuf       = %d\n", TaskSetupParams->Size.MaxBuf);
 #else
	printf("TaskSetup: NumBytes     = %d\n", TaskSetupParams->Size.NumBytes);
 #endif
	printf("TaskSetup: Initiator    = %d\n", TaskSetupParams->Initiator);
	printf("TaskSetup: StartAddrSrc = 0x%08X\n", TaskSetupParams->StartAddrSrc);
	printf("TaskSetup: IncrSrc      = %d\n", TaskSetupParams->IncrSrc);
	printf("TaskSetup: SzSrc        = %d\n", TaskSetupParams->SzSrc);
	printf("TaskSetup: StartAddrDst = 0x%08X\n", TaskSetupParams->StartAddrDst);
	printf("TaskSetup: IncrDst      = %d\n", TaskSetupParams->IncrDst);
	printf("TaskSetup: SzDst        = %d\n", TaskSetupParams->SzDst);
#endif

#if (DEBUG_BESTCOMM_API>=20)
	printf("\nTaskSetup: Task-API Parameter Settings After TaskSetup Initialization:\n");
	printf("TaskSetup: TaskNum      = %d\n", ((TaskAPI->TaskNum)));
	printf("TaskSetup: TaskPragma   = 0x%02X\n", *((TaskAPI->TaskPragma)));

 #if (MAX_BD>0)
	printf("TaskSetup: BDTableBase  = 0x%08X\n", *((TaskAPI->BDTableBase)));
	printf("TaskSetup: BDTableLast  = 0x%08X\n", *((TaskAPI->BDTableLast)));
	printf("TaskSetup: BDTableStart = 0x%08X\n", *((TaskAPI->BDTableStart)));
	printf("TaskSetup: AddrEnable   = 0x%08X\n", *((TaskAPI->AddrEnable)));
  #if (INCR_TYPE_SRC==0)
	printf("TaskSetup: AddrSrcFIFO  = 0x%08X\n", *((TaskAPI->AddrSrcFIFO)));
  #endif
  #if (INCR_TYPE_DST==0)
	printf("TaskSetup: AddrDstFIFO  = 0x%08X\n", *((TaskAPI->AddrDstFIFO)));
  #endif
  #if (BD_FLAG)
	printf("TaskSetup: AddrDRD      = 0x%08X\n", *((TaskAPI->AddrDRD)));
	printf("TaskSetup: AddrDRDIdx   = %d\n", ((TaskAPI->AddrDRDIdx)));
  #endif
 #else
	printf("TaskSetup: IterExtra    = %d\n", *((TaskAPI->IterExtra)));
  #if (INCR_TYPE_SRC==0)
	printf("TaskSetup: AddrSrcFIFO  = 0x%08X\n", *((TaskAPI->AddrSrcFIFO)));
  #else
	printf("TaskSetup: StartAddrSrc = 0x%08X\n", *((TaskAPI->StartAddrSrc)));
  #endif
  #if (INCR_TYPE_DST==0)
	printf("TaskSetup: AddrDstFIFO  = 0x%08X\n", *((TaskAPI->AddrDstFIFO)));
  #else
	printf("TaskSetup: StartAddrDst = 0x%08X\n", *((TaskAPI->StartAddrDst)));
  #endif
 #endif
 #if (INCR_TYPE_SRC!=0)
	printf("TaskSetup: IncrSrc      = 0x%04X\n", *((TaskAPI->IncrSrc)));
  #if (MISALIGNED | MISALIGNED_START)
	printf("TaskSetup: IncrSrcMA    = 0x%04X\n", *((TaskAPI->IncrSrcMA)));
  #endif
 #endif
 #if (INCR_TYPE_DST!=0)
	printf("TaskSetup: IncrDst      = 0x%04X\n", *((TaskAPI->IncrDst)));
  #if (MISALIGNED | MISALIGNED_START)
	printf("TaskSetup: IncrDstMA    = 0x%04X\n", *((TaskAPI->IncrDstMA)));
  #endif
 #endif
	printf("TaskSetup: Bytes        = %d\n", *((TaskAPI->Bytes)));
	printf("TaskSetup: IncrBytes    = %d\n", *((TaskAPI->IncrBytes)));
#endif

	return TaskNum;
}
