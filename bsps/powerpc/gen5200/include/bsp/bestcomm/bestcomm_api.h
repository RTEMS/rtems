#ifndef __BESTCOMM_API_H
#define __BESTCOMM_API_H 1

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

/*!
 * \file	bestcomm_api.h
 *
 * Bestcomm_api.h is the only header necessary for inclusion by user
 * code. The include path the C compiler searches to find .h files
 * should contain bestcomm/capi and one of bestcomm/code_dma/image_*.
 * This second entry selects which set of BestComm tasks will be used.
 * Of course the appropriate files in image_* must also be compiled and
 * linked.
 */

#include <rtems.h>

#include "include/ppctypes.h"
#include "include/mgt5200/sdma.h"
#include "task_api/tasksetup_bdtable.h"
#include "task_api/bestcomm_cntrl.h"
#include "task_api/bestcomm_api_mem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * \brief	TaskSetup() debugging
 *
 * Define this macro as follows for debugging printf()s to see
 * what the API receives and sets from the TaskSetupParamSet_t
 * struct. Implemented in capi/task_api/tasksetup_general.h.
 *
 * \verbatim
 * >0  : print basic debug messages
 * >=10: also print C-API interface variables
 * >=20: also print task API interface variables
 * else: do nothing
 * \endverbatim
 */
#define DEBUG_BESTCOMM_API 0

/*!
 * \brief	Maximum number of tasks in the system.
 * This number is hardware-dependent and not user configuration.
 */
#define MAX_TASKS 16

/*
 * This may need to be removed in certain implementations.
 */
#ifndef NULL
# define NULL ((void *)0)
#endif	/* NULL */

typedef sint8 TaskId;
typedef sint32 BDIdx;

/*
 * Special "task IDs" for interrupt handling API functions
 */
/*! \brief	Debug interrupt "task ID" */
#define	DEBUG_INTR_ID	SDMA_INT_BIT_DBG

/*! \brief	TEA interrupt "task ID"	*/
#define TEA_INTR_ID		SDMA_INT_BIT_TEA

/*! \brief	Task start autostart enable */
#define TASK_AUTOSTART_ENABLE	1

/*! \brief	Task start autostart disable */
#define TASK_AUTOSTART_DISABLE	0

/*! \brief	Task start interrupt enable */
#define TASK_INTERRUPT_ENABLE	1

/*! \brief	Task start interrupt disable */
#define TASK_INTERRUPT_DISABLE	0

/*
 * Buffer descriptor flags to pass to TaskBDAssign().
 */
/*! \brief	Transmit frame done */
#define TASK_BD_TFD	(1 << SDMA_DRD_BIT_TFD)

/*! \brief	Interrupt on frame done	*/
#define TASK_BD_INT	(1 << SDMA_DRD_BIT_INT)

/*!
 * \brief	Data transfer size
 */
typedef enum {
	SZ_FLEX		= 3,	/*!< invalid for TaskSetupParamSet_t */
	SZ_UINT8	= 1,	/*!< 1-byte	*/
	SZ_UINT16	= 2,	/*!< 2-byte	*/
	SZ_UINT32	= 4		/*!< 4-byte	*/
} Sz_t;

/*!
 * \brief	API error codes
 */
typedef enum {
	TASK_ERR_NO_ERR			= -1,	/*!< No error					*/
	TASK_ERR_NO_INTR		= TASK_ERR_NO_ERR,
									/*!< No interrupt				*/
	TASK_ERR_INVALID_ARG	= -2,	/*!< Invalid function argument	*/
	TASK_ERR_BD_RING_FULL	= -3,	/*!< Buffer descriptor ring full*/
	TASK_ERR_API_ALREADY_INITIALIZED
							= -4,	/*!< API has already been initialized */
	TASK_ERR_SIZE_TOO_LARGE = -5,	/*!< Buffer descriptor cannot support size parameter */
	TASK_ERR_BD_RING_EMPTY	= -6,	/*!< Buffer descriptor ring is empty*/
	TASK_ERR_BD_BUSY		= -7,	/*!< The buffer descriptor is in use
										 by the BestComm			*/
	TASK_ERR_TASK_RUNNING	= -8	/*!< The task is running.		*/

} TaskErr_t;

/*!
 * \brief	BestComm initiators
 *
 * These are assigned by TaskSetup().
 */
typedef enum {

	INITIATOR_ALWAYS	=  0,
	INITIATOR_SCTMR_0	=  1,
	INITIATOR_SCTMR_1	=  2,
	INITIATOR_FEC_RX	=  3,
	INITIATOR_FEC_TX	=  4,
	INITIATOR_ATA_RX	=  5,
	INITIATOR_ATA_TX	=  6,
	INITIATOR_SCPCI_RX	=  7,
	INITIATOR_SCPCI_TX	=  8,
	INITIATOR_PSC3_RX	=  9,
	INITIATOR_PSC3_TX	= 10,
	INITIATOR_PSC2_RX	= 11,
	INITIATOR_PSC2_TX	= 12,
	INITIATOR_PSC1_RX	= 13,
	INITIATOR_PSC1_TX	= 14,
	INITIATOR_SCTMR_2	= 15,

	INITIATOR_SCLPC		= 16,
	INITIATOR_PSC5_RX	= 17,
	INITIATOR_PSC5_TX	= 18,
	INITIATOR_PSC4_RX	= 19,
	INITIATOR_PSC4_TX	= 20,
	INITIATOR_I2C2_RX	= 21,
	INITIATOR_I2C2_TX	= 22,
	INITIATOR_I2C1_RX	= 23,
	INITIATOR_I2C1_TX	= 24,
	INITIATOR_PSC6_RX	= 25,
	INITIATOR_PSC6_TX	= 26,
	INITIATOR_IRDA_RX	= 25,
	INITIATOR_IRDA_TX	= 26,
	INITIATOR_SCTMR_3	= 27,
	INITIATOR_SCTMR_4	= 28,
	INITIATOR_SCTMR_5	= 29,
	INITIATOR_SCTMR_6	= 30,
	INITIATOR_SCTMR_7	= 31

} MPC5200Initiator_t;

/*!
 * \brief	Parameters for TaskSetup()
 *
 * All parameters can be hard-coded by the task API. Hard-coded values
 * will be changed in the struct passed to TaskSetup() for the user to
 * examine later.
 */
typedef struct {
	uint32	NumBD;			/*!< Number of buffer descriptors				*/

	union {
	   uint32 MaxBuf;		/*!< Maximum buffer size						*/
	   uint32 NumBytes;		/*!< Number of bytes to transfer				*/
	} Size;					/*!< Buffer size union for BD and non-BD tasks	*/

	MPC5200Initiator_t
			Initiator;		/*!< BestComm initiator (ignored if hard-wired)	*/
	uint32	StartAddrSrc;	/*!< Address of the DMA source (e.g. a FIFO)	*/
	sint16	IncrSrc;		/*!< Amount to increment source pointer			*/
	Sz_t	SzSrc;			/*!< Size of source data access					*/
	uint32	StartAddrDst;	/*!< Address of the DMA destination (e.g. a FIFO) */
	sint16	IncrDst;		/*!< Amount to increment data pointer			*/
	Sz_t	SzDst;			/*!< Size of destination data access			*/
} TaskSetupParamSet_t;

/*!
 * \brief	Parameters for TaskDebug()
 *
 * TaskDebug() and the contents of this data structure are yet to be
 * determined.
 */
typedef struct {
	int dummy;				/* Some compilers don't like empty struct typedefs */
} TaskDebugParamSet_t;

/*!
 * \brief	Generic buffer descriptor.
 *
 * It is generally used as a pointer which should be cast to one of the
 * other BD types based on the number of buffers per descriptor.
 */
typedef struct {
	uint32 Status;			/*!< Status and length bits		*/
} TaskBD_t;

/*!
 * \brief	Single buffer descriptor.
 */
typedef struct {
	uint32 Status;			/*!< Status and length bits		*/
	uint32 DataPtr[1];		/*!< Pointer to data buffer		*/
} TaskBD1_t;

/*!
 * \brief	Dual buffer descriptor.
 */
typedef struct {
	uint32 Status;			/*!< Status and length bits		*/
	uint32 DataPtr[2];		/*!< Pointer to data buffers	*/
} TaskBD2_t;



/***************************
 * Start of API Prototypes
 ***************************/

#include "bestcomm_priv.h"
#include "dma_image.capi.h"

/*!
 * \brief	Initialize a single task.
 * \param	TaskName	Type of task to initialize. E.g. PCI transmit,
 *						ethernet receive, general purpose dual-pointer.
 *						Values expected can be found in the TaskName_t
 *						enum defined in dma_image.capi.h.
 * \param	TaskSetupParams	Task-specific parameters. The user must fill out
 *						the pertinent parts of a TaskSetupParamSet_t
 *						data structure.
 * \returns TaskId task identification token which is a required
 *			parameter for most other API functions.
 *
 * This function returns a task identification token which is a required
 * parameter for most other API functions.
 *
 * Certain values of the structure pointed to by TaskParams are set
 * as a side-effect based on task type. These may be examined after
 * a successful call to TaskSetup(). User-specified values may be
 * overridden.
 *
 * TaskId TaskSetup( TaskName_t TaskName,
 *                   TaskSetupParamSet_t *TaskSetupParams );
 */
#define			TaskSetupHelper(TaskName, TaskSetupParams)	\
				TaskSetup_ ## TaskName (TaskName ## _api, TaskSetupParams)
#define			TaskSetup(TaskName, TaskSetupParams) \
				TaskSetupHelper(TaskName, TaskSetupParams)

const char		*TaskVersion(void);

int				TasksInitAPI(uint8 *MBarRef);

int				TasksInitAPI_VM(uint8 *MBarRef, uint8 *MBarPhys);

void			TasksLoadImage(sdma_regs *sdma);
int				TasksAttachImage(sdma_regs *sdma);

int				TaskStart(TaskId taskId, uint32 autoStartEnable,
						  TaskId autoStartTask, uint32 intrEnable);
int				TaskStop(TaskId taskId);
static int		TaskStatus(TaskId taskId);
BDIdx			TaskBDAssign(TaskId taskId, void *buffer0, void *buffer1,
							 int size, uint32 bdFlags);
BDIdx			TaskBDRelease(TaskId taskId);
BDIdx			TaskBDReset(TaskId taskId);
static TaskBD_t	*TaskGetBD(TaskId taskId, BDIdx bd);
static TaskBD_t	*TaskGetBDRing(TaskId taskId);
int				TaskDebug(TaskId taskId, TaskDebugParamSet_t *paramSet);
static int		TaskIntClear(TaskId taskId);
static TaskId	TaskIntStatus(TaskId taskId);
static int		TaskIntPending(TaskId taskId);
static TaskId	TaskIntSource(void);
static uint16	TaskBDInUse(TaskId taskId);


/*!
 * \brief	Get the enable/disable status of a task.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	Boolean true indicates enabled or false indicates disabled
 *			or invalid taskId.
 */
static inline int TaskStatus(TaskId taskId)
{
	return SDMA_TASK_STATUS(SDMA_TCR, taskId) & 0x8000;
}

/*!
 * \brief	Return a pointer to a buffer descriptor at index BDIdx
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \param	bd		Buffer descriptor handle returned by
 *					TaskBDAssign() or TaskBDRelease().
 * \returns	Pointer to the requested buffer descriptor or NULL on error.
 *
 * The returned pointer should be cast to the appropriate buffer
 * descriptor type, TaskBD1_t or TaskBD2_t.
 */
static inline TaskBD_t *TaskGetBD(TaskId taskId, BDIdx bd)
{
	void *bdTab;

	bdTab = TaskBDIdxTable[taskId].BDTablePtr;
	if (TaskBDIdxTable[taskId].numPtr == 1) {
		return (TaskBD_t *)&(((TaskBD1_t *)bdTab)[bd]);
	} else {
		return (TaskBD_t *)&(((TaskBD2_t *)bdTab)[bd]);
	}
}

/*!
 * \brief	Return a pointer to the first buffer descriptor in the ring.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	Pointer to the array of buffer descriptors making up the
 *			ring or NULL on error.
 *
 * A device driver author may choose to use this in lieu of
 * TaskBDAssign()/TaskBDRelease() to get direct access to the
 * BD ring with the warning that the underlying data structure may change.
 * Use at one's own discretion.
 */
static inline TaskBD_t *TaskGetBDRing(TaskId taskId)
{
	return (TaskBD_t *) TaskBDIdxTable[taskId].BDTablePtr;
}

/*!
 * \brief	Clear the interrupt for a given BestComm task.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	TASK_ERR_NO_ERR (which is not really an error) for success
 */
static inline int TaskIntClear(TaskId taskId)
{
	SDMA_CLEAR_IEVENT(SDMA_INT_PEND, taskId);
	return TASK_ERR_NO_ERR;	/* success */
}

/*!
 * \brief	Get the interrupt status for a given task.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	TASK_ERR_NO_INTR (which is not really an error) for no interrupt
 *			pending, taskId for a regular interrupt, DEBUG_INTR_ID for
 *			a debug interrupt and TEA_INTR_ID for a TEA interrupt.
 *			\b Note: TaskIntStatus() may return 0, but this means that that
 *			taskId 0 is interrupt pending.
 */
static inline TaskId TaskIntStatus(TaskId taskId)
{
	uint32 pending;

	pending = SDMA_INT_PENDING(SDMA_INT_PEND, SDMA_INT_MASK);

	if (SDMA_INT_TEST(pending, taskId)) {
		return taskId;
	} else if (SDMA_INT_TEST(pending, DEBUG_INTR_ID)) {
		return DEBUG_INTR_ID;
	} else if (SDMA_INT_TEST(pending, TEA_INTR_ID)) {
		return TEA_INTR_ID;
	}

	return TASK_ERR_NO_INTR;
}

/*!
 * \brief	Get the interrupt pending status for a given task.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	0 if task does not have a pending interrupt. 1 if the task
 *			has an interrupt pending.
 */
static inline int TaskIntPending(TaskId taskId)
{
	uint32 pending;

	pending = SDMA_INT_PENDING(SDMA_INT_PEND, SDMA_INT_MASK);
	if (SDMA_INT_TEST(pending, taskId)) {
		return 1;
	} else {
		return 0;
	}
}

/*!
 * \brief	Returns the task ID of an interrupting BestComm task.
 * \returns	TASK_ERR_NO_INTR (which is not really an error) for no interrupt
 *			pending or the taskId of the interrupting task.
 *
 * The user must query TaskIntStatus() to discover if this is a debug
 * or TEA interrupt. This function is designed for use by an operating
 * system interrupt handler.
 */
static inline TaskId TaskIntSource(void)
{
	uint32 pending;
	uint32 mask = 1 << (MAX_TASKS - 1);
	TaskId i;

	pending = SDMA_INT_PENDING(SDMA_INT_PEND, SDMA_INT_MASK);

	if (SDMA_INT_TEST(pending, SDMA_INT_BIT_TEA)) {
		return (TaskId)SDMA_TEA_SOURCE(SDMA_INT_PEND);
	}

	for (i = (MAX_TASKS - 1); i >= 0; --i, mask >>= 1) {
		if (pending & mask) {
			return i;
		}
	}

	return TASK_ERR_NO_INTR;
}

/*!
 * \brief	Get a count of in-use buffer descriptors.
 * \param	taskId	Task handle passed back from a successful TaskSetup()
 * \returns	Count of the current number of BDs in use by the given task.
 */
static inline uint16 TaskBDInUse(TaskId taskId)
{
	return TaskBDIdxTable[taskId].currBDInUse;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __BESTCOMM_API_H */
