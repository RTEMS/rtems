/*
 *  This file contains the NvRAM driver for the PPCn_60x
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */

#include <bsp.h>
#include "ds1385.h"
#include "mk48t18.h"
#include "stk11c68.h"

/*
 * Private types
 */
typedef
void
(*PNVRAMWRITE)
(
	unsigned32 ulOffset,
	unsigned8 ucByte
);

typedef
unsigned8
(*PNVRAMREAD)
(
	unsigned32 ulOffset
);

typedef
void
(*PNVRAMCOMMIT)
(
);

typedef struct _NVRAM_ENTRY_TABLE
{
	PNVRAMWRITE	nvramWrite;
	PNVRAMREAD	nvramRead;
	PNVRAMCOMMIT	nvramCommit;
	unsigned32	nvramSize;
} NVRAM_ENTRY_TABLE, *PNVRAM_ENTRY_TABLE;

/*
 * Private routines
 */

/*
 * This routine provides a stub for NvRAM devices which
 * do not require a commit operation
 */
static void nvramCommitStub();

/*
 * DS1385 specific routines
 */
static void nvramDsWrite(unsigned32 ulOffset, unsigned8 ucByte);
static unsigned8 nvramDsRead(unsigned32 ulOffset);

/*
 * MK48T18 specific routines
 */
static void nvramMkWrite(unsigned32 ulOffset, unsigned8 ucByte);
static unsigned8 nvramMkRead(unsigned32 ulOffset);

/*
 * STK11C68 specific routines
 */
static void nvramStk11C68Commit();
/*
 * STK11C88 specific routines
 */
static void nvramStk11C88Commit();

/*
 * NvRAM hook tables
 */
NVRAM_ENTRY_TABLE nvramDsTable =
{
	nvramDsWrite,
	nvramDsRead,
	nvramCommitStub,
	DS1385_NVSIZE
};

NVRAM_ENTRY_TABLE nvramMkTable =
{
	nvramMkWrite,
	nvramMkRead,
	nvramCommitStub,
	MK48T18_NVSIZE
};

/*
 * As the STK devicxe is at the same address as the MK device,
 * the MK read/write routines may be used
 */
NVRAM_ENTRY_TABLE nvramStkTable =
{
	nvramMkWrite,
	nvramMkRead,
	nvramStk11C68Commit,
	STK11C68_NVSIZE
};

NVRAM_ENTRY_TABLE nvramStk88Table =
{
	nvramMkWrite,
	nvramMkRead,
	nvramStk11C88Commit,
	STK11C88_NVSIZE
};

/*
 * Private variables
 */
static PNVRAM_ENTRY_TABLE pNvRAMFunc;
static boolean		bNvRAMChanged=FALSE;
static unsigned32	ulPRePOSAreaLength;
static unsigned32	ulPRePOSAreaOffset;

/*
 * Mutual-exclusion semaphore
 */
static rtems_id semNvRAM;

/*
 * These routines support the ds1385
 */
static unsigned8 nvramDsRead(unsigned32 ulOffset)
{
	unsigned8 ucTemp;

	ucTemp = ulOffset & 0xff;
	outport_byte(DS1385_PORT_BASE, ucTemp);

	ucTemp = (ulOffset >> 8) & 0xf;
	outport_byte((DS1385_PORT_BASE + 1) , ucTemp);

	inport_byte(DS1385_PORT_BASE+3, ucTemp);
	return(ucTemp);
}

static void nvramDsWrite(unsigned32 ulOffset, unsigned8 ucData)
{
	unsigned8 ucTemp;

	ucTemp = (unsigned8)(ulOffset & 0xff);
	outport_byte(DS1385_PORT_BASE, (unsigned8) ucTemp);

	ucTemp = (unsigned8)((ulOffset >> 8) & 0xf);
	outport_byte((DS1385_PORT_BASE + 1) , (unsigned8)ucTemp);

	outport_byte((DS1385_PORT_BASE+3), ucData);
}

/*
 * These routines support the MK48T18 and STK11C68
 */
static unsigned8 nvramMkRead(unsigned32 ulOffset)
{
	unsigned8 *pNvRAM = (unsigned8 *)MK48T18_BASE;

	return(pNvRAM[ulOffset]);
}

static void nvramMkWrite(unsigned32 ulOffset, unsigned8 ucData)
{
	unsigned8 *pNvRAM = (unsigned8 *)MK48T18_BASE;

	pNvRAM[ulOffset]=ucData;
}

/*
 * This routine provides a stub for NvRAM devices which
 * do not require a commit operation
 */
static void nvramCommitStub()
{
}

/*
 * This routine triggers a transfer from the NvRAM to the
 * EE array in the STK11C68 device
 */
static void nvramStk11C68Commit()
{
#if 0
	rtems_interval		ticks_per_second;
	rtems_status_code	status;
#endif

	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	/*
	 * Issue Store command
	 */
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x0000);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x1555);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x0aaa);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x1fff);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x10f0);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x0f0f);
	EIEIO;
	/*
	 * Delay for 10mS to allow store to
	 * complete
	 */
#if 0
	status = rtems_clock_get(
		RTEMS_CLOCK_GET_TICKS_PER_SECOND,
		&ticks_per_second
	);

	status = rtems_task_wake_after(ticks_per_second/100);
#endif
	bNvRAMChanged=FALSE;

	rtems_semaphore_release(semNvRAM);
}

/*
 * This routine triggers a transfer from the NvRAM to the
 * EE array in the STK11C88 device
 */
static void nvramStk11C88Commit()
{
#if 0
	rtems_interval		ticks_per_second;
	rtems_status_code	status;
#endif

	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	/*
	 * Issue Store command
	 */
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x0e38);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x31c7);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x03e0);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x3c1f);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x303f);
	EIEIO;
	(void)pNvRAMFunc->nvramRead(0x0fc0);
	EIEIO;
	/*
	 * Delay for 10mS to allow store to
	 * complete
	 */
#if 0
	status = rtems_clock_get(
		RTEMS_CLOCK_GET_TICKS_PER_SECOND,
		&ticks_per_second
	);

	status = rtems_task_wake_after(ticks_per_second/100);
#endif
	bNvRAMChanged=FALSE;

	rtems_semaphore_release(semNvRAM);
}

/*
 * These are the publically accessable routines
 */
/*
 * This routine returns the size of the NvRAM
 */
unsigned32 SizeNvRAM()
{
	return(ulPRePOSAreaLength);
}

/*
 * This routine commits changes to the NvRAM
 */
void CommitNvRAM()
{
	if(bNvRAMChanged)
	{
		(pNvRAMFunc->nvramCommit)();
	}
}

/*
 * This routine reads a byte from the NvRAM
 */
rtems_status_code ReadNvRAM8(unsigned32 ulOffset, unsigned8 *pucData)
{
	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	*pucData=pNvRAMFunc->nvramRead(ulPRePOSAreaOffset+ulOffset);
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

/*
 * This routine writes a byte to the NvRAM
 */
rtems_status_code WriteNvRAM8(unsigned32 ulOffset, unsigned8 ucValue)
{
	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	pNvRAMFunc->nvramWrite(ulPRePOSAreaOffset+ulOffset, ucValue);
	bNvRAMChanged=TRUE;
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

/*
 * This routine reads a block of bytes from the NvRAM
 */
rtems_status_code ReadNvRAMBlock(
  unsigned32 ulOffset, unsigned8 *pucData, unsigned32 length)
{
	unsigned32 i;

	if((ulOffset + length) > ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	for ( i=0 ; i<length ; i++ )
		pucData[i] =
			pNvRAMFunc->nvramRead(ulPRePOSAreaOffset+ulOffset+i);
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

/*
 * This routine writes a block of bytes to the NvRAM
 */
rtems_status_code WriteNvRAMBlock(
  unsigned32 ulOffset, unsigned8 *ucValue, unsigned32 length)
{
	unsigned32 i;

	if((ulOffset + length) > ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

	for ( i=0 ; i<length ; i++ )
		pNvRAMFunc->nvramWrite(
			ulPRePOSAreaOffset+ulOffset+i, ucValue[i]);
	bNvRAMChanged=TRUE;
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

/*
 * The NVRAM holds data in Big-Endian format
 */
rtems_status_code ReadNvRAM16 (unsigned32 ulOffset, unsigned16 *pusData)
{
	unsigned32 ulTrueOffset=ulPRePOSAreaOffset+ulOffset;

	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        *pusData=(pNvRAMFunc->nvramRead(ulTrueOffset) << 8) +
		 (pNvRAMFunc->nvramRead(ulTrueOffset + 1));
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

rtems_status_code WriteNvRAM16 (unsigned32 ulOffset, unsigned16 usValue)
{
	unsigned32 ulTrueOffset=ulPRePOSAreaOffset+ulOffset;

	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        pNvRAMFunc->nvramWrite(ulTrueOffset, (unsigned8) (usValue >> 8));
        pNvRAMFunc->nvramWrite(ulTrueOffset + 1, (unsigned8) usValue);
	bNvRAMChanged=TRUE;
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

rtems_status_code ReadNvRAM32 (unsigned32 ulOffset, unsigned32 *pulData)
{
	unsigned32 ulTrueOffset=ulPRePOSAreaOffset+ulOffset;

	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        *pulData=(pNvRAMFunc->nvramRead(ulTrueOffset) << 24) +
		 (pNvRAMFunc->nvramRead(ulTrueOffset + 1) << 16) +
		 (pNvRAMFunc->nvramRead(ulTrueOffset + 2) << 8) +
		 (pNvRAMFunc->nvramRead(ulTrueOffset + 3));
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

rtems_status_code WriteNvRAM32 (unsigned32 ulOffset, unsigned32 ulValue)
{
	unsigned32 ulTrueOffset=ulPRePOSAreaOffset+ulOffset;

	if(ulOffset>ulPRePOSAreaLength)
	{
		return RTEMS_INVALID_ADDRESS;
	}
	rtems_semaphore_obtain(semNvRAM, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
        pNvRAMFunc->nvramWrite(ulTrueOffset, (unsigned8) (ulValue >> 24));
        pNvRAMFunc->nvramWrite(ulTrueOffset + 1, (unsigned8) (ulValue >> 16));
        pNvRAMFunc->nvramWrite(ulTrueOffset + 2, (unsigned8) (ulValue >> 8));
        pNvRAMFunc->nvramWrite(ulTrueOffset + 3, (unsigned8) ulValue);
	bNvRAMChanged=TRUE;
	rtems_semaphore_release(semNvRAM);
	return(RTEMS_SUCCESSFUL);
}

void
InitializeNvRAM(void)
{
	PHEADER pNvHeader = (PHEADER)0;
	rtems_status_code sc;
	unsigned32 ulLength, ulOffset;

	if(ucSystemType==SYS_TYPE_PPC1)
	{
		if(ucBoardRevMaj<5)
		{
			pNvRAMFunc=&nvramDsTable;
		}
		else
		{
			pNvRAMFunc=&nvramMkTable;
		}
	}
	else if(ucSystemType==SYS_TYPE_PPC1a)
	{
		pNvRAMFunc=&nvramMkTable;
	}
	else if(ucSystemType==SYS_TYPE_PPC4)
	{
		pNvRAMFunc=&nvramStk88Table;
	}
	else
	{
		pNvRAMFunc=&nvramStkTable;
	}

        /*
         * Set up mutex semaphore
         */
        sc = rtems_semaphore_create (
                rtems_build_name ('N', 'V', 'R', 's'),
                1,
                RTEMS_BINARY_SEMAPHORE |
                RTEMS_INHERIT_PRIORITY |
                RTEMS_PRIORITY,
                RTEMS_NO_PRIORITY,
                &semNvRAM);
        if (sc != RTEMS_SUCCESSFUL)
        {
                rtems_fatal_error_occurred (sc);
        }

	/*
	 * Initially access the whole of NvRAM until we determine where the
	 * OS Area is located.
	 */
	ulPRePOSAreaLength=0xffffffff;
	ulPRePOSAreaOffset=0;

	/*
	 * Access the header at the start of NvRAM
	 */
	ReadNvRAM32((unsigned32)(&pNvHeader->OSAreaLength), &ulLength);
	ReadNvRAM32((unsigned32)(&pNvHeader->OSAreaAddress), &ulOffset);

	/*
	 * Now set limits for future accesses
	 */
	ulPRePOSAreaLength=ulLength;
	ulPRePOSAreaOffset=ulOffset;
}


