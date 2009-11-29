#ifndef __TASK_API_TASKSETUP_BDTABLE_H
#define __TASK_API_TASKSETUP_BDTABLE_H 1

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
 * Table of BD rings for all BestComm tasks indexed by task ID.
 *
 *    +-----+------+--------------+    +------+-------+
 * 0: |numBD|numPtr|BDTablePtr ---|--->|status|dataPtr|
 *    +-----+------+--------------+    +------+-------+
 * 1: |numBD|numPtr|BDTablePtr    |    |status|dataPtr|
 *    +-----+------+--------------+    .      .       .
 * 2: |numBD|numPtr|BDTablePtr ---|-+  .      .       .
 *    .            .              . |  .      .       .
 *    .            .              . |  |status|dataPtr|
 *    .            .              . |  +------+-------+
 * 15:|numBD|numPtr|BDTablePtr    | |
 *    +-----+------+--------------+ |
 *                                  |
 *                                  V
 *                                  +------+--------+--------+
 *                                  |status|dataPtr0|dataPtr1|
 *                                  +------+--------+--------+
 *                                  |status|dataPtr0|dataPtr1|
 *                                  .      .        .        .
 *                                  .      .        .        .
 *                                  .      .        .        .
 *                                  |status|dataPtr0|dataPtr1|
 *                                  +------+--------+--------+
 */
typedef struct {
	uint16 numBD;		/* Size of BD ring									*/
	uint8  numPtr;		/* Number of data buffer pointers per BD			*/
	uint8  apiConfig;	/* API configuration flags							*/
	void   *BDTablePtr;	/* Pointer to BD tables, must be cast to TaskBD1_t	*/
						/*   or TaskBD2_t									*/
	volatile uint32
		   *BDStartPtr;	/* Task's current BD pointer. This pointer is
						 * used to set a task's BD pointer upon startup.
						 * It is only valid for BD tasks and only after
						 * TaskSetup() or TaskBDReset() are called. You
						 * cannot use this to track a task's BD pointer.
						 */
	uint16 currBDInUse; /* Current number of buffer descriptors assigned but*/
						/*   not released yet.                              */
} TaskBDIdxTable_t;

typedef enum {
	API_CONFIG_NONE		= 0x00,
	API_CONFIG_BD_FLAG	= 0x01
} ApiConfig_t;

/*
 * Allocates BD table if needed and updates the BD index table.
 * Do we want to hide this from the C API since it operates on task API?
 */
void TaskSetup_BDTable(volatile uint32 *BasePtr,
                       volatile uint32 *LastPtr,
                       volatile uint32 *StartPtr,
                       int TaskNum, uint32 NumBD, uint16 MaxBD,
                       uint8 NumPtr, ApiConfig_t ApiConfig, uint32 Status );

#endif	/* __TASK_API_TASKSETUP_BDTABLE_H */
