#ifndef __TASK_API_BESTCOMM_CNTRL_H
#define __TASK_API_BESTCOMM_CNTRL_H 1

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

/*******************************************************************************
 * Defines to control SmartDMA and its tasks. These defines are used for the
 * task build process to minimize disconnects at the task/driver interface.
 ******************************************************************************/

#define SDMA_INT_BIT_DBG             31          /* debug interrupt bit        */
#define SDMA_INT_BIT_TEA             28          /* TEA interrupt bit          */
#define SDMA_INT_BIT_TEA_TASK        24          /* lsb for TEA task number    */
#define SDMA_INT_BIT_IMPL            0x9000FFFF

#define SDMA_PTDCTRL_BIT_TEA         14          /* TEA detection enable bit   */

#define SDMA_TCR_BIT_AUTO            15          /* auto start bit             */
#define SDMA_TCR_BIT_HOLD             5          /* hold initiator bit         */

#define SDMA_STAT_BIT_ALARM          17
#define SDMA_FIFO_ALARM_MASK         0x0020000

#define SDMA_DRD_BIT_TFD             27   /* mark last buffer of frame         */
#define SDMA_DRD_BIT_INT             26   /* interrupt after buffer processed  */
#define SDMA_DRD_BIT_INIT            21   /* lsb position of initiator         */
#define SDMA_DRD_MASK_FLAGS          0x0C000000  /* BD_FLAGS flag bits            */
#define SDMA_DRD_MASK_LENGTH         0x03FFFFFF  /* BD_FLAGS length mask          */
#define SDMA_BD_BIT_READY            30          /* Status BD ready bit           */
#ifdef SAS_COMPILE
 #define SDMA_BD_MASK_READY          constant(1<<SDMA_BD_BIT_READY)
#else
 #define SDMA_BD_MASK_READY          (1<<SDMA_BD_BIT_READY)
#endif
#define SDMA_BD_MASK_SIGN            0x7FFFFFFF  /* task code needs Status>0      */

#define SDMA_PRAGMA_BIT_RSV          7  /* reserved pragma bit                    */
#define SDMA_PRAGMA_BIT_PRECISE_INC  6  /* increment 0=when possible, 1=iter end  */
#define SDMA_PRAGMA_BIT_RST_ERROR_NO 5  /* don't reset errors on task enable      */
#define SDMA_PRAGMA_BIT_PACK         4  /* pack data enable                       */
#define SDMA_PRAGMA_BIT_INTEGER      3  /* data alignment 0=frac(msb), 1=int(lsb) */
#define SDMA_PRAGMA_BIT_SPECREAD     2  /* XLB speculative read enable            */
#define SDMA_PRAGMA_BIT_CW           1  /* write line buffer enable               */
#define SDMA_PRAGMA_BIT_RL           0  /* read line buffer enable                */

#define SDMA_TASK_ENTRY_BYTES       32  /* Bytes per task in entry table  */
#define SDMA_TASK_GROUP_NUM         16  /* Number of tasks per task group */
#define SDMA_TASK_GROUP_BYTES       (SDMA_TASK_ENTRY_BYTES*SDMA_TASK_GROUP_NUM)


/*******************************************************************************
 * Task group control macros, use when TaskNum > 15
 ******************************************************************************/
#define SDMA_TASKNUM_EXT(OldTaskNum) (OldTaskNum%16)

#define SDMA_TASKBAR_CHANGE(sdma, OldTaskNum) {	\
	sdma->taskBar += (((int)(OldTaskNum/SDMA_TASK_GROUP_NUM))*SDMA_TASK_GROUP_BYTES); \
}

#define SDMA_TASKBAR_RESTORE(sdma, OldTaskNum) {	\
	sdma->taskBar -= (((int)(OldTaskNum/SDMA_TASK_GROUP_NUM))*SDMA_TASK_GROUP_BYTES); \
}


/*******************************************************************************
 * Task control macros
 ******************************************************************************/
#define SDMA_TASK_CFG(RegAddr, TaskNum, AutoStart, AutoStartNum) { \
	*(((volatile uint16 *)RegAddr)+TaskNum) = (uint16)(0x0000     |  \
									 ((AutoStart!=0)<<7) |  \
									 (AutoStartNum&0xF)  ); \
}

#define SDMA_TASK_AUTO_START(RegAddr, TaskNum, AutoStart, AutoStartNum) { \
	*(((volatile uint16 *)RegAddr)+TaskNum) = (uint16)((*(((volatile uint16 *)RegAddr)+TaskNum) & \
									 (uint16) 0xff30) | ((uint16)(0x0000 |  \
									 ((AutoStart!=0)<<7) |  \
									 (AutoStartNum&0xF))  )); \
}

#define SDMA_TASK_ENABLE(RegAddr, TaskNum) {	\
	*(((volatile uint16 *)RegAddr)+TaskNum) |=  (uint16)0x8000; \
}

#define SDMA_TASK_DISABLE(RegAddr, TaskNum) {	\
	*(((volatile uint16 *)RegAddr)+TaskNum) &= ~(uint16)0x8000; \
}

#define SDMA_TASK_STATUS(RegAddr, TaskNum)	\
	*(((volatile uint16 *)RegAddr)+TaskNum)


/*******************************************************************************
 * Interrupt control macros
 ******************************************************************************/
#define SDMA_INT_ENABLE(RegAddr, Bit) \
  do { \
    rtems_interrupt_level level; \
    rtems_interrupt_disable(level); \
    *((volatile uint32 *) RegAddr) &= ~((uint32) (1 << Bit)); \
    rtems_interrupt_enable(level); \
  } while (0)

#define SDMA_INT_DISABLE(RegAddr, Bit) \
  do { \
    rtems_interrupt_level level; \
    rtems_interrupt_disable(level); \
    *((volatile uint32 *) (RegAddr)) |= ((uint32)(1 << Bit)); \
    rtems_interrupt_enable(level); \
  } while (0)

#define SDMA_INT_SOURCE(RegPend, RegMask)	\
	(*((volatile uint32 *)(RegPend)) & (~*((volatile uint32 *)(RegMask))) & (uint32)SDMA_INT_BIT_IMPL)

#define SDMA_INT_PENDING(RegPend, RegMask)	\
	(*((volatile uint32 *)(RegPend)) & (~*((volatile uint32 *)(RegMask))))

#define SDMA_INT_TEST(IntSource, Bit)	\
	(((uint32)IntSource) & ((uint32)(1<<Bit)))

/*
 * define SDMA_INT_FIND to get int bit rather than scan all bits use
 * cntlzw
 */

/* Clear the IntPend bit */
#define SDMA_CLEAR_IEVENT(RegAddr, Bit) {	\
	*((volatile uint32 *)RegAddr) = ((uint32)(1<<Bit)); \
}

#define SDMA_GET_PENDINGBIT(sdma, Bit)	\
	(sdma->IntPend & (uint32)(1<<Bit))

#define SDMA_GET_MASKBIT(sdma, Bit)	\
	(sdma->IntMask & (uint32)(1<<Bit))


/*******************************************************************************
 * SmartDMA FIFO control macros
 ******************************************************************************/

/*******************************************************************************
 * SmartDMA TEA detection control macros
 ******************************************************************************/
/* Enable SmartDMA TEA detection and TEA interrupt */
#define SDMA_TEA_ENABLE(sdma) {								\
	SDMA_INT_ENABLE(sdma, SDMA_INT_BIT_TEA);				\
	sdma->PtdCntrl &= ~((uint32)(1<<SDMA_PTDCTRL_BIT_TEA));	\
}

/* Disable SmartDMA TEA detection and TEA interrupt */
#define SDMA_TEA_DISABLE(sdma) {							\
	SDMA_INT_DISABLE(sdma, SDMA_INT_BIT_TEA);				\
	sdma->PtdCntrl |=  ((uint32)(1<<SDMA_PTDCTRL_BIT_TEA));	\
}

/* Clear the TEA interrupt */
#define SDMA_TEA_CLEAR(sdma) {									\
	sdma->IntPend =  ((uint32)(0x1F<<SDMA_INT_BIT_TEA_TASK));	\
}

/* Determine which task caused a TEA on the XLB */
#define SDMA_TEA_SOURCE(RegPend)	\
	(uint32)(((*(volatile uint32 *)RegPend)>>SDMA_INT_BIT_TEA_TASK) & 0xF)


/*******************************************************************************
 * SmartDMA debug control macros
 ******************************************************************************/
/* Enable the SmartDMA debug unit and DBG interrupt */
/* add sdma->dbg_regs setup? */
#define SDMA_DBG_ENABLE(sdma) {					\
	SDMA_INT_ENABLE(sdma, SDMA_INT_BIT_DBG);	\
}

#define SDMA_DBG_DISABLE(sdma) {				\
	SDMA_INT_DISABLE(sdma, SDMA_INT_BIT_DBG);	\
}

/* Clear the debug interrupt */
#define SDMA_DBG_CLEAR(sdma) {					\
	SDMA_CLEAR_IEVENT(sdma, SDMA_INT_BIT_DBG);	\
}

#define SDMA_DBG_MDE(dst, sdma, addr) {	\
	sdma->MDEDebug = addr;				\
	dst = sdma->MDEDebug;				\
}

#define SDMA_DBG_ADS(dst, sdma, addr) {	\
	sdma->ADSDebug = addr;				\
	dst = sdma->ADSDebug;				\
}

#define SDMA_DBG_PTD(dst, sdma, addr) {	\
	sdma->PTDDebug = addr;				\
	dst = sdma->PTDDebug;				\
}


/*******************************************************************************
 * Initiator control macros
 ******************************************************************************/

/* This macro may not work, getting compile errors */
/* Set the Transfer Size */
/* Note that masking the size w/ 0x3 gives the desired value for uint32 */
/*    (expressed as 4), namely 0. */
#define SDMA_SET_SIZE(RegAddr, TaskNum, SrcSize, DstSize)			\
	*(((volatile uint8 *)RegAddr)+((uint32)(TaskNum/2))) =					\
	(uint8)((*(((volatile uint8 *)RegAddr)+((uint32)(TaskNum/2))) &			\
			((TaskNum%2) ? 0xf0 : 0x0f)) |							\
	((uint8)(((SrcSize & 0x3)<<2) |									\
			( DstSize & 0x3 ) ) <<(4*((int)(1-(TaskNum%2))))));

/* This macro may not work */
/* Set the Initiator in TCR */
#define SDMA_SET_INIT(RegAddr, TaskNum, Initiator)					\
{																	\
	*(((volatile uint16 *)RegAddr)+TaskNum) &= (uint16)0xE0FF;				\
	*(((volatile uint16 *)RegAddr)+TaskNum) |= (((0x01F & Initiator)<<8) |	\
									  (0<<SDMA_TCR_BIT_HOLD));		\
}

/* Change DRD initiator number */
#define SDMA_INIT_CHANGE(task, oldInitiator, newInitiator) {		\
	int i;															\
	for (i=0; i<task->NumDRD; i++) {								\
		if (SDMA_INIT_READ(task->DRD[i]) == (uint32)oldInitiator) {	\
			SDMA_INIT_WRITE(task->DRD[i],newInitiator);				\
		}															\
	}																\
}

/* Set the Initiator Priority */
#define SDMA_SET_INITIATOR_PRIORITY(sdma, initiator, priority)	\
	*(((volatile uint8 *)&sdma->IPR0)+initiator) = priority;


/* Read DRD initiator number */
#define SDMA_INIT_READ(PtrDRD)	\
	(((*(volatile uint32 *)PtrDRD)>>SDMA_DRD_BIT_INIT) & (uint32)0x1F)

/* Write DRD initiator number */
#define SDMA_INIT_WRITE(PtrDRD, Initiator) {					\
	*(volatile uint32 *)PtrDRD = ((*(volatile uint32 *)PtrDRD) & 0xFC1FFFFF) |	\
						(Initiator<<SDMA_DRD_BIT_INIT);			\
}

/* Change DRD initiator number */
#define SDMA_INIT_CHANGE(task, oldInitiator, newInitiator) {		\
	int i;															\
	for (i=0; i<task->NumDRD; i++) {								\
		if (SDMA_INIT_READ(task->DRD[i]) == (uint32)oldInitiator) {	\
			SDMA_INIT_WRITE(task->DRD[i],newInitiator);				\
		}															\
	}																\
}

#endif	/* __TASK_API_BESTCOMM_CNTRL_H */
